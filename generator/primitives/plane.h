#ifndef PLANE_H
#define PLANE_H
#include <vector>

#include "../../utils/point.h"
#include "../../utils/geometry.h"

using namespace std;


//para a fase 4 vamos mudar de vector<Point3D> para a
//strct geometryPointData, para guardar as normais e coordenadas de textura.

//até à fase 3 só era preciso vector<Point3D> porque só tínhamos os vértices
//agr na fase 4 além dos vertices calculamos tbm as normais,
// que para um plano são sempre as mesmas, apontando para cima
// e as coordenadas de textura (que podem ser calculadas com base nas coordenadas x e z dos vértices, 
// normalizadas para o intervalo [0,1] usando o comprimento do plano)

inline geometryPointData gerarPlane(float comprimento, int divisoes)
{
    geometryPointData data;

    Point3D normal(0.0f, 1.0f, 0.0f); //normal do plano, aponta para cima
    

    if (divisoes < 1)
    {
        divisoes = 1;
    }

    float tamSubDivisao = comprimento / divisoes;

    float metade = comprimento / 2.0f;

    //para gerar os vertices do plano.
    for (int i = 0; i < divisoes; i++)
    {
        for (int j = 0; j < divisoes; j++)
        {
            
            float x1 = -metade + i * tamSubDivisao; // coordenada x do vértice
            float x2 = -metade + (i + 1) * tamSubDivisao; // coordenada x do vértice diferente do anterior, para formar o quad
            float z1 = -metade + j * tamSubDivisao; // coordenada z do vértice
            float z2 = -metade + (j + 1) * tamSubDivisao; // coordenada z do vértice diferente do anterior, para formar o quad

            float y = 0.0f;


            //calcular agora para esta fase4 as normais e coordenadas de textura para cada vértice, e adicionar à struct geometryPointData
            // para um plano as normais são sempre as mesmas (aponta para cima (0,1,0))
            // as coordenadas de textura podem ser calculadas com base nas coordenadas x e z dos vértices, normalizadas para o intervalo [0,1] usando o comprimento do plano

            float s1 = float(i) / divisoes; // coordenada s da textura, normalizada para [0,1]
            float s2 = float(i + 1) / divisoes; // coordenada s da textura, normalizada  tbm
            float t1 = float(j) / divisoes; // coordenada t da textura, normalizada 
            float t2 = float(j + 1) / divisoes; // coordenada t da textura, normalizada 
            
            Point3D v1(x1, y, z1);
            Point3D v2(x1, y, z2);
            Point3D v3(x2, y, z2);
            Point3D v4(x2, y, z1);


            data.vertices.push_back(v1);
            data.normals.push_back(normal);
            data.texCoords.push_back(Point2D(s1, t1));

            data.vertices.push_back(v2);
            data.normals.push_back(normal);
            data.texCoords.push_back(Point2D(s1, t2));

            data.vertices.push_back(v3);
            data.normals.push_back(normal);
            data.texCoords.push_back(Point2D(s2, t2));

            data.vertices.push_back(v1);
            data.normals.push_back(normal);
            data.texCoords.push_back(Point2D(s1, t1));

            data.vertices.push_back(v3);
            data.normals.push_back(normal);
            data.texCoords.push_back(Point2D(s2, t2));

            data.vertices.push_back(v4);
            data.normals.push_back(normal);
            data.texCoords.push_back(Point2D(s2, t1));
        }
    }

    return data;
}

#endif