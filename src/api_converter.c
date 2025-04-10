#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project.h"

#define BUFFER_SIZE 4096

// Fonction pour convertir un véhicule en JSON
void vehicleToJSON(Vehicle* vehicle, char* buffer) {
    const char* status_str;
    switch (vehicle->status) {
        case VEHICLE_IN_ROUTE: status_str = "in_route"; break;
        case VEHICLE_LOADING: status_str = "loading"; break;
        case VEHICLE_ON_BREAK: status_str = "on_break"; break;
        case VEHICLE_OUT_OF_SERVICE: status_str = "out_of_service"; break;
        default: status_str = "unknown";
    }

    snprintf(buffer, BUFFER_SIZE,
        "{\"id\":%d,\"type\":\"%s\",\"status\":\"%s\",\"current_location\":\"%s\",\"current_battery\":%.1f}",
        vehicle->id,
        vehicle->type,
        status_str,
        vehicle->current_location,
        vehicle->current_battery
    );
}

// Fonction pour convertir un colis en JSON
void packageToJSON(Package* pkg, char* buffer) {
    const char* status_str;
    switch (pkg->status) {
        case PACKAGE_PENDING: status_str = "pending"; break;
        case PACKAGE_IN_TRANSIT: status_str = "in_transit"; break;
        case PACKAGE_DELIVERED: status_str = "delivered"; break;
        case PACKAGE_DELAYED: status_str = "delayed"; break;
        default: status_str = "unknown";
    }

    snprintf(buffer, BUFFER_SIZE,
        "{\"id\":%d,\"status\":\"%s\",\"priority\":%d,\"source\":\"%s\",\"destination\":\"%s\",\"weight\":%.1f}",
        pkg->id,
        status_str,
        pkg->priority,
        pkg->source,
        pkg->destination,
        pkg->weight
    );
}

// Fonction pour convertir un nœud du réseau en JSON
void nodeToJSON(Node* node, char* buffer) {
    const char* type_str = node->type == NODE_WAREHOUSE ? "warehouse" : "delivery";
    
    snprintf(buffer, BUFFER_SIZE,
        "{\"id\":%d,\"name\":\"%s\",\"type\":\"%s\"}",
        node->id,
        node->name,
        type_str
    );
}

// Fonction pour convertir une arête du réseau en JSON
void edgeToJSON(Edge* edge, char* buffer) {
    snprintf(buffer, BUFFER_SIZE,
        "{\"source\":%d,\"target\":%d}",
        edge->source,
        edge->target
    );
}

// Fonction pour générer le JSON complet des véhicules
char* vehiclesToJSON(Vehicle* vehicles, int num_vehicles) {
    char* json = (char*)malloc(BUFFER_SIZE);
    char vehicle_json[BUFFER_SIZE];
    
    strcpy(json, "[");
    for (int i = 0; i < num_vehicles; i++) {
        vehicleToJSON(&vehicles[i], vehicle_json);
        strcat(json, vehicle_json);
        if (i < num_vehicles - 1) strcat(json, ",");
    }
    strcat(json, "]");
    
    return json;
}

// Fonction pour générer le JSON complet des colis
char* packagesToJSON(Package* packages, int num_packages) {
    char* json = (char*)malloc(BUFFER_SIZE);
    char package_json[BUFFER_SIZE];
    
    strcpy(json, "[");
    for (int i = 0; i < num_packages; i++) {
        packageToJSON(&packages[i], package_json);
        strcat(json, package_json);
        if (i < num_packages - 1) strcat(json, ",");
    }
    strcat(json, "]");
    
    return json;
}

// Fonction pour générer le JSON complet du réseau
char* networkToJSON(Graph* graph) {
    char* json = (char*)malloc(BUFFER_SIZE);
    char node_json[BUFFER_SIZE];
    char edge_json[BUFFER_SIZE];
    
    strcpy(json, "{\"nodes\":[");
    
    // Ajouter les nœuds
    for (int i = 0; i < graph->V; i++) {
        nodeToJSON(&graph->array[i].node_attr, node_json);
        strcat(json, node_json);
        if (i < graph->V - 1) strcat(json, ",");
    }
    
    strcat(json, "],\"edges\":[");
    
    // Ajouter les arêtes
    int edge_count = 0;
    for (int i = 0; i < graph->V; i++) {
        AdjListNode* current = graph->array[i].head;
        while (current) {
            edgeToJSON(&current->attr, edge_json);
            strcat(json, edge_json);
            if (edge_count < graph->E - 1) strcat(json, ",");
            edge_count++;
            current = current->next;
        }
    }
    
    strcat(json, "]}");
    
    return json;
} 