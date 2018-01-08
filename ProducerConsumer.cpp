#include <iostream>
#include <queue>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <pthread.h> // POSIX Threads used. Compiled and tested in macOS Terminal.

using namespace std;

queue<char> Buff;
int buffSize = 20;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
bool timesUp = false;
ofstream outFile("output.txt");

void* Produce(void* prodNum)
{
    int producerNumber = *(int*)&prodNum;
    ostringstream os;
    os << producerNumber;
    string value = os.str(); // Example: Producer1 produces '1'
    while(true)
    {
        pthread_mutex_lock(&m);
        if(Buff.size() < buffSize)
        {
            Buff.push(value[0]);
            cout << "Producer_" << producerNumber << " produces element: " << value[0] << endl;
            outFile << "Producer_" << producerNumber << " produces element: " << value[0] << endl;
            pthread_cond_signal(&c);
        }
        pthread_mutex_unlock(&m);
        sleep(0); // Just to control the rate of production if needed
        if(timesUp)
            pthread_exit(0);
    }
}

void* Consume(void* consNum)
{
    int consumerNumber = *(int*)&consNum;
    while(true)
    {
        pthread_mutex_lock(&m);
        while(Buff.empty())    
            pthread_cond_wait(&c, &m); 
        pthread_mutex_unlock(&m);
        sleep(0); // Just to control the rate of consumption if needed
        pthread_mutex_lock(&m);
        cout << "Consumer_" << consumerNumber << " consumes element: " << Buff.front() << endl;
        outFile << "Consumer_" << consumerNumber << " consumes element: " << Buff.front() << endl;
        Buff.pop();
        pthread_mutex_unlock(&m);
        if(timesUp)
            pthread_exit(0);
    }
}

int main()
{
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&c, NULL);
    
    pthread_t Producer1;
    pthread_t Producer2;
    pthread_t Producer3;
    pthread_t Producer4;
    
    pthread_t Consumer1;
    pthread_t Consumer2;
    pthread_t Consumer3;
    pthread_t Consumer4;
    
    pthread_create(&Producer1, NULL, Produce, (void*) 1);
    pthread_create(&Producer2, NULL, Produce, (void*) 2);
    pthread_create(&Producer3, NULL, Produce, (void*) 3);
    pthread_create(&Producer4, NULL, Produce, (void*) 4);
    
    pthread_create(&Consumer1, NULL, Consume, (void*) 1);
    pthread_create(&Consumer2, NULL, Consume, (void*) 2);
    pthread_create(&Consumer3, NULL, Consume, (void*) 3);
    pthread_create(&Consumer4, NULL, Consume, (void*) 4);
    
    sleep(30); // To hold the main thread for 30 seconds;
    
    timesUp = true;
    
    pthread_join(Producer1, NULL);
    pthread_join(Producer2, NULL);
    pthread_join(Producer3, NULL);
    pthread_join(Producer4, NULL);
    
    pthread_join(Consumer1, NULL);
    pthread_join(Consumer2, NULL);
    pthread_join(Consumer3, NULL);
    pthread_join(Consumer4, NULL);
    
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&c);
    
    outFile.close();
    
    return 0;
}
