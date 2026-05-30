#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <iostream>
#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "../parser/xmlparser.h"

using namespace std;

struct Camera
{
    float alpha = 0.0f;
    float beta = 0.0f;
    float radius = 5.0f;
    bool orbital = false;

    CameraConfig cfg;

    void initFromConfig(const CameraConfig &camCfg)
    {
        cfg = camCfg;

        float dist = sqrt(cfg.posX * cfg.posX +
                          cfg.posY * cfg.posY +
                          cfg.posZ * cfg.posZ);
        alpha = atan2(cfg.posX, cfg.posZ);
        beta = asin(cfg.posY / dist);
        radius = dist;
    }

    void apply() const
    {
        if (orbital)
        {
            float cx = radius * cos(beta) * sin(alpha);
            float cy = radius * sin(beta);
            float cz = radius * cos(beta) * cos(alpha);
            gluLookAt(cx, cy, cz,
                      cfg.lookAtX, cfg.lookAtY, cfg.lookAtZ,
                      cfg.upX, cfg.upY, cfg.upZ);
        }
        else
        {
            gluLookAt(cfg.posX, cfg.posY, cfg.posZ,
                      cfg.lookAtX, cfg.lookAtY, cfg.lookAtZ,
                      cfg.upX, cfg.upY, cfg.upZ);
        }
    }

    void resize(int w, int h) const
    {
        if (h == 0)
            h = 1;
        float ratio = (float)w / (float)h;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, w, h);
        gluPerspective(cfg.fov, ratio, cfg.near, cfg.far);
        glMatrixMode(GL_MODELVIEW);
    }

    void printMode(const string &reason = "") const
    {
        cout << "Camera: " << (orbital ? "orbital" : "fixa XML");
        if (!reason.empty())
            cout << " (" << reason << ")";
        cout << endl;
    }

    void printOrbitalState(const string &action) const
    {
        cout << "Camera orbital [" << action << "]: alpha=" << alpha
             << " beta=" << beta << " radius=" << radius << endl;
    }

    void setOrbitalMode(bool value, const string &reason)
    {
        bool changed = (orbital != value);
        orbital = value;

        if (changed)
            printMode(reason);
        else
            printMode(reason + ", sem alteracao");
    }

    void ensureOrbitalForControl(const string &action)
    {
        if (!orbital)
        {
            orbital = true;
            printMode("ativada por " + action);
        }
    }

    void processKey(unsigned char key)
    {
        switch (key)
        {
        case 'c':
        case 'C':
            orbital = !orbital;
            printMode("toggle C");
            break;
        case 'o':
        case 'O':
            setOrbitalMode(true, "tecla O");
            break;
        case 'f':
        case 'F':
            setOrbitalMode(false, "tecla F");
            break;
        case 'w':
        case 'W':
            ensureOrbitalForControl("W");
            beta += 0.1f;
            if (beta > 1.5f)
                beta = 1.5f;
            printOrbitalState("subir");
            break;
        case 's':
        case 'S':
            ensureOrbitalForControl("S");
            beta -= 0.1f;
            if (beta < -1.5f)
                beta = -1.5f;
            printOrbitalState("descer");
            break;
        case 'a':
        case 'A':
            ensureOrbitalForControl("A");
            alpha -= 0.1f;
            printOrbitalState("rodar esquerda");
            break;
        case 'd':
        case 'D':
            ensureOrbitalForControl("D");
            alpha += 0.1f;
            printOrbitalState("rodar direita");
            break;
        case 'q':
        case 'Q':
            ensureOrbitalForControl("Q");
            radius -= 0.5f;
            if (radius < 1.0f)
                radius = 1.0f;
            printOrbitalState("aproximar");
            break;
        case 'e':
        case 'E':
            ensureOrbitalForControl("E");
            radius += 0.5f;
            printOrbitalState("afastar");
            break;
        }
    }
};

#endif
