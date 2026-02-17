//
// Created by francisco0504 on 17/02/26.
//

#ifndef PLANE_H
#define PLANE_H
#include <vector>

#include "../../utils/point.h"

using namespace  std;

inline vector<Point3D> gerarPlane(float length, int divisions) {
    vector<Point3D> vertices;

    if (divisions < 1) {
        divisions = 1;
    }

    // Calcular o tamanho de cada subdivisão
    float tamSubDivisao = length / divisions;

    //calcular o offset para centralizar o plano na origem
    float halfLength = length / 2.0f;

    // agr geramos os triângulos para cada célula da grelha
    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            // Calcular as coordenadas dos 4 cantos da célula
            float x1 = -halfLength + i * tamSubDivisao;
            float x2 = -halfLength + (i + 1) * tamSubDivisao;
            float z1 = -halfLength + j * tamSubDivisao;
            float z2 = -halfLength + (j + 1) * tamSubDivisao;

            // Todos os pontos têm y = 0 (plano XZ)
            float y = 0.0f;

            // Vértices dos 4 cantos da célula
            Point3D v1(x1, y, z1);  // Canto inferior esquerdo
            Point3D v2(x1, y, z2);  // Canto superior esquerdo
            Point3D v3(x2, y, z2);  // Canto superior direito
            Point3D v4(x2, y, z1);  // Canto inferior direito

            // Primeiro triângulo (v1, v2, v3)
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);

            // Segundo triângulo (v1, v3, v4)
            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v4);
        }
    }

    return vertices;
}

#endif //PLANE_H