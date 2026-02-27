#ifndef BOX_H
#define BOX_H

#include <vector>
#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarBox(float tamanho, int divisoes)
{
    vector<Point3D> vertices;

    if (divisoes < 1)
    {
        divisoes = 1;
    }

    float metade = tamanho / 2.0f;

    float passo = tamanho / divisoes;

    for (int i = 0; i < divisoes; i++)
    {
        for (int j = 0; j < divisoes; j++)
        {

            float x1 = -metade + i * passo;
            float x2 = -metade + (i + 1) * passo;
            float y1 = -metade + j * passo;
            float y2 = -metade + (j + 1) * passo;

            vertices.push_back(Point3D(x1, y1, metade));
            vertices.push_back(Point3D(x2, y1, metade));
            vertices.push_back(Point3D(x1, y2, metade));

            vertices.push_back(Point3D(x2, y1, metade));
            vertices.push_back(Point3D(x2, y2, metade));
            vertices.push_back(Point3D(x1, y2, metade));

            vertices.push_back(Point3D(x2, y1, -metade));
            vertices.push_back(Point3D(x1, y1, -metade));
            vertices.push_back(Point3D(x2, y2, -metade));

            vertices.push_back(Point3D(x1, y1, -metade));
            vertices.push_back(Point3D(x1, y2, -metade));
            vertices.push_back(Point3D(x2, y2, -metade));

            float z1 = -metade + i * passo;
            float z2 = -metade + (i + 1) * passo;
            y1 = -metade + j * passo;
            y2 = -metade + (j + 1) * passo;

            vertices.push_back(Point3D(metade, y1, z2));
            vertices.push_back(Point3D(metade, y1, z1));
            vertices.push_back(Point3D(metade, y2, z2));

            vertices.push_back(Point3D(metade, y1, z1));
            vertices.push_back(Point3D(metade, y2, z1));
            vertices.push_back(Point3D(metade, y2, z2));

            vertices.push_back(Point3D(-metade, y1, z1));
            vertices.push_back(Point3D(-metade, y1, z2));
            vertices.push_back(Point3D(-metade, y2, z1));

            vertices.push_back(Point3D(-metade, y1, z2));
            vertices.push_back(Point3D(-metade, y2, z2));
            vertices.push_back(Point3D(-metade, y2, z1));

            x1 = -metade + i * passo;
            x2 = -metade + (i + 1) * passo;
            z1 = -metade + j * passo;
            z2 = -metade + (j + 1) * passo;

            vertices.push_back(Point3D(x1, metade, z1));
            vertices.push_back(Point3D(x1, metade, z2));
            vertices.push_back(Point3D(x2, metade, z2));

            vertices.push_back(Point3D(x1, metade, z1));
            vertices.push_back(Point3D(x2, metade, z2));
            vertices.push_back(Point3D(x2, metade, z1));

            vertices.push_back(Point3D(x2, -metade, z1));
            vertices.push_back(Point3D(x2, -metade, z2));
            vertices.push_back(Point3D(x1, -metade, z1));

            vertices.push_back(Point3D(x2, -metade, z2));
            vertices.push_back(Point3D(x1, -metade, z2));
            vertices.push_back(Point3D(x1, -metade, z1));
        }
    }

    return vertices;
}

#endif