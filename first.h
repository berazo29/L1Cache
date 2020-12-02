//
// Created by Bryan Erazo on 12/2/20.
//

#ifndef L1CACHE_FIRST_H
#define L1CACHE_FIRST_H

// DATA STRUCTURE
struct Cache{
    size_t capacity;
    size_t memory_read;
    size_t memory_write;
    size_t cache_hit;
    size_t cache_miss;
};
struct Node {
    long address;
    bool free;
    struct Node *next;
};

// Data-Structure Nodes Functions
void insertNodeInTheBeginning(struct Node** head, int new_data, bool free);
void deleteLinkedList(struct Node** head);


// Functions
long getCacheSize(char *arg);
long getBlockSize(char *arg);
int getCachePolicy(char *arg);
unsigned int getAssociativity(char *arg);
int getReadWriteAction(char action);
int calculateNumberCacheAddresses(int cache_size, int cache_block );
struct Node *createCacheLinkedList(struct Node *head, unsigned int capacity);

// Utility functions
bool IsPowerOfTwo(unsigned long x);
bool isEven(long int n);
unsigned int checkAssociativityInput(char *arg);
long getNumberFromAssoc(char *arg);

// Data structure functions
void insertNodeInTheBeginning(struct Node** head, int new_data, bool free){

    // Allocate new node and insert O(1)
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->address = new_data;
    new_node->free = free;
    new_node->next = (*head);
    (*head) = new_node;

}
void deleteLinkedList(struct Node** head){
    if ( head == NULL){
        return;
    }
    struct Node *ptr = (*head);
    struct Node *tmp;
    while ( ptr != NULL){
        tmp = ptr->next;
        free(ptr);
        ptr = tmp;
    }
    *head = NULL;
}
void printList(struct Node *head){
    printf("*****LINKED-LIST*****\n");
    if (head == NULL){
        printf("Empty List\n");
        printf("***LINKED-LIST-END***\n");
        return;
    }
    struct Node *ptr = head;

    while (ptr != NULL){
        printf("f:%d addr:%lu\t",ptr->free, ptr->address);
        ptr = ptr->next;
    }
    printf("(NULL)");
    printf("\n***LINKED-LIST-END***\n");

}

#endif //L1CACHE_FIRST_H
