#include "../../include/engine/Engine.hpp"

using namespace opencmd;

std::optional<std::shared_ptr<TreeElement>> Engine::evaluateElement(const SchemaElement& element){
    Logger::getInstance().log("Calling evaluateElement with object: " + element.to_string(), Logger::Level::WARNING);
    
    if(element.isObject()){
        const auto& object = element.getObject().value();
        for (auto it = object.begin(); it != object.end(); ++it) {
            auto key = it->first;
            auto value = it->second;
            if (key == "type" && value.isString()) {
                Logger::getInstance().log("Found element with type: " + value.getString().value(), Logger::Level::DEBUG);
                auto obj = SchemaCatalog::getInstance().wrap_create(value.getString().value());
                if(obj){
                    Logger::getInstance().log("Object created", Logger::Level::DEBUG);
                }
                return obj;
            }
        }

        //std::shared_ptr<TreeElement> node = std::make_shared<TreeElement>();
        //return node;
        return std::nullopt;
    }
    
    return std::nullopt;
}

std::optional<std::shared_ptr<NodeRoot>> Engine::evaluateSchema(const Schema& schema){
    std::shared_ptr<NodeRoot> node_root = std::make_shared<NodeRoot>();
    for(auto element : schema.getStructure()){
        auto result = evaluateElement(element);
        if(!result){
            Logger::getInstance().log("Error in evaluating element", Logger::Level::ERROR);
            return std::nullopt;
        }
        node_root->addChild(result.value());
    }
    return node_root;
}