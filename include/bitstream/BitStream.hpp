#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <sstream>


namespace opencmd {
    class BitStream {
    private:
        std::unique_ptr<uint8_t[]> buffer; 
        size_t capacity;                   
        size_t offset;                     

        // Helper: copia i bit da un offset specifico
        void copyBits(uint8_t*, size_t, const uint8_t*, size_t, size_t) const;

    public:

        BitStream() : capacity(0), offset(0), buffer(nullptr) {}

        BitStream(const uint8_t*, size_t);

        BitStream(const std::string&);

        void set(const uint8_t*, size_t);

        std::unique_ptr<uint8_t[]> read(size_t) const;

        std::unique_ptr<uint8_t[]> consume(size_t);

        void append(const BitStream&);

        std::string to_string() const;

        size_t getCapacity() const { return capacity; }
        size_t getOffset() const { return offset;}
        
    private:

        static constexpr uint8_t INVALID_CHAR = 0xFF;

        static constexpr uint8_t base64_lookup[256] = {
            // 0-31
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            // 32-63
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, 62, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, 63,  // '+'=62, '/'=63
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  // A-Z
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
            36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  // a-z
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR,
            INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR, INVALID_CHAR
        };

        static constexpr char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        static size_t base64_decode(const std::string& encoded_string, std::unique_ptr<uint8_t[]>& decoded_data) {
            size_t input_length = encoded_string.length();
            if (input_length % 4 != 0) {
                throw std::invalid_argument("Invalid Base64 string length.");
            }

            size_t padding = 0;
            if (input_length > 0 && encoded_string[input_length - 1] == '=') padding++;
            if (input_length > 1 && encoded_string[input_length - 2] == '=') padding++;
            size_t output_length = (input_length / 4) * 3 - padding;

            decoded_data = std::make_unique<uint8_t[]>(output_length);

            size_t output_index = 0;
            uint32_t buffer = 0;
            int bits_collected = 0;

            for (char c : encoded_string) {
                if (c == '=') break; // Il padding non contribuisce ai dati decodificati
                uint8_t value = base64_lookup[static_cast<uint8_t>(c)];
                if (value == INVALID_CHAR) {
                    throw std::invalid_argument("Invalid character in Base64 string.");
                }
                buffer = (buffer << 6) | value; // Aggiunge 6 bit al buffer
                bits_collected += 6;
                if (bits_collected >= 8) {
                    bits_collected -= 8;
                    decoded_data[output_index++] = (buffer >> bits_collected) & 0xFF;
                }
            }
            if (output_index != output_length) {
                throw std::runtime_error("Decoding error: output length mismatch.");
            }
            return output_length;
        };

        static std::string base64_encode(const uint8_t* data, size_t input_length) {
            std::string encoded_string;
            encoded_string.reserve(((input_length + 2) / 3) * 4); 
            size_t i = 0;
            uint32_t buffer = 0;

            while (i < input_length) {
                buffer = data[i++] << 16; 
                if (i < input_length) buffer |= data[i++] << 8; 
                if (i < input_length) buffer |= data[i++];     

                encoded_string.push_back(base64_chars[(buffer >> 18) & 0x3F]);
                encoded_string.push_back(base64_chars[(buffer >> 12) & 0x3F]);
                if ((i - 2) < input_length) {
                    encoded_string.push_back(base64_chars[(buffer >> 6) & 0x3F]);
                } else {
                    encoded_string.push_back('='); // Padding
                }
                if ((i - 1) < input_length) {
                    encoded_string.push_back(base64_chars[buffer & 0x3F]);
                } else {
                    encoded_string.push_back('='); // Padding
                }
            }

            return encoded_string;
        };
    };
}


/* 
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
                            char_array_4[i] = std::find(BitStream::base64_chars, 
                                                        BitStream::base64_chars + 64, 
                                                        reinterpret_cast<const char*>(&char_array_4[i])) - BitStream::base64_chars;

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
                        char_array_4[j] = std::find(BitStream::base64_chars,
                                                    BitStream::base64_chars + 64,
                                                    reinterpret_cast<const char*>(&char_array_4[j])) - BitStream::base64_chars;


//                        char_array_4[j] = std::find(BitStream::base64_chars, BitStream::base64_chars + 64, char_array_4[j]) - BitStream::base64_chars;

                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                    for (j = 0; (j < i - 1); j++)
                        decoded_data[k++] = char_array_3[j];
                }
                return static_cast<size_t>(k);
            }

    }; */
//}
