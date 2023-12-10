#include <stdio.h>
#include <sys/uio.h>
#include <string.h>

int main(int argc, char* agrv[])
{
    struct iovec vec[2];
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";
    int str_len;

    vec[0].iov_base = buf1;
    vec[0].iov_len = sizeof(buf1);
    vec[1].iov_base = buf2;
    vec[1].iov_len = strlen(buf2);

    str_len = writev(1, vec, 2);
    puts("");
    printf("Write %d bytes\n", str_len);

    return 0;
}