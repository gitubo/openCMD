#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <string>

class BitStream {
private:
    std::unique_ptr<uint8_t[]> buffer; 
    size_t capacity;                   
    size_t offset;                     

    // Helper: copia i bit da un offset specifico
    void copyBits(uint8_t* dest, size_t destBitOffset, const uint8_t* src, size_t srcBitOffset, size_t length) const {
        for (size_t i = 0; i < length; ++i) {
            size_t srcByteIndex = (srcBitOffset + i) / 8;
            size_t srcBitIndex = 7 - ((srcBitOffset + i) % 8);

            bool bit = (src[srcByteIndex] >> srcBitIndex) & 1;

            size_t destByteIndex = (destBitOffset + i) / 8;
            size_t destBitIndex = 7 - ((destBitOffset + i) % 8);

            if (bit) {
                dest[destByteIndex] |= (1 << destBitIndex);
            } else {
                dest[destByteIndex] &= ~(1 << destBitIndex);
            }
        }
    }

public:

    BitStream() : capacity(0), offset(0), buffer(nullptr) {}

    BitStream(const uint8_t* inputBuffer, size_t initialCapacityInBits)
        : capacity(initialCapacityInBits), offset(0) {
        if (!inputBuffer || initialCapacityInBits == 0) {
            throw std::invalid_argument("BitStream::set - Input data is invalid or empty");
        }
        size_t byteCapacity = (capacity + 7) >> 3;
        buffer = std::make_unique<uint8_t[]>(byteCapacity);
        std::memcpy(buffer.get(), inputBuffer, byteCapacity);
    }

    BitStream(const std::string& base64_str) : offset(0) {
        if (base64_str.empty()) {
            throw std::invalid_argument("BitStream::set - Input base64 string is invalid or empty");
        }
        size_t lengthInBytes = base64_decode(base64_str, buffer);
        capacity = lengthInBytes * 8;
    }

    void set(const uint8_t* inputBuffer, size_t inputBitLength) {
        if (!inputBuffer || inputBitLength == 0) {
            throw std::invalid_argument("BitStream::set - Input data is invalid or empty");
        }
        capacity = inputBitLength;
        size_t byteCapacity = (capacity + 7) >> 3;            
        buffer = std::make_unique<uint8_t[]>(byteCapacity);
        std::memcpy(buffer.get(), inputBuffer, byteCapacity);
        offset = 0;
    }

    std::unique_ptr<uint8_t[]> read(size_t length) const {
        if (offset + length > capacity) {
            throw std::out_of_range("BitStream: you are trying to read beyond the end of the bit stream (offset + length > capacity)");
        }

        size_t byteLength = (length + 7) >> 3;
        auto result = std::make_unique<uint8_t[]>(byteLength);
        std::fill(result.get(), result.get() + byteLength, 0);

        copyBits(result.get(), 0, buffer.get(), offset, length);

        return result; 
    }

    std::unique_ptr<uint8_t[]> consume(size_t length) {
        if (offset + length > capacity) {
            throw std::out_of_range("BitStream: tentativo di consumare oltre la fine del buffer");
        }
        auto result = read(length); 
        offset += length;
        return result;
    }

    void append(const BitStream& other) {
        size_t newCapacity = capacity + other.capacity;
        size_t newByteCapacity = (newCapacity + 7) >> 3;

        auto newBuffer = std::make_unique<uint8_t[]>(newByteCapacity);
        std::fill(newBuffer.get(), newBuffer.get() + newByteCapacity, 0);

        size_t currentByteCapacity = (capacity + 7) / 8;
        std::memcpy(newBuffer.get(), buffer.get(), currentByteCapacity);

        copyBits(newBuffer.get(), capacity, other.buffer.get(), 0, other.capacity);

        buffer = std::move(newBuffer);
        capacity = newCapacity; 
    }

    std::string to_string() const {
        std::ostringstream oss;
        int lengthInBytes = (capacity + 7) >> 3;
        for(int i = 0; i < lengthInBytes; i++){
            for (int j = 7; j >= 0; j--) {
                oss << ((buffer[i] >> j) & 1);
            }
        }
        return oss.str();
    }

    size_t getCapacity() const { return capacity; }
    size_t getOffset() const { return offset;}
    
    private:
        static constexpr const char base64_chars[64] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
        };

        static inline bool is_base64(unsigned char c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }

        static size_t base64_decode(const std::string& encoded_string, std::unique_ptr<uint8_t[]>& decoded_data) {
            size_t in_len = encoded_string.size();
            size_t i = 0;
            size_t j = 0;
            size_t in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
            int k = 0;
            decoded_data = std::make_unique<uint8_t[]>((in_len * 3) / 4 + 1);

            while (in_len-- && (encoded_string[in_] != '=') && (isalnum(encoded_string[in_]) || (encoded_string[in_] == '+') || (encoded_string[in_] == '/'))) {
                char_array_4[i++] = encoded_string[in_]; in_++;
                if (i == 4) {
                    for (i = 0; i < 4; i++)
                        char_array_4[i] = std::find(BitStream::base64_chars, BitStream::base64_chars + 64, char_array_4[i]) - BitStream::base64_chars;

                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                    for (i = 0; i < 3; i++)
                        decoded_data[k++] = char_array_3[i];
                    i = 0;
                }
            }

            if (i) {
                for (j = i; j < 4; j++)
                    char_array_4[j] = 0;

                for (j = 0; j < 4; j++)
                    char_array_4[j] = std::find(BitStream::base64_chars, BitStream::base64_chars + 64, char_array_4[j]) - BitStream::base64_chars;

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (j = 0; (j < i - 1); j++)
                    decoded_data[k++] = char_array_3[j];
            }
            return static_cast<size_t>(k);
        }

};
