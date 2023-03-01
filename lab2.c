#include "lab2.h"
/**
 * Functions for the cache simulator
 * @author: Kor Shelley
 * @date: 2023-02-28
 * @version: 1.0
 */

/**
 * Calculates if a number is a power of 2 
 * @param x: The number to check
 * @return: 1 if x is a power of 2, 0 otherwise
 */ 
int isPowerOfTwo(int x){
    return (x != 0) && ((x & (x - 1)) == 0);
}

/**
 * Reads the config file and returns an array of the values
 * @param configFile: The config file to read
 * @return: An array of the values in the config file
 */
int* readConfig(FILE *configFile){
    int *config = malloc(sizeof(int) * 6);
    char *line = malloc(sizeof(char) * 100);
    int i = 0;
    while(fgets(line, sizeof(line), configFile)){
        printf("%s", line);
        int tmp = atoi(line);

        switch (i)
        {
        case 0:
            if(!isPowerOfTwo(tmp)){
                printf("Error: Line size is not a power of 2.\n");
                exit(1);
            }
            config[0] = tmp;
            break;
        case 1:
            if(!isPowerOfTwo(tmp) && tmp != 0){
                printf("Error: Associativity is not a power of 2.\n");
                exit(1);
            }
            config[1] = tmp;
            break;
        case 2:
            if(!isPowerOfTwo(tmp)){
                printf("Error: Data size is not a power of 2.\n");
                exit(1);
            }
            config[2] = tmp;
            break;
        case 3:
            if(tmp != 0 && tmp != 1){
                printf("Error: Replacement policy is not 0 or 1.\n");
                exit(1);
            }
            config[3] = tmp;
            break;
        case 4:
            config[4] = tmp;
            break;
        case 5:
            if(tmp != 0 && tmp != 1){
                printf("Error: Write policy is not 0 or 1.\n");
                exit(1);
            }
            config[5] = tmp;
            break;
        default:
            printf("Error: Too many lines in config file.\n");
            exit(1);
            break;
        }
        i++;
    }
    fclose(configFile);
    free(line);
    return config;
}