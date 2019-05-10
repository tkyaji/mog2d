#include "mog/base/ScrollGroup.h"

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

std::shared_ptr<ScrollGroup> ScrollGroup::create(const mog::Size &scrollSize, const mog::Size &contentSize, unsigned char scrollFlag) {
    auto group = std::shared_ptr<ScrollGroup>(new ScrollGroup());
    group->init(scrollSize, contentSize, scrollFlag);
    return group;
}

void ScrollGroup::init(const mog::Size &scrollSize, const mog::Size &contentSize, unsigned char scrollFlag) {
    this->scrollFlag = scrollFlag;
    this->enableBatching = true;
    this->numOfTexture = 1;
    this->contentGroup = Group::create();
    this->contentGroup->setSize(contentSize);
    Group::add(this->contentGroup);
    
    this->renderer->vertexShader = BasicShader::getShader(BasicShader::Type::VertexColorWithTexture, ShaderType::VertexShader);
    this->renderer->fragmentShader = Shader::create(fragmentShaderSource, ShaderType::FragmentShader);
    
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

void ScrollGroup::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, unsigned char parentReRenderFlag) {
    if (!this->dragging && (abs(this->velocity.x) >= 0.00001f || abs(this->velocity.y) >= 0.00001f)) {
        this->setScrollPosition(this->contentGroup->getPosition() + this->velocity);
        this->velocity *= 0.9f;
    }

    Group::updateFrame(engine, delta, parentMatrix, parentReRenderFlag);
}

void ScrollGroup::drawFrame(float delta) {
    if ((this->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        auto pos = this->getAbsolutePosition();
        auto size = this->getAbsoluteSize();
        this->renderer->setUniformParameter("u_position", pos.x, pos.y);
        this->renderer->setUniformParameter("u_size", size.width, size.height);
    }
    Group::drawFrame(delta);
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

std::shared_ptr<Entity> ScrollGroup::cloneEntity() {
    // TODO
    return nullptr;
}
