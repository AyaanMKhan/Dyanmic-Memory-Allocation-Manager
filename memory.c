#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#define MEMORY_SIZE 4*1024
#define MEMORY_NODE_SIZE sizeof(memory_node_t)
#define POINT_NULL ((void*)0)

// Double Linked List to track memory blocks
typedef struct memory_node {
    uint32_t size;
    bool used;
    struct memory_node *next;
    struct memory_node *prev;
}memory_node_t;

// 4 KB for memory to be allocated from
static uint8_t memory_area[MEMORY_SIZE];

// Points to the first memory block, manager of memory
static memory_node_t *memory_start;

// Setting up the memory block
void init_dynamic_memory(){

    memory_start = (memory_node_t *)memory_area;
    memory_start->size = MEMORY_SIZE - MEMORY_NODE_SIZE;
    memory_start->next = POINT_NULL;
    memory_start->prev = POINT_NULL;

}

void *find_memory_block(memory_node_t *dynamic_memory, size_t size){

    // Init pointer with NULL
    memory_node_t *block = (memory_node_t *) POINT_NULL;
    uint32_t block_size = MEMORY_SIZE + 1;

    // Looking for the smallest used memory 
    memory_node_t *current = dynamic_memory;
    while(current){
        // Checks for the smallest possible usable block of memory
        if(!current->used && current->size >= (size + MEMORY_NODE_SIZE) && current->size <= block_size) {
            // if found then the block takes the size of the currently found
            block = current;
            block_size = current->size;
        }
        // move to next block
        current = current->next;
    }

    return block; 

}


void *memory_allocation(size_t size) {
    // Find a suitable memory block
    memory_node_t *block = find_memory_block(memory_start, size); 

    // Checks if a valid block was found
    if (block != POINT_NULL) {
        // Calculate the remaining size after allocation
        uint32_t remaining_size = block->size - size - MEMORY_NODE_SIZE;
        
        // If there's enough space to create a new block
        if (remaining_size >= MEMORY_NODE_SIZE) {
            block->size = remaining_size; // Reduce the size of the current block

            // Create a new memory node for the allocated block
            memory_node_t *node_allocate = (memory_node_t *)(((uint8_t *)block) + MEMORY_NODE_SIZE + block->size);
            node_allocate->size = size;
            node_allocate->used = true;
            node_allocate->next = block->next;
            node_allocate->prev = block;

            // Re-link the list
            if (block->next != POINT_NULL) {
                block->next->prev = node_allocate;
            }
            block->next = node_allocate;


            // Return the usable memory (after the memory node metadata)
            return (void *)((uint8_t *)node_allocate + MEMORY_NODE_SIZE);
        } else {
            // Not enough space to split the block, so just use the entire block
            block->used = true;

            

            return (void *)((uint8_t *)block + MEMORY_NODE_SIZE);
        }
    }

    return POINT_NULL; // No suitable block found
}


// Function connects the current with the next memory
void *merge_next_current(memory_node_t *current){

    memory_node_t *next = current->next;
    if(next != POINT_NULL && !next->used){

        // Adds the next and current blocks together
        current->size += current->next->size;
        current->size += MEMORY_NODE_SIZE;


        // Remove next from the list
        current->next = current->next->next;
        if(current->next != POINT_NULL){
            current->next->prev = current;
        }
    }    

    return current;
}

// Function connects the prev memory with the current
void *merge_current_prev(memory_node_t *current){

    memory_node_t *prev = current->prev;

    if(prev != POINT_NULL && !prev->used){

        // Adds the previous and current blocks together
        prev->size += current->size;
        prev->size += MEMORY_NODE_SIZE;

        // Remove current from the list
        prev->next = current->next;
        if(current->next != POINT_NULL){
            current->next->prev = prev;
        }

    }

}


// This is display function, for test the coding
void print_memory_blocks() {
    memory_node_t *current = memory_start;
    printf("\nMemory Blocks:\n");
    printf("%-15s %-10s %-10s %-10s\n", "Address", "Size", "Used", "Next");

    while (current) {
        printf("%-15p %-10u %-10s %-15p\n", 
            (void *)current, 
            current->size, 
            current->used ? "true" : "false", 
            (void *)current->next);
        current = current->next;
    }
    printf("\n");
}


void free_memory(void *p){

    if (p == POINT_NULL){
        return;
    }

    // Get the pointer's memory
    memory_node_t *current = (memory_node_t *) ((uint8_t *) p - MEMORY_NODE_SIZE);

    if(current == POINT_NULL){
        return;
    }

    // make block unused
    current->used = false;

    // merge all the unsed 
    current = merge_next_current(current);
    merge_current_prev(current);

}


int main(){

    init_dynamic_memory();
    print_memory_blocks();

    printf("Allocating 500 bytes of memory \n");
    void *p1 = memory_allocation(500);
    print_memory_blocks();

    printf("Allocating 200 bytes of memory\n");
    void *p2 = memory_allocation(200);
    print_memory_blocks();
    
    printf("Allocating 1000 bytes of memory\n");
    void *p3 = memory_allocation(1000);
    print_memory_blocks();

    printf("Freeing 500 bytes of memory from %p", p1);
    free_memory(p1);
    print_memory_blocks();

    printf("Freeing 200 bytes of memory from %p\n", p2);
    free_memory(p2);
    print_memory_blocks();

    printf("Freeing 1000 bytes of money from %p \n", p3);
    free_memory(p3);
    print_memory_blocks();

}
