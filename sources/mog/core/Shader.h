#ifndef Shader_h
#define Shader_h

#include "mog/core/opengl.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace mog {
    enum class ShaderType {
        VertexShader,
        FragmentShader,
    };
    
    class Shader {
    public:
        static std::shared_ptr<Shader> create(const GLchar *shaderSource, ShaderType shaderType);
        static std::shared_ptr<Shader> createWithAsset(std::string filename, ShaderType shaderType);
        ~Shader();
        
        GLuint glShader;
        ShaderType shaderType;
        
    protected:
        Shader();
        void init(const GLchar *shaderSource, ShaderType shaderType);
    };
    
    
    class BasicShader {
    public:
        enum class Type {
            SolidColor,
            VertexColor,
            SolidColorWithTexture,
            VertexColorWithTexture,
            PointSprite,
        };
        
        static std::shared_ptr<Shader> getShader(Type type, ShaderType shaderType);
        
    private:
        static std::unordered_map<unsigned int, std::shared_ptr<Shader>> shaderCache;
    };
}

#endif /* Shader_h */
