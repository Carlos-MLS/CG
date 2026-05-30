#ifndef POINT_H
#define POINT_H

//isto usa-se em quase tudo, por isso é importante ter uma struct simples para guardar os pontos 3D, e evitar ter que incluir coisas desnecessárias (como a parte de modelos ou de transformações) só para ter um ponto
struct Point3D { // struct para guardar um ponto
    float x, y, z;
    //same as point2D
    Point3D(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};


//para a fase 4 adicionar tbm um Point2D para coordenadas de textura
struct Point2D {
    float s, t;
    //construtor com valores default para facilitar a criação de pontos sem ter que especificar os valores
    Point2D(float s = 0.0f, float t = 0.0f) : s(s), t(t) {}
};

#endif
