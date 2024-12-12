#pragma once

#include <string>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include "../bitstream/BitStream.hpp"
#include "../logger/Logger.hpp"


namespace opencmd {

    using Attribute = std::variant<
            bool,
            double,
            int64_t,
            std::string
        >;

    class TreeElement {

    private:
        std::string name;
        std::string parentName;
        std::map<std::string, Attribute> attributeMap;

    public:

        TreeElement() : name(""), parentName("") {}
        TreeElement(std::string name, std::string parentName)
            : name(name), parentName(parentName) {}
        virtual ~TreeElement() = default;
        
        const std::string getName() const { return name; }
        const std::string getParentName() const { return parentName; }
        const std::string getFullName() const { return parentName + name;}
        const std::map<std::string, Attribute>& getAttributeMap() const { return attributeMap; }
        const std::optional<Attribute> getAttribute(const std::string& key) const {
            if(attributeMap.find(key) != attributeMap.end()){
                return attributeMap.at(key);
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
        void addAttribute(const std::string& key, const Attribute& attribute) {
            attributeMap[key] = attribute;
        }
        void clearAttributes() { attributeMap.clear(); }

        virtual std::unique_ptr<TreeElement> clone() const { 
            return std::make_unique<TreeElement>(*this);
        }
        virtual int json_to_bitstream(nlohmann::json&, BitStream&) { return 0; };
        virtual int bitstream_to_json(BitStream&, nlohmann::json&) { return 0; };
    };
}

