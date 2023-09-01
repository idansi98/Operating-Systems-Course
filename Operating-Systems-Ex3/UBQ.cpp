#include "UBQ.h"
using namespace std;

void UBQ::push(string str) {
    sem_wait(mutex);
    queue<string>::push(str);
    sem_post(used);
    sem_post(mutex);
}

string UBQ::pop() {
    sem_wait(used);
    sem_wait(mutex);
    string str = queue<string>::front();
    queue<string>::pop();
    sem_post(mutex);
    return str;
}