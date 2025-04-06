#ifndef PROJECT_H
#define PROJECT_H

#include <stdbool.h>

// ----- Structures de données -----
// Attributs d'une arête
typedef struct {
    double distance;
    double baseTime;
    double cost;
    int roadType;
    double reliability;
    int restrictions;
    // Variation temporelle (multiplicateur)
    double timeFactor[3]; // 0=matin,1=après-midi,2=nuit
} EdgeAttr;

// Nœud d'adjacence
typedef struct AdjListNode {
    int dest;
    EdgeAttr attr;
    struct AdjListNode* next;
} AdjListNode;

// Liste d'adjacence
typedef struct {
    AdjListNode* head;
} AdjList;

// Graphe
typedef struct {
    int V;
    AdjList* array;
} Graph;

// ----- Prototypes -----
// Création et manipulation du graphe
Graph* createGraph(int V);
void addEdge(Graph* g, int src, int dest, EdgeAttr attr);
void removeEdge(Graph* g, int src, int dest);
void freeGraph(Graph* g);
void printGraph(Graph* g);

// Chargement / sauvegarde
Graph* loadGraphJSON(const char* filename);
void saveGraphJSON(Graph* g, const char* filename);

// Parcours
void DFS(Graph* g, int start, bool* visited);
void BFS(Graph* g, int start, bool* visited);
bool detectCycle(Graph* g);
int connectedComponents(Graph* g, int* comp);
bool isAccessible(Graph* g, int src, int dest);

// Optimisation
void floydWarshall(Graph* g, double** dist);
bool bellmanFord(Graph* g, int src, double* dist);
int* tspNearestNeighbor(Graph* g, int start);

// Glouton
void greedyAssignment(/* paramètres */);

// Génétique
void geneticAlgorithm(/* paramètres */);

// Utilitaires
void exampleUsage();

#endif // PROJECT_H