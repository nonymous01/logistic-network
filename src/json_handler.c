#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "project.h"
#include "cJSON.h"

#define JSON_BUFFER_SIZE 4096

// Fonction pour lire un fichier JSON
NetworkState* readNetworkState(const char* filename) {
    printf("Ouverture du fichier: %s\n", filename);
    FILE* file = fopen(filename, "rb"); // Mode binaire
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier %s (errno: %d)\n", filename, errno);
        return NULL;
    }

    printf("Fichier ouvert avec succès\n");
    // Lire le contenu du fichier
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    printf("Taille du fichier: %ld octets\n", file_size);
    fseek(file, 0, SEEK_SET);
    
    char* json_string = (char*)malloc(file_size + 1);
    if (!json_string) {
        printf("Erreur: Impossible d'allouer la mémoire pour le contenu JSON\n");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(json_string, 1, file_size, file);
    printf("Octets lus: %lu\n", (unsigned long)bytes_read);
    if (bytes_read != file_size) {
        printf("Erreur: Nombre d'octets lus (%lu) différent de la taille du fichier (%ld)\n", 
               (unsigned long)bytes_read, file_size);
        free(json_string);
        fclose(file);
        return NULL;
    }
    json_string[file_size] = '\0';
    fclose(file);

    // Parser le JSON
    printf("Parsing du JSON...\n");
    cJSON* root = cJSON_Parse(json_string);
    if (!root) {
        printf("Erreur: JSON invalide - %s\n", cJSON_GetErrorPtr());
        free(json_string);
        return NULL;
    }
    printf("JSON parsé avec succès\n");
    free(json_string);

    NetworkState* state = (NetworkState*)malloc(sizeof(NetworkState));
    if (!state) {
        printf("Erreur: Impossible d'allouer la mémoire pour NetworkState\n");
        cJSON_Delete(root);
        return NULL;
    }
    memset(state, 0, sizeof(NetworkState));

    // Parser les véhicules
    printf("Parsing des véhicules...\n");
    cJSON* vehicles = cJSON_GetObjectItem(root, "vehicles");
    if (vehicles && cJSON_IsArray(vehicles)) {
        state->vehicle_count = cJSON_GetArraySize(vehicles);
        printf("Nombre de véhicules trouvés: %d\n", state->vehicle_count);
        state->vehicles = (Vehicle*)malloc(state->vehicle_count * sizeof(Vehicle));
        if (!state->vehicles) {
            printf("Erreur: Impossible d'allouer la mémoire pour les véhicules\n");
            free(state);
            cJSON_Delete(root);
            return NULL;
        }
        
        for (int i = 0; i < state->vehicle_count; i++) {
            printf("Traitement du véhicule %d...\n", i + 1);
            cJSON* vehicle = cJSON_GetArrayItem(vehicles, i);
            state->vehicles[i].id = cJSON_GetObjectItem(vehicle, "id")->valueint;
            strcpy(state->vehicles[i].type, cJSON_GetObjectItem(vehicle, "type")->valuestring);
            strcpy(state->vehicles[i].status, cJSON_GetObjectItem(vehicle, "status")->valuestring);
            strcpy(state->vehicles[i].current_location, cJSON_GetObjectItem(vehicle, "current_location")->valuestring);
            state->vehicles[i].current_battery = cJSON_GetObjectItem(vehicle, "current_battery")->valuedouble;
            state->vehicles[i].current_node = cJSON_GetObjectItem(vehicle, "current_node")->valueint;
            
            // Parser la route
            cJSON* route = cJSON_GetObjectItem(vehicle, "route");
            if (route && cJSON_IsArray(route)) {
                state->vehicles[i].route_length = cJSON_GetArraySize(route);
                state->vehicles[i].route = (int*)malloc(state->vehicles[i].route_length * sizeof(int));
                if (!state->vehicles[i].route) {
                    printf("Erreur: Impossible d'allouer la mémoire pour la route du véhicule %d\n", i + 1);
                    // TODO: Libérer la mémoire précédemment allouée
                    free(state->vehicles);
                    free(state);
                    cJSON_Delete(root);
                    return NULL;
                }
                for (int j = 0; j < state->vehicles[i].route_length; j++) {
                    state->vehicles[i].route[j] = cJSON_GetArrayItem(route, j)->valueint;
                }
            }
            
            // Parser les colis
            cJSON* packages = cJSON_GetObjectItem(vehicle, "packages");
            if (packages && cJSON_IsArray(packages)) {
                state->vehicles[i].package_count = cJSON_GetArraySize(packages);
                state->vehicles[i].packages = (int*)malloc(state->vehicles[i].package_count * sizeof(int));
                if (!state->vehicles[i].packages) {
                    printf("Erreur: Impossible d'allouer la mémoire pour les colis du véhicule %d\n", i + 1);
                    // TODO: Libérer la mémoire précédemment allouée
                    free(state->vehicles[i].route);
                    free(state->vehicles);
                    free(state);
                    cJSON_Delete(root);
                    return NULL;
                }
                for (int j = 0; j < state->vehicles[i].package_count; j++) {
                    state->vehicles[i].packages[j] = cJSON_GetArrayItem(packages, j)->valueint;
                }
            }
            printf("Véhicule %d traité avec succès\n", i + 1);
        }
    }

    // Parser les colis
    printf("Parsing des colis...\n");
    cJSON* packages = cJSON_GetObjectItem(root, "packages");
    if (packages && cJSON_IsArray(packages)) {
        state->package_count = cJSON_GetArraySize(packages);
        printf("Nombre de colis trouvés: %d\n", state->package_count);
        state->packages = (Package*)malloc(state->package_count * sizeof(Package));
        if (!state->packages) {
            printf("Erreur: Impossible d'allouer la mémoire pour les colis\n");
            // TODO: Libérer la mémoire précédemment allouée
            for (int i = 0; i < state->vehicle_count; i++) {
                free(state->vehicles[i].route);
                free(state->vehicles[i].packages);
            }
            free(state->vehicles);
            free(state);
            cJSON_Delete(root);
            return NULL;
        }
        
        for (int i = 0; i < state->package_count; i++) {
            printf("Traitement du colis %d...\n", i + 1);
            cJSON* package = cJSON_GetArrayItem(packages, i);
            state->packages[i].id = cJSON_GetObjectItem(package, "id")->valueint;
            strcpy(state->packages[i].status, cJSON_GetObjectItem(package, "status")->valuestring);
            state->packages[i].priority = cJSON_GetObjectItem(package, "priority")->valueint;
            strcpy(state->packages[i].source, cJSON_GetObjectItem(package, "source")->valuestring);
            strcpy(state->packages[i].destination, cJSON_GetObjectItem(package, "destination")->valuestring);
            state->packages[i].weight = cJSON_GetObjectItem(package, "weight")->valuedouble;
            state->packages[i].source_node = cJSON_GetObjectItem(package, "source_node")->valueint;
            state->packages[i].destination_node = cJSON_GetObjectItem(package, "destination_node")->valueint;
            state->packages[i].assigned_vehicle = cJSON_GetObjectItem(package, "assigned_vehicle")->valueint;
            printf("Colis %d traité avec succès\n", i + 1);
        }
    }

    // Parser le réseau
    printf("Parsing du réseau...\n");
    cJSON* network = cJSON_GetObjectItem(root, "network");
    if (network) {
        // Parser les nœuds
        printf("Parsing des nœuds...\n");
        cJSON* nodes = cJSON_GetObjectItem(network, "nodes");
        if (nodes && cJSON_IsArray(nodes)) {
            state->node_count = cJSON_GetArraySize(nodes);
            printf("Nombre de nœuds trouvés: %d\n", state->node_count);
            state->nodes = (NetworkNode*)malloc(state->node_count * sizeof(NetworkNode));
            if (!state->nodes) {
                printf("Erreur: Impossible d'allouer la mémoire pour les nœuds\n");
                // TODO: Libérer la mémoire précédemment allouée
                for (int i = 0; i < state->vehicle_count; i++) {
                    free(state->vehicles[i].route);
                    free(state->vehicles[i].packages);
                }
                free(state->vehicles);
                free(state->packages);
                free(state);
                cJSON_Delete(root);
                return NULL;
            }
            
            for (int i = 0; i < state->node_count; i++) {
                printf("Traitement du nœud %d...\n", i + 1);
                cJSON* node = cJSON_GetArrayItem(nodes, i);
                state->nodes[i].id = cJSON_GetObjectItem(node, "id")->valueint;
                strcpy(state->nodes[i].name, cJSON_GetObjectItem(node, "name")->valuestring);
                strcpy(state->nodes[i].type, cJSON_GetObjectItem(node, "type")->valuestring);
                
                if (strcmp(state->nodes[i].type, "warehouse") == 0) {
                    state->nodes[i].capacity = cJSON_GetObjectItem(node, "capacity")->valuedouble;
                    state->nodes[i].current_load = cJSON_GetObjectItem(node, "current_load")->valuedouble;
                } else {
                    state->nodes[i].demand = cJSON_GetObjectItem(node, "demand")->valuedouble;
                    state->nodes[i].current_demand = cJSON_GetObjectItem(node, "current_demand")->valuedouble;
                }
                printf("Nœud %d traité avec succès\n", i + 1);
            }
        }

        // Parser les arêtes
        printf("Parsing des arêtes...\n");
        cJSON* edges = cJSON_GetObjectItem(network, "edges");
        if (edges && cJSON_IsArray(edges)) {
            state->edge_count = cJSON_GetArraySize(edges);
            printf("Nombre d'arêtes trouvées: %d\n", state->edge_count);
            state->edges = (NetworkEdge*)malloc(state->edge_count * sizeof(NetworkEdge));
            if (!state->edges) {
                printf("Erreur: Impossible d'allouer la mémoire pour les arêtes\n");
                // TODO: Libérer la mémoire précédemment allouée
                for (int i = 0; i < state->vehicle_count; i++) {
                    free(state->vehicles[i].route);
                    free(state->vehicles[i].packages);
                }
                free(state->vehicles);
                free(state->packages);
                free(state->nodes);
                free(state);
                cJSON_Delete(root);
                return NULL;
            }
            
            for (int i = 0; i < state->edge_count; i++) {
                printf("Traitement de l'arête %d...\n", i + 1);
                cJSON* edge = cJSON_GetArrayItem(edges, i);
                state->edges[i].source = cJSON_GetObjectItem(edge, "source")->valueint;
                state->edges[i].target = cJSON_GetObjectItem(edge, "target")->valueint;
                state->edges[i].distance = cJSON_GetObjectItem(edge, "distance")->valuedouble;
                state->edges[i].time = cJSON_GetObjectItem(edge, "time")->valuedouble;
                state->edges[i].cost = cJSON_GetObjectItem(edge, "cost")->valuedouble;
                printf("Arête %d traitée avec succès\n", i + 1);
            }
        }
    }

    // Parser les statistiques
    printf("Parsing des statistiques...\n");
    cJSON* stats = cJSON_GetObjectItem(root, "statistics");
    if (stats) {
        state->stats.delivered_packages = cJSON_GetObjectItem(stats, "delivered_packages")->valueint;
        state->stats.active_vehicles = cJSON_GetObjectItem(stats, "active_vehicles")->valueint;
        state->stats.average_delivery_time = cJSON_GetObjectItem(stats, "avg_delivery_time")->valuedouble;
        state->stats.network_efficiency = cJSON_GetObjectItem(stats, "network_efficiency")->valuedouble;
        printf("Statistiques traitées avec succès\n");
    }

    printf("Nettoyage de la mémoire JSON...\n");
    cJSON_Delete(root);
    printf("Lecture du fichier JSON terminée avec succès\n");
    return state;
}

// Fonction pour écrire l'état du réseau dans un fichier JSON
void writeNetworkState(const char* filename, const NetworkState* state) {
    cJSON* root = cJSON_CreateObject();
    
    // Créer le tableau des véhicules
    cJSON* vehicles = cJSON_CreateArray();
    for (int i = 0; i < state->vehicle_count; i++) {
        cJSON* vehicle = cJSON_CreateObject();
        cJSON_AddNumberToObject(vehicle, "id", state->vehicles[i].id);
        cJSON_AddStringToObject(vehicle, "type", state->vehicles[i].type);
        cJSON_AddStringToObject(vehicle, "status", state->vehicles[i].status);
        cJSON_AddStringToObject(vehicle, "current_location", state->vehicles[i].current_location);
        cJSON_AddNumberToObject(vehicle, "current_battery", state->vehicles[i].current_battery);
        cJSON_AddNumberToObject(vehicle, "current_node", state->vehicles[i].current_node);
        
        // Ajouter la route
        cJSON* route = cJSON_CreateArray();
        for (int j = 0; j < state->vehicles[i].route_length; j++) {
            cJSON_AddItemToArray(route, cJSON_CreateNumber(state->vehicles[i].route[j]));
        }
        cJSON_AddItemToObject(vehicle, "route", route);
        
        // Ajouter les colis
        cJSON* packages = cJSON_CreateArray();
        for (int j = 0; j < state->vehicles[i].package_count; j++) {
            cJSON_AddItemToArray(packages, cJSON_CreateNumber(state->vehicles[i].packages[j]));
        }
        cJSON_AddItemToObject(vehicle, "packages", packages);
        
        cJSON_AddItemToArray(vehicles, vehicle);
    }
    cJSON_AddItemToObject(root, "vehicles", vehicles);
    
    // Créer le tableau des colis
    cJSON* packages = cJSON_CreateArray();
    for (int i = 0; i < state->package_count; i++) {
        cJSON* package = cJSON_CreateObject();
        cJSON_AddNumberToObject(package, "id", state->packages[i].id);
        cJSON_AddStringToObject(package, "status", state->packages[i].status);
        cJSON_AddNumberToObject(package, "priority", state->packages[i].priority);
        cJSON_AddStringToObject(package, "source", state->packages[i].source);
        cJSON_AddStringToObject(package, "destination", state->packages[i].destination);
        cJSON_AddNumberToObject(package, "weight", state->packages[i].weight);
        cJSON_AddNumberToObject(package, "source_node", state->packages[i].source_node);
        cJSON_AddNumberToObject(package, "destination_node", state->packages[i].destination_node);
        cJSON_AddNumberToObject(package, "assigned_vehicle", state->packages[i].assigned_vehicle);
        cJSON_AddItemToArray(packages, package);
    }
    cJSON_AddItemToObject(root, "packages", packages);
    
    // Créer l'objet réseau
    cJSON* network = cJSON_CreateObject();
    
    // Créer le tableau des nœuds
    cJSON* nodes = cJSON_CreateArray();
    for (int i = 0; i < state->node_count; i++) {
        cJSON* node = cJSON_CreateObject();
        cJSON_AddNumberToObject(node, "id", state->nodes[i].id);
        cJSON_AddStringToObject(node, "name", state->nodes[i].name);
        cJSON_AddStringToObject(node, "type", state->nodes[i].type);
        
        if (strcmp(state->nodes[i].type, "warehouse") == 0) {
            cJSON_AddNumberToObject(node, "capacity", state->nodes[i].capacity);
            cJSON_AddNumberToObject(node, "current_load", state->nodes[i].current_load);
        } else {
            cJSON_AddNumberToObject(node, "demand", state->nodes[i].demand);
            cJSON_AddNumberToObject(node, "current_demand", state->nodes[i].current_demand);
        }
        
        cJSON_AddItemToArray(nodes, node);
    }
    cJSON_AddItemToObject(network, "nodes", nodes);
    
    // Créer le tableau des arêtes
    cJSON* edges = cJSON_CreateArray();
    for (int i = 0; i < state->edge_count; i++) {
        cJSON* edge = cJSON_CreateObject();
        cJSON_AddNumberToObject(edge, "source", state->edges[i].source);
        cJSON_AddNumberToObject(edge, "target", state->edges[i].target);
        cJSON_AddNumberToObject(edge, "distance", state->edges[i].distance);
        cJSON_AddNumberToObject(edge, "time", state->edges[i].time);
        cJSON_AddNumberToObject(edge, "cost", state->edges[i].cost);
        cJSON_AddItemToArray(edges, edge);
    }
    cJSON_AddItemToObject(network, "edges", edges);
    
    cJSON_AddItemToObject(root, "network", network);
    
    // Créer l'objet statistiques
    cJSON* stats = cJSON_CreateObject();
    cJSON_AddNumberToObject(stats, "delivered_packages", state->stats.delivered_packages);
    cJSON_AddNumberToObject(stats, "active_vehicles", state->stats.active_vehicles);
    cJSON_AddNumberToObject(stats, "average_delivery_time", state->stats.average_delivery_time);
    cJSON_AddNumberToObject(stats, "network_efficiency", state->stats.network_efficiency);
    cJSON_AddItemToObject(root, "statistics", stats);
    
    // Écrire le JSON dans le fichier
    char* json_string = cJSON_Print(root);
    FILE* file = fopen(filename, "w");
    if (file) {
        fputs(json_string, file);
        fclose(file);
    }
    free(json_string);
    cJSON_Delete(root);
}

// Fonction pour libérer la mémoire
void freeNetworkState(NetworkState* state) {
    if (!state) return;
    
    // Libérer les véhicules
    for (int i = 0; i < state->vehicle_count; i++) {
        free(state->vehicles[i].route);
        free(state->vehicles[i].packages);
    }
    free(state->vehicles);
    
    // Libérer les colis
    free(state->packages);
    
    // Libérer les nœuds
    free(state->nodes);
    
    // Libérer les arêtes
    free(state->edges);
    
    // Libérer la structure principale
    free(state);
} 