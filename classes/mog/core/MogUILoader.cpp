#include "mog/core/MogUILoader.h"
#include "mog/core/FileUtils.h"
#include "mog/core/DataStore.h"

using namespace std;
using namespace mog;

const std::string MogUILoader::PropertyNames::EntityType = "entity_type";
const std::string MogUILoader::PropertyNames::Name = "name";
const std::string MogUILoader::PropertyNames::Tag = "tag";
const std::string MogUILoader::PropertyNames::PositionX = "position_x";
const std::string MogUILoader::PropertyNames::PositionY = "position_y";
const std::string MogUILoader::PropertyNames::Width = "width";
const std::string MogUILoader::PropertyNames::Height = "height";
const std::string MogUILoader::PropertyNames::Rotation = "rotation";
const std::string MogUILoader::PropertyNames::ScaleX = "scale_x";
const std::string MogUILoader::PropertyNames::ScaleY = "scale_y";
const std::string MogUILoader::PropertyNames::AnchorX = "anchor_x";
const std::string MogUILoader::PropertyNames::AnchorY = "anchor_y";
const std::string MogUILoader::PropertyNames::OriginX = "origin_x";
const std::string MogUILoader::PropertyNames::OriginY = "origin_y";
const std::string MogUILoader::PropertyNames::ZIndex = "z_index";
const std::string MogUILoader::PropertyNames::ColorR = "color_r";
const std::string MogUILoader::PropertyNames::ColorG = "color_g";
const std::string MogUILoader::PropertyNames::ColorB = "color_b";
const std::string MogUILoader::PropertyNames::ColorA = "color_a";
const std::string MogUILoader::PropertyNames::CornerRadius = "corner_radius";
const std::string MogUILoader::PropertyNames::Radius = "radius";
const std::string MogUILoader::PropertyNames::Text = "text";
const std::string MogUILoader::PropertyNames::FontFilename = "font_filename";
const std::string MogUILoader::PropertyNames::FontSize = "font_size";
const std::string MogUILoader::PropertyNames::FontHeight = "font_height";
const std::string MogUILoader::PropertyNames::Filename = "filename";
const std::string MogUILoader::PropertyNames::RectX = "rect_x";
const std::string MogUILoader::PropertyNames::RectY = "rect_y";
const std::string MogUILoader::PropertyNames::RectWidth = "rect_width";
const std::string MogUILoader::PropertyNames::RectHeight = "rect_height";
const std::string MogUILoader::PropertyNames::CenterRectX = "center_rect_x";
const std::string MogUILoader::PropertyNames::CenterRectY = "center_rect_y";
const std::string MogUILoader::PropertyNames::CenterRectWidth = "center_rect_width";
const std::string MogUILoader::PropertyNames::CenterRectHeight = "center_rect_height";
const std::string MogUILoader::PropertyNames::FrameWidth = "frame_width";
const std::string MogUILoader::PropertyNames::FrameHeight = "frame_height";
const std::string MogUILoader::PropertyNames::FrameCount = "frame_count";
const std::string MogUILoader::PropertyNames::Margin = "margin";
const std::string MogUILoader::PropertyNames::EnableBatching = "enableBatching";
const std::string MogUILoader::PropertyNames::ChildEntities = "childEntities";


std::shared_ptr<mog::Entity> MogUILoader::load(std::string filename) {
    unsigned char *data = nullptr;
    int len = 0;
    FileUtils::readBytesAsset(filename, &data, &len);
    
    auto uiDict = DataStore::deserialize<mog::Dictionary>(data, len);
    return deserialize(uiDict);
}

Dictionary MogUILoader::serialize(const std::shared_ptr<Entity> &entity) {
    Dictionary dict;
    
    dict.put(PropertyNames::EntityType, Int((int)entity->getEntityType()));
    dict.put(PropertyNames::Name, String(entity->getName()));
    dict.put(PropertyNames::Tag, String(entity->getTag()));
    dict.put(PropertyNames::PositionX, Float(entity->getPositionX()));
    dict.put(PropertyNames::PositionY, Float(entity->getPositionY()));
    dict.put(PropertyNames::Width, Float(entity->getWidth()));
    dict.put(PropertyNames::Height, Float(entity->getHeight()));
    dict.put(PropertyNames::Rotation, Float(entity->getRotation()));
    dict.put(PropertyNames::ScaleX, Float(entity->getScaleX()));
    dict.put(PropertyNames::ScaleY, Float(entity->getScaleX()));
    dict.put(PropertyNames::AnchorX, Float(entity->getAnchorX()));
    dict.put(PropertyNames::AnchorY, Float(entity->getAnchorY()));
    dict.put(PropertyNames::OriginX, Float(entity->getOriginX()));
    dict.put(PropertyNames::OriginY, Float(entity->getOriginY()));
    dict.put(PropertyNames::ZIndex, Float(entity->getZIndex()));
    dict.put(PropertyNames::ColorR, Float(entity->getColor().r));
    dict.put(PropertyNames::ColorG, Float(entity->getColor().g));
    dict.put(PropertyNames::ColorB, Float(entity->getColor().b));
    dict.put(PropertyNames::ColorA, Float(entity->getColor().a));
    
    auto entityType = entity->getEntityType();
    switch (entityType) {
        case EntityType::RoundedRectangle: {
            auto roundedRectangle = static_pointer_cast<RoundedRectangle>(entity);
            dict.put(PropertyNames::CornerRadius, Float(roundedRectangle->getCornerRadius()));
            break;
        }
        case EntityType::Circle: {
            auto circle = static_pointer_cast<Circle>(entity);
            dict.put(PropertyNames::Radius, Float(circle->getRadius()));
            break;
        }
        case EntityType::Label: {
            auto label = static_pointer_cast<Label>(entity);
            dict.put(PropertyNames::Text, String(label->getText()));
            dict.put(PropertyNames::FontFilename, String(label->getFontFilename()));
            dict.put(PropertyNames::FontSize, Float(label->getFontSize()));
            dict.put(PropertyNames::FontHeight, Float(label->getHeight()));
            break;
        }
        case EntityType::Sprite:
        case EntityType::Slice9Sprite:
        case EntityType::SpriteSheet: {
            auto sprite = static_pointer_cast<Sprite>(entity);
            dict.put(PropertyNames::Filename, String(sprite->getFilename()));
            dict.put(PropertyNames::RectX, Float(sprite->getRect().position.x));
            dict.put(PropertyNames::RectY, Float(sprite->getRect().position.y));
            dict.put(PropertyNames::RectWidth, Float(sprite->getRect().size.width));
            dict.put(PropertyNames::RectHeight, Float(sprite->getRect().size.height));

            if (entityType == EntityType::Slice9Sprite) {
                auto slice9Sprite = static_pointer_cast<Slice9Sprite>(entity);
                dict.put(PropertyNames::CenterRectX, Float(slice9Sprite->getCenterRect().position.x));
                dict.put(PropertyNames::CenterRectY, Float(slice9Sprite->getCenterRect().position.y));
                dict.put(PropertyNames::CenterRectWidth, Float(slice9Sprite->getCenterRect().size.width));
                dict.put(PropertyNames::CenterRectHeight, Float(slice9Sprite->getCenterRect().size.height));

            } else if (entityType == EntityType::SpriteSheet) {
                auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
                dict.put(PropertyNames::FrameWidth, Float(spriteSheet->getFrameSize().width));
                dict.put(PropertyNames::FrameHeight, Float(spriteSheet->getFrameSize().height));
                dict.put(PropertyNames::FrameCount, Long(spriteSheet->getFrameCount()));
                dict.put(PropertyNames::Margin, Long(spriteSheet->getMargin()));
             }
            break;
        }
        case EntityType::Group: {
            auto group = static_pointer_cast<Group>(entity);
            dict.put(PropertyNames::EnableBatching, Bool(group->isEnableBatching()));
            Array arr;
            for (auto child : group->getChildEntities()) {
                auto childDict = serialize(child);
                arr.append(childDict);
            }
            dict.put(PropertyNames::ChildEntities, arr);
            break;
        }
        default:
            break;
    }

    return dict;
}

std::shared_ptr<Entity> MogUILoader::deserialize(const Dictionary &uiDict) {
    EntityType entityType = (EntityType)(uiDict.get<Int>(PropertyNames::EntityType).value);
    
    string name = uiDict.get<String>(PropertyNames::Name).value;
    string tag = uiDict.get<String>(PropertyNames::Tag).value;
    float positionX = uiDict.get<Float>(PropertyNames::PositionX).value;
    float positionY = uiDict.get<Float>(PropertyNames::PositionY).value;
    float width = uiDict.get<Float>(PropertyNames::Width).value;
    float height = uiDict.get<Float>(PropertyNames::Height).value;
    float rotation = uiDict.get<Float>(PropertyNames::Rotation).value;
    float scaleX = uiDict.get<Float>(PropertyNames::ScaleX).value;
    float scaleY = uiDict.get<Float>(PropertyNames::ScaleY).value;
    float anchorX = uiDict.get<Float>(PropertyNames::AnchorX).value;
    float anchorY = uiDict.get<Float>(PropertyNames::AnchorY).value;
    float originX = uiDict.get<Float>(PropertyNames::OriginX).value;
    float originY = uiDict.get<Float>(PropertyNames::OriginY).value;
    int zIndex = uiDict.get<Int>(PropertyNames::ZIndex).value;
    float colorR = uiDict.get<Float>(PropertyNames::ColorR).value;
    float colorG = uiDict.get<Float>(PropertyNames::ColorG).value;
    float colorB = uiDict.get<Float>(PropertyNames::ColorB).value;
    float colorA = uiDict.get<Float>(PropertyNames::ColorA).value;

    shared_ptr<Entity> entity = nullptr;
    switch (entityType) {
        case EntityType::Rectangle: {
            entity = Rectangle::create(Size(width, height));
            break;
        }
        case EntityType::RoundedRectangle: {
            float cornerRadius = uiDict.get<Float>(PropertyNames::CornerRadius).value;
            entity = RoundedRectangle::create(Size(width, height), cornerRadius);
            break;
        }
        case EntityType::Circle: {
            float radius = uiDict.get<Float>(PropertyNames::Radius).value;
            entity = Circle::create(radius);
            break;
        }
        case EntityType::Label: {
            string text = uiDict.get<String>(PropertyNames::Text).value;
            string fontFilename = uiDict.get<String>(PropertyNames::FontFilename).value;
            float fontSize = uiDict.get<Float>(PropertyNames::FontSize).value;
            float height = uiDict.get<Float>(PropertyNames::FontHeight).value;
            entity = Label::create(text, fontSize, fontFilename, height);
            break;
        }
        case EntityType::Sprite:
        case EntityType::Slice9Sprite:
        case EntityType::SpriteSheet: {
            string filename = uiDict.get<String>(PropertyNames::Filename).value;
            float rectX = uiDict.get<Float>(PropertyNames::RectX).value;
            float rectY = uiDict.get<Float>(PropertyNames::RectY).value;
            float rectWidth = uiDict.get<Float>(PropertyNames::RectWidth).value;
            float rectHeight = uiDict.get<Float>(PropertyNames::RectHeight).value;
            auto sprite = Sprite::create(filename, Rect(rectX, rectY, rectWidth, rectHeight));
            
            if (entityType == EntityType::Slice9Sprite) {
                float centerRectX = uiDict.get<Float>(PropertyNames::CenterRectX).value;
                float centerRectY = uiDict.get<Float>(PropertyNames::CenterRectY).value;
                float centerRectWidth = uiDict.get<Float>(PropertyNames::CenterRectWidth).value;
                float centerRectHeight = uiDict.get<Float>(PropertyNames::CenterRectHeight).value;
                entity = Slice9Sprite::create(sprite, Rect(centerRectX, centerRectY, centerRectWidth, centerRectHeight));
                
            } else if (entityType == EntityType::SpriteSheet) {
                auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
                float frameWidth = uiDict.get<Float>(PropertyNames::FrameWidth).value;
                float frameHeight = uiDict.get<Float>(PropertyNames::FrameHeight).value;
                unsigned int frameCount = (unsigned int )(uiDict.get<Long>(PropertyNames::FrameCount).value);
                unsigned int margin = (unsigned int )(uiDict.get<Long>(PropertyNames::Margin).value);
                entity = SpriteSheet::create(sprite, Size(frameWidth, frameHeight), frameCount, margin);
                
            } else {
                entity = sprite;
            }
            break;
        }
        case EntityType::Group: {
            bool enableBatching = uiDict.get<Bool>(PropertyNames::EnableBatching).value;
            shared_ptr<Group> group = Group::create(enableBatching);
            auto arr = uiDict.get<Array>(PropertyNames::ChildEntities);
            for (int i = 0; i < arr.size(); i++) {
                auto childDict = arr.at<Dictionary>(i);
                auto childEntity = deserialize(childDict);
                group->add(childEntity);
            }
            entity = group;
            break;
        }
        default:
            break;
    }
    
    entity->setName(name);
    entity->setTag(tag);
    entity->setPosition(positionX, positionY);
    entity->setSize(width, height);
    entity->setRotation(rotation);
    entity->setScale(scaleX, scaleY);
    entity->setAnchor(anchorX, anchorY);
    entity->setOrigin(originX, originY);
    entity->setZIndex(zIndex);
    entity->setColor(colorR, colorG, colorB, colorA);
    
    return entity;
}
