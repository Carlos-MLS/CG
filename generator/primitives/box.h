#ifndef BOX_H
#define BOX_H

#include <vector>
#include "../../utils/point.h"
#include "../../utils/geometry.h"
using namespace std;

//usar geometryPointData para a fase 4, para guardar as normais e coordenadas de textura
//até à fase 3 só era preciso vector<Point3D> porque só tínhamos os vértices


inline geometryPointData gerarBox(float tamanho, int divisoes)
{
    geometryPointData data;
    //por ex para a face frontal fazemos z=+metade, para a face traseira z=-metade, e as normais da face frontal apontam para frente (0,0,1) e as normais da face traseira apontam para trás (0,0,-1), e assim por diante para as outras faces do box, usando os sinais das coordenadas x,y,z para determinar a direção das normais
    if (divisoes < 1)
    {
        divisoes = 1;
    }

    float metade = tamanho / 2.0f;

    float passo = tamanho / divisoes;

    auto map01 = [&](float v) {
        return (v + metade) / tamanho;
    };

    auto pushVertex = [&](const Point3D& p, const Point3D& n, float s, float t) {
        data.vertices.push_back(p);
        data.normals.push_back(n);
        data.texCoords.push_back(Point2D(s, t));
    };

    for (int i = 0; i < divisoes; i++)
    {
        for (int j = 0; j < divisoes; j++)
        {

            float x1 = -metade + i * passo;
            float x2 = -metade + (i + 1) * passo;
            float y1 = -metade + j * passo;
            float y2 = -metade + (j + 1) * passo;

            // para cada face do box, calculamos os vértices, as normais e as coordenadas de textura correspondentes, 
            //e adicionamos à struct geometryPointData, para depois escrever no file e ler no parser para desenhar o box com iluminação e texturas

            // Face frontal (z = +metade)
            Point3D nFront(0.0f, 0.0f, 1.0f);
            float s1 = map01(x1);
            float s2 = map01(x2);
            float t1 = map01(y1);
            float t2 = map01(y2);

            pushVertex(Point3D(x1, y1, metade), nFront, s1, t1);
            pushVertex(Point3D(x2, y1, metade), nFront, s2, t1);
            pushVertex(Point3D(x2, y2, metade), nFront, s2, t2);

            pushVertex(Point3D(x1, y1, metade), nFront, s1, t1);
            pushVertex(Point3D(x2, y2, metade), nFront, s2, t2);
            pushVertex(Point3D(x1, y2, metade), nFront, s1, t2);

            // Face traseira (z = -metade)
            Point3D nBack(0.0f, 0.0f, -1.0f);
            pushVertex(Point3D(x2, y1, -metade), nBack, s2, t1);
            pushVertex(Point3D(x1, y1, -metade), nBack, s1, t1);
            pushVertex(Point3D(x2, y2, -metade), nBack, s2, t2);

            pushVertex(Point3D(x1, y1, -metade), nBack, s1, t1);
            pushVertex(Point3D(x1, y2, -metade), nBack, s1, t2);
            pushVertex(Point3D(x2, y2, -metade), nBack, s2, t2);

            // Faces laterais (X)
            float z1 = -metade + i * passo;
            float z2 = -metade + (i + 1) * passo;

            Point3D nRight(1.0f, 0.0f, 0.0f);
            float s3 = map01(z1);
            float s4 = map01(z2);

            pushVertex(Point3D(metade, y1, z2), nRight, s4, t1);
            pushVertex(Point3D(metade, y1, z1), nRight, s3, t1);
            pushVertex(Point3D(metade, y2, z2), nRight, s4, t2);

            pushVertex(Point3D(metade, y1, z1), nRight, s3, t1);
            pushVertex(Point3D(metade, y2, z1), nRight, s3, t2);
            pushVertex(Point3D(metade, y2, z2), nRight, s4, t2);

            Point3D nLeft(-1.0f, 0.0f, 0.0f);
            pushVertex(Point3D(-metade, y1, z1), nLeft, s3, t1);
            pushVertex(Point3D(-metade, y1, z2), nLeft, s4, t1);
            pushVertex(Point3D(-metade, y2, z2), nLeft, s4, t2);

            pushVertex(Point3D(-metade, y1, z1), nLeft, s3, t1);
            pushVertex(Point3D(-metade, y2, z2), nLeft, s4, t2);
            pushVertex(Point3D(-metade, y2, z1), nLeft, s3, t2);

            // Faces de cima e baixo (Y)
            float zt1 = -metade + j * passo;
            float zt2 = -metade + (j + 1) * passo;
            float t3 = map01(zt1);
            float t4 = map01(zt2);

            Point3D nTop(0.0f, 1.0f, 0.0f);
            pushVertex(Point3D(x1, metade, zt1), nTop, s1, t3);
            pushVertex(Point3D(x1, metade, zt2), nTop, s1, t4);
            pushVertex(Point3D(x2, metade, zt2), nTop, s2, t4);

            pushVertex(Point3D(x1, metade, zt1), nTop, s1, t3);
            pushVertex(Point3D(x2, metade, zt2), nTop, s2, t4);
            pushVertex(Point3D(x2, metade, zt1), nTop, s2, t3);

            Point3D nBottom(0.0f, -1.0f, 0.0f);
            pushVertex(Point3D(x2, -metade, zt1), nBottom, s2, t3);
            pushVertex(Point3D(x2, -metade, zt2), nBottom, s2, t4);
            pushVertex(Point3D(x1, -metade, zt1), nBottom, s1, t3);

            pushVertex(Point3D(x2, -metade, zt2), nBottom, s2, t4);
            pushVertex(Point3D(x1, -metade, zt2), nBottom, s1, t4);
            pushVertex(Point3D(x1, -metade, zt1), nBottom, s1, t3);
        }
    }

    return data;
}

#endif