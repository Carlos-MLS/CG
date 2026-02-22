//
// Created by francisco0504 on 17/02/26.
//

#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <cmath>
#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarSphere(float radius, int slices, int stacks)
{
    vector<Point3D> vertices;

    if (slices < 3)
        slices = 3;
    if (stacks < 2)
        stacks = 2;

    for (int i = 0; i < stacks; i++)
    {
        // angulo polar: de 0 (polo norte) a PI (polo sul)
        float theta1 = M_PI * i / stacks;
        float theta2 = M_PI * (i + 1) / stacks;

        for (int j = 0; j < slices; j++)
        {
            // angulo azimutal: volta toda de 0 a 2PI
            float phi1 = 2.0f * M_PI * j / slices;
            float phi2 = 2.0f * M_PI * (j + 1) / slices;

            // calcular os 4 vertices do quad com coordenadas esféricas
            // x = r * sin(theta) * cos(phi)
            // y = r * cos(theta)
            // z = r * sin(theta) * sin(phi)
            Point3D v1(radius * sin(theta1) * cos(phi1),
                       radius * cos(theta1),
                       radius * sin(theta1) * sin(phi1));

            Point3D v2(radius * sin(theta2) * cos(phi1),
                       radius * cos(theta2),
                       radius * sin(theta2) * sin(phi1));

            Point3D v3(radius * sin(theta2) * cos(phi2),
                       radius * cos(theta2),
                       radius * sin(theta2) * sin(phi2));

            Point3D v4(radius * sin(theta1) * cos(phi2),
                       radius * cos(theta1),
                       radius * sin(theta1) * sin(phi2));

            // no polo norte o triangulo degenera, entao skip
            if (i != 0)
            {
                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v4);
            }

            // no polo sul tambem degenera
            if (i != stacks - 1)
            {
                vertices.push_back(v2);
                vertices.push_back(v3);
                vertices.push_back(v4);
            }
        }
    }

    return vertices;
}

#endif // SPHERE_H