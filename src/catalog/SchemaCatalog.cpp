#include "../../include/catalog/SchemaCatalog.hpp"

using namespace opencmd;

std::shared_ptr<TreeNode> SchemaCatalog::cloneAbstractTree(const std::string& schemaName) const {
    auto it = schemaMap.find(schemaName);
    if (it != schemaMap.end()) {
        return it->second.getAbstractTree()->clone();
    } else {
        Logger::getInstance().log("Requested schema <" + schemaName + "> does not exist in the loaded catalog", Logger::Level::WARNING);
    }
    return nullptr;
}

int SchemaCatalog::parseSchema(const std::string& name, const nlohmann::json& jsonSchema) {
    Schema schema;
    schema.setCatalogName(name);
    if(jsonSchema.is_object()){
       for (const auto& [key, val] : jsonSchema.items()) {
            if(key=="structure" && val.type() == nlohmann::json::value_t::array){
                auto rootNode = evalArray(val);
                if(!rootNode){
                    Logger::getInstance().log("Error in parsing structure '/'", Logger::Level::ERROR);
                    return 4;
                }
                rootNode.value()->setName("/");
                rootNode.value()->setParentName("");
                schema.abstractTree = std::dynamic_pointer_cast<NodeRoot>(rootNode.value());
                std::cout << " TREE: " << schema.abstractTree->to_string() << std::endl;
            } else if(key=="metadata" && val.type() == nlohmann::json::value_t::object){
                std::map<std::string, std::string> metadata;
                for(auto& [key, val] : val.items()){
                    if(val.type() == nlohmann::json::value_t::string){
                        metadata[key] = val.get<std::string>();
                    } else {
                        Logger::getInstance().log("Unsupported type for element <" + key + ">: elements in <metadata> section can be only string", Logger::Level::WARNING);
                        return 3;
                    }
                }
                schema.setMetadata(metadata);
            } else if(key=="version" && val.type() == nlohmann::json::value_t::string){
                schema.setVersion(val.get<std::string>());
            } else {
                Logger::getInstance().log("Unsupported type: " + std::to_string(static_cast<int>(jsonSchema.type())) + " for element named <" + key + "> in file <" + name + ">", Logger::Level::WARNING);
                return 2;
            }
        }
    } else {
        Logger::getInstance().log("Provided JSON Schema is not an object", Logger::Level::ERROR);
        return 1;
    } 

    schemaMap[name] = schema;
    return 0;
}

std::optional<std::shared_ptr<TreeNode>> SchemaCatalog::evalArray(const nlohmann::json& jsonArray){
    std::shared_ptr<NodeRoot> thisNode = std::make_shared<NodeRoot>();
    for (const auto &entry : jsonArray) {
        if(entry.type() == nlohmann::json::value_t::array){
            Logger::getInstance().log("Error not expected an array in array", Logger::Level::ERROR);
            return std::nullopt;
        } else {
            auto child = evalObject(entry);
            if(!child){
                Logger::getInstance().log("Error in parsing structure '/'", Logger::Level::ERROR);
                return std::nullopt;
            }
            thisNode->addChild(child.value());
        }
    }
    return thisNode;
}

std::optional<std::shared_ptr<TreeNode>> SchemaCatalog::evalObject(const nlohmann::json& jsonObject){
    
    // Type
    std::string type;
    if(jsonObject.contains("type") && jsonObject.at("type").is_string()){
        type = jsonObject.at("type").get<std::string>();
    } else {
        Logger::getInstance().log("Impossible to find a value for key 'type'", Logger::Level::ERROR);
        return std::nullopt;
    }

    // Instantiate object
    auto thisNode = TreeFactory::getInstance().create(type);
    if(!thisNode){
        Logger::getInstance().log("Node creation failed for type: " + type, Logger::Level::ERROR);
        return std::nullopt;
    }
    Logger::getInstance().debug("Node created for type: " + type);

    // Name
    if(jsonObject.contains("name") && jsonObject.at("name").is_string()){
        thisNode->setName(jsonObject.at("name").get<std::string>());
    } else {
        Logger::getInstance().log("Impossible to find key 'name' or it is not a string, default empty value provided", Logger::Level::WARNING);
    }

    // Attributes
    if(jsonObject.contains("attributes")){
        if(!jsonObject.at("attributes").is_object()){
            Logger::getInstance().log("Invalid type for key 'attributes', object expected", Logger::Level::ERROR);
            return std::nullopt;
        }
        auto attributes = jsonObject.at("attributes");
        for (auto& [attributeName, attributeValue] : attributes.items()) {
            auto attribute = evalAttribute(attributeValue);
            if(!attribute){
                Logger::getInstance().error("Invalid attribute with attribute name <"+attributeName+">");
                return std::nullopt;
            }
            thisNode->addAttribute(attributeName, *attribute.value());
        }
    } 

    // Structure
    if(jsonObject.contains("structure")){
        if(!jsonObject.at("structure").is_array()){
            Logger::getInstance().error("Invalid type for key 'structure', array expected");
            return std::nullopt;
        }
        auto localRootNode = evalArray(jsonObject.at("structure"));
        if(!localRootNode){
            Logger::getInstance().log("Error in parsing structure '...'", Logger::Level::ERROR);
            return std::nullopt;
        }
        // Copy evaluated children to the current root node
        for(const auto& localChild : localRootNode.value()->getChildren()){
            auto child = localChild;
            thisNode->addChild(child);
        }
    } 

    return thisNode;
}

std::optional<std::shared_ptr<TreeNodeAttribute>> SchemaCatalog::evalAttribute(const nlohmann::json& jsonAttribute){
    if(jsonAttribute.is_boolean()){
        std::shared_ptr<TreeNodeAttribute> thisAttribute = std::make_shared<TreeNodeAttribute>(jsonAttribute.get<bool>());
        return thisAttribute;
    } else if(jsonAttribute.is_number_integer()){
        std::shared_ptr<TreeNodeAttribute> thisAttribute = std::make_shared<TreeNodeAttribute>(jsonAttribute.get<int64_t>());
        return thisAttribute;
    } else if(jsonAttribute.is_number_float()){
        std::shared_ptr<TreeNodeAttribute> thisAttribute = std::make_shared<TreeNodeAttribute>(jsonAttribute.get<double>());
        return thisAttribute;
    } else if(jsonAttribute.is_string()){
        std::shared_ptr<TreeNodeAttribute> thisAttribute = std::make_shared<TreeNodeAttribute>(jsonAttribute.get<std::string>());
        return thisAttribute;
    } else if(jsonAttribute.is_array()){
        TreeNodeAttribute::TreeNodeAttributeArray attributeArray;
        attributeArray.reserve(jsonAttribute.size());
        for (auto& [attributeName, attributeValue] : jsonAttribute.items()) {
            auto attribute = evalAttribute(attributeValue);
            if(!attribute){
                Logger::getInstance().error("Invalid attribute in array at position <"+attributeName+">");
                return std::nullopt;
            }
            attributeArray.emplace_back(*attribute.value());
        }
        std::shared_ptr<TreeNodeAttribute> thisAttribute = std::make_shared<TreeNodeAttribute>(attributeArray);
        return thisAttribute;
    } else if(jsonAttribute.is_object()){
        TreeNodeAttribute::TreeNodeAttributeObject attributeObject;
        attributeObject.reserve(jsonAttribute.size());
        for (auto& [attributeName, attributeValue] : jsonAttribute.items()) {
            auto attribute = evalAttribute(attributeValue);
            if(!attribute){
                Logger::getInstance().error("Invalid attribute with key <"+attributeName+">");
                return std::nullopt;
            }
            std::pair<std::string,TreeNodeAttribute> thisPair (attributeName, *attribute.value());
            attributeObject.insert(thisPair);
        }
        std::shared_ptr<TreeNodeAttribute> thisAttribute = std::make_shared<TreeNodeAttribute>(attributeObject);
        return thisAttribute;
    }
    Logger::getInstance().error("Invalid attribute type");
    return std::nullopt;
}

std::string SchemaCatalog::to_string(const SchemaElement::SchemaElementArray& elements) {
    std::ostringstream oss;
    for (const auto& element : elements) {
        oss << "- " << element.to_string(2) << "\n"; 
    }
    return oss.str();
}
