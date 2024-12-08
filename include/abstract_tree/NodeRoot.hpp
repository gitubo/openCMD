#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeRoot : public TreeNode {

    public:
        NodeRoot(std::string name = "root") : TreeNode(name) {}

        void from_bitstream(BitStream& bitstream) override {
            for (auto& child : this->getChildren()) {
                child->from_bitstream(bitstream);
            }
        };

        void from_json(const nlohmann::json json) override {};

    };
}

