#ifndef Entity_h
#define Entity_h

#include <vector>
#include <unordered_map>
#include "mog/base/Drawable.h"
#include "mog/core/Collision.h"

extern void *enabler;

namespace mog {
    class Group;
    class TouchEventListener;
    class Tween;
    class Collider;
    
    enum class EntityType {
        Rectangle,
        RoundedRectangle,
        Circle,
        Label,
        Line,
        Sprite,
        SpriteSheet,
        Slice9Sprite,
        TiledSprite,
        Group,
        ScrollGroup,
        VerticalGroup,
        HorizontalGroup,
    };

    class Entity : public Drawable {
        friend class Group;
        friend class EntityCreator;
    public:
        std::string getName();
        void setName(std::string name);
        std::string getTag();
        void setTag(std::string tag);
        std::shared_ptr<Group> getGroup();
        virtual Point getAbsolutePosition();
        virtual Size getAbsoluteSize();
        virtual Point getAbsoluteScale();

        virtual bool contains(const Point &point);
        virtual bool collidesWith(const std::shared_ptr<Entity> &other);
        unsigned int addTouchEvent(const std::shared_ptr<TouchEventListener> &listener);
        std::shared_ptr<TouchEventListener> getTouchEvent(unsigned int eventId);
        void removeTouchEvent(unsigned int eventId);
        void removeAllTouchEvents();
        void fireTouchBeginEvent(const Touch &touch);
        void fireTouchMoveEvent(const Touch &touch);
        void fireTouchEndEvent(const Touch &touch);
        void setSwallowTouches(bool swallowTouches);
        bool isSwallowTouches();
        void setTouchEnable(bool enable);
        bool isTouchEnable();
        virtual float *getMatrix() override;
        virtual std::shared_ptr<Collider> getCollider();
        virtual std::shared_ptr<Dictionary> serialize();
        
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) override;
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag);
        virtual void updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) override;

    protected:
        Entity() {}
        
        virtual void init() = 0;
        virtual void extractEvent(const std::shared_ptr<Engine> &engine, float delta);
        virtual void bindVertex() override;
        virtual void bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform);
        virtual void bindVertexColors(const std::shared_ptr<Renderer> &renderer, int *idx);
        virtual void bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h);
        virtual void copyProperties(const std::shared_ptr<Entity> &entity);
        virtual std::shared_ptr<Entity> cloneEntity() = 0;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params);
        virtual bool isGroup();
        void initRendererVertices(int verticesNum, int indicesNum);

        virtual std::shared_ptr<OBB> getOBB();
        virtual std::shared_ptr<AABB> getAABB();

        std::string name;
        std::string tag;
        std::shared_ptr<Collider> collider = nullptr;
        unsigned int eventIdCounter = 0;
        bool touchEnable = true;
        bool swallowTouches = false;
        std::unordered_map<unsigned int, std::shared_ptr<TouchEventListener>> touchListeners;
        float matrix[20] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 1, 1, 1,
        };

        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> getPropertyData(const std::shared_ptr<Dictionary> &dict, std::string propKey, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
            std::unordered_map<std::string, std::shared_ptr<Data>> paramMap;
            if (params.count(this->name) > 0) {
                paramMap = params.at(this->name);
                if (paramMap.count(propKey) > 0) {
                    auto data = paramMap.at(propKey);
                    return data->cast<T>();
                }
            }
            
            return dict->get<T>(propKey);
        }
    };
}

#endif /* Entity_h */
