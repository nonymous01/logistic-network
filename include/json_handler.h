#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include "types.h"

// Fonctions de gestion JSON
NetworkState* readNetworkState(const char* filename);
void writeNetworkState(const char* filename, const NetworkState* state);
void freeNetworkState(NetworkState* state);

#endif // JSON_HANDLER_H 