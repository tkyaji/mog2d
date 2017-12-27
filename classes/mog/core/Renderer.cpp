#include "mog/Constants.h"
#include "mog/core/Renderer.h"
#include "mog/core/Engine.h"
#include "mog/core/MogStats.h"
#include <math.h>

using namespace mog;

void checkGLError(const char *label) {
#ifndef MOG_QT
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        LOGE("glError=%d : %s", glError, label);
    }
#endif
}

float Renderer::identityMatrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};;

Renderer::Renderer() {
}

Renderer::~Renderer() {
    if (this->vertexBuffer[0] > 0) {
        glDeleteBuffers(2, this->vertexBuffer);
    }
    if (this->vertexTexCoordsBuffer > 0) {
        glDeleteBuffers(1, &this->vertexTexCoordsBuffer);
    }
    if (this->vertexColorsBuffer > 0) {
        glDeleteBuffers(1, &this->vertexColorsBuffer);
    }
}

void Renderer::bindVertex(float *vertices, int verticesSize, short *indices, int indicesSize, bool dynamicDraw) {
    if (this->vertexBuffer[0] == 0) {
        glGenBuffers(2, this->vertexBuffer);
    }
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, vertices, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertexBuffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * indicesSize, indices, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    this->indicesNum = indicesSize;

    checkGLError("bindVertex");
}

void Renderer::bindTextureVertex(int textureId, float *vertexTexCoords, int size, bool dynamicDraw) {
    if (this->vertexTexCoordsBuffer == 0) {
        glGenBuffers(1, &this->vertexTexCoordsBuffer);
    }
    
    glEnable(GL_TEXTURE_2D);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexTexCoordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, vertexTexCoords, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    this->textureId = textureId;
    this->enableTexture = true;

    checkGLError("bindTextureVertex");
}

void Renderer::bindColorsVertex(float *vertexColors, int size, bool dynamicDraw) {
    if (this->vertexColorsBuffer == 0) {
        glGenBuffers(1, &this->vertexColorsBuffer);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexColorsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, vertexColors, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->enableColor = true;

    checkGLError("bindColorsVertex");
}

void Renderer::bindVertexSub(float *vertices, int size, int offset) {
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float) * size, vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    checkGLError("bindVertexSub");
}

void Renderer::bindColorsVertexSub(float *vertexColors, int size, int offset) {
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexColorsBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float) * size, vertexColors);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    checkGLError("bindColorsVertexSub");
}

void Renderer::drawFrame(const shared_ptr<Transform> &transform, float screenScale) {
    this->pushMatrix();
    this->pushColor();
    
    this->applyTransform(transform, screenScale);
    this->drawFrame();
    
    this->popColor();
    this->popMatrix();

    checkGLError("drawFrame");
}

void Renderer::drawFrame() {
    // bind vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    
    // bind indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertexBuffer[1]);

    if (this->enableTexture) {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glBindTexture(GL_TEXTURE_2D, this->textureId);
        
        // bind vertex texture coords
        glBindBuffer(GL_ARRAY_BUFFER, this->vertexTexCoordsBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
    }
    
    if (this->enableColor) {
        glEnableClientState(GL_COLOR_ARRAY);
        // bind vertex colors
        glBindBuffer(GL_ARRAY_BUFFER, this->vertexColorsBuffer);
        glColorPointer(4, GL_FLOAT, 0, 0);
    }
    
    // draw
    glDrawElements(GL_TRIANGLE_STRIP, this->indicesNum, GL_UNSIGNED_SHORT, 0);
    
    MogStats::drawCallCount++;
    
    // reset
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    checkGLError("drawFrame_main");
}

void Renderer::applyTransform(const shared_ptr<Transform> &transform, float screenScale, bool enableColor) {
    // translate
    if (transform->position.x != 0 || transform->anchor.x != 0 || transform->position.y != 0 || transform->anchor.y != 0) {
        glTranslatef((transform->position.x - (transform->size.width * transform->anchor.x)) * screenScale,
                     (transform->position.y - (transform->size.height * transform->anchor.y)) * screenScale, 0);
    }
    
    // move from origin point
    if (transform->rotation != 0 || transform->scale.x != 1.0f || transform->scale.y != 1.0f) {
        glTranslatef(transform->size.width * transform->anchor.x * screenScale,
                     transform->size.height * transform->anchor.y * screenScale, 0);
    }
    
    // rotate
    if (transform->rotation != 0) {
        glRotatef(transform->rotation, 0, 0, 1.0f);
    }
    
    // scale
    if (transform->scale.x != 1.0f || transform->scale.y != 1.0f) {
        glScalef(transform->scale.x, transform->scale.y, 1.0f);
    }
    
    // move to origin point
    if (transform->rotation != 0 || transform->scale.x != 1.0f || transform->scale.y != 1.0f) {
        glTranslatef(transform->size.width * -transform->anchor.x * screenScale,
                     transform->size.height * -transform->anchor.y * screenScale, 0);
    }
    
    // color
    if (enableColor) {
        if (transform->color.r < 1.0f || transform->color.g < 1.0f || transform->color.b < 1.0f || transform->color.a < 1.0f) {
            glColor4f(transform->color.r * this->_color[0], transform->color.g * this->_color[1],
                      transform->color.b * this->_color[2], transform->color.a * this->_color[3]);
        }
    }
    
    glGetFloatv(GL_MODELVIEW_MATRIX, this->matrix);

    checkGLError("applyTransform");
}

void Renderer::getMatrix(float *matrix) {
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
}

void Renderer::setMatrix(float *matrix) {
    glLoadMatrixf(matrix);
}

void Renderer::pushMatrix() {
    glPushMatrix();
}

void Renderer::popMatrix() {
    glPopMatrix();
}

void Renderer::pushColor() {
    glGetFloatv(GL_CURRENT_COLOR, this->_color);
}

void Renderer::popColor() {
    glColor4f(this->_color[0], this->_color[1], this->_color[2], this->_color[3]);
}
