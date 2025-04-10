#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 6060
#define BUFFER_SIZE 4096
#define JSON_PATH "../data/network_state.json"

void send_response(SOCKET sock, const char* content, const char* content_type) {
    char headers[1024];
    int headers_len = snprintf(headers, sizeof(headers),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: %zu\r\n\r\n",
        content_type, strlen(content));
    
    send(sock, headers, headers_len, 0);
    send(sock, content, strlen(content), 0);
}

void handle_api_request(SOCKET sock) {
    FILE* file = fopen(JSON_PATH, "rb");
    if (!file) {
        const char* error = "{\"error\":\"File not found\"}";
        send_response(sock, error, "application/json");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* json = malloc(size + 1);
    fread(json, 1, size, file);
    fclose(file);
    json[size] = '\0';

    send_response(sock, json, "application/json");
    free(json);
}

void serve_file(SOCKET sock, const char* path) {
    char fullpath[BUFFER_SIZE];
    snprintf(fullpath, sizeof(fullpath), "../web%s", path);

    FILE* file = fopen(fullpath, "rb");
    if (!file) {
        const char* error = "File not found";
        send_response(sock, error, "text/plain");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(size + 1);
    fread(content, 1, size, file);
    fclose(file);
    content[size] = '\0';

    const char* content_type = "text/plain";
    if (strstr(path, ".html")) content_type = "text/html";
    else if (strstr(path, ".js")) content_type = "application/javascript";
    else if (strstr(path, ".css")) content_type = "text/css";

    send_response(sock, content, content_type);
    free(content);
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 5);

    printf("Serveur en ecoute sur le port %d...\n", PORT);

    while(1) {
        SOCKET client = accept(server, NULL, NULL);
        char buffer[BUFFER_SIZE];
        int bytes = recv(client, buffer, BUFFER_SIZE, 0);
        
        if(bytes > 0) {
            buffer[bytes] = '\0';
            
            if(strstr(buffer, "GET /data ")) {
                handle_api_request(client);
            }
            else if(strstr(buffer, "GET ")) {
                char path[256] = {0};
                sscanf(buffer, "GET %255s", path);
                serve_file(client, path);
            }
        }
        closesocket(client);
    }

    closesocket(server);
    WSACleanup();
    return 0;
}