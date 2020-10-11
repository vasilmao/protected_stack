#include "stack.h"

const char MODE_X_CONSTANT      = 1;
const char MODE_PLUS_DELTA      = 2;
const long long KANAREYKA_L_ST = 0xBADBEDAA;
const long long KANAREYKA_R_ST = 0xABACADAF;
const long long KANAREYKA_L    = 0xDEADBEEF;
const long long KANAREYKA_R    = 0xABCDEABC;

void ror(void* ptr, uint64_t size) {
    for (int i = 0; i < size; ++i) {
        *((unsigned char*)ptr + i) = *((unsigned char*)ptr + i) << 1 | ((*((unsigned char*)ptr + i) & (1 << 7)) >> 7);
    }
}

void rol(void* ptr, uint64_t size) {
    for (int i = 0; i < size; ++i) {
        *((unsigned char*)ptr + i) = *((unsigned char*)ptr + i) >> 1 | (*((unsigned char*)ptr + i) & 1) << 7;
    }
}

uint64_t dynamic_stack_count_hash(struct dynamic_stack *st) {
    st->hash = 0;
    uint64_t hash = 0;
    rol(st, sizeof(dynamic_stack));
    for (int i = 0; i < sizeof(dynamic_stack); i+= 8) {
        hash ^= *(((uint64_t *)st)+i);
    }
    ror(st, sizeof(dynamic_stack));
    rol(st->array, sizeof(Elem_t));
    for (int i = 0; i < st->capacity; ++i) {
        hash ^= *((uint64_t*)st->array + i);
    }
    ror(st->array, sizeof(Elem_t));
    return hash;
}

StackErrors dynamic_stack_check_hash(struct dynamic_stack* st) {
    uint64_t last_hash = st->hash;
    uint64_t new_hash = dynamic_stack_count_hash(st);
    if (new_hash != last_hash) {
        return HASHERROR;
    }
    st->hash = last_hash;
    return STACKOK;
}

struct dynamic_stack *dynamic_stack_construct(uint64_t start_size, uint64_t delta, float constant, char mode) {
    struct dynamic_stack *st = (struct dynamic_stack *)calloc(1, sizeof(dynamic_stack));
    st->kanareyka_l = KANAREYKA_L_ST;
    st->kanareyka_r = KANAREYKA_R_ST;
    st->size = 0;
    st->capacity = start_size;
    st->array = (Elem_t *)calloc(start_size * sizeof(Elem_t) + 2 * sizeof(long long), 1);
    *(long long *)st->array = KANAREYKA_L;
    st->array = (Elem_t *)((long long *)st->array + 1);
    *(long long *)((char*)st->array + start_size * sizeof(Elem_t)) = KANAREYKA_R;
    for (int i = 0; i < start_size; ++i) {
        st->array[i] = NAN;
    }
    st->delta = delta;
    st->mode = mode;
    st->constant = constant;
    printf("bruh\n");
    st->hash = dynamic_stack_count_hash(st);
    printf("yeah\n");
    ASSERT_OK(st);
    return st;
}

void dynamic_stack_increase_capacity(struct dynamic_stack *st) {
    ASSERT_OK(st);
    if (st->size < st->capacity) {
        return ;
    }
    if (st->mode == MODE_PLUS_DELTA) {
        st->array = (Elem_t*)realloc((char*)st->array - sizeof(long long), (st->capacity + st->delta) * sizeof(Elem_t) + 2 * sizeof(long long));
        st->array = (Elem_t*)((char*)st->array + sizeof(long long));
        assert(st->array);
        for (int i = st->capacity; i < st->capacity + st->delta; ++i) {
            st->array[i] = NAN;
        }
        st->capacity += st->delta;
        *get_array_kanareyka_r(st) = KANAREYKA_R;
        //*(long long *)((char*)st->array + st->capacity * sizeof(Elem_t)) = KANAREYKA_R;
    } else if (st->mode == MODE_X_CONSTANT) {
        st->array = (Elem_t *)realloc(st->array, st->capacity * st->constant);
        assert(st->array);
        for (int i = st->capacity; i < st->capacity * st->constant; ++i) {
            st->array[i] = NAN;
        }
        st->capacity *= st->constant;
        *get_array_kanareyka_r(st) = KANAREYKA_R;
        //*(long long *)((char*)st->array + st->capacity * sizeof(Elem_t)) = KANAREYKA_R;
    }
}

void dynamic_stack_decrease_capacity(struct dynamic_stack *st) {
    ASSERT_OK(st);
    if (st->mode == MODE_PLUS_DELTA) {
        if (st->size + 30 + st->delta <= st->capacity) {
            st->array = (Elem_t *)realloc(st->array, (st->capacity - st->delta) * sizeof(Elem_t));
            assert(st->array);
            st->capacity -= st->delta;
        }
    } else if (st->mode == MODE_X_CONSTANT) {
        if ((st->size + 30) * st->constant <= st->capacity) {
            st->array = (Elem_t *)realloc(st->array, (st->capacity / st->constant) * sizeof(Elem_t));
            assert(st->array);
            st->capacity /= st->constant;
        }
    }
}

void dynamic_stack_push(struct dynamic_stack *st, Elem_t el) {
    ASSERT_OK(st);
    dynamic_stack_increase_capacity(st);
    st->array[st->size++] = el;
    st->hash = dynamic_stack_count_hash(st);
}

void dynamic_stack_pop(struct dynamic_stack *st) {
    ASSERT_OK(st);
    assert(st->size > 0);
    //dynamic_stack_decrease_capacity(st);
    st->size--;
    st->hash = dynamic_stack_count_hash(st);
}


int dynamic_stack_get_top(struct dynamic_stack *st) {
    ASSERT_OK(st);
    assert(st->size > 0);
    return st->array[st->size - 1];
}

void dynamic_stack_clear(struct dynamic_stack *st) {
    ASSERT_OK(st);
    st->size = 0;
    st->hash = dynamic_stack_count_hash(st);
}

void dynamic_stack_destroy(struct dynamic_stack *st) {
    ASSERT_OK(st);
    free(st->array);
    free(st);
}

long long* get_array_kanareyka_l(struct dynamic_stack *st) {
    return (long long*)((char*)st->array - sizeof(long long));
}

long long* get_array_kanareyka_r(struct dynamic_stack *st) {
    return (long long *)((char*)st->array + st->capacity * sizeof(Elem_t));
}

StackErrors dynamic_stack_ok(struct dynamic_stack *st) {
    printf("yes\n");
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

    if (*get_array_kanareyka_l(st) != KANAREYKA_L) {
        return KANAREYKAERROR;
    }

    if (*get_array_kanareyka_r(st) != KANAREYKA_R) {
        return KANAREYKAERROR;
    }

    if (st->kanareyka_l != KANAREYKA_L_ST) {
        return KANAREYKAERROR;
    }

    if (st->kanareyka_r != KANAREYKA_R_ST) {
        return KANAREYKAERROR;
    }
    printf("yes\n");
    if (dynamic_stack_check_hash(st) != STACKOK) {
        return HASHERROR;
    }


    for (int i = 0; i < st->capacity; ++i) {
        if (isnan(st->array[i]) && i < st->size) {
            return POISONERROR;
        } else if (!isnan(st->array[i]) && i >= st->size) {
            return POISONERROR;
        }
    }
    return STACKOK;
}

void dynamic_stack_dump(struct dynamic_stack *st) {
    StackErrors res = dynamic_stack_ok(st);
    FILE* output = fopen("stackdump.txt", "w");
    assert(output);
    if (res == STACKOK) {
        fprintf(output, "dynamic_stack (ok) [%p] {\n", st);
        fprintf(output, "\tLEFT  KANAREYKA = %8X ", st->kanareyka_l); fprintf(output, "(must be %X)\n", KANAREYKA_L_ST);
        fprintf(output, "\tsize            = %llu\n", st->size);
        fprintf(output, "\tcapacity        = %llu\n", st->capacity);
        fprintf(output, "\tdelta           = %llu\n", st->delta);
        fprintf(output, "\tconstant        = %f  \n",   st->constant);
        fprintf(output, "\tmode            = %d  \n",   st->mode);
        fprintf(output, "\tRIGHT KANAREYKA = %8X ", st->kanareyka_r); fprintf(output, "(must be %X)\n", KANAREYKA_R_ST);
        fprintf(output, "\tarray [%p] {\n", st->array);
        fprintf(output, "\t\tLEFT  KANAREYKA = %8X ", *get_array_kanareyka_l(st));
        fprintf(output, "(must be %X)\n", KANAREYKA_L);
        for (int i = 0; i < st->capacity; ++i) {
            if (i < st->size) {
                fprintf(output, "\t   *[%d] = %lf\n", i, st->array[i]);
            } else {
                fprintf(output, "\t\t[%d] = %lf (!!POISON!!)\n", i, st->array[i]);
            }
        }
        fprintf(output, "\t\tRIGHT ARRAY KANAREYKA = %8X ", *get_array_kanareyka_r(st));
        fprintf(output, "(must be %X)\n", KANAREYKA_R);
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
        } else if (res == KANAREYKAERROR) {
            fprintf(output, "KANAREYKAERROR");
        } else if (res == HASHERROR) {
            fprintf(output, "HASHERROR");
        }
        fprintf(output, ") [%p] {\n", st);
        fprintf(output, "\tLEFT  KANAREYKA = %8X ", st->kanareyka_l); fprintf(output, "(must be %X)\n", KANAREYKA_L_ST);
        fprintf(output, "\tsize            = %llu\n", st->size);
        fprintf(output, "\tcapacity        = %llu\n", st->capacity);
        fprintf(output, "\tdelta           = %llu\n", st->delta);
        fprintf(output, "\tconstant        = %f  \n",   st->constant);
        fprintf(output, "\tmode            = %d  \n",   st->mode);
        fprintf(output, "\tRIGHT KANAREYKA = %8X ", st->kanareyka_r); fprintf(output, "(must be %X)\n", KANAREYKA_R_ST);
        fprintf(output, "\tarray [%p] {\n", st->array);
        fprintf(output, "\t\tLEFT  ARRAY KANAREYKA = %8X ", *get_array_kanareyka_l(st));
        fprintf(output, "(must be %X)\n", KANAREYKA_L);
        for (int i = 0; i < st->capacity; ++i) {
            if (i < st->size) {
                fprintf(output, "\t   *[%d] = %lf\n", i, st->array[i]);
            } else {
                fprintf(output, "\t\t[%d] = %lf (!!POISON!!)\n", i, st->array[i]);
            }
        }
        fprintf(output, "\t\tRIGHT ARRAY KANAREYKA = %8X ", *get_array_kanareyka_r(st));
        fprintf(output, "(must be %X)\n", KANAREYKA_R);
        fprintf(output, "\t}\n");
        fprintf(output, "}\n");
    }
}


int min(int a, int b) {
    return a < b ? a : b;
}
