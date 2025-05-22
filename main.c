
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    Display *display;
    Window window;
    int screen;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Не удалось подключиться к X-серверу\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 10, 10, 500, 300, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            XDrawString(display, window, DefaultGC(display, screen),
                        50, 50, "Hello, World!", strlen("Hello, World!"));
        }
        if (event.type == KeyPress)
            break;


    }

    XCloseDisplay(display);
    return 0;
}
