#include "mog/base/ScrollGroup.h"
#include "mog/core/TouchEventListener.h"

using namespace mog;

static const GLchar *fragmentShaderSource = "\
uniform sampler2D u_texture0;\
uniform highp vec2 u_screenSize;\
uniform highp float u_screenScale;\
uniform highp vec2 u_position;\
uniform highp vec2 u_size;\
varying highp vec2 v_uv0;\
varying mediump vec4 v_color;\
void main() {\
    if (gl_FragCoord.x < u_position.x * u_screenScale || gl_FragCoord.y > (u_screenSize.y - u_position.y) * u_screenScale) {\
        discard;\
    } else if (gl_FragCoord.x > (u_position.x + u_size.x) * u_screenScale || gl_FragCoord.y < (u_screenSize.y - (u_position.y + u_size.y)) * u_screenScale) {\
        discard;\
    } else if (v_uv0.x < -0.1) {\
        gl_FragColor = v_color;\
    } else {\
        gl_FragColor = texture2D(u_texture0, v_uv0) * v_color;\
    }\
}\
";

std::shared_ptr<ScrollGroup> ScrollGroup::create(const mog::Size &contentSize, unsigned char scrollFlag) {
    auto group = std::shared_ptr<ScrollGroup>(new ScrollGroup());
    group->contentSize = contentSize;
    group->scrollFlag = scrollFlag;
    group->init();
    return group;
}

void ScrollGroup::init() {
    this->enableBatching = true;
    this->enableTexture = true;
    this->contentGroup = Group::create();
    this->contentGroup->setSize(this->contentSize);
    Group::add(this->contentGroup);
    
    this->renderer->shader->vertexShader = BasicShader::getShaderUnit(BasicShader::Type::VertexColorWithTexture, ShaderType::VertexShader);
    this->renderer->shader->fragmentShader = ShaderUnit::create(fragmentShaderSource, ShaderType::FragmentShader);
    
    auto listener = TouchEventListener::create();
    listener->setOnTouchMoveEvent([this](const Touch &t, const std::shared_ptr<Entity> &e) {
        this->dragging = true;
        auto deltaPosition = t.deltaPosition;
        if ((this->scrollFlag & SCROLL_HORIZONTAL) == SCROLL_HORIZONTAL) {
            this->velocity.x = fmin(this->velocity.x * 0.3f + t.deltaPosition.x, 20.0f);
        } else {
            deltaPosition.x = 0;
        }
        if ((this->scrollFlag & SCROLL_VERTICAL) == SCROLL_VERTICAL) {
            this->velocity.y = fmin(this->velocity.y * 0.3f + t.deltaPosition.y, 20.0f);
        } else {
            deltaPosition.y = 0;
        }
        this->setScrollPosition(this->contentGroup->getPosition() + deltaPosition);
    });
    listener->setOnTouchBeginEvent([this](const Touch &t, const std::shared_ptr<Entity> &e) {
        if (!this->contentGroup) return false;
        this->velocity = Point::zero;
        return true;
    });
    listener->setOnTouchEndEvent([this](const Touch &t, const std::shared_ptr<Entity> &e) {
        this->dragging = false;
    });
    this->addTouchEvent(listener);
}

void ScrollGroup::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentDirtyFlag) {
    if (!this->dragging && (abs(this->velocity.x) >= 0.00001f || abs(this->velocity.y) >= 0.00001f)) {
        this->setScrollPosition(this->contentGroup->getPosition() + this->velocity);
        this->velocity *= 0.9f;
    }

    Group::updateFrame(engine, delta, parentMatrix, parentDirtyFlag);
}

void ScrollGroup::drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) {
    if ((this->dirtyFlag & DIRTY_VERTEX) == DIRTY_VERTEX) {
        auto pos = this->getAbsolutePosition();
        auto size = this->getAbsoluteSize();
        this->renderer->shader->setUniformParameter("u_position", pos.x, pos.y);
        this->renderer->shader->setUniformParameter("u_size", size.width, size.height);
    }
    Group::drawFrame(delta, touches);
}

void ScrollGroup::add(const std::shared_ptr<Entity> &entity) {
    this->contentGroup->add(entity);
}

void ScrollGroup::remove(const std::shared_ptr<Entity> &entity) {
    this->contentGroup->remove(entity);
}

void ScrollGroup::removeAll() {
    this->contentGroup->removeAll();
}

std::vector<std::shared_ptr<Entity>> ScrollGroup::getChildEntities() {
    return this->contentGroup->getChildEntities();
}

std::shared_ptr<Entity> ScrollGroup::findChildByName(std::string name, bool recursive) {
    return this->contentGroup->findChildByName(name, recursive);
}

std::shared_ptr<Entity> ScrollGroup::findFirstChildByTag(std::string tag, bool recursive) {
    return this->contentGroup->findFirstChildByTag(tag, recursive);
}

std::vector<std::shared_ptr<Entity>> ScrollGroup::findChildrenByTag(std::string tag, bool recursive) {
    return this->contentGroup->findChildrenByTag(tag, recursive);
}

void ScrollGroup::setScrollPosition(const Point &position) {
    auto scrollSize = this->getAbsoluteSize();
    auto contentSize = this->contentGroup->getAbsoluteSize();
    Point p = position;
    p.x = fmax(p.x, scrollSize.width - contentSize.width);
    p.x = fmin(p.x, 0);
    p.y = fmax(p.y, scrollSize.height - contentSize.height);
    p.y = fmin(p.y, 0);
    this->contentGroup->setPosition(p);
}

std::shared_ptr<ScrollGroup> ScrollGroup::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<ScrollGroup>(e);
}

std::shared_ptr<Entity> ScrollGroup::cloneEntity() {
    auto scrollGroup = ScrollGroup::create(this->contentSize, this->scrollFlag);
    auto clonedContentGroup = std::static_pointer_cast<ScrollGroup>(shared_from_this())->contentGroup->clone();
    scrollGroup->contentGroup = clonedContentGroup;
    scrollGroup->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return scrollGroup;
}
