#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



typedef struct HashNode
{
    char *key;
    char *value;
    struct HashNode *next;
} HashNode;

typedef struct
{
    HashNode **nodes;
    int size;
} HashMap;

unsigned long hash(char *key) //djb2 algorithm
{
unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
}

HashMap *create(int size)
{
    HashMap *map = malloc(sizeof(HashMap));
    map->nodes = calloc(size, sizeof(HashNode *));
    map->size = size;
    return map;
}

void destroy(HashMap* map) {
    for (int i = 0; i < map->size; i++) {
        HashNode* node = map->nodes[i];
        while (node != NULL) {
            HashNode* next = node->next;
            free(node->key);
            free(node->value);
            free(node);
            node = next;
        }
    }
    free(map->nodes);
    free(map);
}

void set(HashMap *map, char *key, char *value)
{

    int index = hash(key);
    
    HashNode *node = map->nodes[index];
    while (node != NULL)
    {
  
        if (strcmp(node->key, key) == 0)
        {
            if(node->value==NULL){
                node->value=value;
                return;
            }
            char *temp = malloc(strlen(node->value) + strlen(value) + 2);
            // Key already exists, concatenate the new value to the existing value
            strcpy(temp, node->value);
            strcat(temp, value);
            node->value = temp;
            return;
        }
        node = node->next;
    }
    // Key does not exist, create a new node with the new value
    HashNode *new_node = malloc(sizeof(HashNode));
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    new_node->next = map->nodes[index];
    map->nodes[index] = new_node;
}

char *get(HashMap *map, char *key)
{
    int index = hash(key);
    HashNode *node = map->nodes[index];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
        {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}
bool has(HashMap *map, char *key)
{
    int index = hash(key);
    HashNode *node = map->nodes[index];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
        {
            return true;
        }
        node = node->next;
    }
    return false;
}


void save(const HashMap* map, const char* filename) {

    char *end = strstr(filename, ".txt");
    if(end!=NULL) strcpy(end, ".dict"); 

    FILE* file = fopen(filename, "w");
    for (int i = 0; i < map->size; i++) {
        HashNode* node = map->nodes[i];
        while (node != NULL) {
            fprintf(file, "\n%s=", node->key);
            char* token = strtok(node->value, "\n");
            while (token != NULL) {
                fprintf(file, "%s,", token);
                token = strtok(NULL, "\n");
            }
            node = node->next;
        }
    }

    fclose(file);
}
HashMap* load(const char* filename, int size){

    char line[200];
    char* key;
    char* temp;
    char* value;
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Dictionary not found\n");
        return NULL;
    }
    
    HashMap* map = create(size);
    
while (fgets(line, sizeof(line), file) != NULL) {
    if(line[0] =='\n' || line[0] == '=')continue; //line starts with "=" is some rare bug cases?
    
    key = strtok(line, "=");

    value = strtok(NULL, "=");
    temp = strchr(value,',');
    while(temp){
        *temp='\n';
        temp=strchr(temp, ',');
    }
    set(map, key,value);
    
}
    return map;

    
}



/*int main()  // test usage
{
    HashMap *map = createHashMap();
    set(map, "key1", "value1");
    set(map, "key3", NULL);
    printf("%d\n", has(map, "key3"));
    set(map, "key3", "great");
    printf("%s\n", get(map, "key3"));

    set(map, "key3", "kused");

   
    printf("%d\n", has(map, "key1"));
    printf("%d\n", has(map, "key2"));
    printf("%d\n", has(map, "key3"));



    printf("%s\n", get(map, "key1"));
    printf("%x\n", get(map, "key2"));
    printf("%s\n", get(map, "key3"));
    destroyHashMap(map);

    return 0;
}
*/