#include <iostream>
#include <vector>
#include <cmath>

#ifndef __APPLE__
#define GL_GLEXT_PROTOTYPES
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <unordered_map>
#include "parser/xmlparser.h"
#include "model/model.h"
#include "camera/camera.h"
#include "scene/group.h"


using namespace std;

// vars globais
WorldConfig config;
Camera camera;

GLenum modoDesenho = GL_LINE; // comeca em wireframe

struct ModelGPU {
    GLuint vbo = 0;
    int vertexCount = 0;
};

unordered_map<std::string, ModelGPU> gpuCache;

void uploadModelIfNeeded(const string& file) {
    if (gpuCache.find(file) != gpuCache.end()) return; //modelo já está no GPU, não precisa de upload

    Model m = loadModel(file);
    if (m.vertices.empty()) return; //fail safe, não tentar fazer upload de um modelo vazio

    ModelGPU g;
    g.vertexCount = (int)m.vertices.size();

    glGenBuffers(1, &g.vbo); //gerar um VBO no GPU e guardar o ID em g.vbo
    glBindBuffer(GL_ARRAY_BUFFER, g.vbo); //fazer bind do VBO para poder usar
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Point3D), m.vertices.data(), GL_STATIC_DRAW); //fazer upload dos vértices para o VBO

    gpuCache[file] = g; //guardar o ModelGPU no cache para não precisar de fazer upload novamente
}

//func para desenhar um modelo a partir do seu ficheiro, usando o cache de VBOs para evitar uploads desnecessários
//está diferente a partir da fase 3 (por causa da parte da animação, mas a ideia geral é a mesma: verificar se o modelo já está no GPU, se não estiver fazer upload, e depois desenhar usando o VBO)


void drawModelByFile(const std::string& file) {
    uploadModelIfNeeded(file);

    auto it = gpuCache.find(file);
    if (it == gpuCache.end()) return;

    const ModelGPU& g = it->second;

    glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Point3D), 0);
    glDrawArrays(GL_TRIANGLES, 0, g.vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//func para dar preload de todos os modelos usados no grupo e nos subgrupos, para garantir que estão no GPU antes de começar a renderizar
void preloadGroupModels(const Group& group) {
    for (const auto& f : group.modelFiles) uploadModelIfNeeded(f);
    for (const auto& child : group.children) preloadGroupModels(child);
}

// funcs para renderizar o grupo, desenhar os modelos, aplicar as transformações, etc
inline void catmull1D (float p0, float p1, float p2, float p3, float t, float &pos, float &deriv) {
    float a0 = -0.5f * p0 + 1.5f * p1 - 1.5f * p2 + 0.5f * p3;
    float a1 = p0 - 2.5f * p1 + 2.0f * p2 - 0.5f * p3;
    float a2 = -0.5f * p0 + 0.5f * p2;
    float a3 = p1;

    pos = ((a0 * t + a1) * t + a2) * t + a3;
    deriv = (3.0f * a0 * t + 2.0f * a1) * t + a2;
}

//Para a fase 3, calcular o ponto e a derivada da curva de Catmull-Rom para um dado t (entre 0 e 1) e um conjunto de pontos de controlo 
inline void getGlobalCatmullRomPoint(const vector<Point3D>& pts, float totalTime, float elapsedSec, Point3D& position, Point3D& deriv) {

    int n = (int)pts.size();
    //gt vai de 0 a 1 ao longo do tempo total da animação, e é usado para determinar em que segmento da curva estamos e qual o t local dentro desse segmento
    float gt = fmod(elapsedSec, totalTime) / totalTime; //gt vai de 0 a 1 ao longo do tempo total da animação
    float t = gt * n; //t é o gt escalado pelo número de pontos, para determinar em que segmento estamos
    int index = (int)floor(t); //índice do segmento atual, que é o inteiro de t
    float lt = t - index; //t local dentro do segmento, que é a parte fracionária de t

    int i0 = (index - 1 + n) % n; //indice do ponto de controlo anterior, usando módulo para wrap-around
    int i1 = index % n; //indice do ponto de controlo atual
    int i2 = (index + 1) % n; //indice do próximo
    int i3 = (index + 2) % n; //indice do proximo-proximo


    //agora temos os 4 pontos de controlo para o segmento atual, e o t local dentro desse segmento, podemos usar a função catmull1D para calcular a posição e a derivada em x,y,z
    catmull1D(pts[i0].x, pts[i1].x, pts[i2].x, pts[i3].x, lt, position.x, deriv.x);
    catmull1D(pts[i0].y, pts[i1].y,pts[i2].y,pts[i3].y, lt, position.y, deriv.y);
    catmull1D(pts[i0].z, pts[i1].z,pts[i2].z,pts[i3].z, lt, position.z, deriv.z);
}

//Serve para calcular a matriz de rotação para alinhar um objeto com a direção da curva, usando a derivada da curva para obter a direção, e uma direção "up" fixa (0,1,0) para calcular os vetores ortogonais da matriz de rotação
inline float len(const Point3D& v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

//func para normalizar um vetor, ou seja, dividir cada componente pelo comprimento do vetor, para obter um vetor unitário na mesma direção
inline Point3D normalize (const Point3D& v) {
    float l = len(v);
    if (l == 0) return Point3D(0, 0, 0);
    return Point3D(v.x / l, v.y / l, v.z / l);
}

//func para calcular o produto vetorial entre dois vetores, que é um vetor perpendicular aos dois, e é usado para calcular os vetores ortogonais da matriz de rotação
inline Point3D cross(const Point3D& a, const Point3D& b) {
    return Point3D(a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x);
}
//para aplicar transformações com o header transform que criaámos
void applyTransform (const Transform &transform){
    switch (transform.type)
    {
    case TransformType::Translate: {
        if(!transform.hasTime || transform.time <= 0.0f || transform.controlPoints.size() < 4) { //fases anteriores à 3
            glTranslatef(transform.x, transform.y, transform.z);
        }
        else {
            //para a fase 3, se a transformação tiver tempo, é uma translate animada ao longo de uma curva de Catmull-Rom definida pelos pontos de controlo
            //então temos de calcular a posição e a derivada da curva para o tempo atual da animação, e usar isso para fazer a tradução e a rotação (se alignToPath for true)
            Point3D position, deriv;
            getGlobalCatmullRomPoint(transform.controlPoints, transform.time, glutGet(GLUT_ELAPSED_TIME) / 1000.0f, position, deriv);

            // Desenhar a curva no espaço global ANTES de aplicar a tradução
            if (transform.hasTime && transform.controlPoints.size() >= 4) {
                glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
                glDisable(GL_LIGHTING);
                glColor3f(1.0f, 1.0f, 1.0f); // linha branca (mesma cor do teapot)
                glLineWidth(2.0f);
                glBegin(GL_LINE_LOOP);
                for (float tt = 0.0f; tt < 1.0f; tt += 0.01f) {
                    Point3D ptmp, dtmp;
                    getGlobalCatmullRomPoint(transform.controlPoints, transform.time, tt * transform.time, ptmp, dtmp);
                    glVertex3f(ptmp.x, ptmp.y, ptmp.z);
                }
                glEnd();
                glPopAttrib();
            }

            // Aplicar a tradução do objeto para a posição na curva
            glTranslatef(position.x, position.y, position.z);

            if(transform.alignToPath) {
                //calcular a matriz de rotação para alinhar o objeto com a direção da curva (derivada), usando uma direção "up" fixa (0,1,0)
                Point3D X = normalize(deriv);
                Point3D up(0.0f, 1.0f, 0.0f);
                Point3D Z = normalize(cross(X, up));
                if (len(Z) == 0) { //caso a direção da curva seja paralela ao vetor up, escolhemos um vetor Z arbitrário perpendicular a X
                    Z = normalize(cross(X, Point3D(1.0f, 0.0f, 0.0f)));
                }
                Point3D Y = normalize(cross(Z, X));

                float matriz[16] = {
                    X.x, X.y, X.z, 0,
                    Y.x, Y.y, Y.z, 0,
                    Z.x, Z.y, Z.z, 0,
                    0, 0, 0, 1
                };
                      glMultMatrixf(matriz);
            }
        }
          break;
    }
    
    case TransformType::Rotate: {
        float a = transform.angle;
        //para a fase 3, se a transformação tiver tempo,
        // é uma rotação animada, então o ângulo é calculado com base no tempo atual da animação e no tempo total da rotação
        if(transform.hasTime && transform.time > 0.0f) { 
            float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; //tempo em segundos
            float gt = fmod(elapsed, transform.time) / transform.time; //gt vai de 0 a 1 ao longo do tempo da rotação   
            a = gt * 360.0f; //ângulo em graus, baseado no gt da animação 
        }
        glRotatef(a, transform.x, transform.y, transform.z);

          break;
    }
    case TransformType::Scale: glScalef(transform.x,transform.y,transform.z); break;
    default:
        break;
    }

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

//oara ser chamada em loop 
void idle() {
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
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //depth buffer, double buffering, RGBA colors
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

    preloadGroupModels(config.rootGroup); //fazer preload de todos os modelos para garantir que estão no GPU antes de começar a renderizar

    cout << "Controlos: WASD (camera), QE (zoom), C (toggle camera), 1/2/3 (render mode), ESC" << endl;


    glutIdleFunc(idle); //para garantir que a cena é redesenhada continuamente, mesmo sem input do user

    glutMainLoop();

    return 0;
}
