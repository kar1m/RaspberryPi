#ifndef PHYALLOC_DYN_H
#define PHYALLOC_DYN_H

void* dyn_alloc(unsigned int size);
void dyn_free(void *ptr, unsigned int size);
void dyn_init(char* start_heap, unsigned int heap_size);

#endif
