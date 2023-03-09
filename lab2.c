#include "lab2.h"
#include <math.h>
/**
 * Functions for the cache simulator
 * @author: Kor Shelley
 * @date: 2023-02-28
 */


// Conversion functions

/**
 * Calculates if a number is a power of 2 
 * @param x: The number to check
 * @return: 1 if x is a power of 2, 0 otherwise
 */ 
int isPowerOfTwo(int x){
    return (x != 0) && ((x & (x - 1)) == 0);
}

/**
 * Converts an integer to binary
 * @param n: The integer to convert
 * @param bits: The number of bits to use (returns NULL if bits is 0)
 * @return: An array of the binary representation of n
 */
int* intToBinary(int n, int bits){
    // Calculate the number of bits needed to represent n
    int b = ceil(log2(n));

    // Check if the number of bits is valid
    if(bits == 0){
        return NULL;
    }
    if(b > bits){
        printf("Error: The number %d cannot be represented in %d bits.\n", n, bits);\
        exit(1);
    }

    // Allocate memory for the binary string (must be freed by the caller)
    int *binary;
    binary = malloc(sizeof(int) * bits);
    if(binary == NULL){
        printf("Error: Could not allocate memory for binary.\n");
        printf("%d n %d bits\n", n, bits);
        exit(1);
    }

    // Convert the integer to binary
    for(int i = bits - 1; i >= 0; i--){
        binary[i] = n % 2;
        n /= 2;
    }
    return binary;
}

/**
 * Converts a hex string to a binary string
 * @param hex: The hex string to convert (must be 8 characters long/32 bits)
 * @return: The binary string (will be 32 bits long)
 */
int* hexToBinary(char *hex){
    int *binary = malloc(sizeof(int) * 32);
    if(binary == NULL){
        printf("Error: Could not allocate memory for binary.\n");
        exit(1);
    }
    // Convert each hex character to binary
    for(int i = 0; i < 8; i++){
        // Grab the integer value of the hex character
        int hexVal = 0;
        if(hex[i] >= '0' && hex[i] <= '9'){
            hexVal = hex[i] - '0';
        } else if(hex[i] >= 'A' && hex[i] <= 'F'){
            hexVal = hex[i] - 'A' + 10;
        } else if(hex[i] >= 'a' && hex[i] <= 'f'){
            hexVal = hex[i] - 'a' + 10;
        } else {
            printf("Error: Invalid hex character: %c.\n", hex[i]);
            exit(1);
        }
        // Convert the int value to binary
        int *tmp = intToBinary(hexVal, 4);
        // Add the binary value to the binary string
        for(int j = 0; j < 4; j++){
            binary[i * 4 + j] = tmp[j];
        }
        free(tmp);
    }

    return binary;
}

/**
 * Converts a binary string to an integer
 * @param binary: The binary string to convert
 * @param bits: The number of bits to use
 * @return: The integer representation of the binary string
 */
int binaryToInt(int *binary, int bits){
    int ret = 0;
    for(int i = 0; i < bits; i++){
        ret += binary[i] * pow(2, bits - i - 1);
    }
    return ret;
}


// Config file functions

/**
 * Reads the config file and returns an array of the values
 * @param configFile: The config file to read
 * @return: An array of the values in the config file
 */
int* readConfig(FILE *configFile){
    int *config = malloc(sizeof(int) * 6);
    if(config == NULL){
        printf("Error: Could not allocate memory for config.\n");
        exit(1);
    }

    char *line = malloc(sizeof(char) * 100);
    if(line == NULL){
        printf("Error: Could not allocate memory for line.\n");
        exit(1);
    }

    int i = 0;
    // Read each line of the config file
    while(fgets(line, sizeof(line), configFile)){
        int tmp = atoi(line); // Convert the line to an int

        switch (i)
        {
        case 0:
            // LINE SIZE: Block size for the cache in bytes
            if(!isPowerOfTwo(tmp)){
                printf("Error: Line size is not a power of 2.\n");
                exit(1);
            }
            config[0] = tmp;
            break;
        case 1:
            // ASSOCIATIVITY: Number of blocks per set
            if(!isPowerOfTwo(tmp) && tmp != 0){
                printf("Error: Associativity is not a power of 2.\n");
                exit(1);
            }
            config[1] = tmp;
            break;
        case 2:
            // DATA SIZE: Size of the cache in kilobytes
            if(!isPowerOfTwo(tmp)){
                printf("Error: Data size is not a power of 2.\n");
                exit(1);
            }
            config[2] = tmp;
            break;
        case 3:
            // REPLACEMENT POLICY: 0 for random, 1 for FIFO
            if(tmp != 0 && tmp != 1){
                printf("Error: Replacement policy is not 0 or 1.\n");
                exit(1);
            }
            config[3] = tmp;
            break;
        case 4:
            // MISS_PENALTY: Number of cycles used on a miss
            config[4] = tmp;
            break;
        case 5:
            // WRITE POLICY: 0 for write-through + no write allocate,
            //               1 for write-back + write allocate
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