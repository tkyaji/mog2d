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

shared_ptr<Label> Label::create(string textKey, float fontSize, string fontFace, float height) {
    auto label = shared_ptr<Label>(new Label());
    label->init(textKey, fontSize, fontFace, height);
    return label;
}

shared_ptr<Label> Label::create(const LocalizedText &localizedText, float fontSize, string fontFace, float height) {
    auto label = shared_ptr<Label>(new Label());
    label->init(localizedText.text, fontSize, fontFace, height);
    return label;
}

Label::Label() {
}

void Label::init(string text, float fontSize, string fontFace, float height) {
    this->texture = Texture2D::createWithText(text, fontSize, fontFace, height);
    
    this->text = text;
    this->fontSize = fontSize;
    this->fontFace = fontFace;
    this->height = height;
    this->transform->size.width = this->texture->width / this->texture->density.value;
    this->transform->size.height = this->texture->height / this->texture->density.value;
}

void Label::setText(string text, float fontSize, string fontFace, float height) {
    if (fontSize <= 0) {
        fontSize = this->fontSize;
    }
    if (fontFace.length() == 0) {
        fontFace = this->fontFace;
    }
    
    this->init(text, fontSize, fontFace, height);
    
    this->reRenderFlag |= RERENDER_ALL;
    this->setReRenderFlag(RERENDER_ALL);
}

void Label::setText(const LocalizedText &localizedText, float fontSize, string fontFace, float height) {
    this->setText(localizedText.text, fontSize, fontFace, height);
}

string Label::getText() {
    return this->text;
}

void Label::setFontSize(float fontSize) {
    this->init(this->text, fontSize, this->fontFace, this->height);
}

float Label::getFontSize() {
    return this->fontSize;
}

void Label::setFontFace(string fontFace) {
    this->init(this->text, this->fontSize, fontFace, this->height);
}

string Label::getFontFace() {
    return this->fontFace;
}

shared_ptr<Label> Label::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<Label>(entity);
}

shared_ptr<Entity> Label::cloneEntity() {
    auto label = shared_ptr<Label>(new Label());
    label->copyFrom(shared_from_this());
    return label;
}

void Label::copyFrom(const shared_ptr<Entity> &src) {
    DrawEntity::copyFrom(src);
    auto srcLabel = static_pointer_cast<Label>(src);
    this->text = srcLabel->text;
    this->fontSize = srcLabel->fontSize;
    this->fontFace = srcLabel->fontFace;
    this->height = srcLabel->height;
}
