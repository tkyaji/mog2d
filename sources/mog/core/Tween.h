#ifndef Tween_h
#define Tween_h

#include <memory>
#include <vector>
#include <functional>
#include "mog/core/plain_objects.h"

namespace mog {
    
    class Drawable;
    
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
        static std::shared_ptr<EasingFunc> getEasingFunc(Easing easing);
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
    
    
    class Tween : public std::enable_shared_from_this<Tween> {
    public:
        virtual void setOnStartEvent(std::function<void(const std::shared_ptr<Drawable> &d)> callback);
        virtual void setOnRestartEvent(std::function<void(const std::shared_ptr<Drawable> &d)> callback);
        virtual void setOnFinishEvent(std::function<void(const std::shared_ptr<Drawable> &d)> callback);
        
        virtual void init();
        virtual void update(float delta, const std::shared_ptr<Drawable> &drawable);
        virtual void pause();
        virtual void resume();
        void addOnFinishEventForParent(std::function<void(const std::shared_ptr<Tween> &m)> callback);
        
        unsigned int getTweenId();
        
    protected:
        static unsigned int tweenIdCounter;
        
        Tween();
        Tween(float start, float end, float duration, Easing easing = Easing::Linear,
              LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        std::shared_ptr<Drawable> drawable = nullptr;
        float delayTime = 0;
        float startValue = 0;
        float endValue = 0;
        float duration = 0;
        Easing easing = Easing::Linear;
        LoopType loopType = LoopType::None;
        std::shared_ptr<EasingFunc> easingFunc = nullptr;
        bool started = false;
        bool pausing = false;
        int loopCount = 0;
        int currentCount = 0;
        
        std::function<void(const std::shared_ptr<Drawable> &d)> onStartEvent;
        std::function<void(const std::shared_ptr<Drawable> &d)> onRestartEvent;
        std::function<void(const std::shared_ptr<Drawable> &d)> onFinishEvent;
        
        std::vector<std::function<void(const std::shared_ptr<Tween> &m)>> onFinishEventsForParent;
        
        float currentValue(float start, float end, float percent);
        
        virtual void onStart(const std::shared_ptr<Drawable> &drawable);
        virtual void onRestart(const std::shared_ptr<Drawable> &drawable);
        virtual void onFinish(const std::shared_ptr<Drawable> &drawable);
        virtual void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable) = 0;
        
    private:
        unsigned int tweenId = 0;
        float tmpDelayTime = 0;
        float elapsedTime = 0;
    };
    
    
    class TweenMove : public Tween {
    public:
        static std::shared_ptr<TweenMove> create(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                                                 LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenMove(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                  LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        Point startPoint = Point::zero;
        Point endPoint = Point::zero;
        
        virtual void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable);
    };
    
    
    class TweenScale : public Tween {
    public:
        static std::shared_ptr<TweenScale> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                                  LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        static std::shared_ptr<TweenScale> create(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                                                  LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenScale(float start, float end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        TweenScale(const Point &start, const Point &end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        Point startScale = Point::zero;
        Point endScale = Point::zero;
        
        void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable);
    };
    
    
    class TweenRotate : public Tween {
    public:
        static std::shared_ptr<TweenRotate> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenRotate(float start, float end, float duration, Easing easing = Easing::Linear,
                    LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable);
    };
    
    
    class TweenAlpha : public Tween {
    public:
        static std::shared_ptr<TweenAlpha> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                                  LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenAlpha(float start, float end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable);
    };
    
    
    class TweenColor : public Tween {
    public:
        static std::shared_ptr<TweenColor> create(const Color &start, const Color &end, float duration, Easing easing = Easing::Linear,
                                                  LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
    protected:
        TweenColor(const Color &start, const Color &end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable);
        
        Color startColor;
        Color endColor;
    };
    
    
    class TweenValue : public Tween {
    public:
        static std::shared_ptr<TweenValue> create(float start, float end, float duration, Easing easing = Easing::Linear,
                                                  LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        virtual void setOnModifyEvent(std::function<void(float value, const std::shared_ptr<Drawable> &d)> callback);
        
    protected:
        TweenValue(float start, float end, float duration, Easing easing = Easing::Linear,
                   LoopType loopType = LoopType::None, int loopCount = 0, float delayTime = 0);
        
        virtual void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable);
        
        std::function<void(float value, const std::shared_ptr<Drawable> &d)> onModifyEvent;
    };
    
    
    class TweenUpdate : public Tween {
    public:
        static std::shared_ptr<TweenUpdate> create();
        
        virtual void setOnModifyEvent(std::function<void(float delta, const std::shared_ptr<Drawable> &d)> callback);
        virtual void update(float delta, const std::shared_ptr<Drawable> &drawable);
        
        void finish();
        
    protected:
        TweenUpdate();
        
        virtual void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable) {}
        
        std::function<void(float delta, const std::shared_ptr<Drawable> &d)> onModifyEvent;
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
        
        virtual void update(float delta, const std::shared_ptr<Drawable> &drawable) override;
        virtual void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable) override;
        virtual void addTweenOne(const std::shared_ptr<Tween> &tween);
        
        std::vector<std::shared_ptr<Tween>> tweens;
    };
    
    
    class TweenConcurrentGroup : public TweenGroup {
    public:
        template<class First, class... Rest>
        static std::shared_ptr<TweenConcurrentGroup> create(const First& first, const Rest&... rest) {
            auto m = std::shared_ptr<TweenConcurrentGroup>(new TweenConcurrentGroup());
            m->addTween(first, rest...);
            return m;
        }
        
        virtual void addTweenOne(const std::shared_ptr<Tween> &tween) override;
        virtual void update(float delta, const std::shared_ptr<Drawable> &drawable) override;
        virtual void pause() override;
        virtual void resume() override;
        
    protected:
        TweenConcurrentGroup();
        
        std::vector<std::shared_ptr<Tween>> tweensToRemove;
    };
    
    
    class TweenSequenceGroup : public TweenGroup {
    public:
        template<class First, class... Rest>
        static std::shared_ptr<TweenSequenceGroup> create(const First& first, const Rest&... rest) {
            auto m = std::shared_ptr<TweenSequenceGroup>(new TweenSequenceGroup());
            m->addTween(first, rest...);
            return m;
        }
        
        virtual void addTweenOne(const std::shared_ptr<Tween> &tween) override;
        virtual void update(float delta, const std::shared_ptr<Drawable> &drawable) override;
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
        static std::shared_ptr<TweenJoinGroup> create(const First& first, const Rest&... rest) {
            auto m = std::shared_ptr<TweenJoinGroup>(new TweenJoinGroup());
            m->addTween(first, rest...);
            return m;
        }
        
        virtual void addTweenOne(const std::shared_ptr<Tween> &tween) override;
        virtual void update(float delta, const std::shared_ptr<Drawable> &drawable) override;
        virtual void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable) override {}
        
    protected:
        TweenJoinGroup();
        
        std::vector<std::shared_ptr<Tween>> tweensToRemove;
    };
    
    
    class TweenDelay : public Tween {
    public:
        static std::shared_ptr<TweenDelay> create(float delayTime);
        static std::shared_ptr<TweenDelay> create(float delayTime, std::function<void(const std::shared_ptr<Drawable> &d)> callback);
        
        virtual void onModify(float currentValue, const std::shared_ptr<Drawable> &drawable) {}
        
    protected:
        TweenDelay(float delayTime);
    };
    
}

#endif /* Tween_h */

