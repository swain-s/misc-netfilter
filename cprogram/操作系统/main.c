#include<stdio.h>
#include<pthread.h>
#define MAX 20
pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;

int buffer = 0;

void *producer(void *ptr)
{
    int i;
    
    for (i=1;i<=MAX;i++){
        pthread_mutex_lock(&the_mutex);
        printf("a lock!\n");
        while(buffer !=0)
        {
            pthread_cond_wait(&condp, &the_mutex);
            printf("waitfor b\n");
            return ptr;
        }
        buffer = i;
        printf("abuffer:%d\n", buffer);
        pthread_cond_signal(&condc);
        printf("aout\n");
        pthread_mutex_unlock(&the_mutex);
    }
    pthread_exit(0);
}

void *consumer(void *ptr)
{
    int i;
    
    for(i=1;i<MAX;i++){
        pthread_mutex_lock(&the_mutex);
        printf("--------b lock!\n");
        while (buffer ==0)
        {
            pthread_cond_wait(&condc, &the_mutex);
            printf("-------------waitfor a\n");
            return ptr;
        }
        buffer = 0;
        printf("------buffer:%d\n", buffer);
        pthread_cond_signal(&condp);
        printf("----------bout\n");
        pthread_mutex_unlock(&the_mutex);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t t1, t2;
    int thread1,thread2;
    pthread_mutex_init(&the_mutex, NULL);
    pthread_cond_init(&condp, NULL);
    pthread_cond_init(&condc, NULL);
    
    thread1 = pthread_create(&t1, NULL, (void *)&producer, (void *)&t2);
    thread2 = pthread_create(&t2, NULL, (void *)&consumer, (void *)&t1);
    printf("%d, %d\n", thread1, thread2);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    return 0;
}
