#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <string>
#include <vector>
#include <tinyxml2.h>
#include <iostream>

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
    vector<string> modelFiles;
};

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
    if (group)
    {
        XMLElement *models = group->FirstChildElement("models");
        if (models)
        {
            for (XMLElement *model = models->FirstChildElement("model");
                 model != nullptr;
                 model = model->NextSiblingElement("model"))
            {

                const char *file = model->Attribute("file");
                if (file)
                {
                    config.modelFiles.push_back(string(file));
                }
            }
        }
    }

    return config;
}

#endif // XMLPARSER_H
