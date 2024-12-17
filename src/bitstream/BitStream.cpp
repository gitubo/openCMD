#include "../../include/bitstream/BitStream.hpp"

using namespace opencmd;

BitStream::BitStream(const uint8_t* inputBuffer, size_t initialCapacityInBits)
    : capacity(initialCapacityInBits), offset(0) {
    if (!inputBuffer || initialCapacityInBits == 0) {
        throw std::invalid_argument("BitStream::set - Input data is invalid or empty");
    }
    size_t byteCapacity = (capacity + 7) / 8;
    buffer = std::make_unique<uint8_t[]>(byteCapacity);
    std::memcpy(buffer.get(), inputBuffer, byteCapacity);
}

BitStream::BitStream(const std::string& base64_str) : offset(0) {
    if (base64_str.empty()) {
        throw std::invalid_argument("BitStream::set - Input base64 string is invalid or empty");
    }
    size_t lengthInBytes = BitStream::base64_decode(base64_str, buffer);
    capacity = lengthInBytes * 8;
}

void BitStream::copyBits(uint8_t* dest, size_t destBitOffset, const uint8_t* src, size_t srcBitOffset, size_t length) const {
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

void BitStream::set(const uint8_t* inputBuffer, size_t inputBitLength) {
    if (!inputBuffer || inputBitLength == 0) {
        throw std::invalid_argument("BitStream::set - Input data is invalid or empty");
    }
    capacity = inputBitLength;
    size_t byteCapacity = (capacity + 7) >> 3;            
    buffer = std::make_unique<uint8_t[]>(byteCapacity);
    std::memcpy(buffer.get(), inputBuffer, byteCapacity);
    offset = 0;
}

void BitStream::reduceCapacity(const size_t newCapacity) {
    if(newCapacity>=capacity){
        return;
    }
    size_t byteCapacity = (capacity + 7) / 8;
    size_t newByteCapacity = (newCapacity + 7) / 8;
    std::unique_ptr<uint8_t[]> tempBuffer = std::make_unique<uint8_t[]>(newByteCapacity);
    int byteIndex = byteCapacity-1;
    for(int newByteIndex = newByteCapacity-1; newByteIndex>=0; newByteIndex--){
        tempBuffer[newByteIndex] = buffer[byteIndex--];
    }
    buffer = std::move(tempBuffer);
    capacity=newCapacity;
}
        
std::unique_ptr<uint8_t[]> BitStream::read(size_t length) const {
    if (offset + length > capacity) {
        throw std::out_of_range("BitStream: you are trying to read beyond the end of the bit stream (offset + length > capacity)");
    }

    size_t byteLength = (length + 7) >> 3;
    auto result = std::make_unique<uint8_t[]>(byteLength);
    std::fill(result.get(), result.get() + byteLength, 0);

    copyBits(result.get(), 0, buffer.get(), offset, length);

    return result; 
}

std::unique_ptr<uint8_t[]> BitStream::consume(size_t length) {
    if (offset + length > capacity) {
        throw std::out_of_range("BitStream: tentativo di consumare oltre la fine del buffer");
    }
    auto result = read(length); 
    offset += length;
    return result;
}

/*
void BitStream::append(const BitStream& other) {

    size_t currentByteCapacity = (capacity + 7) / 8;
    size_t newCapacity = capacity + other.capacity;
    size_t newByteCapacity = (newCapacity + 7) / 8;

    std::unique_ptr<uint8_t[]> tempBuffer = std::make_unique<uint8_t[]>(newByteCapacity);
    std::fill(tempBuffer.get(), tempBuffer.get() + newByteCapacity, 0);

    int byteIndex = newByteCapacity-1;
    int currentByteIndex = currentByteCapacity-1;
    for(; byteIndex>=0 && currentByteIndex>=0; byteIndex--, currentByteIndex--){
        tempBuffer[byteIndex] = buffer[currentByteIndex];
    }

    std::memcpy(newBuffer.get(), buffer.get(), currentByteCapacity);

    copyBits(newBuffer.get(), capacity, other.buffer.get(), 0, other.capacity);

    buffer = std::move(newBuffer);
    capacity = newCapacity; 
}*/


void BitStream::append(const BitStream& other) {

    size_t newCapacity = capacity + other.capacity;
    size_t newByteCapacity = (newCapacity + 7) / 8;

    std::unique_ptr<uint8_t[]> newBuffer = std::make_unique<uint8_t[]>(newByteCapacity);
    std::fill(newBuffer.get(), newBuffer.get() + newByteCapacity, 0);

    size_t currentByteCapacity = (capacity + 7) / 8;
    std::memcpy(newBuffer.get(), buffer.get(), currentByteCapacity);

    copyBits(newBuffer.get(), capacity, other.buffer.get(), 0, other.capacity);

    buffer = std::move(newBuffer);
    capacity = newCapacity; 
}


int BitStream::shift(const size_t shiftAmount, const bool shiftRight) {
    if (shiftAmount == 0 || capacity == 0){
        return 1;
    }

    if (shiftAmount > capacity) {
        // shifting more bits than the actual capacity 
        std::fill(buffer.get(), buffer.get()+capacity, 0);
        return 0;
    }

    size_t capacityInBytes = (capacity + 7) / 8;
    std::unique_ptr<uint8_t[]> tempBuffer = std::make_unique<uint8_t[]>(capacityInBytes);
    std::fill(tempBuffer.get(), tempBuffer.get() + capacityInBytes, 0);

    if (shiftRight) {
        // Right Shift
/* TBD */
/*
        size_t shiftByte = (shiftAmount+7) / 8;
        size_t shiftBit  = shiftAmount % 8;

        for(int byteIndex = capacityInBytes-1; byteIndex>=0; byteIndex--){

            if(byteIndex+shiftByte > capacityInBytes) continue;

            uint16_t aux16 = buffer[byteIndex];
            aux16 <<= shiftBit;
            uint8_t carry = (aux16 >> 8) & 0xFF;
            uint8_t value = aux16 & 0xFF;

            tempBuffer[byteIndex+shiftByte] |= carry;
            tempBuffer[byteIndex+shiftByte-1] = value;
        }
*/
    } else {
        // Left Shift
        size_t shiftByte = (shiftAmount+7) / 8;
        size_t shiftBit  = shiftAmount % 8;

        for(int byteIndex = capacityInBytes-1; byteIndex>=0; byteIndex--){

            if(byteIndex+shiftByte > capacityInBytes) continue;

            uint16_t aux16 = buffer[byteIndex];
            aux16 <<= shiftBit;
            uint8_t carry = (aux16 >> 8) & 0xFF;
            uint8_t value = aux16 & 0xFF;

            tempBuffer[byteIndex+shiftByte] |= carry;
            tempBuffer[byteIndex+shiftByte-1] = value;
        }
    }
    std::memcpy(buffer.get(), tempBuffer.get(), capacityInBytes); 
    return 0;
}

std::string BitStream::to_string() const {
    std::ostringstream oss;
    int lengthInBytes = (capacity + 7) / 8;
    int bitsInLastByte = capacity % 8;
    int minBit = 0;
    for(int i = lengthInBytes - 1; i >= 0; i--){
        if(i==0){
            minBit = 7-bitsInLastByte+1;
        }
        for (int j = 7; j >= minBit; j--) {
            oss << ((buffer[i] >> j) & 1);
        }
    }
    return oss.str();
}




