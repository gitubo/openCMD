#pragma once

#include <string>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include "../bitstream/BitStream.hpp"
#include "../logger/Logger.hpp"
#include "TreeNodeAttribute.hpp"


namespace opencmd {

    class TreeNode {

    private:
        std::string name;
        std::string parentName;
        std::map<std::string, TreeNodeAttribute> attributeMap;
        std::vector<std::shared_ptr<TreeNode>> children;

    public:

        TreeNode() : name(""), parentName("") {}
        TreeNode(std::string name, std::string parentName) : name(name), parentName(parentName) {}
        TreeNode(const TreeNode& other) : name(other.name), parentName(other.parentName) { 
            for(auto it = other.attributeMap.begin(); it != other.attributeMap.end(); ++it){
                attributeMap[it->first] = it->second;
            }
            children.reserve(other.children.size());
            for (const auto& child : other.children) {
                if (child) {
                    children.emplace_back(child->clone()); 
                }
            }
        }

        TreeNode& operator=(const TreeNode& other) {
            if (this != &other) {
                this->setName(other.getName());
                this->setParentName(other.getParentName());
                attributeMap.clear();
                for(auto it = other.getAttributeMap().begin(); it != other.getAttributeMap().end(); ++it){
                    attributeMap[it->first] = it->second;
                }
                children.clear();
                children.reserve(other.children.size());
                for (const auto& child : other.children) {
                    if (child) {
                        children.emplace_back(child->clone());
                    }
                }
            }
            return *this;
        }

        virtual std::unique_ptr<TreeNode> clone() const = 0;

        virtual ~TreeNode() = default;
        
        const std::string getName() const { return name; }
        const std::string getParentName() const { return parentName; }
        const std::string getFullName() const { return parentName + name;}
        const std::map<std::string, TreeNodeAttribute>& getAttributeMap() const { return attributeMap; }
        std::optional<TreeNodeAttribute> getAttribute(const std::string& key) const {
            auto it = attributeMap.find(key);
            if (it != attributeMap.end()) {
                return it->second;
            }
            return std::nullopt;
        }
        
        void setName(std::string name) { this->name = name; }
        void setParentName(std::string parentName) { 
            if (!parentName.empty() && parentName.back() != '/') {
                parentName += '/';
            }
            this->parentName = parentName; 
        }

        virtual void addAttribute(const std::string& key, const TreeNodeAttribute& attribute) {
            attributeMap[key] = attribute;
        }
        virtual void clearAttributes() { attributeMap.clear(); }

        const std::vector<std::shared_ptr<TreeNode>>& getChildren() const { return children; }

        void addChild(const std::shared_ptr<TreeNode>& child) {
            child->setParentName(this->getFullName()); 
            children.push_back(child); 
        }

        virtual int json_to_bitstream(const nlohmann::json&, BitStream&) = 0; //{ return 0; };
        virtual int bitstream_to_json(BitStream&, nlohmann::ordered_json&) = 0; //{ return 0; };

        virtual std::string to_string(size_t indent = 0) const { 
            std::ostringstream oss;
            std::string indentStr(indent, ' ');
            oss << indentStr << "{\n" << indentStr; 
            oss << indentStr << "  \"name\": \"" << name << "\",\n";
            oss << indentStr << "  \"parentName\": \"" << parentName << "\",\n";
            oss << indentStr << "  \"attributeMap\": {\n";
            for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it){
                oss << indentStr << "  \"" << it->first << "\": " << it->second.to_string();
                if (std::next(it) != attributeMap.end()) { 
                    oss << ",\n";
                } else {
                    oss << "\n";
                }
            }
            oss << indentStr << "  },\n";
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

