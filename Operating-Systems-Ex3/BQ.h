#ifndef OSEX3_BQ_H
#define OSEX3_BQ_H
#include <iostream>
#include <queue>
#include <string>
#include <mutex>
#include <semaphore.h>
#include <vector>

using namespace std;
class BQ :public std::queue<string> {
protected:
    sem_t* used;
    sem_t* left;
    sem_t* mutex;
public:
    explicit BQ(int maxNum):queue<string>() {
        this->mutex = new sem_t();
        this->used = new sem_t();
        this->left = new sem_t();
        sem_init(this->left, 0, maxNum);
        sem_init(this->used, 0, 0);
        sem_init(this->mutex, 0, 1);
    }
    void push(string str);
    string pop();
};


#endif //OSEX3_BQ_H
