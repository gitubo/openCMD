#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../logger/Logger.hpp"
#include "../abstract_tree/TreeFactory.hpp"
#include "../abstract_tree/ElementUnsignedInteger.hpp"
#include "Schema.hpp"

namespace opencmd {

    class SchemaCatalog {
    public:

        static SchemaCatalog& getInstance() {
            static SchemaCatalog instance; 
            return instance;
        }

        const Schema* getSchema(const std::string&) const;
        int parseSchema(const std::string&, const nlohmann::json&);

        std::string to_string(const SchemaElement::SchemaElementArray&);

        std::unique_ptr<TreeElement> wrap_create(const std::string& className){
            auto obj = TreeFactory::getInstance().create(className);
            if(obj){
                Logger::getInstance().log("Object created", Logger::Level::DEBUG);
            }
            return obj;
        }

    private:
        std::map<std::string, Schema> schemaMap;

    private:
        SchemaCatalog() {
            TreeFactory::getInstance().registerClass<ElementUnsignedInteger>("unsigned integer");
        };
        //SchemaCatalog(const Logger&) = delete;
        SchemaCatalog& operator=(const SchemaCatalog&) = delete;

        int parseSchemaStructure(const nlohmann::json&, SchemaElement::SchemaElementArray&);
        int parseSchemaElement(const nlohmann::json&, SchemaElement::SchemaElementObject&);
    };

}