#include "mog/core/Transform.h"
#include "mog/core/Renderer.h"
#include "mog/core/mog_functions.h"
#include <string.h>

using namespace mog;

void Transform::updateMatrix(float *parentMatrix) {
    if (parentMatrix != nullptr) {
        memcpy(&this->tmpMatrix[0], parentMatrix, sizeof(float) * 16);
    } else {
        memcpy(&this->tmpMatrix[0], Renderer::identityMatrix, sizeof(float) * 16);
    }

    memcpy(&this->tmpMatrix[16], Renderer::identityMatrix, sizeof(float) * 16);
    float x = this->size.width * this->pivot.x;
    float y = this->size.height * this->pivot.y;
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
    tmpMatrix[16+12] = this->position.x + this->offest.x;
    tmpMatrix[16+13] = this->position.y + this->offest.y;
    Transform::multiplyMatrix(&tmpMatrix[32], &tmpMatrix[16], &tmpMatrix[0]);

    memcpy(this->matrix, &tmpMatrix[0], sizeof(float) * 16);
}

void Transform::updateColor(float *parentMatrix) {
    this->matrix[16] = this->color.r;
    this->matrix[17] = this->color.g;
    this->matrix[18] = this->color.b;
    this->matrix[19] = this->color.a;
}

void Transform::setMatrix(float *matrix) {
    memcpy(this->matrix, matrix, 16);
    this->position.x = matrix[12];
    this->position.y = matrix[13];
    float radian = atan2f(matrix[1], matrix[5]);
    this->rotation = radian * (180.0f / M_PI);
    this->scale.x = sqrt(matrix[0] * matrix[0] + matrix[4] * matrix[4]);
    this->scale.y = sqrt(matrix[1] * matrix[1] + matrix[5] * matrix[5]);
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

bool Transform::inverseMatrix(float *matrix, float *dstMatrix) {
    float *m = matrix;
    float det = m[0] * m[5] * m[10] * m[15] + m[0] * m[6] * m[11] * m[13] + m[0] * m[7] * m[9] * m[14] +
                m[1] * m[4] * m[11] * m[14] + m[1] * m[6] * m[8] * m[15] + m[1] * m[7] * m[10] * m[12] +
                m[2] * m[4] * m[9] * m[15] + m[2] * m[5] * m[11] * m[12] + m[2] * m[7] * m[8] * m[13] +
                m[3] * m[4] * m[10] * m[13] + m[3] * m[5] * m[8] * m[14] + m[3] * m[6] * m[9] * m[12] -
                m[0] * m[5] * m[11] * m[14] - m[0] * m[6] * m[9] * m[15] - m[0] * m[7] * m[10] * m[13] -
                m[1] * m[4] * m[10] * m[15] - m[1] * m[6] * m[11] * m[12] - m[1] * m[7] * m[8] * m[14] -
                m[2] * m[4] * m[11] * m[13] - m[2] * m[5] * m[8] * m[15] - m[2] * m[7] * m[9] * m[12] -
                m[3] * m[4] * m[9] * m[14] - m[3] * m[5] * m[10] * m[12] - m[3] * m[6] * m[8] * m[13];
    
    if (det < 0.0000000001f || approximately(det, 1.0f)) {
        memcpy(dstMatrix, matrix, 16);
        return false;
    }
    
    dstMatrix[0] = det * (m[5] * m[10] * m[15] + m[6] * m[11] * m[13] + m[7] * m[9] * m[14] - m[5] * m[11] * m[14] - m[6] * m[9] * m[15] - m[7] * m[10] * m[13]);
    dstMatrix[1] = det * (m[1] * m[11] * m[14] + m[2] * m[9] * m[15] + m[3] * m[10] * m[13] - m[1] * m[10] * m[15] - m[2] * m[11] * m[13] - m[3] * m[9] * m[14]);
    dstMatrix[2] = det * (m[1] * m[6] * m[15] + m[2] * m[7] * m[13] + m[3] * m[5] * m[14] - m[1] * m[7] * m[14] - m[2] * m[5] * m[15] - m[3] * m[6] * m[13]);
    dstMatrix[3] = det * (m[1] * m[7] * m[10] + m[2] * m[5] * m[11] + m[3] * m[6] * m[9] - m[1] * m[6] * m[11] - m[2] * m[7] * m[9] - m[3] * m[5] * m[10]);
    dstMatrix[4] = det * (m[4] * m[11] * m[14] + m[6] * m[8] * m[15] + m[7] * m[10] * m[12] - m[4] * m[10] * m[15] - m[6] * m[11] * m[12] - m[7] * m[8] * m[14]);
    dstMatrix[5] = det * (m[0] * m[10] * m[15] + m[2] * m[11] * m[12] + m[3] * m[8] * m[14] - m[0] * m[11] * m[14] - m[2] * m[8] * m[15] - m[3] * m[10] * m[12]);
    dstMatrix[6] = det * (m[0] * m[7] * m[14] + m[2] * m[4] * m[15] + m[3] * m[6] * m[12] - m[0] * m[6] * m[15] - m[2] * m[7] * m[12] - m[3] * m[4] * m[14]);
    dstMatrix[7] = det * (m[0] * m[6] * m[11] + m[2] * m[7] * m[8] + m[3] * m[4] * m[10] - m[0] * m[7] * m[10] - m[2] * m[4] * m[11] - m[3] * m[6] * m[8]);
    dstMatrix[8] = det * (m[4] * m[9] * m[15] + m[5] * m[11] * m[12] + m[7] * m[8] * m[13] - m[4] * m[11] * m[13] - m[5] * m[8] * m[15] - m[7] * m[9] * m[12]);
    dstMatrix[9] = det * (m[0] * m[11] * m[13] + m[1] * m[8] * m[15] + m[3] * m[9] * m[12] - m[0] * m[9] * m[15] - m[1] * m[11] * m[12] - m[3] * m[8] * m[13]);
    dstMatrix[10] = det * (m[0] * m[5] * m[15] + m[1] * m[7] * m[12] + m[3] * m[4] * m[13] - m[0] * m[7] * m[13] - m[1] * m[4] * m[15] - m[3] * m[5] * m[12]);
    dstMatrix[11] = det * (m[0] * m[7] * m[9] + m[1] * m[4] * m[11] + m[3] * m[5] * m[8] - m[0] * m[5] * m[11] - m[1] * m[7] * m[8] - m[3] * m[4] * m[9]);
    dstMatrix[12] = det * (m[4] * m[10] * m[13] + m[5] * m[8] * m[14] + m[6] * m[9] * m[12] - m[4] * m[9] * m[14] - m[5] * m[10] * m[12] - m[6] * m[8] * m[13]);
    dstMatrix[13] = det * (m[0] * m[9] * m[14] + m[1] * m[10] * m[12] + m[2] * m[8] * m[13] - m[0] * m[10] * m[13] - m[1] * m[8] * m[14] - m[2] * m[9] * m[12]);
    dstMatrix[14] = det * (m[0] * m[6] * m[13] + m[1] * m[4] * m[14] + m[2] * m[5] * m[12] - m[0] * m[5] * m[14] - m[1] * m[6] * m[12] - m[2] * m[4] * m[13]);
    dstMatrix[15] = det * (m[0] * m[5] * m[10] + m[1] * m[6] * m[8] + m[2] * m[4] * m[9] - m[0] * m[6] * m[9] - m[1] * m[4] * m[10] - m[2] * m[5] * m[8]);
    
    return true;
}
