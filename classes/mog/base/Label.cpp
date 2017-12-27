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

shared_ptr<Label> Label::create(string textKey, float fontSize, string fontFilename, float height) {
    auto label = shared_ptr<Label>(new Label());
    label->init(textKey, fontSize, fontFilename, height);
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
    this->size.width = this->texture->width / this->texture->density.value;
    this->size.height = this->texture->height / this->texture->density.value;
    this->transform->size = this->size;
}

void Label::setText(string text, float fontSize, string fontFilename, float height) {
    if (fontSize <= 0) {
        fontSize = this->fontSize;
    }
    if (fontFilename.length() == 0) {
        fontFilename = this->fontFilename;
    }
    
    this->init(text, fontSize, fontFilename, height);
    
    this->reRenderFlag |= RERENDER_ALL;
    this->setReRenderFlag(RERENDER_ALL);
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
    this->fontFilename = srcLabel->fontFilename;
    this->height = srcLabel->height;
}

EntityType Label::getEntityType() {
    return EntityType::Label;
}
