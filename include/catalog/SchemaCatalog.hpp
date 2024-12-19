#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <optional>
#include <nlohmann/json.hpp>

#include "../logger/Logger.hpp"
#include "../abstract_tree/TreeFactory.hpp"
#include "../abstract_tree/NodeRoot.hpp"
#include "../abstract_tree/NodeRouter.hpp"
#include "../abstract_tree/NodeArray.hpp"
#include "../abstract_tree/NodeUnsignedInteger.hpp"
#include "Schema.hpp"

namespace opencmd {

    class SchemaCatalog {
    private:
        std::unordered_map<std::string, Schema> schemaMap;

    public:

        static SchemaCatalog& getInstance() {
            static SchemaCatalog instance; 
            return instance;
        }

        int parseSchema(const std::string&, const nlohmann::json&);
        std::shared_ptr<TreeNode> cloneAbstractTree(const std::string& key) const;
        std::string to_string(const SchemaElement::SchemaElementArray&);

    private:
        SchemaCatalog() {

            TreeFactory::getInstance().registerClass<NodeArray>("array");
            TreeFactory::getInstance().registerClass<NodeRouter>("router");

            TreeFactory::getInstance().registerClass<NodeUnsignedInteger>("unsigned integer");

        };
        SchemaCatalog& operator=(const SchemaCatalog&) = delete;

        std::optional<std::shared_ptr<TreeNode>> evalArray(const nlohmann::json&);
        std::optional<std::shared_ptr<TreeNode>> evalObject(const nlohmann::json&);
        std::optional<std::shared_ptr<TreeNodeAttribute>> evalAttribute(const nlohmann::json&);
    };

}