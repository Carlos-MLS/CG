#ifndef GEOMETRY_H
#define GEOMETRY_H

//Este file é para a Fase 4 porque precisamos da nova estrutura de dados, n chega o ponto
using namespace std;

#include "point.h" //para o Point3D e Point2D
#include <vector> //para usar vector

//esta struct é para guardar os dados de um modelo, ou seja, 
//os vértices, as normais e as coordenadas de textura, 
//para facilitar o acesso a esses dados na fase 4, 
//quando vamos precisar deles para desenhar os modelos com iluminação e texturas
// e desta maneira temos tudo numa Struct só.

struct geometryPointData {
    vector<Point3D> vertices; //x,y,z de cada vértice
    vector<Point3D> normals; //normais para cada vértice, para iluminação
    vector<Point2D> texCoords; //coordenadas de textura para cada vértice, S e T
};
  

#endif