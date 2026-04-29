#ifndef BEZIER_H
#define BEZIER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include "../../utils/point.h"

using namespace std;

//funcs para gerar os triângulos a partir dos patches de Bezier,
//são chamadas no final de gerarBezierPatches, depois de ler o ficheiro e guardar os patches e os pontos de controlo

//calcula os coeficientes de Bernstein para um dado t (entre 0 e 1)
//cálculos baseados na fórmula de Bernstein para curvas de Bezier cúbicas,
//vai tar no relatório, mas basicamente é uma função que recebe um t e devolve um array b de 4 floats
// com os coeficientes de Bernstein para esse t
inline void calcularBernstein(float t, float b[4]) {
    float it = 1.0f - t;
    b[0] = it * it * it;
    b[1] = 3.0f * t * it * it;
    b[2] = 3.0f * t * t * it;
    b[3] = t * t * t;
}

//calcula o ponto de Bezier para um dado u,v (entre 0 e 1) e um patch definido pelos seus índices e pelos pontos de controlo
//basicamente é a função que recebe um u e um v, e um patch
//
inline Point3D calcularPontoBezier(float u, float v, const vector<int>& idxPatch, const vector<Point3D>& controlPoints) {
    float bu[4], bv[4];
    calcularBernstein(u, bu); //calcula os coeficientes de Bernstein para u e v
    calcularBernstein(v, bv); //agora temos os coeficientes de Bernstein para u e v, podemos calcular o ponto de Bezier usando a fórmula de Bezier cúbica

    Point3D ponto(0.0f, 0.0f, 0.0f);

    //fórmula de Bezier cúbica é P(u,v) = Σ(i=0..3) Σ(j=0..3) B_i(u) * B_j(v) * P_ij
    for (int i = 0; i < 4; i++) {
        //para cada i e j, pegamos o ponto de controlo correspondente ao patch (usando os índices do patch para aceder aos pontos de controlo),
        // multiplicamos pelas Bernstein e somamos ao ponto final
        for (int j = 0; j < 4; j++) {
            const Point3D& cp = controlPoints[idxPatch[i * 4 + j]];
            float coef = bu[i] * bv[j];

            ponto.x += coef * cp.x;
            ponto.y += coef * cp.y;
            ponto.z += coef * cp.z;
        }
    }

    return ponto;
}

inline vector<Point3D> gerarBezierPatches(const string& patchFile, int tess) 
{
    //pegar no int no início para ter o nº de linhas
    //remover virgulas e espaços
    // guardar os valores de cada linha 

    if (tess < 1) tess = 1; //tessellation tem de ser pelo menos 1

    ifstream file(patchFile); //abrir o ficheiro para ler
    if (!file.is_open()) {
        cerr << "Erro ao abrir o file patch " << patchFile << endl;
        return {};
    }

    string lineIDK;

    //primeira line tem o num de patches , são 16 pontos por linha
    if (!getline(file, lineIDK)) {
        cerr << "falha ao ler o numero de patches" << endl;
        return {};
    }
    int numPatches = stoi(lineIDK);
    vector<vector<int>> patches;

    patches.reserve(numPatches); //reservar espaço para os patches
    //para prencher a seguir;

    //iteramos agora para ler os patches
    for (int i = 0; i < numPatches; i++) {
        if (!getline(file, lineIDK)) {
            cerr << "falha aqui " << i << endl; //failsafe
            return {}; 
        }
        //agora remover espaços e virgulas

        //primeiro trocamos as virgulas por espaços para facilitar o split
        replace(lineIDK.begin(), lineIDK.end(), ',', ' '); 

        //agora temos uma linha com 16 ints separados por espaços
        stringstream stringStream(lineIDK);

        vector<int> idxPatch(16);

        for (int j = 0; j < 16; j++) {
            //ler cada int para o vetor patch
            stringStream >> idxPatch[j]; 
            //podia colocar um failsafe aqui para verificar se leu mesmo 16 ints, 
            //mas assumo que o ficheiro está bem formatado (pelo menos
            //o que os profs forneceram eram)
        }
       //adicionar o patch à lista de patches
        patches.push_back(idxPatch); 
    }

    //agora ler o num de pontos de controlo

    int numControlPoints; 
    lineIDK.clear(); //limpar a string para ler a próxima linha

     //ler a linha com o num de pontos de controlo
    if (!getline(file, lineIDK)) {
        cerr << "falha ao ler o num de pontos de controlo" << endl;
        return {};
    }
    numControlPoints = stoi(lineIDK);

    vector<Point3D> controlPoints;
    controlPoints.reserve(numControlPoints); //reservar espaço para os pontos de controlo

    //agora ler os pontos de controlo
    for (int i = 0; i < numControlPoints; i++) {
        //ler a linha com as coordenadas do ponto de controlo para lineIDK
        if (!getline(file, lineIDK)) {
            cerr << "falha ao ler o ponto de controlo " << i << endl;
            return {};
        }

        //fazer a mesma coisa de antes para remover as virgulas e facilitar o split
        replace(lineIDK.begin(), lineIDK.end(), ',', ' ');
        stringstream stringStream(lineIDK);

        //ler as coordenadas x,y,z do ponto de controlo
        float x, y, z;

        //msma coisa aqui, podia colocar um failsafe para verificar se leu mesmo 3 floats, mas assumo que o ficheiro está bem formatado
        stringStream >> x >> y >> z;
        controlPoints.push_back(Point3D(x, y, z));
    }

    //TODO: verificar se os índices dos patches são válidos (entre 0 e numControlPoints-1), 
    //mas assumo que o ficheiro está bem formatado

    //agr gerar os triângulos Bezier
    

    vector<Point3D> vertices; //vetor para guardar os vértices dos triângulos gerados
    float passo = 1.0f / (float)tess; //calcular o passo para iterar sobre u e v

    //iterar sobre os patches, para cada patch iterar sobre u e v de 0 a 1 com o passo calculado, 
    //e calcular os pontos de Bezier para cada combinação de u e v usando a função calcularPontoBezier,
    //depois adicionar os triângulos ao vetor de vértices (triângulo 1: p1,p2,p3 e triângulo 2: p1,p3,p4)
    for (int p = 0; p < numPatches; p++) {
        const vector<int>& idxPatch = patches[p]; //obter os índices do patch atual

        //iterar sobre u e v de 0 a 1 com o passo calculado
        for (int uStep = 0; uStep < tess; uStep++) {
            float u = uStep * passo;
            for (int vStep = 0; vStep < tess; vStep++) {
                float v = vStep * passo;
                //calcular os 4 pontos do quad usando a função calcularPontoBezier
                Point3D p1 = calcularPontoBezier(u, v, idxPatch, controlPoints);
                Point3D p2 = calcularPontoBezier(u + passo, v, idxPatch, controlPoints);
                Point3D p3 = calcularPontoBezier(u + passo, v + passo, idxPatch, controlPoints);
                Point3D p4 = calcularPontoBezier(u, v + passo, idxPatch, controlPoints);

                //adicionar os triângulos ao vetor de vértices (triângulo 1: p1,p4,p2 e triângulo 2: p2,p4,p3)
                vertices.push_back(p1);
                vertices.push_back(p4);
                vertices.push_back(p2);

                vertices.push_back(p2);
                vertices.push_back(p4);
                vertices.push_back(p3);
            }
        }
    }
    return vertices;
}


#endif