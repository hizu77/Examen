#include <iostream>
#include <cstdint>


template<typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

static const uint8_t  kByteSize = 7;

class BoolReference{
public:
    BoolReference(uint8_t* byte, uint8_t current_bit)
        : byte_(byte)
        , current_bit_(current_bit)
    {}

    BoolReference& operator=(bool value) {
        if (value) {
            *byte_ |= (1 << (kByteSize - current_bit_));
        } else {
            *byte_ &= ~(1 << (kByteSize - current_bit_));
        }
        return *this;
    }


private:
    uint8_t* byte_;
    uint8_t current_bit_;
};


class BoolArray{
public:
    BoolArray(uint8_t* array, size_t size)
        : size_(size)
        , array_(new uint8_t[(size + 7) / 8])

    {
        for (size_t i = 0; i < (size + 7) / 8; ++i) {
            array_[i] = array[i];
        }
    }

    BoolArray()
        : size_(1)
        , array_(new uint8_t[1])
    {
        for (size_t i = 0; i < (size_ + 7) / 8; ++i) {
            array_[i] = 0;
        }
    }

    ~BoolArray() {
        delete[] array_;
    }

    BoolArray& operator=(const BoolArray& other) {
        if (this == &other) {
            return *this;
        }
        size_ = other.size_;
        delete[] array_;
        size_t size_in_bytes = (other.size_ + 7) / 8;
        array_ = new uint8_t[size_in_bytes];
        for (size_t i = 0; i < size_in_bytes; ++i) {
            array_[i] = other.array_[i];
        }
        return *this;
    }

    BoolArray(const BoolArray& other) {
        size_ = other.size_;
        size_t size_in_bytes = (other.size_ + 7) / 8;
        array_ = new uint8_t[size_in_bytes];
        for (size_t i = 0; i < size_in_bytes; ++i) {
            array_[i] = other.array_[i];
        }
    }

    BoolReference operator[](uint32_t index) {
        uint8_t current_bit = index % 8;
        size_t current_byte = index / 8;
        size_t old_size = size_;
        if (size_ <= index) {
            size_ = index + 1;
            auto* temp = new uint8_t[(size_ + 7) / 8];
            for (size_t i = 0; i < size_; ++i) {
                temp[i] = i < old_size ? array_[i] : 0;
            }
            delete[] array_;
            array_ = temp;
        }
        return {&array_[current_byte], current_bit};
    }

    friend std::ostream& operator<<(std::ostream& stream, const BoolArray& elem);

    BoolArray operator&(const BoolArray& other) {
        size_t max_len_in_byte = max(other.size_, size_);
        uint8_t result[(max_len_in_byte + 7) / 8];
        for (size_t i = 0; i < max_len_in_byte; ++i) {
            size_t current_byte = i / 8;
            size_t current_bit = i % 8;
            uint8_t first_bit = i < size_ ? (array_[current_byte] >> (kByteSize - current_bit)) & 1 : 0;
            uint8_t second_bit = i < size_ ? (other.array_[current_byte] >> (kByteSize - current_bit)) & 1 : 0;
            uint8_t result_bit = first_bit * second_bit;
            result[current_byte] |= (result_bit << (kByteSize - current_bit));
        }
        return {result, max_len_in_byte};
    }

    BoolArray operator|(const BoolArray& other) {
        size_t max_len_in_byte = max(other.size_, size_);
        uint8_t result[(max_len_in_byte + 7) / 8];
        for (size_t i = 0; i < max_len_in_byte; ++i) {
            size_t current_byte = i / 8;
            size_t current_bit = i % 8;
            uint8_t first_bit = i < size_ ? (array_[current_byte] >> (kByteSize - current_bit)) & 1 : 0;
            uint8_t second_bit = i < size_ ? (other.array_[current_byte] >> (kByteSize - current_bit)) & 1 : 0;
            uint8_t result_bit = first_bit + second_bit;
            result[current_byte] |= (result_bit << (kByteSize - current_bit));
        }
        return {result, max_len_in_byte};
    }

private:
    uint8_t* array_;
    size_t size_;
};

std::ostream& operator<<(std::ostream& stream, const BoolArray& elem) {
    for (size_t i = 0; i < elem.size_; ++i) {
        size_t current_byte = i / 8;
        size_t current_bit = i % 8;
        stream << (elem.array_[current_byte] >> (kByteSize - current_bit) & 1);
    }
    return stream;
}

int main() {

    BoolArray b;

    for(int i = 0; i < 4; ++i)  {
        b[i] = false;
    }

    b[0] = 1;
    b[3] = 1;

    std::cout << b << '\n';

    BoolArray b2;
    b2[0] = true;
    b2[1] = false;
    b2[2] = false;
    b2[3] = true;

    std::cout << b2 << '\n';

    BoolArray b3 = b2 | b;
    std::cout << b3;





    return 0;
}