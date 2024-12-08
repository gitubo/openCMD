#pragma once

#include <iostream>
#include "TreeComponent.hpp"

namespace opencmd {

    class TreeElement : public TreeComponent{

    public:
        TreeElement(std::string name = "") : TreeComponent(name) {}

        std::shared_ptr<TreeComponent> clone() const override {
            auto cloned_entity = std::make_shared<TreeElement>(this->getName());
            return cloned_entity;
        }

        BitStream to_bitstream() const override {};
        nlohmann::json to_json() const override {};
        void from_bitstream(BitStream&) override {};
        void from_json(const nlohmann::json) override {};
    };
}

