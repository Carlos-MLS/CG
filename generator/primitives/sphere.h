#ifndef SPHERE_H
#define SPHERE_H

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include "../../utils/point.h"

using namespace std;

inline vector<Point3D> gerarSphere(float raio, int fatias, int camadas)
{
    vector<Point3D> vertices;

    if (fatias < 3)
        fatias = 3;
    if (camadas < 2)
        camadas = 2;

    for (int i = 0; i < camadas; i++)
    {
        // angulo polar: de 0 a PI
        float theta1 = M_PI * i / camadas;
        float theta2 = M_PI * (i + 1) / camadas;

        for (int j = 0; j < fatias; j++)
        {
            // angulo azimutal: volta toda de 0 (norte) a 2PI (sul)
            float phi1 = 2.0f * M_PI * j / fatias;
            float phi2 = 2.0f * M_PI * (j + 1) / fatias;

            // calcular os 4 vertices do quad com coordenadas esféricas
            // x = r * sin(theta) * sin(phi)
            Point3D v1(raio * sin(theta1) * sin(phi1),
                       // y = r * cos(theta)
                       raio * cos(theta1),
                       // z = r * sin(theta) * cos(phi)
                       raio * sin(theta1) * cos(phi1));

            Point3D v2(raio * sin(theta2) * sin(phi1),
                       raio * cos(theta2),
                       raio * sin(theta2) * cos(phi1));

            Point3D v3(raio * sin(theta2) * sin(phi2),
                       raio * cos(theta2),
                       raio * sin(theta2) * cos(phi2));

            Point3D v4(raio * sin(theta1) * sin(phi2),
                       raio * cos(theta1),
                       raio * sin(theta1) * cos(phi2));

            // Diagonal v1-v3 (sup-esquerdo -> inf-direito)
            // Triangulo 1: v1,v2,v3 (no polo norte v1=polo, v2,v3 distintos -> valido)
            // No polo sul v2=v3=polo -> degenera, skip
            if (i != camadas - 1)
            {
                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);
            }

            // Triangulo 2: v1,v3,v4 (no polo sul v3=polo, v1,v4 distintos -> valido)
            // No polo norte v1=v4=polo -> degenera, skip
            if (i != 0)
            {
                vertices.push_back(v1);
                vertices.push_back(v3);
                vertices.push_back(v4);
            }
        }
    }

    return vertices;
}

#endif