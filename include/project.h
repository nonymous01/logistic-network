#ifndef PROJECT_H  // Si le symbole PROJECT_H n'est pas défini, inclure ce fichier. Cela évite que ce fichier soit inclus plusieurs fois.
#define PROJECT_H  // Définit le symbole PROJECT_H pour signaler que ce fichier a été inclus.

#include <stdbool.h>  // Inclut la bibliothèque pour utiliser les types booléens (true, false).

// Définition de la structure AdjListNode, représentant un noeud d'une liste d'adjacence
// Chaque noeud contient un sommet de destination, un poids (poids de l'arête) et un pointeur vers le noeud suivant.
typedef struct AdjListNode {
    int dest;               // Sommet de destination de l'arête.
    float weight;           // Poids de l'arête (par exemple, la distance entre les sommets).
    struct AdjListNode* next;  // Pointeur vers le prochain noeud de la liste d'adjacence.
} AdjListNode;

// Définition de la structure AdjList, représentant une liste d'adjacence pour un sommet.
typedef struct AdjList {
    AdjListNode* head;   // Pointeur vers le premier noeud de la liste d'adjacence pour ce sommet.
} AdjList;

// Définition de la structure Graph, représentant un graphe.
typedef struct Graph {
    int V;              // Nombre de sommets dans le graphe.
    AdjList* array;     // Tableau de listes d'adjacence, une pour chaque sommet.
} Graph;

// Déclaration des fonctions du graphe, que l'on va définir dans le fichier source (.c).

// Crée un graphe avec un nombre de sommets V.
Graph* createGraph(int V);

// Ajoute une arête entre deux sommets (src, dest) avec un poids spécifié.
void addEdge(Graph* graph, int src, int dest, float weight);

// Détecte s'il existe un cycle dans le graphe.
bool detectCycle(Graph* g);

// Affiche les arêtes et les poids de chaque sommet du graphe.
void printGraph(Graph* graph);

// Calcule les plus courts chemins entre tous les sommets à l'aide de l'algorithme de Floyd-Warshall.
void floydWarshall(Graph* g, float** dist);

// Affiche une solution pour le problème du voyageur de commerce (TSP).
void tsp(Graph* g);

// Génère un fichier DOT qui peut être utilisé pour générer un graphique avec Graphviz.
void generateDot(Graph* g, const char* filename);

// Libère la mémoire allouée pour le graphe et ses éléments.
void freeGraph(Graph* g);

#endif  // Fin de la directive #ifndef pour éviter les inclusions multiples.
