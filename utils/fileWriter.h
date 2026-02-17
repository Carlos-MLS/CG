//
// Created by francisco0504 on 17/02/26.
//
#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <iostream>
#include <fstream>
#include <vector>
#include "point.h"

using namespace std;
/*"Classe responsável por escrever para os files*/

class FileWriter {
public:
    static bool writeToFile(const string& filename, const vector<Point3D>& vertices) {
        ofstream file(filename);
        ///file not opened
        if (!file.is_open()) {
            cerr << "Não foi possível criar o file " << filename << endl;
            return false;
        }

        // Escrever número de vértices
        file << vertices.size() << endl;

        // Escrever cada vértice
        for (const auto& vertex : vertices) {
            file << vertex.x << " " << vertex.y << " " << vertex.z << endl;
        }

        file.close();

        return true;
    }
};

#endif //FILEWRITER_H
