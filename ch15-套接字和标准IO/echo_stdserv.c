#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_sz;
    char buf[BUF_SIZE];

    if(argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_sz = sizeof(clnt_addr);
    for(int i = 0; i < 5; i++)
    {
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, 
            &clnt_addr_sz);
        if(clnt_sock == -1)
            error_handling("accept() error");
        else
            puts("New Client connected...");

        FILE* readfp = fdopen(clnt_sock, "r");
        FILE* writefp = fdopen(clnt_sock, "w");

        while(!feof(readfp))
        {
            fgets(buf, BUF_SIZE, readfp);
            fputs(buf, writefp);
            fflush(writefp);
        }

        fclose(readfp);
        fclose(writefp);
    }

    close(serv_sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}