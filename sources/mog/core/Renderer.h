#ifndef Renderer_h
#define Renderer_h

#include <vector>
#include <string>
#include <unordered_map>
#include "mog/core/opengl.h"
#include "mog/core/Shader.h"
#include "mog/core/plain_objects.h"

#define VBO_VERTICES 0
#define VBO_INDICES 1
#define VBO_TEXTURES 2
#define VBO_COLORS 3

namespace mog {
    class Engine;
    
    enum class DrawType {
        TrinangleStrip  = GL_TRIANGLE_STRIP,
        TriangleFan     = GL_TRIANGLE_FAN,
        Triangles       = GL_TRIANGLES,
        LineStrip       = GL_LINE_STRIP,
        LineLoop        = GL_LINE_LOOP,
        Lines           = GL_LINES,
        Points          = GL_POINTS,
    };
    
    enum class BlendingFactor {
        Zero                    = GL_ZERO,
        One                     = GL_ONE,
        SrcColor                = GL_SRC_COLOR,
        OneMinusSrcColor        = GL_ONE_MINUS_SRC_COLOR,
        DstColor                = GL_DST_COLOR,
        OneMinusDstColor        = GL_ONE_MINUS_DST_COLOR,
        SrcAlpha                = GL_SRC_ALPHA,
        OneMinusSrcAlpha        = GL_ONE_MINUS_SRC_ALPHA,
        DstAlpha                = GL_DST_ALPHA,
        OneMinusDstAlpha        = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor           = GL_CONSTANT_COLOR,
        OneMinusConstantColor   = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha           = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha   = GL_ONE_MINUS_CONSTANT_ALPHA,
        SrcAlphaSaturate        = GL_SRC_ALPHA_SATURATE,
    };

    
    class Renderer {
    public:
        static float identityMatrix[20];
        static std::unordered_map<unsigned long long, std::weak_ptr<Renderer>> allRenderers;
        static unsigned long long rendererIdCounter;
        
        static void releaseAllBuffer();
        static std::shared_ptr<Renderer> create();

        unsigned long long rendererId = 0;
        int textureId = 0;
        int verticesNum = 0;
        int indicesNum = 0;
        float *vertices = nullptr;
        short *indices = nullptr;
        float *vertexColors = nullptr;
        float *vertexTexCoords[4] = {
            nullptr, nullptr, nullptr, nullptr,
        };
        float matrix[20] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 1, 1, 1,
        };

        ~Renderer();
        
        void setDrawType(DrawType drawType);
        
        void initScreenParameters(const std::shared_ptr<Engine> &engine);
        void setBlendFunc(BlendingFactor blendingFactorSrc, BlendingFactor blendingFactorDest);
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

        unsigned int bindAttributeLocation(std::string name);
        void setVertexAttributeParameter(unsigned int location, float f1);
        void setVertexAttributeParameter(unsigned int location, float f1, float f2);
        void setVertexAttributeParameter(unsigned int location, float f1, float f2, float f3);
        void setVertexAttributeParameter(unsigned int location, float f1, float f2, float f3, float f4);
        void setVertexAttributeParameter(unsigned int location, float *values, int arrSize, int size, bool dynamicDraw = false, bool normalized = false, int stride = 0);
        void setVertexAttributeParameter(unsigned int location, int *values, int arrSize, int size, bool dynamicDraw = false, bool normalized = false, int stride = 0);
        void setVertexAttributeParameter(unsigned int location, short *values, int arrSize, int size, bool dynamicDraw = false, bool normalized = false, int stride = 0);
        void bindVertexAttributePointerSub(unsigned int location, float *value, int arrSize, int offset);
        
        float getMaxLineWidth();
        float getMaxPointSize();
        
        void bindVertex(bool dynamicDraw = false);
        void bindVertexTexCoords(int textureId, int textureIdx, bool dynamicDraw = false);
        void bindVertexColors(bool dynamicDraw = false);

        void bindVertexSub(int index, int size);
        void bindVertexTexCoordsSub(int index, int size, int textureIdx = 0);
        void bindVertexColorsSub(int index, int size);

        bool setVerticesNum(int verticesNum);
        bool setIndicesNum(int indicesNum);
        void newVerticesArr();
        void newIndicesArr();
        void newVertexColorsArr();
        void newVertexTexCoordsArr(int textureIdx = 0);
        void releaseBuffer();
        
        void drawFrame();
        
        std::shared_ptr<Shader> vertexShader = nullptr;
        std::shared_ptr<Shader> fragmentShader = nullptr;
        
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
            enum class Type {
                Float1,
                Float2,
                Float3,
                Float4,
                VertexPointer,
            };
            
            Type type = Type::Float1;
            int f[4] = {0, 0, 0, 0};
            int index = 0;
            GLenum glType = GL_FLOAT;
            int size = 0;
            bool normalized = false;
            int stride = 0;
            
            VertexAttributeParameter();
            VertexAttributeParameter(float f1);
            VertexAttributeParameter(float f1, float f2);
            VertexAttributeParameter(float f1, float f2, float f3);
            VertexAttributeParameter(float f1, float f2, float f3, float f4);
            VertexAttributeParameter(int index, GLenum glType, int size, bool normalized, int stride);
            
            void setVertexAttribute(unsigned int location);
        };
        
        GLuint vertexBuffer[2] = {0, 0};
        GLuint glShaderProgram = 0;

        Renderer() {}
        std::unordered_map<std::string, UniformParameter> uniformParamsMap;
        std::unordered_map<std::string, bool> dirtyUniformParamsMap;
        
        std::unordered_map<std::string, unsigned int> attributeLocationMap;
        std::unordered_map<unsigned int, unsigned int> bufferIndexMap;
        std::unordered_map<unsigned int, VertexAttributeParameter> vertexAttributeParamsMap;
        unsigned int attributeLocationIndexCounter = 1;
        bool screenParameterInitialized = false;
        bool enableVertexColor = false;

        DrawType drawType = DrawType::TrinangleStrip;
        BlendingFactor blendingFactorSrc = BlendingFactor::SrcAlpha;
        BlendingFactor blendingFactorDest = BlendingFactor::OneMinusSrcAlpha;

        void initShaderProgram();
        void setUniformParameter(std::string name, const UniformParameter &param);
        void setVertexAttributeParameter(unsigned int location, const VertexAttributeParameter &param);
        unsigned int getBufferIndex(unsigned int location);
        std::shared_ptr<Shader> getDefaultShader(ShaderType shaderType);
    };
}

#endif /* Renderer_hpp */
