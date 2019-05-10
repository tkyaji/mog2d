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
    public:
        static std::shared_ptr<Label> create(std::string text, float fontSize, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        static std::shared_ptr<Label> create(std::string text, float fontSize, std::string fontFilename, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        static std::shared_ptr<Label> create(std::string text, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);

        static std::shared_ptr<Label> create(const LocalizedText &localizedText, float fontSize, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        static std::shared_ptr<Label> create(const LocalizedText &localizedText, float fontSize, std::string fontFilename, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        static std::shared_ptr<Label> create(const LocalizedText &localizedText, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);

        void setText(std::string text, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        void setText(std::string text, float fontSize, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        void setText(std::string text, float fontSize, std::string fontFilename, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        void setText(std::string text, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);

        void setText(const LocalizedText &localizedText, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        void setText(const LocalizedText &localizedText, float fontSize, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        void setText(const LocalizedText &localizedText, float fontSize, std::string fontFilename, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        void setText(const LocalizedText &localizedText, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode = TextDrawingMode::Fill, float strokeWidth = 0);
        
        std::string getText();
        
        void setFontSize(float fontSize);
        float getFontSize();
        
        void setFontFilename(std::string fontFilename);
        std::string getFontFilename();
        
        void setFontHeight(float height);
        float getFontHeight();
        
        void setTextDrawingMode(TextDrawingMode textMode, float strokeWidth = 0);
        TextDrawingMode getTextDrawingMode();
        
        void setStrokeWidth(float strokeWidth);
        float getStrokeWidth();
        
        std::shared_ptr<Label> clone();

    protected:
        Label() {}
        
        void init(std::string text, float fontSize, std::string fontFilename = "", float height = 0, TextDrawingMode textMode = TextDrawingMode::Fill, float outlineWidth = 0);
        virtual std::shared_ptr<Entity> cloneEntity() override;
        
        std::string text;
        float fontSize;
        std::string fontFilename;
        float height;
        TextDrawingMode textMode = TextDrawingMode::Fill;
        float strokeWidth;
    };
}

#endif /* Label_h */
