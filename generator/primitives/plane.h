#ifndef PLANE_H
#define PLANE_H
#include <vector>

#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarPlane(float comprimento, int divisoes)
{
    vector<Point3D> vertices;

    if (divisoes < 1)
    {
        divisoes = 1;
    }

    float tamSubDivisao = comprimento / divisoes;

    float metade = comprimento / 2.0f;

    for (int i = 0; i < divisoes; i++)
    {
        for (int j = 0; j < divisoes; j++)
        {
            float x1 = -metade + i * tamSubDivisao;
            float x2 = -metade + (i + 1) * tamSubDivisao;
            float z1 = -metade + j * tamSubDivisao;
            float z2 = -metade + (j + 1) * tamSubDivisao;

            float y = 0.0f;

            Point3D v1(x1, y, z1);
            Point3D v2(x1, y, z2);
            Point3D v3(x2, y, z2);
            Point3D v4(x2, y, z1);

            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);

            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v4);
        }
    }

    return vertices;
}

#endif