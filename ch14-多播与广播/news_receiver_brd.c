#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 #include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void setnonblockingmode(int fd);

int main(int argc, char* argv[])
{
    int recv_sock;
    struct sockaddr_in addr;
    int str_len;
    char buf[BUF_SIZE];

    if(argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(recv_sock == -1)
        error_handling("socket() error");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    if(bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        error_handling("bind() error");
    
    setnonblockingmode(recv_sock);
    while(1)
    {
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0,
            NULL, 0);
        
        printf("%d\n", errno);
        printf("str_len: %d\n", str_len);
        if(str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }

    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void setnonblockingmode(int fd)
{
	int flag=fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}