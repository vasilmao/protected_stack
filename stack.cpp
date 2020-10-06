#include "stack.h"

const int MODE_X_CONSTANT = 1;
const int MODE_PLUS_DELTA = 2;


struct dynamic_stack *dynamic_stack_construct(uint64_t start_size, uint64_t delta, float constant, char mode) {
    struct dynamic_stack *st = (struct dynamic_stack *)calloc(1, sizeof(dynamic_stack));
    st->size = 0;
    st->capacity = start_size;
    st->array = (double *)calloc(start_size, sizeof(double));
    for (int i = 0; i < start_size; ++i) {
        st->array[i] = NAN;
    }
    st->delta = delta;
    st->mode = mode;
    st->constant = constant;
    return st;
}

void dynamic_stack_increase_capacity(struct dynamic_stack *st) {
    if (st->size < st->capacity) {
        return ;
    }
    if (st->mode == MODE_PLUS_DELTA) {
        st->array = (double *)realloc(st->array, (st->capacity + st->delta) * sizeof(double));
        assert(st->array);
        for (int i = st->capacity; i < st->capacity + st->delta; ++i) {
            st->array[i] = NAN;
        }
        st->capacity += st->delta;
    } else if (st->mode == MODE_X_CONSTANT) {
        st->array = (double *)realloc(st->array, st->capacity * st->constant);
        assert(st->array);
        for (int i = st->capacity; i < st->capacity * st->constant; ++i) {
            st->array[i] = NAN;
        }
        st->capacity *= st->constant;
    }
}

void dynamic_stack_decrease_capacity(struct dynamic_stack *st) {
    if (st->mode == MODE_PLUS_DELTA) {
        if (st->size + 30 + st->delta <= st->capacity) {
            st->array = (double *)realloc(st->array, (st->capacity - st->delta) * sizeof(double));
            assert(st->array);
            st->capacity -= st->delta;
        }
    } else if (st->mode == MODE_X_CONSTANT) {
        if (st->size * st->constant + 30 <= st->capacity) {
            st->array = (double *)realloc(st->array, (st->capacity / st->constant) * sizeof(double));
            assert(st->array);
            st->capacity /= st->constant;
        }
    }
}

void dynamic_stack_push(struct dynamic_stack *st, double el) {
    dynamic_stack_increase_capacity(st);
    st->array[st->size++] = el;
}

void dynamic_stack_pop(struct dynamic_stack *st) {
    assert(st->size > 0);
    dynamic_stack_decrease_capacity(st);
    st->size--;
}


int dynamic_stack_get_top(struct dynamic_stack *st) {
    assert(st->size > 0);
    return st->array[st->size - 1];
}

void dynamic_stack_clear(struct dynamic_stack *st) {
    st->size = 0;
}

void dynamic_stack_destroy(struct dynamic_stack *st) {
    free(st->array);
    free(st);
}

StackErrors dynamic_stack_ok(struct dynamic_stack *st) {
    if (st->size > st->capacity) {
        return OVERFLOWERROR;
    }

    if (st->capacity < 0) {
        return CAPACITYERROR;
    }

    if (st->size < 0) {
        return SIZEERROR;
    }

    for (int i = 0; i < st->capacity; ++i) {
        if (isnan(st->array[i]) && i < st->size) {
            return POISONERROR;
        } else if (!isnan(st->array[i]) && i >= st->size) {
            return POISONERROR;
        }
    }
    return StackErrors::STACKOK;
}

void dynamic_stack_dump(struct dynamic_stack *st) {
    StackErrors res = dynamic_stack_ok(st);
    if (res == STACKOK) {
        printf("dynamic_stack (ok) [%p] {\n", st);
        printf("\tsize = %ld\n\tcapacity = %ld\n\tdelta = %ld\n\tconstant = %f\n\tmode = %d\n", st->size, st->capacity, st->delta, st->constant, st->mode);
        printf("\tarray [%p] {\n", st->array);
        for (int i = 0; i < st->capacity; ++i) {
            if (i < st->size) {
                printf("\t\t*[%d] = %f\n", i, st->array[i]);
            } else {
                printf("\t\t [%d] = %f (!!POISON!!)\n", i, st->array[i]);
            }
        }
        printf("\t}\n");
        printf("}\n");
    } else {
        printf("dynamic stack (");
        if (res == OVERFLOWERROR) {
            printf("OVERFLOWERROR");
        } else if (res == POISONERROR) {
            printf("POISONERROR");
        } else if (res == CAPACITYERROR) {
            printf("CAPACITYERROR");
        } else if (res == SIZEERROR) {
            printf("SIZEERROR");
        }
        printf(") [%p] {\n", st);
        printf("\tsize = %ld\n\tcapacity = %ld\n\tdelta = %ld\n\tconstant = %f\n\tmode = %d\n", st->size, st->capacity, st->delta, st->constant, st->mode);
        printf("\tarray [%p] {\n", st->array);
        for (int i = 0; i < st->capacity; ++i) {
            if (i < st->size) {
                printf("\t\t*[%d] = %f\n", i, st->array[i]);
            } else {
                printf("\t\t [%d] = %f (!!POISON!!)\n", i, st->array[i]);
            }
        }
        printf("\t}\n");
        printf("}\n");
    }
}




struct stack *stack_construct() {
    return (struct stack *) calloc(1, sizeof(struct stack));
}

void stack_push(struct stack *st, int el) {
    assert(st);

    if (st->size == 0) {
        st->last_node = (node *)calloc(1, sizeof(struct node));
        st->last_node->element = el;
        st->last_node->minimum = el;
        st->last_node->next = NULL;
    } else {
        struct node *new_node = (node *)calloc(1, sizeof(struct node));
        new_node->element = el;
        new_node->minimum = el < st->last_node->minimum ? el : st->last_node->minimum;
        new_node->next = st->last_node;
        st->last_node = new_node;
    }
    st->size++;
}

void stack_pop(struct stack *st) {
    assert(st);
    assert(st->size > 0);
    struct node *old_node = st->last_node;
    st->last_node = st->last_node->next;
    free(old_node);
    st->size--;
}

int stack_get_top(struct stack *st) {
    assert(st);
    assert(st->size > 0);
    return st->last_node->element;
}

int stack_get_minimum(struct stack *st) {
    assert(st);
    assert(st->size > 0);
    return st->last_node->minimum;
}

void stack_clear(struct stack *st) {
    while (st->size > 0) {
        stack_pop(st);
    }
    //free(st);
}

void stack_destroy(struct stack *st) {
    stack_clear(st);
    free(st);
}



int min(int a, int b) {
    return a < b ? a : b;
}
