#include <stdio.h>

int main(int argc, char* argv[]){
    if(argc != 3){ //if not enough parameters were provided
        perror("not enough parameters provided");
        exit(-1);
    }

    printf("Multiplying the numbers\n"); //write the first char of the string to stdout
    int result = atoi(argv[1]) + atoi(argv[2]);
    printf("%d", result);
    exit(0);
}