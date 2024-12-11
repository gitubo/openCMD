#pragma once

#include "TreeElement.hpp"

namespace opencmd {

    class ElementUnsignedInteger : public TreeElement {

    public:
        enum class Endianness {
            BIG,
            LITTLE,
            MIDDLE
        };

    private:
        uint64_t value;
        size_t bitLength;
        Endianness endianness;

    public:

        ElementUnsignedInteger(std::string name, size_t bitLength, Endianness endianness = Endianness::BIG)
            : TreeElement(name), value(0), bitLength(bitLength), endianness(endianness) {}

        std::unique_ptr<TreeComponent> clone() const override {
            return std::make_unique<ElementUnsignedInteger>(*this);
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
                    Logger::getInstance().log("Unsupported endianness: MIDDLE_ENDIAN not yet implemented", Logger::Level::WARNING);
                    return 100;
                default:
                    Logger::getInstance().log("Unsupported endianness", Logger::Level::WARNING);
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
        };


        int json_to_bitstream(nlohmann::json&, BitStream&) override {
            return 0;
        };

    };
}