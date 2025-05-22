// RENDERING.C
#include "include/geometry.h"
#include <math.h>
#include <stdlib.h>

#define INF 10000000

extern double tx, ty, tz;
extern double rx, ry, rz;
extern double scale;

extern int type_of_project;

float zBuffer[WIDTH][HEIGHT];
Color frameBuffer[WIDTH][HEIGHT];

void clearBuffers() {
    for (int x = 0; x < WIDTH; ++x)
        for (int y = 0; y < HEIGHT; ++y) {
            zBuffer[x][y] = INF; // "Бесконечная" глубина
            frameBuffer[x][y] = (Color) {255, 255, 255}; // Чёрный
        }
}


void draw_line_zbuffer(Display* display, Window window, GC gc, Point3D p1, Point3D p2, int width, int height) {
    Point2D pp1 = project(p1, width, height, type_of_project);
    Point2D pp2 = project(p2, width, height, type_of_project);

    int x0 = pp1.x;
    int y0 = pp1.y;
    int x1 = pp2.x;
    int y1 = pp2.y;

    float z0 = p1.z;
    float z1 = p2.z;

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    int x = x0, y = y0;
    int steps = fmax(abs(x1 - x0), abs(y1 - y0));
    for (int i = 0; i <= steps; i++) {
        float t = (steps == 0) ? 0.0f : (float)i / steps;
        float z = z0 * (1 - t) + z1 * t;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            if (z < zBuffer[x][y]) {
                zBuffer[x][y] = z;
                frameBuffer[x][y] = (Color){0, 0, 0};

                XSetForeground(display, gc, BlackPixel(display, 0));
                XDrawPoint(display, window, gc, x, y);
            }
        }

        if (x == x1 && y == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }
}


void draw_triangle_zbuffer(Display* display, Window window, GC gc, Triangle tri, int width, int height, Color color)
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
                    XDrawPoint(display, window, gc, x, y);
                }
            }
        }
    }
}


// void draw_triangle_zbuffer(Display* display, Window window, GC gc, Triangle tri, int width, int height, Color Color)
// {

//     // Предполагается, что есть функция: Point2D project(Point3D p);
//     Point3D v0 = tri.v[0];
//     Point3D v1 = tri.v[1];
//     Point3D v2 = tri.v[2];



//     Point2D p0 = project(v0,width,height,type_of_project);
//     Point2D p1 = project(v1,width,height,type_of_project);
//     Point2D p2 = project(v2,width,height,type_of_project);

//     // Вычисляем границы bbox
//     int minX = fmax(0, fmin(fmin(p0.x, p1.x), p2.x));
//     int maxX = fmin(width - 1, fmax(fmax(p0.x, p1.x), p2.x));
//     int minY = fmax(0, fmin(fmin(p0.y, p1.y), p2.y));
//     int maxY = fmin(height - 1, fmax(fmax(p0.y, p1.y), p2.y));

//     // Вычисляем площадь треугольника
//     float denom = (float)(
//         (p1.y - p2.y) * (p0.x - p2.x) +
//         (p2.x - p1.x) * (p0.y - p2.y)
//     );

//     if (fabs(denom) < 1e-6) return; // вырожденный треугольник

//     for (int y = minY; y <= maxY; ++y) {
//         for (int x = minX; x <= maxX; ++x) {
//             // Barycentric coordinates
//             float w0 = ((p1.y - p2.y)*(x - p2.x) + (p2.x - p1.x)*(y - p2.y)) / denom;
//             float w1 = ((p2.y - p0.y)*(x - p2.x) + (p0.x - p2.x)*(y - p2.y)) / denom;
//             float w2 = 1.0f - w0 - w1;

//             if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
//                 // интерполяция глубины
//                 float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;

//                 if (z < zBuffer[x][y]) {
//                     zBuffer[x][y] = z;
//                     frameBuffer[x][y] = (Color){0, 255, 0}; // Белый, можно поменять

//                     //XSetForeground(display, gc, BlackPixel(display, 0));
//                     XDrawPoint(display, window, gc, x, y);
//                 }
//             }
//         }
//     }
// }


void draw_scene(Display* display, Window window, GC gc, int width, int height, double size) {
    double s = size / 2.0;

    clearBuffers();

    Color cubeColor = {0, 0, 255};      // Синий
    Color pyramidColor = {255, 0, 0};  

    Point3D cube_v[8] = {
        {-s,-s,-s},{ s,-s,-s},{ s, s,-s},{-s, s,-s},
        {-s,-s, s},{ s,-s, s},{ s, s, s},{-s, s, s}
    };
    int cube_e[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    int cube_faces[12][3] = {
    {0, 1, 2}, {0, 2, 3}, // нижняя грань (z = -s)
    {4, 5, 6}, {4, 6, 7}, // верхняя грань (z = +s)
    {0, 1, 5}, {0, 5, 4}, // передняя
    {3, 2, 6}, {3, 6, 7}, // задняя
    {0, 3, 7}, {0, 7, 4}, // левая
    {1, 2, 6}, {1, 6, 5}  // правая
    };

    double factor_xy = 1.75;
    double factor_z = 2.0;
    Point3D pyr_v[5] = {
        { 0, 0,  s * factor_z +10},
        {-s * factor_xy, -s * factor_xy, -s+10},
        { s * factor_xy, -s * factor_xy, -s+10},
        { s * factor_xy,  s * factor_xy, -s+10},
        {-s * factor_xy,  s * factor_xy, -s+10}
    };
    int pyr_e[8][2] = {
        {0,1},{0,2},{0,3},{0,4},
        {1,2},{2,3},{3,4},{4,1}
    };

    int pyr_faces[6][3] = {
    {1, 2, 3}, {1, 3, 4}, // основание
    {0, 1, 2},
    {0, 2, 3},
    {0, 3, 4},
    {0, 4, 1}
    };

    // // Рисуем рёбра куба с z-буфером
    // for (int i = 0; i < 12; i++) {
    //     Point3D p1 = transform_point(cube_v[cube_e[i][0]]);
    //     Point3D p2 = transform_point(cube_v[cube_e[i][1]]);
    //     draw_line_zbuffer(display, window, gc, p1, p2, width, height);
        
    // }

    // // Рисуем рёбра пирамиды с z-буфером
    // for (int i = 0; i < 8; i++) {
    //     Point3D p1 = transform_point(pyr_v[pyr_e[i][0]]);
    //     Point3D p2 = transform_point(pyr_v[pyr_e[i][1]]);
    //     draw_line_zbuffer(display, window, gc, p1, p2, width, height);
    // }
    for (int i = 0; i < 12; i++) {
        Triangle tri = {
            .v = {
                transform_point(cube_v[cube_faces[i][0]]),
                transform_point(cube_v[cube_faces[i][1]]),
                transform_point(cube_v[cube_faces[i][2]])
            }
        };
        draw_triangle_zbuffer(display, window, gc, tri, width, height, cubeColor);
    }

    for (int i = 0; i < 6; i++) {
        Triangle tri = {
            .v = {
                transform_point(pyr_v[pyr_faces[i][0]]),
                transform_point(pyr_v[pyr_faces[i][1]]),
                transform_point(pyr_v[pyr_faces[i][2]])
            }
        };
        draw_triangle_zbuffer(display, window, gc, tri, width, height, pyramidColor);
    }

}



// void draw_scene(Display* display, Window window, GC gc, int width, int height, double size) {
//     double s = size / 2.0;
    
//     clearBuffers();
//     // Вершины куба (остаются как есть)
//     Point3D cube_v[8] = {
//         {-s,-s,-s},{ s,-s,-s},{ s, s,-s},{-s, s,-s},
//         {-s,-s, s},{ s,-s, s},{ s, s, s},{-s, s, s}
//     };
//     int cube_e[12][2] = {
//         {0,1},{1,2},{2,3},{3,0},
//         {4,5},{5,6},{6,7},{7,4},
//         {0,4},{1,5},{2,6},{3,7}
//     };

//     int cube_faces[12][3] = {
//     {0, 1, 2}, {0, 2, 3}, // нижняя грань (z = -s)
//     {4, 5, 6}, {4, 6, 7}, // верхняя грань (z = +s)
//     {0, 1, 5}, {0, 5, 4}, // передняя
//     {3, 2, 6}, {3, 6, 7}, // задняя
//     {0, 3, 7}, {0, 7, 4}, // левая
//     {1, 2, 6}, {1, 6, 5}  // правая
//     };

//     // Вытянутая пирамида
//     // Ширина и высота увеличены по сравнению с кубом
//     double factor_xy = 0.75;  // увеличение основания по X и Y
//     double factor_z = 2.0;   // вытягивание вершины по Z

//     Point3D pyr_v[5] = {
//         { 0, 0,  s * factor_z},             // верхняя вершина
//         {-s * factor_xy, -s * factor_xy, -s}, // основание
//         { s * factor_xy, -s * factor_xy, -s},
//         { s * factor_xy,  s * factor_xy, -s},
//         {-s * factor_xy,  s * factor_xy, -s}
//     };
//     int pyr_e[8][2] = {
//         {0,1},{0,2},{0,3},{0,4},
//         {1,2},{2,3},{3,4},{4,1}
//     };

//     int pyr_faces[6][3] = {
//     {1, 2, 3}, {1, 3, 4}, // основание
//     {0, 1, 2},
//     {0, 2, 3},
//     {0, 3, 4},
//     {0, 4, 1}
//     };

//     // Рисуем куб
//     for (int i = 0; i < 12; i++) {
//         Point3D p1 = transform_point(cube_v[cube_e[i][0]]);
//         Point3D p2 = transform_point(cube_v[cube_e[i][1]]);
//         Point2D pp1 = project(p1, width, height, type_of_project);
//         Point2D pp2 = project(p2, width, height, type_of_project);
//         XDrawLine(display, window, gc, pp1.x, pp1.y, pp2.x, pp2.y);
//     }

//     // Рисуем пирамиду
//     for (int i = 0; i < 8; i++) {
//         Point3D p1 = transform_point(pyr_v[pyr_e[i][0]]);
//         Point3D p2 = transform_point(pyr_v[pyr_e[i][1]]);
//         Point2D pp1 = project(p1, width, height, type_of_project);
//         Point2D pp2 = project(p2, width, height, type_of_project);
//         XDrawLine(display, window, gc, pp1.x, pp1.y, pp2.x, pp2.y);
//     }


// }




void draw_cube(Display* display, Window window, GC gc, int width, int height, double size) {
    double s = size / 2.0;

    // Вершины куба (относительно центра)
    Point3D vertices[8] = {
        {-s, -s, -s}, {s, -s, -s}, {s, s, -s}, {-s, s, -s}, // задняя грань
        {-s, -s,  s}, {s, -s,  s}, {s, s,  s}, {-s, s,  s}  // передняя грань
    };

    // Пары индексов вершин, соединённых рёбрами
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // задняя грань
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // передняя грань
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // соединения перед-зад
    };

    // Отрисовка рёбер
    for (int i = 0; i < 12; i++) {
        Point2D p1 = project(vertices[edges[i][0]], width, height, type_of_project);
        Point2D p2 = project(vertices[edges[i][1]], width, height, type_of_project);
        XDrawLine(display, window, gc, p1.x, p1.y, p2.x, p2.y);
    }
}


void draw_tetrahedron(Display* display, Window window, GC gc, int width, int height, double size) {
    double s = size / 2.0;

    // Вершины тетраэдра, центрированы в начале координат
    Point3D vertices[4] = {
        { 0,  0,  s},                         // Верхняя вершина (апекс)
        {-s, -s, -s},                         // Основание 1
        { s, -s, -s},                         // Основание 2
        { 0,  s, -s}                          // Основание 3
    };

    // Пары индексов вершин, соединённых рёбрами
    int edges[6][2] = {
        {0, 1}, {0, 2}, {0, 3},              // рёбра от апекса к основанию
        {1, 2}, {2, 3}, {3, 1}               // рёбра основания
    };

    // Отрисовка 
    for (int i = 0; i < 6; i++) {
        Point2D p1 = project(vertices[edges[i][0]], width, height, type_of_project);
        Point2D p2 = project(vertices[edges[i][1]], width, height, type_of_project);
        XDrawLine(display, window, gc, p1.x, p1.y, p2.x, p2.y);
    }
}




// void draw_scene(Display* display, Window window, GC gc, int width, int height, double size) {
//     double s = size / 2.0;

//     // Исходные вершины
//     Point3D cube_v[8] = {
//         {-s,-s,-s},{ s,-s,-s},{ s, s,-s},{-s, s,-s},
//         {-s,-s, s},{ s,-s, s},{ s, s, s},{-s, s, s}
//     };
//     int cube_e[12][2] = {
//         {0,1},{1,2},{2,3},{3,0},
//         {4,5},{5,6},{6,7},{7,4},
//         {0,4},{1,5},{2,6},{3,7}
//     };

//     Point3D tet_v[4] = {
//         { 0,  0,  s},
//         {-s, -s, -s},
//         { s, -s, -s},
//         { 0,  s, -s}
//     };
//     int tet_e[6][2] = {
//         {0,1},{0,2},{0,3},
//         {1,2},{2,3},{3,1}
//     };

//     // Рисуем куб
//     for (int i = 0; i < 12; i++) {
//         Point3D p1 = transform_point(cube_v[cube_e[i][0]]);
//         Point3D p2 = transform_point(cube_v[cube_e[i][1]]);
//         Point2D pp1 = orto_project(p1, width, height);
//         Point2D pp2 = orto_project(p2, width, height);
//         XDrawLine(display, window, gc, pp1.x, pp1.y, pp2.x, pp2.y);
//     }

//     // Рисуем тетраэдр
//     for (int i = 0; i < 6; i++) {
//         Point3D p1 = transform_point(tet_v[tet_e[i][0]]);
//         Point3D p2 = transform_point(tet_v[tet_e[i][1]]);
//         Point2D pp1 = orto_project(p1, width, height);
//         Point2D pp2 = orto_project(p2, width, height);
//         XDrawLine(display, window, gc, pp1.x, pp1.y, pp2.x, pp2.y);
//     }
// }


