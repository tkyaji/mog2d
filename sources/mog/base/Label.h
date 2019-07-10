#ifndef Label_h
#define Label_h

#include "mog/base/Scene.h"
#include "mog/base/Entity.h"
#include "mog/core/Texture2D.h"

namespace mog {
    
#pragma - LocalizedText

    class LocalizedText {
    public:
        LocalizedText(std::string textKey, ...);
        std::string text;
        
    private:
        std::string getLocalizedText(const char *textKey, int count, ...);
    };
    
    
#pragma - Label
    
    class Label : public Entity {
        friend class EntityCreator;
    public:
        static std::shared_ptr<Label> create(std::string text, float fontSize, std::string fontFilename = "", float fontHeight = 0);
        static std::shared_ptr<Label> create(const LocalizedText &localizedText, float fontSize, std::string fontFilename = "", float fontHeight = 0);

        void setText(std::string text);
        void setText(const LocalizedText &localizedText);
        
        std::string getText();
        
        void setFontSize(float fontSize);
        float getFontSize();
        
        void setFontFilename(std::string fontFilename);
        std::string getFontFilename();
        
        void setFontHeight(float fontHeight);
        float getFontHeight();
        
        std::shared_ptr<Label> clone();
        virtual std::shared_ptr<Dictionary> serialize() override;

    protected:
        Label() {}
        
        virtual void init() override;
        virtual std::shared_ptr<Entity> cloneEntity() override;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict) override;
        
        std::string text;
        float fontSize;
        std::string fontFilename;
        float fontHeight;
    };
}

#endif /* Label_h */
