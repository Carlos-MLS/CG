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
#include "camera/camera.h"
#include "scene/group.h"


using namespace std;

// vars globais
WorldConfig config;
Camera camera;

GLenum modoDesenho = GL_LINE; // comeca em wireframe


//para aplicar transformações com o header transform que criaámos
void applyTransform (const Transform &transform){
    switch (transform.type)
    {
    case TransformType::Translate: glTranslatef(transform.x,transform.y,transform.z); break;
    case TransformType::Rotate: glRotatef(transform.angle,transform.x,transform.y,transform.z); break;
    case TransformType::Scale: glScalef(transform.x,transform.y,transform.z); break;
    default:
        break;
    }

}

//agora desenhar o model
void drawModelByFile (const string &file) {
    Model model = loadModel(file);
    if (model.vertices.empty()) return; //fail safe

    glBegin(GL_TRIANGLES);
    for (const auto &v : model.vertices) glVertex3f(v.x, v.y, v.z);
    glEnd();

}

//Dar render ao group agora com as funcs que definimos antes 
void renderGroup(const Group &group) {
    glPushMatrix();
    for (const auto &t : group.transforms) applyTransform(t);
    for (const auto &f : group.modelFiles) drawModelByFile(f);
    for (const auto &child : group.children) renderGroup(child);

    glPopMatrix();
}

void changeSize(int w, int h)
{
    camera.resize(w, h);
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // camera
    camera.apply();

    glPolygonMode(GL_FRONT_AND_BACK, modoDesenho);

    // desenhar eixos ANTES dos modelos (com depth test ativo)
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

    // desenhar os modelos todos
    glColor3f(1.0f, 1.0f, 1.0f);
    renderGroup(config.rootGroup);
    
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
    default:
        camera.processKey(key);
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


    // inicializar a camera com base na config do XML
    camera.initFromConfig(config.camera);

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
