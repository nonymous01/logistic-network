#include "../include/project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/**
 * @mainpage Réseau Logistique Intelligent
 * @section intro Introduction
 * Système d'optimisation de réseaux logistiques avec :
 * - Algorithmes de graphes avancés
 * - Optimisation multi-contraintes
 * - Gestion temps-réel
 * 
 * @section modules Modules clés
 * 1. @ref graph_handling - Gestion du graphe
 * 2. @ref optimization_algs - Algorithmes d'optimisation
 * 3. @ref genetic_alg - Algorithme génétique
 */

/**
 * @defgroup graph_handling Gestion du graphe
 * Création, manipulation et analyse du réseau logistique
 */

/**
 * @brief Crée un graphe vide
 * @param V Nombre de sommets
 * @return Pointeur vers le graphe alloué
 * @warning Le graphe doit être libéré avec freeGraph()
 * @ingroup graph_handling
 */
Graph* createGraph(int V);

/**
 * @brief Ajoute une arête pondérée
 * @param g Graphe cible
 * @param src Source
 * @param dest Destination
 * @param attr Attributs de l'arête
 * @return 0 si succès, -1 si erreur
 * @throws EINVAL Si le graphe ou les sommets sont invalides
 * @ingroup graph_handling
 */
int addEdge(Graph* g, int src, int dest, EdgeAttr attr);

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
    int* tour = malloc((V + 1) * sizeof(int));
    bool* visited = calloc(V, sizeof(bool));
    int tour_index = 1;
    int current = start;

    tour[0] = start;
    visited[start] = true;

    while (tour_index < V) {
        AdjListNode* node = g->array[current].head;
        int next = -1;
        float min_dist = INF;

        while (node) {
            if (!visited[node->dest] && node->attr.distance < min_dist) {
                min_dist = node->attr.distance;
                next = node->dest;
            }
            node = node->next;
        }

        if (next == -1) break;

        tour[tour_index++] = next;
        visited[next] = true;
        current = next;
    }

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
        vehicle_capacity[i] = vehicles[i].max_capacity;
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
        remaining_capacity[i] = vehicles[i].max_capacity;
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
    initMetrics();
    int current_time = 0;

    while (current_time < 1440) {
        int next_package = -1;
        int earliest_window = INT_MAX;

        // Sélection du colis
        for (int i = 0; i < num_packages; i++) {
            if (!packages[i].delivered && 
                current_time + min_time <= packages[i].delivery_window_end) {
                earliest_window = packages[i].delivery_window_start;
                next_package = i;
            }
        }

        if (next_package == -1) break;

        // Sélection du véhicule
        int best_vehicle = -1;
        float min_time = INF;
        
        for (int i = 0; i < num_vehicles; i++) {
            float travel_time = calculateRouteCost(g, 
                &vehicles[i].current_location, packages[next_package].destination);
            
            if (travel_time < min_time && 
                vehicles[i].current_load + packages[next_package].volume <= vehicles[i].max_capacity) {
                min_time = travel_time;
                best_vehicle = i;
            }
        }

        if (best_vehicle != -1) {
            // Mise à jour des états
            if (vehicles[best_vehicle].current_load + packages[next_package].volume <= vehicles[best_vehicle].max_capacity) {
                packages[next_package].delivered = true;
                vehicles[best_vehicle].current_load += packages[next_package].volume;
                // Ajouter un log de confirmation
                printf("Véhicule %d a livré le colis %d (Charge: %.2f/%.2f kg)\n", 
                      best_vehicle, next_package,
                      vehicles[best_vehicle].current_load,
                      vehicles[best_vehicle].max_capacity);
            }
            vehicles[best_vehicle].current_location = packages[next_package].destination;
            current_time += (int)min_time;

            // Journalisation
            logMetric(&g_perf_stats.capacity_ts, 
                     vehicles[best_vehicle].current_load / vehicles[best_vehicle].max_capacity * 100,
                     CAPACITY_METRIC);
            g_perf_stats.successful_assignments++;
        }
        
        current_time++;
    }
    visualizeMetrics();
}

void dynamicReallocation(Graph* g, Package* packages, int num_packages,
                        Vehicle* vehicles, int num_vehicles, int failed_edge) {
    // Identification des colis affectés par la défaillance
    int* affected_packages = (int*)calloc(num_packages, sizeof(int));
    int num_affected = 0;
    
    for (int i = 0; i < num_packages; i++) {
        // Vérifier les routes passant par le nœud défaillant
        if (packages[i].route != NULL && 
            containsEdge(packages[i].route, failed_edge)) {
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

// ====================
// Structures modifiées
// ====================
typedef struct Package {
    int id;
    float volume;
    int priority;
    int origin;
    int destination;
    int delivery_window_start;
    int delivery_window_end;
    bool delivered;  // Nouveau champ
} Package;

typedef struct {
    float avg_fitness;
    int successful_assignments;
    float avg_reassignment_time;
    TimeSeries capacity_ts;
    TimeSeries delivery_ts;
} PerformanceStats;

// ====================
// Variables globales
// ====================
_Thread_local ErrorCode last_error;
_Thread_local char error_message[256];
PerformanceStats g_perf_stats;

// ====================
// Fonctions modifiées
// ====================
int* tspNearestNeighbor(Graph* g, int start) {
    int V = g->V;
    int* tour = malloc((V + 1) * sizeof(int));
    bool* visited = calloc(V, sizeof(bool));
    int tour_index = 1;
    int current = start;

    tour[0] = start;
    visited[start] = true;

    while (tour_index < V) {
        AdjListNode* node = g->array[current].head;
        int next = -1;
        float min_dist = INF;

        while (node) {
            if (!visited[node->dest] && node->attr.distance < min_dist) {
                min_dist = node->attr.distance;
                next = node->dest;
            }
            node = node->next;
        }

        if (next == -1) break;

        tour[tour_index++] = next;
        visited[next] = true;
        current = next;
    }

    tour[tour_index] = start;
    free(visited);
    return tour;
}

void dailyPlanning(Graph* g, Package* packages, int num_packages,
                  Vehicle* vehicles, int num_vehicles) {
    initMetrics();
    int current_time = 0;

    while (current_time < 1440) {
        int next_package = -1;
        int earliest_window = INT_MAX;

        // Sélection du colis
        for (int i = 0; i < num_packages; i++) {
            if (!packages[i].delivered && 
                current_time + min_time <= packages[i].delivery_window_end) {
                earliest_window = packages[i].delivery_window_start;
                next_package = i;
            }
        }

        if (next_package == -1) break;

        // Sélection du véhicule
        int best_vehicle = -1;
        float min_time = INF;
        
        for (int i = 0; i < num_vehicles; i++) {
            float travel_time = calculateRouteCost(g, 
                &vehicles[i].current_location, packages[next_package].destination);
            
            if (travel_time < min_time && 
                vehicles[i].current_load + packages[next_package].volume <= vehicles[i].max_capacity) {
                min_time = travel_time;
                best_vehicle = i;
            }
        }

        if (best_vehicle != -1) {
            // Mise à jour des états
            if (vehicles[best_vehicle].current_load + packages[next_package].volume <= vehicles[best_vehicle].max_capacity) {
                packages[next_package].delivered = true;
                vehicles[best_vehicle].current_load += packages[next_package].volume;
                // Ajouter un log de confirmation
                printf("Véhicule %d a livré le colis %d (Charge: %.2f/%.2f kg)\n", 
                      best_vehicle, next_package,
                      vehicles[best_vehicle].current_load,
                      vehicles[best_vehicle].max_capacity);
            }
            vehicles[best_vehicle].current_location = packages[next_package].destination;
            current_time += (int)min_time;

            // Journalisation
            logMetric(&g_perf_stats.capacity_ts, 
                     vehicles[best_vehicle].current_load / vehicles[best_vehicle].max_capacity * 100,
                     CAPACITY_METRIC);
            g_perf_stats.successful_assignments++;
        }
        
        current_time++;
    }
    visualizeMetrics();
}

// ====================
// Système de métriques
// ====================
void initMetrics() {
    g_perf_stats = (PerformanceStats){
        .capacity_ts = {.capacity = 1000, .count = 0},
        .delivery_ts = {.capacity = 1000, .count = 0}
    };
    g_perf_stats.capacity_ts.points = malloc(1000 * sizeof(DataPoint));
    g_perf_stats.delivery_ts.points = malloc(1000 * sizeof(DataPoint));
}

void logMetric(TimeSeries* ts, float value, int type) {
    if (ts->count >= ts->capacity) {
        ts->capacity *= 2;
        ts->points = realloc(ts->points, ts->capacity * sizeof(DataPoint));
    }
    
    ts->points[ts->count++] = (DataPoint){
        .timestamp = time(NULL),
        .metric_value = value,
        .metric_type = type
    };
}

// ====================
// Visualisation
// ====================
void visualizeMetrics() {
    FILE* fp = fopen("dashboard.html", "w");
    fprintf(fp, "<!DOCTYPE html><html><head>\n");
    fprintf(fp, "<script src='https://cdn.plot.ly/plotly-latest.min.js'></script>\n");
    fprintf(fp, "<style>.plot-container {margin: 20px; padding: 20px; border: 1px solid #ddd;}</style>\n");
    fprintf(fp, "</head><body>\n");

    // Graphique de capacité
    fprintf(fp, "<div id='capacity' class='plot-container'></div>\n");
    
    // Graphique de livraisons
    fprintf(fp, "<div id='deliveries' class='plot-container'></div>\n");

    // Script de données
    fprintf(fp, "<script>\n");
    fprintf(fp, "const capacityData = {\n");
    fprintf(fp, "  x: [%s],\n", getTimestamps(&g_perf_stats.capacity_ts));
    fprintf(fp, "  y: [%s],\n", getValues(&g_perf_stats.capacity_ts));
    fprintf(fp, "  type: 'scatter', name: 'Capacité'\n};\n");
    
    fprintf(fp, "const deliveryData = {\n");
    fprintf(fp, "  x: [%s],\n", getTimestamps(&g_perf_stats.delivery_ts));
    fprintf(fp, "  y: [%s],\n", getValues(&g_perf_stats.delivery_ts));
    fprintf(fp, "  type: 'bar', name: 'Livraisons'\n};\n");

    fprintf(fp, "Plotly.newPlot('capacity', [capacityData], {title: 'Utilisation de la capacité'});\n");
    fprintf(fp, "Plotly.newPlot('deliveries', [deliveryData], {title: 'Livraisons par heure'});\n");
    fprintf(fp, "</script>\n</body></html>");
    fclose(fp);
}

// ====================
// Tests unitaires
// ====================
static void test_full_delivery_cycle(void** state) {
    Graph* g = generateSmallNetwork();
    Package pkg = {.id=1, .volume=5, .delivered=false};
    Vehicle vehicle = {.id=0, .max_capacity=10, .current_load=0};
    
    dailyPlanning(g, &pkg, 1, &vehicle, 1);
    
    assert_true(pkg.delivered);
    assert_true(vehicle.current_load == 5);
    
    freeGraph(g);
}

static void test_metrics_system(void** state) {
    initMetrics();
    logMetric(&g_perf_stats.capacity_ts, 75.0, CAPACITY_METRIC);
    
    assert_non_null(g_perf_stats.capacity_ts.points);
    assert_int_equal(g_perf_stats.capacity_ts.count, 1);
    assert_true(g_perf_stats.capacity_ts.points[0].metric_value == 75.0);
    
    free(g_perf_stats.capacity_ts.points);
}

// Ajouter cette constante
#define LANG_FR 0
#define LANG_EN 1

void log_message(int lang, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    if(lang == LANG_FR) {
        char* translations[] = {
            "[Livraison] Véhicule %d a livré colis %d\n",
            "[Erreur] Capacité dépassée véhicule %d\n"
        };
        vprintf(translations[msg_id], args);
    }
    // ... gestion d'autres langues
    va_end(args);
}

typedef struct {
    float cost;
    float time;
    float reliability;
} ParetoSolution;

ParetoSolution* findParetoFront(Graph* g, int origin, int dest, int* front_size) {
    int capacity = 10;
    ParetoSolution* front = malloc(capacity * sizeof(ParetoSolution));
    *front_size = 0;
    
    // Génération de solutions candidates
    float** dist = malloc(g->V * sizeof(float*));
    for(int i=0; i<g->V; i++) dist[i] = malloc(g->V * sizeof(float));
    floydWarshall(g, dist);
    
    // Exploration des routes
    for(int k=0; k<g->V; k++) {
        if(dist[origin][k] + dist[k][dest] < INF) {
            ParetoSolution sol = {
                .cost = dist[origin][k] + dist[k][dest],
                .time = (dist[origin][k] + dist[k][dest]) * 0.8,
                .reliability = 0.95
            };
            
            // Vérification dominance Pareto
            bool dominated = false;
            for(int i=0; i<*front_size; i++) {
                if(front[i].cost <= sol.cost && 
                   front[i].time <= sol.time && 
                   front[i].reliability >= sol.reliability) {
                    dominated = true;
                    break;
                }
            }
            
            if(!dominated) {
                if(*front_size >= capacity) {
                    capacity *= 2;
                    front = realloc(front, capacity * sizeof(ParetoSolution));
                }
                front[(*front_size)++] = sol;
            }
        }
    }
    
    // Nettoyage mémoire
    for(int i=0; i<g->V; i++) free(dist[i]);
    free(dist);
    
    return front;
}

int main() {
    // Initialisation
    Graph* g = createGraph(100);
    initWebsocket(9000);
    
    // Planification
    ParetoSolution* pareto = findParetoFront(g, 0, 99, &front_size);
    Chromosome* solution = geneticAlgorithm(g, 0, 99, 100, 50);
    
    // Monitoring temps-réel
    while(1) {
        updateMetrics(g);
        broadcastMetrics();
        sleep(1);
    }
    
    return 0;
}