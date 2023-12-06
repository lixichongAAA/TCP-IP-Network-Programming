#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    unsigned short host_port = 0x1234;
    unsigned short net_port;
    unsigned long host_addr = 0x12345678;
    unsigned long net_addr;

    net_port = htons(host_port);
    net_addr = htonl(host_addr);

    printf("Host Byte Order Port: %#x\n", host_port);
    printf("NewWork Byte Order Port: %#x\n", net_port);
    printf("Host Byte Order Address: %#lx\n", host_addr);
    printf("NewWork Byte Order Address: %#lx\n", net_addr);

    return 0;
}