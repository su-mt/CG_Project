//INPUT.C

#include "include/geometry.h"
#include <stdlib.h>
#include <math.h>


extern double tx, ty, tz;
extern double rx, ry, rz;
extern double scale;

int type_of_project = PERSPECTIVE;

void event_loop(Display* display, Window window, GC gc)
{
    Pixmap pixmap = create_pixmap_buffer(display, window, WIDTH, HEIGHT);

    clear_pixmap(display, pixmap, gc, WIDTH, HEIGHT);
    draw_scene_pixmap(display, pixmap, gc, WIDTH, HEIGHT, DEFAULT_SIZE);
    copy_pixmap_to_window(display, window, gc, pixmap, WIDTH, HEIGHT);

    while (1) {
        XEvent e;
        XNextEvent(display, &e);

        if (e.type == Expose) {
            copy_pixmap_to_window(display, window, gc, pixmap, WIDTH, HEIGHT);
        }
        if (e.type == KeyPress) {
            KeySym key;
            char text[10];
            XLookupString(&e.xkey, text, sizeof(text), &key, NULL);

            switch (text[0]) {
                case 'w': ty += DELTA_TRANSLATION; break;
                case 's': ty -= DELTA_TRANSLATION; break;
                case 'a': tx -= DELTA_TRANSLATION; break;
                case 'd': tx += DELTA_TRANSLATION; break;
                case 'z': tz += DELTA_TRANSLATION; break;
                case 'x': tz -= DELTA_TRANSLATION; break;
                case 'q': ry += DELTA_ROTATION; break;
                case 'e': ry -= DELTA_ROTATION; break;
                case 'r': rx += DELTA_ROTATION; break;
                case 'f': rx -= DELTA_ROTATION; break;
                case ',': rz -= DELTA_ROTATION; break;
                case '.': rz += DELTA_ROTATION; break;
                case '+': scale += DELTA_SCALE; break;
                case '-': scale = fmax(MIN_SCALE, scale - DELTA_SCALE); break;
                case 'p': type_of_project = (type_of_project%3+1); break;
                case 'Q': 
                    XFreePixmap(display, pixmap);
                    return;
                default: continue;
            }

            printf("tx=%.2f ty=%.2f tz=%.2f | rx=%.2f ry=%.2f rz=%.2f | scale=%.2f | type_of_project=%d\n", 
                   tx, ty, tz, rx, ry, rz, scale, type_of_project);

            clear_pixmap(display, pixmap, gc, WIDTH, HEIGHT);
            draw_scene_pixmap(display, pixmap, gc, WIDTH, HEIGHT, DEFAULT_SIZE);
            copy_pixmap_to_window(display, window, gc, pixmap, WIDTH, HEIGHT);
        }
    }
}