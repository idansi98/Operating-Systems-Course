//Idan Simai 206821258
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

//Check if a ' ' or '\n'.
int isSpaceOrLineFeed(char buff) {
    if(buff == ' ' || buff == '\n') {
        return 1;
    }
    return 0;
}

//The loop to read the file while using the isSpaceOrLineFedd function.
void readingLoop(int fd, char* buff) {
    while(isSpaceOrLineFeed(*buff)) {
        int read1 = read(fd, buff, 1);
        if(read1 < 0) {
            write(2, "Error in: read\n", sizeof("Error in: read\n"));
            exit(-1);
        }
        if (read1 == 0) {
            return;
        }
    }
}

//Check if a specific char is a letter.
int isALetter(char buff) {
    if((buff >= 'a' && buff <= 'z' ) || (buff >= 'A' && buff <= 'Z' )) {
        return 1;
    }
    return 0;
}

//Check if a Capital letter.
int isCapital(char buff) {
    if (buff >= 'A' && buff <= 'Z') {
        return 1;
    }
    return 0;
}

//Check if the letter is a capital of the letter or the opposite.
int isUpperCase(char buff1, char buff2) {
    if (isCapital(buff1) && isCapital(buff2)) {
        return 0;
    }
    if (isCapital(buff1) && !isCapital(buff2)) {
        if (buff2 - buff1 == 32) {
            return 1;
        }
        return 0;
    }
    if (isCapital(buff2) && !isCapital(buff1)) {
        if (buff1 - buff2 == 32) {
            return 1;
        }
        return 0;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int flag = 0;
    int value = 0;
    //Check enough parameters.
    if (argc != 3) {
        write(2, "Not enough parameters!\n", sizeof("Not enough parameters!\n"));
        exit(-1);

    }
    //Open the 2 files and get their file descriptors, if failed print error.
    int fileDescriptor1 = open(argv[1], 'r');
    if (fileDescriptor1 < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    int fileDescriptor2 = open(argv[2], 'r');
    if (fileDescriptor2 < 0) {
        write(2, "Error in: open\n", sizeof("Error in: open\n"));
        exit(-1);
    }
    char buff1 = 0, buff2 = 0, buff3 = 0, buff4 = 0;
    while (1) {
        //Read the 2 files byte by byte to buffers, if failed print an error.
        buff3 = buff1;
        buff4 = buff2;
        int read1 = read(fileDescriptor1, &buff1, 1);
        if (read1 < 0) {
            write(2, "Error in: read\n", sizeof("Error in: read\n"));
            exit(-1);
        }
        int read2 = read(fileDescriptor2, &buff2, 1);
        if (read2 < 0) {
            write(2, "Error in: read\n", sizeof("Error in: read\n"));
            exit(-1);
        }

        if (read1 == 0 && read2 == 0) {
            if (flag == 0) {
                value = 1;
                break;
            }
            value = 3;
            break;
        }
        if (read1 == 0) {
            if ((buff3 == buff4) || (((isALetter(buff3)) && (isALetter(buff4))) &&
                (isUpperCase(buff3, buff4) || isUpperCase(buff4, buff3)))) {
                readingLoop(fileDescriptor2, &buff2);
                if (isSpaceOrLineFeed(buff2)) {
                    value = 3;
                    break;
                }
                value = 2;
                break;
            }
            readingLoop(fileDescriptor2, &buff2);
            if (buff2 != buff1) {
                value = 2;
                break;
            }
            continue;
        }
        if (read2 == 0) {
            if ((buff4 == buff3) || (((isALetter(buff3)) && (isALetter(buff4))) &&
                (isUpperCase(buff3, buff4) || isUpperCase(buff4, buff3)))) {
                readingLoop(fileDescriptor1, &buff1);
                if (isSpaceOrLineFeed(buff1)) {
                    value = 3;
                    break;
                }
                value = 2;
                break;
            }
            readingLoop(fileDescriptor1, &buff1);
            if (buff1 != buff2) {
                value = 2;
                break;
            }
            continue;
        }

        if (buff1 != buff2) {
            flag = 1;
            if (isSpaceOrLineFeed(buff1)) {
                readingLoop(fileDescriptor1, &buff1);
            }
            if (isSpaceOrLineFeed(buff2)) {
                readingLoop(fileDescriptor2, &buff2);
            }
            if (buff1 == buff2) {
                continue;
            }
            if (isALetter(buff1) && isALetter(buff2)) {
                if (isUpperCase(buff1, buff1) || isUpperCase(buff2, buff1)) {
                    continue;
                }
            }
            int close1 = close(fileDescriptor1);
            if (close1 < 0) {
                exit(-1);
            }
            int close2 = close(fileDescriptor2);
            if (close2 < 0) {
                exit(-1);
            }
            value = 2;
            break;
        }
    }
    return value;
}
