#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../logger/Logger.hpp"
#include "../abstract_tree/TreeFactory.hpp"
#include "../abstract_tree/NodeArray.hpp"
#include "../abstract_tree/NodeUnsignedInteger.hpp"
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
            Logger::getInstance().log("After create", Logger::Level::DEBUG);
            if(obj){
                Logger::getInstance().log("Object created", Logger::Level::DEBUG);
            }
            return obj;
        }

    private:
        std::map<std::string, Schema> schemaMap;

    private:
        SchemaCatalog() {
            TreeFactory::getInstance().registerClass<NodeArray>("array");
            TreeFactory::getInstance().registerClass<NodeUnsignedInteger>("unsigned integer");
        };
        SchemaCatalog& operator=(const SchemaCatalog&) = delete;

        int parseSchemaStructure(const nlohmann::json&, SchemaElement::SchemaElementArray&);
        int parseSchemaElement(const nlohmann::json&, SchemaElement::SchemaElementObject&);
    };

}