//Idan Simai 206821258
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

int openOutput();

//This function creates the results.csv file.
int createResultsFile() {
    int fid = open("results.csv", O_WRONLY | O_CREAT , 0777);
    if(fid < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    return fid;
}

//This function creates the errors.txt file.
int createErrorsFile() {
    int fid = open("errors.txt", O_WRONLY | O_CREAT , 0777);
    if(fid < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    return fid;
}

//Check if a file is a directory.
int isADirectory(char* name) {
    DIR* dir = opendir(name);
    if(dir) {
        return 1;
    }
    if(ENONET == errno) {
        return 0;
    }
    else {
        return -1;
    }
}

//function to read one line from a file using the read sys call.
void readLine(char* buff, int fd, int* index) {
    char s [1] = {0};
    while(*s != '\n') {
        int read1 = read(fd, s, 1);
        if(read1 < 0) {
            write(2, "Error in: read\n", sizeof("Error in: read\n"));
            exit(-1);
        }
        strcat(buff, s);
        (*index)++;
    }
}

//Function to check the validation of the first line from the configuration file.
void firstLineValidation(char* firstLine) {
    if(!isADirectory(firstLine) || access(firstLine, F_OK) != 0) {
        write(2, "Not a valid directory\n", sizeof("Not a valid directory\n"));
        exit(-1);
    }
}

//Function to check the validation of the second line from the configuration file.
void secondLineValidation(char* path) {
    if(access(path, F_OK) != 0) {
        write(2, "Input file not exist\n", sizeof("Input file not exist\n"));
        exit(-1);
    }
}

//Function to check the validation of the third line from the configuration file.
void thirdLineValidation(char* path) {
    if(access(path, F_OK) != 0) {
        write(2, "Output file not exist\n", sizeof("Output file not exist\n"));
        exit(-1);
    }
}

//Function to compile the c file in the directory.
void compileFile(int* checker, int fd, char* directoryPath) {
    DIR* dir = opendir(directoryPath);
    struct dirent* dirent1;
    while((dirent1 = readdir(dir)) != NULL) {
        char *ext = strrchr(dirent1->d_name, '.');
        if (!ext) {
            continue;
        }
        char str [150] = {0};
        strcpy(str, directoryPath);
        strcat(str, "/");
        strcat(str, dirent1->d_name);
        if (strcmp(ext, ".c") == 0 && isADirectory(str) != 1) {
            *checker = 1;
            pid_t pid = fork();
            if (pid < 0) {
                write(2, "Error in: fork\n", sizeof("Error in: fork\n"));
                exit(-1);
            }
            if (pid == 0) {
                dup2(fd, 2);
                strcat(directoryPath, "/");
                strcat(directoryPath, dirent1->d_name);
                //chdir(dirent->d_name);
                if ((execl("/usr/bin/gcc", "/usr/bin/gcc", directoryPath, "-o", "idan.out",
                           (char *) NULL)) < 0) {
                    write(2, "Error in: exec\n", sizeof("Error in: exec\n"));
                    exit(-1);
                }
            }
            wait(NULL);
        }
    }
}

//Function to run the compiled file.
void runFile(int fd, int fd1, int fd2) {
    pid_t pid = fork();
    if (pid < 0) {
        write(2, "Error in: fork\n", sizeof("Error in: fork\n"));
        exit(-1);
    }
    if (pid == 0) {
        dup2(fd1, 0);
        dup2(fd2, 1);
        dup2(fd, 2);
        if ((execl("./idan.out", "./idan.out", (char *) NULL)) < 0) {
            write(2, "Error in: exec\n", sizeof("Error in: exec\n"));
            exit(-1);
        }
    }
    wait(NULL);
}

//Check if the file compiled successfully.
int didCompile() {
    if(access("idan.out", F_OK) == 0) {
        return 1;
    }

    return 0;
}

//Compare the output of the programs using the ex21 program.
int compareOutput(char* path1, char* path2, int fd) {
    int status = 0;
    pid_t pid = fork();
    if (pid < 0) {
        write(2 ,"Error in: fork\n", sizeof("Error in: fork\n"));
        exit(-1);
    }
    if (pid == 0) {
        dup2(fd, 2);
        if ((execl("./comp.out", "./comp.out", path1, path2, (char *) NULL)) < 0) {
            write(2, "Error in: exec\n", sizeof("Error in: exec\n"));
            exit(-1);
        }
    }
    wait(&status);
    int value = WEXITSTATUS(status);
    return value;
}

//Execute the required things for a user.
void executeUser(struct dirent* dirent, int fd2, char* realOutputPath, int resultsFid, int errorsFid,
        char* directoryPath) {
    if ((strcmp(dirent->d_name, ".") != 0) && (strcmp(dirent->d_name, "..") != 0)) {
        char string[500] = {0};
        int checker = 0;
        compileFile(&checker, errorsFid, directoryPath);
        if (didCompile()) {
            //chdir(dirent->d_name);
            int fd4 = openOutput();
            runFile(errorsFid, fd2, fd4);
            char outputAbsPathLen[150];
            char *ptr1 = realpath("output.txt", outputAbsPathLen);
            int value = compareOutput(ptr1, realOutputPath,  errorsFid);
            if(value == 1) {
                strcat(string, dirent->d_name);
                strcat(string, ",100,EXCELLENT\n");
                if (write(resultsFid, string, strlen(string)) < 0) {
                    close(fd4);
                    exit(-1);
                }
            }
            if(value == 2) {
                strcat(string, dirent->d_name);
                strcat(string, ",50,WRONG\n");
                if (write(resultsFid, string, strlen(string)) < 0) {
                    close(fd4);
                    exit(-1);
                }
            }
            if(value == 3) {
                strcat(string, dirent->d_name);
                strcat(string, ",75,SIMILAR\n");
                if (write(resultsFid, string, strlen(string)) < 0) {
                    close(fd4);
                    exit(-1);
                }
            }
                if (remove("idan.out") < 0 || remove("output.txt") < 0) {
                    write(2, "Error in: remove\n", sizeof("Error in: remove\n"));
                    exit(-1);
                }
            lseek(fd2, 0, SEEK_SET);
            close(fd4);
        } else {
            if (checker == 0) {
                strcat(string, dirent->d_name);
                strcat(string, ",0,NO_C_FILE\n");
                if (write(resultsFid, string, strlen(string)) < 0) {
                    exit(-1);
                }
                lseek(fd2, 0, SEEK_SET);
                return;
            }
            if (checker == 1) {
                strcat(string, dirent->d_name);
                strcat(string, ",10,COMPILATION_ERROR\n");
                if (write(resultsFid, string, strlen(string)) < 0) {
                    exit(-1);
                }
                lseek(fd2, 0, SEEK_SET);
                return;
            }
        }
    }
}

//This function opens the output.txt file.
int openOutput() {
    int fd4 = open("output.txt", O_WRONLY | O_CREAT, 0777);
    if (fd4 < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    return fd4;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        write(2, "Not enough parameters!\n", sizeof("Not enough parameters!\n"));
    }
    int resultsFd = createResultsFile();
    int errorsFd = createErrorsFile();
    int fd1 = open(argv[1], 'r');
    if (fd1 < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    char firstLine[150] = {0};
    char secondLine[150] = {0};
    char thirdLine[150] = {0};
    char str1[150] = {0};
    char str2[150] = {0};
    int index = 0;
    readLine(firstLine, fd1, &index);
    firstLine[index - 1] = 0;
    index = 0;
    firstLineValidation(firstLine);
    readLine(secondLine, fd1, &index);
    secondLine[index - 1] = 0;
    index = 0;
    secondLineValidation(secondLine);
    int fd2 = open(secondLine, O_RDONLY);
    if (fd2 < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    readLine(thirdLine, fd1, &index);
    thirdLine[index - 1] = 0;
    index = 0;
    thirdLineValidation(thirdLine);
    char *path1 = realpath(thirdLine, str1);
    int fd3 = open(thirdLine, O_RDONLY);
    if (fd3 < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    DIR *dir = opendir(firstLine);
    struct dirent *dirent;
    while ((dirent = readdir(dir)) != NULL) {
        char s[150] = {0};
        strcat(s, firstLine);
        strcat(s, "/");
        strcat(s, dirent->d_name);
        if (isADirectory(s) == 1) {
            executeUser(dirent, fd2, path1,  resultsFd, errorsFd, s);
        }
    }
    if (close(fd1) < 0 || close(fd2) < 0 || close(fd3) < 0 || close(errorsFd) < 0 ||
    close(resultsFd) < 0) {
        exit(-1);
    }
}