#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../logger/Logger.hpp"
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

    private:
        std::map<std::string, Schema> schemaMap;

    private:
        SchemaCatalog() = default;
        SchemaCatalog(const Logger&) = delete;
        SchemaCatalog& operator=(const SchemaCatalog&) = delete;

        int parseSchemaStructure(const nlohmann::json&, SchemaElement::SchemaElementArray&);
        int parseSchemaElement(const nlohmann::json&, SchemaElement::SchemaElementObject&);
    };

}