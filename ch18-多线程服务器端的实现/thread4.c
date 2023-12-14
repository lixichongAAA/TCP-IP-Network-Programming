#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREAD 100

void* thread_inc(void* arg);
void* thread_dec(void* arg);

long long num = 0;

int main(int argc, char* argv[])
{
    pthread_t thread_id[NUM_THREAD];

    printf("sizeof long long: %ld\n", sizeof(long long));
    for(int i = 0; i < NUM_THREAD; i++)
    {
        if(i&1)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_dec, NULL);
    }

    for(int i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("Result: %lld\n", num);

    return 0;
}

void* thread_inc(void* arg)
{
    for(int i = 0; i < 5000000; i++)
        num += 1;
    return NULL;
}

void* thread_dec(void* arg)
{
    for(int i = 0; i < 5000000; i++)
        num -= 1;
    return NULL;
}