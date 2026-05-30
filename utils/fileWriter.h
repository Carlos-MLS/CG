#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <iostream>
#include <fstream>
#include <vector>
#include "point.h"
#include "geometry.h"

using namespace std;
/*"Classe responsável por escrever para os files*/

//agr para esta fase 4 n vamos escrever vector<Point3D> mas sim geometryPointData, para guardar as normais e coordenadas de textura tbm, 
//mas a ideia geral é escrever o num vertice, dps normais e de seguida as coordenadas de textura, 
//para depois ler isso no parser e usar para desenhar os modelos com iluminação e texturas

class FileWriter {
public:
    static bool writeToFile(const string& filename, const geometryPointData& data) {
        //verificar se o número de vértices,
        //normais e coordenadas de textura são os mesmos, porque depois na leitura vamos assumir que estão alinhados
        //o vértice i tem a normal i e a coordenada de textura i
        if (data.vertices.size() != data.normals.size() || data.vertices.size() != data.texCoords.size()) {
            cerr << "Falha aqui(numVertices, numNormais, numTexCoords) são diferentes: " << data.vertices.size() << ", " << data.normals.size() << ", " << data.texCoords.size() << endl;
            return false;
        }

        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Não foi possível criar o file " << filename << endl;
            return false;
        }

        int numVertices = (int)data.vertices.size();

        file << numVertices << endl; //escrever o número de vértices no início do file, para facilitar a leitura depois

        //vertices
        for (const auto& vertex : data.vertices) {
            file << vertex.x << " " << vertex.y << " " << vertex.z << endl;
        }

        //normais
        for (const auto& normal : data.normals) {
            file << normal.x << " " << normal.y << " " << normal.z << endl;

        }

        //coordenadas de textura
        for (const auto& texCoord : data.texCoords) {
            file << texCoord.s << " " << texCoord.t << endl;
        }

        file.close();

        return true;
    }
};

#endif
