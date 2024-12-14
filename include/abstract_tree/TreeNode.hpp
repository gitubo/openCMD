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

        virtual std::string to_string(size_t indent = 0) const { 
            std::ostringstream oss;
            std::string indentStr(indent, ' ');
            oss << indentStr << "{\n" << indentStr; 
            oss << indentStr << "  \"name\": \"" << this->getName() << "\",\n";
            oss << indentStr << "  \"parentName\": \"" << this->getParentName() << "\",\n";
            oss << indentStr << "  \"attributeMap\": {\n";
            for(auto it = this->getAttributeMap().begin(); it != this->getAttributeMap().end(); ++it){
                oss << indentStr << "  \"" << it->first << "\": " << it->second.to_string();
                if (std::next(it) != this->getAttributeMap().end()) { 
                    oss << ",\n";
                } else {
                    oss << "\n";
                }
            }
            oss << indentStr << "  }\n";
            oss << indentStr << "  \"children\": [\n";
            for(const auto& child : children){
                oss << child->to_string(indent + 2);
            }
            oss << indentStr << "  ]\n";
            oss << indentStr << "}\n";
            return oss.str();
        }
    };
}


