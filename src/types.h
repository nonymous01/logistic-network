#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

// Types de nœuds
typedef enum {
    NODE_DEPOT,
    NODE_HUB,
    NODE_DELIVERY_POINT
} NodeType;

// Types de véhicules
typedef enum {
    VEHICLE_TRUCK,
    VEHICLE_VAN,
    VEHICLE_BIKE
} VehicleType;

// État des colis
typedef enum {
    PACKAGE_PENDING,
    PACKAGE_IN_TRANSIT,
    PACKAGE_DELIVERED,
    PACKAGE_FAILED
} PackageStatus;

// Structure pour un nœud
typedef struct {
    int id;
    char name[50];
    NodeType type;
    double latitude;
    double longitude;
    int capacity;
} Node;

// Structure pour un véhicule
typedef struct {
    int id;
    char registration[20];
    VehicleType type;
    int capacity;
    int current_load;
    int current_node_id;
    bool is_available;
} Vehicle;

// Structure pour un colis
typedef struct {
    int id;
    char tracking_number[20];
    int source_node_id;
    int destination_node_id;
    int weight;
    PackageStatus status;
    int assigned_vehicle_id;
} Package;

// Structure pour les statistiques
typedef struct {
    int total_packages;
    int delivered_packages;
    int active_vehicles;
    double average_delivery_time;
    double network_efficiency;
} Statistics;

// Structure pour l'état du réseau
typedef struct {
    Node* nodes;
    int node_count;
    Vehicle* vehicles;
    int vehicle_count;
    Package* packages;
    int package_count;
    Statistics statistics;
} NetworkState;

#endif // TYPES_H 