#ifndef TRANSFORM_H
#define TRANSFORM_H

    enum class TransformType {
        Translate, Rotate, Scale
    };

    struct Transform {
        TransformType type;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float angle = 0.0f;
    };

#endif