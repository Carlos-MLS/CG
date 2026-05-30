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


#include <IL/il.h>     // Core DevIL functions
#include <IL/ilu.h>    // Image manipulation
#include <IL/ilut.h>   // Utility toolkit (OpenGL, etc.)


using namespace std;

// vars globais
WorldConfig config;
Camera camera;

GLenum modoDesenho = GL_FILL; // comeca em solido

// buffers do modelo no GPU (vertices/normais/texcoords)
struct ModelGPU {
    GLuint vboVertices = 0;
    GLuint vboNormals = 0;
    GLuint vboTexCoords = 0;
    int vertexCount = 0; // numero total de vertices
    bool hasNormals = false; // tem normais por vertice
    bool hasTexCoords = false; // tem texcoords por vertice
};

// cache por caminho do ficheiro (evita uploads repetidos)
unordered_map<std::string, ModelGPU> gpuCache;

// faz upload do modelo para o GPU apenas uma vez
void uploadModelIfNeeded(const string& file) {
    if (gpuCache.find(file) != gpuCache.end()) return; //modelo já está no GPU, não precisa de upload

    Model m = loadModel(file);
    if (m.vertices.empty()) return; //fail safe, não tentar fazer upload de um modelo vazio

    ModelGPU g;
    g.vertexCount = (int)m.vertices.size();

    glGenBuffers(1, &g.vboVertices); //gerar um VBO no GPU e guardar o ID em g.vboVertices
    glBindBuffer(GL_ARRAY_BUFFER, g.vboVertices); //fazer bind do VBO para poder usar
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Point3D), m.vertices.data(), GL_STATIC_DRAW); //fazer upload dos vértices para o VBO

    if (m.normals.size() == m.vertices.size()) // normais alinhadas com vertices
    {
        g.hasNormals = true;
        glGenBuffers(1, &g.vboNormals);
        glBindBuffer(GL_ARRAY_BUFFER, g.vboNormals);
        glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(Point3D), m.normals.data(), GL_STATIC_DRAW);
    }

    if (m.texCoords.size() == m.vertices.size()) // texcoords alinhadas com vertices
    {
        g.hasTexCoords = true;
        glGenBuffers(1, &g.vboTexCoords);
        glBindBuffer(GL_ARRAY_BUFFER, g.vboTexCoords);
        glBufferData(GL_ARRAY_BUFFER, m.texCoords.size() * sizeof(Point2D), m.texCoords.data(), GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    gpuCache[file] = g; //guardar o ModelGPU no cache para não precisar de fazer upload novamente
}

//func para desenhar um modelo a partir do seu ficheiro, usando o cache de VBOs para evitar uploads desnecessários
//está diferente a partir da fase 3 (por causa da parte da animação, mas a ideia geral é a mesma: verificar se o modelo já está no GPU, se não estiver fazer upload, e depois desenhar usando o VBO)


// desenha o modelo usando os VBOs associados
void drawModelByFile(const std::string& file) {
    uploadModelIfNeeded(file);

    auto it = gpuCache.find(file);
    if (it == gpuCache.end()) return;

    const ModelGPU& g = it->second;

    glBindBuffer(GL_ARRAY_BUFFER, g.vboVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Point3D), 0);

    if (g.hasNormals)
    {
        glBindBuffer(GL_ARRAY_BUFFER, g.vboNormals);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, sizeof(Point3D), 0);
    }

    if (g.hasTexCoords)
    {
        glBindBuffer(GL_ARRAY_BUFFER, g.vboTexCoords);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Point2D), 0);
    }

    glDrawArrays(GL_TRIANGLES, 0, g.vertexCount);
    glDisableClientState(GL_VERTEX_ARRAY);

    if (g.hasNormals)
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (g.hasTexCoords)
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//func para dar preload de todos os modelos usados no grupo e nos subgrupos, para garantir que estão no GPU antes de começar a renderizar
void preloadGroupModels(const Group& group) {
    for (const auto& m : group.models) uploadModelIfNeeded(m.file);
    for (const auto& child : group.children) preloadGroupModels(child);
}

// aplicar material do modelo no pipeline fixo
void applyMaterial(const Material& mat)
{
    GLfloat diffuse[4] = { mat.diffuse.r, mat.diffuse.g, mat.diffuse.b, 1.0f };
    GLfloat ambient[4] = { mat.ambient.r, mat.ambient.g, mat.ambient.b, 1.0f };
    GLfloat specular[4] = { mat.specular.r, mat.specular.g, mat.specular.b, 1.0f };
    GLfloat emissive[4] = { mat.emissive.r, mat.emissive.g, mat.emissive.b, 1.0f };

    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);
}

// configurar luzes do XML no OpenGL (max 8)
void applyLights()
{
    if (config.lights.empty())
    {
        glDisable(GL_LIGHTING);
        return;
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE); // corrige normais quando ha escalas

    int count = (int)config.lights.size();
    if (count > 8) count = 8;

    GLfloat lightAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; //ambiente base por luz

    for (int i = 0; i < count; i++)
    {
        const Light& light = config.lights[i];
        GLenum lightId = GL_LIGHT0 + i;

        GLfloat white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glEnable(lightId);
        glLightfv(lightId, GL_AMBIENT, lightAmbient);
        glLightfv(lightId, GL_DIFFUSE, white);
        glLightfv(lightId, GL_SPECULAR, white);

        if (light.type == LightType::Directional)
        {
            // nos testes, a direcao ja aponta da luz para a cena
            GLfloat pos[4] = { light.direction.x, light.direction.y, light.direction.z, 0.0f };
            glLightfv(lightId, GL_POSITION, pos);
            glLightf(lightId, GL_SPOT_CUTOFF, 180.0f);
        }
        else
        {
            GLfloat pos[4] = { light.position.x, light.position.y, light.position.z, 1.0f };
            glLightfv(lightId, GL_POSITION, pos);

            if (light.type == LightType::Spot)
            {
                // nos testes, a direcao ja aponta da luz para a cena
                GLfloat dir[3] = { light.direction.x, light.direction.y, light.direction.z };
                glLightfv(lightId, GL_SPOT_DIRECTION, dir);
                glLightf(lightId, GL_SPOT_CUTOFF, light.cutoff);
            }
            else
            {
                glLightf(lightId, GL_SPOT_CUTOFF, 180.0f);
            }
        }
    }

    for (int i = count; i < 8; i++)
    {
        glDisable(GL_LIGHT0 + i);
    }
}

//Para a fase 4 do DevIL()
//n esquecer que no glBindTexture(GL_TEXTURE_2D, 0) 
//é para desfazer o bind da textura, e glDisable(GL_TEXTURE_2D) 

unordered_map<string, GLuint> texturaCache; //usado em baixo
 //cache de texturas para evitar uploads repetidos

 //incializar o DevIL
void initDevIL() {
    ilInit(); //inicializar o DevIL
    iluInit(); //inicializar o utilitario do DevIL
    ilEnable(IL_ORIGIN_SET); //definir a origem das imagens para o canto inferior esquerdo, 
    //para ser consistente com as coordenadas de textura do OpenGL
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
}

//dar load da textura do file, devil -> OpenGL
GLuint loadTexturaFile(const string& path) {

    ILuint imageId = 0; //ID da imagem no DevIL
    ilGenImages(1, &imageId); //gerar uma imagem no DevIL
    ilBindImage(imageId); //fazer bind da imagem para usar 

    if (!ilLoadImage(path.c_str())) { //tentar carregar a imagem do file
        ilDeleteImages(1, &imageId); //dar delete para evitar leaks
        return 0; //retornar 0 para indicar falha
    }

    //se chegar aqui agr convertemos para RGBA para garantir que tem um formato consistente, 
    //e depois fazemos upload para o OpenGL
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    int largura = ilGetInteger(IL_IMAGE_WIDTH);
    int altura = ilGetInteger(IL_IMAGE_HEIGHT);
    unsigned char* data = ilGetData();

    GLuint texturaId = 0; //id da textura no OpenGL
    glGenTextures(1, &texturaId); //gerar uma textura no OpenGL
    glBindTexture(GL_TEXTURE_2D, texturaId); //fazer bind da textura para usar

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //definir o wrapping para repetir
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //same para T
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //definir o filtro de minificação para linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //definir o filtro de magnificação para linear

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //fazer upload dos dados da imagem para a textura

    glBindTexture(GL_TEXTURE_2D, 0); //desfazer bind da textura
    ilDeleteImages(1, &imageId); //dar delete da imagem no DevIL

    return texturaId; //retornar o ID da textura no OpenGL

}

//para cachear a textura por caminho 
GLuint getTextura(const string& path) {
    auto iter = texturaCache.find(path);

    if (iter != texturaCache.end()) return iter->second; //se a textura já está no cache, retornar o ID

    GLuint texturaId = loadTexturaFile(path); //caso contrário, carregar a textura do file
    if (texturaId != 0) {
        texturaCache[path] = texturaId; //guardar no cache para evitar uploads repetidos
    }
    return texturaId;
}


//dar/tirar textura do render  
void applyTextura(const ModelInfo& model) {
    
    //na fase 4 os XMLs podem ter um atributo de textura opcional para cada modelo, 
    //e se tiverem, devemos ativar a textura e fazer bind da textura correta antes de desenhar o modelo, e depois desativar a textura para não interferir com outros modelos que possam não ter textura
    if (!model.hasTexture || model.textureFile.empty()) {
        glDisable(GL_TEXTURE_2D); //sem textura definida no XML
        return;
    }

    uploadModelIfNeeded(model.file); //garantir que o modelo está no GPU para poder usar as suas texcoords

    auto iter = gpuCache.find(model.file);
    //verificar se o modelo tem texcoords, para decidir se ativamos ou não a textura
    bool hasTexCoords = (iter != gpuCache.end()) && iter->second.hasTexCoords;
    if (!hasTexCoords) {
        glDisable(GL_TEXTURE_2D); //desativar textura se o modelo não tem texcoords
        return;
    }

    GLuint texturaId = getTextura(model.textureFile); //obter o ID da textura a partir do caminho do file
    if (texturaId == 0) {
        glDisable(GL_TEXTURE_2D); //desativar textura se falhou ao carregar
        return;
    }

    //chega aqui ent temos uma textura válida, então ativamos a textura e fazemos bind para usar
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaId); //fazer bind da textura para usar
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //definir o modo de aplicação da textura para modulate, para que a textura seja combinada com a cor do material

}

//só é chamada dentro do applyTextura, para garantir que só desativa a textura se o modelo não tiver texcoords ou se falhar ao carregar a textura, e não interfere com outros modelos que possam usar textura
void unbindTexturaIfNeeded() {
    glBindTexture(GL_TEXTURE_2D, 0); //desfazer bind da textura
    glDisable(GL_TEXTURE_2D); //desativar textura
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
    for (const auto &m : group.models) {
        applyMaterial(m.material);
        applyTextura(m); //ativar textura (se existir)
        drawModelByFile(m.file);
        unbindTexturaIfNeeded(); //evitar contaminar o proximo modelo
    }
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

    applyLights();

    glPolygonMode(GL_FRONT_AND_BACK, modoDesenho);

    // desenhar eixos ANTES dos modelos (com depth test ativo)
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
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
    glPopAttrib();

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
        cerr << "Controlos: O/F/C (camera orbital/fixa/toggle), WASD/QE (controlar orbital), 1/2/3 (solid/wire/points), ESC (sair)" << endl;
        return 1;
    }

    // ler config do XML
    config = parseXML(argv[1]);

    cout << "Janela: " << config.window.width << "x" << config.window.height << endl;
    cout << "Camera pos: (" << config.camera.posX << ", " << config.camera.posY
         << ", " << config.camera.posZ << ")" << endl;


    // inicializar a camera com base na config do XML
    camera.initFromConfig(config.camera);
    camera.printMode("inicial");

    // setup GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //depth buffer, double buffering, RGBA colors
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(config.window.width, config.window.height);
    glutCreateWindow("CG Engine - Fase 4");

    initDevIL(); //inicializar DevIL depois de criar o contexto OpenGL

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processKeys);

    // opengl stuff
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_RESCALE_NORMAL); // normaliza normais apos escalas uniformes
    GLfloat amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // luz ambiente global
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    preloadGroupModels(config.rootGroup); //fazer preload de todos os modelos para garantir que estão no GPU antes de começar a renderizar

    cout << "Controlos:" << endl;
    cout << "  O - camera orbital" << endl;
    cout << "  F - camera fixa definida no XML" << endl;
    cout << "  C - alternar entre orbital/fixa" << endl;
    cout << "  WASD - rodar camera orbital (ativa orbital automaticamente)" << endl;
    cout << "  Q/E - aproximar/afastar camera orbital" << endl;
    cout << "  1/2/3 - modo solido/wireframe/pontos" << endl;
    cout << "  ESC - sair" << endl;


    glutIdleFunc(idle); //para garantir que a cena é redesenhada continuamente, mesmo sem input do user

    glutMainLoop();

    return 0;
}
