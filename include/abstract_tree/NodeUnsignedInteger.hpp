#pragma once

#include "TreeElement.hpp"

namespace opencmd {
    class NodeUnsignedInteger : public TreeElement {

    public:
        enum class Endianness {
            BIG,
            LITTLE,
            MIDDLE
        };

    private:
        uint64_t value = 0;
        size_t bitLength = 0;
        Endianness endianness = Endianness::BIG;

    public:
        NodeUnsignedInteger() : TreeElement() {}

        NodeUnsignedInteger(std::string name) : TreeElement(){
            this->setName(name);
        }

        void addAttribute(const std::string& key, const TreeElementAttribute& attribute) override {
            TreeElement::addAttribute(key,attribute);
            
            if(key=="bit_length"){
                if(!attribute.isInteger()){
                    Logger::getInstance().log("Attribute <bit_length> is not an integer", Logger::Level::ERROR);
                } else {
                    bitLength = attribute.getInteger().value();
                    if(bitLength<=0){
                        Logger::getInstance().log("Attribute <bit_length> is not valid (<=0)", Logger::Level::ERROR);
                        bitLength = 0;
                    }
                }
            } else if(key=="endianness"){
                if(!attribute.isString()){
                    Logger::getInstance().log("Attribute <endianness> is not a string", Logger::Level::ERROR);
                } else {
                    auto endianess_str = attribute.getString().value();
                    if(endianess_str.compare("big")){
                        endianness=Endianness::BIG;
                    } else if(endianess_str.compare("little")){
                        endianness=Endianness::LITTLE;
                    } else if(endianess_str.compare("middle")){
                        endianness=Endianness::MIDDLE;
                    } else {
                        Logger::getInstance().log("Attribute <endianness> is not valid ("+endianess_str+")", Logger::Level::ERROR);
                    }
                }
            }
        }

        std::unique_ptr<TreeElement> clone() const override {
            return std::make_unique<NodeUnsignedInteger>(*this);
        }

        int bitstream_to_json(BitStream& bitStream, nlohmann::json& outputJson) override {
            size_t numberOfBytes = bitLength + 7 >> 3;
            auto buffer = bitStream.consume(bitLength);
            uint64_t result = 0;
            switch (endianness){
                case Endianness::BIG:
                    for (size_t i = 0; i < numberOfBytes; ++i) {
                        result |= buffer[i] << ((numberOfBytes - 1 - i) * 8);
                    }
                    break;
                case Endianness::LITTLE:
                    for (size_t i = 0; i < numberOfBytes; ++i) {
                        result |= buffer[i] << (i * 8);
                    }
                    break;
                case Endianness::MIDDLE:
                    Logger::getInstance().warning("Unsupported endianness: MIDDLE not (yet) implemented");
                    return 100;
                default:
                    Logger::getInstance().warning("Unsupported endianness");
                    return 100;
            }

            // Align to the right if the value is not a multiple of 8
            if (bitLength % 8) {
                value = result >> (8 - (bitLength % 8));
            } else {
                value = result;
            }

            // Prepare the json output
            std::string key = this->getFullName();
            outputJson[key] = value;
            return 0;
        }

        int json_to_bitstream(nlohmann::json&, BitStream&) override { return 0; }
    };
}