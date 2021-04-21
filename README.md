# protected_stack
This repo has with protections such as canaries, hashsum and poison values. To make stack protected, compile it with _DEBUG. 
1. Canaries are values at the start and at the end of stack structure, and at the start and end stack array. Their change means that some memory manipulations from outside changed them.
2. Hash sum. It contains hashed stack structure and array, but not the hash value, recomputed after every push/pop operation. If Hash(stack) != current hash sum, then something is wrong.
3. Poison values: stack is actually dynamic array with some capacity, realloced when filled. All elements that are alloced, but not used as values, must be "poison" values. If alloced but not used value is not a poison value, than something is wrong.
Stack dump function helps to see what is wrong.
