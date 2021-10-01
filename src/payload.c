#include <stdio.h>

#define FILE_PATH ("/tmp/y0u_h4v3_b33n_pwn3d")

int main(int argc, char *argv[])
{
    FILE *file = fopen(FILE_PATH, "w");
    return fclose(file);
}
