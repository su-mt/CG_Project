// TRANSFORMS.C

#include "include/geometry.h"
#include <math.h>



double tx = 0, ty = 0, tz = 0;
double rx = 0, ry = 0, rz = 0;
double scale = 1.0;

Point2D project(Point3D p, int width, int height, int type_of_project)
{
    if (type_of_project == ORTOGRAPHIC){
        return orto_project(p,width,height);
    }

    if (type_of_project == PERSPECTIVE){
        return perspective_project(p,width,height,750,1000);
    }

    return perspective_project(p,width,height,750,1000);

}

Point2D orto_project(Point3D p, int width, int height) {
    Point2D result;
    result.x = (int)(p.x + width / 2);
    result.y = (int)(-p.y + height / 2);
    return result;
}

Point2D perspective_project(Point3D p, int width, int height, double fov, double viewer_distance) {
    Point2D res;
    double factor = fov / (viewer_distance + p.z);
    res.x = width / 2 + p.x * factor;
    res.y = height / 2 - p.y * factor;
    return res;
}



Point3D transform_point(Point3D p) {
    Point3D r;
    // масштаб
    p.x *= scale;  p.y *= scale;  p.z *= scale;
    // вращение вокруг X
    r.x = p.x;
    r.y = p.y * cos(rx) - p.z * sin(rx);
    r.z = p.y * sin(rx) + p.z * cos(rx);
    p = r;
    // вращение вокруг Y
    r.x =  p.x * cos(ry) + p.z * sin(ry);
    r.y =  p.y;
    r.z = -p.x * sin(ry) + p.z * cos(ry);
    p = r;
    // вращение вокруг Z
    r.x = p.x * cos(rz) - p.y * sin(rz);
    r.y = p.x * sin(rz) + p.y * cos(rz);
    r.z = p.z;
    // смещение
    r.x += tx;
    r.y += ty;
    r.z += tz;
    return r;
}