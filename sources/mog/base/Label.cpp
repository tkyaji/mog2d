#include "mog/Constants.h"
#include "mog/base/Label.h"
#include "mog/core/Texture2D.h"
#include "mog/core/Texture2DNative.h"
#include "mog/core/Engine.h"

using namespace mog;

LocalizedText::LocalizedText(string textKey, ...) {
    va_list args;
    va_start(args, textKey);
    this->text = Texture2DNative::getLocalizedTextNative(textKey.c_str(), args);
    va_end(args);
}


#pragma - Label

shared_ptr<Label> Label::create(string text, float fontSize, string fontFilename, float height) {
    auto label = shared_ptr<Label>(new Label());
    label->init(text, fontSize, fontFilename, height);
    return label;
}

shared_ptr<Label> Label::create(const LocalizedText &localizedText, float fontSize, string fontFilename, float height) {
    auto label = shared_ptr<Label>(new Label());
    label->init(localizedText.text, fontSize, fontFilename, height);
    return label;
}

Label::Label() {
}

void Label::init(string text, float fontSize, string fontFilename, float height) {
    this->texture = Texture2D::createWithText(text, fontSize, fontFilename, height);
    
    this->text = text;
    this->fontSize = fontSize;
    this->fontFilename = fontFilename;
    this->height = height;
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
    this->initRendererVertices(4, 4);
    
    this->reRenderFlag |= RERENDER_ALL;
}

void Label::setText(string text, float fontSize, string fontFilename, float height) {
    if (fontSize <= 0) {
        fontSize = this->fontSize;
    }
    if (fontFilename.length() == 0) {
        fontFilename = this->fontFilename;
    }
    
    this->init(text, fontSize, fontFilename, height);
}

void Label::setText(const LocalizedText &localizedText, float fontSize, string fontFilename, float height) {
    this->setText(localizedText.text, fontSize, fontFilename, height);
}

string Label::getText() {
    return this->text;
}

void Label::setFontSize(float fontSize) {
    this->init(this->text, fontSize, this->fontFilename, this->height);
}

float Label::getFontSize() {
    return this->fontSize;
}

void Label::setFontFilename(string fontFilename) {
    this->init(this->text, this->fontSize, fontFilename, this->height);
}

string Label::getFontFilename() {
    return this->fontFilename;
}

void Label::setFontHeight(float height) {
    this->init(this->text, this->fontSize, this->fontFilename, height);
}

float Label::getFontHeight() {
    return this->height;
}
