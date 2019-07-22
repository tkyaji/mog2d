#ifndef EntityCreator_h
#define EntityCreator_h

#include <memory>
#include <string>
#include "mog/base/Entity.h"

namespace mog {
    class EntityCreator {
    public:
        static std::shared_ptr<Entity> create(EntityType entityType, const std::shared_ptr<Dictionary> &dict);
        static std::shared_ptr<Entity> create(EntityType entityType, const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params);
    };
}

#endif /* EntityCreator_h */
