#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    char str1[] = "Hi~\n";
    char str2[] = "It's a nice day~~\n";

    int cfd1 = dup(1);
    int cfd2 = dup2(1, 7);

    printf("cfd1: %d cfd2: %d\n", cfd1, cfd2);
    write(cfd1, str1, sizeof(str1));
    write(cfd2, str2, sizeof(str2));
    close(cfd1);
    close(cfd2);

    write(1, str1, sizeof(str1));
    close(1);
    write(1, str2, sizeof(str2)); // 这一行应该不会被输出

    return 0;
}