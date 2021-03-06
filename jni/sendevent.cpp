// Based on system/core/toolbox/sendevent.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>
#include <unistd.h> // usleep, write
#include "screen.h"

int main (int argc, char *argv[])
{
    const int verbose = 1;
    int dev_fd;
    FILE *input_file = NULL;
    int ret;
    int version;
    struct input_event event;
    float timestamp = -1, old_timestamp = -1;
    const size_t BUF_SIZE = 256;
    char buf[BUF_SIZE];
    int screen_id;

    if (argc != 4) {
        fprintf (stderr, "usage: %s screen_id powerbutton_id event_data\n", argv[0]);
        return 1;
    }

    screen_id = atoi(argv[2]);
    wait_for_screen (screen_id);
    sleep (2);

    if (strcmp (argv[1], "/dev/null") != 0)
    {
        sprintf (buf, "/dev/input/event%d", atoi(argv[1]));
    }
    else
    {
        strcpy (buf, argv[1]);
    }
    dev_fd = open (buf, O_RDWR);
    if (dev_fd < 0) {
        fprintf (stderr, "could not open device %s, %s\n", buf, strerror (errno));
        return 1;
    }
    if (strcmp (buf, "/dev/null") != 0 && ioctl (dev_fd, EVIOCGVERSION, &version)) {
        fprintf (stderr, "could not get driver version for %s, %s\n", buf, strerror (errno));
        return 1;
    }
    input_file = fopen (argv[3], "rb");
    if (!input_file) {
        fprintf (stderr, "could not open input file %s, %s\n", argv[3], strerror (errno));
        return 1;
    }
    while (true) {
        old_timestamp = timestamp;
        fgets (buf, BUF_SIZE, input_file);
        if (feof (input_file)) {
            if (verbose) {
                printf ("End of input\n");
            }
            break;
        }
        // for easier parsing
        for (size_t i = 0; i < BUF_SIZE; i++) {
            if (buf[i] == '\0') {
                break;
            }
            if (buf[i] == '[' || buf[i] == ']') {
                buf[i] = ' ';
            }
        }
        ret = sscanf (buf, "%f %hx %hx %x", &timestamp, &(event.type), &(event.code), &(event.value));
        if (ret != 4) {
            fprintf (stderr, "parsing failed\n");
            return 1;
        }
        if (verbose) {
            printf ("timestamp = %f, type = %hx, code = %hx, value = %hx\n", timestamp, event.type, event.code, event.value);
        }
        if (old_timestamp > 0) {
            if (verbose) {
                printf ("Waiting for %f seconds\n", timestamp - old_timestamp);
            }
            usleep ((timestamp - old_timestamp) * 1E6);
        }
        ret = write (dev_fd, &event, sizeof (event));
        if (ret != sizeof (event)) {
            fprintf (stderr, "write event failed, %s\n", strerror (errno));
            return -1;
        }
    }

    sleep (2);
    toggle_screen (screen_id);

    return 0;
}
