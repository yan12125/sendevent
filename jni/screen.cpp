#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

const char* POWER_BUTTON_PATH = "/dev/input/event5";

void write_event (int fd, uint16_t type, uint16_t code, int32_t value)
{
    struct input_event event;
    event.type = type;
    event.code = code;
    event.value = value;
    write (fd, &event, sizeof (event));
}

void toggle_screen ()
{
    // simulate the power button
    // Observed from getevent
    int power_fd = open (POWER_BUTTON_PATH, O_RDWR);

    write_event (power_fd, 1, 0x74, 1);
    write_event (power_fd, 0, 0, 0);
    write_event (power_fd, 1, 0x74, 0);
    write_event (power_fd, 0, 0, 0);

    close (power_fd);
}

void wait_for_screen ()
{
    int power_fd = open (POWER_BUTTON_PATH, O_RDWR);
    struct input_event event;
    read (power_fd, &event, sizeof (event));
}
