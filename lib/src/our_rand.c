#include "our_rand.h"

static unsigned long int next = 1;

int our_rand(void)   // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

void our_srand(unsigned int seed) {
    next = seed;
}

unsigned long int get_next() {
    return next;
}