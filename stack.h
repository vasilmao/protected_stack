#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef double Elem_t;

extern const char MODE_PLUS_DELTA;
extern const char MODE_X_CONSTANT;
extern const long long KANAREYKA_L_ST;
extern const long long KANAREYKA_R_ST;
extern const long long KANAREYKA_L;
extern const long long KANAREYKA_R;


enum StackErrors {
    STACKOK = 0,
    OVERFLOWERROR,
    POISONERROR,
    SIZEERROR,
    CAPACITYERROR,
    KANAREYKAERROR
};


#ifdef _DEBUG
    #define ASSERT_OK(st) if (dynamic_stack_ok(st) != STACKOK) { dynamic_stack_dump(st); assert(!"OK"); }
#else
    #define ASSERT_OK(st)
#endif


struct dynamic_stack {
    long long kanareyka_l;
    Elem_t* array;
    uint64_t size;
    uint64_t capacity;
    uint64_t delta;
    uint64_t coefficient;
    float constant;
    char mode;
    long long kanareyka_r;
};


struct dynamic_stack *dynamic_stack_construct(uint64_t start_size, uint64_t delta, float constant, char mode);

void dynamic_stack_increase_capacity(struct dynamic_stack *st);

void dynamic_stack_decrease_capacity(struct dynamic_stack *st);

void dynamic_stack_push(struct dynamic_stack *st, Elem_t el);

void dynamic_stack_pop(struct dynamic_stack *st);

int dynamic_stack_get_top(struct dynamic_stack *st);

void dynamic_stack_clear(struct dynamic_stack *st);

void dynamic_stack_destroy(struct dynamic_stack *st);

void dynamic_stack_dump(struct dynamic_stack *st);

StackErrors dynamic_stack_ok(struct dynamic_stack *st);
