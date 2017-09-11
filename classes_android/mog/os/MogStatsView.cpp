#include "mog/os/MogStatsView.h"
#include <jni.h>
#include "mog/os/AndroidHelper.h"

using namespace mog;

bool MogStatsView::enabled = false;

bool MogStatsView::isEnabled() {
    return MogStatsView::enabled;
}

void MogStatsView::setStats(float delta, int drawCall, int instants) {
    if (!MogStatsView::enabled) return;
    AndroidHelper::runOnUiThread([delta, drawCall, instants]() {
        auto statsView = Engine::getInstance()->getNativeObject("STATS_VIEW");
        statsView->execute("setStats", NArg(delta), NArg(drawCall), NArg(instants));
    });
}

void MogStatsView::setEnable(bool enable) {
    if (MogStatsView::enabled == enable) return;
    AndroidHelper::runOnUiThread([enable]() {
        if (enable) {
            auto statsView = NativeClass::create("org.mog2d.MogStatsView")->newInstance(NArg(AndroidHelper::getActivity()));
            NRet layout = AndroidHelper::getActivity()->execute("getLayout", NRet::Type::Object);
            layout.o->execute("addView", NArg(statsView));
            Engine::getInstance()->setNativeObject("STATS_VIEW", statsView);
        } else {
            auto statsView = Engine::getInstance()->getNativeObject("STATS_VIEW");
            NRet layout = AndroidHelper::getActivity()->execute("getLayout", NRet::Type::Object);
            layout.o->execute("removeView", NArg(statsView));
            Engine::getInstance()->removeNativeObject("STATS_VIEW");
        }
    });
    MogStatsView::enabled = enable;
}

void MogStatsView::setAlignment(Alignment alignment) {
    if (!MogStatsView::enabled) return;
    AndroidHelper::runOnUiThread([alignment]() {
        auto values = NativeClass::create("org.mog2d.MogStatsView.Alignment")->execute("values");
        jobjectArray valuesArr = (jobjectArray)values.o->getObject();
        jobject value = AndroidHelper::getEnv()->GetObjectArrayElement(valuesArr, (int)alignment);
        auto statsView = Engine::getInstance()->getNativeObject("STATS_VIEW");
        statsView->execute("setAlignment", NArg(NativeObject::create(value)));
        AndroidHelper::getEnv()->DeleteLocalRef(valuesArr);
        AndroidHelper::getEnv()->DeleteLocalRef(value);
    });
}
