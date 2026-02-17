//
// Created by francisco0504 on 17/02/26.
//

#ifndef CONE_H
#define CONE_H

#include <vector>
#include <cmath>
#include "../../utils/point.h"

using namespace std;

// TODO: Implementar geração de cone

inline vector<Point3D> gerarCone(float bottomRadius, float height, int slices, int stacks) {
    vector<Point3D> vertices;

    // TODO: Implementar lógica aqui
    // - Base: círculo no plano XZ (y=0)
    // - Corpo: triângulos que conectam a base ao vértice
    // - Vértice: ponto (0, height, 0)

    return vertices;
}

#endif //CONE_H