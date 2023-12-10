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
    int serv_sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];

    if(argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    else
        puts("Connected...");
    
    FILE* readfp = fdopen(serv_sock, "r");
    FILE* writefp = fdopen(serv_sock, "w");

    while(1)
    {
        fputs("Input message(q/Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        
        fputs(message, writefp);
        fflush(writefp);
        fgets(message, BUF_SIZE, readfp);
        printf("Message from server: %s\n", message);
    }

    fclose(writefp);
    fclose(readfp);

    return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}