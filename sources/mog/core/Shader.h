#ifndef Shader_h
#define Shader_h

#include "mog/core/opengl.h"
#include <string>
#include <memory>
#include <unordered_map>

#define ATTR_LOCATION_IDX_POSITION 0
#define ATTR_LOCATION_IDX_COLOR 1
#define ATTR_LOCATION_IDX_UV0 2
#define ATTR_LOCATION_IDX_UV1 3
#define ATTR_LOCATION_IDX_UV2 4
#define ATTR_LOCATION_IDX_UV3 5
#define ATTR_LOCATION_IDX_UV_START 2
#define ATTR_LOCATION_IDX_USER_START 6

namespace mog {
    enum class ShaderType {
        VertexShader,
        FragmentShader,
    };

    
#pragma - ShaderUnit

    class ShaderUnit {
    public:
        static void releaseAllBufferes();
        static std::shared_ptr<ShaderUnit> create(const GLchar *shaderSource, ShaderType shaderType);
        static std::shared_ptr<ShaderUnit> createWithAsset(std::string vertexShaderSourceFilename, ShaderType shaderType);
        ~ShaderUnit();
        void compileIfNeed();
        void releaseBuffer();
        
        ShaderType shaderType;
        GLchar *shaderSource = nullptr;
        
        GLuint glShader = 0;
        
    private:
        static std::unordered_map<intptr_t, std::weak_ptr<ShaderUnit>> allShaderUnits;
        ShaderUnit() {}
    };
        
    
#pragma - Shader

    class Shader {
        friend class Renderer;
    private:
        class UniformParameter {
        public:
            enum class Type {
                Float1,
                Float2,
                Float3,
                Float4,
                Int1,
                Int2,
                Int3,
                Int4,
                Matrix2,
                Matrix3,
                Matrix4,
            };
            
            Type type = Type::Float1;
            int i[4] = {0, 0, 0, 0};
            float f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            float matrix[16] = {
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
            };
            
            UniformParameter();
            UniformParameter(float f1);
            UniformParameter(float f1, float f2);
            UniformParameter(float f1, float f2, float f3);
            UniformParameter(float f1, float f2, float f3, float f4);
            
            UniformParameter(int i1);
            UniformParameter(int i1, int i2);
            UniformParameter(int i1, int i2, int i3);
            UniformParameter(int i1, int i2, int i3, int i4);
            
            UniformParameter(const float *matrix, int size = 4);
            
            void setUniform(GLuint program, std::string name);
        };
        
        
        class VertexAttributeParameter {
        public:
            class Data {
            public:
                void *values = nullptr;
                size_t valueSize = 0;
                
                Data(void *values, size_t valueSize);
                ~Data();
            };
            
            enum class Type {
                Float1,
                Float2,
                Float3,
                Float4,
                VertexPointer,
            };
            
            Type type = Type::Float1;
            int f[4] = {0, 0, 0, 0};
            GLenum glType = GL_FLOAT;
            int size = 0;
            std::shared_ptr<Data> data = nullptr;
            bool dynamicDraw = true;
            bool normalized = false;
            int stride = 0;
            bool dirty = false;

            VertexAttributeParameter();
            VertexAttributeParameter(float f1);
            VertexAttributeParameter(float f1, float f2);
            VertexAttributeParameter(float f1, float f2, float f3);
            VertexAttributeParameter(float f1, float f2, float f3, float f4);
            VertexAttributeParameter(GLenum glType, void *values, size_t valueSize, int size, bool dynamicDraw, bool normalized, int stride);

            void setVertexAttribute(unsigned int location, int bufferIndex = 0);
        };
        
        
    public:
        static std::shared_ptr<Shader> create();
        
        /*
        static std::shared_ptr<Shader> create(const GLchar *vertexShaderSource, const GLchar *fragmentShaderSource);
        static std::shared_ptr<Shader> create(const std::shared_ptr<ShaderUnit> &vertexShader, const std::shared_ptr<ShaderUnit> &fragmentShader);
        static std::shared_ptr<Shader> createWithAsset(std::string vertexShaderSourceFilename, std::string fragmentShaderSourceFilename);
         */
        
//        static void releaseAllBufferes();

        ~Shader();
        
        void compileIfNeed();

        void setParameters();
        
        void setLineWidth(float width);
        void setUniformPointSize(float size);
        void setUniformMatrix(const float *matrix);
        void setUniformColor(float r, float g, float b, float a);
        
        void setUniformParameter(std::string name, float f1);
        void setUniformParameter(std::string name, float f1, float f2);
        void setUniformParameter(std::string name, float f1, float f2, float f3);
        void setUniformParameter(std::string name, float f1, float f2, float f3, float f4);
        void setUniformParameter(std::string name, int i1);
        void setUniformParameter(std::string name, int i1, int i2);
        void setUniformParameter(std::string name, int i1, int i2, int i3);
        void setUniformParameter(std::string name, int i1, int i2, int i3, int i4);
        void setUniformParameter(std::string name, const float *matrix, int size = 4);
        
        void setVertexAttributeParameter(std::string name, float f1);
        void setVertexAttributeParameter(std::string name, float f1, float f2);
        void setVertexAttributeParameter(std::string name, float f1, float f2, float f3);
        void setVertexAttributeParameter(std::string name, float f1, float f2, float f3, float f4);
        void setVertexAttributeParameter(std::string name, float *values, int arrLength, int size, bool dynamicDraw = false, bool normalized = false, int stride = 0);
        void setVertexAttributeParameter(std::string name, int *values, int arrLength, int size, bool dynamicDraw = false, bool normalized = false, int stride = 0);
        void setVertexAttributeParameter(std::string name, short *values, int arrLength, int size, bool dynamicDraw = false, bool normalized = false, int stride = 0);
        
        void bindVertexAttributeParameter(unsigned int location, float *values, int arrLength, int size, bool dynamicDraw = false, bool normalized = false, int stride = 0);
        void bindVertexAttributePointerSub(unsigned int location, float *value, int arrLength, int offset);
        
        unsigned int bindAttributeLocation(std::string name);
        void bindAttributeLocation(std::string name, unsigned int location);

        float getMaxLineWidth();
        float getMaxPointSize();
        void releaseBuffer();
        
        void attachVertexShader(const std::shared_ptr<ShaderUnit> &vertexShader);
        void attachFragmentShader(const std::shared_ptr<ShaderUnit> &fragmentShader);

    private:
//        static std::unordered_map<intptr_t, std::weak_ptr<Shader>> allShaders;

        std::shared_ptr<ShaderUnit> vertexShader = nullptr;
        std::shared_ptr<ShaderUnit> fragmentShader = nullptr;
        GLuint glShaderProgram = 0;

        std::unordered_map<unsigned int, unsigned int> bufferIndexMap;
        std::unordered_map<std::string, UniformParameter> uniformParamsMap;
        std::unordered_map<std::string, bool> dirtyUniformParamsMap;
        std::unordered_map<unsigned int, VertexAttributeParameter> vertexAttributeParamsMap;
        std::unordered_map<std::string, unsigned int> attributeLocationMap;
        unsigned int attributeLocationIndexCounter = ATTR_LOCATION_IDX_USER_START;

        Shader() {}
        void setUniformParameter(std::string name, const UniformParameter &param);
        void setVertexAttributeParameter(std::string name, const VertexAttributeParameter &param);
        unsigned int getBufferIndex(unsigned int location);
    };
    
    
#pragma - BasicShader

    class BasicShader {
    public:
        enum class Type {
            SolidColor,
            VertexColor,
            SolidColorWithTexture,
            VertexColorWithTexture,
            PointSprite,
        };
        
//        static std::shared_ptr<Shader> getShader(Type type);
        static std::shared_ptr<ShaderUnit> getShaderUnit(Type type, ShaderType shaderType);
        static void clearShaderCache();
        
    private:
        static std::unordered_map<int, std::shared_ptr<ShaderUnit>> vertexShaderCache;
        static std::unordered_map<int, std::shared_ptr<ShaderUnit>> fragmentShaderCache;
    };
}

#endif /* Shader_h */
