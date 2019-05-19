#ifndef MogStats_h
#define MogStats_h

#include <memory>
#include <unordered_map>
#include "Renderer.h"
#include "Transform.h"
#include "Texture2D.h"

namespace mog {
    class Engine;
    enum class Alignment;
    
    class MogStats {
    public:
        static int drawCallCount;
        static int instanceCount;

        static std::shared_ptr<MogStats> create(bool enable);
        void drawFrame(float delta, unsigned char parentReRenderFlag = 0);
        bool isEnabled();
        void setEnable(bool enable);
        void setAlignment(Alignment alignment);

    private:
        std::shared_ptr<Renderer> renderer;
        std::shared_ptr<Transform> transform;
        std::shared_ptr<Texture2D> texture;
        unsigned char *data;
        std::weak_ptr<Engine> engine;
        int width = 0;
        int height = 0;
        bool enable = false;
        bool initialized = false;
        bool dirtyPosition = false;
        float tmpDelta = 0;
        Alignment alignment;
        std::unordered_map<char, std::shared_ptr<Texture2D>> numberTexture2ds;

        void bindVertex();
        void init();
        void updatePosition();
        void setTextToData(std::shared_ptr<Texture2D> text, int x, int y);
        void setNumberToData(float value, int intLength, int decimalLength, int x, int y);
        std::shared_ptr<Texture2D> createLabelTexture(std::string text);
        void updateValues(float delta);
        std::unordered_map<int, std::pair<int, int>> positions;
    };
}

#endif /* MogStats_h */
