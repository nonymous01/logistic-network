#ifndef PROJECT_H
#define PROJECT_H

#include "types.h"
#include <winsock2.h>

// Constantes
#define PORT 6060
#define BUFFER_SIZE 4096
#define WEB_ROOT "../web"
#define JSON_PATH "../data/network_state.json"

// Fonctions du serveur web
void setup_console();
void handle_request(SOCKET sock);
void send_response(SOCKET sock, int status_code, const char* content_type, const char* content, size_t content_length);
void serve_file(SOCKET sock, const char* path);
void handle_api_request(SOCKET sock, const char* path);

// Fonctions de gestion des données
NetworkState* read_network_state(const char* filename);
void free_network_state(NetworkState* state);
char* create_api_response(const NetworkState* state);

// Fonctions utilitaires
const char* get_status_message(int status_code);

#endif // PROJECT_H 