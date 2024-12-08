#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../bitstream/BitStream.hpp"
#include "../logger/Logger.hpp"


namespace opencmd {

    class TreeComponent {
    private:
        std::string name;
        std::string parentName;
    public:
        TreeComponent(std::string name = "", std::string parentName = "") 
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

        virtual std::unique_ptr<TreeComponent> clone() const = 0;

        virtual ~TreeComponent() = default;

        virtual int bitstream_to_json(BitStream&, nlohmann::json&) = 0;
        virtual int json_to_bitstream(nlohmann::json&, BitStream&) = 0;
    };
}

