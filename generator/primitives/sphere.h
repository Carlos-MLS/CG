//
// Created by francisco0504 on 17/02/26.
//

#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <cmath>
#include "../../utils/point.h"

using namespace std;

// TODO: Implementar geração de esfera

inline vector<Point3D> gerarSphere(float radius, int slices, int stacks) {
    vector<Point3D> vertices;

    // TODO: Implementar lógica aqui
    // x = radius * sin(theta) * cos(phi)
    // y = radius * cos(theta)
    // z = radius * sin(theta) * sin(phi)

    return vertices;
}

#endif //SPHERE_H