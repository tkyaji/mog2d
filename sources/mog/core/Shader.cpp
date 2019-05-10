#include "mog/core/Shader.h"
#include "mog/core/opengl.h"
#include "mog/core/FileUtils.h"
#include "mog/Constants.h"

using namespace mog;

inline static GLuint compileShader(ShaderType shaderType, const GLchar *source) {
    GLuint glShaderType = 0;
    if (shaderType == ShaderType::VertexShader) {
        glShaderType = GL_VERTEX_SHADER;
    } else if (shaderType == ShaderType::FragmentShader) {
        glShaderType = GL_FRAGMENT_SHADER;
    }
    GLuint shader = glCreateShader(glShaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
#ifdef MOG_DEBUG
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLsizei bufsize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufsize);
        GLsizei length = 0;
        GLchar *infolog = (GLchar *)mogmalloc(sizeof(GLchar) * bufsize);
        glGetShaderInfoLog(shader, bufsize, &length, infolog);
        LOGE("Shader compile error: %s", (const char *)infolog);
        mogfree(infolog);
    }
#endif
    
    return shader;
}

std::shared_ptr<Shader> Shader::create(const GLchar *shaderSource, ShaderType shaderType) {
    auto shader = std::shared_ptr<Shader>(new Shader());
    shader->init(shaderSource, shaderType);
    return shader;
}

std::shared_ptr<Shader> Shader::createWithAsset(std::string filename, ShaderType shaderType) {
    std::string shaderSource = FileUtils::readTextAsset(filename);
    auto shader = std::shared_ptr<Shader>(new Shader());
    shader->init(shaderSource.c_str(), shaderType);
    return shader;
}

void Shader::init(const GLchar *shaderSource, ShaderType shaderType) {
    this->glShader = compileShader(shaderType, shaderSource);
    this->shaderType = shaderType;
}

Shader::Shader() {
}

Shader::~Shader() {
    glDeleteShader(this->glShader);
}


static const GLchar *solidColor_vertexShaderSource = "\
attribute highp vec2 a_position;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
void main() {\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *solidColor_fragmentShaderSource = "\
uniform mediump vec4 u_color;\
void main() {\
    gl_FragColor = u_color;\
}\
";

static const GLchar *vertexColor_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute mediump vec4 a_color;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
uniform mediump vec4 u_color;\
varying mediump vec4 v_color;\
void main() {\
    v_color = a_color * u_color;\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *vertexColor_fragmentShaderSource = "\
varying mediump vec4 v_color;\
void main() {\
    gl_FragColor = v_color;\
}\
";

static const GLchar *solidColorWithTexture_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute highp vec2 a_uv0;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
varying highp vec2 v_uv0;\
void main() {\
    v_uv0 = a_uv0;\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *solidColorWithTexture_fragmentShaderSource = "\
uniform sampler2D u_texture0;\
uniform mediump vec4 u_color;\
varying highp vec2 v_uv0;\
void main() {\
    gl_FragColor = texture2D(u_texture0, v_uv0) * u_color;\
}\
";

static const GLchar *vertexColorWithTexture_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute highp vec2 a_uv0;\
attribute mediump vec4 a_color;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
uniform mediump vec4 u_color;\
varying highp vec2 v_uv0;\
varying mediump vec4 v_color;\
void main() {\
    v_uv0 = a_uv0;\
    v_color = a_color * u_color;\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
}\
";
static const GLchar *vertexColorWithTexture_fragmentShaderSource = "\
uniform sampler2D u_texture0;\
varying highp vec2 v_uv0;\
varying mediump vec4 v_color;\
void main() {\
    if (v_uv0.x < -0.1) {\
        gl_FragColor = v_color;\
    } else {\
        gl_FragColor = texture2D(u_texture0, v_uv0) * v_color;\
    }\
}\
";

static const GLchar *pointSprite_vertexShaderSource = "\
attribute highp vec2 a_position;\
attribute mediump vec4 a_color;\
uniform highp mat4 u_matrix;\
uniform highp vec2 u_screenSize;\
uniform highp float u_pointSize;\
void main() {\
    highp vec4 pos = u_matrix * vec4(a_position, 0.0, 1.0);\
    gl_Position = vec4(pos.x / u_screenSize.x * 2.0 - 1.0, 1.0 - pos.y / u_screenSize.y * 2.0, 0.0, 1.0);\
    gl_PointSize = u_pointSize;\
}\
";

std::unordered_map<unsigned int, std::shared_ptr<Shader>> BasicShader::shaderCache;

std::shared_ptr<Shader> BasicShader::getShader(Type type, ShaderType shaderType) {
    unsigned int cacheKey = (int)type * 1000 + (int)shaderType;
    if (shaderCache.count(cacheKey) == 0) {
        switch (type) {
            case Type::SolidColor:
                if (shaderType == ShaderType::VertexShader) {
                    shaderCache[cacheKey] = Shader::create(solidColor_vertexShaderSource, shaderType);
                } else {
                    shaderCache[cacheKey] = Shader::create(solidColor_fragmentShaderSource, shaderType);
                }
                break;
            case Type::VertexColor:
                if (shaderType == ShaderType::VertexShader) {
                    shaderCache[cacheKey] = Shader::create(vertexColor_vertexShaderSource, shaderType);
                } else {
                    shaderCache[cacheKey] = Shader::create(vertexColor_fragmentShaderSource, shaderType);
                }
                break;
            case Type::SolidColorWithTexture:
                if (shaderType == ShaderType::VertexShader) {
                    shaderCache[cacheKey] = Shader::create(solidColorWithTexture_vertexShaderSource, shaderType);
                } else {
                    shaderCache[cacheKey] = Shader::create(solidColorWithTexture_fragmentShaderSource, shaderType);
                }
                break;
            case Type::VertexColorWithTexture:
                if (shaderType == ShaderType::VertexShader) {
                    shaderCache[cacheKey] = Shader::create(vertexColorWithTexture_vertexShaderSource, shaderType);
                } else {
                    shaderCache[cacheKey] = Shader::create(vertexColorWithTexture_fragmentShaderSource, shaderType);
                }
                break;
            case Type::PointSprite:
                if (shaderType == ShaderType::VertexShader) {
                    shaderCache[cacheKey] = Shader::create(pointSprite_vertexShaderSource, shaderType);
                } else {
                    shaderCache[cacheKey] = getShader(Type::SolidColor, shaderType);
                }
                break;
        }
    }
    return shaderCache[cacheKey];
}

void BasicShader::clearShaderCache() {
    shaderCache.clear();
}
