#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc != 3){
        perror("not enough parameters provided");
        exit(-1);
    }

    int file1 = open(argv[1], O_RDONLY);
    int file2 = open(argv[2], O_RDONLY);

    if(file1 == -1){
        close(file2);
        perror("couldnt open first file");
        exit(-1);
    }

    if(file2 == -1){
        close(file1);
        perror("couldnt open second file");
        exit(-1);
    }

    char ch1;
    char ch2;
    //Get the first char of the files
    int bytesRead1 = read(file1, &ch1, 1);
    if(bytesRead1 == -1)  exit(-1);
    int bytesRead2 = read(file2, &ch2, 1);
    if(bytesRead2 == -1)  exit(-1);

     while(bytesRead1 != 0 && bytesRead2 != 0){
        if(ch1 != ch2){
            exit(1);
        }
        // get the next char of the files
        bytesRead1 = read(file1, &ch1, 1);
        bytesRead2 = read(file2, &ch2, 1);
    }
    if((bytesRead1 == 0 && bytesRead2 != 0) || (bytesRead1 != 0 && bytesRead2 == 0)) exit(1);

    exit(2);
}