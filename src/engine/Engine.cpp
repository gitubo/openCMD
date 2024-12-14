#include "../../include/engine/Engine.hpp"

using namespace opencmd;


std::optional<std::shared_ptr<NodeRoot>> Engine::evaluateStructure(const std::vector<SchemaElement>& structure, const std::string& name){

    // Prepare the local root node 
    std::shared_ptr<NodeRoot> thisNode = std::make_shared<NodeRoot>();
    thisNode->setName(name);

    // Iterate througt the array and analize the elements
    for (auto it = structure.begin(); it != structure.end(); ++it) {

        if(!it->isObject()){
            Logger::getInstance().log("SchemaEleemnt is not an object as expected", Logger::Level::ERROR);
            return std::nullopt;
        }
        auto result = evaluateElement(*it->getObject());
        if(!result){
            Logger::getInstance().log("Error in evaluating element of the structure", Logger::Level::ERROR);
            return std::nullopt;
        }
        result.value()->setParentName(name);
        Logger::getInstance().log("Adding objcet with name <" + result.value()->getName() + "> as child of object with name <"+thisNode->getName()+">", Logger::Level::DEBUG);
        thisNode->addChild(result.value());
    }
    return thisNode;
}

std::optional<std::shared_ptr<TreeNode>> Engine::evaluateElement(const std::map<std::string, SchemaElement>& element){
    
    std::string key = "";

    // Retrieve 'type' of the element
    key = "type";
    if(element.find(key) == element.end()){
        Logger::getInstance().log("Impossible to find a value for key '"+key+"'", Logger::Level::ERROR);
        return std::nullopt;
    }
    if(!element.at(key).isString()){
        Logger::getInstance().log("Invalid type for key '"+key+"', not a string", Logger::Level::ERROR);
        return std::nullopt;
    }
    auto type = element.at(key).getString().value();

    // Retrieve 'name' of the element
    key = "name";
    if(element.find(key) == element.end()){
        Logger::getInstance().log("Impossible to find a value for key <"+key+">", Logger::Level::ERROR);
        return std::nullopt;
    }
    if(!element.at(key).isString()){
        Logger::getInstance().log("Invalid type for key <"+key+">, not a string", Logger::Level::ERROR);
        return std::nullopt;
    }
    auto name = element.at(key).getString().value();

    // Instantiate object
    auto obj = TreeFactory::getInstance().create(type);
    if(!obj){
        Logger::getInstance().log("Object creation failed for type: " + type, Logger::Level::ERROR);
        return std::nullopt;
    }
    obj->setName(name);

    // Retrieve 'attributes' of the element
    key = "attributes";
    if(element.find(key) != element.end()){
        if(!element.at(key).isObject()){
            Logger::getInstance().log("Invalid type for key <"+key+">, not an object", Logger::Level::ERROR);
            return std::nullopt;
        }
        auto attributes = element.at(key).getObject().value();
        for(auto it = attributes.begin(); it != attributes.end(); ++it){
            auto element = it->second;
            if(element.isBool()){
                TreeNodeAttribute attribute = TreeNodeAttribute(element.getBool().value());
                obj->addAttribute(it->first, attribute);
            } else if(element.isInteger()){
                TreeNodeAttribute attribute = TreeNodeAttribute(element.getInteger().value());
                obj->addAttribute(it->first, attribute);
            } else if(element.isDecimal()){
                TreeNodeAttribute attribute = TreeNodeAttribute(element.getDecimal().value());
                obj->addAttribute(it->first, attribute);
            } else if(element.isString()){
                TreeNodeAttribute attribute = TreeNodeAttribute(element.getString().value());
                obj->addAttribute(it->first, attribute);
            } else {
                Logger::getInstance().log("Invalid type for attribute <"+it->first+">", Logger::Level::ERROR);
                return std::nullopt;
            }
        }
    } else {
       Logger::getInstance().log("Key <"+key+"> not found for this object", Logger::Level::ERROR); 
    }

    key = "structure";
    if(element.find(key) != element.end()){
        if(!element.at(key).isArray()){
            Logger::getInstance().log("Invalid type for key <"+key+">, not an array", Logger::Level::ERROR);
            return std::nullopt;
        }

        // Evaluate inner structure
        auto localStructure = element.at(key).getArray().value();
        auto localRootNode = evaluateStructure(localStructure, obj->getName());
        // Copy evaluated children
        if(localRootNode){
            for(const auto& localChild : localRootNode.value()->getChildren()){
                auto child = localChild;
                obj->addChild(child);
            }
        }
    } else {
        Logger::getInstance().log("Key <"+key+"> not found for this object", Logger::Level::DEBUG); 
    }

    return obj;
}
