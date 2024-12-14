#pragma once

#include <memory>
#include <optional>

#include "../logger/Logger.hpp"
#include "../catalog/Schema.hpp"
#include "../catalog/SchemaCatalog.hpp"
#include "../abstract_tree/NodeRoot.hpp"
#include "../abstract_tree/TreeFactory.hpp"

namespace opencmd {
    class Engine {
    private:
        static std::optional<std::shared_ptr<TreeElement>> evaluateElement(const std::map<std::string, SchemaElement>&);
    public:
        static std::optional<std::shared_ptr<NodeRoot>> evaluateStructure(const std::vector<SchemaElement>&, const std::string&);
    };
}