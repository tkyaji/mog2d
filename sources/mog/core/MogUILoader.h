#ifndef MogUILoader_h
#define MogUILoader_h

#include <memory>
#include <string>
#include "mog/core/Data.h"
#include "mog/base/Entity.h"
#include "mog/base/Sprite.h"
#include "mog/base/Label.h"
#include "mog/base/SpriteSheet.h"
#include "mog/base/Circle.h"
#include "mog/base/Rectangle.h"
#include "mog/base/RoundedRectangle.h"
#include "mog/base/Group.h"
#include "mog/base/Slice9Sprite.h"

namespace mog {

    class MogUILoader {
    public:
        static std::shared_ptr<Group> load(std::string filename);

        static std::shared_ptr<List> serialize(const std::shared_ptr<Group> &group);
        static std::shared_ptr<Group> deserialize(const std::shared_ptr<List> &dataList);
    };
}

#endif /* MogUILoader_h */
