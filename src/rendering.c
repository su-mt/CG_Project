// RENDERING.C
#include "include/geometry.h"
#include <math.h>
#include <stdlib.h>

extern double tx, ty, tz;
extern double rx, ry, rz;
extern double scale;
extern int type_of_project;

float zBuffer[WIDTH][HEIGHT];
Color frameBuffer[WIDTH][HEIGHT];

void clearBuffers()
{
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            zBuffer[x][y] = INF;
            frameBuffer[x][y] = (Color) {255, 255, 255};
        }
    }
}

Pixmap create_pixmap_buffer(Display* display, Window window, int width, int height)
{
    return XCreatePixmap(display, window, width, height, DefaultDepth(display, DefaultScreen(display)));
}

void clear_pixmap(Display* display, Pixmap pixmap, GC gc, int width, int height)
{
    unsigned long old_fg = 0;
    XGCValues values;
    XGetGCValues(display, gc, GCForeground, &values);
    old_fg = values.foreground;
    
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XFillRectangle(display, pixmap, gc, 0, 0, width, height);
    
    XSetForeground(display, gc, old_fg);
}

void copy_pixmap_to_window(Display* display, Window window, GC gc, Pixmap pixmap, int width, int height)
{
    XCopyArea(display, pixmap, window, gc, 0, 0, width, height, 0, 0);
}

void draw_shadow_pixmap(Display* display, Drawable drawable, GC gc, Triangle tri, Point3D light, int width, int height, double size)
{
    Point3D ground_v[3];
    for (int i = 0; i < 3; i++) {
        ground_v[i] = project_to_ground(tri.v[i], light, -size / 2.0);
    }

    Triangle shadow_tri = {
        .v = { ground_v[0], ground_v[1], ground_v[2] }
    };

    Color shadowColor = {SHADOW_COLOR_R, SHADOW_COLOR_G, SHADOW_COLOR_B};
    draw_triangle_zbuffer(display, drawable, gc, shadow_tri, width, height, shadowColor);
}

void draw_triangle_zbuffer(Display* display, Drawable drawable, GC gc, Triangle tri, int width, int height, Color color)
{
    Point3D v0 = tri.v[0];
    Point3D v1 = tri.v[1];
    Point3D v2 = tri.v[2];

    Point2D p0 = project(v0, width, height, type_of_project);
    Point2D p1 = project(v1, width, height, type_of_project);
    Point2D p2 = project(v2, width, height, type_of_project);

    int minX = fmax(0, fmin(fmin(p0.x, p1.x), p2.x));
    int maxX = fmin(width - 1, fmax(fmax(p0.x, p1.x), p2.x));
    int minY = fmax(0, fmin(fmin(p0.y, p1.y), p2.y));
    int maxY = fmin(height - 1, fmax(fmax(p0.y, p1.y), p2.y));

    float denom = (float)(
        (p1.y - p2.y) * (p0.x - p2.x) +
        (p2.x - p1.x) * (p0.y - p2.y)
    );

    if (fabs(denom) < 1e-6) return;

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float w0 = ((p1.y - p2.y)*(x - p2.x) + (p2.x - p1.x)*(y - p2.y)) / denom;
            float w1 = ((p2.y - p0.y)*(x - p2.x) + (p0.x - p2.x)*(y - p2.y)) / denom;
            float w2 = 1.0f - w0 - w1;

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;

                if (z < zBuffer[x][y]) {
                    zBuffer[x][y] = z;
                    frameBuffer[x][y] = color;

                    XSetForeground(display, gc, (color.r << 16) | (color.g << 8) | color.b);
                    XDrawPoint(display, drawable, gc, x, y);
                }
            }
        }
    }
}

void draw_scene_pixmap(Display* display, Drawable drawable, GC gc, int width, int height, double size)
{
    double s = size / 2.0;

    clearBuffers();

    Color cubeColor = {CUBE_COLOR_R, CUBE_COLOR_G, CUBE_COLOR_B};
    Color pyramidColor = {PYRAMID_COLOR_R, PYRAMID_COLOR_G, PYRAMID_COLOR_B};

    Point3D cube_v[8] = {
        {-s,-s,-s},{ s,-s,-s},{ s, s,-s},{-s, s,-s},
        {-s,-s, s},{ s,-s, s},{ s, s, s},{-s, s, s}
    };

    int cube_faces[12][3] = {
        {0, 1, 2}, {0, 2, 3},
        {4, 5, 6}, {4, 6, 7},
        {0, 1, 5}, {0, 5, 4},
        {3, 2, 6}, {3, 6, 7},
        {0, 3, 7}, {0, 7, 4},
        {1, 2, 6}, {1, 6, 5}
    };

    Point3D pyr_v[5] = {
        { 0, 0,  s * PYRAMID_FACTOR_Z + PYRAMID_OFFSET_Z},
        {-s * PYRAMID_FACTOR_XY, -s * PYRAMID_FACTOR_XY, -s + PYRAMID_OFFSET_Z},
        { s * PYRAMID_FACTOR_XY, -s * PYRAMID_FACTOR_XY, -s + PYRAMID_OFFSET_Z},
        { s * PYRAMID_FACTOR_XY,  s * PYRAMID_FACTOR_XY, -s + PYRAMID_OFFSET_Z},
        {-s * PYRAMID_FACTOR_XY,  s * PYRAMID_FACTOR_XY, -s + PYRAMID_OFFSET_Z}
    };

    int pyr_faces[6][3] = {
        {1, 2, 3}, {1, 3, 4},
        {0, 1, 2},
        {0, 2, 3},
        {0, 3, 4},
        {0, 4, 1}
    };

    Point3D light = {0, 0, LIGHT_Z};
    
    for (int i = 0; i < 12; i++) {
        Triangle tri = {
            .v = {
                transform_point(cube_v[cube_faces[i][0]]),
                transform_point(cube_v[cube_faces[i][1]]),
                transform_point(cube_v[cube_faces[i][2]])
            }
        };
        draw_shadow_pixmap(display, drawable, gc, tri, light, width, height, size);
    }

    for (int i = 0; i < 6; i++) {
        Triangle tri = {
            .v = {
                transform_point(pyr_v[pyr_faces[i][0]]),
                transform_point(pyr_v[pyr_faces[i][1]]),
                transform_point(pyr_v[pyr_faces[i][2]])
            }
        };
        draw_shadow_pixmap(display, drawable, gc, tri, light, width, height, size);
    }

    for (int i = 0; i < 12; i++) {
        Triangle tri = {
            .v = {
                transform_point(cube_v[cube_faces[i][0]]),
                transform_point(cube_v[cube_faces[i][1]]),
                transform_point(cube_v[cube_faces[i][2]])
            }
        };
        draw_triangle_zbuffer(display, drawable, gc, tri, width, height, cubeColor);
    }

    for (int i = 0; i < 6; i++) {
        Triangle tri = {
            .v = {
                transform_point(pyr_v[pyr_faces[i][0]]),
                transform_point(pyr_v[pyr_faces[i][1]]),
                transform_point(pyr_v[pyr_faces[i][2]])
            }
        };
        draw_triangle_zbuffer(display, drawable, gc, tri, width, height, pyramidColor);
    }
}

void draw_scene(Display* display, Window window, GC gc, int width, int height, double size)
{
    draw_scene_pixmap(display, window, gc, width, height, size);
}
