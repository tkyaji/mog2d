#ifndef MogStats_h
#define MogStats_h

#include <memory>
#include <unordered_map>
#include "Renderer.h"
#include "Transform.h"
#include "Texture2D.h"

using namespace std;

namespace mog {
    enum class Alignment;
    
    class MogStats {
    public:
        static shared_ptr<MogStats> create(bool enable);
        void drawFrame(float delta);
        bool isEnabled();
        void setEnable(bool enable);
        void setAlignment(Alignment alignment);

    private:
        shared_ptr<Renderer> renderer;
        shared_ptr<Transform> transform;
        shared_ptr<Texture2D> texture;
        unsigned char *data;
        int width = 0;
        int height = 0;
        bool enable = 0;
        bool initialized = false;
        float tmpDelta = 0;
        Alignment alignment;
        unordered_map<char, shared_ptr<Texture2D>> numberTexture2ds;

        void bindVertex();
        void init();
        void setTextToData(shared_ptr<Texture2D> text, int x, int y);
        void setNumberToData(float value, int intLength, int decimalLength, int x, int y);
        shared_ptr<Texture2D> createLabelTexture(string text);
        void updateValues(float delta);
        unordered_map<int, pair<int, int>> positions;
    };
}

#endif /* MogStats_h */
