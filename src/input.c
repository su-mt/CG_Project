//INPUT.C

#include "include/geometry.h"
#include <stdlib.h>
#include <math.h>


extern double tx, ty, tz;
extern double rx, ry, rz;
extern double scale;

int type_of_project = PERSPECTIVE;

void event_loop(Display* display, Window window, GC gc) {
    const int width  = 800;
    const int height = 600;
    const double delta_t = 10.0;           // пикселей для смещения
    const double delta_r = M_PI / 60.0;    // ~3° поворота
    const double delta_s = 0.1;            // изменение масштаба

    // Первичная отрисовка
    XClearWindow(display,window);
    draw_scene(display, window, gc, width, height, 200.0);
    

    while (1) {
        XEvent e;
        XNextEvent(display, &e);

        if (e.type == Expose) {
            XClearWindow(display,window);
            draw_scene(display, window, gc, width, height, 200.0);
        }
        if (e.type == KeyPress) {
            KeySym key;
            char text[10];
            XLookupString(&e.xkey, text, sizeof(text), &key, NULL);

            switch (text[0]) {
                // смещение
                case 'w': ty += delta_t; break;
                case 's': ty -= delta_t; break;
                case 'a': tx -= delta_t; break;
                case 'd': tx += delta_t; break;
                case 'z': tz += delta_t; break;
                case 'x': tz -= delta_t; break;
                // вращение
                case 'q': rx += delta_r; break;
                case 'e': ry += delta_r; break;
                case 'f': rz += delta_r; break;
                // масштаб
                case '+': scale += delta_s; break;
                case '-': scale = fmax(0.1, scale - delta_s); break;

                case 'p': type_of_project = (type_of_project%3+1); break;
                // выход
                case 'Q': return;
                default: continue;
            }

            XClearWindow(display, window);
            printf("tx=%.2f ty=%.2f tz=%.2f | rx=%.2f ry=%.2f rz=%.2f | scale=%.2f | type_of_project=%d\n", 
    tx, ty, tz, rx, ry, rz, scale, type_of_project);

            draw_scene(display, window, gc, width, height, 200.0);
        }
    }
}