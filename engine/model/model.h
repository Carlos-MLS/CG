#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "../../utils/point.h"

using namespace std;

struct Model
{
    string filename;
    vector<Point3D> vertices;
};

// carrega um ficheiro .3d e devolve o modelo
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
    file >> numVertices;

    for (int i = 0; i < numVertices; i++)
    {
        float x, y, z;
        file >> x >> y >> z;
        model.vertices.push_back(Point3D(x, y, z));
    }
    file.close();

    cout << "Modelo " << filename << " carregado (" << model.vertices.size() << " vertices)" << endl;

    return model;
}

#endif // MODEL_H
