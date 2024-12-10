#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../logger/Logger.hpp"
#include "SchemaElement.hpp"

namespace opencmd {

    // Definition of a schema describing the format of a message
    struct Schema{
        std::string catalogName;
        std::string version;
        std::map<std::string, std::string> metadata;
        SchemaElement::SchemaElementArray structure;
    };

    class SchemaCatalog {
    public:

        static SchemaCatalog& getInstance() {
            static SchemaCatalog instance; 
            return instance;
        }

        Schema* getSchema(const std::string&);
        int parseSchema(const std::string&, const nlohmann::json&);

    private:
        std::map<std::string, Schema> schemaMap;

    private:
        SchemaCatalog() = default;
        SchemaCatalog(const Logger&) = delete;
        SchemaCatalog& operator=(const SchemaCatalog&) = delete;

        SchemaElement::SchemaElementArray parseSchemaStructure(const nlohmann::json&);
        SchemaElement::SchemaElementObject parseSchemaElement(const nlohmann::json&);

        std::string printMessageElementList(const SchemaElement::SchemaElementObject&);

    };

}