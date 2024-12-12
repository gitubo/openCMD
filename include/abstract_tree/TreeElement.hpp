#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../bitstream/BitStream.hpp"
#include "../logger/Logger.hpp"


namespace opencmd {

    class TreeElement {
    private:
        std::string name;
        std::string parentName;
    public:
        TreeElement(std::string name = "", std::string parentName = "") 
            : name(name), parentName(parentName) {}

        const std::string getName() const { return name; }
        const std::string getParentName() const { return parentName; }
        void setName(std::string name) { this->name = name; }
        void setParentName(std::string parentName) { 
            if (parentName.empty() || parentName.back() != '/') {
                parentName += '/';
            }
            this->parentName = parentName; 
        }
        std::string getFullName() { return parentName + name;}

        virtual ~TreeElement() = default;
        virtual std::unique_ptr<TreeElement> clone() const { 
            //return nullptr; 
            return std::make_unique<TreeElement>(*this);
        }
        virtual int json_to_bitstream(nlohmann::json&, BitStream&) { return 0; };
        virtual int bitstream_to_json(BitStream&, nlohmann::json&) { return 0; };
    };
}

