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
    long associativity = getAssociativity(argv[2]);
    int cache_policy = getCachePolicy(argv[3]);

    printf("cache_size: %lu\n",cache_size);
    printf("block_size: %lu\n",block_size);
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
    char action = '\0';
    int memory_address = 0x012;
    //printf("sizeof(associativity[]:%lu\n",sizeof(associativity));
    while ( fscanf( fp, "%c %x",&action, &memory_address) != EOF ){
        printf("action: %c memory_address: %x\n",action, memory_address );
    }
    // Close the file and destroy memory allocations
    fclose(fp);

    struct Node *linked_list=NULL;
    linked_list = createCacheLinkedList(linked_list,calculateNumberCacheAddresses(cache_size,block_size));
    printList(linked_list);
    deleteLinkedList(&linked_list);
    printList(linked_list);
    linked_list = createCacheLinkedList(linked_list,5);
    printList(linked_list);
    deleteLinkedList(&linked_list);
    printList(linked_list);
    linked_list = createCacheLinkedList(linked_list,15);
    printList(linked_list);
    deleteLinkedList(&linked_list);
    printList(linked_list);


    return EXIT_SUCCESS;
}
// Create an empty cache with given capacity or lines
struct Node *createCacheLinkedList(struct Node *head, unsigned int capacity){
    struct Node *ptr = head;
    if (ptr == NULL){
        // Initialize empty cache
        for (int i = 0; i < capacity; ++i) {
            insertNodeInTheBeginning(&ptr,0,true);
        }
        return ptr;
    } else{
        printf("DEV 4:Linked List is not empty\n");
        return NULL;
    }
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