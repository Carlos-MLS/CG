#ifndef CONE_H
#define CONE_H

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarCone(float raioBase, float altura, int fatias, int camadas)
{
    vector<Point3D> vertices;

    if (fatias < 3)
        fatias = 3;
    if (camadas < 1)
        camadas = 1;

    float alturaCamada = altura / camadas;
    float anguloFatia = 2.0f * M_PI / fatias;

    Point3D centro(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < fatias; i++)
    {
        float a1 = i * anguloFatia;
        float a2 = (i + 1) * anguloFatia;

        Point3D p1(raioBase * cos(a1), 0.0f, raioBase * sin(a1));
        Point3D p2(raioBase * cos(a2), 0.0f, raioBase * sin(a2));

        vertices.push_back(centro);
        vertices.push_back(p1);
        vertices.push_back(p2);
    }

    for (int i = 0; i < camadas; i++)
    {
        float y1 = i * alturaCamada;
        float y2 = (i + 1) * alturaCamada;

        float r1 = raioBase * (1.0f - (float)i / camadas);
        float r2 = raioBase * (1.0f - (float)(i + 1) / camadas);

        for (int j = 0; j < fatias; j++)
        {
            float a1 = j * anguloFatia;
            float a2 = (j + 1) * anguloFatia;

            Point3D v1(r1 * cos(a1), y1, r1 * sin(a1));
            Point3D v2(r1 * cos(a2), y1, r1 * sin(a2));
            Point3D v3(r2 * cos(a2), y2, r2 * sin(a2));
            Point3D v4(r2 * cos(a1), y2, r2 * sin(a1));

            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v2);

            if (i != camadas - 1)
            {
                vertices.push_back(v1);
                vertices.push_back(v4);
                vertices.push_back(v3);
            }
        }
    }

    return vertices;
}

#endif