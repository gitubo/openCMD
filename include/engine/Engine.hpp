#pragma once

#include <memory>

#include "../logger/Logger.hpp"
#include "../catalog/Schema.hpp"
#include "../abstract_tree/NodeRoot.hpp"

namespace opencmd {
    class Engine {
    private:
        static std::shared_ptr<TreeComponent> evaluateElement(const SchemaElement&);
    public:
        static std::shared_ptr<NodeRoot> evaluateSchema(const Schema&);
    };
}