#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define SIZE 50

//this function gets a file descriptor and a char address and writes to the var
int readLine(int fd, char* line){
    char c;
    int i = 0;
    while(read(fd, &c, 1) > 0){
        if(c == '\n') break;

        line[i] = c;
        i++;
    }
    if(i == 0) return -1;
    line[i] = '\0';
    return i;
}

//TODO: making sure the error writes writes to the CLI 
int main(int argc, char* argv[]){

    if(argc != 2){// if note enough parameters were provided
        write(2, "not enough parameters provided", 50);
        exit(-1);
    }

    //creating results.csv file with the right permissions -rw-rw-rw-
    int resultFile = open("results.csv", O_WRONLY | O_RDONLY | O_CREAT, 0666);

    //if creating result file failed
    if(resultFile < 0){
        write(2, "results.csv file didn't open", 50);
        exit(-1);
    }

    int studentsListFile = open("studentsList.txt", O_WRONLY | O_RDONLY | O_CREAT, 0666);
    if(studentsListFile < 0){
        write(2, "students list file didn't open", 50);
        exit(-1);
    }

    //opening the file given in the first parameter
    int config = open(argv[1], O_RDONLY);
    if(config < 0){
        write(2, "config.txt file didn't open", 50);
        exit(-1);
    }


    //we will define variables for storing
    //allStudents names
    char exc1[SIZE];
    //input file
    char input[SIZE];
    //expected file
    char expected[SIZE];

    //reading correct lines to the vars above
    readLine(config, exc1);
    readLine(config, input);
    readLine(config, expected);
    close(config);

    char* asPath = strcat(exc1, "allStudents");
    //we will insert the content that ls returns on allStudents into a file to read from
    char* arguments[10];
    arguments[0] = (char*)malloc(strlen(asPath) + 1);
    strcpy(arguments[0], asPath);
    arguments[1] = NULL;

    //creating a new proccess
    pid_t pid;
    int stat;
    if((pid = fork()) < 0){
        write(2, "fork failed\n", 50);
        exit(-1);
    }
    if(pid == 0){ //the son proccess
        int ret_code, stat;
        close(1); //closing stdout
        dup2(studentsListFile, 1); //now instead of writing to the screen, we'll write to the file given 
        ret_code = execvp("ls", arguments);
        if(ret_code == -1){
            write(2, "execvp failed line 76\n", 50);
            exit(-1);
        }
    }else{
        wait(&stat);
        if(stat != 0){
            write(2, "wait failed\n", 50);
            exit(-1);
        }
    }
    //reading from studentsListFile
    studentsListFile = open("studentsList.txt", O_RDONLY);
    if(studentsListFile < 0){
        write(2, "studentsList.txt file didn't open", 50);
        exit(-1);
    }
    
    //reading from input the two parameter arguments
    int fdInput = open("configure/input.txt", O_RDONLY);
    if(fdInput < 0){
        write(2, "input.txt file didn't open", 50);
        exit(-1);
    }

    //saving the two parameters
    char* firstInput,secondInput;
    readLine(fdInput, firstInput);
    readLine(fdInput, secondInput);

    //running a while loop that runs on the studentsListFile and perfoms comparison for their exe output
    while(studentsListFile != EOF){
        char* path = strdup(exc1);
        //reading a name from the studentListFile
        char* name;
        readLine(studentsListFile, name);

        //creating a new proccess
        pid_t pid;
        int ret_code, stat;
        if((pid = fork()) < 0){
            write(2, "fork failed\n", 50);
            exit(-1);
        }
        if(pid == 0){ //the son proccess
            close(1); //closing stdout
            int fdOutput = open("output.txt", O_WRONLY | O_RDONLY | O_CREAT, 0666);
            if(fdOutput < 0){
                write(2, "output.txt file didn't open", 50);
                exit(-1);
            }
            close(1);
            int stdOutCopy = dup(1); //making a copy of fd 1
            dup2(fdOutput, 1); //now instead of writing to the screen, we'll write to the file given
            char* prePath = strcat(strcat(path, "allStudents/"), name);
            char* finalPath = strcat(prePath, "/main.exe");

            char* args[10];
            args[0] = (char*)malloc(strlen(firstInput) + 1);
            strcpy(args[0], firstInput);
            args[1] = (char*)malloc(strlen(secondInput) + 1);
            strcpy(args[1], secondInput);
            args[2] = NULL;
            ret_code = execvp(finalPath, args);
            if(ret_code == -1){
                write(2, "execvp failed, line 135\n", 50);
            }
            free(prePath);
            free(finalPath);
        }else{ //father
            wait(&stat);
            if(stat != 0){
                write(2, "student main.exe failed\n", 50);
                write(2, "grade: 0\n", 50);
                write(resultFile, name, strlen(name) + 1);
                write(resultFile, ",0\n", 6);
                exit(-1);
            }

            //performing a comparison between the output and the expected output with comp.out
            char* args[10];
            args[0] = (char*)malloc(strlen("output.txt") + 1);
            strcpy(args[0], "output.txt");
            args[1] = (char*)malloc(strlen("configure/expected.txt") + 1);
            strcpy(args[1], "configure/expected.txt");
            args[2] = NULL;

            //creating a new proccess
            if((pid = fork()) < 0){
                write(2, "fork failed\n", 50);
                exit(-1);
            }
            if(pid == 0){// son
                ret_code = execvp("./comp.out", args);
                if(ret_code == -1){
                    write(2, "comp execvp failed, line 142\n", 50);
                    exit(-1);
                }
            }else { //father
                wait(&stat);
                if(stat / 256 == 2){
                    write(resultFile, name, strlen(name) + 1);
                    write(resultFile, ",100\n", 6);
                }else{
                    write(resultFile, name, strlen(name) + 1);
                    write(resultFile, ",0\n", 4);
                }
            }
        }
        free(path);
        free(name);
    }
    return 1;
}