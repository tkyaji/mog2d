#include "mog/base/Entity.h"
#include "mog/base/Group.h"
#include "mog/core/Engine.h"
#include "mog/core/TouchEventListener.h"
#include "mog/core/MogStats.h"
#include "mog/core/Tween.h"
#include <math.h>

using namespace mog;

Entity::Entity() : colliderShape(ColliderShape::Rect) {
    MogStats::instanceCount++;
    
    this->renderer = make_shared<Renderer>();
    this->transform = make_shared<Transform>();
    this->tweenIdsToRemove.reserve(16);
}

Entity::~Entity() {
    MogStats::instanceCount--;
}

shared_ptr<Group> Entity::getGroup() {
    return this->group.lock();
}

void Entity::setGroup(shared_ptr<Group> group) {
    this->group = group;
    this->dirtyFlag |= DIRTY_POSITION;
}

shared_ptr<Renderer> Entity::getRenderer() {
    return this->renderer;
}

void Entity::updateFrame(const shared_ptr<Engine> &engine, float delta) {
    this->screenScale = engine->getScreenScale();
    this->updatePositionAndSize();
    this->extractEvent(engine, delta);
    this->onUpdate(delta);
}

void Entity::drawFrame(float delta) {
    if (!this->visible) return;
    
    if (this->reRenderFlag > 0) {
        this->bindVertex();
        this->reRenderFlag = 0;
    }
    
    this->renderer->drawFrame(this->transform, this->screenScale);
}

void Entity::extractEvent(const shared_ptr<Engine> &engine, float delta) {
    auto self = static_pointer_cast<Entity>(shared_from_this());
    
    if (this->touchEnable && (!this->swallowTouches || this->touchListeners.size() > 0)) {
        engine->pushTouchableEntity(self);
    }
    
    if (this->tweens.size() > 0) {
        for (const auto &m : this->tweens) {
            m.second->update(delta, self);
        }
    }
    if (this->tweenIdsToRemove.size() > 0) {
        for (unsigned id : this->tweenIdsToRemove) {
            this->tweens.erase(id);
        }
        this->tweenIdsToRemove.clear();
    }
}

void Entity::onUpdate(float delta) {
}

void Entity::updateMatrix() {
    this->renderer->pushMatrix();
    this->renderer->applyTransform(this->transform, this->screenScale, false);
    this->renderer->popMatrix();
}

void Entity::bindVertex() {
    if ((this->reRenderFlag & RERENDER_VERTEX) != RERENDER_VERTEX) {
        return;
    }
    
    int indiciesNum = 0;
    this->getIndiciesNum(&indiciesNum);
    int verticesNum = 0;
    this->getVerticesNum(&verticesNum);
    
    auto indices = new short[indiciesNum];
    int idx = 0;
    this->bindIndices(indices, &idx, 0);
    auto vertices = new float[verticesNum * 2];
    idx = 0;
    this->bindVertices(vertices, &idx, false);
    
    this->renderer->bindVertex(vertices, verticesNum * 2, indices, indiciesNum, this->dynamicDraw);
    
    safe_delete_arr(indices);
    safe_delete_arr(vertices);
}

void Entity::bindVertices(float *vertices, int *idx, bool bakeTransform) {
    if (!this->visible) return;
    float *m;
    if (bakeTransform) {
        this->renderer->pushMatrix();
        this->renderer->applyTransform(this->transform, this->screenScale, false);
        m = this->renderer->matrix;
        this->renderer->popMatrix();
    } else {
        m = Renderer::identityMatrix;
    }
    
    auto v1 = Point(m[0], m[1]);
    auto v2 = Point(m[4], m[5]);
    auto p1 = Point::zero;
    auto p2 = v2 * this->transform->size.height * this->screenScale;
    auto p3 = v1 * this->transform->size.width * this->screenScale;
    auto p4 = p2 + p3;
    auto offset = Point(m[12], m[13]);
    
    vertices[(*idx)++] = p1.x + offset.x;   vertices[(*idx)++] = p1.y + offset.y;
    vertices[(*idx)++] = p2.x + offset.x;   vertices[(*idx)++] = p2.y + offset.y;
    vertices[(*idx)++] = p3.x + offset.x;   vertices[(*idx)++] = p3.y + offset.y;
    vertices[(*idx)++] = p4.x + offset.x;   vertices[(*idx)++] = p4.y + offset.y;
}

void Entity::bindIndices(short *indices, int *idx, int start) {
    if (!this->visible) return;
    if (start > 0) {
        indices[*idx] = indices[(*idx) - 1];
        (*idx)++;
        indices[(*idx)++] = 0 + start;
    }
    indices[(*idx)++] = 0 + start;
    indices[(*idx)++] = 1 + start;
    indices[(*idx)++] = 2 + start;
    indices[(*idx)++] = 3 + start;
}

void Entity::bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
}

void Entity::bindVertexColors(float *vertexColors, int *idx, const Color &parentColor) {
}

void Entity::bindVertexSub() {
}

void Entity::getVerticesNum(int *num) {
    if (!this->visible) return;
    (*num) += 4;
}

void Entity::getIndiciesNum(int *num) {
    if (!this->visible) return;
    if (*num > 0) {
        (*num) += 2;
    }
    (*num) += 4;
}

void Entity::show() {
    this->visible = true;
}

void Entity::hide() {
    this->visible = false;
}

void Entity::removeFromParent() {
    auto self = static_pointer_cast<Entity>(shared_from_this());
    if (auto group = this->group.lock()) {
        group->remove(self);
    }
}

void Entity::setAnchor(const Point &position) {
    this->transform->anchor = position;
    this->setReRenderFlag(RERENDER_VERTEX);
}

void Entity::setAnchor(float x, float y) {
    this->transform->anchor.x = x;
    this->transform->anchor.y = y;
    this->setReRenderFlag(RERENDER_VERTEX);
}

void Entity::setAnchorX(float x) {
    this->transform->anchor.x = x;
    this->setReRenderFlag(RERENDER_VERTEX);
}

void Entity::setAnchorY(float y) {
    this->transform->anchor.y = y;
    this->setReRenderFlag(RERENDER_VERTEX);
}

Point Entity::getAnchor() {
    return this->transform->anchor;
}

float Entity::getAnchorX() {
    return this->transform->anchor.x;
}

float Entity::getAnchorY() {
    return this->transform->anchor.y;
}

void Entity::setPosition(const Point &position) {
    this->position = position;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

void Entity::setPosition(float x, float y) {
    this->position.x = x;
    this->position.y = y;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

void Entity::setPositionX(float x) {
    this->position.x = x;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

void Entity::setPositionY(float y) {
    this->position.y = y;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

Point Entity::getPosition() {
    return this->position;
}

float Entity::getPositionX() {
    return this->position.x;
}

float Entity::getPositionY() {
    return this->position.y;
}

Point Entity::getPositionFrom(Point origin) {
    if (auto group = this->group.lock()) {
        if (origin == this->origin) {
            return this->getPosition();
        }

        auto parentSize = group->getSize();
        Point offset1 = parentSize * this->origin;
        Point offset2 = parentSize * origin;
        return this->getPosition() + offset1 - offset2;
        
    } else {
        return this->getPosition();
    }
}

Point Entity::getPositionFrom(float x, float y) {
    return this->getPositionFrom(Point(x, y));
}

void Entity::setOrigin(Point origin) {
    this->origin = origin;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

void Entity::setOrigin(float x, float y) {
    this->origin.x = x;
    this->origin.y = y;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

void Entity::setOriginX(float x) {
    this->origin.x = x;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

void Entity::setOriginY(float y) {
    this->origin.y = y;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_POSITION;
}

Point Entity::getOrigin() {
    return this->origin;
}

float Entity::getOriginX() {
    return this->origin.x;
}

float Entity::getOriginY() {
    return this->origin.y;
}

void Entity::setScale(float scale) {
    this->setScale(Point(scale, scale));
    this->setReRenderFlag(RERENDER_VERTEX);
}

void Entity::setScale(float scaleX, float scaleY) {
    this->transform->scale.x = scaleX;
    this->transform->scale.y = scaleY;
    this->setReRenderFlag(RERENDER_VERTEX);
}

void Entity::setScale(const Point &scale) {
    this->transform->scale = scale;
    this->setReRenderFlag(RERENDER_VERTEX);
}

void Entity::setScaleX(float scaleX) {
    this->transform->scale.x = scaleX;
    this->setReRenderFlag(RERENDER_VERTEX);
}

void Entity::setScaleY(float scaleY) {
    this->transform->scale.y = scaleY;
    this->setReRenderFlag(RERENDER_VERTEX);
}

Point Entity::getScale() {
    return this->transform->scale;
}

float Entity::getScaleX() {
    return this->transform->scale.x;
}

float Entity::getScaleY() {
    return this->transform->scale.y;
}

void Entity::setRotation(float angle) {
    this->transform->rotation = angle;
    this->setReRenderFlag(RERENDER_VERTEX);
}

float Entity::getRotation() {
    return this->transform->rotation;
}

void Entity::setWidth(float width, bool isRatio) {
    this->size.width = width;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_SIZE;
    this->ratioWidth = isRatio;
}

float Entity::getWidth() {
    return this->getSize().width;
}

float Entity::getWidthValue() {
    return this->size.width;
}

void Entity::setHeight(float height, bool isRatio) {
    this->size.height = height;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_SIZE;
    this->ratioHeight = isRatio;
}

float Entity::getHeight() {
    return this->getSize().height;
}

float Entity::getHeightValue() {
    return this->size.height;
}

void Entity::setSize(const Size &size, bool isRatio) {
    this->size = size;
    this->ratioWidth = isRatio;
    this->ratioHeight = isRatio;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_SIZE;
}

void Entity::setSize(float width, float height, bool isRatio) {
    this->size.width = width;
    this->size.height = height;
    this->ratioWidth = isRatio;
    this->ratioHeight = isRatio;
    this->setReRenderFlag(RERENDER_VERTEX);
    this->dirtyFlag |= DIRTY_SIZE;
}

Size Entity::getSize() {
    this->updatePositionAndSize();
    return this->transform->size;
}

Size Entity::getSizeValue() {
    return this->size;
}

bool Entity::isRatioWidth() {
    return this->ratioWidth;
}

bool Entity::isRatioHeight() {
    return this->ratioHeight;
}

void Entity::setColor(const Color &color) {
    this->transform->color = color;
    this->setReRenderFlag(RERENDER_COLOR);
}

void Entity::setColor(float r, float g, float b, float a) {
    this->transform->color.r = r;
    this->transform->color.g = g;
    this->transform->color.b = b;
    this->transform->color.a = a;
    this->setReRenderFlag(RERENDER_COLOR);
}

void Entity::setColorR(float r) {
    this->transform->color.r = r;
    this->setReRenderFlag(RERENDER_COLOR);
}

void Entity::setColorG(float g) {
    this->transform->color.g = g;
    this->setReRenderFlag(RERENDER_COLOR);
}

void Entity::setColorB(float b) {
    this->transform->color.b = b;
    this->setReRenderFlag(RERENDER_COLOR);
}

void Entity::setColorA(float a) {
    this->transform->color.a = a;
    this->setReRenderFlag(RERENDER_COLOR);
}

Color Entity::getColor() {
    return this->transform->color;
}

void Entity::setZIndex(int zIndex) {
    this->zIndex = zIndex;
    if (auto group = this->group.lock()) {
        group->enableSortOrderDirty();
    }
}

int Entity::getZIndex() {
    return this->zIndex;
}

shared_ptr<Texture2D> Entity::getTexture() {
    return this->texture;
}

Point Entity::getAbsolutePosition() {
    return Point(this->renderer->matrix[12] / this->screenScale, this->renderer->matrix[13] / this->screenScale);
}

Point Entity::getAbsoluteScale() {
    float x = sqrt(this->renderer->matrix[0] * this->renderer->matrix[0] +
                   this->renderer->matrix[1] * this->renderer->matrix[1]);
    float y = sqrt(this->renderer->matrix[4] * this->renderer->matrix[4] +
                   this->renderer->matrix[5] * this->renderer->matrix[5]);
    return Point(x, y);
}

Point Entity::getAbsoluteSize() {
    return this->getSize() * this->getAbsoluteScale();
}

string Entity::getName() {
    return this->name;
}

void Entity::setName(string name) {
    this->name = name;
}

string Entity::getTag() {
    return this->tag;
}

void Entity::setTag(string tag) {
    this->tag = tag;
}

bool Entity::contains(const Point &position) {
    return Collision::collides(this->getCollider(), position);
}

bool Entity::collidesWith(const shared_ptr<Entity> &other) {
    return Collision::collides(this->getCollider(), other->getCollider());
}

unsigned int Entity::addTouchEvent(const shared_ptr<TouchEventListener> &listener) {
    unsigned int eventId = ++this->eventIdCounter;
    this->touchListeners[eventId] = listener;
    return eventId;
}

shared_ptr<TouchEventListener> Entity::getTouchEvent(unsigned int eventId) {
    return this->touchListeners[eventId];
}

void Entity::removeTouchEvent(unsigned int eventId) {
    this->touchListeners.erase(eventId);
}

void Entity::removeAllTouchEvents() {
    this->touchListeners.clear();
}

void Entity::fireTouchBeginEvent(const Touch &touch) {
    if (!this->touchEnable) return;
    
    auto self = static_pointer_cast<Entity>(shared_from_this());
    for (auto pair : this->touchListeners) {
        auto listener = pair.second;
        listener->touchBegin(touch, self);
    }
}

void Entity::fireTouchMoveEvent(const Touch &touch) {
    if (!this->touchEnable) return;
    
    auto self = static_pointer_cast<Entity>(shared_from_this());
    for (auto pair : this->touchListeners) {
        auto listener = pair.second;
        listener->touchMove(touch, self);
    }
}

void Entity::fireTouchEndEvent(const Touch &touch) {
    if (!this->touchEnable) return;
    
    auto self = static_pointer_cast<Entity>(shared_from_this());
    for (auto pair : this->touchListeners) {
        auto listener = pair.second;
        listener->touchEnd(touch, self);
    }
}

void Entity::setSwallowTouches(bool swallowTouches) {
    this->swallowTouches = swallowTouches;
}

bool Entity::isSwallowTouches() {
    return this->swallowTouches;
}

void Entity::setTouchEnable(bool enable) {
    this->touchEnable = enable;
}

bool Entity::isTouchEnable() {
    return this->touchEnable;
}

void Entity::runTween(const shared_ptr<Tween> &tween) {
    this->tweens[tween->getTweenId()] = tween;
    tween->addOnFinishEventForParent([this](const shared_ptr<Tween> &t) {
        this->tweenIdsToRemove.emplace_back(t->getTweenId());
    });
    auto self = static_pointer_cast<Entity>(shared_from_this());
    tween->init();
    tween->update(0, self);
}

void Entity::cancelTween(unsigned int tweenId) {
    this->tweens.erase(tweenId);
}

void Entity::cancelAllTweens() {
    this->tweens.clear();
}

shared_ptr<Group> Entity::findParentByName(string name) {
    shared_ptr<Entity> e = shared_from_this();
    while (auto g = e->getGroup()) {
        if (g->getName() == name) return g;
        e = g;
    }
    return nullptr;
}

shared_ptr<Group> Entity::findParentByTag(string tag) {
    shared_ptr<Entity> e = shared_from_this();
    while (auto g = e->getGroup()) {
        if (g->getTag() == tag) return g;
        e = g;
    }
    return nullptr;
}

shared_ptr<OBB> Entity::getOBB() {
    float scaleX = sqrt(this->renderer->matrix[0] * this->renderer->matrix[0] +
                        this->renderer->matrix[1] * this->renderer->matrix[1]);
    float scaleY = sqrt(this->renderer->matrix[4] * this->renderer->matrix[4] +
                        this->renderer->matrix[5] * this->renderer->matrix[5]);
    float x = this->renderer->matrix[12] / this->screenScale;
    float y = this->renderer->matrix[13] / this->screenScale;
    auto vec1 = Vec2(this->renderer->matrix[0] / scaleX, this->renderer->matrix[1] / scaleX,
                     this->transform->size.width * scaleX);
    auto vec2 = Vec2(this->renderer->matrix[4] / scaleY, this->renderer->matrix[5] / scaleY,
                     this->transform->size.height * scaleY);
    auto vec3 = Vec2(vec1.x * vec1.vLen + vec2.x * vec2.vLen, vec1.y * vec1.vLen + vec2.y * vec2.vLen);
    float centerX = x + (vec3.x * scaleX * 0.5f);
    float centerY = y + (vec3.y * scaleY * 0.5f);
    
    return shared_ptr<OBB>(new OBB(vec1, vec2, centerX, centerY));
}

shared_ptr<AABB> Entity::getAABB() {
    auto v1 = Point(this->renderer->matrix[0], this->renderer->matrix[1]);
    auto v2 = Point(this->renderer->matrix[4], this->renderer->matrix[5]);
    auto offset = Point(this->renderer->matrix[12], this->renderer->matrix[13]) / this->screenScale;
    auto size = this->transform->size;
    auto p1 = Point::zero;
    auto p2 = Point(v1.x * size.width, v1.y * size.width);
    auto p3 = Point(v2.x * size.height, v2.y * size.height);
    auto p4 = p2 + p3;
    Point minP = Point(min(min(p1.x, p2.x), min(p3.x, p4.x)), min(min(p1.y, p2.y), min(p3.y, p4.y)));
    Point maxP = Point(max(max(p1.x, p2.x), max(p3.x, p4.x)), max(max(p1.y, p2.y), max(p3.y, p4.y)));
    
    return shared_ptr<AABB>(new AABB(offset.x + minP.x, offset.y + minP.y, offset.x + maxP.x, offset.y + maxP.y));
}

shared_ptr<Collider> Entity::getCollider() {
    auto collider = shared_ptr<Collider>(new Collider(ColliderShape::Rect));
    collider->aabb = this->getAABB();
    collider->obb = this->getOBB();
    return collider;
}

void Entity::setReRenderFlag(unsigned char flag) {
    if (auto group = this->group.lock()) {
        group->setReRenderFlag(flag);
    }
    this->reRenderFlag |= flag;
}

void Entity::setReRenderFlagToChild(unsigned char flag) {
    this->reRenderFlag |= flag;
}

void Entity::addTextureTo(const shared_ptr<TextureAtlas> &textureAtlas) {
    textureAtlas->addTexture(this->getTexture());
}

void Entity::copyFrom(const shared_ptr<Entity> &src) {
    this->texture = src->texture;
    this->transform->copyFrom(src->transform);
}

void Entity::updatePositionAndSize() {
    if ((this->dirtyFlag & DIRTY_POSITION) == DIRTY_POSITION) {
        this->transform->position = this->getPositionFrom(Point::zero);
        this->reRenderFlag |= RERENDER_VERTEX;
    }
    
    if ((this->dirtyFlag & DIRTY_SIZE) == DIRTY_SIZE) {
        float w = 0;
        float h = 0;
        
        if (this->ratioWidth) {
            if (auto group = this->group.lock()) {
                w = group->getWidth() * this->size.width;
            }
        } else {
            w = this->size.width;
        }
        
        if (this->ratioHeight) {
            if (auto group = this->group.lock()) {
                h = group->getHeight() * this->size.height;
            }
        } else {
            h = this->size.height;
        }
        
        if (!approximately(w, this->transform->size.width)) {
            this->transform->size.width = w;
            this->reRenderFlag |= RERENDER_VERTEX;
        }
        if (!approximately(h, this->transform->size.height)) {
            this->transform->size.height = h;
            this->reRenderFlag |= RERENDER_VERTEX;
        }
    }

    this->dirtyFlag = 0;
}

