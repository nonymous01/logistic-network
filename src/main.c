#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <pthread.h>
#include "project.h"

// Constantes pour l'algorithme génétique
#define POPULATION_SIZE 100
#define GENERATIONS 50
#define ELITE_SIZE 10
#define MUTATION_RATE 0.1

// Implémentation des fonctions de base du graphe
Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    graph->array = (AdjList*)malloc(V * sizeof(AdjList));
    
    for (int i = 0; i < V; i++) {
        graph->array[i].head = NULL;
        graph->array[i].node_attr.type = NODE_DELIVERY_POINT;
        graph->array[i].node_attr.capacity = 0;
        graph->array[i].node_attr.has_charging_station = false;
        graph->array[i].node_attr.service_time = 0;
        graph->array[i].node_attr.name = NULL;
    }
    
    return graph;
}

AdjNode* newAdjNode(int dest, EdgeAttr attr) {
    AdjNode* newNode = (AdjNode*)malloc(sizeof(AdjNode));
    newNode->dest = dest;
    newNode->attr = attr;
    newNode->next = NULL;
    return newNode;
}

void addEdge(Graph* g, int src, int dest, EdgeAttr attr) {
    AdjNode* newNode = newAdjNode(dest, attr);
    newNode->next = g->array[src].head;
    g->array[src].head = newNode;
}

void removeEdge(Graph* g, int src, int dest) {
    AdjNode* current = g->array[src].head;
    AdjNode* prev = NULL;
    
    while (current && current->dest != dest) {
        prev = current;
        current = current->next;
    }
    
    if (current) {
        if (prev) {
            prev->next = current->next;
        } else {
            g->array[src].head = current->next;
        }
        free(current);
    }
}

void printGraph(Graph* g) {
    printf("\nStructure du réseau :\n\n");
    for (int v = 0; v < g->V; v++) {
        printf("Liste d'adjacence du sommet %d\n", v);
        printf("Type: %d, Capacité: %d, Borne de recharge: %s\n",
               g->array[v].node_attr.type,
               g->array[v].node_attr.capacity,
               g->array[v].node_attr.has_charging_station ? "Oui" : "Non");
        
        AdjNode* node = g->array[v].head;
        while (node) {
            printf("-> %d (distance: %.2f km, vitesse: %.2f km/h, état: %d)\n",
                   node->dest,
                   node->attr.distance,
                   node->attr.speed_limit,
                   node->attr.condition);
            node = node->next;
        }
        printf("\n\n");
    }
}

void freeGraph(Graph* g) {
    for (int i = 0; i < g->V; i++) {
        AdjNode* current = g->array[i].head;
        while (current) {
            AdjNode* next = current->next;
            free(current);
            current = next;
        }
        if (g->array[i].node_attr.name) {
            free(g->array[i].node_attr.name);
        }
    }
    free(g->array);
    free(g);
}

// Implémentation des algorithmes de parcours
void DFS_util(Graph* g, int v, bool* visited) {
    visited[v] = true;
    printf("%d ", v);

    AdjNode* pCrawl = g->array[v].head;
    while (pCrawl) {
        if (!visited[pCrawl->dest])
            DFS_util(g, pCrawl->dest, visited);
        pCrawl = pCrawl->next;
    }
}

void DFS(Graph* g, int start, bool* visited) {
    DFS_util(g, start, visited);
}

void BFS(Graph* g, int start) {
    bool* visited = (bool*)calloc(g->V, sizeof(bool));
    int* queue = (int*)malloc(g->V * sizeof(int));
    int front = 0, rear = 0;

    visited[start] = true;
    queue[rear++] = start;

    while (front < rear) {
        int v = queue[front++];
        printf("%d ", v);

        AdjNode* pCrawl = g->array[v].head;
        while (pCrawl) {
            if (!visited[pCrawl->dest]) {
                visited[pCrawl->dest] = true;
                queue[rear++] = pCrawl->dest;
            }
            pCrawl = pCrawl->next;
        }
    }

    free(visited);
    free(queue);
}

bool detectCycleUtil(Graph* g, int v, bool* visited, bool* recStack) {
    if (!visited[v]) {
        visited[v] = true;
        recStack[v] = true;

        AdjNode* pCrawl = g->array[v].head;
        while (pCrawl) {
            if (!visited[pCrawl->dest] && detectCycleUtil(g, pCrawl->dest, visited, recStack))
                return true;
            else if (recStack[pCrawl->dest])
                return true;
            pCrawl = pCrawl->next;
        }
    }
    recStack[v] = false;
    return false;
}

bool detectCycle(Graph* g) {
    bool* visited = (bool*)calloc(g->V, sizeof(bool));
    bool* recStack = (bool*)calloc(g->V, sizeof(bool));

    for (int i = 0; i < g->V; i++)
        if (!visited[i] && detectCycleUtil(g, i, visited, recStack)) {
            free(visited);
            free(recStack);
            return true;
        }

    free(visited);
    free(recStack);
    return false;
}

// Créer un réseau de test
Graph* createSampleNetwork() {
    Graph* g = createGraph(5);
    
    // Configuration des nœuds
    g->array[0].node_attr.type = NODE_DISTRIBUTION_CENTER;
    g->array[0].node_attr.capacity = 1000;
    g->array[0].node_attr.has_charging_station = true;
    g->array[0].node_attr.service_time = 30;
    
    g->array[1].node_attr.type = NODE_RELAY_POINT;
    g->array[1].node_attr.capacity = 500;
    g->array[1].node_attr.has_charging_station = true;
    g->array[1].node_attr.service_time = 15;
    
    g->array[2].node_attr.type = NODE_RELAY_POINT;
    g->array[2].node_attr.capacity = 500;
    g->array[2].node_attr.has_charging_station = false;
    g->array[2].node_attr.service_time = 20;
    
    g->array[3].node_attr.type = NODE_REST_AREA;
    g->array[3].node_attr.capacity = 0;
    g->array[3].node_attr.has_charging_station = true;
    g->array[3].node_attr.service_time = 45;
    
    g->array[4].node_attr.type = NODE_DELIVERY_POINT;
    g->array[4].node_attr.capacity = 0;
    g->array[4].node_attr.has_charging_station = false;
    g->array[4].node_attr.service_time = 10;
    
    // Configuration des arêtes
    EdgeAttr attr;
    
    // Route 0 -> 1 (excellente)
    attr.distance = 10.0;
    attr.speed_limit = 90.0;
    attr.condition = ROAD_EXCELLENT;
    attr.traffic_level = 20;
    attr.restrictions = 0;
    attr.cost = 5.0;
    addEdge(g, 0, 1, attr);
    addEdge(g, 1, 0, attr);
    
    // Route 1 -> 2 (bonne)
    attr.distance = 15.0;
    attr.speed_limit = 70.0;
    attr.condition = ROAD_GOOD;
    attr.traffic_level = 40;
    attr.restrictions = 0;
    attr.cost = 8.0;
    addEdge(g, 1, 2, attr);
    addEdge(g, 2, 1, attr);
    
    // Route 0 -> 2 (moyenne)
    attr.distance = 20.0;
    attr.speed_limit = 50.0;
    attr.condition = ROAD_FAIR;
    attr.traffic_level = 60;
    attr.restrictions = 0;
    attr.cost = 12.0;
    addEdge(g, 0, 2, attr);
    addEdge(g, 2, 0, attr);
    
    printf("Réseau de test créé.\n");
    return g;
}

// Créer des véhicules de test
Vehicle* createSampleVehicles(int* num_vehicles) {
    *num_vehicles = 3;
    Vehicle* vehicles = (Vehicle*)malloc(*num_vehicles * sizeof(Vehicle));
    
    // Large electric vehicle
    vehicles[0].id = 0;
    vehicles[0].type = VEHICLE_LARGE;
    vehicles[0].capacity = 2000.0f;
    vehicles[0].volume_capacity = 20.0f;
    vehicles[0].speed = 80.0f;
    vehicles[0].cost_per_km = 0.5f;
    vehicles[0].battery_capacity = 100.0f;
    vehicles[0].current_battery = 100.0f;
    vehicles[0].consumption_rate = 0.2f;
    vehicles[0].charging_rate = 50.0f;
    vehicles[0].current_location = 0;
    vehicles[0].driving_time = 0.0f;
    vehicles[0].last_break_time = 0.0f;
    vehicles[0].max_driving_time = 4.5f;
    
    // Medium electric vehicle
    vehicles[1].id = 1;
    vehicles[1].type = VEHICLE_MEDIUM;
    vehicles[1].capacity = 1000.0f;
    vehicles[1].volume_capacity = 10.0f;
    vehicles[1].speed = 90.0f;
    vehicles[1].cost_per_km = 0.3f;
    vehicles[1].battery_capacity = 75.0f;
    vehicles[1].current_battery = 75.0f;
    vehicles[1].consumption_rate = 0.15f;
    vehicles[1].charging_rate = 40.0f;
    vehicles[1].current_location = 0;
    vehicles[1].driving_time = 0.0f;
    vehicles[1].last_break_time = 0.0f;
    vehicles[1].max_driving_time = 4.5f;
    
    // Small electric vehicle
    vehicles[2].id = 2;
    vehicles[2].type = VEHICLE_SMALL;
    vehicles[2].capacity = 500.0f;
    vehicles[2].volume_capacity = 5.0f;
    vehicles[2].speed = 100.0f;
    vehicles[2].cost_per_km = 0.2f;
    vehicles[2].battery_capacity = 50.0f;
    vehicles[2].current_battery = 50.0f;
    vehicles[2].consumption_rate = 0.1f;
    vehicles[2].charging_rate = 30.0f;
    vehicles[2].current_location = 0;
    vehicles[2].driving_time = 0.0f;
    vehicles[2].last_break_time = 0.0f;
    vehicles[2].max_driving_time = 4.5f;
    
    return vehicles;
}

// Créer des colis de test
Package* createSamplePackages(int* num_packages) {
    *num_packages = 2;
    Package* packages = (Package*)malloc(*num_packages * sizeof(Package));
    
    // Premier colis
    packages[0].id = 0;
    packages[0].source = 0;
    packages[0].destination = 1;
    packages[0].weight = 100.0;
    packages[0].volume = 0.5;
    packages[0].priority = 5.0;
    packages[0].is_assigned = false;
    packages[0].is_delivered = false;
    packages[0].assigned_vehicle = -1;
    packages[0].delivery_window_start = 0.0;
    packages[0].delivery_window_end = 480.0;  // 8 heures
    
    // Deuxième colis
    packages[1].id = 1;
    packages[1].source = 0;
    packages[1].destination = 2;
    packages[1].weight = 200.0;
    packages[1].volume = 1.0;
    packages[1].priority = 3.0;
    packages[1].is_assigned = false;
    packages[1].is_delivered = false;
    packages[1].assigned_vehicle = -1;
    packages[1].delivery_window_start = 120.0;  // 2 heures
    packages[1].delivery_window_end = 600.0;    // 10 heures
    
    printf("Colis de test créés : %d colis\n", *num_packages);
    return packages;
}

// Fonctions de comparaison
int comparePackages(const void* a, const void* b) {
    const Package* p1 = (const Package*)a;
    const Package* p2 = (const Package*)b;
    
    // Comparaison par priorité d'abord
    if (p1->priority != p2->priority)
        return p1->priority - p2->priority;
    
    // Si même priorité, comparer par début de fenêtre de livraison
    return p1->delivery_window_start - p2->delivery_window_start;
}

int compareVehicles(const void* a, const void* b) {
    const Vehicle* v1 = (const Vehicle*)a;
    const Vehicle* v2 = (const Vehicle*)b;
    
    // Comparaison par capacité
    if (v1->capacity != v2->capacity)
        return (v1->capacity > v2->capacity) ? -1 : 1;
    
    // Si même capacité, comparer par coût par km
    return (v1->cost_per_km < v2->cost_per_km) ? -1 : 1;
}

int compareSolutions(const void* a, const void* b) {
    const Solution* sol1 = (const Solution*)a;
    const Solution* sol2 = (const Solution*)b;
    
    if (sol1->fitness > sol2->fitness) return -1;
    if (sol1->fitness < sol2->fitness) return 1;
    return 0;
}

// Calculer le temps de trajet entre deux points
float calculateTravelTime(Graph* g, int src, int dest, float vehicle_speed) {
    AdjNode* current = g->array[src].head;
    while (current && current->dest != dest) {
        current = current->next;
    }
    
    if (!current) return INF;
    
    float effective_speed = (vehicle_speed < current->attr.speed_limit) ?
                           vehicle_speed : current->attr.speed_limit;
    
    // Ajuster la vitesse selon l'état de la route
    switch (current->attr.condition) {
        case ROAD_EXCELLENT:
            effective_speed *= 1.0;
            break;
        case ROAD_GOOD:
            effective_speed *= 0.9;
            break;
        case ROAD_FAIR:
            effective_speed *= 0.7;
            break;
        case ROAD_POOR:
            effective_speed *= 0.5;
            break;
        case ROAD_IMPASSABLE:
            return INF;
    }
    
    // Ajuster la vitesse selon le trafic
    float traffic_factor = 1.0 - (current->attr.traffic_level / 100.0);
    effective_speed *= traffic_factor;
    
    // Calculer le temps en minutes
    return (current->attr.distance / effective_speed) * 60.0;
}

// Vérifier si un véhicule peut livrer un colis
bool canVehicleDeliverPackage(Vehicle* vehicle, Package* package, Graph* g) {
    // Vérifier la capacité
    if (package->weight > vehicle->capacity) {
        printf("Véhicule %d : capacité insuffisante (%.2f > %.2f)\n",
               vehicle->id, package->weight, vehicle->capacity);
        return false;
    }
    
    // Calculer le temps de trajet
    float time_to_source = calculateTravelTime(g, vehicle->current_location,
                                             package->source, vehicle->speed);
    
    if (time_to_source == INF) {
        printf("Véhicule %d : pas de route vers la source\n", vehicle->id);
        return false;
    }
    
    float time_to_dest = calculateTravelTime(g, package->source,
                                           package->destination, vehicle->speed);
    
    if (time_to_dest == INF) {
        printf("Véhicule %d : pas de route vers la destination\n", vehicle->id);
        return false;
    }
    
    // Vérifier la fenêtre de livraison
    float total_time = time_to_source + time_to_dest +
                      g->array[package->source].node_attr.service_time +
                      g->array[package->destination].node_attr.service_time;
    
    if (total_time > package->delivery_window_end) {
        printf("Véhicule %d : délai de livraison trop long (%.2f > %.2f)\n",
               vehicle->id, total_time, package->delivery_window_end);
        return false;
    }
    
    return true;
}

// Fonction pour calculer la distance totale d'une route
float calculateRouteDistance(Graph* g, int* route, int num_nodes) {
    float total_distance = 0.0;
    for (int i = 0; i < num_nodes - 1; i++) {
        AdjNode* current = g->array[route[i]].head;
        while (current && current->dest != route[i + 1]) {
            current = current->next;
        }
        if (current) {
            total_distance += current->attr.distance;
        }
    }
    return total_distance;
}

// Fonction pour inverser une section de route
void reverseRoute(int* route, int start, int end) {
    while (start < end) {
        int temp = route[start];
        route[start] = route[end];
        route[end] = temp;
        start++;
        end--;
    }
}

// Fonction pour optimiser une route avec l'algorithme 2-opt
void optimizeRoute(Route* route, Vehicle* vehicle, Graph* g) {
    int* new_route = (int*)malloc(route->num_nodes * sizeof(int));
    memcpy(new_route, route->nodes, route->num_nodes * sizeof(int));
    
    float best_cost = route->total_cost;
    bool improved = true;
    
    while (improved) {
        improved = false;
        
        for (int i = 1; i < route->num_nodes - 1; i++) {
            for (int j = i + 1; j < route->num_nodes; j++) {
                // Try swapping nodes i and j
                int temp = new_route[i];
                new_route[i] = new_route[j];
                new_route[j] = temp;
                
                // Calculate new cost
                float new_cost = 0.0f;
                float new_time = 0.0f;
                float current_battery = vehicle->current_battery;
                float current_driving_time = vehicle->driving_time;
                
                for (int k = 0; k < route->num_nodes - 1; k++) {
                    float required_energy = calculateEnergyConsumption(g, new_route[k], 
                                                                    new_route[k+1], vehicle);
                    float travel_time = calculateTravelTime(g, new_route[k], 
                                                         new_route[k+1], vehicle->speed);
                    
                    // Check if we need to charge
                    if (current_battery < required_energy) {
                        int charging_station = findNearestChargingStation(g, new_route[k]);
                        if (charging_station != -1) {
                            // Add charging time
                            float charging_time = calculateChargingTime(vehicle);
                            new_time += charging_time;
                            chargeVehicle(vehicle, charging_time);
                            current_battery = vehicle->current_battery;
                        }
                    }
                    
                    // Check if we need a break
                    if (needsBreak(vehicle, current_driving_time + travel_time)) {
                        int rest_area = findNearestRestArea(g, new_route[k]);
                        if (rest_area != -1) {
                            // Add break time
                            new_time += BREAK_DURATION;
                            current_driving_time = 0.0f;
                        }
                    }
                    
                    current_battery -= required_energy;
                    current_driving_time += travel_time;
                    new_cost += required_energy * vehicle->cost_per_km;
                    new_time += travel_time;
                }
                
                // If the new route is better, keep it
                if (new_cost < best_cost) {
                    best_cost = new_cost;
                    memcpy(route->nodes, new_route, route->num_nodes * sizeof(int));
                    route->total_cost = new_cost;
                    route->total_time = new_time;
                    improved = true;
                } else {
                    // Otherwise, swap back
                    temp = new_route[i];
                    new_route[i] = new_route[j];
                    new_route[j] = temp;
                }
            }
        }
    }
    
    free(new_route);
    
    // Plan charging stops and breaks
    planChargingStops(route, vehicle, g);
    planBreaks(route, vehicle, g);
}

// Create an empty loading
Loading createEmptyLoading() {
    Loading loading;
    loading.package_indices = NULL;
    loading.num_packages = 0;
    loading.total_weight = 0.0f;
    loading.total_volume = 0.0f;
    loading.total_value = 0.0f;
    return loading;
}

// Add a package to a loading
void addPackageToLoading(Loading* loading, int package_index, Package* package) {
    // Reallocate memory for the new package index
    loading->package_indices = realloc(loading->package_indices, 
                                     (loading->num_packages + 1) * sizeof(int));
    loading->package_indices[loading->num_packages] = package_index;
    
    // Update loading metrics
    loading->num_packages++;
    loading->total_weight += package->weight;
    loading->total_volume += package->volume;
    loading->total_value += package->priority;
}

// Free memory allocated for a loading
void freeLoading(Loading* loading) {
    if (loading->package_indices != NULL) {
        free(loading->package_indices);
        loading->package_indices = NULL;
    }
    loading->num_packages = 0;
    loading->total_weight = 0.0f;
    loading->total_volume = 0.0f;
    loading->total_value = 0.0f;
}

// Update package assignments from a loading
void updateAssignmentsFromLoading(Loading* loading, Package* packages, int vehicle_id) {
    for (int i = 0; i < loading->num_packages; i++) {
        int package_index = loading->package_indices[i];
        packages[package_index].assigned_vehicle = vehicle_id;
    }
}

// Fonction pour optimiser le chargement d'un véhicule
Loading optimizeLoading(Vehicle* vehicle, Package* packages, int num_packages) {
    Loading best_loading = createEmptyLoading();
    Loading current_loading = createEmptyLoading();
    
    // Create an array of package indices
    int* indices = (int*)malloc(num_packages * sizeof(int));
    for (int i = 0; i < num_packages; i++) {
        indices[i] = i;
    }
    
    // Try all possible combinations
    for (int i = 0; i < num_packages; i++) {
        // Reset current loading
        freeLoading(&current_loading);
        current_loading = createEmptyLoading();
        
        // Try adding packages starting from index i
        for (int j = i; j < num_packages; j++) {
            int package_index = indices[j];
            addPackageToLoading(&current_loading, package_index, &packages[package_index]);
            
            // Check if this loading is better
            if (current_loading.total_value > best_loading.total_value &&
                current_loading.total_weight <= vehicle->capacity &&
                current_loading.total_volume <= vehicle->volume_capacity) {
                // Free previous best loading
                freeLoading(&best_loading);
                
                // Copy current loading to best loading
                best_loading.package_indices = malloc(current_loading.num_packages * sizeof(int));
                memcpy(best_loading.package_indices, current_loading.package_indices,
                       current_loading.num_packages * sizeof(int));
                best_loading.num_packages = current_loading.num_packages;
                best_loading.total_weight = current_loading.total_weight;
                best_loading.total_volume = current_loading.total_volume;
                best_loading.total_value = current_loading.total_value;
            }
        }
    }
    
    // Free memory
    free(indices);
    freeLoading(&current_loading);
    
    return best_loading;
}

// Modification de la fonction greedyAssignment pour utiliser l'optimisation des chargements
void greedyAssignment(Graph* g, Package* packages, int num_packages, Vehicle* vehicles, int num_vehicles) {
    // Sort packages by priority
    int* indices = (int*)malloc(num_packages * sizeof(int));
    for (int i = 0; i < num_packages; i++) {
        indices[i] = i;
    }
    
    // Sort indices by priority (descending)
    for (int i = 0; i < num_packages - 1; i++) {
        for (int j = i + 1; j < num_packages; j++) {
            if (packages[indices[i]].priority < packages[indices[j]].priority) {
                int temp = indices[i];
                indices[i] = indices[j];
                indices[j] = temp;
            }
        }
    }
    
    // Assign packages to vehicles
    int total_assigned = 0;
    for (int v = 0; v < num_vehicles && total_assigned < num_packages; v++) {
        Loading loading = optimizeLoading(&vehicles[v], packages, num_packages);
        updateAssignmentsFromLoading(&loading, packages, v);
        total_assigned += loading.num_packages;
        
        printf("Véhicule %d : %d colis assignés (%.2f kg, %.2f m³)\n",
               v, loading.num_packages, loading.total_weight, loading.total_volume);
        
        freeLoading(&loading);
    }
    
    free(indices);
}

void dailyPlanning(Graph* g, Vehicle* vehicles, int num_vehicles, 
                  Package* packages, int num_packages) {
    printf("\nPlanification journalière en cours...\n");
    
    for (int v = 0; v < num_vehicles; v++) {
        // Compter les colis affectés à ce véhicule
        int assigned_packages = 0;
        for (int i = 0; i < num_packages; i++) {
            if (packages[i].is_assigned && packages[i].assigned_vehicle == v) {
                assigned_packages++;
            }
        }
        
        if (assigned_packages == 0) {
            printf("Véhicule %d : aucun colis affecté\n", v);
            continue;
        }
        
        printf("\nPlanification pour le véhicule %d (%d colis):\n", v, assigned_packages);
        
        // Créer une route pour le véhicule
        Route route;
        route.nodes = (int*)malloc((assigned_packages * 2 + 1) * sizeof(int));
        route.num_nodes = 0;
        
        // Ajouter le centre de distribution comme point de départ
        route.nodes[route.num_nodes++] = 0;
        
        // Ajouter les points de collecte et de livraison des colis affectés
        for (int i = 0; i < num_packages; i++) {
            if (packages[i].is_assigned && packages[i].assigned_vehicle == v) {
                route.nodes[route.num_nodes++] = packages[i].source;
                route.nodes[route.num_nodes++] = packages[i].destination;
            }
        }
        
        // Optimiser la route
        optimizeRoute(&route, &vehicles[v], g);
        
        // Afficher la route optimisée
        printf("Points visités: ");
        for (int i = 0; i < route.num_nodes; i++) {
            printf("%d ", route.nodes[i]);
        }
        printf("\n");
        
        // Calculer les retours à vide possibles
        float min_return_cost = INF;
        int best_return_node = -1;
        
        for (int i = 0; i < g->V; i++) {
            if (g->array[i].node_attr.type == NODE_DISTRIBUTION_CENTER) {
                float return_cost = calculateTravelTime(g, route.nodes[route.num_nodes - 1], 
                                                     i, vehicles[v].speed) * 
                                  vehicles[v].cost_per_km;
                if (return_cost < min_return_cost) {
                    min_return_cost = return_cost;
                    best_return_node = i;
                }
            }
        }
        
        // Si un retour à vide est possible, l'ajouter à la route
        if (best_return_node != -1 && best_return_node != route.nodes[route.num_nodes - 1]) {
            int* new_nodes = (int*)malloc((route.num_nodes + 1) * sizeof(int));
            memcpy(new_nodes, route.nodes, route.num_nodes * sizeof(int));
            new_nodes[route.num_nodes] = best_return_node;
            free(route.nodes);
            route.nodes = new_nodes;
            route.num_nodes++;
            
            printf("Retour à vide vers le centre de distribution %d\n", best_return_node);
        }
        
        printf("Temps total estimé: %.2f minutes\n", route.total_time);
        printf("Coût total estimé: %.2f euros\n", route.total_cost);
        
        free(route.nodes);
    }
}

void multiDayPlanning(Graph* g, Vehicle* vehicles, int num_vehicles, 
                     Package* packages, int num_packages) {
    printf("\nPlanification sur plusieurs jours en cours...\n");
    
    // Sort packages by delivery window
    int* indices = (int*)malloc(num_packages * sizeof(int));
    for (int i = 0; i < num_packages; i++) {
        indices[i] = i;
    }
    
    // Sort indices by delivery window start time
    for (int i = 0; i < num_packages - 1; i++) {
        for (int j = i + 1; j < num_packages; j++) {
            if (packages[indices[i]].delivery_window_start > packages[indices[j]].delivery_window_start) {
                int temp = indices[i];
                indices[i] = indices[j];
                indices[j] = temp;
            }
        }
    }
    
    // Calculate number of days needed
    int max_days = 0;
    for (int i = 0; i < num_packages; i++) {
        int days = packages[indices[i]].delivery_window_end / (24 * 60); // Convert to days
        if (days > max_days) max_days = days;
    }
    
    printf("Planification sur %d jours\n", max_days + 1);
    
    // Plan each day
    for (int day = 0; day <= max_days; day++) {
        printf("\nJour %d :\n", day + 1);
        
        // Filter packages for current day
        int day_start = day * 24 * 60;
        int day_end = (day + 1) * 24 * 60;
        
        // Count packages to deliver today
        int day_packages = 0;
        for (int i = 0; i < num_packages; i++) {
            if (!packages[indices[i]].is_delivered && 
                packages[indices[i]].delivery_window_start >= day_start && 
                packages[indices[i]].delivery_window_start < day_end) {
                day_packages++;
            }
        }
        
        if (day_packages == 0) {
            printf("Aucun colis à livrer aujourd'hui\n");
            continue;
        }
        
        printf("%d colis à livrer aujourd'hui\n", day_packages);
        
        // Reset vehicle locations to distribution center
        for (int i = 0; i < num_vehicles; i++) {
            vehicles[i].current_location = 0;
        }
        
        // Reset package assignments for today's packages
        for (int i = 0; i < num_packages; i++) {
            if (!packages[indices[i]].is_delivered && 
                packages[indices[i]].delivery_window_start >= day_start && 
                packages[indices[i]].delivery_window_start < day_end) {
                packages[indices[i]].is_assigned = false;
                packages[indices[i]].assigned_vehicle = -1;
            }
        }
        
        // Assign packages to vehicles
        greedyAssignment(g, packages, num_packages, vehicles, num_vehicles);
        
        // Plan routes for each vehicle
        dailyPlanning(g, vehicles, num_vehicles, packages, num_packages);
        
        // Mark delivered packages
        for (int i = 0; i < num_packages; i++) {
            if (packages[indices[i]].is_assigned && !packages[indices[i]].is_delivered && 
                packages[indices[i]].delivery_window_start >= day_start && 
                packages[indices[i]].delivery_window_start < day_end) {
                packages[indices[i]].is_delivered = true;
                printf("Colis %d assigné au véhicule %d\n", indices[i], packages[indices[i]].assigned_vehicle);
            }
        }
    }
    
    // Check if all packages were delivered
    int delivered = 0;
    for (int i = 0; i < num_packages; i++) {
        if (packages[i].is_delivered) delivered++;
    }
    printf("\nRésumé de la planification sur plusieurs jours :\n");
    printf("%d colis livrés sur %d\n", delivered, num_packages);
    
    free(indices);
}

void dynamicReallocation(Graph* g, Vehicle* vehicles, int num_vehicles, 
                        Package* packages, int num_packages, int failed_node) {
    printf("\nDynamic reallocation due to failure at node %d...\n", failed_node);
    
    // Identify affected packages
    int affected_packages = 0;
    for (int i = 0; i < num_packages; i++) {
        if (packages[i].is_assigned && !packages[i].is_delivered &&
            (packages[i].source == failed_node || packages[i].destination == failed_node)) {
            affected_packages++;
            printf("Package %d affected by failure\n", i);
        }
    }
    
    if (affected_packages == 0) {
        printf("No packages affected by failure\n");
        return;
    }
    
    // Reset assignments for affected packages
    for (int i = 0; i < num_packages; i++) {
        if (packages[i].is_assigned && !packages[i].is_delivered &&
            (packages[i].source == failed_node || packages[i].destination == failed_node)) {
            packages[i].is_assigned = false;
            packages[i].assigned_vehicle = -1;
        }
    }
    
    // Find replacement node
    int replacement_node = -1;
    float min_distance = INF;
    
    for (int i = 0; i < g->V; i++) {
        if (i != failed_node && g->array[i].node_attr.type == g->array[failed_node].node_attr.type) {
            float distance = calculateTravelTime(g, 0, i, vehicles[0].speed);
            if (distance < min_distance) {
                min_distance = distance;
                replacement_node = i;
            }
        }
    }
    
    if (replacement_node == -1) {
        printf("No replacement node found\n");
        return;
    }
    
    printf("Replacement node found: %d\n", replacement_node);
    
    // Update affected packages with new node
    for (int i = 0; i < num_packages; i++) {
        if (!packages[i].is_assigned && !packages[i].is_delivered &&
            (packages[i].source == failed_node || packages[i].destination == failed_node)) {
            if (packages[i].source == failed_node) {
                packages[i].source = replacement_node;
            }
            if (packages[i].destination == failed_node) {
                packages[i].destination = replacement_node;
            }
            printf("Package %d redirected to node %d\n", i, replacement_node);
        }
    }
    
    // Reassign packages to vehicles
    greedyAssignment(g, packages, num_packages, vehicles, num_vehicles);
    
    // Update vehicle routes
    dailyPlanning(g, vehicles, num_vehicles, packages, num_packages);
    
    printf("Reallocation completed\n");
}

// Créer une solution aléatoire
Solution createRandomSolution(int num_packages, int num_vehicles) {
    Solution sol;
    sol.num_packages = num_packages;
    sol.num_vehicles = num_vehicles;
    sol.assignments = (int*)malloc(num_packages * sizeof(int));
    sol.fitness = 0.0;
    
    for (int i = 0; i < num_packages; i++) {
        sol.assignments[i] = rand() % num_vehicles;
    }
    
    return sol;
}

// Calculer le score d'une solution
float calculateFitness(Solution* sol, Graph* g, Package* packages, Vehicle* vehicles) {
    float total_cost = 0.0f;
    float total_time = 0.0f;
    float priority_score = 0.0f;
    
    for (int i = 0; i < sol->num_vehicles; i++) {
        float vehicle_load = 0.0f;
        float vehicle_volume = 0.0f;
        
        for (int j = 0; j < sol->num_packages; j++) {
            if (sol->assignments[j] == i) {
                Package* pkg = &packages[j];
                vehicle_load += pkg->weight;
                vehicle_volume += pkg->volume;
                priority_score += pkg->priority;
                
                // Calculate travel time and cost from source to destination
                AdjNode* node = g->array[pkg->source].head;
                while (node != NULL && node->dest != pkg->destination) {
                    total_time += node->attr.distance / vehicles[i].speed;
                    total_cost += node->attr.cost * vehicles[i].cost_per_km;
                    node = node->next;
                }
            }
        }
        
        // Penalize if vehicle capacity is exceeded
        if (vehicle_load > vehicles[i].capacity || vehicle_volume > vehicles[i].volume_capacity) {
            return 0.0f;  // Invalid solution
        }
    }
    
    // Combine metrics into fitness score (higher is better)
    float fitness = priority_score / (1.0f + total_cost + total_time);
    return fitness;
}

// Initialiser une population
Population* initPopulation(int size, int num_packages, int num_vehicles) {
    Population* pop = (Population*)malloc(sizeof(Population));
    pop->size = size;
    pop->individuals = malloc(size * sizeof(Solution));
    
    for (int i = 0; i < size; i++) {
        Solution new_sol = createRandomSolution(num_packages, num_vehicles);
        memcpy(&pop->individuals[i], &new_sol, sizeof(Solution));
    }
    
    return pop;
}

// Évaluer un individu
void evaluateIndividual(Solution* sol, Graph* g, Package* packages, Vehicle* vehicles) {
    sol->fitness = calculateFitness(sol, g, packages, vehicles);
}

// Select a parent using tournament selection
int selectParent(Solution* individuals, int population_size) {
    // Randomly select two individuals
    int idx1 = rand() % population_size;
    int idx2 = rand() % population_size;
    
    // Return the one with better fitness
    return (individuals[idx1].fitness > individuals[idx2].fitness) ? idx1 : idx2;
}

// Croiser deux parents pour créer deux enfants
void crossover(Solution* parent1, Solution* parent2, Solution* child1, Solution* child2) {
    // Copier les propriétés des parents
    child1->num_packages = parent1->num_packages;
    child1->num_vehicles = parent1->num_vehicles;
    child2->num_packages = parent1->num_packages;
    child2->num_vehicles = parent1->num_vehicles;
    
    // Allouer la mémoire pour les enfants
    child1->assignments = (int*)malloc(parent1->num_packages * sizeof(int));
    child2->assignments = (int*)malloc(parent1->num_packages * sizeof(int));
    
    // Point de croisement aléatoire
    int crossover_point = rand() % parent1->num_packages;
    
    // Croiser les assignations
    for (int i = 0; i < parent1->num_packages; i++) {
        if (i < crossover_point) {
            child1->assignments[i] = parent1->assignments[i];
            child2->assignments[i] = parent2->assignments[i];
        } else {
            child1->assignments[i] = parent2->assignments[i];
            child2->assignments[i] = parent1->assignments[i];
        }
    }
}

// Mutate a solution
void mutate(Solution* sol, float mutation_rate) {
    for (int i = 0; i < sol->num_packages; i++) {
        if ((float)rand() / RAND_MAX < mutation_rate) {
            // Randomly reassign this package to a different vehicle
            int new_vehicle = rand() % sol->num_vehicles;
            sol->assignments[i] = new_vehicle;
        }
    }
}

// Algorithme génétique parallélisé
void geneticAlgorithm(Graph* g, Package* packages, int num_packages, Vehicle* vehicles, int num_vehicles) {
    Population pop;
    pop.size = POPULATION_SIZE;
    pop.individuals = malloc(POPULATION_SIZE * sizeof(Solution));
    
    // Initialize population
    for (int i = 0; i < POPULATION_SIZE; i++) {
        Solution new_sol = createRandomSolution(num_packages, num_vehicles);
        memcpy(&pop.individuals[i], &new_sol, sizeof(Solution));
    }
    
    Solution best_solution;
    float best_fitness = 0.0f;
    
    #pragma omp parallel
    {
        Population local_pop;
        local_pop.size = POPULATION_SIZE;
        local_pop.individuals = malloc(POPULATION_SIZE * sizeof(Solution));
        memcpy(local_pop.individuals, pop.individuals, POPULATION_SIZE * sizeof(Solution));
        
        for (int gen = 0; gen < GENERATIONS; gen++) {
            #pragma omp for
            for (int i = 0; i < POPULATION_SIZE; i++) {
                evaluateIndividual(&local_pop.individuals[i], g, packages, vehicles);
            }
            
            #pragma omp single
            {
                // Sort population by fitness
                qsort(local_pop.individuals, POPULATION_SIZE, sizeof(Solution), compareSolutions);
                
                // Keep track of best solution
                if (local_pop.individuals[0].fitness > best_fitness) {
                    memcpy(&best_solution, &local_pop.individuals[0], sizeof(Solution));
                    best_fitness = local_pop.individuals[0].fitness;
                }
            }
            
            #pragma omp for
            for (int i = ELITE_SIZE; i < POPULATION_SIZE - 1; i += 2) {
                int parent1 = selectParent(local_pop.individuals, POPULATION_SIZE);
                int parent2 = selectParent(local_pop.individuals, POPULATION_SIZE);
                
                Solution child1, child2;
                crossover(&local_pop.individuals[parent1], &local_pop.individuals[parent2], &child1, &child2);
                
                if ((float)rand() / RAND_MAX < MUTATION_RATE) {
                    mutate(&child1, MUTATION_RATE);
                }
                if ((float)rand() / RAND_MAX < MUTATION_RATE) {
                    mutate(&child2, MUTATION_RATE);
                }
                
                memcpy(&local_pop.individuals[i], &child1, sizeof(Solution));
                if (i + 1 < POPULATION_SIZE) {
                    memcpy(&local_pop.individuals[i + 1], &child2, sizeof(Solution));
                }
            }
        }
        
        free(local_pop.individuals);
    }
    
    free(pop.individuals);
    
    // Use best solution found
    printf("Meilleure solution trouvée (fitness : %f)\n", best_fitness);
    for (int i = 0; i < num_packages; i++) {
        printf("Colis %d assigné au véhicule %d\n", i, best_solution.assignments[i]);
    }
}

// Calculer la consommation d'énergie
float calculateEnergyConsumption(Graph* g, int src, int dest, Vehicle* vehicle) {
    AdjNode* node = g->array[src].head;
    float total_energy = 0.0f;
    
    while (node != NULL && node->dest != dest) {
        // Base energy consumption per km
        float base_consumption = vehicle->consumption_rate;
        
        // Adjust for road conditions
        switch (node->attr.condition) {
            case ROAD_POOR:
                base_consumption *= 1.3f;
                break;
            case ROAD_FAIR:
                base_consumption *= 1.1f;
                break;
            case ROAD_EXCELLENT:
                base_consumption *= 0.9f;
                break;
            default:
                break;
        }
        
        // Adjust for traffic
        switch (node->attr.traffic_level) {
            case TRAFFIC_HEAVY:
                base_consumption *= 1.4f;
                break;
            case TRAFFIC_MODERATE:
                base_consumption *= 1.2f;
                break;
            case TRAFFIC_LIGHT:
                base_consumption *= 1.0f;
                break;
            default:
                break;
        }
        
        // Calculate energy for this segment
        total_energy += base_consumption * node->attr.distance;
        node = node->next;
    }
    
    return total_energy;
}

// Calculer le temps de recharge
float calculateChargingTime(Vehicle* vehicle) {
    float remaining_capacity = vehicle->battery_capacity - vehicle->current_battery;
    return remaining_capacity / vehicle->charging_rate;
}

// Recharger un véhicule
void chargeVehicle(Vehicle* vehicle, float charging_time) {
    float energy_added = charging_time * vehicle->charging_rate;
    vehicle->current_battery = fmin(vehicle->battery_capacity, 
                                  vehicle->current_battery + energy_added);
}

// Planifier les arrêts de recharge
void planChargingStops(Route* route, Vehicle* vehicle, Graph* g) {
    int current_node = route->nodes[0];
    float current_battery = vehicle->current_battery;
    int num_nodes = route->num_nodes;
    
    for (int i = 1; i < num_nodes; i++) {
        int next_node = route->nodes[i];
        float energy_needed = calculateEnergyConsumption(g, current_node, next_node, vehicle);
        
        // Check if we need to charge
        if (current_battery < energy_needed) {
            // Find nearest charging station
            int charging_station = findNearestChargingStation(g, current_node);
            if (charging_station != -1) {
                // Insert charging stop
                insertNodeInRoute(route, i, charging_station);
                num_nodes++;
                i++; // Skip the charging station in next iteration
                
                // Calculate charging time needed
                float charging_time = calculateChargingTime(vehicle);
                route->total_time += charging_time;
                
                // Update battery level
                chargeVehicle(vehicle, charging_time);
                current_battery = vehicle->current_battery;
            }
        }
        
        // Update battery level after moving to next node
        current_battery -= energy_needed;
        current_node = next_node;
    }
}

// Vérifier si une pause est nécessaire
bool needsBreak(Vehicle* vehicle, float driving_time) {
    return (driving_time - vehicle->last_break_time) >= MAX_DRIVING_TIME;
}

// Planifier les pauses conducteur
void planBreaks(Route* route, Vehicle* vehicle, Graph* g) {
    int current_node = route->nodes[0];
    float current_driving_time = 0.0f;
    int num_nodes = route->num_nodes;
    
    for (int i = 1; i < num_nodes; i++) {
        int next_node = route->nodes[i];
        AdjNode* edge = g->array[current_node].head;
        float segment_time = 0.0f;
        
        // Find the edge to the next node
        while (edge != NULL && edge->dest != next_node) {
            edge = edge->next;
        }
        
        if (edge != NULL) {
            segment_time = edge->attr.distance / vehicle->speed;
            
            // Check if we need a break before this segment
            if (needsBreak(vehicle, current_driving_time + segment_time)) {
                // Find nearest rest area
                int rest_area = findNearestRestArea(g, current_node);
                if (rest_area != -1) {
                    // Insert rest stop
                    insertNodeInRoute(route, i, rest_area);
                    num_nodes++;
                    i++; // Skip the rest area in next iteration
                    
                    // Add break time
                    route->total_time += BREAK_DURATION;
                    vehicle->last_break_time = current_driving_time;
                }
            }
            
            current_driving_time += segment_time;
            updateDrivingTime(vehicle, segment_time);
        }
        
        current_node = next_node;
    }
}

// Mettre à jour le temps de conduite
void updateDrivingTime(Vehicle* vehicle, float time) {
    vehicle->driving_time += time;
    if (needsBreak(vehicle, vehicle->driving_time)) {
        vehicle->last_break_time = vehicle->driving_time;
    }
}

// Find the nearest charging station from a given node
int findNearestChargingStation(Graph* g, int current_node) {
    int nearest_station = -1;
    float min_distance = INF;
    
    for (int i = 0; i < g->V; i++) {
        if (g->array[i].node_attr.type == NODE_CHARGING_STATION) {
            float distance = calculateTravelTime(g, current_node, i, 1.0f);
            if (distance < min_distance) {
                min_distance = distance;
                nearest_station = i;
            }
        }
    }
    
    return nearest_station;
}

// Find the nearest rest area from a given node
int findNearestRestArea(Graph* g, int current_node) {
    int nearest_rest_area = -1;
    float min_distance = INF;
    
    for (int i = 0; i < g->V; i++) {
        if (g->array[i].node_attr.type == NODE_REST_AREA) {
            float distance = calculateTravelTime(g, current_node, i, 1.0f);
            if (distance < min_distance) {
                min_distance = distance;
                nearest_rest_area = i;
            }
        }
    }
    
    return nearest_rest_area;
}

// Insert a node into a route at a specific position
void insertNodeInRoute(Route* route, int position, int node) {
    // Shift all nodes after the position one place to the right
    for (int i = route->num_nodes; i > position; i--) {
        route->nodes[i] = route->nodes[i-1];
    }
    
    // Insert the new node
    route->nodes[position] = node;
    route->num_nodes++;
}

int main() {
    // Configurer l'encodage UTF-8 pour la console Windows
    SetConsoleOutputCP(CP_UTF8);
    
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));
    
    // Création du réseau de test
    Graph* g = createSampleNetwork();
    printf("Réseau de test créé.\n\n");

    // Affichage du graphe
    printf("Structure du réseau :\n");
    printGraph(g);

    // Test du parcours en profondeur
    printf("\nParcours en profondeur à partir du nœud 0 :\n");
    bool* visited = (bool*)calloc(g->V, sizeof(bool));
    DFS(g, 0, visited);
    free(visited);

    // Test du parcours en largeur
    printf("\n\nParcours en largeur à partir du nœud 0 :\n");
    BFS(g, 0);

    // Test de la détection de cycle
    printf("\n\nDétection de cycle : %s\n", detectCycle(g) ? "Cycle détecté" : "Pas de cycle");

    // Création des véhicules de test
    int num_vehicles;
    Vehicle* vehicles = createSampleVehicles(&num_vehicles);
    printf("\nVéhicules de test créés : %d véhicules\n", num_vehicles);

    // Création des colis de test
    int num_packages;
    Package* packages = createSamplePackages(&num_packages);
    printf("Colis de test créés : %d colis\n", num_packages);

    // Démarrer le serveur API dans un thread séparé
    pthread_t api_thread;
    pthread_create(&api_thread, NULL, (void*(*)(void*))startAPIServer, 
                  (void*)g, (void*)vehicles, num_vehicles, 
                  (void*)packages, num_packages);
    pthread_detach(api_thread);

    // Test des algorithmes d'optimisation
    printf("\nTest des algorithmes d'optimisation :\n");
    greedyAssignment(g, packages, num_packages, vehicles, num_vehicles);
    dailyPlanning(g, vehicles, num_vehicles, packages, num_packages);
    multiDayPlanning(g, vehicles, num_vehicles, packages, num_packages);
    dynamicReallocation(g, vehicles, num_vehicles, packages, num_packages, 1);
    geneticAlgorithm(g, packages, num_packages, vehicles, num_vehicles);

    // Libération de la mémoire
    freeGraph(g);
    free(vehicles);
    free(packages);

    return 0;
} 