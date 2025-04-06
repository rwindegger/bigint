//
// Created by Rene Windegger on 21/03/2025.
//

#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <compare>
#include <cstddef>
#include <cstdint>
#ifndef bigint_DISABLE_IO
#include <iostream>
#endif
#include <stdexcept>
#include <string>
#include <string_view>

namespace bigint {
    template<std::size_t bits, bool is_signed>
    class bigint {
    private:
        std::array<std::uint8_t, bits / CHAR_BIT> data_{};

        [[nodiscard]] bool get_bit(std::size_t const pos) const {
            std::size_t byte_index = pos / 8;
            std::size_t const bit_index = pos % 8;
            if constexpr (std::endian::native == std::endian::little) {
                return (data_[byte_index] >> bit_index) & 1;
            } else {
                std::size_t index = data_.size() - 1 - byte_index;
                return (data_[index] >> bit_index) & 1;
            }
        }

        void set_bit(std::size_t const pos, bool const value) {
            std::size_t byte_index = pos / 8;
            std::size_t const bit_index = pos % 8;
            if constexpr (std::endian::native == std::endian::little) {
                if (value) {
                    data_[byte_index] |= static_cast<std::uint8_t>(1U << bit_index);
                } else {
                    data_[byte_index] &= static_cast<std::uint8_t>(~(1U << bit_index));
                }
            } else {
                std::size_t index = data_.size() - 1 - byte_index;
                if (value) {
                    data_[index] |= static_cast<std::uint8_t>(1U << bit_index);
                } else {
                    data_[index] &= static_cast<std::uint8_t>(~(1U << bit_index));
                }
            }
        }

        void multiply_by(std::uint32_t const multiplier) {
            std::uint32_t carry = 0;
            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < data_.size(); ++i) {
                    std::uint32_t const prod = static_cast<std::uint32_t>(data_[i]) * multiplier + carry;
                    data_[i] = static_cast<std::uint8_t>(prod & 0xFF);
                    carry = prod >> 8;
                }
            } else {
                for (std::size_t i = data_.size(); i > 0; --i) {
                    std::size_t const idx = i - 1;
                    std::uint32_t const prod = static_cast<std::uint32_t>(data_[idx]) * multiplier + carry;
                    data_[idx] = static_cast<std::uint8_t>(prod & 0xFF);
                    carry = prod >> 8;
                }
            }
            if (carry != 0) {
                throw std::overflow_error("Overflow during multiplication");
            }
        }

        void add_value(std::uint8_t const value) {
            std::uint16_t carry = value;
            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < data_.size() and carry; ++i) {
                    std::uint16_t const sum = static_cast<std::uint16_t>(data_[i]) + carry;
                    data_[i] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                }
            } else {
                for (std::size_t i = data_.size(); i > 0 and carry; --i) {
                    std::size_t const idx = i - 1;
                    std::uint16_t const sum = static_cast<std::uint16_t>(data_[idx]) + carry;
                    data_[idx] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                }
            }
            if (carry != 0) {
                throw std::overflow_error("Overflow during addition");
            }
        }

        void init_from_string_base(char const *const str, std::size_t const length, std::uint32_t const base) {
            std::fill(data_.begin(), data_.end(), 0);
            for (std::size_t i = 0; i < length; ++i) {
                char const c = str[i];
                if (c == '\'' or c == ' ')
                    continue;
                std::uint8_t digit = 0;
                if (c >= '0' and c <= '9') {
                    digit = c - '0';
                } else if (c >= 'a' and c <= 'f') {
                    digit = 10 + (c - 'a');
                } else if (c >= 'A' and c <= 'F') {
                    digit = 10 + (c - 'A');
                } else {
                    throw std::runtime_error("Invalid digit in input string.");
                }
                if (digit >= base) {
                    throw std::runtime_error("Digit out of range for base.");
                }
                multiply_by(base);
                add_value(digit);
            }
        }

        template<std::integral T>
        void init(T const data) {
            static_assert(bits / CHAR_BIT >= sizeof(T),
                          "Can't assign values with a larger bit count than the target type.");
            std::uint8_t fill = 0;
            if constexpr (std::is_signed_v<T>) {
                fill = (data < 0 ? 0xFF : 0);
            }

            if constexpr (std::endian::native == std::endian::little) {
                std::copy_n(reinterpret_cast<const std::uint8_t *>(std::addressof(data)), sizeof(T), data_.begin());
                std::fill_n(data_.begin() + sizeof(T), data_.size() - sizeof(T), fill);
            } else {
                std::copy_n(reinterpret_cast<const std::uint8_t *>(std::addressof(data)), sizeof(T),
                            data_.end() - sizeof(T));
                std::fill_n(data_.begin(), data_.size() - sizeof(T), fill);
            }
        }

        template<std::size_t other_bits, bool other_is_signed>
        void init(bigint<other_bits, other_is_signed> other) {
            static_assert(bits >= other_bits, "Can't assign values with a larger bit count than the target type.");
            if (other_is_signed and other < static_cast<std::int8_t>(0)) {
                std::fill_n(data_.data(), data_.size(), 0xFF);
            } else {
                std::fill_n(data_.data(), data_.size(), 0);
            }
            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < other.data_.size(); ++i) {
                    data_[i] = other.data_[i];
                }
            } else {
                for (std::size_t i = other.data_.size(); i > 0; --i) {
                    data_[data_.size() - i - 1] = other.data_[other.data_.size() - i - 1];
                }
            }
        }

        void init(std::string_view const str) {
            if (str.length() > 2 and str[0] == '0') {
                switch (str[1]) {
                    case 'x':
                        init_from_string_base(&str[2], str.length() - 2, 16);
                        break;
                    case 'b':
                        init_from_string_base(&str[2], str.length() - 2, 2);
                        break;
                    default:
                        init_from_string_base(&str[1], str.length() - 1, 8);
                        break;
                }
            } else {
                if (str[0] == '-') {
                    if constexpr (!is_signed) {
                        throw std::runtime_error("Cannot initialize an unsigned bigint23 with a negative value.");
                    } else {
                        init_from_string_base(&str[1], str.length() - 1, 10);
                        *this = -*this;
                    }
                } else {
                    init_from_string_base(&str[0], str.length(), 10);
                }
            }
        }

    public:
        [[nodiscard]] bigint() = default;

        template<std::integral T>
        [[nodiscard]] bigint(T const data) {
            init(data);
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] bigint(bigint<other_bits, other_is_signed> other) {
            init(other);
        }

        [[nodiscard]] bigint(std::string_view const str) {
            init(str);
        }

        template<std::size_t N>
        [[nodiscard]] bigint(char const (&data)[N]) {
            init(data);
        }

        [[nodiscard]] bigint(std::string const &str) {
            init(str);
        }

        template<std::integral T>
        bigint &operator=(T const rhs) {
            init(rhs);
            return *this;
        }

        template<std::size_t other_bits, bool other_is_signed>
        bigint &operator=(bigint<other_bits, other_is_signed> rhs) {
            init(rhs);
            return *this;
        }

        bigint &operator=(std::string_view const str) {
            init(str);
            return *this;
        }

        template<std::size_t N>
        bigint &operator=(char const (&rhs)[N]) {
            init(rhs);
            return *this;
        }

        bigint &operator=(std::string const &rhs) {
            init(rhs);
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] std::strong_ordering operator<=>(T const other) const {
            static_assert(bits / CHAR_BIT >= sizeof(T),
                          "Can't compare values with a larger bit count than the target type.");

            std::uint8_t fill = 0;
            if constexpr (std::is_signed_v<T>) {
                fill = (other < 0 ? 0xFF : 0);
            }

            std::array<std::uint8_t, bits / CHAR_BIT> extended{};
            if constexpr (std::endian::native == std::endian::little) {
                std::copy_n(reinterpret_cast<const std::uint8_t *>(&other), sizeof(T), extended.begin());
                std::fill_n(extended.begin() + sizeof(T), extended.size() - sizeof(T), fill);
                for (std::size_t i = extended.size(); i > 0; --i) {
                    if constexpr (std::is_signed_v<T>) {
                        if (static_cast<int8_t>(data_[i - 1]) < static_cast<int8_t>(extended[i - 1])) {
                            return std::strong_ordering::less;
                        }
                        if (static_cast<int8_t>(data_[i - 1]) > static_cast<int8_t>(extended[i - 1])) {
                            return std::strong_ordering::greater;
                        }
                    } else {
                        if (data_[i - 1] < extended[i - 1]) {
                            return std::strong_ordering::less;
                        }
                        if (data_[i - 1] > extended[i - 1]) {
                            return std::strong_ordering::greater;
                        }
                    }
                }
            } else {
                std::copy_n(reinterpret_cast<const std::uint8_t *>(&other), sizeof(T), extended.end() - sizeof(T));
                std::fill_n(extended.begin(), extended.size() - sizeof(T), fill);
                for (std::size_t i = 0; i < extended.size(); ++i) {
                    if constexpr (std::is_signed_v<T>) {
                        if (static_cast<int8_t>(data_[i]) < static_cast<int8_t>(extended[i])) {
                            return std::strong_ordering::less;
                        }
                        if (static_cast<int8_t>(data_[i]) > static_cast<int8_t>(extended[i])) {
                            return std::strong_ordering::greater;
                        }
                    } else {
                        if (data_[i] < extended[i]) {
                            return std::strong_ordering::less;
                        }
                        if (data_[i] > extended[i]) {
                            return std::strong_ordering::greater;
                        }
                    }
                }
            }

            return std::strong_ordering::equal;
        }

        template<std::integral T>
        [[nodiscard]] bool operator==(T const other) const {
            static_assert(bits / CHAR_BIT >= sizeof(T),
                          "Can't compare values with a larger bit count than the target type.");
            if (std::is_signed_v<T> and !is_signed and other < 0) {
                return false;
            }

            std::array<std::uint8_t, bits / CHAR_BIT> extended{};
            std::uint8_t fill = (std::is_signed_v<T> and other < 0) ? 0xFF : 0;

            if constexpr (std::endian::native == std::endian::little) {
                std::copy_n(reinterpret_cast<const std::uint8_t *>(&other), sizeof(T), extended.begin());
                std::fill_n(extended.begin() + sizeof(T), extended.size() - sizeof(T), fill);
            } else {
                std::fill_n(extended.begin(), extended.size() - sizeof(T), fill);
                std::copy_n(reinterpret_cast<const std::uint8_t *>(&other), sizeof(T),
                            extended.begin() + (extended.size() - sizeof(T)));
            }

            return extended == data_;
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] std::strong_ordering operator<=>(bigint<other_bits, other_is_signed> const &other) const {
            constexpr std::size_t lhs_size = bits / CHAR_BIT;
            constexpr std::size_t rhs_size = other_bits / CHAR_BIT;
            constexpr std::size_t max_size = (lhs_size > rhs_size ? lhs_size : rhs_size);

            std::array<std::uint8_t, max_size> lhs_extended{};
            std::array<std::uint8_t, max_size> rhs_extended{};

            std::uint8_t lhs_fill = 0;
            if constexpr (is_signed) {
                if constexpr (std::endian::native == std::endian::little) {
                    if (data_[lhs_size - 1] & 0x80) {
                        lhs_fill = 0xFF;
                    }
                } else {
                    if (data_[0] & 0x80) {
                        lhs_fill = 0xFF;
                    }
                }
            }

            std::uint8_t rhs_fill = 0;
            if constexpr (other_is_signed) {
                if constexpr (std::endian::native == std::endian::little) {
                    if (other.data_[rhs_size - 1] & 0x80) {
                        rhs_fill = 0xFF;
                    }
                } else {
                    if (other.data_[0] & 0x80) {
                        rhs_fill = 0xFF;
                    }
                }
            }

            if constexpr (std::endian::native == std::endian::little) {
                std::copy(data_.begin(), data_.end(), lhs_extended.begin());
                std::fill(lhs_extended.begin() + lhs_size, lhs_extended.end(), lhs_fill);
                std::copy(other.data_.begin(), other.data_.end(), rhs_extended.begin());
                std::fill(rhs_extended.begin() + rhs_size, rhs_extended.end(), rhs_fill);
            } else {
                std::fill(lhs_extended.begin(), lhs_extended.begin() + (max_size - lhs_size), lhs_fill);
                std::copy(data_.begin(), data_.end(), lhs_extended.begin() + (max_size - lhs_size));
                std::fill(rhs_extended.begin(), rhs_extended.begin() + (max_size - rhs_size), rhs_fill);
                std::copy(other.data_.begin(), other.data_.end(), rhs_extended.begin() + (max_size - rhs_size));
            }

            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = max_size; i > 0; --i) {
                    if constexpr (!is_signed and !other_is_signed) {
                        auto const a = lhs_extended[i - 1];
                        auto const b = rhs_extended[i - 1];
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    } else if constexpr (!is_signed and other_is_signed) {
                        auto const a = lhs_extended[i - 1];
                        auto const b = static_cast<int8_t>(rhs_extended[i - 1]);
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    } else if constexpr (is_signed and !other_is_signed) {
                        auto const a = static_cast<int8_t>(lhs_extended[i - 1]);
                        auto const b = rhs_extended[i - 1];
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    } else {
                        auto const a = static_cast<int8_t>(lhs_extended[i - 1]);
                        auto const b = static_cast<int8_t>(rhs_extended[i - 1]);
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    }
                }
            } else {
                for (std::size_t i = 0; i < max_size; ++i) {
                    if constexpr (!is_signed and !other_is_signed) {
                        auto const a = lhs_extended[i];
                        auto const b = rhs_extended[i];
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    } else if constexpr (!is_signed and other_is_signed) {
                        auto const a = lhs_extended[i];
                        auto const b = static_cast<int8_t>(rhs_extended[i]);
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    } else if constexpr (is_signed and !other_is_signed) {
                        auto const a = static_cast<int8_t>(lhs_extended[i]);
                        auto const b = rhs_extended[i];
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    } else {
                        auto const a = static_cast<int8_t>(lhs_extended[i]);
                        auto const b = static_cast<int8_t>(rhs_extended[i]);
                        if (a < b) {
                            return std::strong_ordering::less;
                        }
                        if (a > b) {
                            return std::strong_ordering::greater;
                        }
                    }
                }
            }

            return std::strong_ordering::equal;
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] bool operator==(bigint<other_bits, other_is_signed> const &other) const {
            if constexpr (bits < other_bits) {
                return other == *this;
            } else {
                static_assert(bits >= other_bits, "Can't compare values with a larger bit count than the target type.");
                if (other_is_signed and !is_signed and other < static_cast<std::int8_t>(0)) {
                    return false;
                }

                std::uint8_t fill = (other_is_signed and other < static_cast<std::int8_t>(0)) ? 0xFF : 0;

                std::array<std::uint8_t, bits / CHAR_BIT> extended{};
                if constexpr (std::endian::native == std::endian::little) {
                    std::copy_n(other.data_.begin(), other.data_.size(), extended.begin());
                    std::fill_n(extended.begin() + other.data_.size(), extended.size() - other.data_.size(), fill);
                } else {
                    std::fill_n(extended.begin(), extended.size() - other.data_.size(), fill);
                    std::copy_n(other.data_.begin(), other.data_.size(),
                                extended.begin() + (extended.size() - other.data_.size()));
                }
                return extended == data_;
            }
        }

        template<std::integral T>
        bigint &operator+=(T const other) {
            *this += bigint(other);
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] bigint operator+(T const other) const {
            bigint result(*this);
            result += other;
            return result;
        }

        template<std::size_t other_bits, bool other_is_signed>
        bigint &operator+=(bigint<other_bits, other_is_signed> const &other) {
            constexpr std::size_t this_size = bits / CHAR_BIT;
            constexpr std::size_t other_size = other_bits / CHAR_BIT;
            std::uint16_t carry = 0;

            std::uint8_t fill = 0;
            if constexpr (other_is_signed) {
                if constexpr (std::endian::native == std::endian::little) {
                    if (other.data_[other_size - 1] & 0x80) {
                        fill = 0xFF;
                    }
                } else {
                    if (other.data_[0] & 0x80) {
                        fill = 0xFF;
                    }
                }
            }

            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < this_size; ++i) {
                    std::uint16_t const other_byte = (i < other_size) ? other.data_[i] : fill;
                    std::uint16_t const sum = static_cast<std::uint16_t>(data_[i]) + other_byte + carry;
                    data_[i] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                }
            } else {
                for (std::size_t i = 0; i < this_size; ++i) {
                    std::size_t idx = this_size - 1 - i;
                    std::uint16_t const other_byte = (i < other_size) ? other.data_[other_size - 1 - i] : fill;
                    std::uint16_t const sum = static_cast<std::uint16_t>(data_[idx]) + other_byte + carry;
                    data_[idx] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                }
            }
            return *this;
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] bigint operator+(bigint<other_bits, other_is_signed> const &other) const {
            bigint result(*this);
            result += other;
            return result;
        }

        template<std::integral T>
        bigint &operator*=(T const other) {
            *this *= bigint(other);
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] bigint operator*(T const other) const {
            bigint result(*this);
            result *= other;
            return result;
        }

        template<std::size_t other_bits, bool other_is_signed>
        bigint &operator*=(bigint<other_bits, other_is_signed> const &other) {
            bool negative_result = false;
            bigint abs_this(*this);
            bigint abs_other(other);

            if constexpr (is_signed) {
                if (*this < static_cast<int8_t>(0)) {
                    negative_result = !negative_result;
                    abs_this = -abs_this;
                }
            }
            if constexpr (other_is_signed) {
                if (other < static_cast<int8_t>(0)) {
                    negative_result = !negative_result;
                    abs_other = -abs_other;
                }
            }

            constexpr std::size_t n = bits / CHAR_BIT;
            constexpr std::size_t m = other_bits / CHAR_BIT;
            bigint result(static_cast<std::int8_t>(0));

            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < n; ++i) {
                    std::uint16_t carry = 0;
                    for (std::size_t j = 0; j < m; ++j) {
                        if (i + j >= n) {
                            break;
                        }
                        std::uint32_t product = static_cast<std::uint32_t>(abs_this.data_[i]) *
                                                static_cast<std::uint32_t>(abs_other.data_[j]);
                        product += static_cast<std::uint32_t>(result.data_[i + j]) + carry;
                        result.data_[i + j] = static_cast<std::uint8_t>(product & 0xFF);
                        carry = product >> 8;
                    }
                    if (i + m < n) {
                        std::uint32_t const sum = static_cast<std::uint32_t>(result.data_[i + m]) + carry;
                        result.data_[i + m] = static_cast<std::uint8_t>(sum & 0xFF);
                    }
                }
            } else {
                for (std::size_t i = 0; i < n; ++i) {
                    std::size_t const idx1 = n - 1 - i;
                    std::uint16_t carry = 0;
                    for (std::size_t j = 0; j < m; ++j) {
                        if (i + j >= n) {
                            break;
                        }
                        std::size_t const idx2 = m - 1 - j;
                        std::size_t const result_idx = n - 1 - (i + j);
                        std::uint32_t product = static_cast<std::uint32_t>(abs_this.data_[idx1]) *
                                                static_cast<std::uint32_t>(abs_other.data_[idx2]);
                        product += static_cast<std::uint32_t>(result.data_[result_idx]) + carry;
                        result.data_[result_idx] = static_cast<std::uint8_t>(product & 0xFF);
                        carry = product >> 8;
                    }
                    if (i + m < n) {
                        std::size_t result_idx = n - 1 - (i + m);
                        std::uint32_t const sum = static_cast<std::uint32_t>(result.data_[result_idx]) + carry;
                        result.data_[result_idx] = static_cast<std::uint8_t>(sum & 0xFF);
                    }
                }
            }
            if constexpr (is_signed) {
                if (negative_result) {
                    result = -result;
                }
            }
            *this = result;
            return *this;
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] bigint operator*(bigint<other_bits, other_is_signed> const &other) const {
            bigint result(*this);
            result *= other;
            return result;
        }

        template<std::integral T>
        bigint &operator-=(T const other) {
            *this -= bigint(other);
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] bigint operator-(T const other) const {
            bigint result(*this);
            result -= other;
            return result;
        }

        template<std::size_t other_bits, bool other_is_signed>
        bigint &operator-=(bigint<other_bits, other_is_signed> const &other) {
            constexpr std::size_t this_size = bits / CHAR_BIT;
            constexpr std::size_t other_size = other_bits / CHAR_BIT;

            std::uint8_t fill = 0;
            if constexpr (other_is_signed and other_size <= this_size) {
                if constexpr (std::endian::native == std::endian::little) {
                    if (other.data_[other_size - 1] & 0x80) {
                        fill = 0xFF;
                    }
                } else {
                    if (other.data_[0] & 0x80) {
                        fill = 0xFF;
                    }
                }
            }

            std::uint16_t borrow = 0;
            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < this_size; ++i) {
                    std::uint16_t const other_byte = (i < other_size) ? other.data_[i] : fill;
                    int16_t diff = static_cast<int16_t>(data_[i]) - static_cast<int16_t>(other_byte) - borrow;
                    if (diff < 0) {
                        diff += 256;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    data_[i] = static_cast<std::uint8_t>(diff);
                }
            } else {
                for (std::size_t i = 0; i < this_size; ++i) {
                    std::size_t idx = this_size - 1 - i;
                    std::uint16_t const other_byte = (i < other_size) ? other.data_[other_size - 1 - i] : fill;
                    int16_t diff = static_cast<int16_t>(data_[idx]) - static_cast<int16_t>(other_byte) - borrow;
                    if (diff < 0) {
                        diff += 256;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    data_[idx] = static_cast<std::uint8_t>(diff);
                }
            }
            return *this;
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] bigint operator-(bigint<other_bits, other_is_signed> const &other) const {
            bigint result(*this);
            result -= other;
            return result;
        }

        template<std::integral T>
        bigint &operator/=(T const other) {
            *this /= bigint(other);
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] bigint operator/(T const other) const {
            bigint result(*this);
            result /= other;
            return result;
        }

        template<std::size_t other_bits, bool other_is_signed>
        bigint &operator/=(bigint<other_bits, other_is_signed> const &other) {
            if (other == static_cast<int8_t>(0)) {
                throw std::overflow_error("Division by zero");
            }

            bigint quotient(static_cast<int8_t>(0));
            bigint remainder(static_cast<int8_t>(0));
            constexpr std::size_t total_bits = bits;

            for (std::size_t i = total_bits; i > 0; --i) {
                remainder <<= static_cast<int8_t>(1);
                if (this->get_bit(i - 1)) {
                    remainder += static_cast<int8_t>(1);
                }
                if (remainder >= other) {
                    remainder -= other;
                    quotient.set_bit(i - 1, true);
                }
            }
            *this = quotient;
            return *this;
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] bigint operator/(bigint<other_bits, other_is_signed> const &other) const {
            bigint result(*this);
            result /= other;
            return result;
        }

        template<std::integral T>
        bigint &operator%=(T const other) {
            *this %= bigint(other);
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] bigint operator%(T const other) const {
            bigint result(*this);
            result %= other;
            return result;
        }

        template<std::size_t other_bits, bool other_is_signed>
        bigint &operator%=(bigint<other_bits, other_is_signed> const &other) {
            if (other == static_cast<int8_t>(0)) {
                throw std::overflow_error("Division by zero");
            }

            bigint quotient(static_cast<int8_t>(0));
            bigint remainder(static_cast<int8_t>(0));
            constexpr std::size_t total_bits = bits; // total number of bits in *this

            for (std::size_t i = total_bits; i > 0; --i) {
                remainder <<= static_cast<int8_t>(1);
                if (this->get_bit(i - 1)) {
                    remainder += static_cast<int8_t>(1);
                }

                if (remainder >= other) {
                    remainder -= other;
                }
            }
            *this = remainder;
            return *this;
        }

        template<std::size_t other_bits, bool other_is_signed>
        [[nodiscard]] bigint operator%(bigint<other_bits, other_is_signed> const &other) const {
            bigint result(*this);
            result %= other;
            return result;
        }

        bigint &operator<<=(std::size_t const shift) {
            constexpr std::size_t n = bits / CHAR_BIT;
            if (shift == 0) {
                return *this;
            }

            std::size_t const byte_shift = shift / 8;
            std::size_t const bit_shift = shift % 8;
            std::array<std::uint8_t, n> result{};

            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < n; ++i) {
                    if (i + byte_shift < n) {
                        result[i + byte_shift] = data_[i];
                    }
                }
            } else {
                for (std::size_t i = 0; i < n; ++i) {
                    if (i >= byte_shift) {
                        result[i - byte_shift] = data_[i];
                    }
                }
            }

            std::uint16_t carry = 0;
            for (std::size_t i = 0; i < n; ++i) {
                std::uint16_t const temp = (static_cast<std::uint16_t>(result[i]) << bit_shift) | carry;
                result[i] = static_cast<std::uint8_t>(temp & 0xFF);
                carry = temp >> 8;
            }

            data_ = result;
            return *this;
        }

        [[nodiscard]] bigint operator<<(std::size_t const shift) const {
            bigint result(*this);
            result <<= shift;
            return result;
        }

        bigint &operator>>=(std::size_t const shift) {
            constexpr std::size_t n = bits / CHAR_BIT;
            if (shift == 0) {
                return *this;
            }

            std::uint8_t fill = 0;
            if constexpr (is_signed) {
                if constexpr (std::endian::native == std::endian::little) {
                    if (data_[n - 1] & 0x80) {
                        fill = 0xFF;
                    }
                } else {
                    if (data_[0] & 0x80) {
                        fill = 0xFF;
                    }
                }
            }

            if (shift >= bits) {
                std::fill(data_.begin(), data_.end(), fill);
                return *this;
            }

            std::size_t const byte_shift = shift / 8;
            std::size_t const bit_shift = shift % 8;
            std::array<std::uint8_t, n> result{};

            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = 0; i < n; ++i) {
                    std::uint8_t const lower = (i + byte_shift < n) ? data_[i + byte_shift] : fill;
                    std::uint8_t const upper = (i + byte_shift + 1 < n) ? data_[i + byte_shift + 1] : fill;
                    if (bit_shift == 0) {
                        result[i] = lower;
                    } else {
                        result[i] = static_cast<std::uint8_t>((lower >> bit_shift) | (upper << (8 - bit_shift)));
                    }
                }
            } else {
                for (std::size_t i = 0; i < n; ++i) {
                    int src = static_cast<int>(i) - static_cast<int>(byte_shift);
                    std::uint8_t const lower = (src >= 0 and static_cast<std::size_t>(src) < n) ? data_[src] : fill;
                    int src2 = src - 1;
                    std::uint8_t const higher = (src2 >= 0 and static_cast<std::size_t>(src2) < n) ? data_[src2] : fill;
                    if (bit_shift == 0) {
                        result[i] = lower;
                    } else {
                        result[i] = static_cast<std::uint8_t>((lower >> bit_shift) | (higher << (8 - bit_shift)));
                    }
                }
            }

            data_ = result;
            return *this;
        }

        [[nodiscard]] bigint operator>>(std::size_t const shift) const {
            bigint result(*this);
            result >>= shift;
            return result;
        }

        [[nodiscard]] bigint operator-() const {
            bigint min_value;
            min_value.data_.fill(0);
            if constexpr (std::endian::native == std::endian::little) {
                min_value.data_.back() = 0x80;
            } else {
                min_value.data_.front() = 0x80;
            }

            if (*this == min_value) {
                throw std::overflow_error("Negation overflow: minimum value cannot be negated");
            }

            bigint result = ~*this;
            result += static_cast<int8_t>(1);
            return result;
        }

        bigint &operator++() {
            *this += 1;
            return *this;
        }

        bigint operator++(int) {
            bigint result(*this);
            ++*this;
            return result;
        }

        bigint &operator--() {
            *this -= 1;
            return *this;
        }

        bigint operator--(int) {
            bigint result(*this);
            --*this;
            return result;
        }

        [[nodiscard]] bigint operator&(bigint const &other) const {
            bigint result(*this);
            result &= other;
            return result;
        }

        bigint &operator&=(bigint const &other) {
            for (std::size_t i = 0; i < data_.size(); ++i) {
                data_[i] &= other.data_[i];
            }
            return *this;
        }

        [[nodiscard]] bigint operator|(bigint const &other) const {
            bigint result(*this);
            result |= other;
            return result;
        }

        bigint &operator|=(bigint const &other) {
            for (std::size_t i = 0; i < data_.size(); ++i) {
                data_[i] |= other.data_[i];
            }
            return *this;
        }

        [[nodiscard]] bigint operator^(bigint const &other) const {
            bigint result(*this);
            result ^= other;
            return result;
        }

        bigint &operator^=(bigint const &other) {
            for (std::size_t i = 0; i < data_.size(); ++i) {
                data_[i] ^= other.data_[i];
            }
            return *this;
        }

        [[nodiscard]] bigint operator~() const {
            bigint result(*this);
            for (auto &byte: result.data_) {
                byte = ~byte;
            }
            return result;
        }

        template<std::size_t other_bits, bool other_is_signed>
        friend class bigint;

#ifndef bigint_DISABLE_IO
        template<std::size_t other_bits, bool other_is_signed>
        friend std::ostream &operator<<(std::ostream &, bigint<other_bits, other_is_signed> const &);

        template<std::size_t other_bits, bool other_is_signed>
        friend std::istream &operator>>(std::istream &, bigint<other_bits, other_is_signed> &);
#endif

        template<std::size_t other_bits, bool other_is_signed>
        friend bigint<other_bits, other_is_signed> byteswap(bigint<other_bits, other_is_signed> const &);
    };

#ifndef bigint_DISABLE_IO
    template<std::size_t bits, bool is_signed>
    std::ostream &operator<<(std::ostream &os, bigint<bits, is_signed> const &data) {
        auto const flags = os.flags();
        std::string result;

        std::ios_base::fmtflags const base_flag = flags & std::ios_base::basefield;
        bool const use_uppercase = (flags & std::ios_base::uppercase) != 0;

        if (base_flag == std::ios_base::hex) {
            if constexpr (std::endian::native == std::endian::little) {
                for (std::size_t i = data.data_.size() - 1; i > 0; --i) {
                    if (result.empty() and data.data_[i - 1] == 0 and i != 1) {
                        continue;
                    }
                    std::array<char, 3> buffer{};
                    std::snprintf(buffer.data(), buffer.size(), use_uppercase ? "%02X" : "%02x", data.data_[i - 1]);
                    result.append(buffer.data());
                }
            } else {
                for (std::size_t i = 0; i < data.data_.size(); ++i) {
                    if (result.empty() and data.data_[i] == 0 and i != 1) {
                        continue;
                    }
                    std::array<char, 3> buffer{};
                    std::snprintf(buffer.data(), buffer.size(), use_uppercase ? "%02X" : "%02x", data.data_[i]);
                    result.append(buffer.data());
                }
            }
        } else if (base_flag == std::ios_base::oct) {
            bigint<bits, is_signed> temp(data);
            if (temp == static_cast<int8_t>(0)) {
                result = "0";
            } else {
                while (temp != static_cast<int8_t>(0)) {
                    bigint<bits, is_signed> r = temp % static_cast<int8_t>(8);
                    int digit = 0;
                    if constexpr (std::endian::native == std::endian::little) {
                        digit = r.data_[0];
                    } else {
                        digit = r.data_[r.data_.size() - 1];
                    }
                    result.push_back('0' + digit);
                    temp /= static_cast<int8_t>(8);
                }
                std::ranges::reverse(result);
            }
        } else if (base_flag == std::ios_base::dec) {
            bigint<bits, is_signed> temp(data);
            bool negative = false;
            if constexpr (is_signed) {
                if (temp < static_cast<int8_t>(0)) {
                    negative = true;
                    temp = -temp;
                }
            }
            if (temp == static_cast<int8_t>(0)) {
                result = "0";
            } else {
                while (temp != static_cast<int8_t>(0)) {
                    bigint<bits, is_signed> r = temp % static_cast<int8_t>(10);
                    int digit = 0;
                    if constexpr (std::endian::native == std::endian::little) {
                        digit = r.data_[0];
                    } else {
                        digit = r.data_[r.data_.size() - 1];
                    }
                    result.push_back('0' + digit);
                    temp /= static_cast<int8_t>(10);
                }
                std::ranges::reverse(result);
                if (negative) {
                    result.insert(result.begin(), '-');
                }
            }
        }
        os << result;
        return os;
    }

    template<std::size_t bits, bool is_signed>
    std::istream &operator>>(std::istream &is, bigint<bits, is_signed> &data) {
        std::string token;
        if (!(is >> token)) {
            return is;
        }

        auto const flags = is.flags();
        auto const base_flag = flags & std::ios_base::basefield;
        std::uint8_t base = 10;
        if (base_flag == std::ios_base::hex) {
            base = 16;
        } else if (base_flag == std::ios_base::oct) {
            base = 8;
        }
        try {
            if (base != 10) {
                if (!token.empty() and token[0] == '-') {
                    throw std::runtime_error("Negative sign not allowed for non-decimal input");
                }
                data = bigint<bits, is_signed>();;
                data.init_from_string_base(token.c_str(), token.size(), base);
            } else {
                data = token;
            }
        } catch (std::exception const &) {
            is.setstate(std::ios::failbit);
        }
        return is;
    }
#endif

    template<std::size_t bits, bool is_signed>
    bigint<bits, is_signed> byteswap(bigint<bits, is_signed> const &data) {
        bigint<bits, is_signed> result{data};
        std::ranges::reverse(result.data_);
        return result;
    }

    template<std::size_t bits, bool is_signed>
    bigint<bits, is_signed> abs(bigint<bits, is_signed> const &data) {
        if constexpr (not is_signed) {
            return data;
        } else {
            bigint<bits, is_signed> result{data};
            if (result < static_cast<int8_t>(0)) {
                result = -result;
            }
            return result;
        }
    }
}
