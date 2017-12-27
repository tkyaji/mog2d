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

#endif /* MogUILoader_h */

namespace mog {

    class MogUILoader {
    public:
        class PropertyNames {
        public:
            static const std::string EntityType;
            static const std::string Name;
            static const std::string Tag;
            static const std::string PositionX;
            static const std::string PositionY;
            static const std::string Width;
            static const std::string Height;
            static const std::string Rotation;
            static const std::string ScaleX;
            static const std::string ScaleY;
            static const std::string AnchorX;
            static const std::string AnchorY;
            static const std::string OriginX;
            static const std::string OriginY;
            static const std::string ZIndex;
            static const std::string ColorR;
            static const std::string ColorG;
            static const std::string ColorB;
            static const std::string ColorA;
            static const std::string CornerRadius;
            static const std::string Radius;
            static const std::string Text;
            static const std::string FontFilename;
            static const std::string FontSize;
            static const std::string FontHeight;
            static const std::string Filename;
            static const std::string RectX;
            static const std::string RectY;
            static const std::string RectWidth;
            static const std::string RectHeight;
            static const std::string CenterRectX;
            static const std::string CenterRectY;
            static const std::string CenterRectWidth;
            static const std::string CenterRectHeight;
            static const std::string FrameWidth;
            static const std::string FrameHeight;
            static const std::string FrameCount;
            static const std::string Margin;
            static const std::string EnableBatching;
            static const std::string ChildEntities;
        };
        
        static std::shared_ptr<mog::Entity> load(std::string filename);

        static Dictionary serialize(const std::shared_ptr<Entity> &entity);
        static std::shared_ptr<Entity> deserialize(const Dictionary &uiDict);
    };
}
