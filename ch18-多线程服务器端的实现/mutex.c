#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100

void *thread_dec(void *arg);
void *thread_inc(void *arg);

long long num = 0;
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < NUM_THREAD; i++)
    {
        if (i & 1)
            pthread_create(&(thread_id[i]), NULL, thread_dec, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
    }

    for (int i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("Result: %lld\n", num);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *thread_dec(void *arg)
{

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 5000000; i++)
        num += 1;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *thread_inc(void *arg)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 5000000; i++)
        num -= 1;
    pthread_mutex_unlock(&mutex);

    return NULL;
}