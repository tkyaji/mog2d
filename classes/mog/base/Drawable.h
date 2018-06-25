#ifndef Drawable_h
#define Drawable_h

#include <memory>
#include "mog/core/Engine.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class DrawableGroup;
    
    class Drawable : public std::enable_shared_from_this<Drawable> {
        friend class Scene;
        friend class DrawableGroup;
    public:
        ~Drawable();

        void setAnchor(const Point &anchor);
        void setAnchor(float x, float y);
        void setAnchorX(float x);
        void setAnchorY(float y);
        Point getAnchor();
        float getAnchorX();
        float getAnchorY();
        void setPosition(const Point &position);
        void setPosition(float x, float y);
        void setPositionX(float x);
        void setPositionY(float y);
        Point getPosition();
        Point getPosition(const Point &anchor);
        float getPositionX();
        float getPositionY();
        void setScale(float scale);
        void setScale(float scaleX, float scaleY);
        void setScale(const Point &scale);
        void setScaleX(float scaleX);
        void setScaleY(float scaleY);
        Point getScale();
        float getScaleX();
        float getScaleY();
        void setRotation(float angle);
        float getRotation();
        void setColor(const Color &color);
        void setColor(float r, float g, float b, float a = 1.0f);
        void setColorR(float r);
        void setColorG(float g);
        void setColorB(float b);
        void setColorA(float a);
        Color getColor();
        void setSize(const Size &size);
        void setSize(float width, float height);
        Size getSize();
        void setWidth(float width);
        float getWidth();
        void setHeight(float height);
        float getHeight();
        void setZIndex(int zIndex);
        int getZIndex();
        void setVisible(bool visible);
        bool isVisible();
        void runTween(const std::shared_ptr<Tween> &tween);
        void cancelTween(unsigned int tweenId);
        void cancelAllTweens();
        void removeFromParent();

        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        void setParam(T param) {
            this->param = shared_ptr<T>(new T(param));
        }
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        T getParam() {
            return *(static_pointer_cast<T>(this->param).get());
        }

        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentReRenderFlag = 0);
        virtual void drawFrame(float delta);
        virtual void updateTween(float delta);

        std::shared_ptr<Renderer> renderer;

    protected:
        Drawable();
        virtual void bindVertex();

        std::weak_ptr<DrawableGroup> drawableGroup;
        int zIndex = 0;
        
        std::shared_ptr<Data> param;
        bool visible = true;
        unsigned char reRenderFlag = RERENDER_ALL;
        std::shared_ptr<Transform> transform;
        float matrix[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
        std::unordered_map<unsigned int, std::shared_ptr<Tween>> tweens;
        std::vector<unsigned int> tweenIdsToRemove;

        void init();
    };
}

#endif /* Drawable_h */
