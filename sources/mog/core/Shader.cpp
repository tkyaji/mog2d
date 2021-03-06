#include "mog/core/Shader.h"
#include "mog/core/opengl.h"
#include "mog/core/FileUtils.h"
#include "mog/core/Renderer.h"
#include "mog/Constants.h"
#include "mog/core/shader_sources.h"
#include "mog/core/Screen.h"

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

#pragma - ShaderUnit

std::unordered_map<intptr_t, std::weak_ptr<ShaderUnit>> ShaderUnit::allShaderUnits;

void ShaderUnit::releaseAllBufferes() {
    for (auto &pair : allShaderUnits) {
        if (auto shaderUnit = pair.second.lock()) {
            shaderUnit->releaseBuffer();
        }
    }
}

std::shared_ptr<ShaderUnit> ShaderUnit::create(const GLchar *shaderSource, ShaderType shaderType) {
    auto shaderUnit = std::shared_ptr<ShaderUnit>(new ShaderUnit());
    shaderUnit->shaderType = shaderType;
    shaderUnit->shaderSource = (GLchar *)mogmalloc(sizeof(GLchar) * strlen(shaderSource) + 1);
    strcpy(shaderUnit->shaderSource, shaderSource);
    allShaderUnits[(intptr_t)shaderUnit.get()] = shaderUnit;
    return shaderUnit;
}

std::shared_ptr<ShaderUnit> ShaderUnit::createWithAsset(std::string shaderSourceFilename, ShaderType shaderType) {
    std::string shaderSource = FileUtils::readTextAsset(shaderSourceFilename);
    return ShaderUnit::create(shaderSource.c_str(), shaderType);
}

void ShaderUnit::compileIfNeed() {
    if (this->glShader) return;
    
    GLenum glShaderType = (this->shaderType == ShaderType::FragmentShader) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;

    this->glShader = glCreateShader(glShaderType);
    glShaderSource(this->glShader, 1, &this->shaderSource, NULL);
    glCompileShader(this->glShader);
    
#ifdef MOG_DEBUG
    GLint status = GL_FALSE;
    glGetShaderiv(this->glShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLsizei bufsize = 0;
        glGetShaderiv(this->glShader, GL_INFO_LOG_LENGTH, &bufsize);
        GLsizei length = 0;
        GLchar *infolog = (GLchar *)mogmalloc(sizeof(GLchar) * bufsize);
        glGetShaderInfoLog(this->glShader, bufsize, &length, infolog);
        LOGE("Shader compile error: %s", (const char *)infolog);
        mogfree(infolog);
    }
#endif
    
    checkGLError("ShaderUnit::compileIfNeed");
}

void ShaderUnit::releaseBuffer() {
    if (this->glShader) {
        glDeleteShader(this->glShader);
        this->glShader = 0;
        checkGLError("ShaderUnit::releaseBuffer");
    }
}

ShaderUnit::~ShaderUnit() {
    this->releaseBuffer();
    mogfree(this->shaderSource);
    
    allShaderUnits.erase((intptr_t)this);
}



#pragma - Shader

/*
std::unordered_map<intptr_t, std::weak_ptr<Shader>> Shader::allShaders;

void Shader::releaseAllBufferes() {
    for (auto &pair : allShaders) {
        if (auto shader = pair.second.lock()) {
            shader->releaseBuffer();
        }
    }
}
*/

std::shared_ptr<Shader> Shader::create() {
    auto shader = std::shared_ptr<Shader>(new Shader());
//    allShaders[(intptr_t)shader.get()] = shader;
    shader->setUniformMatrix(Renderer::identityMatrix);
    shader->setUniformColor(1.0f, 1.0f, 1.0f, 1.0f);
    return shader;
}

void Shader::compileIfNeed() {
    if (this->glShaderProgram) return;
    
    this->glShaderProgram = glCreateProgram();
    
    this->vertexShader->compileIfNeed();
    this->fragmentShader->compileIfNeed();
    
    glAttachShader(this->glShaderProgram, this->vertexShader->glShader);
    glAttachShader(this->glShaderProgram, this->fragmentShader->glShader);
    checkGLError("Shader::glAttachShader");

    int max;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
    for (auto pair : this->attributeLocationMap) {
        if (pair.second >= max) {
            LOGE("glBindAttribLocation: attribute index is overflow (max=%d)\n", max);
        }
        glBindAttribLocation(this->glShaderProgram, pair.second, pair.first.c_str());
    }
    checkGLError("Shader::glBindAttribLocation");

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
    
    checkGLError("ShaderUnit::compileIfNeed");
}

void Shader::setParameters() {
    for (auto pair : this->vertexAttributeParamsMap) {
        auto attrParam = pair.second;
        int bufferIndex = 0;
        if (attrParam.type == VertexAttributeParameter::Type::VertexPointer) {
            bufferIndex = this->getBufferIndex(pair.first);
        }
        attrParam.setVertexAttribute(pair.first, bufferIndex);
    }
    for (auto pair : this->dirtyUniformParamsMap) {
        auto uniform = this->uniformParamsMap[pair.first];
        uniform.setUniform(this->glShaderProgram, pair.first);
    }
    this->dirtyUniformParamsMap.clear();
}

void Shader::setLineWidth(float width) {
    glLineWidth(width);
}

void Shader::setUniformPointSize(float size) {
    this->setUniformParameter("u_pointSize", size);
}

void Shader::setUniformMatrix(const float *matrix) {
    this->setUniformParameter("u_matrix", matrix, 4);
}

void Shader::setUniformColor(float r, float g, float b, float a) {
    this->setUniformParameter("u_color", r, g, b, a);
}

void Shader::setUniformParameter(std::string name, const UniformParameter &param) {
    this->uniformParamsMap[name] = param;
    this->dirtyUniformParamsMap[name] = true;
}

void Shader::setUniformParameter(std::string name, float f1) {
    this->setUniformParameter(name, UniformParameter(f1));
}

void Shader::setUniformParameter(std::string name, float f1, float f2) {
    this->setUniformParameter(name, UniformParameter(f1, f2));
}

void Shader::setUniformParameter(std::string name, float f1, float f2, float f3) {
    this->setUniformParameter(name, UniformParameter(f1, f2, f3));
}

void Shader::setUniformParameter(std::string name, float f1, float f2, float f3, float f4) {
    this->setUniformParameter(name, UniformParameter(f1, f2, f3, f4));
}

void Shader::setUniformParameter(std::string name, int i1) {
    this->setUniformParameter(name, UniformParameter(i1));
}

void Shader::setUniformParameter(std::string name, int i1, int i2) {
    this->setUniformParameter(name, UniformParameter(i1, i2));
}

void Shader::setUniformParameter(std::string name, int i1, int i2, int i3) {
    this->setUniformParameter(name, UniformParameter(i1, i2, i3));
}

void Shader::setUniformParameter(std::string name, int i1, int i2, int i3, int i4) {
    this->setUniformParameter(name, UniformParameter(i1, i2, i3, i4));
}

void Shader::setUniformParameter(std::string name, const float *matrix, int size) {
    this->setUniformParameter(name, UniformParameter(matrix, size));
}

void Shader::setVertexAttributeParameter(std::string name, float f1) {
    this->setVertexAttributeParameter(name, VertexAttributeParameter(f1));
}

void Shader::setVertexAttributeParameter(std::string name, float f1, float f2) {
    this->setVertexAttributeParameter(name, VertexAttributeParameter(f1, f2));
}

void Shader::setVertexAttributeParameter(std::string name, float f1, float f2, float f3) {
    this->setVertexAttributeParameter(name, VertexAttributeParameter(f1, f2, f3));
}

void Shader::setVertexAttributeParameter(std::string name, float f1, float f2, float f3, float f4) {
    this->setVertexAttributeParameter(name, VertexAttributeParameter(f1, f2, f3, f4));
}

void Shader::setVertexAttributeParameter(std::string name, float *values, int arrLength, int size, bool dynamicDraw, bool normalized, int stride) {
//    int index = this->getBufferIndex(location);
    
    /*
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrSize, values, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGLError("Shader::setVertexAttributeParameter0");
     */
    
    this->setVertexAttributeParameter(name, VertexAttributeParameter(GL_FLOAT, values, sizeof(float) * arrLength, size, dynamicDraw, normalized, stride));
}

void Shader::setVertexAttributeParameter(std::string name, int *values, int arrLength, int size, bool dynamicDraw, bool normalized, int stride) {
//    int index = this->getBufferIndex(location);

    /*
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(int) * arrSize, values, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGLError("Shader::setVertexAttributeParameter1");
     */

    this->setVertexAttributeParameter(name, VertexAttributeParameter(GL_INT, values, sizeof(int) * arrLength, size, dynamicDraw, normalized, stride));
}

void Shader::setVertexAttributeParameter(std::string name, short *values, int arrLength, int size, bool dynamicDraw, bool normalized, int stride) {
//    int index = this->getBufferIndex(location);
    
    /*
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(short) * arrSize, values, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGLError("Shader::setVertexAttributeParameter2");
     */

    this->setVertexAttributeParameter(name, VertexAttributeParameter(GL_SHORT, values, sizeof(short) * arrLength, size, dynamicDraw, normalized, stride));
}

void Shader::setVertexAttributeParameter(std::string name, const VertexAttributeParameter &param) {
    unsigned int location = this->bindAttributeLocation(name);
    this->vertexAttributeParamsMap[location] = param;
}

void Shader::bindVertexAttributeParameter(unsigned int location, float *values, int arrLength, int size, bool dynamicDraw, bool normalized, int stride) {
    int index = this->getBufferIndex(location);
    
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * arrLength, values, (dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGLError("Shader::bindVertexAttributeParameter");
    
    this->vertexAttributeParamsMap[location] = VertexAttributeParameter(GL_FLOAT, nullptr, 0, size, dynamicDraw, normalized, stride);
}

void Shader::bindVertexAttributePointerSub(unsigned int location, float *value, int arrLength, int offset) {
    int index = this->getBufferIndex(location);
    
    glBindBuffer(GL_ARRAY_BUFFER, index);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * offset, sizeof(float) * arrLength, value);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGLError("Shader::bindVertexAttributePointerSub");
}

unsigned int Shader::bindAttributeLocation(std::string name) {
    if (this->attributeLocationMap.count(name) == 0) {
        this->attributeLocationMap[name] = this->attributeLocationIndexCounter++;
    }
    return this->attributeLocationMap[name];
}

void Shader::bindAttributeLocation(std::string name, unsigned int location) {
    this->attributeLocationMap[name] = location;
}

unsigned int Shader::getBufferIndex(unsigned int location) {
    if (this->bufferIndexMap.count(location) == 0) {
        unsigned int index;
        glGenBuffers(1, &index);
        this->bufferIndexMap[location] = index;
    }
    checkGLError("Shader::getBufferIndex");
    return this->bufferIndexMap[location];
}

float Shader::getMaxLineWidth() {
    GLfloat p;
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &p);
    return p;
}

float Shader::getMaxPointSize() {
    GLfloat p;
    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, &p);
    return p;
}

Shader::UniformParameter::UniformParameter() {
}

Shader::UniformParameter::UniformParameter(float f1) {
    this->type = Type::Float1;
    this->f[0] = f1;
}

Shader::UniformParameter::UniformParameter(float f1, float f2) {
    this->type = Type::Float2;
    this->f[0] = f1;
    this->f[1] = f2;
}

Shader::UniformParameter::UniformParameter(float f1, float f2, float f3) {
    this->type = Type::Float3;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
}

Shader::UniformParameter::UniformParameter(float f1, float f2, float f3, float f4) {
    this->type = Type::Float4;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
    this->f[3] = f4;
}

Shader::UniformParameter::UniformParameter(int i1) {
    this->type = Type::Int1;
    this->i[0] = i1;
}

Shader::UniformParameter::UniformParameter(int i1, int i2) {
    this->type = Type::Int2;
    this->i[0] = i1;
    this->i[1] = i2;
}

Shader::UniformParameter::UniformParameter(int i1, int i2, int i3) {
    this->type = Type::Int3;
    this->i[0] = i1;
    this->i[1] = i2;
    this->i[2] = i3;
}

Shader::UniformParameter::UniformParameter(int i1, int i2, int i3, int i4) {
    this->type = Type::Int4;
    this->i[0] = i1;
    this->i[1] = i2;
    this->i[2] = i3;
    this->i[3] = i4;
}

Shader::UniformParameter::UniformParameter(const float *matrix, int size) {
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

void Shader::UniformParameter::setUniform(GLuint program, std::string name) {
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
    checkGLError("Shader::UniformParameter::setUniform");
}

Shader::VertexAttributeParameter::VertexAttributeParameter() {
}

Shader::VertexAttributeParameter::VertexAttributeParameter(float f1) {
    this->type = Type::Float1;
    this->f[0] = f1;
}

Shader::VertexAttributeParameter::VertexAttributeParameter(float f1, float f2) {
    this->type = Type::Float2;
    this->f[0] = f1;
    this->f[1] = f2;
}

Shader::VertexAttributeParameter::VertexAttributeParameter(float f1, float f2, float f3) {
    this->type = Type::Float3;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
}

Shader::VertexAttributeParameter::VertexAttributeParameter(float f1, float f2, float f3, float f4) {
    this->type = Type::Float4;
    this->f[0] = f1;
    this->f[1] = f2;
    this->f[2] = f3;
    this->f[3] = f4;
}

Shader::VertexAttributeParameter::VertexAttributeParameter(GLenum glType, void *values, size_t valueSize, int size, bool dynamicDraw, bool normalized, int stride) {
    this->type = Type::VertexPointer;
    this->glType = glType;
    this->size = size;
    this->dynamicDraw = dynamicDraw;
    this->normalized = normalized;
    this->stride = stride;
    if (valueSize > 0) {
        this->data = std::make_shared<Data>(values, valueSize);
        this->dirty = true;
    }
}


void Shader::VertexAttributeParameter::setVertexAttribute(unsigned int location, int bufferIndex) {
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
            glBindBuffer(GL_ARRAY_BUFFER, bufferIndex);
            if (this->dirty) {
                glBufferData(GL_ARRAY_BUFFER, this->data->valueSize, this->data->values, (this->dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
                this->dirty = false;
            }
            glVertexAttribPointer(location, this->size, this->glType, this->normalized, this->stride, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            break;
    }
    checkGLError("Shader::VertexAttributeParameter::setVertexAttribute");
}

Shader::VertexAttributeParameter::Data::Data(void *values, size_t valueSize) {
    this->valueSize = valueSize;
    this->values = mogmalloc(valueSize);
    memcpy(this->values, values, valueSize);
}

Shader::VertexAttributeParameter::Data::~Data() {
    if (this->values) {
        mogfree(this->values);
    }
}


Shader::~Shader() {
    this->releaseBuffer();
//    allShaders.erase((intptr_t)this);
}

void Shader::attachVertexShader(const std::shared_ptr<ShaderUnit> &vertexShader) {
    this->releaseBuffer();
    this->vertexShader = vertexShader;
}

void Shader::attachFragmentShader(const std::shared_ptr<ShaderUnit> &fragmentShader) {
    this->releaseBuffer();
    this->fragmentShader = fragmentShader;
}

void Shader::releaseBuffer() {
    if (this->bufferIndexMap.size() > 0) {
        for (auto pair : this->bufferIndexMap) {
            glDeleteBuffers(1, &pair.second);
        }
        checkGLError("Shader::releaseBuffer glDeleteBuffers");
    }
    this->bufferIndexMap.clear();
    
    if (this->glShaderProgram > 0) {
        if (this->vertexShader && this->vertexShader->glShader) {
            glDetachShader(this->glShaderProgram, this->vertexShader->glShader);
            checkGLError("Shader::releaseBuffer glDetachShader(vertexShader)");
        }
        if (this->fragmentShader && this->fragmentShader->glShader) {
            glDetachShader(this->glShaderProgram, this->fragmentShader->glShader);
            checkGLError("Shader::releaseBuffer glDetachShader(fragmentShader)");
        }
        
        glDeleteProgram(this->glShaderProgram);
        this->glShaderProgram = 0;
        checkGLError("Shader::releaseBuffer glDeleteProgram");
    }
    /*
    if (this->vertexShader) {
        this->vertexShader->releaseBuffer();
    }
    if (this->fragmentShader) {
        this->fragmentShader->releaseBuffer();
    }
     */
    for (auto &pair : this->vertexAttributeParamsMap) {
        if (pair.second.data != nullptr) {
            pair.second.dirty = true;
        }
    }
    for (auto &pair : this->uniformParamsMap) {
        this->dirtyUniformParamsMap[pair.first] = true;
    }
}


#pragma - BasicShader

std::unordered_map<int, std::shared_ptr<ShaderUnit>> BasicShader::vertexShaderCache;
std::unordered_map<int, std::shared_ptr<ShaderUnit>> BasicShader::fragmentShaderCache;

std::shared_ptr<ShaderUnit> BasicShader::getShaderUnit(Type type, ShaderType shaderType) {
    if (shaderType == ShaderType::VertexShader) {
        if (vertexShaderCache.count((int)type) == 0) {
            switch (type) {
                case Type::SolidColor:
                    vertexShaderCache[(int)type] = ShaderUnit::create(solidColor_vertexShaderSource, ShaderType::VertexShader);
                    break;
                case Type::VertexColor:
                    vertexShaderCache[(int)type] = ShaderUnit::create(vertexColor_vertexShaderSource, ShaderType::VertexShader);
                    break;
                case Type::SolidColorWithTexture:
                    vertexShaderCache[(int)type] = ShaderUnit::create(solidColorWithTexture_vertexShaderSource, ShaderType::VertexShader);
                    break;
                case Type::VertexColorWithTexture:
                    vertexShaderCache[(int)type] = ShaderUnit::create(vertexColorWithTexture_vertexShaderSource, ShaderType::VertexShader);
                    break;
                case Type::PointSprite:
                    vertexShaderCache[(int)type] = ShaderUnit::create(pointSprite_vertexShaderSource, ShaderType::VertexShader);
                    break;
            }
        }
        return vertexShaderCache[(int)type];
        
    } else {
        if (fragmentShaderCache.count((int)type) == 0) {
            switch (type) {
                case Type::SolidColor:
                    fragmentShaderCache[(int)type] = ShaderUnit::create(solidColor_fragmentShaderSource, ShaderType::FragmentShader);
                    break;
                case Type::VertexColor:
                    fragmentShaderCache[(int)type] = ShaderUnit::create(vertexColor_fragmentShaderSource, ShaderType::FragmentShader);
                    break;
                case Type::SolidColorWithTexture:
                    fragmentShaderCache[(int)type] = ShaderUnit::create(solidColorWithTexture_fragmentShaderSource, ShaderType::FragmentShader);
                    break;
                case Type::VertexColorWithTexture:
                    fragmentShaderCache[(int)type] = ShaderUnit::create(vertexColorWithTexture_fragmentShaderSource, ShaderType::FragmentShader);
                    break;
                case Type::PointSprite:
                    fragmentShaderCache[(int)type] = BasicShader::getShaderUnit(Type::SolidColor, ShaderType::FragmentShader);
                    break;
            }
        }
        return fragmentShaderCache[(int)type];
    }
}
