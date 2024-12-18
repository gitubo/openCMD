#pragma once

#include "TreeNode.hpp"

namespace opencmd {
    class NodeUnsignedInteger : public TreeNode {

    public:
        enum class Endianness {
            BIG,
            LITTLE,
            MIDDLE
        };

    private:
        size_t bitLength = 0;
        Endianness endianness = Endianness::BIG;

    public:
        NodeUnsignedInteger() : TreeNode() {}

        NodeUnsignedInteger(std::string name) : TreeNode(){
            this->setName(name);
        }

        NodeUnsignedInteger(const NodeUnsignedInteger& other) : TreeNode(other), 
            bitLength(other.bitLength), 
            endianness(other.endianness) {}

        virtual std::shared_ptr<TreeNode> clone() const override { 
            return std::make_unique<NodeUnsignedInteger>(*this); 
        }

        void addAttribute(const std::string& key, const TreeNodeAttribute& attribute) override {
            TreeNode::addAttribute(key,attribute);
            
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

        int bitstream_to_json(BitStream& bitStream, nlohmann::ordered_json& outputJson) override {
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
            int64_t value = 0;
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

        int json_to_bitstream(const nlohmann::json& inputJson, BitStream& bitStream) override { 
            if(!inputJson.is_object()){
                Logger::getInstance().error("The provided json is not an object");
                return 100;
            }
            if(!inputJson.contains(this->getFullName()) || !inputJson[this->getFullName()].is_number_integer()){
                Logger::getInstance().error("Key <"+this->getFullName()+"> not found in the provided json object or the related value is not an integer");
                return 100;      
            }
            uint64_t rawValue = inputJson[this->getFullName()];
            uint64_t value = 0;
            switch (endianness){
                case Endianness::BIG:
                    value = rawValue;
/*                     for (int i = 0; i < sizeof(uint64_t); ++i) {
                        value |= ((rawValue >> (i * 8)) & 0xFF) << ((7 - i) * 8);
                    }
 */                    break;
                case Endianness::LITTLE:
                    // No convertion needed
                    value = rawValue;
                    break;
                case Endianness::MIDDLE:
                    Logger::getInstance().warning("Unsupported endianness: MIDDLE not (yet) implemented");
                    return 100;
                default:
                    Logger::getInstance().warning("Unsupported endianness");
                    return 100;
            }
            Logger::getInstance().debug("Appending <"+this->getFullName()+"> with value <"+std::to_string(rawValue)+"> (bits <"+std::to_string(bitLength)+">)");
     
            uint8_t inputBuffer[(bitLength+7)/8];
            std::memcpy(inputBuffer, &value, (bitLength+7)/8);
            BitStream bsInteger(inputBuffer, bitLength);
            bitStream.append(bsInteger);
            Logger::getInstance().debug("Post <"+bitStream.to_string()+">");
            return 0;
        }
    };
}