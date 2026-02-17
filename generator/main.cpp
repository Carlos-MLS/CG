#include <iostream>
#include <cstring>
#include <vector>
#include "../utils/point.h"
#include "../utils/fileWriter.h"
#include "primitives/plane.h"
#include "primitives/box.h"
#include "primitives/sphere.h"
#include "primitives/cone.h"

using namespace std;



void printUsage() {
    cout << "Uso do Gerador de Primitivas:" << endl;
    cout << "  generator plane <length> <divisions> <output_file>" << endl;
    cout << "  generator box <x> <y> <z> <divisions> <output_file>" << endl;
    cout << "  generator sphere <radius> <slices> <stacks> <output_file>" << endl;
    cout << "  generator cone <radius> <height> <slices> <stacks> <output_file>" << endl;
    cout << endl;
    cout << "Exemplos:" << endl;
    cout << "  generator plane 5 10 plane.3d" << endl;
    cout << "  generator box 2 3 4 5 box.3d" << endl;
    cout << "  generator sphere 1 20 20 sphere.3d" << endl;
    cout << "  generator cone 1 2 20 10 cone.3d" << endl;
}


int main (int argc, char** argv) {
    if (argc < 2) {
        cerr << "Número de argumentos errado!" << endl;
        printUsage();
        return 1;
    }

    string primitiva = argv[1];
    vector<Point3D> arrayVertices;

    try {
        if (strcmp(primitiva.c_str(),"plane") == 0) {
            if (argc != 5) {
                cerr << "Erro: Número incorreto de argumentos para 'plane'" << endl;
                printUsage();
                return 1;
            }
            //args para o plane
            float length  = stof(argv[2]);
            int divisions = stoi(argv[3]);
            string outputPath = argv[4];

            //lógica para o para gerar o plane aqui
            arrayVertices = gerarPlane(length, divisions);
            if (!FileWriter::writeToFile(outputPath, arrayVertices)) {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
        }
        else if (strcmp(primitiva.c_str(),"box") == 0) {
            if (argc != 7) {
                cerr << "Erro com Número incorreto de argumentos para 'box'" << endl;
                printUsage();
                return 1;
            }
            //args para box
            float x = stof(argv[2]);
            float y = stof(argv[3]);
            float z = stof(argv[4]);
            int divisions =  stoi(argv[5]);
            string outputPath = argv[6];

            //lógica para box


        }
        else if (strcmp(primitiva.c_str(),"sphere") == 0) {
            if (argc != 6) {
                cerr << "Erro com Número incorreto de argumentos para 'sphere'" << endl;
                printUsage();
                return 1;
            }
            //args para sphere
            float radius = stof(argv[2]);
            int slices = stoi(argv[3]);
            int stacks  = stoi(argv[4]);
            string outputPath = argv[5];

            //lógica para sphere aqui agr
            // TODO: Implementar gerarSphere(radius, slices, stacks)
            // arrayVertices = gerarSphere(radius, slices, stacks);

            if (!FileWriter::writeToFile(outputPath, arrayVertices)) {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
        }
        else if (strcmp(primitiva.c_str(),"cone") == 0) {
            if (argc != 7) {
                cerr << "Erro pq número incorreto de argumentos para 'cone'" << endl;
                printUsage();
                return 1;
            }
            float bottomRadius = stof(argv[2]);
            float height = stof(argv[3]);
            int slices = stoi(argv[4]);
            int stacks  = stoi(argv[5]);
            string outputPath = argv[6];

            //lógica para o cone agr
            // TODO: Implementar gerarCone(bottomRadius, height, slices, stacks)
            // arrayVertices = gerarCone(bottomRadius, height, slices, stacks);

            if (!FileWriter::writeToFile(outputPath, arrayVertices)) {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Cone gerado com sucesso: " << outputPath << endl;
        }
        else {
            cerr <<  "Erro de Primitiva desconhecida '" << primitiva << "'" << endl;
            printUsage();
        }
    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}