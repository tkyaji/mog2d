#ifndef Screen_h
#define Screen_h

#include "mog/core/plain_objects.h"
#include <memory>
#include <vector>
#include <functional>

namespace mog {
    class Density {
    public:
        enum class Type {
            x1_0 = 0,
            x1_5,
            x2_0,
            x3_0,
            x4_0,
        };
        
        operator int() const {
            return (int)this->type;
        }
        
        static Density getDensity(float deviceDensity);
        Density(Type type);
        
        static const Density x1_0;
        static const Density x1_5;
        static const Density x2_0;
        static const Density x3_0;
        static const Density x4_0;
        static std::vector<Density> allDensities;
        
        Type type;
        float value;
        std::string directory;
    };

    
    class Screen {
    public:
        static std::shared_ptr<Screen> create();
        static Size getSize();
        static Size getDisplaySize();
        static Size getViewSize();
        static Density getDensity();
        static float getDeviceDensityValue();
        static float getScreenScale();
        static void setSize(float width, float height);

        void setDisplaySize(const Size &displaySize, const Size &viewSize, float deviceDensity);
        void setResizeCallback(std::function<void()> callback);
        
    private:
        static std::weak_ptr<Screen> instance;
        
        Screen() {}
        
        Density density = Density::x1_0;
        std::function<void()> resizeCallback;
        Size screenSize;
        Size displaySize;
        Size viewSize;
        float deviceDensity;
        
        void resetScreenSize();
        void invokeCallback();
        void setScreenSize(float width, float height);
    };
}

#endif /* Screen_h */
