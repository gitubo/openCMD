#pragma once

#include "TreeNode.hpp"

namespace opencmd {

    class NodeRoot : public TreeNode {

    public:
        NodeRoot() : TreeNode("/","") {}

        NodeRoot(const NodeRoot& other) : TreeNode(other) {}

        virtual std::unique_ptr<TreeNode> clone() const override { 
            return std::make_unique<NodeRoot>(*this); 
        }

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) override {
            for (auto& child : this->getChildren()) {
                auto retVal = child->bitstream_to_json(bitStream, outputJson);
                if(retVal){
                    return retVal;
                }
            }
            return 0; 
        };

        int json_to_bitstream(const nlohmann::json& inputJson, BitStream& bitStream) override {
            for (auto& child : this->getChildren()) {
                auto retVal = child->json_to_bitstream(inputJson, bitStream);
                if(retVal){
                    return retVal;
                }
            }
            return 0; 
        };


    };
}

