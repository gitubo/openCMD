#pragma once

#include "TreeElement.hpp"

namespace opencmd {

    class NodeRoot : public TreeElement {

    public:
        NodeRoot() : TreeElement() {
            this->setName("/");
            this->setParentName("");
         }

        int bitstream_to_json(BitStream& bitStream, nlohmann::json& outputJson) override {
            for (auto& child : this->getChildren()) {
                child->bitstream_to_json(bitStream, outputJson);
            }
            return 0; 
        };

    };
}

