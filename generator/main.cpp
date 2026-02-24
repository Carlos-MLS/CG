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

void printUsage()
{
    cout << "Uso do Gerador de Primitivas:" << endl;
    cout << "  generator plane <length> <divisions> <output_file>" << endl;
    cout << "  generator box <size> <divisions> <output_file>" << endl;
    cout << "  generator sphere <radius> <slices> <stacks> <output_file>" << endl;
    cout << "  generator cone <radius> <height> <slices> <stacks> <output_file>" << endl;
    cout << endl;
    cout << "Exemplos:" << endl;
    cout << "  generator plane 5 10 plane.3d" << endl;
    cout << "  generator box 2 3 box.3d" << endl;
    cout << "  generator sphere 1 20 20 sphere.3d" << endl;
    cout << "  generator cone 1 2 20 10 cone.3d" << endl;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Número de argumentos errado!" << endl;
        printUsage();
        return 1;
    }

    string primitiva = argv[1];
    vector<Point3D> arrayVertices;

    try
    {
        if (strcmp(primitiva.c_str(), "plane") == 0)
        {
            if (argc != 5)
            {
                cerr << "Erro: Número incorreto de argumentos para 'plane'" << endl;
                printUsage();
                return 1;
            }

            float length = stof(argv[2]);
            int divisions = stoi(argv[3]);
            string outputPath = argv[4];

            arrayVertices = gerarPlane(length, divisions);
            if (!FileWriter::writeToFile(outputPath, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Plane gerado com sucesso: " << outputPath << endl;
        }
        else if (strcmp(primitiva.c_str(), "box") == 0)
        {
            if (argc != 5)
            {
                cerr << "Erro com Número incorreto de argumentos para 'box'" << endl;
                printUsage();
                return 1;
            }
            float size = stof(argv[2]);
            int divisions = stoi(argv[3]);
            string outputPath = argv[4];

            // gerar a box e escrever no ficheiro
            arrayVertices = gerarBox(size, divisions);
            if (!FileWriter::writeToFile(outputPath, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Box gerada com sucesso: " << outputPath << endl;
        }
        else if (strcmp(primitiva.c_str(), "sphere") == 0)
        {
            if (argc != 6)
            {
                cerr << "Erro com Número incorreto de argumentos para 'sphere'" << endl;
                printUsage();
                return 1;
            }
            float radius = stof(argv[2]);
            int slices = stoi(argv[3]);
            int stacks = stoi(argv[4]);
            string outputPath = argv[5];

            // gerar esfera
            arrayVertices = gerarSphere(radius, slices, stacks);

            if (!FileWriter::writeToFile(outputPath, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Esfera gerada com sucesso: " << outputPath << endl;
        }
        else if (strcmp(primitiva.c_str(), "cone") == 0)
        {
            if (argc != 7)
            {
                cerr << "Erro pq número incorreto de argumentos para 'cone'" << endl;
                printUsage();
                return 1;
            }
            float bottomRadius = stof(argv[2]);
            float height = stof(argv[3]);
            int slices = stoi(argv[4]);
            int stacks = stoi(argv[5]);
            string outputPath = argv[6];

            // gerar o cone
            arrayVertices = gerarCone(bottomRadius, height, slices, stacks);

            if (!FileWriter::writeToFile(outputPath, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Cone gerado com sucesso: " << outputPath << endl;
        }
        else
        {
            cerr << "Erro de Primitiva desconhecida '" << primitiva << "'" << endl;
            printUsage();
        }
    }
    catch (const exception &e)
    {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}