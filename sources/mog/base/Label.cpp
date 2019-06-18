#include "mog/Constants.h"
#include "mog/base/Label.h"
#include "mog/core/Texture2D.h"
#include "mog/core/Texture2DNative.h"
#include "mog/core/Engine.h"

using namespace mog;

LocalizedText::LocalizedText(std::string textKey, ...) {
    va_list args;
    va_start(args, textKey);
    this->text = Texture2DNative::getLocalizedTextNative(textKey.c_str(), args);
    va_end(args);
}


#pragma - Label

std::shared_ptr<Label> Label::create(std::string text, float fontSize, TextDrawingMode textMode, float strokeWidth) {
    return Label::create(text, fontSize, "", 0, textMode, strokeWidth);
}

std::shared_ptr<Label> Label::create(std::string text, float fontSize, std::string fontFilename, TextDrawingMode textMode, float strokeWidth) {
    return Label::create(text, fontSize, fontFilename, 0, textMode, strokeWidth);
}

std::shared_ptr<Label> Label::create(std::string text, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode, float strokeWidth) {
    auto label = std::shared_ptr<Label>(new Label());
    label->init(text, fontSize, fontFilename, height, textMode, strokeWidth);
    return label;
}

std::shared_ptr<Label> Label::create(const LocalizedText &localizedText, float fontSize, TextDrawingMode textMode, float strokeWidth) {
    return Label::create(localizedText, fontSize, "", 0, textMode, strokeWidth);
}

std::shared_ptr<Label> Label::create(const LocalizedText &localizedText, float fontSize, std::string fontFilename, TextDrawingMode textMode, float strokeWidth) {
    return Label::create(localizedText, fontSize, fontFilename, 0, textMode, strokeWidth);
}

std::shared_ptr<Label> Label::create(const LocalizedText &localizedText, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode, float strokeWidth) {
    return Label::create(localizedText.text, fontSize, fontFilename, height, textMode, strokeWidth);
}

void Label::init(std::string text, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode, float strokeWidth) {
    this->textures[0] = Texture2D::createWithText(text, fontSize, fontFilename, height, textMode, strokeWidth);
    this->text = text;
    this->fontSize = fontSize;
    this->fontFilename = fontFilename;
    this->height = height;
    this->strokeWidth = strokeWidth;
    this->transform->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->transform->size.height = this->textures[0]->height / this->textures[0]->density.value;
    this->initRendererVertices(4, 4);
    
    this->dirtyFlag |= DIRTY_ALL;
}



void Label::setText(std::string text, TextDrawingMode textMode, float strokeWidth) {
    this->setText(text, this->fontSize, this->fontFilename, this->height, textMode, strokeWidth);
}

void Label::setText(std::string text, float fontSize, TextDrawingMode textMode, float strokeWidth) {
    this->setText(text, fontSize, this->fontFilename, this->height, textMode, strokeWidth);
}

void Label::setText(std::string text, float fontSize, std::string fontFilename, TextDrawingMode textMode, float strokeWidth) {
    this->setText(text, fontSize, fontFilename, this->height, textMode, strokeWidth);
}

void Label::setText(std::string text, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode, float strokeWidth) {
    this->init(text, fontSize, fontFilename, height, textMode, strokeWidth);
}

void Label::setText(const LocalizedText &localizedText, TextDrawingMode textMode, float strokeWidth) {
    this->setText(localizedText.text, this->fontSize, this->fontFilename, this->height, textMode, strokeWidth);
}

void Label::setText(const LocalizedText &localizedText, float fontSize, TextDrawingMode textMode, float strokeWidth) {
    this->setText(localizedText.text, fontSize, this->fontFilename, this->height, textMode, strokeWidth);
}

void Label::setText(const LocalizedText &localizedText, float fontSize, std::string fontFilename, TextDrawingMode textMode, float strokeWidth) {
    this->setText(localizedText.text, fontSize, fontFilename, this->height, textMode, strokeWidth);
}

void Label::setText(const LocalizedText &localizedText, float fontSize, std::string fontFilename, float height, TextDrawingMode textMode, float strokeWidth) {
    this->init(localizedText.text, fontSize, fontFilename, height, textMode, strokeWidth);
}

std::string Label::getText() {
    return this->text;
}

void Label::setFontSize(float fontSize) {
    this->init(this->text, fontSize, this->fontFilename, this->height, this->textMode, this->strokeWidth);
}

float Label::getFontSize() {
    return this->fontSize;
}

void Label::setFontFilename(std::string fontFilename) {
    this->init(this->text, this->fontSize, fontFilename, this->height, this->textMode, this->strokeWidth);
}

std::string Label::getFontFilename() {
    return this->fontFilename;
}

void Label::setFontHeight(float height) {
    this->init(this->text, this->fontSize, this->fontFilename, height, this->textMode, this->strokeWidth);
}

float Label::getFontHeight() {
    return this->height;
}

void Label::setTextDrawingMode(TextDrawingMode textMode, float strokeWidth) {
    this->init(this->text, this->fontSize, this->fontFilename, this->height, textMode, strokeWidth);
}

TextDrawingMode Label::getTextDrawingMode() {
    return this->textMode;
}

void Label::setStrokeWidth(float strokeWidth) {
    this->init(this->text, this->fontSize, this->fontFilename, this->height, this->textMode, strokeWidth);
}

float Label::getStrokeWidth() {
    return this->strokeWidth;
}

std::shared_ptr<Label> Label::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<Label>(e);
}

std::shared_ptr<Entity> Label::cloneEntity() {
    auto label = Label::create(this->text, this->fontSize, this->fontFilename, this->height, this->textMode, this->strokeWidth);
    label->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return label;
}
