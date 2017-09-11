#ifndef Entity_h
#define Entity_h

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "mog/core/Renderer.h"
#include "mog/core/Collision.h"
#include "mog/core/plain_objects.h"
#include "mog/core/Touch.h"
#include "mog/core/TouchEventListener.h"
#include "mog/core/Texture2D.h"
#include "mog/core/TextureAtlas.h"
#include "mog/Constants.h"

namespace mog {
    class Scene;
    class Group;
    class TouchEventListener;
    class Tween;
    enum class Easing;
    
    class Entity : public enable_shared_from_this<Entity> {
    public:
        static int instanceNumCounter;

        ~Entity();

        virtual void updateFrame(float delta);
        virtual void drawFrame(float delta);
        virtual void updateMatrix();
        virtual bool contains(const Point &point);
        virtual bool collidesWith(const shared_ptr<Entity> &other);

        unsigned int getEntityId();
        shared_ptr<Group> getGroup();
        void setGroup(shared_ptr<Group> group);
        shared_ptr<Renderer> getRenderer();
        void show();
        void hide();
        void removeFromParent();
        void setAnchor(const Point &position);
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
        float getPositionX();
        float getPositionY();
        void setScale(float scale);
        void setScale(const Point &scale);
        void setScaleX(float scaleX);
        void setScaleY(float scaleY);
        Point getScale();
        float getScaleX();
        float getScaleY();
        void setRotation(float angle);
        float getRotation();
        void setWidth(float width);
        float getWidth();
        void setHeight(float height);
        float getHeight();
        void setSize(const Size &size);
        void setSize(float width, float height);
        Size getSize();
        void setColor(const Color &color);
        void setColor(float r, float g, float b, float a = 1.0f);
        void setColorR(float r);
        void setColorG(float g);
        void setColorB(float b);
        void setColorA(float a);
        Color getColor();
        void setZIndex(int zIndex);
        int getZIndex();
        Point getAbsolutePosition();
        Point getAbsoluteScale();
        Point getAbsoluteSize();
        string getTag();
        void setTag(string tag);
        virtual shared_ptr<Texture2D> getTexture();
        virtual shared_ptr<Entity> cloneEntity();
        
        unsigned int addTouchEvent(const shared_ptr<TouchEventListener> &listener);
        shared_ptr<TouchEventListener> getTouchEvent(unsigned int eventId);
        void removeTouchEvent(unsigned int eventId);
        void removeAllTouchEvents();
        void fireTouchBeginEvent(const Touch &touch);
        void fireTouchMoveEvent(const Touch &touch);
        void fireTouchEndEvent(const Touch &touch);
        void setSwallowTouches(bool swallowTouches);
        bool isSwallowTouches();
        void setTouchEnable(bool enable);
        bool isTouchEnable();

        void runTween(const shared_ptr<Tween> &tween);
        void cancelTween(unsigned int tweenId);
        void cancelAllTweens();
        
        shared_ptr<Group> findParentGroup(string tag);
        
        virtual void getVerticesNum(int *num);
        virtual void getIndiciesNum(int *num);
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false);
        virtual void bindIndices(short *indices, int *idx, int start);
        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h);
        virtual void bindVertexColors(float *vertexColors, int *idx, const Color &parentColor = Color::white);
        virtual void bindVertexSub();

        virtual void addTextureTo(const shared_ptr<TextureAtlas> &textureAtlas);
        virtual void setReRenderFlag(unsigned char flag);
        virtual void setReRenderFlagToChild(unsigned char flag);

    protected:
        // field
        static unsigned int entityIdCounter;
        
        unsigned int entityId = 0;
        string tag = "";
        unsigned char reRenderFlag = RERENDER_ALL;
        weak_ptr<Group> group;
        shared_ptr<Renderer> renderer;
        unsigned int eventIdCounter = 0;
        ColliderShape colliderShape;
        shared_ptr<Transform> transform;
        long uptime = 0;
        int zIndex = 0;
        bool visible = true;
        bool touchEnable = true;
        bool swallowTouches = true;
        bool dynamicDraw = false;
        unordered_map<unsigned int, shared_ptr<TouchEventListener>> touchListeners;
        unordered_map<unsigned int, shared_ptr<Tween>> tweens;
        vector<unsigned int> tweenIdsToRemove;
        shared_ptr<Texture2D> texture;
        
        // constructor
        Entity();
        
        // method
        virtual shared_ptr<Collider> getCollider();
        virtual shared_ptr<OBB> getOBB();
        virtual shared_ptr<AABB> getAABB();
        virtual void extractEvent(float delta);
        virtual void bindVertex();
        virtual void onUpdate(float delta);
        virtual void copyFrom(const shared_ptr<Entity> &src);
    };
}

#endif /* Entity_h */
