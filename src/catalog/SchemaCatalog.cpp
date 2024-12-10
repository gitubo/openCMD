#include "../../include/catalog/SchemaCatalog.hpp"

Schema* SchemaCatalog::getSchema(const std::string& key) {
    auto it = schemaMap.find(name);
    if (it != schemaMap.end()) {
        return &(it->second);
    } else {
        Logger::getInstance().log("Requested schema <" + key + "> does not exist in the loaded catalog", Logger::Level::WARNING);
    }
    return nullptr;
}

int SchemaCatalog::parseSchema(const std::string& name, const nlohmann::json& jsonSchema) {
    Logger::getInstance().log("Entering parseSchema", Logger::Level::DEBUG);
    Schema schema;
    schema.catalogName = name;
    if(jsonSchema.is_object()){
       for (const auto& [key, val] : json_value.items()) {
            Logger::getInstance().log("  key = " + key, Logger::Level::DEBUG);
    
            if(key=="structure" && val.type() == json::value_t::array){
                schema.structure = parseSchemaStructure(val);
                if(schema.structure == nullptr){
                    Logger::getInstance().log("Error in parsing structure", Logger::Level::WARNING);
                    return 4;
                }
            } else if(key=="metadata" && val.type() == json::value_t::object){
                std::map<std::string, std::string> metadata;
                for(auto& [key, val] : val.items()){
                    if(val.type() == json::value_t::string){
                        metadata[key] = val.get<std::string>();
                    } else {
                        Logger::getInstance().log("Unsupported type for element <" + key + ">: elements in <metadata> section can be only string", Logger::Level::WARNING);
                        return 3;
                    }
                }
                schema.metadata = metadata;
            } else if(key=="version" && val.type() == json::value_t::string){
                schema.version = val.get<std::string>();
            } else {
                Logger::getInstance().log("Unsupported type: " + std::to_string(static_cast<int>(json_value.type())) + " for element named <" + key + "> in file <" + name + ">", Logger::Level::WARNING);
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

SchemaElement::SchemaElementArray SchemaCatalog::parseSchemaStructure(const nlohmann::json& jsonStructure){
    Logger::getInstance().log("Entering parseSchemaStructure", Logger::Level::DEBUG);
    SchemaElement::SchemaElementArray msgStructure;
    if(not (jsonStructure.type() == json::value_t::array)){
        Logger::getInstance().log("Provided structure is not an array: " + std::to_string(static_cast<int>(jsonStructure.type())), Logger::Level::ERROR);
        return msgStructure;
    }
    for (const auto &entry : jsonStructure) {
        Logger::getInstance().log("Calling parseSchemaElement", Logger::Level::DEBUG);
        msgStructure.push_back(parseSchemaElement(entry));
    }
    return msgStructure;
}

SchemaElement::SchemaElementObject SchemaCatalog::parseSchemaElement(const nlohmann::json& jsonElement){
    if(not (jsonElement.type() == json::value_t::object)){
        Logger::getInstance().log("Provided message element is not an object: " + std::to_string(static_cast<int>(json_value.type())), Logger::Level::ERROR);
        return nullptr;
    }
    SchemaElement::SchemaElementObject msgElement;
    for (const auto& [key, val] : jsonElement.items()) {
        if(key=="name" && val.type() == json::value_t::string){  
            msgElement.emplace("name", val.get<std::string>());
        } else if(key=="bit_length" && (val.type() == json::value_t::number_integer || val.type() == json::value_t::number_unsigned)){
            msgElement.emplace("bit_length", val.get<int>());
        } else if(key=="type" && val.type() == json::value_t::string){
            msgElement.emplace("type", val.get<std::string>());
        } else {
            Logger::getInstance().log("Unsupported key or value is not of the correct type: " + key, Logger::Level::DEBUG);
            continue;
        }
    }
    return msgElement;
}

std::string SchemaCatalog::printMessageElementList(const SchemaElement::SchemaElementObject& vec) {
    std::ostringstream oss;
    oss << "{\"structure\" : [";
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        oss << *it;
        if((it < vec.end() - 1)){ oss << ", ";}
    }
    oss << "]}" << std::endl;
    return oss.str();
}
