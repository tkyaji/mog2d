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

std::shared_ptr<Label> Label::create(std::string text, float fontSize, std::string fontFilename, float fontHeight) {
    auto label = std::shared_ptr<Label>(new Label());
    label->text = text;
    label->fontSize = fontSize;
    label->fontFilename = fontFilename;
    label->fontHeight = fontHeight;
    label->init();
    return label;
}

std::shared_ptr<Label> Label::create(const LocalizedText &localizedText, float fontSize, std::string fontFilename, float fontHeight) {
    return Label::create(localizedText.text, fontSize, fontFilename, fontHeight);
}

void Label::init() {
    this->textures[0] = Texture2D::createWithText(this->text, this->fontSize, this->fontFilename, this->fontHeight);
    this->size.width = this->textures[0]->width / this->textures[0]->density.value;
    this->size.height = this->textures[0]->height / this->textures[0]->density.value;

    this->initRendererVertices(4, 4);
    this->dirtyFlag |= DIRTY_ALL;
}



void Label::setText(std::string text) {
    this->text = text;
    this->init();
}

void Label::setText(const LocalizedText &localizedText) {
    this->setText(localizedText.text);
}

std::string Label::getText() {
    return this->text;
}

void Label::setFontSize(float fontSize) {
    this->fontSize = fontSize;
    this->init();
}

float Label::getFontSize() {
    return this->fontSize;
}

void Label::setFontFilename(std::string fontFilename) {
    this->fontFilename = fontFilename;
    this->init();
}

std::string Label::getFontFilename() {
    return this->fontFilename;
}

void Label::setFontHeight(float fontHeight) {
    this->fontHeight = fontHeight;
    this->init();
}

float Label::getFontHeight() {
    return this->fontHeight;
}

std::shared_ptr<Label> Label::clone() {
    auto e = this->cloneEntity();
    return std::static_pointer_cast<Label>(e);
}

std::shared_ptr<Entity> Label::cloneEntity() {
    auto label = Label::create(this->text, this->fontSize, this->fontFilename, this->fontHeight);
    label->copyProperties(std::static_pointer_cast<Entity>(shared_from_this()));
    return label;
}

std::shared_ptr<Dictionary> Label::serialize() {
    auto dict = Entity::serialize();
    dict->put(PROP_KEY_ENTITY_TYPE, Int::create((int)EntityType::Label));
    dict->put(PROP_KEY_TEXT, String::create(this->text));
    dict->put(PROP_KEY_FONT_SIZE, Float::create(this->fontSize));
    dict->put(PROP_KEY_FONT_FILENAME, String::create(this->fontFilename));
    dict->put(PROP_KEY_FONT_HEIGHT, Float::create(this->fontHeight));
    return dict;
}

void Label::deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    Entity::deserializeData(dict, params);
    this->text = this->getPropertyData<String>(dict, PROP_KEY_TEXT, params)->getValue();
    this->fontSize = this->getPropertyData<Float>(dict, PROP_KEY_FONT_SIZE, params)->getValue();
    this->fontFilename = this->getPropertyData<String>(dict, PROP_KEY_FONT_FILENAME, params)->getValue();
    this->fontHeight = this->getPropertyData<Float>(dict, PROP_KEY_FONT_HEIGHT, params)->getValue();
}
