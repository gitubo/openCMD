#pragma once

#include <iostream>
#include "TreeComponent.hpp"

namespace opencmd {

    class TreeElement : public TreeComponent{

    public:
        TreeElement(std::string name = "") : TreeComponent(name) {}

        std::unique_ptr<TreeComponent> clone () const override{
            return std::make_unique<TreeElement>(*this);
        }

        BitStream to_bitstream() const override {};
        nlohmann::json to_json() const override {};
        void from_bitstream(BitStream&) override {};
        void from_json(const nlohmann::json) override {};
    };
}

