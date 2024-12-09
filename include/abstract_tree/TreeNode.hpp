#pragma once

#include <vector>
#include <memory>
#include "TreeComponent.hpp"

namespace opencmd {

    class TreeNode : public TreeComponent {
    
    private:
        std::vector<std::shared_ptr<TreeComponent>> children;

    public:
        TreeNode(std::string name = "") : TreeComponent(name) {}

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

        const std::vector<std::shared_ptr<TreeComponent>>& getChildren() const { return children; }

        void addChild(const std::shared_ptr<TreeComponent>& child) { children.push_back(child); }

        BitStream to_bitstream() const override {
            for (const auto& child : children) {
                child->to_bitstream(); 
            }
        }

        nlohmann::json to_json() const override {
            nlohmann::json result;
            for (const auto& child : children) {
                nlohmann::json child_json = child->to_json();
                result.update(child_json);
            }
            return result;
        }

        void from_bitstream(BitStream&) override {};
        void from_json(const nlohmann::json) override {};

    };
}


