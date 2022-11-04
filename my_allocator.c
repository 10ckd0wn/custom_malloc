#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define u8 uint8_t
#define u16 uint16_t
#define STACK_SIZE 32
#define HEAP_SIZE STACK_SIZE * 4
#define HEADER 4

static u16 IN_USE;

typedef struct virtual_memory {
    u8 stack[STACK_SIZE];
    char** unmapped;
    u8 heap[HEAP_SIZE];

    struct{
        char** data;
        char** bss;
        char* text;
    }data_t;
}virtual_memory_t;

void* my_malloc(size_t size);

void my_free(void* ptr);

int main(int argc, char const *argv[]) {
    printf("Размер заголовка 4 байта \n");
    if (strcmp(argv[1], "int")==0){
        int* var;
        int temp = atoi(argv[2]);
        var = my_malloc(sizeof(int));
        memcpy(var, &temp, sizeof(int));
        printf("Address: [%p], data: [%d] \n", var, *var);
        my_free(var);
    }
    else {
        char* str;
        str = my_malloc(5);
        strcpy(str, argv[2]);
        printf("Address: [%p], data: [%s] \n", str, str);
        my_free(str);
    }
    return 0;
}

typedef struct entity{
    u8* ptr;
    u16 size;
}entity_t;

entity_t LIST[40];

void LOG(){
    printf("\n");
    for (unsigned i = 0; i < IN_USE; i ++){
        printf("Данные + заголовок. [%p]. Свободная память в хипе:[%u]\n", LIST[i].ptr, LIST[i].size);
    }
    printf("Используемое количество entity:[%d] \n", (sizeof(LIST)/sizeof(LIST[0])-IN_USE));
}



entity_t* new_entity(size_t size){
    if(LIST[0].ptr == NULL && LIST[0].size == 0){
        static virtual_memory_t vm;
        LIST[0].ptr = vm.heap;
        LIST[0].size = HEAP_SIZE;
        IN_USE++;
        LOG();
    }
    entity_t* best = LIST;

    for (unsigned i=0; i < IN_USE; i++){
		if (LIST[i].size >= size && LIST[i].size < best->size){
            best = &LIST[i];
        }
    }

    return best;
}


void* my_malloc(size_t size){
    assert(size <= HEAP_SIZE);

    size += HEADER;

    entity_t* e = new_entity(size);

    u8* start_ptr = e->ptr;
    u8* user_ptr = start_ptr + HEADER;

    *start_ptr = size;

    e->ptr += size;
    e->size -=size;

    //assert();
    LOG();
    return user_ptr;
}

void my_free(void* ptr){
    u8* start_ptr = (u8*)ptr - HEADER;
    LIST[IN_USE].ptr = &(*start_ptr);
    LIST[IN_USE].size = (u8)*((u8*)ptr - HEADER);
    IN_USE++;
    LOG();
}
