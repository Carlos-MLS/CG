#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include <vector>
#include <tinyxml2.h>
#include <iostream>
#include "../scene/group.h"

using namespace std;
using namespace tinyxml2;

// configuracao da janela definida no XML
struct WindowConfig
{
    int width = 512;
    int height = 512;
};

// configuracao da camera (posicao, alvo, up e projecao)
struct CameraConfig
{
    float posX = 3, posY = 2, posZ = 1;
    float lookAtX = 0, lookAtY = 0, lookAtZ = 0;
    float upX = 0, upY = 1, upZ = 0;
    float fov = 60, near = 1, far = 1000;
};

// tipos de luz suportados no XML
enum class LightType {
    Point,
    Directional,
    Spot
};

// dados de uma luz (os campos usados variam por tipo)
struct Light
{
    LightType type = LightType::Point;
    Point3D position; // point/spot
    Point3D direction; // directional/spot
    float cutoff = 0.0f; // spot
};

// configuração global da cena
struct WorldConfig
{
    WindowConfig window;
    CameraConfig camera;
    vector<Light> lights;
    Group rootGroup;
};

// converte valores 0..255 para 0..1 e aplica defaults se faltar alguma cor
inline Color3 parseColor255(XMLElement *elem, float defR, float defG, float defB)
{
    float r = defR, g = defG, b = defB;
    if (elem)
    {
        elem->QueryFloatAttribute("R", &r);
        elem->QueryFloatAttribute("G", &g);
        elem->QueryFloatAttribute("B", &b);
    }
    return Color3(r / 255.0f, g / 255.0f, b / 255.0f); // normalizar
}

// ler floats com compatibilidade para atributos em minusculas
// isto foi pq alguns XMLs antigos tinham os atributos em minusculas (x,y,z)
// e outros em maiusculas (X,Y,Z), entao tentamos ler ambos

inline bool queryFloatAttrAny(XMLElement* elem, const char* upper, const char* lower, float* out)
{
    if (elem->QueryFloatAttribute(upper, out) == XML_SUCCESS) return true;
    if (elem->QueryFloatAttribute(lower, out) == XML_SUCCESS) return true;
    return false;
}

// parse de um <group>: transforma em estrutura recursiva com transformacoes, modelos e filhos
inline Group parseGroup(XMLElement *groupElem) {
    Group group;

    // 1) transformacoes (opcional)
    XMLElement *transform = groupElem->FirstChildElement("transform");
    if (transform) {
        for (XMLElement *t = transform->FirstChildElement(); t; t = t->NextSiblingElement()) {
            string name = t->Value();
            Transform tr;
            if (name == "translate") {
                tr.type = TransformType::Translate;

                // se tiver "time", e uma translate animada por curva Catmull-Rom
                if (t->QueryFloatAttribute("time", &tr.time) == XML_SUCCESS && tr.time > 0.0f) {
                    tr.hasTime = true;

                    const char* align = t->Attribute("align");
                    if (!align) align = t->Attribute("alignToPath");
                    if (align) {
                        string alignValue = align;
                        tr.alignToPath = (alignValue == "true" || alignValue == "True" || alignValue == "TRUE" || alignValue == "1");
                    }

                    // pontos de controlo da curva (novo formato)
                    for (XMLElement* cp = t->FirstChildElement("point"); cp; cp = cp->NextSiblingElement("point")) {
                        Point3D point;
                        cp->QueryFloatAttribute("x", &point.x);
                        cp->QueryFloatAttribute("y", &point.y);
                        cp->QueryFloatAttribute("z", &point.z);
                        tr.controlPoints.push_back(point);
                    }

                    // compatibilidade com XMLs antigos (controlPoint)
                    if (tr.controlPoints.empty()) {
                        for (XMLElement* cp = t->FirstChildElement("controlPoint"); cp; cp = cp->NextSiblingElement("controlPoint")) {
                            Point3D point;
                            cp->QueryFloatAttribute("x", &point.x);
                            cp->QueryFloatAttribute("y", &point.y);
                            cp->QueryFloatAttribute("z", &point.z);
                            tr.controlPoints.push_back(point);
                        }
                    }

                    // se houver menos de 4 pontos, cai para translate estatica
                    if (tr.controlPoints.size() < 4) {
                        tr.hasTime = false;
                        tr.time = 0.0f;
                        t->QueryFloatAttribute("x", &tr.x);
                        t->QueryFloatAttribute("y", &tr.y);
                        t->QueryFloatAttribute("z", &tr.z);
                    }
                }
                else {
                    // translate normal (sem animacao)
                    t->QueryFloatAttribute("x", &tr.x);
                    t->QueryFloatAttribute("y", &tr.y);
                    t->QueryFloatAttribute("z", &tr.z);
                }

                group.transforms.push_back(tr);
            } else if (name == "rotate") {
                tr.type = TransformType::Rotate;
                t->QueryFloatAttribute("x", &tr.x);
                t->QueryFloatAttribute("y", &tr.y);
                t->QueryFloatAttribute("z", &tr.z);

                // se tiver "time", e uma rotacao animada (360o no tempo)
                if (t->QueryFloatAttribute("time", &tr.time) == XML_SUCCESS && tr.time > 0.0f) {
                    tr.hasTime = true;
                }
                else {
                    // caso contrario usa o angulo fixo
                    t->QueryFloatAttribute("angle", &tr.angle);
                }

                group.transforms.push_back(tr);
            } else if (name == "scale") {
                tr.type = TransformType::Scale;
                // scale simples (x,y,z)
                t->QueryFloatAttribute("x", &tr.x);
                t->QueryFloatAttribute("y", &tr.y);
                t->QueryFloatAttribute("z", &tr.z);
                group.transforms.push_back(tr);
            }
        }
    }

    // 2) models (ficheiro e obrigatorio; textura/cores sao opcionais)
    XMLElement *models = groupElem->FirstChildElement("models");
    if (models) {
        for (XMLElement *m = models->FirstChildElement("model"); m; m = m->NextSiblingElement("model")) {
            const char *file = m->Attribute("file");
            if (!file) continue;

            ModelInfo info;
            info.file = file;

            XMLElement *tex = m->FirstChildElement("texture"); // textura opcional
            if (tex) {
                const char *texFile = tex->Attribute("file");
                if (texFile) {
                    info.textureFile = texFile;
                    info.hasTexture = true;
                }
            }

            XMLElement *color = m->FirstChildElement("color"); // cores opcionais
            if (color) {
                // cada componente usa defaults se faltar no XML
                info.material.diffuse = parseColor255(color->FirstChildElement("diffuse"), 200.0f, 200.0f, 200.0f);
                info.material.ambient = parseColor255(color->FirstChildElement("ambient"), 50.0f, 50.0f, 50.0f);
                info.material.specular = parseColor255(color->FirstChildElement("specular"), 0.0f, 0.0f, 0.0f);
                info.material.emissive = parseColor255(color->FirstChildElement("emissive"), 0.0f, 0.0f, 0.0f);

                XMLElement *shininess = color->FirstChildElement("shininess");
                if (shininess) {
                    shininess->QueryFloatAttribute("value", &info.material.shininess);
                }
            }

            group.models.push_back(info);
        }
    }

    // 3) subgrupos (recursivo)
    for (XMLElement *child = groupElem->FirstChildElement("group"); child; child = child->NextSiblingElement("group"))
        group.children.push_back(parseGroup(child));

    return group;
}


// parse do XML principal (world -> window, camera, lights e group)
inline WorldConfig parseXML(const string &filename)
{
    WorldConfig config;
    XMLDocument doc;

    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS)
    {
        cerr << "Erro ao abrir ficheiro XML: " << filename << endl;
        return config;
    }

    XMLElement *world = doc.FirstChildElement("world");
    if (!world)
    {
        cerr << "elemento <world> nao encontrado!" << endl;
        return config;
    }

    // janela (width/height)
    XMLElement *window = world->FirstChildElement("window");
    if (window)
    {
        window->QueryIntAttribute("width", &config.window.width);
        window->QueryIntAttribute("height", &config.window.height);
    }

    // camera (position, lookAt, up, projection)
    XMLElement *camera = world->FirstChildElement("camera");
    if (camera)
    {
        XMLElement *pos = camera->FirstChildElement("position");
        if (pos)
        {
            pos->QueryFloatAttribute("x", &config.camera.posX);
            pos->QueryFloatAttribute("y", &config.camera.posY);
            pos->QueryFloatAttribute("z", &config.camera.posZ);
        }

        XMLElement *lookAt = camera->FirstChildElement("lookAt");
        if (lookAt)
        {
            lookAt->QueryFloatAttribute("x", &config.camera.lookAtX);
            lookAt->QueryFloatAttribute("y", &config.camera.lookAtY);
            lookAt->QueryFloatAttribute("z", &config.camera.lookAtZ);
        }

        XMLElement *up = camera->FirstChildElement("up");
        if (up)
        {
            up->QueryFloatAttribute("x", &config.camera.upX);
            up->QueryFloatAttribute("y", &config.camera.upY);
            up->QueryFloatAttribute("z", &config.camera.upZ);
        }

        XMLElement *proj = camera->FirstChildElement("projection");
        if (proj)
        {
            proj->QueryFloatAttribute("fov", &config.camera.fov);
            proj->QueryFloatAttribute("near", &config.camera.near);
            proj->QueryFloatAttribute("far", &config.camera.far);
        }
    }

    // lights (ate 8 no OpenGL)
    XMLElement *lights = world->FirstChildElement("lights");
    if (lights)
    {
        for (XMLElement *l = lights->FirstChildElement("light"); l; l = l->NextSiblingElement("light"))
        {
            const char *type = l->Attribute("type");
            if (!type) continue;

            string typeStr = type;
            Light light;

            if (typeStr == "point")
            {
                light.type = LightType::Point;
                queryFloatAttrAny(l, "posX", "posx", &light.position.x);
                queryFloatAttrAny(l, "posY", "posy", &light.position.y);
                queryFloatAttrAny(l, "posZ", "posz", &light.position.z);
            }
            else if (typeStr == "directional")
            {
                light.type = LightType::Directional;
                queryFloatAttrAny(l, "dirX", "dirx", &light.direction.x);
                queryFloatAttrAny(l, "dirY", "diry", &light.direction.y);
                queryFloatAttrAny(l, "dirZ", "dirz", &light.direction.z);
            }
            else if (typeStr == "spot" || typeStr == "spotlight")
            {
                light.type = LightType::Spot;
                queryFloatAttrAny(l, "posX", "posx", &light.position.x);
                queryFloatAttrAny(l, "posY", "posy", &light.position.y);
                queryFloatAttrAny(l, "posZ", "posz", &light.position.z);
                queryFloatAttrAny(l, "dirX", "dirx", &light.direction.x);
                queryFloatAttrAny(l, "dirY", "diry", &light.direction.y);
                queryFloatAttrAny(l, "dirZ", "dirz", &light.direction.z);
                l->QueryFloatAttribute("cutoff", &light.cutoff);
            }
            else
            {
                continue;
            }

            config.lights.push_back(light);
        }
    }

    // group -> models
    XMLElement *group = world->FirstChildElement("group");
    if (group) {
        config.rootGroup = parseGroup(group);
    }

    return config;
}



#endif // XMLPARSER_H
