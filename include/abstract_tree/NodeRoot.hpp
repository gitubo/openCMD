#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeRoot : public TreeNode {

    public:
        NodeRoot(std::string name = "/", std::string parentName = "") : TreeNode(name, parentName) {}

        int bitstream_to_json(BitStream& bitStream, nlohmann::json& outputJson) override {
            for (auto& child : this->getChildren()) {
                child->bitstream_to_json(bitStream, outputJson);
            }
            return 0; 
        };

    };
}

