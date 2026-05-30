#ifndef GROUP_H
#define GROUP_H

#include <vector> 
#include <string>
#include "transform.h"

using namespace std;
// cor RGB normalizada (0..1) para os materiais
struct Color3 {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    Color3() = default;
    Color3(float r, float g, float b) : r(r), g(g), b(b) {}
};

// material do OpenGL (cores + brilho)
struct Material {
    Color3 diffuse = Color3(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f);
    Color3 ambient = Color3(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f);
    Color3 specular = Color3(0.0f, 0.0f, 0.0f);
    Color3 emissive = Color3(0.0f, 0.0f, 0.0f);
    float shininess = 0.0f;
};

// info de um modelo no XML (ficheiro + material + textura)
struct ModelInfo {
    string file;
    string textureFile; // caminho da textura (opcional)
    bool hasTexture = false; // true se tiver textura definida
    Material material;
};

// grupo de cena com transformacoes, modelos e subgrupos
struct Group {
    vector<Transform> transforms;
    vector<ModelInfo> models;
    vector<Group> children;
};

#endif