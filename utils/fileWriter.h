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
        if (!file.is_open()) {
            cerr << "Não foi possível criar o file " << filename << endl;
            return false;
        }

        file << vertices.size() << endl;

        for (const auto& vertex : vertices) {
            file << vertex.x << " " << vertex.y << " " << vertex.z << endl;
        }

        file.close();

        return true;
    }
};

#endif
