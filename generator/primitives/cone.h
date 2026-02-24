//
// Created by francisco0504 on 17/02/26.
//

#ifndef CONE_H
#define CONE_H

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarCone(float bottomRadius, float height, int slices, int stacks)
{
    vector<Point3D> vertices;

    if (slices < 3)
        slices = 3;
    if (stacks < 1)
        stacks = 1;

    float alturaStack = height / stacks;
    float anguloSlice = 2.0f * M_PI / slices;

    // primeiro a base - fan de triangulos no plano XZ (y=0)
    Point3D centro(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < slices; i++)
    {
        float a1 = i * anguloSlice;
        float a2 = (i + 1) * anguloSlice;

        Point3D p1(bottomRadius * cos(a1), 0.0f, bottomRadius * sin(a1));
        Point3D p2(bottomRadius * cos(a2), 0.0f, bottomRadius * sin(a2));

        // CCW visto de baixo (normal aponta para baixo)
        vertices.push_back(centro);
        vertices.push_back(p1);
        vertices.push_back(p2);
    }

    // agora o corpo - dividido em stacks
    // o raio vai diminuindo linearmente ate chegar a 0 no topo
    for (int i = 0; i < stacks; i++)
    {
        float y1 = i * alturaStack;
        float y2 = (i + 1) * alturaStack;

        float r1 = bottomRadius * (1.0f - (float)i / stacks);
        float r2 = bottomRadius * (1.0f - (float)(i + 1) / stacks);

        for (int j = 0; j < slices; j++)
        {
            float a1 = j * anguloSlice;
            float a2 = (j + 1) * anguloSlice;

            Point3D v1(r1 * cos(a1), y1, r1 * sin(a1));
            Point3D v2(r1 * cos(a2), y1, r1 * sin(a2));
            Point3D v3(r2 * cos(a2), y2, r2 * sin(a2));
            Point3D v4(r2 * cos(a1), y2, r2 * sin(a1));

            // triangulo 1 (diagonal v1-v3, winding CCW visto de fora)
            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v2);

            // triangulo 2 (na ultima stack ja converge no vertice, nao precisa)
            if (i != stacks - 1)
            {
                vertices.push_back(v1);
                vertices.push_back(v4);
                vertices.push_back(v3);
            }
        }
    }

    return vertices;
}

#endif // CONE_H