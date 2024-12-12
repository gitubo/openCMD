#pragma once

#include <memory>
#include <optional>

#include "../logger/Logger.hpp"
#include "../catalog/Schema.hpp"
#include "../catalog/SchemaCatalog.hpp"
#include "../abstract_tree/NodeRoot.hpp"

namespace opencmd {
    class Engine {
    private:
        static std::optional<std::shared_ptr<TreeElement>> evaluateElement(const SchemaElement&);
    public:
        static std::optional<std::shared_ptr<NodeRoot>> evaluateSchema(const Schema&);
    };
}