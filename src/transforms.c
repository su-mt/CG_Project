// TRANSFORMS.C

#include "include/geometry.h"
#include <math.h>

double tx = 0, ty = CAMERA_TY, tz = CAMERA_TZ;
double rx = CAMERA_RX, ry = CAMERA_RY, rz = CAMERA_RZ;
double scale = DEFAULT_SCALE;

Point2D project(Point3D p, int width, int height, int type_of_project)
{
    if (type_of_project == ORTOGRAPHIC) {
        return orto_project(p, width, height);
    }

    if (type_of_project == PERSPECTIVE) {
        return perspective_project(p, width, height, FOV, VIEWER_DISTANCE);
    }

    if (type_of_project == ISOMETRIC) {
        return isometric_project(p, width, height);
    }

    return perspective_project(p, width, height, FOV, VIEWER_DISTANCE);
}

Point2D isometric_project(Point3D p, int width, int height)
{
    Point2D result;
    double iso_x = p.x * cos(ISO_ANGLE) - p.z * cos(ISO_ANGLE);
    double iso_y = p.y + p.x * sin(ISO_ANGLE) + p.z * sin(ISO_ANGLE);
    
    result.x = (int)(iso_x + width / 2);
    result.y = (int)(-iso_y + height / 2);
    return result;
}

Point2D orto_project(Point3D p, int width, int height)
{
    Point2D result;
    result.x = (int)(p.x + width / 2);
    result.y = (int)(-p.y + height / 2);
    return result;
}

Point2D perspective_project(Point3D p, int width, int height, double fov, double viewer_distance)
{
    Point2D res;
    double factor = fov / (viewer_distance + p.z);
    res.x = width / 2 + p.x * factor;
    res.y = height / 2 - p.y * factor;
    return res;
}

Point3D transform_point(Point3D p)
{
    Point3D r;
    
    p.x *= scale;
    p.y *= scale;
    p.z *= scale;
    
    r.x = p.x;
    r.y = p.y * cos(rx) - p.z * sin(rx);
    r.z = p.y * sin(rx) + p.z * cos(rx);
    p = r;
    
    r.x = p.x * cos(ry) + p.z * sin(ry);
    r.y = p.y;
    r.z = -p.x * sin(ry) + p.z * cos(ry);
    p = r;
    
    r.x = p.x * cos(rz) - p.y * sin(rz);
    r.y = p.x * sin(rz) + p.y * cos(rz);
    r.z = p.z;
    
    r.x += tx;
    r.y += ty;
    r.z += tz;
    return r;
}

Point3D project_to_ground(Point3D p, Point3D light, double ground_z)
{
    double t = (ground_z - p.z) / (light.z - p.z);
    Point3D projected;
    projected.x = p.x + (light.x - p.x) * t;
    projected.y = p.y + (light.y - p.y) * t;
    projected.z = ground_z;
    return projected;
}

Point3D project_to_zx_plane(Point3D p, Point3D light, double ground_y)
{
    double t = (ground_y - p.y) / (light.y - p.y);
    Point3D projected;
    projected.x = p.x + (light.x - p.x) * t;
    projected.y = ground_y;
    projected.z = p.z + (light.z - p.z) * t;
    return projected;
}

