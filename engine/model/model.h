#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "../../utils/point.h"

using namespace std;

// dados lidos do .3d (vertices, normais e coordenadas de textura)
struct Model
{
    string filename;
    vector<Point3D> vertices;
    vector<Point3D> normals;
    vector<Point2D> texCoords;
};

// carrega um ficheiro .3d e devolve o modelo
// formato: N vertices -> N linhas de vertices -> N linhas de normais -> N linhas de texcoords
inline Model loadModel(const string &filename)
{
    Model model;
    model.filename = filename;

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Nao consegui abrir: " << filename << endl;
        return model;
    }

    int numVertices;
    if (!(file >> numVertices))
    {
        cerr << "Erro ao ler numero de vertices: " << filename << endl;
        return model;
    }

    model.vertices.reserve(numVertices);
    model.normals.reserve(numVertices);
    model.texCoords.reserve(numVertices);

    // 1) vertices
    for (int i = 0; i < numVertices; i++)
    {
        float x, y, z;
        if (!(file >> x >> y >> z))
        {
            cerr << "Erro ao ler vertices: " << filename << endl;
            model.vertices.clear();
            return model;
        }
        model.vertices.push_back(Point3D(x, y, z));
    }

    // 2) normais (se faltar, assumimos ficheiro antigo)
    bool hasNormals = true;
    for (int i = 0; i < numVertices; i++)
    {
        float nx, ny, nz;
        if (!(file >> nx >> ny >> nz))
        {
            hasNormals = false;
            break;
        }
        model.normals.push_back(Point3D(nx, ny, nz));
    }

    if (!hasNormals)
    {
        model.normals.clear();
        model.texCoords.clear();
        file.close();
        return model;
    }

    // 3) texcoords (opcional)
    bool hasTexCoords = true;
    for (int i = 0; i < numVertices; i++)
    {
        float s, t;
        if (!(file >> s >> t))
        {
            hasTexCoords = false;
            break;
        }
        model.texCoords.push_back(Point2D(s, t));
    }

    if (!hasTexCoords)
    {
        model.texCoords.clear();
    }
    file.close();

    cout << "Modelo " << filename << " carregado (" << model.vertices.size() << " vertices)" << endl;

    return model;
}

#endif
