#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vector>

#include "../../utils/point.h"

enum class TransformType {
    Translate, Rotate, Scale
};

struct Transform {
    TransformType type;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float angle = 0.0f;

    //para a fase 3 agora
    bool hasTime = false;
    float time = 0.0f; //segundos para completar a transformação
    bool alignToPath = false; //apenas para translate animada, se true a orientação do objeto é ajustada para seguir a direção do movimento

    std::vector<Point3D> controlPoints; //para a curva Catmull-rom
    //asrray de pontos de controlo

};

#endif