#include "mog/core/EntityCreator.h"
#include "mog/base/Rectangle.h"
#include "mog/base/RoundedRectangle.h"
#include "mog/base/Circle.h"
#include "mog/base/Label.h"
#include "mog/base/Line.h"
#include "mog/base/Sprite.h"
#include "mog/base/SpriteSheet.h"
#include "mog/base/Slice9Sprite.h"
#include "mog/base/TiledSprite.h"
#include "mog/base/Group.h"
#include "mog/base/ScrollGroup.h"

using namespace mog;

std::shared_ptr<Entity> EntityCreator::create(EntityType entityType, const std::shared_ptr<Dictionary> &dict) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> emptyParams;
    return create(entityType, dict, emptyParams);
}

std::shared_ptr<Entity> EntityCreator::create(EntityType entityType, const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    std::shared_ptr<Entity> entity = nullptr;
    switch (entityType) {
        case EntityType::Rectangle:
            entity = std::shared_ptr<Rectangle>(new Rectangle());
            break;
            
        case EntityType::RoundedRectangle:
            entity = std::shared_ptr<RoundedRectangle>(new RoundedRectangle());
            break;

        case EntityType::Circle:
            entity = std::shared_ptr<Circle>(new Circle());
            break;

        case EntityType::Label:
            entity = std::shared_ptr<Label>(new Label());
            break;

        case EntityType::Line:
            entity = std::shared_ptr<Line>(new Line());
            break;

        case EntityType::Sprite:
            entity = std::shared_ptr<Sprite>(new Sprite());
            break;

        case EntityType::SpriteSheet:
            entity = std::shared_ptr<SpriteSheet>(new SpriteSheet());
            break;

        case EntityType::Slice9Sprite:
            entity = std::shared_ptr<Slice9Sprite>(new Slice9Sprite());
            break;

        case EntityType::TiledSprite:
            entity = std::shared_ptr<TiledSprite>(new TiledSprite());
            break;

        case EntityType::Group:
            entity = std::shared_ptr<Group>(new Group());
            break;

        case EntityType::ScrollGroup:
            entity = std::shared_ptr<ScrollGroup>(new ScrollGroup());
            break;
    }
    
    entity->deserializeData(dict, params);
    entity->init();
    
    return entity;
}
