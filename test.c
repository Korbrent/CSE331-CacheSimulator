#include <stdlib.h>
#include <stdio.h>
#include "lab2.h"

int main(int argc, char *argv[]){
    // int* config = malloc(sizeof(int) * 6);
    // config[0] = 4;
    // config[1] = 2;
    // config[2] = 8;
    // config[3] = 0;
    // config[4] = 10;
    // config[5] = 0;
    // struct Cache cache = createCache(config);
    // printCache(cache.cache, cache.config[0], cache.config[1], cache.config[2]);


    // int* bin = intToBinary(10, 0);
    // for(int i = 0; i < bin[0]; i++){
    //     printf("i: \t%d \t| a[i] \t%d\n", i, bin[i]);
    // }

    // for(int i = 1; i < bin[0]; i++){
    //     printf("i: \t%d \t| a[i] \t%d\n", i, bin[i]);
    // }

    // free(bin);
    // bin = intToBinary(10, 4);
    // for(int i = 0; i < 4; i++){
    //     printf("i: \t%d \t| a[i] \t%d\n", i, bin[i]);
    // }

    int* binary = intToBinary(3295, 32);
    int num = binaryToInt(binary, 32);
    printf("num: %d\n", num);

    return 0;
}