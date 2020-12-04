/*
 * Cache L1 simulator
 * Author: Bryan Erazo
 * Interface: ./first <cache size><associativity><cache policy><block size><trace file>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "first.h"
#define ARR_MAX 100

void insertNodeInCache( struct Cache **cache, u_int64_t key);
int searchInCache(struct Cache **cache, u_int64_t key );
void fifo( struct Cache **cache, u_int64_t key );
void lru( struct Cache **cache, u_int64_t key );


int main( int argc, char *argv[argc+1]) {

    long cache_size;
    long block_size;
    char trace_file[ARR_MAX];

    //File name from arguments
    if (argc != 6 ){
        printf("DEV Error 1: Give 5 arg as int: cache_size, str: associativity, str: cache_policy, int: block_size, str: trace_file\n");
        printf("error");
        return EXIT_SUCCESS;
    }
    // Check for power of 2 for cache_size and block_size
    cache_size = getCacheSize(argv[1]);
    block_size = getBlockSize(argv[4]);
    if (cache_size == 0 || block_size == 0){
        printf("DEV Error 2: cache_size and block_size must be power of 2 and > 0\n");
        printf("error");
        return EXIT_SUCCESS;
    }
    long associativity;
    unsigned int associativityAction = checkAssociativityInput(argv[2]);

    // action 1,2,3 -> direct, fully, n associativity and 0 is error
    if (associativityAction == 1){
        associativity = 1;
    } else if (associativityAction == 2){
        associativity = calculateNumberCacheAddresses(cache_size, block_size);
    } else if (associativityAction == 3){
        associativity = getAssociativity(argv[2]);
    } else{
        printf("DEV ERROR: associativityAction input is incorrect\n");
        return 0;
    }
    int cache_policy = getCachePolicy(argv[3]);

    printf("cache_size: %lu\n",cache_size);
    printf("block_size: %lu\n",block_size);
    printf("associativityAction: %d\n",associativityAction);
    printf("associativity: %lu\n",associativity);
    printf("cache_policy: %d\n",cache_policy);

    // Declare the read file and read it
    FILE *fp;
    fp = fopen( argv[5], "r");
    // Check if the file is empty
    if ( fp == NULL ){
        printf("DEV Error 3:Unable to read the file\n");
        printf("error\n");
        return EXIT_SUCCESS;
    }

    // File data
    char action;
    unsigned long long memory_address = 0x0;
    struct Cache *cache=NULL;
    struct CacheStats *cacheStats=NULL;
    cache = createCache(cache, cache_size, block_size,associativityAction, associativity, &cacheStats);

    //printf("sizeof(associativity[]:%lu\n",sizeof(associativity));
    while ( fscanf( fp, "%c %llx",&action, &memory_address) != EOF ){
        //printf("action: %c memory_address: %x",action, memory_address );
        if ( getReadWriteAction(action) == 0 ){
            continue;
        }
        //fifo(&cache,memory_address);
        lru(&cache,memory_address);
//        if (getReadWriteAction(action) == 1){
//            int x = searchInCache(&cache, memory_address);
//            if (x){
//                cacheStats->memory_read++;
//            }
//        } else if (getReadWriteAction(action) == 2){
//            insertNodeInCache(&cache, memory_address);
//        } else{
//            printf("error in while c x action memory_address\n");
//        }
    }
    // Close the file and destroy memory allocations
    fclose(fp);

    printCache(cache,1);
    printCacheStats(cacheStats);
    free(cacheStats);

    //printCache(cache,0);
    //printList(linked_list);
    //deleteLinkedList(&linked_list);
    //printList(linked_list);


    return EXIT_SUCCESS;
}
// Eviction policy lru
void lru( struct Cache **cache, u_int64_t key ){
    struct Cache *pCache = (*cache);
    assert(pCache != NULL );
    int NodeFound = searchInCache( &pCache, key);

    if (NodeFound){
        // 1. The pCache is not empty, at least have one Node
        // 2.
        // Remove and insert in the head of the linked list in the cache
        // Remove node at position
        size_t len = pCache[0].len;
        size_t index = key % len;
        struct Node *linked_list = pCache[index].linked_list;

        // If one Node node
        if (pCache[index].max_nodes_allow == 1){
            pCache[index].linked_list->address=key;
        } else{
            // More than one node
            if (pCache[index].linked_list->address == key){
                // if in the key in head nothing to insert
                return;
            } else{
                removeNodeAtAddress(&pCache[index].linked_list, key);
                pCache[index].number_nodes_in_linked_list--;
                insertNodeInTheBeginning(&pCache[index].linked_list, key);
                pCache[index].number_nodes_in_linked_list++;
                // Remove last node if greater than the max_nodes_allow
                if (pCache[index].number_nodes_in_linked_list > pCache[index].max_nodes_allow){
                    // Remove the last Node
                    pCache[index].linked_list = removeLastNode(pCache[index].linked_list);
                }
            }
        }
        //removeNodeAtAddress(&pCache[index].linked_list, key);


    } else{
        // 1. The cache linked list could be empty
        // Remove last node and insert new Node
        size_t len = pCache[0].len;
        size_t index = key % len;
        struct Node *linked_list = pCache[index].linked_list;
        if (linked_list == NULL){
            insertNodeInTheBeginning(&pCache[index].linked_list, key);
            pCache[index].number_nodes_in_linked_list++;
        } else{
            //printf("DEV: not empty linkedlist\n");
            // Linked List contain nodes
            insertNodeInTheBeginning(&pCache[index].linked_list, key);
            pCache[index].number_nodes_in_linked_list++;
            if (pCache[index].number_nodes_in_linked_list > pCache[index].max_nodes_allow){
                pCache[index].linked_list = removeLastNode(pCache[index].linked_list);
                pCache[index].number_nodes_in_linked_list--;
            }

        }
    }
}
// Eviction policy fifo
void fifo(struct Cache **cache, u_int64_t key ){

    struct Cache *pCache = (*cache);
    assert(pCache != NULL );
    int NodeFound = searchInCache( &pCache, key);
    if ( NodeFound ){
        printf("DEV ERROR 7: key is the list already\n");
        return;
    } else{
        size_t len = pCache[0].len;
        size_t index = key % len;
        struct Node *linked_list = pCache[index].linked_list;
        // List is empty
        if (linked_list == NULL){
            insertNodeInTheBeginning(&pCache[index].linked_list, key);
            pCache[index].number_nodes_in_linked_list++;
        } else{
            // list have one node
            // More Than One Nodes
            if (pCache[index].number_nodes_in_linked_list < pCache[index].max_nodes_allow){
                insertNodeInTheBeginning(&pCache[index].linked_list, key);
                pCache[index].number_nodes_in_linked_list++;
            } else{
                insertNodeInTheBeginning(&pCache[index].linked_list, key);
                // Remove the last Node
                pCache[index].linked_list = removeLastNode(pCache[index].linked_list);
                if (pCache[index].number_nodes_in_linked_list == pCache[index].max_nodes_allow){
                    return;
                } else{
                    pCache[index].number_nodes_in_linked_list++;
                }

            }

        }
    }

}

// Search if Node exits in Cache linked-list
int searchInCache(struct Cache **cache, u_int64_t key ){
    struct Cache *ptr = (*cache);

    if ( ptr == NULL ){
        return 0;
    }
    //pCacheStats->memory_read++;
    size_t len = ptr[0].len;
    size_t index = key % len;
    struct Node *plinkedList = ptr[index].linked_list;
    if (plinkedList == NULL){
        return 0;
    }
    while (plinkedList != NULL){
        if (plinkedList->address == key){
            return 1;
        }
        plinkedList = plinkedList->next;
    }
    return 0;
}

// Insert in the beginning of the linked list in the cache
void insertNodeInCache(struct Cache **cache, u_int64_t key){
    struct Cache *ptr = (*cache);
    if ( ptr == NULL ){
        return;
    }
    size_t len = ptr[0].len;
    size_t index = key % len;

    insertNodeInTheBeginning(&ptr[index].linked_list, key);
    ptr[index].number_nodes_in_linked_list++;

}
// Create an empty cache with given capacity or lines
struct Cache *createCache(struct Cache *cache, size_t cache_size, size_t block_size,unsigned int assocAction, size_t assoc, struct CacheStats **cacheStats){

    assert(cache == NULL);
    struct Cache *new_cache = NULL;
    if ( assocAction == 1){
        size_t number_addresses = calculateNumberCacheAddresses(cache_size, block_size);
        if (number_addresses == 0 ){
            return NULL;
        }
        // Initialize cache in an array
        new_cache = malloc(number_addresses*sizeof(struct Cache));
        for (size_t i = 0; i < number_addresses; ++i) {
            new_cache[i].len = number_addresses;
            new_cache[i].max_nodes_allow = assoc;
            new_cache[i].number_nodes_in_linked_list = 0;
            new_cache[i].linked_list = NULL;
        }
    } else if ( assocAction == 2 ){
        size_t number_addresses = calculateNumberCacheAddresses(cache_size, block_size);
        if (number_addresses == 0 ){
            return NULL;
        }
        new_cache = malloc(sizeof(struct Cache));
        new_cache[0].len = 1;
        new_cache[0].max_nodes_allow = number_addresses;
        new_cache[0].number_nodes_in_linked_list = 0;
        new_cache[0].linked_list = NULL;
    } else if (assocAction == 3){
        size_t number_addresses = calculateNumberCacheAddresses(cache_size, block_size)/assoc;
        if ( number_addresses == 0 ){
            number_addresses = calculateNumberCacheAddresses(cache_size, block_size);
            if ( number_addresses == 0 ){
                return NULL;
            }
            new_cache = malloc(sizeof(struct Cache));
            new_cache[0].len = 1;
            new_cache[0].max_nodes_allow = number_addresses;
            new_cache[0].number_nodes_in_linked_list = 0;
            new_cache[0].linked_list = NULL;
        } else{
            // Initialize cache in an array
            new_cache = malloc(number_addresses*sizeof(struct Cache));
            for (size_t i = 0; i < number_addresses; ++i) {
                new_cache[i].len = number_addresses;
                new_cache[i].max_nodes_allow = assoc;
                new_cache[i].number_nodes_in_linked_list = 0;
                new_cache[i].linked_list = NULL;
            }
        }

    } else{
        return NULL;
    }
    struct CacheStats *new_cacheStats = malloc(sizeof(struct CacheStats));
    new_cacheStats->cache_hit=0;
    new_cacheStats->cache_miss=0;
    new_cacheStats->memory_write=0;
    new_cacheStats->memory_read=0;
    (*cacheStats) = new_cacheStats;

    return new_cache;
}
bool IsPowerOfTwo(unsigned long x){
    return (x != 0) && ((x & (x - 1)) == 0);
}

bool isEven(long int n){
    // n^1 is n+1, then even, else odd
    assert(n >= 0);
    if ( (n ^ 1) == n + 1 ){
        return true;
    } else{
        return false;
    }
}

// Functions
long getCacheSize(char *arg){

    assert(arg != NULL);

    char *ptr;
    long cache_size;
    cache_size = strtol(arg, &ptr,10);
    if ( IsPowerOfTwo(cache_size) ){
        return cache_size;
    }
    return 0;
}

long getBlockSize(char *arg){

    assert(arg != NULL);

    char *ptr;
    long block_size;
    block_size = strtol(arg, &ptr,10);
    if ( IsPowerOfTwo(block_size) ){
        return block_size;
    }
    return 0;
}
unsigned int checkAssociativityInput(char *arg){
    /* Doc Function use
     * 0 is an error
     * 1 is a direct
     * 2 is full associativity
     * 3 is n associativity: the argument is in the correct input and contains the n value;
    */
    char *direct = "direct";
    char *assoc = "assoc";
    char *assoc_n = "assoc:";
    if (arg == NULL){
        return 0;
    }
    char str[ARR_MAX];
    str[0]='\0';
    strcpy(str,arg);

    if ( strcmp(str,direct) == 0 ){
        return 1;
    } else if ( strcmp(str, assoc) == 0 ){
        return 2;
    } else{
        if (strcmp(str, assoc_n) <= 0 ){
            return 0;
        }
        unsigned int len = strlen(assoc_n);
        char tmp[ARR_MAX];
        tmp[0]='\0';
        for (int i = 0; i < len; ++i) {
            if (str[i] != assoc_n[i]){
                return 0;
            }
        }
        unsigned int len_arg = strlen(arg);
        int k = 0;
        for (unsigned int i = len; i < len_arg; ++i) {
            tmp[k]=str[i];
            k++;
        }
        tmp[len_arg-len]='\0';
        char *ptr;
        long num;
        num = strtol(tmp, &ptr,10);
        if (num == 1){
            return 1;
        }
        else if (num > 0){
            return 3;
        } else{
            return 0;
        }
    }
}

// Get the direct, assoc, and assoc:n
unsigned int getAssociativity(char *arg){
    assert(arg != NULL);
    char str[ARR_MAX];
    str[0]='\0';
    strcpy(str,arg);

    unsigned int assoc_action = checkAssociativityInput(str);
    assert( assoc_action >= 0 && assoc_action <= 3);

    if ( assoc_action == 0 ){
        printf("DEV ERROR 5: Action 0 is invalid and must print error in production\n");
        return 0;
    }
    // This is fully direct cache
    if ( assoc_action == 1){
        return 1;
    }
    // This is fully associate cache
    if ( assoc_action == 2){
        return 2;
    } else{
        // This is n associate cache
        long num = getNumberFromAssoc(arg);
        printf("DEV num:%lu\n",num);
        return num;
    }
}

long getNumberFromAssoc(char *arg){
    char *assoc_n = "assoc:";
    unsigned int len = strlen(assoc_n);
    char tmp[ARR_MAX];
    tmp[0]='\0';
    unsigned int len_arg = strlen(arg);
    int k = 0;
    for (unsigned int i = len; i < len_arg; ++i) {
        tmp[k]=arg[i];
        k++;
    }
    tmp[len_arg-len]='\0';
    char *ptr;
    long num;
    num = strtol(tmp, &ptr,10);
    return num;
}

int getCachePolicy(char *arg){
    /* Function eviction policy
     * 0 is an error
     * 1 is FIFO -> First In First Out
     * 2 is LRU -> least Recently Used
     */
    if (arg == NULL){
        return 0;
    }
    char str[ARR_MAX];
    str[0]='\0';
    char *fifo = "fifo";
    char *lru = "lru";

    // Accept the input in any form if the same word
    strcpy(str,arg);
    for(int i = 0; str[i]; i++){
        str[i] = (char )tolower(str[i]);
    }

    if ( strcmp(str, fifo) == 0 ){
        return 1;
    } else if (strcmp(str, lru) == 0 ){
        return 2;
    } else {
        return 0;
    }
}
size_t calculateNumberCacheAddresses(size_t cache_size, size_t cache_block ){
    return cache_size/cache_block;
}
int getReadWriteAction(char action){
    /* DOC
     * 0 for errors
     * 1 for READ
     * 2 for WRITE
     */
    if (action == '\0'){
        return 0;
    }

    char r='r';
    char w='w';
    char str = (char)tolower(action);
    if ( str == r){
        return 1;
    } else if ( str == w ){
        return 2;
    } else{
        return 0;
    }
}