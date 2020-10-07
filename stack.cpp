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
    ASSERT_OK(st);
    return st;
}

void dynamic_stack_increase_capacity(struct dynamic_stack *st) {
    ASSERT_OK(st);
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
    ASSERT_OK(st);
    if (st->mode == MODE_PLUS_DELTA) {
        if (st->size + 30 + st->delta <= st->capacity) {
            st->array = (double *)realloc(st->array, (st->capacity - st->delta) * sizeof(double));
            assert(st->array);
            st->capacity -= st->delta;
        }
    } else if (st->mode == MODE_X_CONSTANT) {
        if ((st->size + 30) * st->constant <= st->capacity) {
            st->array = (double *)realloc(st->array, (st->capacity / st->constant) * sizeof(double));
            assert(st->array);
            st->capacity /= st->constant;
        }
    }
}

void dynamic_stack_push(struct dynamic_stack *st, double el) {
    ASSERT_OK(st);
    dynamic_stack_increase_capacity(st);
    st->array[st->size++] = el;
}

void dynamic_stack_pop(struct dynamic_stack *st) {
    ASSERT_OK(st);
    assert(st->size > 0);
    dynamic_stack_decrease_capacity(st);
    st->size--;
}


int dynamic_stack_get_top(struct dynamic_stack *st) {
    ASSERT_OK(st);
    assert(st->size > 0);
    return st->array[st->size - 1];
}

void dynamic_stack_clear(struct dynamic_stack *st) {
    ASSERT_OK(st);
    st->size = 0;
}

void dynamic_stack_destroy(struct dynamic_stack *st) {
    ASSERT_OK(st);
    free(st->array);
    free(st);
}

StackErrors dynamic_stack_ok(struct dynamic_stack *st) {
    assert(st);
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
    ASSERT_OK(st);
    StackErrors res = dynamic_stack_ok(st);
    FILE* output = open("stackdump.txt", "w");
    assert(output);
    if (res == STACKOK) {
        fprintf(output, "dynamic_stack (ok) [%p] {\n", st);
        fprintf(output, "\tsize = %ld\n\tcapacity = %ld\n\tdelta = %ld\n\tconstant = %f\n\tmode = %d\n", st->size, st->capacity, st->delta, st->constant, st->mode);
        fprintf(output, "\tarray [%p] {\n", st->array);
        for (int i = 0; i < st->capacity; ++i) {
            if (i < st->size) {
                fprintf(output, "\t\t*[%d] = %f\n", i, st->array[i]);
            } else {
                fprintf(output, "\t\t [%d] = %f (!!POISON!!)\n", i, st->array[i]);
            }
        }
        fprintf(output, "\t}\n");
        fprintf(output, "}\n");
    } else {
        fprintf(output, "dynamic stack (");
        if (res == OVERFLOWERROR) {
            fprintf(output, "OVERFLOWERROR");
        } else if (res == POISONERROR) {
            fprintf(output, "POISONERROR");
        } else if (res == CAPACITYERROR) {
            fprintf(output, "CAPACITYERROR");
        } else if (res == SIZEERROR) {
            fprintf(output, "SIZEERROR");
        }
        fprintf(output, ") [%p] {\n", st);
        fprintf(output, "\tsize = %ld\n\tcapacity = %ld\n\tdelta = %ld\n\tconstant = %f\n\tmode = %d\n", st->size, st->capacity, st->delta, st->constant, st->mode);
        fprintf(output, "\tarray [%p] {\n", st->array);
        for (int i = 0; i < st->capacity; ++i) {
            if (i < st->size) {
                fprintf(output, "\t\t*[%d] = %f\n", i, st->array[i]);
            } else {
                fprintf(output, "\t\t [%d] = %f (!!POISON!!)\n", i, st->array[i]);
            }
        }
        fprintf(output, "\t}\n");
        fprintf(output, "}\n");
    }
}


int min(int a, int b) {
    return a < b ? a : b;
}
