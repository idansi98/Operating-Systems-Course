//Idan Simai 206821258
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

//Function to check the validation of the input.
int checkValidation(char* argv[]) {
    int i = 0, j = 1;
    for (;j < 5; j++) {
        unsigned long len = strlen(argv[j]);
        if (argv[j][i] == '-') {
            i++;
        }
        for (; i < len; i++) {
            if (argv[j][i] < '0' || argv[j][i] > '9') {
                return 0;
                }
          }
    }
    return 1;
}

//Function to remove the Client's file.
int removeClientFile () {
    char str1 [500] = {0}, str2 [500] = {0};
    strcat(str1, "to_client_");
    sprintf(str2, "%d", (int) getpid());
    strcat(str1, str2);
    if (remove(str1) < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    return 0;
}

//Function to remove thr Server's file.
int removeServerFile () {
    if (remove("to_srv") < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    return 0;
}

//Function to Write to the server file' so we could send the signal to it.
int writeToServerFile(char* pid, char* firstNum, char* operation, char* secondNum) {
    int fd = open("to_srv", O_CREAT | O_WRONLY, 0777);
    if (fd < 0) {
        return -1;
    }
    close(fd);
    FILE* fp;
    fp = fopen ("to_srv", "w");
    if ((fprintf(fp, "%s %s %s %s", pid, firstNum, operation, secondNum)) < 0) {
        if (fclose(fp) < 0) {
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        }
        removeServerFile();
        exit(-1);
    }
    fclose(fp);
    return 0;
}

//This function loops 10 times so the client tries to open and write to the file.
void clientLoop(char* argv []) {
    int i = 0, wasEntered = 0;
    for (; i <= 9; ++i) {
        if (access("to_srv", F_OK) != 0) {
            char clientPid[500];
            sprintf(clientPid, "%d", (int) getpid());
            int checker = writeToServerFile(clientPid, argv[2], argv[3], argv[4]);
            if (checker >= 0) {
                if (kill(atoi(argv[1]), SIGUSR1) < 0) {
                    removeServerFile();
                    exit(-1);
                }
                wasEntered = 1;
                alarm(30);
                pause();
            }
            if (checker < 0) {
                sleep(1 + rand() % 5);
                continue;
            }
        }
        sleep(1 + rand() % 5);
    }
    //If we entered the signal sending;
    if (wasEntered == 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
}

//This function prints the results from the server file.
void printResultsFromServer() {
    char str1 [500] = {0}, str2 [500] = {0}, str3 [500] = {0};
    sprintf(str1, "%d", (int) getpid());
    strcpy(str2, "to_client_");
    strcat(str2, str1);
    int fd = open(str2, O_RDONLY);
    if (fd < 0 || read(fd, str3, 500) < 0) {
        removeClientFile();
        exit(-1);
    }
    printf("%s\n", str3);
    removeClientFile();
    exit(0);
}

//The Signal handler.
void clientGotSignal() {
    alarm(0);
    printResultsFromServer();
}

//The alarm handler.
void clientGotAlarm() {
    printf("Client closed because no response was received from the server for 30 seconds\n");
    exit(-1);
}

int main(int argc, char* argv[]) {
    signal(SIGUSR1, clientGotSignal);
    signal(SIGALRM, clientGotAlarm);
    if (argc != 5 ) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    if (checkValidation(argv) != 1) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    if ((atoi(argv[3]) > 4) || (atoi(argv[3]) < 1)) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    clientLoop(argv);
}