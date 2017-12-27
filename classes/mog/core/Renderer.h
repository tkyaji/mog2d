#ifndef Renderer_h
#define Renderer_h

#include <vector>
#include "mog/core/opengl.h"
#include "mog/core/Transform.h"
#include "mog/core/plain_objects.h"

#define VBO_VERTICES 0
#define VBO_INDICES 1
#define VBO_TEXTURES 2
#define VBO_COLORS 3

using namespace std;

namespace mog {
    class Engine;
    
    class Renderer {
    public:
        static float identityMatrix[16];

        int indicesNum = 0;
        int textureId = 0;
        float matrix[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };

        Renderer();
        ~Renderer();
        
        void bindVertex(float *vertices, int verticesSize, short *indices, int indicesSize, bool dynamicDraw = false);
        void bindTextureVertex(int textureId, float *vertexTexCoords, int size, bool dynamicDraw = false);
        void bindColorsVertex(float *vertexColors, int size, bool dynamicDraw = false);

        void bindVertexSub(float *vertices, int verticesNum, int offset = 0);
        void bindColorsVertexSub(float *vertexColors, int size, int offset = 0);

        void drawFrame(const shared_ptr<Transform> &transform, float screenScale);

        void applyTransform(const shared_ptr<Transform> &transform, float screenScale, bool enableColor = true);
        void getMatrix(float *matrix);
        void setMatrix(float *matrix);
        void pushMatrix();
        void popMatrix();
        void pushColor();
        void popColor();
        
    private:
        GLuint vertexBuffer[2] = {0, 0};
        GLuint vertexTexCoordsBuffer = 0;
        GLuint vertexColorsBuffer = 0;
        
        bool enableTexture = false;
        bool enableColor = false;
        float _color[4] = {1, 1, 1, 1};
        
        void drawFrame();
    };
}

#endif /* Renderer_hpp */
