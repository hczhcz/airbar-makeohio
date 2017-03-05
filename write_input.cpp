#include <cstdio>

#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("input", O_WRONLY);

    while (true) {
        char c;
        read(fd, &c, sizeof(c));
        write(fd, &c, sizeof(c));
    }

    return 0;
}
