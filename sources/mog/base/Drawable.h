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
        
        virtual void setAnchor(const Point &anchor);
        virtual void setAnchor(float x, float y);
        virtual void setAnchorX(float x);
        virtual void setAnchorY(float y);
        virtual Point getAnchor();
        virtual float getAnchorX();
        virtual float getAnchorY();
        virtual void setPosition(const Point &position);
        virtual void setPosition(float x, float y);
        virtual void setPositionX(float x);
        virtual void setPositionY(float y);
        virtual Point getPosition();
        virtual Point getPosition(const Point &anchor);
        virtual float getPositionX();
        virtual float getPositionY();
        virtual void setScale(float scale);
        virtual void setScale(float scaleX, float scaleY);
        virtual void setScale(const Point &scale);
        virtual void setScaleX(float scaleX);
        virtual void setScaleY(float scaleY);
        virtual Point getScale();
        virtual float getScaleX();
        virtual float getScaleY();
        virtual void setRotation(float angle);
        virtual float getRotation();
        virtual void setColor(const Color &color);
        virtual void setColor(float r, float g, float b, float a = 1.0f);
        virtual void setColorR(float r);
        virtual void setColorG(float g);
        virtual void setColorB(float b);
        virtual void setColorA(float a);
        virtual void setColor(std::string hexString);
        virtual Color getColor();
        virtual void setSize(const Size &size);
        virtual void setSize(float width, float height);
        virtual Size getSize();
        virtual void setWidth(float width);
        virtual float getWidth();
        virtual void setHeight(float height);
        virtual float getHeight();
        virtual void setZIndex(int zIndex);
        virtual int getZIndex();
        virtual void setActive(bool active);
        virtual bool isActive();
        void runTween(const std::shared_ptr<Tween> &tween);
        void cancelTween(unsigned int tweenId);
        void cancelAllTweens();
        void removeFromParent();
        
        virtual std::shared_ptr<Renderer> getRenderer();
        virtual std::shared_ptr<Transform> getTransform();
        virtual unsigned char getReRenderFlag();
        virtual float *getMatrix();
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        void setParam(const std::shared_ptr<T> &param) {
            this->param = param;
        }
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> getParam() {
            return static_pointer_cast<T>(this->param);
        }

        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentReRenderFlag = 0);
        virtual void drawFrame(float delta);
        virtual void updateTween(float delta);
        
    protected:
        Drawable();
        virtual void bindVertex();
        
        std::shared_ptr<Renderer> renderer;
        std::shared_ptr<Transform> transform;
        unsigned char reRenderFlag = RERENDER_ALL;
        
        std::weak_ptr<DrawableGroup> parentDrawableGroup;
        int zIndex = 0;
        
        std::shared_ptr<Data> param;
        bool active = true;
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
