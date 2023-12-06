#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    char *addr1 = "127.168.198.111";
    char *addr2 = "127.199.198.256";

    unsigned long conv_addr = inet_addr(addr1);
    if(conv_addr == INADDR_NONE)
        printf("Error Occured!\n");
    else
        printf("NewWork Byte Order Integer Address: %#lx\n", conv_addr);
    
    conv_addr = inet_addr(addr2);
    if(conv_addr == INADDR_NONE)
        printf("Error Occured!\n");
    else
        printf("NewWork Byte Order Integer Address: %#lx\n", conv_addr);

    return 0;
}