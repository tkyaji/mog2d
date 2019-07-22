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
};


std::shared_ptr<Group> MogUILoader::load(std::string filename) {
    std::vector<Param> params;
    return load(filename, params);
}

std::shared_ptr<Group> MogUILoader::load(std::string filename, const std::vector<Param> &params) {
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
        
        if (entityType == EntityType::Group) {
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
