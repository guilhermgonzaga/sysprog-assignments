/* hello1.c and hello2.S: calling x86 assembly from C and vice versa */

#include <stdio.h>

int value = 42;
int timestwo(int);

int main(int argc, char *argv[])
{
    printf("%d\n", timestwo(2));
    return 0;
}
