#include <iostream>
#include <queue>
#include <string>
#include <mutex>
#include <semaphore.h>
#include <vector>
#include "BQ.h"
using namespace std;

void BQ::push(string str) {
    sem_wait(left);
    sem_wait(mutex);
    queue<string>::push(str);
    sem_post(used);
    sem_post(mutex);
}

string BQ::pop() {
    sem_wait(used);
    sem_wait(mutex);
    string str = queue<string>::front();
    queue<string>::pop();
    sem_post(left);
    sem_post(mutex);
    return str;
}