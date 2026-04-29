#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include <vector>
#include <tinyxml2.h>
#include <iostream>
#include "../scene/group.h"

using namespace std;
using namespace tinyxml2;

struct WindowConfig
{
    int width = 512;
    int height = 512;
};

struct CameraConfig
{
    float posX = 3, posY = 2, posZ = 1;
    float lookAtX = 0, lookAtY = 0, lookAtZ = 0;
    float upX = 0, upY = 1, upZ = 0;
    float fov = 60, near = 1, far = 1000;
};

struct WorldConfig
{
    WindowConfig window;
    CameraConfig camera;
    Group rootGroup;
};

inline Group parseGroup(XMLElement *groupElem) {
    Group group;

    XMLElement *transform = groupElem->FirstChildElement("transform");
    if (transform) {
        for (XMLElement *t = transform->FirstChildElement(); t; t = t->NextSiblingElement()) {
            string name = t->Value();
            Transform tr;
            if (name == "translate") {
                tr.type = TransformType::Translate;

                if (t->QueryFloatAttribute("time", &tr.time) == XML_SUCCESS && tr.time > 0.0f) {
                    tr.hasTime = true;

                    const char* align = t->Attribute("align");
                    if (!align) align = t->Attribute("alignToPath");
                    if (align) {
                        string alignValue = align;
                        tr.alignToPath = (alignValue == "true" || alignValue == "True" || alignValue == "TRUE" || alignValue == "1");
                    }

                    for (XMLElement* cp = t->FirstChildElement("point"); cp; cp = cp->NextSiblingElement("point")) {
                        Point3D point;
                        cp->QueryFloatAttribute("x", &point.x);
                        cp->QueryFloatAttribute("y", &point.y);
                        cp->QueryFloatAttribute("z", &point.z);
                        tr.controlPoints.push_back(point);
                    }

                    // compatibilidade com XMLs antigos
                    if (tr.controlPoints.empty()) {
                        for (XMLElement* cp = t->FirstChildElement("controlPoint"); cp; cp = cp->NextSiblingElement("controlPoint")) {
                            Point3D point;
                            cp->QueryFloatAttribute("x", &point.x);
                            cp->QueryFloatAttribute("y", &point.y);
                            cp->QueryFloatAttribute("z", &point.z);
                            tr.controlPoints.push_back(point);
                        }
                    }

                    if (tr.controlPoints.size() < 4) {
                        tr.hasTime = false;
                        tr.time = 0.0f;
                        t->QueryFloatAttribute("x", &tr.x);
                        t->QueryFloatAttribute("y", &tr.y);
                        t->QueryFloatAttribute("z", &tr.z);
                    }
                }
                else {
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

                if (t->QueryFloatAttribute("time", &tr.time) == XML_SUCCESS && tr.time > 0.0f) {
                    tr.hasTime = true;
                }
                else {
                    t->QueryFloatAttribute("angle", &tr.angle);
                }

                group.transforms.push_back(tr);
            } else if (name == "scale") {
                tr.type = TransformType::Scale;
                t->QueryFloatAttribute("x", &tr.x);
                t->QueryFloatAttribute("y", &tr.y);
                t->QueryFloatAttribute("z", &tr.z);
                group.transforms.push_back(tr);
            }
        }
    }

    XMLElement *models = groupElem->FirstChildElement("models");
    if (models) {
        for (XMLElement *m = models->FirstChildElement("model"); m; m = m->NextSiblingElement("model")) {
            const char *file = m->Attribute("file");
            if (file) group.modelFiles.push_back(file);
        }
    }

    for (XMLElement *child = groupElem->FirstChildElement("group"); child; child = child->NextSiblingElement("group"))
        group.children.push_back(parseGroup(child));

    return group;
}


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

    // janela
    XMLElement *window = world->FirstChildElement("window");
    if (window)
    {
        window->QueryIntAttribute("width", &config.window.width);
        window->QueryIntAttribute("height", &config.window.height);
    }

    // camera
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

    // group -> models
    XMLElement *group = world->FirstChildElement("group");
    if (group) {
        config.rootGroup = parseGroup(group);
    }

    return config;
}



#endif // XMLPARSER_H
