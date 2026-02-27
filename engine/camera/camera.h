#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include <iostream>

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

    void processKey(unsigned char key)
    {
        switch (key)
        {
        case 'c':
        case 'C':
            orbital = !orbital;
            cout << (orbital ? "Camera orbital ON" : "Camera fixa (XML)") << endl;
            break;
        case 'w':
        case 'W':
            beta += 0.1f;
            if (beta > 1.5f)
                beta = 1.5f;
            orbital = true;
            break;
        case 's':
        case 'S':
            beta -= 0.1f;
            if (beta < -1.5f)
                beta = -1.5f;
            orbital = true;
            break;
        case 'a':
        case 'A':
            alpha -= 0.1f;
            orbital = true;
            break;
        case 'd':
        case 'D':
            alpha += 0.1f;
            orbital = true;
            break;
        case 'q':
        case 'Q':
            radius -= 0.5f;
            if (radius < 1.0f)
                radius = 1.0f;
            orbital = true;
            break;
        case 'e':
        case 'E':
            radius += 0.5f;
            orbital = true;
            break;
        }
    }
};

#endif
