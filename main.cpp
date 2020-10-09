#include "stack.h"

int main() {
    struct dynamic_stack *st = dynamic_stack_construct(20, 20, 0, MODE_PLUS_DELTA);

    int n = 10;
    //dynamic_stack_push(st, 1);
    for(int i = 0; i < n; ++i) {
        dynamic_stack_push(st, i);
        //printf("%d YAY\n", i);
    }
    //st->size = 30;
    //st->array[15] = 10;
    ASSERT_OK(st);
    /*for(int i = 0; i < n / 2; ++i) {
        dynamic_stack_pop(st);
        //printf("%d YOY\n", i);
    }
    for(int i = 0; i < n / 2; ++i) {
        dynamic_stack_push(st, i);
        //printf("%d YuY\n", i);
    }*/
    dynamic_stack_dump(st);
    printf("yeah\n");
}
