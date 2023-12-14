#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define NAME_SIZE 20
#define BUF_SIZE 100

void* rcv_msg(void* arg);
void* snd_msg(void* arg);
void error_handling(char* message);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t id_snd, id_rcv;
    void* thread_rt;
    
    if(argc != 4)
    {
        printf("Usage: %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    
    pthread_create(&id_snd, NULL, snd_msg, (void*)&sock);
    pthread_create(&id_rcv, NULL, rcv_msg, (void*)&sock);
    pthread_join(id_snd, &thread_rt);
    pthread_join(id_rcv, &thread_rt);
    close(sock);

    return 0;
}

void* snd_msg(void* arg)
{
    int sock = *((int *)arg);
    char name_msg[BUF_SIZE + NAME_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;    
}

void* rcv_msg(void* arg)
{
    int sock = *((int*)arg);
    char name_msg[BUF_SIZE + NAME_SIZE];
    int str_len;
    while(1)
    {
        str_len = read(sock, name_msg, BUF_SIZE + NAME_SIZE - 1);
        if(str_len == -1)
            return (void*)-1;
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    
    return NULL;
}

void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}