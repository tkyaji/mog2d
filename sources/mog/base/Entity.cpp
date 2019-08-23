#include "mog/base/Entity.h"
#include "mog/base/Group.h"
#include "mog/core/TouchEventListener.h"

using namespace mog;

void Entity::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) {
    this->updateFrame(engine, delta, parentMatrix, parentMatrix, parentDirtyFlag);
}

void Entity::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) {
    this->dirtyFlag |= parentDirtyFlag;
    Drawable::updateFrame(engine, delta, parentRendererMatrix, parentDirtyFlag);
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        Transform::multiplyMatrix(this->transform->matrix, parentMatrix, this->matrix);
        this->collider = nullptr;
    }
    if ((this->dirtyFlag & DIRTY_COLOR) == DIRTY_COLOR) {
        Transform::multiplyColor(this->transform->matrix, parentMatrix, this->matrix);
    }
    this->extractEvent(engine, delta);
}

void Entity::extractEvent(const std::shared_ptr<Engine> &engine, float delta) {
    if (!this->active) return;
    auto self = std::static_pointer_cast<Entity>(shared_from_this());
    if (this->touchEnable && (this->swallowTouches || this->touchListeners.size() > 0)) {
        engine->pushTouchableEntity(self);
    }
}

void Entity::initRendererVertices(int verticesNum, int indicesNum) {
    this->renderer->setVerticesNum(verticesNum);
    this->renderer->setIndicesNum(indicesNum);
    this->renderer->newVerticesArr();
    this->renderer->newIndicesArr();
}

void Entity::bindVertex() {
    if (this->dirtyFlag == 0) return;
    
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        int vertexIdx = 0;
        int indexIdx = 0;
        this->bindVertices(this->renderer, &vertexIdx, &indexIdx, false);
        this->renderer->bindVertex();
    }

    int i = 0;
    while (this->textures[i]) {
        if ((this->dirtyFlag & DIRTY_TEXTURE) == DIRTY_TEXTURE) {
            this->textures[i]->bindTexture(i);
        }
        if ((this->dirtyFlag & DIRTY_TEX_COORDS) == DIRTY_TEX_COORDS) {
            int vertexTexCoordsIdx = 0;
            if (!this->renderer->vertexTexCoords[i]) this->renderer->newVertexTexCoordsArr(i);
            this->bindVertexTexCoords(this->renderer, &vertexTexCoordsIdx, 0, 0, 0, 1.0f, 1.0f);
            this->renderer->bindVertexTexCoords(i);
            this->renderer->bindTexture(this->textures[i], i);
        }
        i++;
    }

    this->dirtyFlag = 0;
}

void Entity::bindVertices(const std::shared_ptr<Renderer> &renderer, int *verticesIdx, int *indicesIdx, bool bakeTransform) {
    Point p1 = Point(0, 0);
    Point p2 = Point(0, this->transform->size.height);
    Point p3 = Point(this->transform->size.width, 0);
    Point p4 = p2 + p3;
    
    if (bakeTransform) {
        auto offset = Point(this->renderer->matrix[12], this->renderer->matrix[13]);
        auto v1 = Point(this->renderer->matrix[0], this->renderer->matrix[1]);
        auto v2 = Point(this->renderer->matrix[4], this->renderer->matrix[5]);
        p1 = v1 * p1.x + v2 * p1.y + offset;
        p2 = v1 * p2.x + v2 * p2.y + offset;
        p3 = v1 * p3.x + v2 * p3.y + offset;
        p4 = v1 * p4.x + v2 * p4.y + offset;
    }

    if (!this->active) {
        p1.x = 0;   p1.y = 0;
        p2.x = 0;   p2.y = 0;
        p3.x = 0;   p3.y = 0;
        p4.x = 0;   p4.y = 0;
    }
    
    int startN = *verticesIdx / 2;
    renderer->vertices[(*verticesIdx)++] = p1.x;  renderer->vertices[(*verticesIdx)++] = p1.y;
    renderer->vertices[(*verticesIdx)++] = p2.x;  renderer->vertices[(*verticesIdx)++] = p2.y;
    renderer->vertices[(*verticesIdx)++] = p3.x;  renderer->vertices[(*verticesIdx)++] = p3.y;
    renderer->vertices[(*verticesIdx)++] = p4.x;  renderer->vertices[(*verticesIdx)++] = p4.y;
    
    if (indicesIdx) {
        int startI = *indicesIdx;
        if (startI > 0) {
            renderer->indices[*indicesIdx] = renderer->indices[(*indicesIdx) - 1];
            (*indicesIdx)++;
            renderer->indices[(*indicesIdx)++] = startN;
        }
        renderer->indices[(*indicesIdx)++] = 0 + startN;
        renderer->indices[(*indicesIdx)++] = 1 + startN;
        renderer->indices[(*indicesIdx)++] = 2 + startN;
        renderer->indices[(*indicesIdx)++] = 3 + startN;
    }
}

void Entity::bindVertexColors(const std::shared_ptr<Renderer> &renderer, int *idx) {
    for (int i = 0; i < this->renderer->verticesNum; i++) {
        renderer->vertexColors[(*idx)++] = this->renderer->matrix[16];
        renderer->vertexColors[(*idx)++] = this->renderer->matrix[17];
        renderer->vertexColors[(*idx)++] = this->renderer->matrix[18];
        renderer->vertexColors[(*idx)++] = this->renderer->matrix[19];
    }
}

void Entity::bindVertexTexCoords(const std::shared_ptr<Renderer> &renderer, int *idx, int texIdx, float x, float y, float w, float h) {
    if (this->textures[0] && this->textures[0]->isFlip) {
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;        renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;        renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        
    } else {
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;        renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x;        renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y;
        renderer->vertexTexCoords[texIdx][(*idx)++] = x + w;    renderer->vertexTexCoords[texIdx][(*idx)++] = y + h;
    }
}

void Entity::updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) {
    unsigned char mergedDirtyFlag = (this->dirtyFlag | parentDirtyFlag);
    
    if ((mergedDirtyFlag & (DIRTY_SIZE | DIRTY_ANCHOR)) > 0) {
        this->updateTransform();
    }
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        this->transform->updateMatrix();
    }
    if ((dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        Transform::multiplyMatrix(this->transform->matrix, parentMatrix, this->matrix);
    }
}

std::string Entity::getName() {
    return this->name;
}

void Entity::setName(std::string name) {
    this->name = name;
}

std::string Entity::getTag() {
    return this->tag;
}

void Entity::setTag(std::string tag) {
    this->tag = tag;
}

std::shared_ptr<Group> Entity::getGroup() {
    if (auto d = this->group.lock()) {
        if (auto g = std::dynamic_pointer_cast<Group>(d)) {
            return g;
        }
    }
    return nullptr;
}

Point Entity::getAbsolutePosition() {
    Drawable::updateMatrix();
    return Point(this->matrix[12], this->matrix[13]);
}

Size Entity::getAbsoluteSize() {
    Drawable::updateMatrix();
    float scaleX = sqrt(this->matrix[0] * this->matrix[0] +
                        this->matrix[1] * this->matrix[1]);
    float scaleY = sqrt(this->matrix[4] * this->matrix[4] +
                        this->matrix[5] * this->matrix[5]);
    return this->transform->size * Point(scaleX, scaleY);
}

Point Entity::getAbsoluteScale() {
    Drawable::updateMatrix();
    float scaleX = sqrt(this->matrix[0] * this->matrix[0] +
                        this->matrix[1] * this->matrix[1]);
    float scaleY = sqrt(this->matrix[4] * this->matrix[4] +
                        this->matrix[5] * this->matrix[5]);
    return Point(scaleX, scaleY);
}

bool Entity::contains(const Point &position) {
    return Collision::collides(this->getCollider(), position);
}

bool Entity::collidesWith(const std::shared_ptr<Entity> &other) {
    return Collision::collides(this->getCollider(), other->getCollider());
}

unsigned int Entity::addTouchEvent(const std::shared_ptr<TouchEventListener> &listener) {
    unsigned int eventId = ++this->eventIdCounter;
    this->touchListeners[eventId] = listener;
    return eventId;
}

std::shared_ptr<TouchEventListener> Entity::getTouchEvent(unsigned int eventId) {
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
    
    auto self = std::static_pointer_cast<Entity>(shared_from_this());
    for (auto pair : this->touchListeners) {
        auto listener = pair.second;
        listener->touchBegin(touch, self);
    }
}

void Entity::fireTouchMoveEvent(const Touch &touch) {
    if (!this->touchEnable) return;
    
    auto self = std::static_pointer_cast<Entity>(shared_from_this());
    for (auto pair : this->touchListeners) {
        auto listener = pair.second;
        listener->touchMove(touch, self);
    }
}

void Entity::fireTouchEndEvent(const Touch &touch) {
    if (!this->touchEnable) return;
    
    auto self = std::static_pointer_cast<Entity>(shared_from_this());
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

float *Entity::getMatrix() {
    return this->matrix;
}

std::shared_ptr<Collider> Entity::getCollider() {
    if (this->collider) return this->collider;
    this->collider = std::shared_ptr<Collider>(new Collider(ColliderShape::Rect));
    this->collider->aabb = this->getAABB();
    this->collider->obb = this->getOBB();
    return this->collider;
}

std::shared_ptr<OBB> Entity::getOBB() {
    float scaleX = sqrt(this->matrix[0] * this->matrix[0] +
                        this->matrix[1] * this->matrix[1]);
    float scaleY = sqrt(this->matrix[4] * this->matrix[4] +
                        this->matrix[5] * this->matrix[5]);
    float x = this->matrix[12];
    float y = this->matrix[13];
    auto vec1 = Vec2(this->matrix[0] / scaleX, this->matrix[1] / scaleX,
                     this->transform->size.width * scaleX * 0.5f);
    auto vec2 = Vec2(this->matrix[4] / scaleY, this->matrix[5] / scaleY,
                     this->transform->size.height * scaleY * 0.5f);
    auto vec3 = Vec2(vec1.x * vec1.vLen + vec2.x * vec2.vLen, vec1.y * vec1.vLen + vec2.y * vec2.vLen, 1.0f);
    float centerX = x + vec3.x;
    float centerY = y + vec3.y;
    
    return std::shared_ptr<OBB>(new OBB(vec1, vec2, centerX, centerY));
}

std::shared_ptr<AABB> Entity::getAABB() {
    auto v1 = Point(this->matrix[0], this->matrix[1]);
    auto v2 = Point(this->matrix[4], this->matrix[5]);
    auto offset = Point(this->matrix[12], this->matrix[13]);
    auto size = this->transform->size;
    auto p1 = Point::zero;
    auto p2 = Point(v1.x * size.width, v1.y * size.width);
    auto p3 = Point(v2.x * size.height, v2.y * size.height);
    auto p4 = p2 + p3;
    Point minP = Point(fmin(fmin(p1.x, p2.x), fmin(p3.x, p4.x)), fmin(fmin(p1.y, p2.y), fmin(p3.y, p4.y)));
    Point maxP = Point(fmax(fmax(p1.x, p2.x), fmax(p3.x, p4.x)), fmax(fmax(p1.y, p2.y), fmax(p3.y, p4.y)));
    
    return std::shared_ptr<AABB>(new AABB(offset.x + minP.x, offset.y + minP.y, offset.x + maxP.x, offset.y + maxP.y));
}

void Entity::copyProperties(const std::shared_ptr<Entity> &entity) {
    this->setTag(entity->getTag());
    this->setPivot(entity->getPivot());
    this->setPosition(entity->getPosition());
    this->setSize(entity->getSize());
    this->setScale(entity->getScale());
    this->setRotation(entity->getRotation());
    this->setColor(entity->getColor());
    this->setSwallowTouches(entity->isSwallowTouches());
    this->setTouchEnable(entity->isTouchEnable());
    this->setZIndex(entity->getZIndex());
    this->setActive(entity->isActive());
    this->setParam(entity->getParam<Data>());
}

std::shared_ptr<Dictionary> Entity::serialize() {
    auto dict = Dictionary::create();
    dict->put(PROP_KEY_ACTIVE, Bool::create(this->active));
    dict->put(PROP_KEY_NAME, String::create(this->name));
    dict->put(PROP_KEY_TAG, String::create(this->tag));
    dict->put(PROP_KEY_POSITION_X, Float::create(this->transform->position.x));
    dict->put(PROP_KEY_POSITION_Y, Float::create(this->transform->position.y));
    dict->put(PROP_KEY_PIVOT_X, Float::create(this->transform->pivot.x));
    dict->put(PROP_KEY_PIVOT_Y, Float::create(this->transform->pivot.y));
    dict->put(PROP_KEY_SCALE_X, Float::create(this->transform->scale.x));
    dict->put(PROP_KEY_SCALE_Y, Float::create(this->transform->scale.y));
    dict->put(PROP_KEY_ANCHOR_X, Float::create(this->anchor.x));
    dict->put(PROP_KEY_ANCHOR_Y, Float::create(this->anchor.y));
    dict->put(PROP_KEY_SIZE_WIDTH, Float::create(this->size.width));
    dict->put(PROP_KEY_SIZE_HEIGHT, Float::create(this->size.height));
    dict->put(PROP_KEY_SIZE_SET_IN_RATIO, Int::create((int)this->sizeSetInRatioFlag));
    dict->put(PROP_KEY_COLOR, String::create(this->getColorCode()));
    dict->put(PROP_KEY_ALPHA, Float::create(this->getColorA()));
    dict->put(PROP_KEY_Z_INDEX, Int::create(this->zIndex));
    return dict;
}

void Entity::deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    this->active = dict->get<Bool>(PROP_KEY_ACTIVE)->getValue();
    this->name = dict->get<String>(PROP_KEY_NAME)->getValue();
    this->tag = this->getPropertyData<String>(dict, PROP_KEY_TAG, params)->getValue();
    this->transform->position.x = this->getPropertyData<Float>(dict, PROP_KEY_POSITION_X, params)->getValue();
    this->transform->position.y = this->getPropertyData<Float>(dict, PROP_KEY_POSITION_Y, params)->getValue();
    this->transform->pivot.x =  this->getPropertyData<Float>(dict, PROP_KEY_PIVOT_X, params)->getValue();
    this->transform->pivot.y = this->getPropertyData<Float>(dict, PROP_KEY_PIVOT_Y, params)->getValue();
    this->transform->scale.x =  this->getPropertyData<Float>(dict, PROP_KEY_SCALE_X, params)->getValue();
    this->transform->scale.y = this->getPropertyData<Float>(dict, PROP_KEY_SCALE_Y, params)->getValue();
    this->anchor.x = this->getPropertyData<Float>(dict, PROP_KEY_ANCHOR_X, params)->getValue();
    this->anchor.y = this->getPropertyData<Float>(dict, PROP_KEY_ANCHOR_Y, params)->getValue();
    this->size.width = this->getPropertyData<Float>(dict, PROP_KEY_SIZE_WIDTH, params)->getValue();
    this->size.height = this->getPropertyData<Float>(dict, PROP_KEY_SIZE_HEIGHT, params)->getValue();
    this->sizeSetInRatioFlag = (unsigned char)this->getPropertyData<Int>(dict, PROP_KEY_SIZE_SET_IN_RATIO, params)->getValue();
    this->transform->color = Color(this->getPropertyData<String>(dict, PROP_KEY_COLOR, params)->getValue());
    this->transform->color.a = this->getPropertyData<Float>(dict, PROP_KEY_ALPHA, params)->getValue();
    this->zIndex = this->getPropertyData<Int>(dict, PROP_KEY_Z_INDEX, params)->getValue();
}

bool Entity::isGroup() {
    return false;
}
