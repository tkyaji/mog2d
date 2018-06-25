#include "mog/core/Transform.h"
#include "mog/core/Renderer.h"
#include <string.h>

using namespace mog;

void Transform::updateMatrix(float *parentMatrix) {
    if (parentMatrix != nullptr) {
        memcpy(&this->tmpMatrix[0], parentMatrix, sizeof(float) * 16);
    } else {
        memcpy(&this->tmpMatrix[0], Renderer::identityMatrix, sizeof(float) * 16);
    }

    memcpy(&this->tmpMatrix[16], Renderer::identityMatrix, sizeof(float) * 16);
    float x = this->size.width * this->anchor.x;
    float y = this->size.height * this->anchor.y;
    this->tmpMatrix[16+12] = -x;
    this->tmpMatrix[16+13] = -y;
    Transform::multiplyMatrix(&tmpMatrix[0], &tmpMatrix[16], &tmpMatrix[32]);
    
    memcpy(&this->tmpMatrix[16], Renderer::identityMatrix, sizeof(float) * 16);
    float radian = 2 * M_PI * this->rotation / 360.0f;
    float cosv = cos(radian);
    float sinv = sin(radian);
    tmpMatrix[16+0] = cosv;
    tmpMatrix[16+1] = sinv;
    tmpMatrix[16+4] = -sinv;
    tmpMatrix[16+5] = cosv;
    Transform::multiplyMatrix(&tmpMatrix[32], &tmpMatrix[16], &tmpMatrix[0]);

    memcpy(&this->tmpMatrix[16], Renderer::identityMatrix, sizeof(float) * 16);
    tmpMatrix[16+0] = this->scale.x;
    tmpMatrix[16+5] = this->scale.y;
    Transform::multiplyMatrix(&tmpMatrix[0], &tmpMatrix[16], &tmpMatrix[32]);

    memcpy(&this->tmpMatrix[16], Renderer::identityMatrix, sizeof(float) * 16);
    tmpMatrix[16+12] = this->position.x;
    tmpMatrix[16+13] = this->position.y;
    Transform::multiplyMatrix(&tmpMatrix[32], &tmpMatrix[16], &tmpMatrix[0]);

    memcpy(this->matrix, &tmpMatrix[0], sizeof(float) * 16);
}

void Transform::updateColor(float *parentMatrix) {
    this->matrix[16] = this->color.r;
    this->matrix[17] = this->color.g;
    this->matrix[18] = this->color.b;
    this->matrix[19] = this->color.a;
}

void Transform::multiplyMatrix(float *matrix1, float *matrix2, float *dstMatrix) {
    for (int i = 0; i < 16; i++) {
        dstMatrix[i] = 0;
        for (int k = 0; k < 4; k++) {
            dstMatrix[i] += matrix1[(int)(i/4)*4+k] * matrix2[i%4+4*k];
        }
    }
}

void Transform::multiplyColor(float *matrix1, float *matrix2, float *dstMatrix) {
    for (int i = 16; i < 20; i++) {
        dstMatrix[i] = matrix1[i] * matrix2[i];
    }
}

