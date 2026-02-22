//
// Created by francisco0504 on 17/02/26.
//

#ifndef BOX_H
#define BOX_H

#include <vector>
#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarBox(float x, float y, float z, int divisions)
{
    vector<Point3D> vertices;

    if (divisions < 1)
    {
        divisions = 1;
    }

    // metade de cada dimensão pra centrar na origem
    float halfX = x / 2.0f;
    float halfY = y / 2.0f;
    float halfZ = z / 2.0f;

    // tamanho de cada subdivisao
    float stepX = x / divisions;
    float stepY = y / divisions;
    float stepZ = z / divisions;

    // iterar sobre a grelha de subdivisões
    for (int i = 0; i < divisions; i++)
    {
        for (int j = 0; j < divisions; j++)
        {

            // face da frente (z = +halfZ)
            float x1 = -halfX + i * stepX;
            float x2 = -halfX + (i + 1) * stepX;
            float y1 = -halfY + j * stepY;
            float y2 = -halfY + (j + 1) * stepY;

            vertices.push_back(Point3D(x1, y1, halfZ));
            vertices.push_back(Point3D(x2, y1, halfZ));
            vertices.push_back(Point3D(x2, y2, halfZ));

            vertices.push_back(Point3D(x1, y1, halfZ));
            vertices.push_back(Point3D(x2, y2, halfZ));
            vertices.push_back(Point3D(x1, y2, halfZ));

            // face de tras (z = -halfZ)
            // aqui invertemos a ordem dos vertices pra ficar CCW visto de fora
            vertices.push_back(Point3D(x2, y1, -halfZ));
            vertices.push_back(Point3D(x1, y1, -halfZ));
            vertices.push_back(Point3D(x1, y2, -halfZ));

            vertices.push_back(Point3D(x2, y1, -halfZ));
            vertices.push_back(Point3D(x1, y2, -halfZ));
            vertices.push_back(Point3D(x2, y2, -halfZ));

            // face direita (x = +halfX)
            float z1 = -halfZ + i * stepZ;
            float z2 = -halfZ + (i + 1) * stepZ;
            y1 = -halfY + j * stepY;
            y2 = -halfY + (j + 1) * stepY;

            vertices.push_back(Point3D(halfX, y1, z2));
            vertices.push_back(Point3D(halfX, y1, z1));
            vertices.push_back(Point3D(halfX, y2, z1));

            vertices.push_back(Point3D(halfX, y1, z2));
            vertices.push_back(Point3D(halfX, y2, z1));
            vertices.push_back(Point3D(halfX, y2, z2));

            // face esquerda (x = -halfX)
            vertices.push_back(Point3D(-halfX, y1, z1));
            vertices.push_back(Point3D(-halfX, y1, z2));
            vertices.push_back(Point3D(-halfX, y2, z2));

            vertices.push_back(Point3D(-halfX, y1, z1));
            vertices.push_back(Point3D(-halfX, y2, z2));
            vertices.push_back(Point3D(-halfX, y2, z1));

            // face de cima (y = +halfY)
            x1 = -halfX + i * stepX;
            x2 = -halfX + (i + 1) * stepX;
            z1 = -halfZ + j * stepZ;
            z2 = -halfZ + (j + 1) * stepZ;

            vertices.push_back(Point3D(x1, halfY, z1));
            vertices.push_back(Point3D(x1, halfY, z2));
            vertices.push_back(Point3D(x2, halfY, z2));

            vertices.push_back(Point3D(x1, halfY, z1));
            vertices.push_back(Point3D(x2, halfY, z2));
            vertices.push_back(Point3D(x2, halfY, z1));

            // face de baixo (y = -halfY)
            vertices.push_back(Point3D(x2, -halfY, z1));
            vertices.push_back(Point3D(x2, -halfY, z2));
            vertices.push_back(Point3D(x1, -halfY, z2));

            vertices.push_back(Point3D(x2, -halfY, z1));
            vertices.push_back(Point3D(x1, -halfY, z2));
            vertices.push_back(Point3D(x1, -halfY, z1));
        }
    }

    return vertices;
}

#endif // BOX_H