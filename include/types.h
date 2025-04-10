#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

// Constantes
#define MAX_VERTICES 100
#define MAX_EDGES 1000
#define MAX_VEHICLES 50
#define MAX_PACKAGES 200
#define INF 1e9
#define POPULATION_SIZE 100
#define GENERATIONS 50
#define ELITE_SIZE 10
#define MUTATION_RATE 0.1

// Constants for traffic levels
#define TRAFFIC_LIGHT 0
#define TRAFFIC_MODERATE 1
#define TRAFFIC_HEAVY 2

// Constants for driver breaks
#define MAX_DRIVING_TIME 4.5f  // Maximum driving time in hours
#define BREAK_DURATION 0.75f   // Break duration in hours

// Types de nœuds
typedef enum {
    NODE_DISTRIBUTION_CENTER,
    NODE_RELAY_POINT,
    NODE_REST_AREA,
    NODE_DELIVERY_POINT,
    NODE_CHARGING_STATION
} NodeType;

// États des routes
typedef enum {
    ROAD_EXCELLENT,
    ROAD_GOOD,
    ROAD_FAIR,
    ROAD_POOR,
    ROAD_IMPASSABLE
} RoadCondition;

// Types de véhicules
typedef enum {
    VEHICLE_SMALL,
    VEHICLE_MEDIUM,
    VEHICLE_LARGE
} VehicleType;

// Attributs des arêtes
typedef struct {
    float distance;
    float speed_limit;
    RoadCondition condition;
    int traffic_level;
    int restrictions;
    float cost;
} EdgeAttr;

// Attributs des nœuds
typedef struct {
    NodeType type;
    int capacity;
    bool has_charging_station;
    int service_time;
    char* name;
} NodeAttr;

// Nœud d'adjacence
typedef struct AdjNode {
    int dest;
    EdgeAttr attr;
    struct AdjNode* next;
} AdjNode;

// Liste d'adjacence
typedef struct {
    AdjNode* head;
    NodeAttr node_attr;
} AdjList;

// Graphe
typedef struct {
    int V;
    AdjList* array;
} Graph;

// Structure pour les véhicules
typedef struct {
    int id;
    char type[50];
    char status[50];
    char current_location[100];
    double current_battery;
    int current_node;
    int* route;
    int route_length;
    int* packages;
    int package_count;
} Vehicle;

// Structure pour les colis
typedef struct {
    int id;
    char status[50];
    int priority;
    char source[100];
    char destination[100];
    double weight;
    int source_node;
    int destination_node;
    int assigned_vehicle;
} Package;

// Structure pour les nœuds du réseau
typedef struct {
    int id;
    char name[100];
    char type[50];
    double capacity;
    double current_load;
    double demand;
    double current_demand;
} NetworkNode;

// Structure pour les arêtes du réseau
typedef struct {
    int source;
    int target;
    double distance;
    double time;
    double cost;
} NetworkEdge;

// Structure pour les statistiques
typedef struct {
    int delivered_packages;
    int active_vehicles;
    double average_delivery_time;
    double network_efficiency;
} NetworkStats;

// Structure principale pour l'état du réseau
typedef struct {
    Vehicle* vehicles;
    int vehicle_count;
    Package* packages;
    int package_count;
    NetworkNode* nodes;
    int node_count;
    NetworkEdge* edges;
    int edge_count;
    NetworkStats stats;
} NetworkState;

// Structure pour les routes
typedef struct {
    int* nodes;
    int num_nodes;
    float total_time;
    float total_cost;
} Route;

// Structure pour les chargements
typedef struct {
    int* package_indices;  // Indices des colis dans le chargement
    int num_packages;      // Nombre de colis
    float total_weight;    // Poids total
    float total_volume;    // Volume total
    float total_value;     // Valeur totale (priorité)
} Loading;

#endif /* TYPES_H */ 