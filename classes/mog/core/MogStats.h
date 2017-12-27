#ifndef MogStats_h
#define MogStats_h

#include <memory>
#include <unordered_map>
#include "Renderer.h"
#include "Transform.h"
#include "Texture2D.h"

using namespace std;

namespace mog {
    class Engine;
    enum class Alignment;
    
    class MogStats {
    public:
        static int drawCallCount;
        static int instanceCount;

        static shared_ptr<MogStats> create(bool enable);
        void drawFrame(const shared_ptr<Engine> &engine, float delta);
        bool isEnabled();
        void setEnable(bool enable);
        void setAlignment(Alignment alignment);

    private:
        shared_ptr<Renderer> renderer;
        shared_ptr<Transform> transform;
        shared_ptr<Texture2D> texture;
        unsigned char *data;
        weak_ptr<Engine> engine;
        int width = 0;
        int height = 0;
        bool enable = false;
        bool initialized = false;
        bool dirtyPosition = false;
        float tmpDelta = 0;
        float screenScale = 1.0f;
        Size screenSize = Size::zero;
        Alignment alignment;
        unordered_map<char, shared_ptr<Texture2D>> numberTexture2ds;

        void bindVertex();
        void init();
        void updatePosition();
        void setTextToData(shared_ptr<Texture2D> text, int x, int y);
        void setNumberToData(float value, int intLength, int decimalLength, int x, int y);
        shared_ptr<Texture2D> createLabelTexture(string text);
        void updateValues(float delta);
        unordered_map<int, pair<int, int>> positions;
    };
}

#endif /* MogStats_h */
