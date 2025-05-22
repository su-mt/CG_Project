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