#include "lab2.h"
#include "cache.h"

int main(int argc, char *argv[]){
    // Check for correct number of arguments
    if(argc < 2){
        printf("Error: Incorrect number of arguments.\n"
                "Usage: ./test <config file>\n");
        return 1;
    }

    // Open the config file
    FILE *configFile = fopen(argv[1], "r");
    if(!configFile){
        printf("Error: Config file not found.\n");
        return 1;
    }
    printf("Line size:          %d B\n", config[0]);
    printf("Associativity:      %d-way mapping\n", config[1]);
    printf("Data size:          %d KB\n", config[2]);
    printf("Replacement policy: %d\n", config[3]);
    printf("Miss penalty:       %d\n", config[4]);
    printf("Write policy:       %d\n", config[5]);

    // Read the config file
    int* config = readConfig(configFile);

    // Create the cache
    struct Cache cache = createCache(config);
    free(config);
    printf("Total Blocks:       %d blocks\n", cache.config.numBlocks);
    printf("Total Sets:         %d sets\n", cache.blockConfig.numSets);
    printf("Blocks/Set:         %d blocks\n", cache.config.associativity);
    printf("Bits/Block:         %d bits\n", cache.blockConfig.blockSize);

    // Print the cache
    printBlockLayout(cache.blockConfig);
    printCache(cache);

    int BUF = 64;

    char input[BUF];
    int numInput;
    while(1){
        printf("Test load 'l' or store 's'? Or print 'p'?");
        scanf("%s", input);
        // get char
        char c = input[0];
        switch (c)
        {
        case 'l':
            
            break;
        case 's':
            /* code */
            break;
        case 'p':
            printCache(cache);
            break;
        default:
            break;
        }

    }

}