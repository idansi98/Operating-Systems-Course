//Idan Simai 206821258.
//I support the first option of the configuration file.
#include <pthread.h>
#include <fstream>
#include <string>
#include <unistd.h>
#include <semaphore.h>
#include <queue>
#include <iostream>

using namespace std;
class UBQ :public std::queue<string> {
protected:
    sem_t* used;
    sem_t* mutex;
public:
    explicit UBQ():queue<string>() {
        this->mutex = new sem_t();
        sem_init(this->mutex, 0, 1);
        this->used = new sem_t();
        sem_init(this->used, 0, 0);
    }
    //Get an item in the queue.
    void enqueue(const string& str) {
        sem_wait(mutex);
        queue<string>::push(str);
        sem_post(used);
        sem_post(mutex);
    }
    //Get an item out of the queue.
    string dequeue() {
        sem_wait(used);
        sem_wait(mutex);
        //Must use front and pop to save the string.
        string str = frontAndPop();
        sem_post(mutex);
        return str;
    }
    string frontAndPop() {
        string str = queue<string>::front();
        queue<string>::pop();
        return str;
    }
};

class BQ :public std::queue<string> {
protected:
    sem_t* used;
    sem_t* left;
    sem_t* mutex;
public:
    explicit BQ(int maxNum):queue<string>() {
        this->mutex = new sem_t();
        sem_init(this->mutex, 0, 1);
        this->used = new sem_t();
        sem_init(this->used, 0, 0);
        this->left = new sem_t();
        sem_init(this->left, 0, maxNum);
    }
    //Get an item in the queue.
    void enqueue(const string& str) {
        //Decrease the number of left indexes in the queue by 1.
        sem_wait(left);
        sem_wait(mutex);
        queue<string>::push(str);
        //Increase the number of used indexes in the queue by 1.
        sem_post(used);
        sem_post(mutex);
    }
    //Get an item out of the queue.
    string dequeue() {
        sem_wait(used);
        sem_wait(mutex);
        //Must use front and pop to save the string.
        string str = frontAndPop();
        sem_post(left);
        sem_post(mutex);
        return str;
    }
    string frontAndPop() {
        string str = queue<string>::front();
        queue<string>::pop();
        return str;
    }
};

typedef struct producer {
    int index;
    int numOfProducts;
    int queueSize;
    BQ* boundedQueue;
} producer;

vector<producer*> producers;
UBQ* sQueue;
UBQ* nQueue;
UBQ* wQueue;
BQ* coBQ;

//This function check if we are done getting the strings from the producer's queues to the 3 UBQ's.
bool checkIfDone(const int checkers [], unsigned long& len) {
    int counter = 0;
    for (int i = 0; i < len; ++i) {
        if (checkers[i] == 1) {
            counter++;
        }
    }
    if(counter == len) {
        return true;
    }
    return false;
}

void zerosInit (int checkers [], unsigned long& len) {
    for (int j = 0; j < len; j++) {
        checkers[len] = 0;
    }
}

//This function produces the news.
void* produce(void* p) {
    int  alreadyProd [3] = {0};
    int check;
    producer* prod = (producer*) p;
    for (int i = 0; i < prod->numOfProducts; i++) {
        string str;
        check = rand() % 3;
        if (check == 0) {
            str = "WEATHER";
        }
        if (check == 1) {
            str = "SPORTS";
        }
        if (check == 2) {
            str = "NEWS";
        }
        prod->boundedQueue->enqueue(string("Producer") + " " + to_string(prod->index) + " " + str +
                                 " " + to_string(alreadyProd[check]));
        alreadyProd[check] += 1;
    }
    prod->boundedQueue->enqueue("DONE");
    return nullptr;
}

//This function handles the dispatching of the news.
void* dispatch(void*) {
    unsigned long len = producers.size();
    int checkers [len];
    zerosInit(checkers, len);
    for (int i = 0; !checkIfDone(checkers, len);i = (i+1) % (int) len) {
        while(checkers[i] == 1) {
            i = (i + 1) % (int) len;
        }
        string str = producers[i]->boundedQueue->dequeue();
        if (str.find("WEATHER") != string::npos) {
            wQueue->enqueue(str);
        }
        if (str.find("SPORTS") != string::npos) {
            sQueue->enqueue(str);
        }
        if (str.find ("NEWS") != string::npos) {
            nQueue->enqueue(str);
        }
        if (str == "DONE") {
            checkers[i] = 1;
        }
    }
    wQueue->enqueue("-1");
    sQueue->enqueue("-1");
    nQueue->enqueue("-1");
    return nullptr;
}

//This function handles the editing of the news.
void* edit(void* queue) {
    string str;
    UBQ* ubQuque = (UBQ*) queue;
    usleep(100000);
    while (str != "-1") {
        str = ubQuque->dequeue();
        coBQ->enqueue(str);
    }
    return nullptr;
}

//This function handles the screen manager.
void* screenManage(void*) {
    int counter = 0;
    while (counter < 3) {
        string str = coBQ->dequeue();
        if (str == "-1") {
            counter++;
            continue;
        }
        cout << str << endl;
    }
    cout << "DONE" << endl;
    return nullptr;
}

//This function gets a number from a line.
int getNumFromALine(string line) {
    unsigned long len = line.length();
    int i = 0;
    for (; line[i] > 57 || line[i] < 48; i++) {
    }
    return atoi(line.substr(i, len - 1).c_str());
}

//This function reads the file, and initializes the producers.
void readFileAndInit(char* path, int& coEditorQueueSize) {
    string line;
    ifstream infile(path);
    int i = 1;
    do {
        getline(infile, line);
    } while (line.find("PRODUCER") == string::npos);
    while (line.find("PRODUCER") != string::npos) {
        producers.push_back(new producer());
        getline(infile, line);
        producers.back()->numOfProducts = getNumFromALine(line);
        getline(infile, line);
        producers.back()->queueSize = getNumFromALine(line);
        producers.back()->index = i;
        producers.back()->boundedQueue = new BQ(producers.back()->queueSize);
        getline(infile, line);
        getline(infile, line);
        i++;
    }
    coEditorQueueSize = getNumFromALine(line);
}

void startProducers() {
    for (producer* prod :producers) {
        pthread_t threadId;
        pthread_create(&threadId, nullptr,&produce, (void*)prod);
    }
}

void startDispatcher() {
    pthread_t threadId;
    pthread_create(&threadId, nullptr, &dispatch, nullptr);
}

void startCoEditors() {
        pthread_t threadId1;
        pthread_create(&threadId1, nullptr, &edit, (void*)wQueue);
        pthread_t threadId2;
        pthread_create(&threadId2, nullptr, &edit, (void*)sQueue);
        pthread_t threadId3;
        pthread_create(&threadId3, nullptr, &edit, (void*)nQueue);
}

pthread_t starScreenManager() {
    pthread_t threadId;
    pthread_create(&threadId, nullptr,&screenManage, nullptr);
    return threadId;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Wrong number of arguments was inserted!" << endl;
        exit(-1);
    }
    int coEditorQueueSize;
    readFileAndInit(argv[1], coEditorQueueSize);
    wQueue = new UBQ();
    sQueue = new UBQ();
    nQueue = new UBQ();
    coBQ = new BQ(coEditorQueueSize);
    startProducers();
    startDispatcher();
    startCoEditors();
    pthread_join(starScreenManager(), nullptr);
}