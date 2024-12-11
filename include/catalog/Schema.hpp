#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include "SchemaElement.hpp"

namespace opencmd {
    class Schema {
    public:
        std::string catalogName;
        std::string version;
        std::map<std::string, std::string> metadata;
        SchemaElement::SchemaElementArray structure;

        std::string to_string(size_t indent = 0) const {
            std::ostringstream oss;
            std::string indentStr(indent, ' ');
            oss << "{\n" << indentStr; 
            oss << indentStr << "  \"catalogName\": \"" << catalogName << "\",\n";
            oss << indentStr << "  \"version\": \"" << version << "\",\n";
            //oss << indentStr << "  \"metadata\": " << std::string(metadata) << ",\n";
            oss << indentStr << "  \"structure\": [\n";
            for (auto it = structure.begin(); it != structure.end(); ++it) {
                oss << indentStr << "  " << it->to_string(indent + 2);
                if (std::next(it) != structure.end()) { 
                    oss << ",\n";
                } else {
                    oss << "\n";
                }
            }
            oss << "]\n";
            oss << "}\n";
            return oss.str();
        }
    };

}