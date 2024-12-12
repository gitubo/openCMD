#include "../../include/engine/Engine.hpp"

using namespace opencmd;


std::optional<std::shared_ptr<NodeRoot>> Engine::evaluateSchema(const Schema& schema){
    return evaluateStructure(schema.getStructure(), "");;
    /*
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
    */
}

std::optional<std::shared_ptr<NodeRoot>> Engine::evaluateStructure(const SchemaElement& structure, const std::string& parentName){
    // Check if it is a valid array
    if(!structure.isArray()){
        return std::nullopt;
    }

    // Prepare the root 
    std::shared_ptr<NodeRoot> thisNode = std::make_shared<NodeRoot>();
    thisNode->setParentName(parentName);

    // Iterate througt the array and analize the elements
    for(auto element : structure.getArray().value()){
        auto result = evaluateElement(element);
        if(!result){
            Logger::getInstance().log("Error in evaluating element of the structure", Logger::Level::ERROR);
            return std::nullopt;
        }
        thisNode->addChild(result.value());
    }
    return thisNode;
}

std::optional<std::shared_ptr<TreeElement>> Engine::evaluateElement(const SchemaElement& element){
    Logger::getInstance().log("Calling evaluateElement with object: " + element.to_string(), Logger::Level::WARNING);
    
    if(element.isObject()){
        const auto& object = element.getObject().value();
        if(object.find("type") == object.end()){
            Logger::getInstance().log("Impossible to find a value for key 'type'", Logger::Level::ERROR);
            return std::nullopt;
        }
        if(!object.at("type").isString()){
            Logger::getInstance().log("Invalid type for key 'type', not a string", Logger::Level::ERROR);
            return std::nullopt;
        }
        auto type = object.at("type").getString().value();
        auto obj = SchemaCatalog::getInstance().wrap_create(type);
        if(!obj){
            Logger::getInstance().log("Object creation failed for type: " + type, Logger::Level::ERROR);
            return std::nullopt;
        }
        obj->setName("aaa");

        return obj;

/*

        // Collect the possible attributes of the object
        std::string type = "";
        std::string name = "";
        for (auto it = object.begin(); it != object.end(); ++it) {
            auto key = it->first;
            auto value = it->second;
            if (key == "type" && value.isString()) {
                type = value.getString().value();
                Logger::getInstance().log("Found element with type: " + type, Logger::Level::DEBUG);
            }
        }

        if(type.empty){
            Logger::getInstance().log("'type' attribute not found", Logger::Level::ERROR);
        }
        auto obj = SchemaCatalog::getInstance().wrap_create(value.getString().value());
        if(!obj){
            Logger::getInstance().log("Object creation failed", Logger::Level::ERROR);
            return std::nullopt;
        }
        return obj;

*/
    }
    
    return std::nullopt;
}
