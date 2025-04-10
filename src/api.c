#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "mongoose.h"

#define BUFFER_SIZE 4096

// Structure pour stocker les données du réseau
typedef struct {
    Vehicle* vehicles;
    int num_vehicles;
    Package* packages;
    int num_packages;
    Graph* network;
} NetworkData;

// Fonction pour gérer la requête des véhicules
void handleVehiclesRequest(struct mg_connection* c, NetworkData* data) {
    printf("Handling vehicles request...\n");
    char* json = vehiclesToJSON(data->vehicles, data->num_vehicles);
    printf("Vehicles JSON: %s\n", json);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
    free(json);
}

// Fonction pour gérer la requête des colis
void handlePackagesRequest(struct mg_connection* c, NetworkData* data) {
    printf("Handling packages request...\n");
    char* json = packagesToJSON(data->packages, data->num_packages);
    printf("Packages JSON: %s\n", json);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
    free(json);
}

// Fonction pour gérer la requête du réseau
void handleNetworkRequest(struct mg_connection* c, NetworkData* data) {
    printf("Handling network request...\n");
    char* json = networkToJSON(data->network);
    printf("Network JSON: %s\n", json);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
    free(json);
}

// Fonction pour gérer les requêtes HTTP
void handle_http_request(struct mg_connection* c, int ev, void* ev_data, void* fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* hm = (struct mg_http_message*)ev_data;
        NetworkData* data = (NetworkData*)fn_data;

        if (mg_http_match_uri(hm, "/api/vehicles")) {
            handleVehiclesRequest(c, data);
        } else if (mg_http_match_uri(hm, "/api/packages")) {
            handlePackagesRequest(c, data);
        } else if (mg_http_match_uri(hm, "/api/network")) {
            handleNetworkRequest(c, data);
        } else if (mg_http_match_uri(hm, "/api/statistics")) {
            char* json = formatStatisticsJSON(data);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
            free(json);
        } else {
            // Servir les fichiers statiques pour le frontend
            struct mg_http_serve_opts opts = {.root_dir = "../web"};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

// Fonction principale de l'API
void startAPI(NetworkData* data) {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, "http://localhost:6060", handle_http_request, data);

    printf("API server started on http://localhost:6060\n");
    
    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    mg_mgr_free(&mgr);
} 