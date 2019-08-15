#include "mog/base/VerticalGroup.h"

using namespace mog;

std::shared_ptr<VerticalGroup> VerticalGroup::create(float padding, bool enableBatching) {
    auto group = std::shared_ptr<VerticalGroup>(new VerticalGroup());
    group->enableBatching = enableBatching;
    group->padding = padding;
    group->init();
    return group;
}

VerticalGroup::VerticalGroup() {
    this->alignmentVertical = true;
}

std::shared_ptr<VerticalGroup> VerticalGroup::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<VerticalGroup>(e);
}

std::shared_ptr<Dictionary> VerticalGroup::serialize() {
    auto dict = AlignmentGroup::serialize();
    dict->put(PROP_KEY_ENTITY_TYPE, Int::create((int)EntityType::VerticalGroup));
    return dict;
}
