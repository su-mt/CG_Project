//MAIN.C

#include <stdio.h>
#include <stdlib.h>

#include "include/geometry.h"



Display* display;
Window window;
GC gc;
int screen;

void init_x11_window(int width, int height, const char* title) 
{
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Не удалось запустить X11");
        exit(1);
    }


    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        100, 100, width, height,
        1,
        BlackPixel(display, screen),
        WhitePixel(display, screen)
    );

    XStoreName(display, window, title);

    XSelectInput(display, window, ExposureMask | KeyPressMask);

    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, NULL);
    XColor green;
    Colormap colormap = DefaultColormap(display, 0);

    // Определяем зелёный цвет (значения RGB от 0 до 65535)
    green.red = 53*256;
    green.green = 122*256;
    green.blue = 32*256;
    green.flags = DoRed | DoGreen | DoBlue;

    XAllocColor(display, colormap, &green);
    XSetForeground(display, gc, green.pixel);
    //XSetForeground(display, gc, BlackPixel(
    //XSetForeground(display, gc, BlackPixel(display, screen));
}



int main() {
    init_x11_window(800, 600, "3D Transformations");
    event_loop(display,window, gc);


    // Очистка ресурсов
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
