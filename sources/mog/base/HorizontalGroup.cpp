#include "mog/base/HorizontalGroup.h"

using namespace mog;

std::shared_ptr<HorizontalGroup> HorizontalGroup::create(float padding, bool enableBatching) {
    auto group = std::shared_ptr<HorizontalGroup>(new HorizontalGroup());
    group->enableBatching = enableBatching;
    group->padding = padding;
    group->init();
    return group;
}

HorizontalGroup::HorizontalGroup() {
    this->alignmentHorizontal = true;
}

std::shared_ptr<HorizontalGroup> HorizontalGroup::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<HorizontalGroup>(e);
}

std::shared_ptr<Dictionary> HorizontalGroup::serialize() {
    auto dict = AlignmentGroup::serialize();
    dict->put(PROP_KEY_ENTITY_TYPE, Int::create((int)EntityType::HorizontalGroup));
    return dict;
}
