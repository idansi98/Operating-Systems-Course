//Idan Simai 206821258

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAXLENGTH 100
#define MAXNUM 100
#define PATHSNUM 5000

struct HISTORY {
    char command [MAXLENGTH];
    int pid;
};
typedef struct HISTORY History;

//This function prints the prompt just like the original shell.
void printPrompt() {
    printf("$ ");
    fflush(stdout);
}

//This function ss the input into the right tokens.
char** getCommandTokens(char* tokens [MAXLENGTH], char* input) {
    char* token = strtok(input, " ");
    int index = 0;
    //We use strtok to split by the " " delimiter.
    while (token != NULL)
    {
        tokens[index] = token;
        token = strtok(NULL, " ");
        index++;
    }
    tokens[index] = NULL;
    return tokens;
}

//Execute the cd command.
void executeCd(char* token) {
    chdir(token);
}

//This function prints the history of the entered command and their pid.
void historyPrinter(History histories[MAXNUM], int index) {
    int i = 0;
    for (; i <= index; ++i) {
        printf("%d %s\n", histories[i].pid, histories[i].command);
    }
}

//This function determines whether a Built-In command was entered or not.
int determineBuiltIn(char* tokens [MAXLENGTH], History histories[MAXNUM], int index) {
    if(strcmp("cd", tokens[0]) == 0) {
        executeCd(tokens[1]);
        histories[index].pid = getpid();
        return 1;
    }
    if(strcmp("exit", tokens[0]) == 0) {
        exit(0);
    }
    if(strcmp("history", tokens[0]) == 0) {
        histories[index].pid = getpid();
        historyPrinter(histories, index);
        return 1;
    }
    return 0;
}

//This function executes the entered command.
void commandExecute(char* tokens [MAXLENGTH], History histories[MAXNUM], int index) {
    if(determineBuiltIn(tokens, histories, index)) {
        return;
    }
    //In case of not Bulit-In command was entered, we create a new process.
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    }
    else if (pid == 0) {
        if (execvp(tokens[0], tokens) < 0) {
            perror("exec failed");
            exit(-1);
        }
    } else {
        histories[index].pid = pid;
        wait(NULL);
    }
}

//This function gets the input from the user.
char* getInput(History histories [MAXNUM], int index) {
    printPrompt();
    char input [MAXLENGTH];
    scanf(" %[^\n]s", input);
    strcpy(histories[index].command, input);
    return histories[index].command;
}

//This function updates the "PATH" environment variable.
void updatePathEnvVariable(int argc, char* argv[]) {
    int i = 1;
    char* PATHValue = getenv("PATH");
    char pathsArray [PATHSNUM];
    //We use strcat to contcate the two strings.
    strcpy(pathsArray, PATHValue);
    for (; i < argc; ++i) {
        strcat(pathsArray, ":");
        strcat(pathsArray, argv[i]);
    }
    setenv("PATH", pathsArray, 1);
}

int main(int argc, char* argv[]) {
  History hisories [MAXNUM];
  char input [MAXLENGTH] = {0};
  int index = 0;
  updatePathEnvVariable(argc, argv);
  while(1) {
    strcpy(input, getInput(hisories, index));
    char** tokens = malloc( MAXLENGTH * sizeof(char*));
    tokens  = getCommandTokens(tokens, input);
    commandExecute(tokens, hisories, index);
    free(tokens);
    index++;
  }
}