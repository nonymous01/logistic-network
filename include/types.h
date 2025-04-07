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

// Structure for a vehicle
typedef struct {
    int id;
    VehicleType type;
    float capacity;           // Maximum weight capacity
    float volume_capacity;    // Maximum volume capacity
    float speed;             // Average speed in km/h
    float cost_per_km;       // Cost per kilometer
    float battery_capacity;  // Battery capacity in kWh
    float current_battery;   // Current battery level
    float consumption_rate;  // Energy consumption rate in kWh/km
    float charging_rate;     // Charging rate in kW
    int current_location;    // Current node location
    float driving_time;      // Current driving time in hours
    float last_break_time;   // Time of last break in hours
    float max_driving_time;  // Maximum allowed driving time in hours
} Vehicle;

// Structure for a loading
typedef struct {
    int* package_indices;  // Indices of packages in the loading
    int num_packages;      // Number of packages
    float total_weight;    // Total weight
    float total_volume;    // Total volume
    float total_value;     // Total value (priority)
} Loading;

// Colis
typedef struct {
    int id;
    int source;
    int destination;
    float weight;
    float volume;
    float priority;
    bool is_assigned;
    bool is_delivered;
    int assigned_vehicle;
    float delivery_window_start;
    float delivery_window_end;
} Package;

// Route
typedef struct {
    int* nodes;
    int num_nodes;
    float total_time;
    float total_cost;
} Route;

// Solution pour l'algorithme génétique
typedef struct {
    int* assignments;
    int num_packages;
    int num_vehicles;
    float fitness;
} Solution;

// Population pour l'algorithme génétique
typedef struct {
    Solution* individuals;
    int size;
} Population;

#endif /* TYPES_H */ 