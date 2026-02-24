#include <iostream>
#include <vector>
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "parser/xmlparser.h"
#include "model/model.h"

using namespace std;

// vars globais
WorldConfig config;
vector<Model> models;

// camera orbital
float camAlpha = 0.0f;
float camBeta = 0.0f;
float camRadius = 5.0f;
bool orbitalCam = false;

GLenum modoDesenho = GL_LINE; // comeca em wireframe

void changeSize(int w, int h)
{
    if (h == 0)
        h = 1;
    float ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(config.camera.fov, ratio, config.camera.near, config.camera.far);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // camera
    if (orbitalCam)
    {
        float cx = camRadius * cos(camBeta) * sin(camAlpha);
        float cy = camRadius * sin(camBeta);
        float cz = camRadius * cos(camBeta) * cos(camAlpha);
        gluLookAt(cx, cy, cz,
                  config.camera.lookAtX, config.camera.lookAtY, config.camera.lookAtZ,
                  config.camera.upX, config.camera.upY, config.camera.upZ);
    }
    else
    {
        gluLookAt(config.camera.posX, config.camera.posY, config.camera.posZ,
                  config.camera.lookAtX, config.camera.lookAtY, config.camera.lookAtZ,
                  config.camera.upX, config.camera.upY, config.camera.upZ);
    }

    glPolygonMode(GL_FRONT_AND_BACK, modoDesenho);

    // desenhar os modelos todos
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto &model : models)
    {
        glBegin(GL_TRIANGLES);
        for (const auto &v : model.vertices)
        {
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }

    // desenhar eixos por cima de tudo (sem depth test)
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    // X vermelho
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    // Y verde
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    // Z azul
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
    glEnable(GL_DEPTH_TEST);

    glutSwapBuffers();
}

void processKeys(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESC
        exit(0);
    case '1':
        modoDesenho = GL_FILL;
        break;
    case '2':
        modoDesenho = GL_LINE;
        break;
    case '3':
        modoDesenho = GL_POINT;
        break;
    case 'c':
    case 'C':
        orbitalCam = !orbitalCam;
        cout << (orbitalCam ? "Camera orbital ON" : "Camera fixa (XML)") << endl;
        break;
    case 'w':
    case 'W':
        camBeta += 0.1f;
        if (camBeta > 1.5f)
            camBeta = 1.5f;
        orbitalCam = true;
        break;
    case 's':
    case 'S':
        camBeta -= 0.1f;
        if (camBeta < -1.5f)
            camBeta = -1.5f;
        orbitalCam = true;
        break;
    case 'a':
    case 'A':
        camAlpha -= 0.1f;
        orbitalCam = true;
        break;
    case 'd':
    case 'D':
        camAlpha += 0.1f;
        orbitalCam = true;
        break;
    case 'q':
    case 'Q':
        camRadius -= 0.5f;
        if (camRadius < 1.0f)
            camRadius = 1.0f;
        orbitalCam = true;
        break;
    case 'e':
    case 'E':
        camRadius += 0.5f;
        orbitalCam = true;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Uso: ./engine <config.xml>" << endl;
        cerr << "Controlos: WASD (camera), QE (zoom), 1/2/3 (solid/wire/points), ESC (sair)" << endl;
        return 1;
    }

    // ler config do XML
    config = parseXML(argv[1]);

    cout << "Janela: " << config.window.width << "x" << config.window.height << endl;
    cout << "Camera pos: (" << config.camera.posX << ", " << config.camera.posY
         << ", " << config.camera.posZ << ")" << endl;

    // carregar os modelos .3d
    for (const auto &f : config.modelFiles)
    {
        Model m = loadModel(f);
        if (!m.vertices.empty())
        {
            models.push_back(m);
        }
    }

    // inicializar a camera orbital com base na posicao do XML
    float dist = sqrt(config.camera.posX * config.camera.posX +
                      config.camera.posY * config.camera.posY +
                      config.camera.posZ * config.camera.posZ);
    camAlpha = atan2(config.camera.posX, config.camera.posZ);
    camBeta = asin(config.camera.posY / dist);
    camRadius = dist;

    // setup GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(config.window.width, config.window.height);
    glutCreateWindow("CG Engine - Fase 1");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processKeys);

    // opengl stuff
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    cout << "Controlos: WASD (camera), QE (zoom), C (toggle camera), 1/2/3 (render mode), ESC" << endl;

    glutMainLoop();

    return 0;
}
