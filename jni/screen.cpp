#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdio.h> // sprintf()

// XXX Not thread-safe
const char* power_button_path(int screen_id)
{
    static char buf[64];
    sprintf (buf, "/dev/input/event%d", screen_id);
    return buf;
}

void write_event (int fd, uint16_t type, uint16_t code, int32_t value)
{
    struct input_event event;
    event.type = type;
    event.code = code;
    event.value = value;
    write (fd, &event, sizeof (event));
}

void toggle_screen (int screen_id)
{
    // simulate the power button
    // Observed from getevent
    int power_fd = open (power_button_path(screen_id), O_RDWR);

    write_event (power_fd, 1, 0x74, 1);
    write_event (power_fd, 0, 0, 0);
    write_event (power_fd, 1, 0x74, 0);
    write_event (power_fd, 0, 0, 0);

    close (power_fd);
}

void wait_for_screen (int screen_id)
{
    int power_fd = open (power_button_path(screen_id), O_RDWR);
    struct input_event event;
    read (power_fd, &event, sizeof (event));
}
