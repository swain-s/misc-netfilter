#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

pthread_t t1,t2;
pthread_mutex_t mutex;
sem_t sem_full, sem_empty;

int buffer = 0;

void *producer()
{
    while(1){
        sem_wait(&sem_empty);
        pthread_mutex_lock(&mutex);
        buffer = 1;
        print("%d", buffer);
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full);
    }
}

void *coustem()
{
    while(1){
        sem_wait(&sem_full);
        pthread_mutex_lock(&mutex);
        buffer = 0;
        print("---------%d", buffer);
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_empty);
    }
}

int main(){
    sem_init(&sem_full);
    sem_init(&sem_empty);
    int a = pthread_mutex_init(&mutex);
    int b = pthread_creat(&t1, producer);
    int c = pthread_creat(&t2, coustem);
    
    pthread_join(t1);
    pthread_join(t2);
}
