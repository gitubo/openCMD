#pragma once

#include <vector>
#include <memory>
#include "TreeComponent.hpp"

namespace opencmd {

    class TreeNode : public TreeComponent {
    
    private:
        std::vector<std::shared_ptr<TreeComponent>> children;

    public:
        TreeNode(std::string name = "", std::string parentName = "/") 
            : TreeComponent(name, parentName) {}

        TreeNode(const TreeNode& other) : TreeComponent(other) { 
            for (const auto& child : other.children) {
                if (child) {
                    children.push_back(child->clone()); 
                }
            }
        }

        TreeNode& operator=(const TreeNode& other) {
            if (this != &other) {
                this->setName(other.getName());
                children.clear();
                for (const auto& child : other.children) {
                    if (child) {
                        children.push_back(child->clone());
                    }
                }
            }
            return *this;
        }

        std::unique_ptr<TreeComponent> clone () const override{
            return std::make_unique<TreeNode>(*this);
        }

        const std::vector<std::shared_ptr<TreeComponent>>& getChildren() const { 
            return children; 
        }

        void addChild(const std::shared_ptr<TreeComponent>& child) {
            child->setParentName(this->getFullName()); 
            children.push_back(child); 
        }

        int json_to_bitstream(nlohmann::json&, BitStream&) override { return 0; };
        int bitstream_to_json(BitStream&, nlohmann::json&) override { return 0; };

    };
}


