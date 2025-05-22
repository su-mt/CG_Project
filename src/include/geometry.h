#ifndef GEOMETRY_H

#define GEOMETRY_H

#include <stdio.h>

#include "X11/Xlib.h"
#include "X11/keysym.h"
#include "X11/Xutil.h"

#define M_PI 3.1459


#define PERSPECTIVE 1
#define ORTOGRAPHIC 2
#define ISOMETRIC   3

#define WIDTH 800
#define HEIGHT 600

typedef struct Point3D{
    double x,y,z;
}Point3D;  


typedef struct {
    int x, y;
} Point2D;

typedef struct Color {
    unsigned char r, g, b;
} Color;

typedef struct {
    Point3D v[3]; // треугольник
} Triangle;



Point3D transform_point(Point3D p);
Point2D orto_project(Point3D p, int width, int height);
Point2D perspective_project(Point3D p, int width, int height, double fov, double viewer_distance);
Point2D project(Point3D p, int width, int height, int type_of_project);
void draw_scene(Display* display, Window window, GC gc, int width, int height, double size);
void event_loop(Display* display, Window window, GC gc);
void init_x11_window(int width, int height, const char* title); 
void draw_cube(Display* display, Window window, GC gc, int width, int height, double size);
void draw_tetrahedron(Display* display, Window window, GC gc, int width, int height, double size);
void clearBuffers();
void draw_triangle_zbuffer(Display* display, Window window, GC gc, Triangle tri, int width, int height, Color Color);

#endif //GEOMETRY_h