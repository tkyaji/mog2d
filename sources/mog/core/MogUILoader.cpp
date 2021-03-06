#include "mog/core/MogUILoader.h"
#include "mog/core/FileUtils.h"
#include "mog/core/DataStore.h"
#include "mog/core/EntityCreator.h"

using namespace mog;

static std::unordered_map<int, std::string> entityTypeMap = {
    { (int)EntityType::Rectangle,        "Rectangle"},
    { (int)EntityType::RoundedRectangle, "RoundedRectangle"},
    { (int)EntityType::Circle,           "Circle"},
    { (int)EntityType::Label,            "Label"},
    { (int)EntityType::Line,             "Line"},
    { (int)EntityType::Sprite,           "Sprite"},
    { (int)EntityType::SpriteSheet,      "SpriteSheet"},
    { (int)EntityType::Slice9Sprite,     "Slice9Sprite"},
    { (int)EntityType::TiledSprite,      "TiledSprite"},
    { (int)EntityType::Group,            "Group"},
    { (int)EntityType::ScrollGroup,      "ScrollGroup"},
    { (int)EntityType::VerticalGroup,    "VerticalGroup"},
    { (int)EntityType::HorizontalGroup,  "HorizontalGroup"},
};

static bool isGroupType(EntityType entityType) {
    if (entityType == EntityType::Group) return true;
    if (entityType == EntityType::ScrollGroup) return true;
    if (entityType == EntityType::VerticalGroup) return true;
    if (entityType == EntityType::HorizontalGroup) return true;

    return false;
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, const std::shared_ptr<Data> &propertyValue) {
    this->init(name, propertyKey, propertyValue);
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, int value) {
    this->init(name, propertyKey, Int::create(value));
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, long value) {
    this->init(name, propertyKey, Long::create(value));
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, float value) {
    this->init(name, propertyKey, Float::create(value));
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, double value) {
    this->init(name, propertyKey, Double::create(value));
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, bool value) {
    this->init(name, propertyKey, Bool::create(value));
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, std::string value) {
    this->init(name, propertyKey, String::create(value));
}

MogUILoader::Param::Param(std::string name, std::string propertyKey, const char *value) {
    this->init(name, propertyKey, String::create(value));
}

void MogUILoader::Param::init(std::string name, std::string propertyKey, const std::shared_ptr<Data> &propertyValue) {
    this->name = name;
    this->propertyKey = propertyKey;
    this->propertyValue = propertyValue;

}

std::shared_ptr<Group> MogUILoader::load(std::string filename) {
    std::vector<Param> params;
    return load(filename, params);
}

std::shared_ptr<Group> MogUILoader::load(std::string filename, const std::vector<Param> &params) {
    if (!FileUtils::existAsset(filename)) {
        LOGE("Asset file not found. [%s]", filename.c_str());
        return nullptr;
    }
    auto bytes = FileUtils::readBytesAsset(filename);
    auto dict = DataStore::deserialize<Dictionary>(bytes);
    std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> paramMap;
    for (auto &p : params) {
        if (paramMap.count(p.name) == 0) {
            std::unordered_map<std::string, std::shared_ptr<Data>> map;
            paramMap[p.name] = map;
        }
        paramMap[p.name][p.propertyKey] = p.propertyValue;
    }
    auto entity = EntityCreator::create(EntityType::Group, dict, paramMap);
    
    int start = (int)filename.find("/") + 1;
    int end = (int)filename.find(".");
    if (end < -1) end = (int)filename.size() - 1;
    std::string entityName = filename.substr(start, end - start);
    entity->setName(entityName);
    
    return std::static_pointer_cast<Group>(entity);
}

std::string MogUILoader::toJsonString(const std::shared_ptr<Dictionary> &dict) {
    std::string jsonStr;
    jsonStr.reserve(1024);
    dictToGroupJsonString(dict, jsonStr);
    return jsonStr;
}

void MogUILoader::dictToGroupJsonString(const std::shared_ptr<Dictionary> &dict, std::string &jsonStr) {
    auto groupEntityType = (EntityType)dict->get<Int>("entityType")->getValue();
    auto groupEntityTypeStr = entityTypeMap[(int)groupEntityType];
    auto groupName = dict->get<String>("name")->getValue();

    jsonStr.append("{");
    jsonStr.append("\"name\":\"").append(groupName).append("\",");
    jsonStr.append("\"entityType\":\"").append(groupEntityTypeStr).append("\",");
    jsonStr.append("\"childEntities\":[");

    auto childEntityList = dict->get<List>("childEntities");
    int len = childEntityList->size();
    for (int i = 0; i < len; i++) {
        auto entityDict = childEntityList->at<Dictionary>(i);
        auto entityType = (EntityType)entityDict->get<Int>("entityType")->getValue();
        
        if (isGroupType(entityType)) {
            dictToGroupJsonString(entityDict, jsonStr);
            
        } else {
            auto entityTypeStr = entityTypeMap[(int)entityType];
            auto name = entityDict->get<String>("name")->getValue();
            
            jsonStr.append("{");
            jsonStr.append("\"name\":\"").append(name).append("\",");
            jsonStr.append("\"entityType\":\"").append(entityTypeStr).append("\"");
            jsonStr.append("}");
        }
        
        if (i < len - 1) {
            jsonStr.append(",");
        }
    }
    
    jsonStr.append("]}");
}
