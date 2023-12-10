#include <stdio.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    int fd = open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);
    if(fd == -1)
    {
        fputs("file open error", stderr);
        return -1;
    }

    FILE* fp = fdopen(fd, "w");
    fputs("I Like ....\n", fp);
    fclose(fp);

    return 0;
}