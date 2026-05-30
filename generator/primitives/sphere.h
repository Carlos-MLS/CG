#ifndef SPHERE_H
#define SPHERE_H

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include "../../utils/point.h"
#include "../../utils/geometry.h"

using namespace std;

//same

inline geometryPointData gerarSphere(float raio, int fatias, int camadas)
{
    geometryPointData data;

    //para calcular as normais da esfera, basta normalizar os vértices, ou seja, dividir as coordenadas x,y,z de cada vértice pelo raio da esfera,
    // porque a esfera é centrada na origem, então a normal em cada ponto é simplesmente o vetor do centro para esse ponto, normalizado para ter comprimento 1, 
    // e como o vetor do centro para o ponto é exatamente o próprio ponto (porque o centro é a origem), 
    // então a normal é o próprio ponto dividido pelo raio da esfera (para normalizar), ou seja, 
    // normal = Point3D(p.x / raio, p.y / raio, p.z / raio)

    auto normalSphere = [&](const Point3D& p) {
        return Point3D(p.x / raio, p.y / raio, p.z / raio);
    };

    if (fatias < 3)
        fatias = 3;
    if (camadas < 2)
        camadas = 2;

    for (int i = 0; i < camadas; i++)
    {
        // angulo polar de 0 a PI
        float theta1 = M_PI * i / camadas;
        float theta2 = M_PI * (i + 1) / camadas;

    
        for (int j = 0; j < fatias; j++)
        {
            // angulo azimutal: volta toda de 0 (norte) a 2PI (sul)
            float phi1 = 2.0f * M_PI * j / fatias;
            float phi2 = 2.0f * M_PI * (j + 1) / fatias;

            //para ST da esfera,
            float s1 = phi1 / (2.0f * M_PI); // coordenada s da textura, normalizada para 0,1
            float t1 = 1.0f - (theta1 / M_PI); // inverter t para alinhar a textura
            float s2 = phi2 / (2.0f * M_PI); 
            float t2 = 1.0f - (theta2 / M_PI); 

            // calcular os 4 vertices do quad com coordenadas esféricas
            // x = r * sin(theta) * sin(phi)
            //aqui agr na fase 4 como nas outras primitivas, além de calcular os vértices, agora tbm calculamos as normais e coordenadas de textura correspondentes para cada vértice.

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
                data.vertices.push_back(v1); data.normals.push_back(normalSphere(v1)); data.texCoords.push_back(Point2D(s1, t1));
                data.vertices.push_back(v2); data.normals.push_back(normalSphere(v2)); data.texCoords.push_back(Point2D(s1, t2));
                data.vertices.push_back(v3); data.normals.push_back(normalSphere(v3)); data.texCoords.push_back(Point2D(s2, t2));
            }
            // Triangulo 2: v1,v3,v4 (no polo sul v3=polo, v1,v4 distintos -> valido)
            // No polo norte v1=v4=polo -> degenera, skip
            if (i != 0)
            {
                data.vertices.push_back(v1); data.normals.push_back(normalSphere(v1)); data.texCoords.push_back(Point2D(s1, t1));
                data.vertices.push_back(v3); data.normals.push_back(normalSphere(v3)); data.texCoords.push_back(Point2D(s2, t2));
                data.vertices.push_back(v4); data.normals.push_back(normalSphere(v4)); data.texCoords.push_back(Point2D(s2, t1));
            }
        }
    }

    return data;
}

#endif