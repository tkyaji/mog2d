#ifndef EntityCreator_h
#define EntityCreator_h

#include <memory>
#include <string>
#include "mog/base/Entity.h"

namespace mog {
    enum class EntityType {
        Rectangle,
        RoundedRectangle,
        Circle,
        Label,
        Line,
        Sprite,
        SpriteSheet,
        Slice9Sprite,
        TiledSprite,
        Group,
        ScrollGroup,
    };
    
    class EntityCreator {
    public:
        static std::shared_ptr<Entity> create(EntityType entityType, const std::shared_ptr<Dictionary> &dict);
    };
}

#endif /* EntityCreator_h */
