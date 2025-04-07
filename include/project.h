#ifndef PROJECT_H
#define PROJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include "types.h"

// Prototypes des fonctions de gestion du graphe
Graph* createGraph(int V);
AdjNode* newAdjNode(int dest, EdgeAttr attr);
void addEdge(Graph* g, int src, int dest, EdgeAttr attr);
void removeEdge(Graph* g, int src, int dest);
void printGraph(Graph* g);
void freeGraph(Graph* g);

// Prototypes des fonctions de parcours et analyse du graphe
void DFS_util(Graph* g, int v, bool* visited);
void DFS(Graph* g, int start, bool* visited);
void BFS(Graph* g, int start);
bool detectCycleUtil(Graph* g, int v, bool* visited, bool* recStack);
bool detectCycle(Graph* g);

// Prototypes des fonctions d'optimisation
void greedyAssignment(Graph* g, Package* packages, int num_packages, Vehicle* vehicles, int num_vehicles);
void dailyPlanning(Graph* g, Vehicle* vehicles, int num_vehicles, Package* packages, int num_packages);
void multiDayPlanning(Graph* g, Vehicle* vehicles, int num_vehicles, Package* packages, int num_packages);
void dynamicReallocation(Graph* g, Vehicle* vehicles, int num_vehicles, Package* packages, int num_packages, int failed_node);
void geneticAlgorithm(Graph* g, Package* packages, int num_packages, Vehicle* vehicles, int num_vehicles);

// Prototypes des fonctions de test
Graph* createSampleNetwork(void);
Vehicle* createSampleVehicles(int* num_vehicles);
Package* createSamplePackages(int* num_packages);

// Energy management functions
float calculateEnergyConsumption(Graph* g, int src, int dest, Vehicle* vehicle);
float calculateChargingTime(Vehicle* vehicle);
void chargeVehicle(Vehicle* vehicle, float charging_time);
void planChargingStops(Route* route, Vehicle* vehicle, Graph* g);

// Break management functions
bool needsBreak(Vehicle* vehicle, float driving_time);
void planBreaks(Route* route, Vehicle* vehicle, Graph* g);
void updateDrivingTime(Vehicle* vehicle, float time);

// Loading management functions
Loading createEmptyLoading();
void addPackageToLoading(Loading* loading, int package_index, Package* package);
void freeLoading(Loading* loading);
void updateAssignmentsFromLoading(Loading* loading, Package* packages, int vehicle_id);

// Route management functions
int findNearestChargingStation(Graph* g, int current_node);
int findNearestRestArea(Graph* g, int current_node);
void insertNodeInRoute(Route* route, int position, int node);

#endif // PROJECT_H 