#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "../bitstream/BitStream.hpp"
#include "../logger/Logger.hpp"


namespace opencmd {

    class TreeComponent {
    private:
        std::string name;
    public:
        TreeComponent(std::string name = "") : name(name) {}
        const std::string getName() const { return name; }
        virtual std::shared_ptr<TreeComponent> clone() const = 0;
        virtual ~TreeComponent() = default;
        virtual BitStream to_bitstream() const = 0;
        virtual nlohmann::json to_json() const = 0;
        virtual void from_bitstream(BitStream&) = 0;
        virtual void from_json(const nlohmann::json) = 0;
    };
}

