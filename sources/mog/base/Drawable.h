#ifndef Drawable_h
#define Drawable_h

#include <memory>
#include <array>
#include "mog/core/Engine.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Texture2D.h"
#include "mog/core/TouchInput.h"
#include "mog/Constants.h"

extern void *enabler;

namespace mog {
    class DrawableContainer;
    class DrawableGroup;
    
    class Drawable : public std::enable_shared_from_this<Drawable> {
        friend class Scene;
        friend class DrawableContainer;
        friend class DrawableGroup;
        
    public:
        ~Drawable();
        
        virtual void setPivot(const Point &pivot);
        virtual void setPivot(float x, float y);
        virtual void setPivotX(float x);
        virtual void setPivotY(float y);
        virtual Point getPivot();
        virtual float getPivotX();
        virtual float getPivotY();
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
        virtual Point getPosition(const Point &pivot, const Point &anchor);
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
        virtual float getColorR();
        virtual float getColorG();
        virtual float getColorB();
        virtual float getColorA();
        virtual std::string getColorCode();
        virtual void setSize(const Size &size, unsigned char setInRatioFlag = SET_IN_FIXED_VALUE);
        virtual void setSize(float width, float height, unsigned char setInRatioFlag = SET_IN_FIXED_VALUE);
        virtual Size getSize();
        virtual Size getRealSize();
        virtual unsigned char getSizeSetInRatioFlag();
        virtual void setWidth(float width, bool setInRatio = false);
        virtual float getWidth();
        virtual float getRealWidth();
        virtual void setHeight(float height, bool setInRatio = false);
        virtual float getHeight();
        virtual float getRealHeight();
        virtual void setZIndex(int zIndex);
        virtual int getZIndex();
        virtual void setActive(bool active);
        virtual bool isActive();
        void runTween(const std::shared_ptr<Tween> &tween);
        void cancelTween(unsigned int tweenId);
        void cancelAllTweens();
        void removeFromParent();

        template <class T, typename std::enable_if<std::is_base_of<Drawable, T>::value>::type*& = enabler>
        std::shared_ptr<T> cast() {
            return std::static_pointer_cast<T>(shared_from_this());
        }
        
        virtual std::shared_ptr<Renderer> getRenderer();
        virtual std::shared_ptr<Transform> getTransform();
        virtual unsigned char getDirtyFlag(bool withParent = false);
        virtual float *getMatrix();
        virtual void updateMatrix();
        virtual void updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag);

        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        void setParam(const std::shared_ptr<T> &param) {
            this->param = param;
        }
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> getParam() {
            return std::static_pointer_cast<T>(this->param);
        }

        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag);
        virtual void drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches);
        virtual void updateTween(float delta);
        std::shared_ptr<Texture2D> getTexture(int textureIdx = 0);
        void setTexture(int textureIdx, const std::shared_ptr<Texture2D> &texture);

    protected:
        Drawable();
        virtual void bindVertex();
        virtual void updateTransform();
        virtual void onUpdate(float delta) {};

        std::shared_ptr<Renderer> renderer = nullptr;
        std::shared_ptr<Transform> transform = nullptr;
        Size size = Size::zero;
        Point anchor = Point::zero;
        unsigned char sizeSetInRatioFlag = SET_IN_FIXED_VALUE;
        std::array<std::shared_ptr<Texture2D>, MULTI_TEXTURE_NUM> textures;
        unsigned char dirtyFlag = (DIRTY_ALL | DIRTY_SIZE | DIRTY_ANCHOR);
        std::weak_ptr<Drawable> group;

        std::weak_ptr<DrawableContainer> parentDrawableContainer;
        int zIndex = 0;
        
        std::shared_ptr<Data> param;
        bool active = true;
        std::unordered_map<unsigned int, std::shared_ptr<Tween>> tweens;
        std::vector<unsigned int> tweenIdsToRemove;
        
        void init();
    };
}

#endif /* Drawable_h */
