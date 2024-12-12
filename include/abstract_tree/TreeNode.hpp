#pragma once

#include <vector>
#include <memory>
#include "TreeElement.hpp"

namespace opencmd {

    class TreeNode : public TreeElement {
    
    private:
        std::vector<std::shared_ptr<TreeElement>> children;

    public:
        TreeNode(){}

        TreeNode(const TreeNode& other) : TreeElement(other) { 
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

        const std::vector<std::shared_ptr<TreeElement>>& getChildren() const { 
            return children; 
        }

        void addChild(const std::shared_ptr<TreeElement>& child) {
            child->setParentName(this->getFullName()); 
            children.push_back(child); 
        }


    };
}


