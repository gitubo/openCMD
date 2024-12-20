#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include "SchemaElement.hpp"

namespace opencmd {
    class Schema {
    private:
        std::string catalogName;
        std::string version;
        std::map<std::string, std::string> metadata;
        //std::vector<SchemaElement> structure;

    public:
        std::shared_ptr<NodeRoot> abstractTree;
        Schema() = default;
        const std::string getCatalogName() const { return this->catalogName; }
        const std::string getVersion() const { return this->version; }
        const std::map<std::string, std::string> getMetadata() const { return this->metadata; }
        const std::shared_ptr<NodeRoot>& getAbstractTree() const { return this->abstractTree; }
        
        //const std::vector<SchemaElement>& getStructure() const { return this->structure; }
        //std::vector<SchemaElement>& getStructureForUpdate() { return this->structure; }
        
        void setCatalogName(const std::string& name){ this->catalogName = name;}
        void setVersion(const std::string& version){ this->version = version;}
        void setMetadata(const std::map<std::string, std::string>& metadata){ this->metadata = metadata;}

        std::string to_string(size_t indent = 0) const {
            std::ostringstream oss;
            std::string indentStr(indent, ' ');
            oss << "{\n" << indentStr; 
            oss << indentStr << "  \"catalogName\": \"" << catalogName << "\",\n";
            oss << indentStr << "  \"version\": \"" << version << "\",\n";
            //oss << indentStr << "  \"metadata\": " << std::string(metadata) << ",\n";
            oss << indentStr << "  \"structure\": " << abstractTree->to_string() << "\n";
            oss << "}\n";
            return oss.str();
        }
    };

}