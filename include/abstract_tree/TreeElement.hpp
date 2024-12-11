#pragma once

#include <iostream>
#include "TreeComponent.hpp"

namespace opencmd {

    class TreeElement : public TreeComponent{

    public:
        TreeElement(std::string name = "", std::string parentName = "/") 
            : TreeComponent(name, parentName) {}

        std::unique_ptr<TreeComponent> clone () const override{
            return std::make_unique<TreeElement>(*this);
        }


    };
}

