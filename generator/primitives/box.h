//
// Created by francisco0504 on 17/02/26.
//

#ifndef BOX_H
#define BOX_H

#include <vector>
#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarBox(float size, int divisions)
{
    vector<Point3D> vertices;

    if (divisions < 1)
    {
        divisions = 1;
    }

    // metade de cada dimensão pra centrar na origem
    float halfSize = size / 2.0f;

    // tamanho de cada subdivisao
    float step = size / divisions;

    // iterar sobre a grelha de subdivisões
    for (int i = 0; i < divisions; i++)
    {
        for (int j = 0; j < divisions; j++)
        {

            // face da frente (z = +halfSize)
            float x1 = -halfSize + i * step;
            float x2 = -halfSize + (i + 1) * step;
            float y1 = -halfSize + j * step;
            float y2 = -halfSize + (j + 1) * step;

            vertices.push_back(Point3D(x1, y1, halfSize));
            vertices.push_back(Point3D(x2, y1, halfSize));
            vertices.push_back(Point3D(x1, y2, halfSize));

            vertices.push_back(Point3D(x2, y1, halfSize));
            vertices.push_back(Point3D(x2, y2, halfSize));
            vertices.push_back(Point3D(x1, y2, halfSize));

            // face de tras (z = -halfSize)
            // aqui invertemos a ordem dos vertices pra ficar CCW visto de fora
            vertices.push_back(Point3D(x2, y1, -halfSize));
            vertices.push_back(Point3D(x1, y1, -halfSize));
            vertices.push_back(Point3D(x2, y2, -halfSize));

            vertices.push_back(Point3D(x1, y1, -halfSize));
            vertices.push_back(Point3D(x1, y2, -halfSize));
            vertices.push_back(Point3D(x2, y2, -halfSize));

            // face direita (x = +halfSize)
            float z1 = -halfSize + i * step;
            float z2 = -halfSize + (i + 1) * step;
            y1 = -halfSize + j * step;
            y2 = -halfSize + (j + 1) * step;

            vertices.push_back(Point3D(halfSize, y1, z2));
            vertices.push_back(Point3D(halfSize, y1, z1));
            vertices.push_back(Point3D(halfSize, y2, z2));

            vertices.push_back(Point3D(halfSize, y1, z1));
            vertices.push_back(Point3D(halfSize, y2, z1));
            vertices.push_back(Point3D(halfSize, y2, z2));

            // face esquerda (x = -halfSize)
            vertices.push_back(Point3D(-halfSize, y1, z1));
            vertices.push_back(Point3D(-halfSize, y1, z2));
            vertices.push_back(Point3D(-halfSize, y2, z1));

            vertices.push_back(Point3D(-halfSize, y1, z2));
            vertices.push_back(Point3D(-halfSize, y2, z2));
            vertices.push_back(Point3D(-halfSize, y2, z1));

            // face de cima (y = +halfSize)
            x1 = -halfSize + i * step;
            x2 = -halfSize + (i + 1) * step;
            z1 = -halfSize + j * step;
            z2 = -halfSize + (j + 1) * step;

            vertices.push_back(Point3D(x1, halfSize, z1));
            vertices.push_back(Point3D(x1, halfSize, z2));
            vertices.push_back(Point3D(x2, halfSize, z1));

            vertices.push_back(Point3D(x1, halfSize, z2));
            vertices.push_back(Point3D(x2, halfSize, z2));
            vertices.push_back(Point3D(x2, halfSize, z1));

            // face de baixo (y = -halfSize)
            vertices.push_back(Point3D(x2, -halfSize, z1));
            vertices.push_back(Point3D(x2, -halfSize, z2));
            vertices.push_back(Point3D(x1, -halfSize, z1));

            vertices.push_back(Point3D(x2, -halfSize, z2));
            vertices.push_back(Point3D(x1, -halfSize, z2));
            vertices.push_back(Point3D(x1, -halfSize, z1));
        }
    }

    return vertices;
}

#endif // BOX_H