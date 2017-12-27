#ifndef Tween_h
#define Tween_h

#include <memory>
#include <vector>
#include <functional>
#include "mog/core/plain_objects.h"
#include "mog/base/Entity.h"

namespace mog {
    
    class Entity;
    
    enum class Easing {
        // Linear
        Linear,
        // Quad
        QuadIn,
        QuadOut,
        QuadInOut,
        // Cubic
        CubicIn,
        CubicOut,
        CubicInOut,
        // Quart
        QuartIn,
        QuartOut,
        QuartInOut,
        // Quint
        QuintIn,
        QuintOut,
        QuintInOut,
        // Sine
        SineIn,
        SineOut,
        SineInOut,
        // Back
        BackIn,
        BackOut,
        BackInOut,
        // Circ
        CircIn,
        CircOut,
        CircInOut,
        // Bounce
        BounceOut,
        BounceIn,
        BounceInOut,
        // Elastic
        ElasticIn,
        ElasticOut,
        ElasticInOut,
    };
    
    enum class LoopType {
        None,
        Loop,
        PingPong,
    };
    
    
    class EasingFunc {
    public:
        virtual float process(float t) = 0;
    };
    // Linear
    class EasingLinear : public EasingFunc {
    public:
        float process(float t);
    };
    // Quad
    class EasingQuadIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingQuadOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingQuadInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Cubic
    class EasingCubicIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingCubicOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingCubicInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Quart
    class EasingQuartIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingQuartOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingQuartInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Quint
    class EasingQuintIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingQuintOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingQuintInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Sine
    class EasingSineIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingSineOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingSineInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Back
    class EasingBackIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingBackOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingBackInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Circ
    class EasingCircIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingCircOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingCircInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Bounce
    class EasingBounceIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingBounceOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingBounceInOut : public EasingFunc {
    public:
        float process(float t);
    };
    // Elastic
    class EasingElasticIn : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingElasticOut : public EasingFunc {
    public:
        float process(float t);
    };
    class EasingElasticInOut : public EasingFunc {
    public:
        float process(float t);
    };
    
    
    class Tween : public enable_shared_from_this<Tween> {
    public:
        virtual void setOnStartEvent(function<void(const shared_ptr<Entity> &e)> callback);
        virtual void setOnRestartEvent(function<void(const shared_ptr<Entity> &e)> callback);
        virtual void setOnFinishEvent(function<void(const shared_ptr<Entity> &e)> callback);

        virtual void init();
        virtual void update(float delta, const shared_ptr<Entity> &entity);
        virtual void pause();
        virtual void resume();
        void addOnFinishEventForParent(function<void(const shared_ptr<Tween> &m)> callback);
        
        unsigned int getTweenId();
        
    protected:
        static unsigned int tweenIdCounter;
        
        Tween();
        Tween(float start, float end, float duration, Easing easing = Easing::Linear,
              LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        shared_ptr<Entity> entity = nullptr;
        float delayTime = 0;
        float startValue = 0;
        float endValue = 0;
        float duration = 0;
        Easing easing = Easing::Linear;
        LoopType loopType = LoopType::None;
        shared_ptr<EasingFunc> easingFunc = nullptr;
        bool started = false;
        bool pausing = false;
        int loopCount = 0;
        int currentCount = 0;

        function<void(const shared_ptr<Entity> &e)> onStartEvent;
        function<void(const shared_ptr<Entity> &e)> onRestartEvent;
        function<void(const shared_ptr<Entity> &e)> onFinishEvent;
        
        vector<function<void(const shared_ptr<Tween> &m)>> onFinishEventsForParent;

        float currentValue(float start, float end, float percent);
        
        virtual void onStart(const shared_ptr<Entity> &entity);
        virtual void onRestart(const shared_ptr<Entity> &entity);
        virtual void onFinish(const shared_ptr<Entity> &entity);
        virtual void onModify(float currentValue, const shared_ptr<Entity> &entity) = 0;
        
    private:
        unsigned int tweenId = 0;
        float tmpDelayTime = 0;
        float elapsedTime = 0;
    };
    
    
    class TweenMove : public Tween {
    public:
        static shared_ptr<TweenMove> create(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                                            LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenMove(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                  LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        Point startPoint = Point::zero;
        Point endPoint = Point::zero;

        virtual void onModify(float currentValue, const shared_ptr<Entity> &entity);
    };
    
    
    class TweenScale : public Tween {
    public:
        static shared_ptr<TweenScale> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                             LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        static shared_ptr<TweenScale> create(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                                             LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenScale(float start, float end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        TweenScale(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        Point startScale = Point::zero;
        Point endScale = Point::zero;
        
        void onModify(float currentValue, const shared_ptr<Entity> &entity);
    };
    
    
    class TweenRotate : public Tween {
    public:
        static shared_ptr<TweenRotate> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                              LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenRotate(float start, float end, float duration, Easing easing = Easing::Linear,
                    LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        void onModify(float currentValue, const shared_ptr<Entity> &entity);
    };

    
    class TweenAlpha : public Tween {
    public:
        static shared_ptr<TweenAlpha> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                             LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenAlpha(float start, float end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        void onModify(float currentValue, const shared_ptr<Entity> &entity);
    };
    
    
    class TweenColor : public Tween {
    public:
        static shared_ptr<TweenColor> create(const Color &start, const Color &end, float duration, Easing easing = Easing::Linear,
                                             LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenColor(const Color &start, const Color &end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        void onModify(float currentValue, const shared_ptr<Entity> &entity);
        
        Color startColor;
        Color endColor;
    };

    
    class TweenValue : public Tween {
    public:
        static shared_ptr<TweenValue> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                             LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        virtual void setOnModifyEvent(function<void(float value, const shared_ptr<Entity> &e)> callback);

    protected:
        TweenValue(float start, float end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);

        virtual void onModify(float currentValue, const shared_ptr<Entity> &entity);

        function<void(float value, const shared_ptr<Entity> &e)> onModifyEvent;
    };

    
    class TweenUpdate : public Tween {
    public:
        static shared_ptr<TweenUpdate> create();
        
        virtual void setOnModifyEvent(function<void(float delta, const shared_ptr<Entity> &e)> callback);
        virtual void update(float delta, const shared_ptr<Entity> &entity);
        
        void finish();

    protected:
        TweenUpdate();

        virtual void onModify(float currentValue, const shared_ptr<Entity> &entity) {}

        function<void(float delta, const shared_ptr<Entity> &e)> onModifyEvent;
    };
    
    
    class TweenGroup : public Tween {
    public:
        template<class First, class... Rest>
        void addTween(const First &first, const Rest&... rest) {
            this->addTweenOne(first);
            this->addTween(rest...);
        }
        void addTween() {}

        virtual void init() override;

    protected:
        TweenGroup() {}

        virtual void update(float delta, const shared_ptr<Entity> &entity) override;
        virtual void onModify(float currentValue, const shared_ptr<Entity> &entity) override;
        virtual void addTweenOne(const shared_ptr<Tween> &tween);

        std::vector<shared_ptr<Tween>> tweens;
    };
    
    
    class TweenConcurrentGroup : public TweenGroup {
    public:
        template<class First, class... Rest>
        static shared_ptr<TweenConcurrentGroup> create(const First& first, const Rest&... rest) {
            auto m = shared_ptr<TweenConcurrentGroup>(new TweenConcurrentGroup());
            m->addTween(first, rest...);
            return m;
        }

        virtual void addTweenOne(const shared_ptr<Tween> &tween) override;
        virtual void update(float delta, const shared_ptr<Entity> &entity) override;
        virtual void pause() override;
        virtual void resume() override;
        
    protected:
        TweenConcurrentGroup();
        
        std::vector<shared_ptr<Tween>> tweensToRemove;
    };
    
    
    class TweenSequenceGroup : public TweenGroup {
    public:
        template<class First, class... Rest>
        static shared_ptr<TweenSequenceGroup> create(const First& first, const Rest&... rest) {
            auto m = shared_ptr<TweenSequenceGroup>(new TweenSequenceGroup());
            m->addTween(first, rest...);
            return m;
        }
        
        virtual void addTweenOne(const shared_ptr<Tween> &tween) override;
        virtual void update(float delta, const shared_ptr<Entity> &entity) override;
        virtual void pause() override;
        virtual void resume() override;
        
    protected:
        TweenSequenceGroup();
        
        int currentIndex = 0;
        bool next = false;
    };
    
    
    class TweenJoinGroup : public TweenGroup {
    public:
        template<class First, class... Rest>
        static shared_ptr<TweenJoinGroup> create(const First& first, const Rest&... rest) {
            auto m = shared_ptr<TweenJoinGroup>(new TweenJoinGroup());
            m->addTween(first, rest...);
            return m;
        }

        virtual void addTweenOne(const shared_ptr<Tween> &tween) override;
        virtual void update(float delta, const shared_ptr<Entity> &entity) override;
        virtual void onModify(float currentValue, const shared_ptr<Entity> &entity) override {}
        
    protected:
        TweenJoinGroup();
        
        std::vector<shared_ptr<Tween>> tweensToRemove;
    };
    
    
    class TweenDelay : public Tween {
    public:
        static shared_ptr<TweenDelay> create(float delayTime);
        static shared_ptr<TweenDelay> create(float delayTime, function<void(const shared_ptr<Entity> &e)> callback);

        virtual void onModify(float currentValue, const shared_ptr<Entity> &entity) {}
        
    protected:
        TweenDelay(float delayTime);
    };
    
}

#endif /* Tween_h */
