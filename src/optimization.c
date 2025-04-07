#include "../include/project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

// ====================
// Algorithmes d'optimisation
// ====================

void floydWarshall(Graph* g, float** dist) {
    int V = g->V;
    
    // Initialisation
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) dist[i][j] = 0;
            else dist[i][j] = INF;
        }
    }
    
    // Remplissage avec les distances directes
    for (int i = 0; i < V; i++) {
        AdjListNode* node = g->array[i].head;
        while (node) {
            dist[i][node->dest] = node->attr.distance;
            node = node->next;
        }
    }
    
    // Algorithme principal
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }
}

int* bellmanFord(Graph* g, int src) {
    int V = g->V;
    float* dist = malloc(V * sizeof(float));
    int* pred = malloc(V * sizeof(int));
    
    // Initialisation
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        pred[i] = -1;
    }
    dist[src] = 0;
    
    // Relaxation des arêtes
    for (int i = 1; i < V; i++) {
        for (int u = 0; u < V; u++) {
            AdjListNode* node = g->array[u].head;
            while (node) {
                int v = node->dest;
                float weight = node->attr.cost;
                if (dist[u] != INF && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    pred[v] = u;
                }
                node = node->next;
            }
        }
    }
    
    // Vérification des cycles négatifs
    for (int u = 0; u < V; u++) {
        AdjListNode* node = g->array[u].head;
        while (node) {
            int v = node->dest;
            float weight = node->attr.cost;
            if (dist[u] != INF && dist[u] + weight < dist[v]) {
                free(dist);
                free(pred);
                return NULL; // Cycle négatif détecté
            }
            node = node->next;
        }
    }
    
    free(dist);
    return pred;
}

int* tspNearestNeighbor(Graph* g, int start) {
    int V = g->V;
    int* tour = malloc(V * sizeof(int));
    bool* visited = calloc(V, sizeof(bool));
    
    tour[0] = start;
    visited[start] = true;
    
    for (int i = 1; i < V; i++) {
        int last = tour[i-1];
        int next = -1;
        float best = INF;
        
        AdjListNode* node = g->array[last].head;
        while (node) {
            if (!visited[node->dest] && node->attr.distance < best) {
            if (!visited[node->dest] && node->attr.distance < min_dist) {
                min_dist = node->attr.distance;
                next = node->dest;
            }
            node = node->next;
        }
        
        if (next == -1) {
            // Aucun voisin non visité trouvé
            free(tour);
            free(visited);
            return NULL;
        }
        
        tour[tour_index++] = next;
        visited[next] = 1;
        current = next;
    }
    
    // Retour au point de départ
    tour[tour_index] = start;
    
    free(visited);
    return tour;
}

void multiDayPlanning(Graph* g, Package* packages, int num_packages,
                     Vehicle* vehicles, int num_vehicles) {
    // Tri des colis par priorité et fenêtre de livraison
    for (int i = 0; i < num_packages - 1; i++) {
        for (int j = 0; j < num_packages - i - 1; j++) {
            if (packages[j].priority < packages[j + 1].priority ||
                (packages[j].priority == packages[j + 1].priority &&
                 packages[j].delivery_window_start > packages[j + 1].delivery_window_start)) {
                Package temp = packages[j];
                packages[j] = packages[j + 1];
                packages[j + 1] = temp;
            }
        }
    }
    
    // Allocation des colis aux véhicules
    int* vehicle_load = (int*)calloc(num_vehicles, sizeof(int));
    float* vehicle_capacity = (float*)calloc(num_vehicles, sizeof(float));
    
    for (int i = 0; i < num_vehicles; i++) {
        vehicle_capacity[i] = vehicles[i].capacity;
    }
    
    for (int i = 0; i < num_packages; i++) {
        int best_vehicle = -1;
        float best_cost = INF;
        
        for (int j = 0; j < num_vehicles; j++) {
            if (vehicle_load[j] < vehicle_capacity[j]) {
                float cost = calculateRouteCost(g, 
                    &vehicles[j].current_location, 1);
                
                if (cost < best_cost) {
                    best_cost = cost;
                    best_vehicle = j;
                }
            }
        }
        
        if (best_vehicle != -1) {
            vehicle_load[best_vehicle]++;
            // Mise à jour de la position du véhicule
            vehicles[best_vehicle].current_location = packages[i].destination;
        }
    }
    
    free(vehicle_load);
    free(vehicle_capacity);
}

// ====================
// Algorithmes gloutons
// ====================

void greedyAssignment(Graph* g, Package* packages, int num_packages,
                     Vehicle* vehicles, int num_vehicles) {
    // Tri des colis par volume décroissant
    for (int i = 0; i < num_packages - 1; i++) {
        for (int j = 0; j < num_packages - i - 1; j++) {
            if (packages[j].volume < packages[j + 1].volume) {
                Package temp = packages[j];
                packages[j] = packages[j + 1];
                packages[j + 1] = temp;
            }
        }
    }
    
    // Allocation gloutonne
    float* remaining_capacity = (float*)malloc(num_vehicles * sizeof(float));
    for (int i = 0; i < num_vehicles; i++) {
        remaining_capacity[i] = vehicles[i].capacity;
    }
    
    for (int i = 0; i < num_packages; i++) {
        int best_vehicle = -1;
        float min_waste = INF;
        
        for (int j = 0; j < num_vehicles; j++) {
            if (remaining_capacity[j] >= packages[i].volume) {
                float waste = remaining_capacity[j] - packages[i].volume;
                if (waste < min_waste) {
                    min_waste = waste;
                    best_vehicle = j;
                }
            }
        }
        
        if (best_vehicle != -1) {
            remaining_capacity[best_vehicle] -= packages[i].volume;
            // Mise à jour de la position du véhicule
            vehicles[best_vehicle].current_location = packages[i].destination;
        }
    }
    
    free(remaining_capacity);
}

void dailyPlanning(Graph* g, Package* packages, int num_packages,
                  Vehicle* vehicles, int num_vehicles) {
    // Planification journalière basée sur les fenêtres de livraison
    int current_time = 0; // Temps en minutes depuis minuit
    
    while (current_time < 1440) { // 24 heures
        // Trouver le colis avec la fenêtre de livraison la plus proche
        int next_package = -1;
        int earliest_window = INT_MAX;
        
        for (int i = 0; i < num_packages; i++) {
            if (packages[i].delivery_window_start >= current_time &&
                packages[i].delivery_window_start < earliest_window) {
                earliest_window = packages[i].delivery_window_start;
                next_package = i;
            }
        }
        
        if (next_package == -1) break;
        
        // Trouver le véhicule disponible le plus proche
        int best_vehicle = -1;
        float min_time = INF;
        
        for (int i = 0; i < num_vehicles; i++) {
            if (vehicles[i].available_time <= current_time) {
                float travel_time = calculateRouteCost(g, 
                    &vehicles[i].current_location, 1);
                
                if (travel_time < min_time) {
                    min_time = travel_time;
                    best_vehicle = i;
                }
            }
        }
        
        if (best_vehicle != -1) {
            // Mise à jour des temps
            current_time = MAX(current_time + (int)min_time,
                             packages[next_package].delivery_window_start);
            vehicles[best_vehicle].available_time = current_time;
            vehicles[best_vehicle].current_location = packages[next_package].destination;
        }
        
        current_time++;
    }
}

void dynamicReallocation(Graph* g, Package* packages, int num_packages,
                        Vehicle* vehicles, int num_vehicles, int failed_edge) {
    // Identification des colis affectés par la défaillance
    int* affected_packages = (int*)calloc(num_packages, sizeof(int));
    int num_affected = 0;
    
    for (int i = 0; i < num_packages; i++) {
        if (packages[i].origin == failed_edge || 
            packages[i].destination == failed_edge) {
            affected_packages[num_affected++] = i;
        }
    }
    
    // Réallocation des colis affectés
    for (int i = 0; i < num_affected; i++) {
        int package_index = affected_packages[i];
        float min_detour = INF;
        int best_vehicle = -1;
        
        // Recherche du meilleur détour
        for (int j = 0; j < num_vehicles; j++) {
            if (vehicles[j].available_time < packages[package_index].delivery_window_end) {
                float detour = calculateRouteCost(g, 
                    &vehicles[j].current_location, 1);
                
                if (detour < min_detour) {
                    min_detour = detour;
                    best_vehicle = j;
                }
            }
        }
        
        if (best_vehicle != -1) {
            // Mise à jour de la position du véhicule
            vehicles[best_vehicle].current_location = packages[package_index].destination;
            vehicles[best_vehicle].available_time += (int)min_detour;
        }
    }
    
    free(affected_packages);
} 