#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "project.h"

#define PORT 8080
#define BUFFER_SIZE 1024

// Structure pour stocker les données du réseau
typedef struct {
    Graph* graph;
    Vehicle* vehicles;
    int num_vehicles;
    Package* packages;
    int num_packages;
} NetworkData;

// Fonction pour formater les données des véhicules en JSON
char* formatVehiclesJSON(Vehicle* vehicles, int num_vehicles) {
    char* json = (char*)malloc(BUFFER_SIZE);
    strcpy(json, "{\"vehicles\":[");
    
    for (int i = 0; i < num_vehicles; i++) {
        char vehicle[256];
        snprintf(vehicle, sizeof(vehicle),
            "{\"id\":%d,\"type\":\"%s\",\"status\":\"%s\",\"current_location\":%d,\"current_battery\":%.1f}",
            vehicles[i].id,
            vehicles[i].type == VEHICLE_SMALL ? "Petit" : 
            vehicles[i].type == VEHICLE_MEDIUM ? "Moyen" : "Grand",
            vehicles[i].current_location == -1 ? "inactive" : "active",
            vehicles[i].current_location,
            vehicles[i].current_battery);
        
        strcat(json, vehicle);
        if (i < num_vehicles - 1) strcat(json, ",");
    }
    
    strcat(json, "]}");
    return json;
}

// Fonction pour formater les données des colis en JSON
char* formatPackagesJSON(Package* packages, int num_packages) {
    char* json = (char*)malloc(BUFFER_SIZE);
    strcpy(json, "{\"packages\":[");
    
    for (int i = 0; i < num_packages; i++) {
        char package[256];
        snprintf(package, sizeof(package),
            "{\"id\":%d,\"source\":%d,\"destination\":%d,\"weight\":%.1f,\"priority\":%.1f,\"status\":\"%s\"}",
            packages[i].id,
            packages[i].source,
            packages[i].destination,
            packages[i].weight,
            packages[i].priority,
            packages[i].is_delivered ? "delivered" : "in_progress");
        
        strcat(json, package);
        if (i < num_packages - 1) strcat(json, ",");
    }
    
    strcat(json, "]}");
    return json;
}

// Fonction pour formater les statistiques en JSON
char* formatStatisticsJSON(NetworkData* data) {
    int delivered_packages = 0;
    int active_vehicles = 0;
    float total_delivery_time = 0;
    int total_deliveries = 0;
    
    for (int i = 0; i < data->num_packages; i++) {
        if (data->packages[i].is_delivered) {
            delivered_packages++;
            total_delivery_time += data->packages[i].delivery_time;
            total_deliveries++;
        }
    }
    
    for (int i = 0; i < data->num_vehicles; i++) {
        if (data->vehicles[i].current_location != -1) {
            active_vehicles++;
        }
    }
    
    float avg_delivery_time = total_deliveries > 0 ? total_delivery_time / total_deliveries : 0;
    float network_efficiency = data->num_packages > 0 ? 
        (float)delivered_packages / data->num_packages * 100 : 0;
    
    char* json = (char*)malloc(BUFFER_SIZE);
    snprintf(json, BUFFER_SIZE,
        "{\"delivered_packages\":%d,\"active_vehicles\":%d,\"avg_delivery_time\":%.1f,\"network_efficiency\":%.1f}",
        delivered_packages,
        active_vehicles,
        avg_delivery_time,
        network_efficiency);
    
    return json;
}

// Fonction pour gérer une requête HTTP
void* handleRequest(void* arg) {
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    // Lire la requête
    read(client_socket, buffer, BUFFER_SIZE);
    
    // Analyser la requête
    if (strstr(buffer, "GET /api/vehicles")) {
        NetworkData* data = (NetworkData*)arg;
        char* json = formatVehiclesJSON(data->vehicles, data->num_vehicles);
        snprintf(response, BUFFER_SIZE,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "\r\n"
            "%s", json);
        free(json);
    }
    else if (strstr(buffer, "GET /api/packages")) {
        NetworkData* data = (NetworkData*)arg;
        char* json = formatPackagesJSON(data->packages, data->num_packages);
        snprintf(response, BUFFER_SIZE,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "\r\n"
            "%s", json);
        free(json);
    }
    else if (strstr(buffer, "GET /api/statistics")) {
        NetworkData* data = (NetworkData*)arg;
        char* json = formatStatisticsJSON(data);
        snprintf(response, BUFFER_SIZE,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "\r\n"
            "%s", json);
        free(json);
    }
    else {
        snprintf(response, BUFFER_SIZE,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "404 Not Found");
    }
    
    // Envoyer la réponse
    write(client_socket, response, strlen(response));
    close(client_socket);
    free(arg);
    return NULL;
}

// Fonction principale du serveur API
void startAPIServer(Graph* graph, Vehicle* vehicles, int num_vehicles, 
                   Package* packages, int num_packages) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Erreur lors de la création du socket");
        return;
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors du bind");
        return;
    }
    
    listen(server_socket, 5);
    printf("Serveur API démarré sur le port %d\n", PORT);
    
    NetworkData data = {
        .graph = graph,
        .vehicles = vehicles,
        .num_vehicles = num_vehicles,
        .packages = packages,
        .num_packages = num_packages
    };
    
    while (1) {
        int* client_socket = malloc(sizeof(int));
        *client_socket = accept(server_socket, NULL, NULL);
        
        pthread_t thread;
        pthread_create(&thread, NULL, handleRequest, client_socket);
        pthread_detach(thread);
    }
} 