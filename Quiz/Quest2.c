#include<stdio.h>


int main() {
    char arr2[] = {1,2,3};
    char *ptr2 = arr2;
    printf("%lu", sizeof(arr2));
    printf("%lu", sizeof(ptr2));
}