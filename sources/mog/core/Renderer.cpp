#include "mog/Constants.h"
#include "mog/core/Renderer.h"
#include "mog/core/Engine.h"
#include "mog/core/MogStats.h"
#include <math.h>
#include <string.h>

using namespace mog;

#if defined(MOG_DEBUG) && !defined(MOG_QT)
void checkGLError(const char *label) {
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

std::unordered_map<unsigned long long, std::weak_ptr<Renderer>> Renderer::allRenderers;
unsigned long long Renderer::rendererIdCounter = 1;

void Renderer::releaseAllBuffer() {
    for (auto &pair : allRenderers) {
        if (auto renderer = pair.second.lock()) {
            renderer->releaseBuffer();
        }
    }
}

std::shared_ptr<Renderer> Renderer::create() {
    auto renderer = std::shared_ptr<Renderer>(new Renderer());
    allRenderers[Renderer::rendererIdCounter++] = renderer;
    return renderer;
}

Renderer::~Renderer() {
    this->releaseBuffer();
    
    mogfree(this->vertices);
    mogfree(this->indices);
    mogfree(this->vertexColors);
    for (int i = 0; i < 4; i++ ) {
        if (this->vertexTexCoords[i]) mogfree(this->vertexTexCoords[i]);
    }
    
    allRenderers.erase(this->rendererId);
}

void Renderer::setDrawType(DrawType drawType) {
    this->drawType = drawType;
}

void Renderer::setLineWidth(float width) {
    glLineWidth(width);
}

void Renderer::setUniformPointSize(float size) {
    this->setUniformParameter("u_pointSize", size);
}

void Renderer::initScreenParameters() {
    if (this->screenParameterInitialized) return;
    auto screenSize = Screen::getSize();
    auto displaySize = Screen::getDisplaySize();
    float screenScale = Screen::getScreenScale();
    this->setUniformParameter("u_screenSize", screenSize.width,screenSize.height);
    this->setUniformParameter("u_displaySize", displaySize.width, displaySize.height);
    this->setUniformParameter("u_screenScale", screenScale);
    this->screenParameterInitialized = true;
}

void Renderer::setBlendFunc(BlendingFactor blendingFactorSrc, BlendingFactor blendingFactorDest) {
    this->blendingFactorSrc = blendingFactorSrc;
    this->blendingFactorDest = blendingFactorDest;
}

void Renderer::setUniformMatrix(const float *matrix) {
    this->setUniformParameter("u_matrix", matrix, 4);
}

void Renderer::setUniformColor(float r, float g, float b, float a) {
    this->setUniformParameter("u_color", r, g, b, a);
}

void Renderer::setUniformParameter(std::string name, const UniformParameter &param) {
    this->uniformParamsMap[name] = param;
    this->dirtyUniformParamsMap[name] = true;
}

void Renderer::setUniformParameter(std::string name, float f1) {
    this->setUniformParameter(name, UniformParameter(f1));
}

void Renderer::setUniformParameter(std::string name, float f1, float f2) {
    this->setUniformParameter(name, UniformParameter(f1, f2));
}

void Renderer::setUniformParameter(std::string name, float f1, float f2, float f3) {
    this->setUniformParameter(name, UniformParameter(f1, f2, f3));
}

void Renderer::setUniformParameter(std::string name, float f1, float f2, float f3, float f4) {
    this->setUniformParameter(name, UniformParameter(f1, f2, f3, f4));
}

void Renderer::setUniformParameter(std::string name, int i1) {
    this->setUniformParameter(name, UniformParameter(i1));
}

void Renderer::setUniformParameter(std::string name, int i1, int i2) {
    this->setUniformParameter(name, UniformParameter(i1, i2));
}

void Renderer::setUniformParameter(std::string name, int i1, int i2, int i3) {
    this->setUniformParameter(name, UniformParameter(i1, i2, i3));
}

void Renderer::setUniformParameter(std::string name, int i1, int i2, int i3, int i4) {
    this->setUniformParameter(name, UniformParameter(i1, i2, i3, i4));
}

void Renderer::setUniformParameter(std::string name, const float *matrix, int size) {
    this->setUniformParameter(name, UniformParameter(matrix, size));
}

unsigned int Renderer::bindAttributeLocation(std::string name) {
    if (this->attributeLocationMap.count(name) == 0) {
        this->attributeLocationMap[name] = this->attributeLocationIndexCounter++;
    }
    return this->attributeLocationMap[name];
}

void Renderer::setVertexAttributeParameter(unsigned int location, float f1) {
    this->setVertexAttributeParameter(location, VertexAttributeParameter(f1));
}

void Renderer::setVertexAttributeParameter(unsigned int location, float f1, float f2) {
    this->setVertexAttributeParameter(location, VertexAttributeParameter(f1, f2));
}

void Renderer::setVertexAttributeParameter(unsigned int location, float f1, float f2, float f3) {
    this->setVertexAttributeParameter(location, VertexAttributeParameter(f1, f2, f3));
}

void Renderer::setVertexAttributeParameter(unsigned int location, float f1, float f2, float f3, float f4) {
    this->setVertexAttributeParameter(location, VertexAttributeParameter(f1, f2, f3, f4));
}

void Renderer::setVertexAttributeParameter(unsigned int location, float *values, int arrSize, int size, bool dynamicDraw, bool normalized, int stride) {
    int index = this->getBufferIndex(location);
    
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrSize, values, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    this->setVertexAttributeParameter(location, VertexAttributeParameter(index, GL_FLOAT, size, normalized, stride));
}

void Renderer::setVertexAttributeParameter(unsigned int location, int *values, int arrSize, int size, bool dynamicDraw, bool normalized, int stride) {
    int index = this->getBufferIndex(location);

    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * arrSize, values, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->setVertexAttributeParameter(location, VertexAttributeParameter(index, GL_INT, size, normalized, stride));
}

void Renderer::setVertexAttributeParameter(unsigned int location, short *values, int arrSize, int size, bool dynamicDraw, bool normalized, int stride) {
    int index = this->getBufferIndex(location);
    
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(short) * arrSize, values, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    this->setVertexAttributeParameter(location, VertexAttributeParameter(index, GL_SHORT, size, normalized, stride));
}

void Renderer::bindVertexAttributePointerSub(unsigned int location, float *value, int arrSize, int offset) {
    int index = this->getBufferIndex(location);
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * offset, sizeof(float) * arrSize, value);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::setVertexAttributeParameter(unsigned int location, const VertexAttributeParameter &param) {
    this->vertexAttributeParamsMap[location] = param;
}

unsigned int Renderer::getBufferIndex(unsigned int location) {
    if (this->bufferIndexMap.count(location) == 0) {
        unsigned int index;
        glGenBuffers(1, &index);
        this->bufferIndexMap[location] = index;
    }
    return this->bufferIndexMap[location];
}


float Renderer::getMaxLineWidth() {
    GLfloat p;
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &p);
    return p;
}

float Renderer::getMaxPointSize() {
    GLfloat p;
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, &p);
    return p;
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
    
    checkGLError("bindVertex");
}

void Renderer::bindVertexTexCoords(int textureId, int textureIdx, bool dynamicDraw) {
    char texStr[16];
    sprintf(texStr, "u_texture%d", textureIdx);
    char uvStr[8];
    sprintf(uvStr, "a_uv%d", textureIdx);
    
    unsigned int location = this->bindAttributeLocation(uvStr);
    this->setVertexAttributeParameter(location, this->vertexTexCoords[textureIdx], this->verticesNum * 2, 2, dynamicDraw);
    this->setUniformParameter(texStr, textureIdx);

    this->textureId = textureId;
    
    checkGLError("bindTextureVertex");
}

void Renderer::bindVertexColors(bool dynamicDraw) {
    unsigned int location = this->bindAttributeLocation("a_color");
    this->setVertexAttributeParameter(location, this->vertexColors, this->verticesNum * 4, 4, dynamicDraw);
    
    this->enableVertexColor = true;
    
    checkGLError("bindColorsVertex");
}

void Renderer::bindVertexSub(int index, int size) {
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * index, sizeof(float) * size * 2, &this->vertices[index]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    checkGLError("bindVertexSub");
}

void Renderer::bindVertexTexCoordsSub(int index, int size, int textureIdx) {
    unsigned int location = this->bindAttributeLocation("a_uv");
    this->bindVertexAttributePointerSub(location, &this->vertexTexCoords[textureIdx][index], size * 2, index);

    checkGLError("bindTextureVertexSub");
}

void Renderer::bindVertexColorsSub(int index, int size) {
    unsigned int location = this->bindAttributeLocation("a_color");
    this->bindVertexAttributePointerSub(location, &this->vertexColors[index], size * 4, index);

    checkGLError("bindColorsVertexSub");
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
    if (!this->glShaderProgram) {
        this->initShaderProgram();
    }
    
    glBlendFunc((GLenum)this->blendingFactorSrc, (GLenum)this->blendingFactorDest);
    glUseProgram(this->glShaderProgram);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vertexBuffer[1]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindTexture(GL_TEXTURE_2D, this->textureId);

    for (auto pair : this->vertexAttributeParamsMap) {
        pair.second.setVertexAttribute(pair.first);
    }
    if (this->dirtyUniformParamsMap.size() > 0) {
        for (auto pair : this->dirtyUniformParamsMap) {
            auto uniform = this->uniformParamsMap[pair.first];
            uniform.setUniform(this->glShaderProgram, pair.first);
        }
        this->dirtyUniformParamsMap.clear();
    }

    // draw
    glDrawElements((int)this->drawType, this->indicesNum, GL_UNSIGNED_SHORT, 0);
    
    MogStats::drawCallCount++;

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    checkGLError("drawFrame");
}

void Renderer::initShaderProgram() {
    if (!this->vertexShader) this->vertexShader = this->getDefaultShader(ShaderType::VertexShader);
    if (!this->fragmentShader) this->fragmentShader = this->getDefaultShader(ShaderType::FragmentShader);

    this->glShaderProgram = glCreateProgram();
    glAttachShader(this->glShaderProgram, this->vertexShader->glShader);
    glAttachShader(this->glShaderProgram, this->fragmentShader->glShader);
    
    glBindAttribLocation(this->glShaderProgram, 0, "a_position");
    
    for (auto pair : this->attributeLocationMap) {
        glBindAttribLocation(this->glShaderProgram, pair.second, pair.first.c_str());
    }
    
    glLinkProgram(this->glShaderProgram);
#ifdef MOG_DEBUG
    GLint status = GL_FALSE;
    glGetProgramiv(this->glShaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        LOGE("Shader link error.");
        GLchar infolog[512];
        GLsizei length = 0;
        glGetProgramInfoLog(this->glShaderProgram, 512, &length, infolog);
        LOGE(infolog);
    }
#endif

    if (this->uniformParamsMap.count("u_matrix") == 0) {
        this->setUniformMatrix(Renderer::identityMatrix);
    }
    if (this->uniformParamsMap.count("u_color") == 0) {
        this->setUniformColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

std::shared_ptr<Shader> Renderer::getDefaultShader(ShaderType shaderType) {
    if (this->textureId > 0) {
        if (this->enableVertexColor) {
            return BasicShader::getShader(BasicShader::Type::VertexColorWithTexture, shaderType);
        } else {
            return BasicShader::getShader(BasicShader::Type::SolidColorWithTexture, shaderType);
        }
    } else if (this->drawType == DrawType::Points) {
        return BasicShader::getShader(BasicShader::Type::PointSprite, shaderType);
    } else {
        if (this->enableVertexColor) {
            return BasicShader::getShader(BasicShader::Type::VertexColor, shaderType);
        } else {
            return BasicShader::getShader(BasicShader::Type::SolidColor, shaderType);
        }
    }
}

void Renderer::releaseBuffer() {
    if (this->vertexBuffer[0] > 0) {
        glDeleteBuffers(2, this->vertexBuffer);
        this->vertexBuffer[0] = 0;
        this->vertexBuffer[1] = 0;
    }
    for (auto pair : this->bufferIndexMap) {
        glDeleteBuffers(1, &pair.second);
    }
    this->bufferIndexMap.clear();
    if (this->glShaderProgram > 0) {
        glDeleteProgram(this->glShaderProgram);
        this->glShaderProgram = 0;
        this->vertexShader = nullptr;
        this->fragmentShader = nullptr;
    }
    for (auto &pair : this->uniformParamsMap) {
        this->dirtyUniformParamsMap[pair.first] = true;
    }
}


Renderer::UniformParameter::UniformParameter() {
}

Renderer::UniformParameter::UniformParameter(float f1) {
    this->type = Type::Float1;
    this->f[0] = f1;
}

Renderer::UniformParameter::UniformParameter(float f1, float f2) {
    this->type = Type::Float2;
    this->f[0] = f1;
    this->f[1] = f2;
}

Renderer::UniformParameter::UniformParameter(float f1, float f2, float f3) {
    this->type = Type::Float3;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
}

Renderer::UniformParameter::UniformParameter(float f1, float f2, float f3, float f4) {
    this->type = Type::Float4;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
    this->f[3] = f4;
}

Renderer::UniformParameter::UniformParameter(int i1) {
    this->type = Type::Int1;
    this->i[0] = i1;
}

Renderer::UniformParameter::UniformParameter(int i1, int i2) {
    this->type = Type::Int2;
    this->i[0] = i1;
    this->i[1] = i2;
}

Renderer::UniformParameter::UniformParameter(int i1, int i2, int i3) {
    this->type = Type::Int3;
    this->i[0] = i1;
    this->i[1] = i2;
    this->i[2] = i3;
}

Renderer::UniformParameter::UniformParameter(int i1, int i2, int i3, int i4) {
    this->type = Type::Int4;
    this->i[0] = i1;
    this->i[1] = i2;
    this->i[2] = i3;
    this->i[3] = i4;
}

Renderer::UniformParameter::UniformParameter(const float *matrix, int size) {
    switch (size) {
        case 2:
            this->type = Type::Matrix2;
            memcpy(this->matrix, matrix, sizeof(float) * 4);
            break;
        case 3:
            this->type = Type::Matrix3;
            memcpy(this->matrix, matrix, sizeof(float) * 9);
            break;
        case 4:
            this->type = Type::Matrix4;
            memcpy(this->matrix, matrix, sizeof(float) * 16);
            break;
        default:
            throw std::invalid_argument("size must be between 2 and 4.");
    }
}

void Renderer::UniformParameter::setUniform(GLuint program, std::string name) {
    GLint location = glGetUniformLocation(program, name.c_str());
    switch (this->type) {
        case Type::Float1:
            glUniform1f(location, this->f[0]);
            break;
        case Type::Float2:
            glUniform2f(location, this->f[0], this->f[1]);
            break;
        case Type::Float3:
            glUniform3f(location, this->f[0], this->f[1], this->f[2]);
            break;
        case Type::Float4:
            glUniform4f(location, this->f[0], this->f[1], this->f[2], this->f[3]);
            break;
        case Type::Int1:
            glUniform1i(location, this->i[0]);
            break;
        case Type::Int2:
            glUniform2i(location, this->i[0], this->i[1]);
            break;
        case Type::Int3:
            glUniform3i(location, this->i[0], this->i[1], this->i[2]);
            break;
        case Type::Int4:
            glUniform4i(location, this->i[0], this->i[1], this->i[2], this->i[3]);
            break;
        case Type::Matrix2:
            glUniformMatrix2fv(location, 1, GL_FALSE, this->matrix);
            break;
        case Type::Matrix3:
            glUniformMatrix3fv(location, 1, GL_FALSE, this->matrix);
            break;
        case Type::Matrix4:
            glUniformMatrix4fv(location, 1, GL_FALSE, this->matrix);
            break;
    }
}

Renderer::VertexAttributeParameter::VertexAttributeParameter() {
}

Renderer::VertexAttributeParameter::VertexAttributeParameter(float f1) {
    this->type = Type::Float1;
    this->f[0] = f1;
}

Renderer::VertexAttributeParameter::VertexAttributeParameter(float f1, float f2) {
    this->type = Type::Float2;
    this->f[0] = f1;
    this->f[1] = f2;
}

Renderer::VertexAttributeParameter::VertexAttributeParameter(float f1, float f2, float f3) {
    this->type = Type::Float3;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
}

Renderer::VertexAttributeParameter::VertexAttributeParameter(float f1, float f2, float f3, float f4) {
    this->type = Type::Float4;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
    this->f[3] = f4;
}

Renderer::VertexAttributeParameter::VertexAttributeParameter(int index, GLenum glType, int size, bool normalized, int stride) {
    this->type = Type::VertexPointer;
    this->index = index;
    this->glType = glType;
    this->size = size;
    this->normalized = normalized;
    this->stride = stride;
}

void Renderer::VertexAttributeParameter::setVertexAttribute(unsigned int location) {
    glEnableVertexAttribArray(location);
    switch (this->type) {
        case Type::Float1:
            glVertexAttrib1f(location, this->f[0]);
            break;
        case Type::Float2:
            glVertexAttrib2f(location, this->f[0], this->f[1]);
            break;
        case Type::Float3:
            glVertexAttrib3f(location, this->f[0], this->f[1], this->f[2]);
            break;
        case Type::Float4:
            glVertexAttrib4f(location, this->f[0], this->f[1], this->f[2], this->f[3]);
            break;
        case Type::VertexPointer:
            glBindBuffer(GL_ARRAY_BUFFER, this->index);
            glVertexAttribPointer(location, this->size, this->glType, this->normalized, this->stride, 0);
            break;
    }
}
