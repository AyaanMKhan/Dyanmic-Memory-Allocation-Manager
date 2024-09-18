#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#define MEMORY_SIZE 4*1024
#define MEMORY_NODE_SIZE sizeof(memory_node_t)
#define POINT_NULL ((void*)0)

static uint8_t memory_area[MEMORY_SIZE];


static memory_node_t *memory_start;

typedef struct memory_node {
    uint32_t size;
    bool used;
    struct memory_node *next;
    struct memory_node *prev;
}memory_node_t;

void init_dynamic_memory();
void *find_memory_block(memory_node_t *dynamic_memory, size_t size);
void *memory_allocation(size_t size);
void *memory_allocation(size_t size);
void *merge_next_current(memory_node_t *current);
void *merge_current_prev(memory_node_t *current);
void free_memory(void *p);