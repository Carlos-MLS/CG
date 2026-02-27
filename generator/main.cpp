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
    cout << "  generator plane <comprimento> <divisoes> <ficheiro_saida>" << endl;
    cout << "  generator box <tamanho> <divisoes> <ficheiro_saida>" << endl;
    cout << "  generator sphere <raio> <fatias> <camadas> <ficheiro_saida>" << endl;
    cout << "  generator cone <raio> <altura> <fatias> <camadas> <ficheiro_saida>" << endl;
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

            float comprimento = stof(argv[2]);
            int divisoes = stoi(argv[3]);
            string caminhoSaida = argv[4];

            arrayVertices = gerarPlane(comprimento, divisoes);
            if (!FileWriter::writeToFile(caminhoSaida, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Plane gerado com sucesso: " << caminhoSaida << endl;
        }
        else if (strcmp(primitiva.c_str(), "box") == 0)
        {
            if (argc != 5)
            {
                cerr << "Erro com Número incorreto de argumentos para 'box'" << endl;
                printUsage();
                return 1;
            }
            float tamanho = stof(argv[2]);
            int divisoes = stoi(argv[3]);
            string caminhoSaida = argv[4];

            // gerar a box e escrever no ficheiro
            arrayVertices = gerarBox(tamanho, divisoes);
            if (!FileWriter::writeToFile(caminhoSaida, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Box gerada com sucesso: " << caminhoSaida << endl;
        }
        else if (strcmp(primitiva.c_str(), "sphere") == 0)
        {
            if (argc != 6)
            {
                cerr << "Erro com Número incorreto de argumentos para 'sphere'" << endl;
                printUsage();
                return 1;
            }
            float raio = stof(argv[2]);
            int fatias = stoi(argv[3]);
            int camadas = stoi(argv[4]);
            string caminhoSaida = argv[5];

            // gerar esfera
            arrayVertices = gerarSphere(raio, fatias, camadas);

            if (!FileWriter::writeToFile(caminhoSaida, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Esfera gerada com sucesso: " << caminhoSaida << endl;
        }
        else if (strcmp(primitiva.c_str(), "cone") == 0)
        {
            if (argc != 7)
            {
                cerr << "Erro pq número incorreto de argumentos para 'cone'" << endl;
                printUsage();
                return 1;
            }
            float raioBase = stof(argv[2]);
            float altura = stof(argv[3]);
            int fatias = stoi(argv[4]);
            int camadas = stoi(argv[5]);
            string caminhoSaida = argv[6];

            // gerar o cone
            arrayVertices = gerarCone(raioBase, altura, fatias, camadas);

            if (!FileWriter::writeToFile(caminhoSaida, arrayVertices))
            {
                cerr << "Erro ao escrever o ficheiro" << endl;
                return 1;
            }
            cout << "Cone gerado com sucesso: " << caminhoSaida << endl;
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