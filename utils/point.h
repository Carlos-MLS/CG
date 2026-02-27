#ifndef POINT_H
#define POINT_H

struct Point3D { // struct para guardar um ponto
    float x, y, z;

    Point3D(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

#endif
