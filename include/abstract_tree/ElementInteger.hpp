#pragma once

#include "TreeElement.hpp"

namespace opencmd {

    class ElementInteger : public TreeElement {

    public:
        enum class EndiannessType {
            BIG_ENDIAN = 0,
            LITTLE_ENDIAN = 1,
            MIDDLE_ENDIAN = 2
        };

    private:
        int value;
        size_t bitLength;
        EndiannessType endianness;

    public:

        ElementInteger(std::string name, int value, size_t bitLength, EndiannessType endianness = EndiannessType::BIG_ENDIAN) 
            : TreeElement(name), value(value), bitLength(bitLength), endianness(endianness) {}

        std::unique_ptr<TreeComponent> clone() const override {
            return std::make_unique<ElementInteger>(*this);
        }

        BitStream to_bitstream() const override {
            std::cout << "ElementInteger to bitstream value: " << value << " and bit_length: " << bitLength << std::endl;
        }

        nlohmann::json to_json() const override {
            nlohmann::json result;
            result[std::string("/") + std::string(this->getName())] = value; 
            return result;
        }

        void from_bitstream(BitStream& bitStream) override {
            size_t numberOfBytes = bitLength + 7 >> 3;
/*             size_t bits = 0;
            switch(numberOfBytes){
                case 1:
                    bits = 8;
                    break;
                case 2:
                    bits=16;
                    break;
                case 4:
                    bits=32;
                    break;
                case 8:
                    bits=64;
                    break;
                default:
                    Logger::getInstance().log("Unsupported number of bytes: " + std::to_string(numberOfBytes), Logger::Level::ERROR);
                    return;
            } */
            auto buffer = bitStream.consume(bitLength);

            int result = 0;
            switch (endianness){
                case EndiannessType::BIG_ENDIAN:
                    for (size_t i = 0; i < numberOfBytes; ++i) {
                        result |= buffer[i] << ((numberOfBytes - 1 - i) * 8);
                    }
                    break;
                case EndiannessType::LITTLE_ENDIAN:
                    for (size_t i = 0; i < numberOfBytes; ++i) {
                        result |= buffer[i] << (i * 8);
                    }
                    break;
                case EndiannessType::MIDDLE_ENDIAN:
                    Logger::getInstance().log("Unsupported endianness: MIDDLE_ENDIAN not yet implemented", Logger::Level::WARNING);
                    break;
                default:
                    Logger::getInstance().log("Unsupported endianness ", Logger::Level::WARNING);
                    break;
            }

            // Align to the right if the extracted value is not a multiple of 8
            if(bitLength % 8){
                value = result >> (8 - (bitLength % 8));
            } else {
                value = result;
            }
        };

        void from_json(const nlohmann::json json) override {};

    };
}

