#ifndef Label_h
#define Label_h

#include "mog/base/Scene.h"
#include "mog/base/Entity.h"

namespace mog {
    
#pragma - LocalizedText

    class LocalizedText {
    public:
        LocalizedText(string textKey, ...);
        string text;
        
    private:
        string getLocalizedText(const char *textKey, int count, ...);
    };
    
    
#pragma - Label
    
    class Label : public Entity {
    public:
        static shared_ptr<Label> create(string text, float fontSize, string fontFilename = "", float height = 0);
        static shared_ptr<Label> create(const LocalizedText &localizedText, float fontSize, string fontFilename = "", float height = 0);
        
        void setText(string text, float fontSize = 0, string fontFilename = "", float height = 0);
        void setText(const LocalizedText &localizedText, float fontSize = 0, string fontFilename = "", float height = 0);
        string getText();
        
        void setFontSize(float fontSize);
        float getFontSize();
        
        void setFontFilename(string fontFilename);
        string getFontFilename();
        
        void setFontHeight(float height);
        float getFontHeight();

    protected:
        Label();
        void init(string text, float fontSize, string fontFilename = "", float height = 0);
        
        string text;
        float fontSize;
        string fontFilename;
        float height;
    };
}

#endif /* Label_h */
