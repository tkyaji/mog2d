#include <math.h>
#include <algorithm>
#include "mog/Constants.h"
#include "mog/core/Tween.h"
#include "mog/core/Engine.h"
#include "mog/base/Entity.h"

using namespace mog;

inline float powIn(float p, float t) {
    return pow(t, p);
}

inline float powOut(float p, float t) {
    return 1 - pow(1 - t, p);
}

inline float powInOut(float p, float t) {
    t *= 2;
    if (t < 1) return 0.5 * pow(t, p);
    return 1 - 0.5 * abs(pow(2 - t, p));
}

inline float bounceOut(float t) {
    if (t < 1 / 2.75f) {
        return (7.5625f * t * t);
    } else if (t < 2 / 2.75f) {
        t -= 1.5f / 2.75f;
        return (7.5625f * t * t + 0.75f);
    } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return (7.5625f * t * t + 0.9375f);
    } else {
        t -= 2.625f / 2.75f;
        return (7.5625f * t * t + 0.984375f);
    }
}

inline float bounceIn(float t) {
    return 1 - bounceOut(1 - t);
}


// Linear
float EasingLinear::process(float t) {
    return t;
}

//Quad
float EasingQuadIn::process(float t) {
    return powIn(2, t);
}

float EasingQuadOut::process(float t) {
    return powOut(2, t);
}

float EasingQuadInOut::process(float t) {
    return powInOut(2, t);
}

// Cubic
float EasingCubicIn::process(float t) {
    return powIn(3, t);
}

float EasingCubicOut::process(float t) {
    return powOut(3, t);
}

float EasingCubicInOut::process(float t) {
    return powInOut(3, t);
}

// Quart
float EasingQuartIn::process(float t) {
    return powIn(4, t);
}

float EasingQuartOut::process(float t) {
    return powOut(4, t);
}

float EasingQuartInOut::process(float t) {
    return powInOut(4, t);
}

// Quint
float EasingQuintIn::process(float t) {
    return powIn(5, t);
}

float EasingQuintOut::process(float t) {
    return powOut(5, t);
}

float EasingQuintInOut::process(float t) {
    return powInOut(5, t);
}

// Sine
float EasingSineIn::process(float t) {
    return 1 - cos(t * M_PI_2);
}

float EasingSineOut::process(float t) {
    return sin(t * M_PI_2);
}

float EasingSineInOut::process(float t) {
    return -0.5f * (cos(M_PI * t) - 1);
}

// Back
float EasingBackIn::process(float t) {
    return t * t * (2.7f * t - 1.7f);
}

float EasingBackOut::process(float t) {
    t--;
    return (t * t * (2.7f * t + 1.7f) + 1);
}

float EasingBackInOut::process(float t) {
    t *= 2;
    if (t < 1) return 0.5 * (t * t * (2.525f * t - 1.525f));
    t -= 2;
    return 0.5f * (t * t * (2.525f * t + 1.525f) + 2);
}

// Circ
float EasingCircIn::process(float t) {
    return -(sqrt(1 - t * t) - 1);
}

float EasingCircOut::process(float t) {
    t--;
    return sqrt(1 - t * t);
}

float EasingCircInOut::process(float t) {
    t *= 2;
    if (t < 1) return -0.5f * (sqrt(1 - t * t) - 1);
    t -=2;
    return 0.5f * (sqrt(1 - t * t) + 1);
}

// Bounce
float EasingBounceIn::process(float t) {
    return 1 - bounceOut(1 - t);
}

float EasingBounceOut::process(float t) {
    return bounceOut(t);
}

float EasingBounceInOut::process(float t) {
    if (t < 0.5f) return bounceIn(t * 2) * 0.5f;
    return bounceOut(t * 2 - 1) * 0.5f + 0.5f;
}

// Elastic
float EasingElasticIn::process(float t) {
    const float pi2 = M_PI * 2.0f;
    if (lround(t * 1000000) == 0 || lround(t * 1000000) == 1000000) return t;
    float s = 0.3f / pi2 * asin(1);
    t -= 1;
    return -(pow(2, 10 * t) * sin((t - s) * pi2 / 0.3f));
}

float EasingElasticOut::process(float t) {
    const float pi2 = M_PI * 2;
    if (lround(t * 1000000) == 0 || lround(t * 1000000) == 1000000) return t;
    float s = 0.3f / pi2 * asin(1);
    return pow(2, -10 * t) * sin((t - s) * pi2 / 0.3f) + 1;
}

float EasingElasticInOut::process(float t) {
    const float pi2 = M_PI * 2;
    float s = 0.45f / pi2 * asin(1);
    t *= 2;
    if (t < 1) {
        t -= 1;
        return -0.5f * (pow(2, 10 * t) * sin((t - s) * pi2 / 0.45f));
    } else {
        t -= 1;
        return pow(2, -10 * t) * sin((t - s) * pi2 / 0.45f) * 0.5f + 1;
    }
}




shared_ptr<EasingFunc> getEasingFunc(Easing easing) {
    switch (easing) {
        // Quad
        case Easing::QuadIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuadIn>());
        case Easing::QuadOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuadOut>());
        case Easing::QuadInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuadInOut>());
        // Cubic
        case Easing::CubicIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingCubicIn>());
        case Easing::CubicOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingCubicOut>());
        case Easing::CubicInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingCubicInOut>());
        // Quart
        case Easing::QuartIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuartIn>());
        case Easing::QuartOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuartOut>());
        case Easing::QuartInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuartInOut>());
        // Quint
        case Easing::QuintIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuintIn>());
        case Easing::QuintOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuintOut>());
        case Easing::QuintInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingQuintInOut>());
        // Sine
        case Easing::SineIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingSineIn>());
        case Easing::SineOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingSineOut>());
        case Easing::SineInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingSineInOut>());
        // Back
        case Easing::BackIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingBackIn>());
        case Easing::BackOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingBackOut>());
        case Easing::BackInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingBackInOut>());
        // Circ
        case Easing::CircIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingCircIn>());
        case Easing::CircOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingCircOut>());
        case Easing::CircInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingCircInOut>());
        // Bounce
        case Easing::BounceIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingBounceIn>());
        case Easing::BounceOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingBounceOut>());
        case Easing::BounceInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingBounceInOut>());
        // Elastic
        case Easing::ElasticIn:
            return static_pointer_cast<EasingFunc>(make_shared<EasingElasticIn>());
        case Easing::ElasticOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingElasticOut>());
        case Easing::ElasticInOut:
            return static_pointer_cast<EasingFunc>(make_shared<EasingElasticInOut>());
        // Linear
        case Easing::Linear:
        default:
            return static_pointer_cast<EasingFunc>(make_shared<EasingLinear>());
    }
}

unsigned int Tween::tweenIdCounter = 0;

Tween::Tween() {
    this->tweenId = ++Tween::tweenIdCounter;
}

Tween::Tween(float start, float end, float duration, Easing easing,
             LoopType loopType, int loopCount, float delayTime) {
    this->delayTime = delayTime;
    this->tmpDelayTime = delayTime;
    this->startValue = start;
    this->endValue = end;
    this->duration = duration;
    this->easing = easing;
    this->easingFunc = getEasingFunc(this->easing);
    this->loopType = loopType;
    this->loopCount = loopCount;
    this->tweenId = ++Tween::tweenIdCounter;
}

void Tween::init() {
    this->tmpDelayTime = this->delayTime;
    this->elapsedTime = 0;
    this->currentCount = 0;
    this->started = false;
    this->pausing = false;
}

unsigned int Tween::getTweenId() {
    return this->tweenId;
}

float Tween::currentValue(float start, float end, float percent) {
    return start + (percent * (end - start));
}

void Tween::update(float delta, const shared_ptr<Entity> &entity) {
    if (this->pausing) return;
    
    if (!this->started) {
        if (this->tmpDelayTime > 0) {
            this->tmpDelayTime -= delta;
            if (this->tmpDelayTime > 0) {
                return;
            }
        }
        this->started = true;
        this->onStart(entity);
        delta = 0;
    }
    
    this->elapsedTime += delta;
    if (this->elapsedTime >= this->duration) {
        if (this->loopType == LoopType::PingPong && this->currentCount % 2 == 1) {
            this->onModify(this->startValue, entity);
        } else {
            this->onModify(this->endValue, entity);
        }

        if (this->loopType == LoopType::None || (this->loopCount > 0 && this->currentCount + 1 >= this->loopCount)) {
            this->onFinish(entity);
            if (this->onFinishEventsForParent.size() > 0) {
                for (auto ev : this->onFinishEventsForParent) {
                    ev(shared_from_this());
                }
            }
            
        } else {
            this->elapsedTime = 0;
            this->currentCount++;
            this->onRestart(entity);
        }
        return;
    }
    
    float percent = this->easingFunc->process(this->elapsedTime / this->duration);
    if (this->loopType == LoopType::PingPong && this->currentCount % 2 == 1) {
        percent = 1.0 - percent;
    }
    float current = this->currentValue(this->startValue, this->endValue, percent);
    
    this->onModify(current, entity);
}

void Tween::pause() {
    this->pausing = true;
}

void Tween::resume() {
    this->pausing = false;
}

void Tween::addOnFinishEventForParent(function<void(const shared_ptr<Tween> &m)> callback) {
    this->onFinishEventsForParent.emplace_back(callback);
}


void Tween::setOnStartEvent(function<void(const shared_ptr<Entity> &e)> callback) {
    this->onStartEvent = callback;
}

void Tween::setOnRestartEvent(function<void(const shared_ptr<Entity> &e)> callback) {
    this->onRestartEvent = callback;
}

void Tween::setOnFinishEvent(function<void(const shared_ptr<Entity> &e)> callback) {
    this->onFinishEvent = callback;
}

void Tween::onStart(const shared_ptr<Entity> &entity) {
    if (this->onStartEvent) {
        this->onStartEvent(entity);
    }
}

void Tween::onRestart(const shared_ptr<Entity> &entity) {
    if (this->onRestartEvent) {
        this->onRestartEvent(entity);
    }
}

void Tween::onFinish(const shared_ptr<Entity> &entity) {
    if (this->onFinishEvent) {
        this->onFinishEvent(entity);
    }
}


shared_ptr<TweenMove> TweenMove::create(const Point &start, const Point &end, float duration, Easing easing,
                                        LoopType loopType, int loopCount, float delayTime) {
    return shared_ptr<TweenMove>(new TweenMove(start, end, duration, easing, loopType, loopCount, delayTime));
}

TweenMove::TweenMove(const Point &start, const Point &end, float duration, Easing easing,
                     LoopType loopType, int loopCount, float delayTime)
: Tween(0, 1.0, duration, easing, loopType, loopCount, delayTime), startPoint(start), endPoint(end) {
}

void TweenMove::onModify(float currentValue, const shared_ptr<Entity> &entity) {
    auto p = this->startPoint + (this->endPoint - this->startPoint) * currentValue;
    entity->setPosition(p);
}


shared_ptr<TweenAlpha> TweenAlpha::create(float start, float end, float duration, Easing easing,
                                          LoopType loopType, int loopCount, float delayTime) {
    return shared_ptr<TweenAlpha>(new TweenAlpha(start, end, duration, easing, loopType, loopCount, delayTime));
}

TweenAlpha::TweenAlpha(float start, float end, float duration, Easing easing,
                       LoopType loopType, int loopCount, float delayTime)
: Tween(start, end, duration, easing, loopType, loopCount, delayTime) {
}

void TweenAlpha::onModify(float currentValue, const shared_ptr<Entity> &entity) {
    auto e = static_pointer_cast<Entity>(entity);
    Color color = e->getColor();
    color.a = currentValue;
    e->setColor(color);
}


shared_ptr<TweenColor> TweenColor::create(const Color &start, const Color &end, float duration, Easing easing,
                                          LoopType loopType, int loopCount, float delayTime) {
    return shared_ptr<TweenColor>(new TweenColor(start, end, duration, easing, loopType, loopCount, delayTime));
}

TweenColor::TweenColor(const Color &start, const Color &end, float duration, Easing easing,
                       LoopType loopType, int loopCount, float delayTime)
: Tween(0, 1.0, duration, easing, loopType, loopCount, delayTime), startColor(start), endColor(end) {
}

void TweenColor::onModify(float currentValue, const shared_ptr<Entity> &entity) {
    auto e = static_pointer_cast<Entity>(entity);

    float r = this->startColor.r + (this->endColor.r - this->startColor.r) * currentValue;
    float g = this->startColor.g + (this->endColor.g - this->startColor.g) * currentValue;
    float b = this->startColor.b + (this->endColor.b - this->startColor.b) * currentValue;
    float a = this->startColor.a + (this->endColor.a - this->startColor.a) * currentValue;
    
    e->setColor(Color(r, g, b, a));
}


shared_ptr<TweenScale> TweenScale::create(float start, float end, float duration, Easing easing,
                                          LoopType loopType, int loopCount, float delayTime) {
    return shared_ptr<TweenScale>(new TweenScale(start, end, duration, easing, loopType, loopCount, delayTime));
}

shared_ptr<TweenScale> TweenScale::create(const Point &start, const Point &end, float duration, Easing easing,
                                          LoopType loopType, int loopCount, float delayTime) {
    return shared_ptr<TweenScale>(new TweenScale(start, end, duration, easing, loopType, loopCount, delayTime));
}

TweenScale::TweenScale(float start, float end, float duration, Easing easing,
                       LoopType loopType, int loopCount, float delayTime)
: Tween(0, 1.0f, duration, easing, loopType, loopCount, delayTime) {
    this->startScale = Point(start, start);
    this->endScale = Point(end, end);
}

TweenScale::TweenScale(const Point &start, const Point &end, float duration, Easing easing,
                       LoopType loopType, int loopCount, float delayTime)
: Tween(0, 1.0f, duration, easing, loopType, loopCount, delayTime) {
    this->startScale = start;
    this->endScale = end;
}

void TweenScale::onModify(float currentValue, const shared_ptr<Entity> &entity) {
    auto s = this->startScale + (this->endScale - this->startScale) * currentValue;
    entity->setScale(s);
}


shared_ptr<TweenRotate> TweenRotate::create(float start, float end, float duration, Easing easing,
                                            LoopType loopType, int loopCount, float delayTime) {
    return shared_ptr<TweenRotate>(new TweenRotate(start, end, duration, easing, loopType, loopCount, delayTime));
}

TweenRotate::TweenRotate(float start, float end, float duration, Easing easing,
                         LoopType loopType, int loopCount, float delayTime)
: Tween(start, end, duration, easing, loopType, loopCount, delayTime) {
}

void TweenRotate::onModify(float currentValue, const shared_ptr<Entity> &entity) {
    entity->setRotation(currentValue);
}



shared_ptr<TweenValue> TweenValue::create(float start, float end, float duration, Easing easing,
                                          LoopType loopType, int loopCount, float delayTime) {
    return shared_ptr<TweenValue>(new TweenValue(start, end, duration, easing, loopType, loopCount, delayTime));
}

TweenValue::TweenValue(float start, float end, float duration, Easing easing,
                       LoopType loopType, int loopCount, float delayTime)
: Tween(start, end, duration, easing, loopType, loopCount, delayTime) {
}

void TweenValue::onModify(float currentValue, const shared_ptr<Entity> &entity) {
    if (this->onModifyEvent) {
        this->onModifyEvent(currentValue, entity);
    }
}

void TweenValue::setOnModifyEvent(function<void(float value, const shared_ptr<Entity> &e)> callback) {
    this->onModifyEvent = callback;
}



shared_ptr<TweenUpdate> TweenUpdate::create() {
    return shared_ptr<TweenUpdate>(new TweenUpdate());
}

TweenUpdate::TweenUpdate() {
}

void TweenUpdate::setOnModifyEvent(function<void(float delta, const shared_ptr<Entity> &e)> callback) {
    this->onModifyEvent = callback;
}

void TweenUpdate::update(float delta, const shared_ptr<Entity> &entity) {
    if (this->onModifyEvent) {
        this->onModifyEvent(delta, entity);
    }
}

void TweenUpdate::finish() {
    if (this->onFinishEventsForParent.size() > 0) {
        for (auto ev : this->onFinishEventsForParent) {
            ev(shared_from_this());
        }
    }
}

void TweenGroup::init() {
    for (const auto &tween : this->tweens) {
        tween->init();
    }
}

void TweenGroup::addTweenOne(const shared_ptr<Tween> &tween) {
    this->tweens.emplace_back(tween);
}

void TweenGroup::update(float delta, const shared_ptr<Entity> &entity) {
}

void TweenGroup::onModify(float currentValue, const shared_ptr<Entity> &entity) {
}


TweenConcurrentGroup::TweenConcurrentGroup() {
}

void TweenConcurrentGroup::addTweenOne(const shared_ptr<Tween> &tween) {
    TweenGroup::addTweenOne(tween);
    tween->addOnFinishEventForParent([this](const shared_ptr<Tween> &m) {
        this->tweensToRemove.emplace_back(m);
    });
}

void TweenConcurrentGroup::update(float delta, const shared_ptr<Entity> &entity) {
    for (const auto &tween : this->tweens) {
        tween->update(delta, entity);
    }
    
    if (this->tweensToRemove.size() > 0) {
        for (const auto &tween : this->tweensToRemove) {
            this->tweens.erase(remove(this->tweens.begin(), this->tweens.end(), tween), this->tweens.end());
            if (this->tweens.size() == 0) {
                if (this->onFinishEvent) {
                    this->onFinishEvent(entity);
                }
                if (this->onFinishEventsForParent.size() > 0) {
                    for (auto ev : this->onFinishEventsForParent) {
                        ev(shared_from_this());
                    }
                }
            }
        }
        this->tweensToRemove.clear();
    }
}

void TweenConcurrentGroup::pause() {
    for (const auto &tween : this->tweens) {
        tween->pause();
    }
}

void TweenConcurrentGroup::resume() {
    for (const auto &tween : this->tweens) {
        tween->resume();
    }
}

TweenSequenceGroup::TweenSequenceGroup() {
}

void TweenSequenceGroup::addTweenOne(const shared_ptr<Tween> &tween) {
    TweenGroup::addTweenOne(tween);
    
    if (this->tweens.size() == 1) {
        tween->addOnFinishEventForParent([this](const shared_ptr<Tween> &t) {
            this->next = true;
        });
    }
    tween->addOnFinishEventForParent([this](const shared_ptr<Tween> &t) {
        this->next = true;
    });
}

void TweenSequenceGroup::update(float delta, const shared_ptr<Entity> &entity) {
    this->tweens[this->currentIndex]->update(delta, entity);
    
    if (this->next) {
        this->currentIndex++;
        
        if (this->currentIndex >= this->tweens.size()) {
            if (this->onFinishEvent) {
                this->onFinishEvent(entity);
            }
            if (this->onFinishEventsForParent.size() > 0) {
                for (auto ev : this->onFinishEventsForParent) {
                    ev(shared_from_this());
                }
            }
            
        } else if (this->currentIndex < this->tweens.size()) {
            auto tween = this->tweens[this->currentIndex];
            tween->init();
        }
        this->next = false;
    }
}

void TweenSequenceGroup::pause() {
    if (this->tweens.size() == 0) return;
    this->tweens[0]->pause();
}

void TweenSequenceGroup::resume() {
    if (this->tweens.size() == 0) return;
    this->tweens[0]->resume();
}


TweenJoinGroup::TweenJoinGroup() {
}

void TweenJoinGroup::addTweenOne(const shared_ptr<Tween> &tween) {
    TweenGroup::addTweenOne(tween);
    tween->addOnFinishEventForParent([this](const shared_ptr<Tween> &t) {
        this->tweensToRemove.emplace_back(t);
    });
}

void TweenJoinGroup::update(float delta, const shared_ptr<Entity> &entity) {
    if (this->tweensToRemove.size() > 0) {
        for (const auto &tween : this->tweensToRemove) {
            this->tweens.erase(remove(this->tweens.begin(), this->tweens.end(), tween), this->tweens.end());
            if (this->tweens.size() == 0) {
                if (this->onFinishEvent) {
                    this->onFinishEvent(entity);
                }
                if (this->onFinishEventsForParent.size() > 0) {
                    for (auto ev : this->onFinishEventsForParent) {
                        ev(shared_from_this());
                    }
                }
            }
        }
        this->tweensToRemove.clear();
    }
}


shared_ptr<TweenDelay> TweenDelay::create(float delayTime) {
    return shared_ptr<TweenDelay>(new TweenDelay(delayTime));
}

shared_ptr<TweenDelay> TweenDelay::create(float delayTime, function<void(const shared_ptr<Entity> &e)> callback) {
    auto delay = shared_ptr<TweenDelay>(new TweenDelay(delayTime));
    delay->setOnFinishEvent(callback);
    return delay;
}

TweenDelay::TweenDelay(float delayTime) : Tween(0, 0, 0, Easing::Linear, LoopType::None, 0, delayTime) {
}

