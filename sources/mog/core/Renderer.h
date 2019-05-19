#ifndef Renderer_h
#define Renderer_h

#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include "mog/Constants.h"
#include "mog/core/opengl.h"
#include "mog/core/Shader.h"
#include "mog/core/Texture2D.h"
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
        
        static void releaseAllBufferes();
        static std::shared_ptr<Renderer> create();

        unsigned long long rendererId = 0;
        std::array<std::weak_ptr<Texture2D>, MULTI_TEXTURE_NUM> textures;
        int verticesNum = 0;
        int indicesNum = 0;
        float *vertices = nullptr;
        short *indices = nullptr;
        float *vertexColors = nullptr;
        float *vertexTexCoords[MULTI_TEXTURE_NUM];
        float matrix[20] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 1, 1, 1,
        };
        std::shared_ptr<Shader> shader = nullptr;

        ~Renderer();
        
        void setDrawType(DrawType drawType);
        
        void initScreenParameters();
        void setBlendFunc(BlendingFactor blendingFactorSrc, BlendingFactor blendingFactorDest);
        
        void bindVertex(bool dynamicDraw = false);
        void bindVertexTexCoords(const std::shared_ptr<Texture2D> &texture, int textureIdx, bool dynamicDraw = false);
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
        
        void drawFrame();
        
    private:
        static std::unordered_map<intptr_t, std::weak_ptr<Renderer>> allRenderers;

        GLuint vertexBuffer[2] = {0, 0};

        Renderer();

        /*
        std::unordered_map<unsigned int, unsigned int> bufferIndexMap;
        std::unordered_map<std::string, UniformParameter> uniformParamsMap;
        std::unordered_map<std::string, bool> dirtyUniformParamsMap;
        std::unordered_map<unsigned int, VertexAttributeParameter> vertexAttributeParamsMap;
         */
        bool screenParameterInitialized = false;
        bool enableVertexColor = false;

        DrawType drawType = DrawType::TrinangleStrip;
        BlendingFactor blendingFactorSrc = BlendingFactor::SrcAlpha;
        BlendingFactor blendingFactorDest = BlendingFactor::OneMinusSrcAlpha;

        /*
        void setUniformParameter(std::string name, const UniformParameter &param);
        void setVertexAttributeParameter(unsigned int location, const VertexAttributeParameter &param);
        unsigned int getBufferIndex(unsigned int location);
         */
        std::shared_ptr<ShaderUnit> getDefaultShader(ShaderType shaderType);
        void releaseBuffer();
    };
}

#endif /* Renderer_hpp */
