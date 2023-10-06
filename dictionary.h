#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <stdbool.h>



typedef struct HashNode {
    char* key;
    char* value;
    struct HashNode* next;
} HashNode;

typedef struct HashMap {
    int size;
    HashNode** nodes;
} HashMap;

HashMap* create(int size);
void destroy(HashMap* map);
void set(HashMap* map, char* key, char* value);
char* get(HashMap* map, char* key);
bool has(HashMap* map, char* key);
void save(const HashMap* map, const char* name);
HashMap* load(const char* name, int size);


#endif