//Idan Simai 206821258
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

//The Handler of the Alarm.
void serverGotAlarm() {
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    while(wait(NULL) != -1);
    exit(-1);
}

//Function to check if was divided by 0.
int isDivisionBy0 (int operation, int num) {
    if (operation == 4 && num == 0) {
        return 1;
    }
    return 0;
}

//Function to remove the client file.
int removeClientFile(char* str) {
    if (remove(str) < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    return 0;
}

//Function to remove the server file.
int removeServerFile() {
    if (remove("to_srv") < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    return 0;
}

//Function check if the server file exists and delete it.
void checkAndDeleteServerFile() {
    if (access("to_srv", F_OK) == 0) {
        removeServerFile();
    }
    alarm(60);
}

//Function to insert the invalid(division by 0).
void insertInvalidResult(char* pid) {
    char str1 [500] = {0}, str2 [500] = {0};
    strcat(str1, "to_client_");
    strcat(str1, pid);
    int fd = open(str1, O_CREAT |  O_WRONLY, 0777);
    if (fd < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    strcpy(str2, "CANNOT_DIVIDE_BY_ZERO");
    if ((write(fd, str2, strlen(str2))) < 0) {
        if (close(fd) < 0) {
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        }
        removeClientFile(str1);
        exit(-1);
    }
    close(fd);
}

//Function to insert the results(no Division by 0).
void insertValidResult(char* pid, int firstNum, int operation, int secondNum) {
    int result;
    if (operation == 1) {
        result = firstNum + secondNum;
    }
    if (operation == 2) {
        result = firstNum - secondNum;
    }
    if (operation == 3) {
        result = firstNum * secondNum;
    }
    if (operation == 4) {
        result = firstNum / secondNum;
    }
    char str1 [500] = {0}, str2 [500] = {0};
    sprintf(str2, "%d", result);
    strcat(str1, "to_client_");
    strcat(str1, pid);
    int fd = open(str1, O_CREAT |  O_WRONLY, 0777);
    if (fd < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    if ((write(fd, str2, strlen(str2))) < 0) {
        if (close(fd) < 0) {
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        }
        removeClientFile(str1);
        exit(-1);
    }
    close(fd);
}

//The handler for the Signal.
void serverGotSignal() {
    int pid = fork();
    if (pid < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    //The child.
    if (pid == 0) {
        if (access("to_srv", F_OK) != 0) {
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        }
        FILE* fp;
        char line1 [500] = {0}, line2 [500] = {0}, line3 [500] = {0}, line4 [500] = {0}, str [500] = {0};
        strcpy(str, "to_client_");
        fp = fopen("to_srv", "r");
        if (fp == NULL) {
            printf("ERROR_FROM_EX4\n");
            removeServerFile();
            exit(-1);
        }
        fscanf(fp, "%s %s %s %s", line1, line2, line3, line4);
        removeServerFile();
        int clientPid = atoi(line1);
        int firstNum = atoi(line2);
        int operation = atoi(line3);
        int secondNum = atoi(line4);
        if (isDivisionBy0(operation, secondNum) == 1) {
            insertInvalidResult(line1);
        }
        if (isDivisionBy0(operation, secondNum) == 0) {
            insertValidResult(line1, firstNum, operation, secondNum);
        }
        if (kill(clientPid, SIGUSR1) < 0) {
            removeClientFile(strcat(str, line1));
            exit (-1);
        }
        exit(0);
    }
    //Back to the father.
    else {
        alarm(60);
        signal(SIGUSR1, serverGotSignal);
    }
}

int main() {
    signal(SIGUSR1, serverGotSignal);
    signal(SIGALRM, serverGotAlarm);
    signal(SIGCHLD, SIG_IGN);
    checkAndDeleteServerFile();
    while (1) {
        pause();
    }
}

