#ifndef CONE_H
#define CONE_H

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include "../../utils/point.h"
#include "../../utils/geometry.h"

using namespace std;

//same as others 

inline geometryPointData gerarCone(float raioBase, float altura, int fatias, int camadas)
{
    geometryPointData data;
    Point3D normalBase(0.0f, -1.0f, 0.0f); //normal da base do cone, aponta para baixo
    //para as normais da parte lateral do cone, podemos calcular a normal de cada triângulo usando o produto vetorial entre os vetores dos lados do triângulo, ou seja
    // para um triângulo formado pelos vértices v1,v2,v3, os vetores dos lados são l1 = v2 - v1 e l2 = v3 - v1, e a normal é n = l1 x l2 (produto vetorial),
    // normalizada para ter comprimento 1 (demos na aula)

    // para as coordenadas de textura da base do cone, podemos usar as coordenadas polares dos vértices em relação ao centro da base
    //dps explicar melhor no relatório, mas basicamente a coordenada s da textura pode ser calculada 
    //com base no ângulo azimutal do ponto em relação ao centro do cone, e a coordenada t da textura pode ser calculada com base na distância do ponto ao centro do cone na base, normalizada para o intervalo 0,1 usando o raio da base do cone
    auto stBase = [&](const Point3D& p) {
        float s = (atan2(p.z, p.x) + M_PI) / (2.0f * M_PI); // coordenada s da textura, baseada no ângulo azimutal do ponto em relação ao centro do cone, normalizada para [0,1]
        float t = sqrt(p.x * p.x + p.z * p.z) / raioBase; // coordenada t da textura, baseada na distância do ponto ao centro do cone na base, normalizada para [0,1]
        return Point2D(s, t);
    };


    auto normalLateral = [&](const Point3D& p) {
        Point3D normal(p.x, raioBase / altura, p.z); // normal lateral do cone, baseada na direção do vetor do centro do cone para o ponto na lateral, com a componente y ajustada para criar a inclinação do cone
        float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (length == 0.0f) return Point3D(0.0f, 1.0f, 0.0f);
        return Point3D(normal.x / length, normal.y / length, normal.z / length); // normalizada para ter comprimento 1
    };

    float s, t;
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

        // Triângulo da base do cone, formado pelo centro da base e os dois pontos na borda da base correspondentes à fatia atual
        Point3D p1(raioBase * cos(a1), 0.0f, raioBase * sin(a1));
        Point3D p2(raioBase * cos(a2), 0.0f, raioBase * sin(a2));

        data.vertices.push_back(centro);data.normals.push_back(normalBase); data.texCoords.push_back(Point2D(0.5f, 0.5f)); // centro da base tem coordenadas de textura (0.5, 0.5) para centralizar a textura na base
        data.vertices.push_back(p1);data.normals.push_back(normalBase); data.texCoords.push_back(stBase(p1)); // ponto na borda da base, com coordenadas de textura calculadas pela função stBase
        data.vertices.push_back(p2);data.normals.push_back(normalBase); data.texCoords.push_back(stBase(p2)); // ponto na borda da base, com coordenadas de textura calculadas pela função stBase
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

            // Triângulo lateral do cone, formado por dois pontos na borda da camada atual e um ponto na borda da próxima camada, correspondentes à fatia atual
            Point3D v1(r1 * cos(a1), y1, r1 * sin(a1));
            Point3D v2(r1 * cos(a2), y1, r1 * sin(a2));
            Point3D v3(r2 * cos(a2), y2, r2 * sin(a2));
            Point3D v4(r2 * cos(a1), y2, r2 * sin(a1));

            data.vertices.push_back(v1);data.normals.push_back(normalLateral(v1)); data.texCoords.push_back(Point2D((float)j / fatias, (float)i / camadas)); // coordenadas de textura baseadas na posição da fatia e camada, normalizadas para 0,1
            data.vertices.push_back(v3);data.normals.push_back(normalLateral(v3)); data.texCoords.push_back(Point2D((float)(j + 1) / fatias, (float)(i + 1) / camadas));
            data.vertices.push_back(v2);data.normals.push_back(normalLateral(v2)); data.texCoords.push_back(Point2D((float)(j + 1) / fatias, (float)i / camadas));

            if (i != camadas - 1)
            {
                data.vertices.push_back(v1);data.normals.push_back(normalLateral(v1)); data.texCoords.push_back(Point2D((float)j / fatias, (float)i / camadas));
                data.vertices.push_back(v4);data.normals.push_back(normalLateral(v4)); data.texCoords.push_back(Point2D((float)j / fatias, (float)(i + 1) / camadas));
                data.vertices.push_back(v3);data.normals.push_back(normalLateral(v3)); data.texCoords.push_back(Point2D((float)(j + 1) / fatias, (float)(i + 1) / camadas));
            }
        }
    }

    return data;
}

#endif