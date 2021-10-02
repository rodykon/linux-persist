#include <fcntl.h>
#include <unistd.h>

#define FILE_PATH ("/tmp/y0u_h4v3_b33n_pwn3d")

void _start(void)
{
    int fd = open(FILE_PATH, O_CREAT);
    _exit(close(fd));
}
