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

#define INF 10000000
#define DEFAULT_SIZE 200.0
#define CAMERA_TY -200.0
#define CAMERA_TZ -400.0
#define CAMERA_RX (-M_PI/3)
#define CAMERA_RY 0.0
#define CAMERA_RZ 0.0
#define DEFAULT_SCALE 1.0
#define FOV 750.0
#define VIEWER_DISTANCE 1000.0
#define LIGHT_Z 1000.0
#define PYRAMID_FACTOR_XY 1.75
#define PYRAMID_FACTOR_Z 2.0
#define PYRAMID_OFFSET_Z 10.0
#define SHADOW_COLOR_R 50
#define SHADOW_COLOR_G 50
#define SHADOW_COLOR_B 50
#define CUBE_COLOR_R 0
#define CUBE_COLOR_G 0
#define CUBE_COLOR_B 255
#define PYRAMID_COLOR_R 255
#define PYRAMID_COLOR_G 0
#define PYRAMID_COLOR_B 0
#define DELTA_TRANSLATION 10.0
#define DELTA_ROTATION (M_PI/60.0)
#define DELTA_SCALE 0.1
#define MIN_SCALE 0.1
#define ISO_ANGLE (M_PI/6)

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


Point2D isometric_project(Point3D p, int width, int height);
void draw_shadow(Display* display, Window window, GC gc, Triangle tri, Point3D light, int width, int height, double size);
Point3D project_to_ground(Point3D p, Point3D light, double ground_z);
Point3D transform_point(Point3D p);
Point2D orto_project(Point3D p, int width, int height);
Point2D perspective_project(Point3D p, int width, int height, double fov, double viewer_distance);
Point2D project(Point3D p, int width, int height, int type_of_project);
Point3D project_to_zx_plane(Point3D p, Point3D light, double ground_y);
void draw_scene(Display* display, Window window, GC gc, int width, int height, double size);
void event_loop(Display* display, Window window, GC gc);
void init_x11_window(int width, int height, const char* title); 
void draw_cube(Display* display, Window window, GC gc, int width, int height, double size);
void draw_tetrahedron(Display* display, Window window, GC gc, int width, int height, double size);
void draw_coordinate_axes(Display* display, Window window, GC gc, int width, int height, double size);
void draw_ground_plane(Display* display, Window window, GC gc, int width, int height, double size);
void draw_triangle_zbuffer(Display* display, Drawable drawable, GC gc, Triangle tri, int width, int height, Color color);
void draw_shadow_pixmap(Display* display, Drawable drawable, GC gc, Triangle tri, Point3D light, int width, int height, double size);
void draw_scene_pixmap(Display* display, Drawable drawable, GC gc, int width, int height, double size);
Pixmap create_pixmap_buffer(Display* display, Window window, int width, int height);
void copy_pixmap_to_window(Display* display, Window window, GC gc, Pixmap pixmap, int width, int height);
void clear_pixmap(Display* display, Pixmap pixmap, GC gc, int width, int height);
#endif //GEOMETRY_h