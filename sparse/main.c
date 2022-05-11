#include <stdio.h>

#ifdef __CHECKER__
#define __void_region __attribute__((noderef, address_space(2)))
#else
#define __void_region
#endif

int main(void)
{
    int i;
    void * __void_region pMemory = NULL;
    pMemory = &i;

    printf("i is: %d\n", *(int*)pMemory);

    return 0;
}
