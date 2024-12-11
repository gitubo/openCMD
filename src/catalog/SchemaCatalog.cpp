#include "../../include/catalog/SchemaCatalog.hpp"

using namespace opencmd;

const Schema* SchemaCatalog::getSchema(const std::string& name) const {
    auto it = schemaMap.find(name);
    if (it != schemaMap.end()) {
        return &(it->second);
    } else {
        Logger::getInstance().log("Requested schema <" + name + "> does not exist in the loaded catalog", Logger::Level::WARNING);
    }
    return nullptr;
}

int SchemaCatalog::parseSchema(const std::string& name, const nlohmann::json& jsonSchema) {
    Logger::getInstance().log("Entering parseSchema", Logger::Level::DEBUG);
    Schema schema;
    schema.setCatalogName(name);
    if(jsonSchema.is_object()){
       for (const auto& [key, val] : jsonSchema.items()) {
            if(key=="structure" && val.type() == nlohmann::json::value_t::array){
                int result = parseSchemaStructure(val, schema.getStructureForUpdate());
                if(result <= 0){
                    Logger::getInstance().log("Error in parsing structure", Logger::Level::WARNING);
                    return 4;
                }
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

int SchemaCatalog::parseSchemaStructure(const nlohmann::json& jsonStructure, SchemaElement::SchemaElementArray& msgStructure){
    if(jsonStructure.type() != nlohmann::json::value_t::array){
        Logger::getInstance().log("Provided structure is not an array: " + std::to_string(static_cast<int>(jsonStructure.type())), Logger::Level::ERROR);
        return -1;
    }

    int totalInserted = 0;
    for (const auto &entry : jsonStructure) {
        Logger::getInstance().log("Calling parseSchemaElement", Logger::Level::DEBUG);
        SchemaElement::SchemaElementObject msgElement;
        int inserted = parseSchemaElement(entry, msgElement);
        if (inserted > 0) {
            msgStructure.emplace_back(msgElement);
            totalInserted++;
        } else if(inserted == 0){
            Logger::getInstance().log("No element to be parsed.", Logger::Level::WARNING);
        } else {
            Logger::getInstance().log("Failed to parse a schema element, skipping.", Logger::Level::ERROR);
            return -2;
        }
    }

    return totalInserted;
}

int SchemaCatalog::parseSchemaElement(const nlohmann::json& jsonElement, SchemaElement::SchemaElementObject& msgElement){

    if(jsonElement.type() != nlohmann::json::value_t::object){
        Logger::getInstance().log("Provided message element is not an object: " + std::to_string(static_cast<int>(jsonElement.type())), Logger::Level::ERROR);
        return -1;
    }

    int insertedCount = 0;

    for (const auto& [key, val] : jsonElement.items()) {
        if (key == "name" && val.type() == nlohmann::json::value_t::string) {
            msgElement.emplace("name", val.get<std::string>());
        } 
        else if (key == "bit_length" && (val.type() == nlohmann::json::value_t::number_integer || val.type() == nlohmann::json::value_t::number_unsigned)) {
            msgElement.emplace("bit_length", val.get<int64_t>());
        } 
        else if (key == "type" && val.type() == nlohmann::json::value_t::string) {
            msgElement.emplace("type", val.get<std::string>());
        } 
        else {
            Logger::getInstance().log("Unsupported key or value is not of the correct type: " + key, Logger::Level::DEBUG);
            continue;
        }
        ++insertedCount;
    }

    return insertedCount;
}

std::string SchemaCatalog::to_string(const SchemaElement::SchemaElementArray& elements) {
    std::ostringstream oss;
    for (const auto& element : elements) {
        oss << "- " << element.to_string(2) << "\n"; 
    }
    return oss.str();
}
