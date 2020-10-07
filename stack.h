#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

extern const int MODE_PLUS_DELTA;
extern const int MODE_X_CONSTANT;


enum StackErrors {
    STACKOK = 0,
    OVERFLOWERROR,
    POISONERROR,
    SIZEERROR,
    CAPACITYERROR
};


#ifdef _DEBUG
    #define ASSERT_OK(st) if (dynamic_stack_ok(st) != STACKOK) { dynamic_stack_dump(st); assert(!"OK"); }
#else
    #define ASSERT_OK(st)
#endif

struct node {
    int element;
    int minimum;
    struct node *next;
};


struct dynamic_stack {
    uint64_t kanareyka1;
    double* array;
    uint64_t size;
    uint64_t capacity;
    uint64_t delta;
    uint64_t coefficient;
    float constant;
    char mode;
    uint64_t kanareyka2;
};


struct dynamic_stack *dynamic_stack_construct(uint64_t start_size, uint64_t delta, float constant, char mode);

void dynamic_stack_increase_capacity(struct dynamic_stack *st);

void dynamic_stack_decrease_capacity(struct dynamic_stack *st);

void dynamic_stack_push(struct dynamic_stack *st, double el);

void dynamic_stack_pop(struct dynamic_stack *st);

int dynamic_stack_get_top(struct dynamic_stack *st);

void dynamic_stack_clear(struct dynamic_stack *st);

void dynamic_stack_destroy(struct dynamic_stack *st);

void dynamic_stack_dump(struct dynamic_stack *st);

StackErrors dynamic_stack_ok(struct dynamic_stack *st);
