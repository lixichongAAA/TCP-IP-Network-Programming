#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

void error_handling(char *message);

int main(int argc, char* argv[])
{
    int sock;
    int state;
    int opt_val;
    socklen_t opt_len = sizeof(opt_val);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    state = getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, &opt_len);
    if(state)
        error_handling("getsockopt() error");

    printf("TCP_NODELAY: %d\n", opt_val);

    opt_val = 1;
    state = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, opt_len);
    if(state)
        error_handling("setsockopt() error");
    state = getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, &opt_len);
    if(state)
        error_handling("getsockopt() error");

    printf("After setting, the value of TCP_NODELAY is: %d\n", opt_val);
    
    return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}