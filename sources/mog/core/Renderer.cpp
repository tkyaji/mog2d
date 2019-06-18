#include "mog/core/Renderer.h"
#include "mog/core/Engine.h"
#include "mog/core/MogStats.h"
#include <math.h>
#include <string.h>

using namespace mog;

#if defined(MOG_DEBUG)
static void checkGLError(const char *label) {
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        LOGD("glError=%d : %s", glError, label);
    }
}
#else
#define checkGLError(label)
#endif

float Renderer::identityMatrix[20] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
    1, 1, 1, 1,
};

std::unordered_map<intptr_t, std::weak_ptr<Renderer>> Renderer::allRenderers;

void Renderer::releaseAllBufferes() {
    for (auto &pair : allRenderers) {
        if (auto renderer = pair.second.lock()) {
            renderer->releaseBuffer();
        }
    }
}

std::shared_ptr<Renderer> Renderer::create() {
    auto renderer = std::shared_ptr<Renderer>(new Renderer());
    allRenderers[(intptr_t)renderer.get()] = renderer;
    renderer->shader = Shader::create();
    return renderer;
}

Renderer::Renderer() {
    for (int i = 0; i < MULTI_TEXTURE_NUM; i++) {
        this->vertexTexCoords[i] = nullptr;
    }
}

Renderer::~Renderer() {
    this->releaseBuffer();

    mogfree(this->vertices);
    mogfree(this->indices);
    mogfree(this->vertexColors);
    for (int i = 0; i < MULTI_TEXTURE_NUM; i++ ) {
        if (this->vertexTexCoords[i]) mogfree(this->vertexTexCoords[i]);
    }
    
    allRenderers.erase((intptr_t)this);
}

void Renderer::setDrawType(DrawType drawType) {
    this->drawType = drawType;
}

void Renderer::initScreenParameters() {
    if (this->screenParameterInitialized) return;
    auto screenSize = Screen::getSize();
    auto displaySize = Screen::getDisplaySize();
    float screenScale = Screen::getScreenScale();
    this->shader->setUniformParameter("u_screenSize", screenSize.width,screenSize.height);
    this->shader->setUniformParameter("u_displaySize", displaySize.width, displaySize.height);
    this->shader->setUniformParameter("u_screenScale", screenScale);
    this->screenParameterInitialized = true;
}

void Renderer::setBlendFunc(BlendingFactor blendingFactorSrc, BlendingFactor blendingFactorDest) {
    this->blendingFactorSrc = blendingFactorSrc;
    this->blendingFactorDest = blendingFactorDest;
}

void Renderer::bindVertex(bool dynamicDraw) {
    if (this->vertexBuffer[0] == 0) {
        glGenBuffers(2, this->vertexBuffer);
    }
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->verticesNum * 2, this->vertices, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertexBuffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * this->indicesNum, this->indices, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    checkGLError("Renderer::bindVertex");
}

void Renderer::bindVertexTexCoords(const std::shared_ptr<Texture2D> &texture, int textureIdx, bool dynamicDraw) {
    char texStr[16];
    sprintf(texStr, "u_texture%d", textureIdx);
    
    this->shader->setVertexAttributeParameter(ATTR_LOCATION_IDX_UV_START + textureIdx, this->vertexTexCoords[textureIdx], this->verticesNum * 2, 2, dynamicDraw);
    this->shader->setUniformParameter(texStr, textureIdx);

    this->textures[textureIdx] = texture;
    
    checkGLError("Renderer::bindTextureVertex");
}

void Renderer::bindVertexColors(bool dynamicDraw) {
    this->shader->setVertexAttributeParameter(ATTR_LOCATION_IDX_COLOR, this->vertexColors, this->verticesNum * 4, 4, dynamicDraw);
    this->enableVertexColor = true;
    checkGLError("Renderer::bindColorsVertex");
}

void Renderer::bindVertexSub(int index, int size) {
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * index, sizeof(float) * size * 2, &this->vertices[index]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    checkGLError("Renderer::bindVertexSub");
}

void Renderer::bindVertexTexCoordsSub(int index, int size, int textureIdx) {
    this->shader->bindVertexAttributePointerSub(ATTR_LOCATION_IDX_UV_START + textureIdx, &this->vertexTexCoords[textureIdx][index], size * 2, index);

    checkGLError("Renderer::bindTextureVertexSub");
}

void Renderer::bindVertexColorsSub(int index, int size) {
    this->shader->bindVertexAttributePointerSub(ATTR_LOCATION_IDX_COLOR, &this->vertexColors[index], size * 4, index);

    checkGLError("Renderer::bindColorsVertexSub");
}

bool Renderer::setVerticesNum(int verticesNum) {
    if (this->verticesNum == verticesNum) return false;
    this->verticesNum = verticesNum;
    return true;
}

bool Renderer::setIndicesNum(int indicesNum) {
    if (this->indicesNum == indicesNum) return false;
    this->indicesNum = indicesNum;
    return true;
}

void Renderer::newVerticesArr() {
    this->vertices = (float *)mogrealloc(this->vertices, sizeof(float) * this->verticesNum * 2);
}

void Renderer::newIndicesArr() {
    this->indices = (short *)mogrealloc(this->indices, sizeof(short) * this->indicesNum);
}

void Renderer::newVertexColorsArr() {
    this->vertexColors = (float *)mogrealloc(this->vertexColors, sizeof(float) * this->verticesNum * 4);
    
}

void Renderer::newVertexTexCoordsArr(int textureIdx) {
    this->vertexTexCoords[textureIdx] = (float *)mogrealloc(this->vertexTexCoords[textureIdx], sizeof(float) * this->verticesNum * 2);
}

void Renderer::drawFrame() {
    if (!this->shader->vertexShader) this->shader->vertexShader = this->getDefaultShader(ShaderType::VertexShader);
    if (!this->shader->fragmentShader) this->shader->fragmentShader = this->getDefaultShader(ShaderType::FragmentShader);
    this->shader->compileIfNeed();
    
    glBlendFunc((GLenum)this->blendingFactorSrc, (GLenum)this->blendingFactorDest);
    glUseProgram(this->shader->glShaderProgram);
    
    glEnableVertexAttribArray(ATTR_LOCATION_IDX_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertexBuffer[1]);
    glVertexAttribPointer(ATTR_LOCATION_IDX_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    for (int i = 0; i < MULTI_TEXTURE_NUM; i++) {
        if (auto texture = this->textures[i].lock()) {
            glBindTexture(GL_TEXTURE_2D, texture->textureId);
        } else {
            break;
        }
    }

    this->shader->setParameters();

    // draw
    glDrawElements((int)this->drawType, this->indicesNum, GL_UNSIGNED_SHORT, 0);
    
    MogStats::drawCallCount++;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    checkGLError("Renderer::drawFrame");
}

std::shared_ptr<ShaderUnit> Renderer::getDefaultShader(ShaderType shaderType) {
    bool hasTexture = false;
    if (auto texture = this->textures[0].lock()) {
        hasTexture = (texture->textureId > 0);
    }
    if (hasTexture) {
        if (this->enableVertexColor) {
            return BasicShader::getShaderUnit(BasicShader::Type::VertexColorWithTexture, shaderType);
        } else {
            return BasicShader::getShaderUnit(BasicShader::Type::SolidColorWithTexture, shaderType);
        }
    } else if (this->drawType == DrawType::Points) {
        return BasicShader::getShaderUnit(BasicShader::Type::PointSprite, shaderType);
    } else {
        if (this->enableVertexColor) {
            return BasicShader::getShaderUnit(BasicShader::Type::VertexColor, shaderType);
        } else {
            return BasicShader::getShaderUnit(BasicShader::Type::SolidColor, shaderType);
        }
    }
}

void Renderer::releaseBuffer() {
    if (this->vertexBuffer[0] > 0) {
        glDeleteBuffers(2, this->vertexBuffer);
        this->vertexBuffer[0] = 0;
        this->vertexBuffer[1] = 0;
    }
    this->screenParameterInitialized = false;

    checkGLError("Renderer::releaseBuffer");
}


