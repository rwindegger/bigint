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
#include <memory>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace bigint {

    enum class BitWidth : std::size_t {};

    enum class Signedness : std::uint8_t {
        Signed,
        Unsigned
    };

    template<BitWidth bits, Signedness signedness>
    class bigint final {
    private:
        static_assert(std::to_underlying(bits) % CHAR_BIT == 0, "bits must be a multiple of CHAR_BIT");
        std::array<std::uint8_t, std::to_underlying(bits) / CHAR_BIT> data_{};

    public:
        [[nodiscard]] constexpr bigint() = default;

        template<std::integral T>
        [[nodiscard]] constexpr bigint(T const data) {
            static_assert(std::to_underlying(bits) / CHAR_BIT >= sizeof(T),
                          "Can't assign values with a larger bit count than the target type.");

            auto fill = std::uint8_t{0};
            if constexpr (std::is_signed_v<T>) {
                fill = (data < 0 ? 0xFF : 0);
            }
            data_.fill(fill);

            if constexpr (std::endian::native == std::endian::little) {
                std::copy_n(reinterpret_cast<std::uint8_t const * const>(std::addressof(data)), sizeof(T),
                            data_.begin());
            } else {
                std::copy_n(reinterpret_cast<std::uint8_t const * const>(std::addressof(data)), sizeof(T),
                            data_.end() - sizeof(T));
            }
        }

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr bigint(bigint<other_bits, other_signedness> const &other) {
            static_assert(bits >= other_bits, "Can't assign values with a larger bit count than the target type.");

            auto fill = std::uint8_t{0};
            if constexpr (other_signedness == Signedness::Signed) {
                fill = other < std::int8_t{0} ? 0xFF : 0;
            }
            data_.fill(fill);

            if constexpr (std::endian::native == std::endian::little) {
                for (auto const i: std::views::iota(0uz, other.data_.size())) {
                    data_[i] = other.data_[i];
                }
            } else {
                for (auto const i: std::views::reverse(std::views::iota(1uz, other.data_.size() + 1))) {
                    data_[data_.size() - i - 1] = other.data_[other.data_.size() - i - 1];
                }
            }
        }

        [[nodiscard]] constexpr bigint(std::string_view const str) {
            if (str.length() > 2 and str[0] == '0') {
                switch (str[1]) {
                    case 'x':
                        init_from_string_base(str.substr(2), 16);
                        break;
                    case 'b':
                        init_from_string_base(str.substr(2), 2);
                        break;
                    default:
                        init_from_string_base(str.substr(1), 8);
                        break;
                }
            } else {
                if (str[0] == '-') {
                    if constexpr (signedness == Signedness::Unsigned) {
                        throw std::runtime_error("Cannot initialize an unsigned bigint23 with a negative value.");
                    } else {
                        init_from_string_base(str.substr(1), 10);
                        *this = -*this;
                    }
                } else {
                    init_from_string_base(str, 10);
                }
            }
        }

        template<std::size_t N>
        [[nodiscard]] constexpr bigint(char const (&data)[N]) : bigint{std::string_view{data, N - 1}} {
        }

        [[nodiscard]] constexpr bigint(std::string const &str) : bigint{std::string_view{str}} {
        }

        template<std::integral T>
        bigint constexpr &operator=(T const rhs) {
            this->~bigint();
            new(this) bigint(rhs);
            return *this;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        bigint constexpr &operator=(bigint<other_bits, other_signedness> const &rhs) {
            if (this != std::addressof(rhs)) {
                this->~bigint();
                new(this) bigint(rhs);
            }
            return *this;
        }

        bigint constexpr &operator=(std::string_view const str) {
            this->~bigint();
            new(this) bigint(str);
            return *this;
        }

        template<std::size_t N>
        bigint constexpr &operator=(char const (&rhs)[N]) {
            this->~bigint();
            new(this) bigint(rhs);
            return *this;
        }

        bigint constexpr &operator=(std::string const &rhs) {
            this->~bigint();
            new(this) bigint(rhs);
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] constexpr std::strong_ordering operator<=>(T const other) const {
            static_assert(std::to_underlying(bits) / CHAR_BIT >= sizeof(T),
                          "Can't compare values with a larger bit count than the target type.");

            auto fill = std::uint8_t{0};
            if constexpr (std::is_signed_v<T>) {
                fill = (other < 0 ? 0xFF : 0);
            }

            std::array<std::uint8_t, std::to_underlying(bits) / CHAR_BIT> extended{};
            extended.fill(fill);

            if constexpr (std::endian::native == std::endian::little) {
                std::copy_n(reinterpret_cast<std::uint8_t const * const>(&other), sizeof(T), extended.begin());
                for (auto const i: std::views::reverse(std::views::iota(1uz, extended.size() + 1))) {
                    if constexpr (std::is_signed_v<T>) {
                        if (static_cast<std::int8_t>(data_[i - 1]) < static_cast<std::int8_t>(extended[i - 1])) {
                            return std::strong_ordering::less;
                        }
                        if (static_cast<std::int8_t>(data_[i - 1]) > static_cast<std::int8_t>(extended[i - 1])) {
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
                std::copy_n(reinterpret_cast<std::uint8_t const * const>(&other), sizeof(T),
                            extended.end() - sizeof(T));
                for (auto const i: std::views::iota(0uz, extended.size())) {
                    if constexpr (std::is_signed_v<T>) {
                        if (static_cast<std::int8_t>(data_[i]) < static_cast<std::int8_t>(extended[i])) {
                            return std::strong_ordering::less;
                        }
                        if (static_cast<std::int8_t>(data_[i]) > static_cast<std::int8_t>(extended[i])) {
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
        [[nodiscard]] constexpr bool operator==(T const other) const {
            static_assert(std::to_underlying(bits) / CHAR_BIT >= sizeof(T),
                          "Can't compare values with a larger bit count than the target type.");
            if constexpr (std::is_signed_v<T> and signedness == Signedness::Unsigned) {
                if (other < 0) {
                    return false;
                }
            }

            auto fill = std::uint8_t{0};
            if constexpr (std::is_signed_v<T>) {
                fill = (other < 0 ? 0xFF : 0);
            }

            std::array<std::uint8_t, std::to_underlying(bits) / CHAR_BIT> extended{};
            extended.fill(fill);

            if constexpr (std::endian::native == std::endian::little) {
                std::copy_n(reinterpret_cast<std::uint8_t const * const>(&other), sizeof(T), extended.begin());
            } else {
                std::copy_n(reinterpret_cast<std::uint8_t const * const>(&other), sizeof(T),
                            extended.begin() + (extended.size() - sizeof(T)));
            }

            return extended == data_;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr std::strong_ordering operator
        <=>(bigint<other_bits, other_signedness> const &other) const {
            static constexpr std::size_t lhs_size = std::to_underlying(bits) / CHAR_BIT;
            static constexpr std::size_t rhs_size = std::to_underlying(other_bits) / CHAR_BIT;
            static constexpr std::size_t max_size = (lhs_size > rhs_size ? lhs_size : rhs_size);

            std::array<std::uint8_t, max_size> lhs_extended{};
            std::array<std::uint8_t, max_size> rhs_extended{};

            auto lhs_fill = std::uint8_t{0};
            if constexpr (signedness == Signedness::Signed) {
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
            lhs_extended.fill(lhs_fill);

            auto rhs_fill = std::uint8_t{0};
            if constexpr (other_signedness == Signedness::Signed) {
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
            rhs_extended.fill(rhs_fill);

            if constexpr (std::endian::native == std::endian::little) {
                std::copy(data_.begin(), data_.end(), lhs_extended.begin());
                std::copy(other.data_.begin(), other.data_.end(), rhs_extended.begin());
            } else {
                std::copy(data_.begin(), data_.end(), lhs_extended.begin() + (max_size - lhs_size));
                std::copy(other.data_.begin(), other.data_.end(), rhs_extended.begin() + (max_size - rhs_size));
            }

            if constexpr (std::endian::native == std::endian::little) {
                for (auto const i: std::views::reverse(std::views::iota(0uz, max_size))) {
                    std::strong_ordering value{std::strong_ordering::equal};
                    if (spaceship_check_index<max_size, other_signedness>(lhs_extended, rhs_extended, i, value)) {
                        return value;
                    }
                }
            } else {
                for (auto const i: std::views::iota(0u, max_size)) {
                    std::strong_ordering value{std::strong_ordering::equal};
                    if (spaceship_check_index<max_size, other_signedness>(lhs_extended, rhs_extended, i, value)) {
                        return value;
                    }
                }
            }

            return std::strong_ordering::equal;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr bool operator==(bigint<other_bits, other_signedness> const &other) const {
            if constexpr (bits < other_bits) {
                return other == *this;
            } else {
                static_assert(bits >= other_bits, "Can't compare values with a larger bit count than the target type.");
                if constexpr (other_signedness == Signedness::Signed and signedness == Signedness::Unsigned) {
                    if (other < std::int8_t{0}) {
                        return false;
                    }
                }

                auto fill = std::uint8_t{0};
                if constexpr (other_signedness == Signedness::Signed) {
                    fill = other < std::int8_t{0} ? 0xFF : 0;
                }

                std::array<std::uint8_t, std::to_underlying(bits) / CHAR_BIT> extended{};
                extended.fill(fill);

                if constexpr (std::endian::native == std::endian::little) {
                    std::copy_n(other.data_.begin(), other.data_.size(), extended.begin());
                } else {
                    std::copy_n(other.data_.begin(), other.data_.size(),
                                extended.begin() + (extended.size() - other.data_.size()));
                }
                return extended == data_;
            }
        }

        template<std::integral T>
        constexpr bigint &operator+=(T const other) {
            *this += bigint{other};
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] constexpr bigint operator+(T const other) const {
            auto result = bigint{*this};
            result += other;
            return result;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        constexpr bigint &operator+=(bigint<other_bits, other_signedness> const &other) {
            static constexpr std::size_t this_size = std::to_underlying(bits) / CHAR_BIT;
            static constexpr std::size_t other_size = std::to_underlying(other_bits) / CHAR_BIT;
            auto carry = std::uint16_t{0};

            auto fill = std::uint8_t{0};
            if constexpr (other_signedness == Signedness::Signed) {
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

            for (auto const i: std::views::iota(0uz, this_size)) {
                if constexpr (std::endian::native == std::endian::little) {
                    auto const other_byte = std::uint16_t{i < other_size ? other.data_[i] : fill};
                    auto const sum = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data_[i]) + other_byte + carry);
                    data_[i] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                } else {
                    auto const idx = std::size_t{this_size - 1 - i};
                    auto const other_byte = std::uint16_t{i < other_size ? other.data_[other_size - 1 - i] : fill};
                    auto const sum = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data_[idx]) + other_byte + carry);
                    data_[idx] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                }
            }
            return *this;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr bigint operator+(bigint<other_bits, other_signedness> const &other) const {
            auto result = bigint{*this};
            result += other;
            return result;
        }

        template<std::integral T>
        constexpr bigint &operator*=(T const other) {
            *this *= bigint{other};
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] constexpr bigint operator*(T const other) const {
            auto result = bigint{*this};
            result *= other;
            return result;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        constexpr bigint &operator*=(bigint<other_bits, other_signedness> const &other) {
            auto negative_result = false;
            auto abs_this = bigint{*this};
            auto abs_other = bigint{other};

            if constexpr (signedness == Signedness::Signed) {
                if (*this < std::int8_t{0}) {
                    negative_result = !negative_result;
                    abs_this = -abs_this;
                }
            }
            if constexpr (other_signedness == Signedness::Signed) {
                if (other < std::int8_t{0}) {
                    negative_result = !negative_result;
                    abs_other = -abs_other;
                }
            }

            static constexpr auto n = std::size_t{std::to_underlying(bits) / CHAR_BIT};
            static constexpr auto m = std::size_t{std::to_underlying(other_bits) / CHAR_BIT};
            auto result = bigint{};

            for (auto const i: std::views::iota(0uz, n)) {
                if constexpr (std::endian::native == std::endian::little) {
                    auto carry = std::uint16_t{0};
                    for (auto const j: std::views::iota(0uz, m)) {
                        if (i + j >= n) {
                            break;
                        }
                        auto product = std::uint32_t{
                            static_cast<std::uint32_t>(abs_this.data_[i]) *
                            static_cast<std::uint32_t>(abs_other.data_[j])
                        };
                        product += static_cast<std::uint32_t>(result.data_[i + j]) + carry;
                        result.data_[i + j] = static_cast<std::uint8_t>(product & 0xFF);
                        carry = product >> 8;
                    }
                    if (i + m < n) {
                        auto const sum = std::uint32_t{static_cast<std::uint32_t>(result.data_[i + m]) + carry};
                        result.data_[i + m] = static_cast<std::uint8_t>(sum & 0xFF);
                    }
                } else {
                    auto const idx1 = std::size_t{n - 1 - i};
                    auto carry = std::uint16_t{0};
                    for (auto const j: std::views::iota(0uz, m)) {
                        if (i + j >= n) {
                            break;
                        }
                        auto const idx2 = std::size_t{m - 1 - j};
                        auto const result_idx = std::size_t{n - 1 - (i + j)};
                        auto product = std::uint32_t{
                            static_cast<std::uint32_t>(abs_this.data_[idx1]) *
                            static_cast<std::uint32_t>(abs_other.data_[idx2])
                        };
                        product += static_cast<std::uint32_t>(result.data_[result_idx]) + carry;
                        result.data_[result_idx] = static_cast<std::uint8_t>(product & 0xFF);
                        carry = product >> 8;
                    }
                    if (i + m < n) {
                        auto const result_idx = std::size_t{n - 1 - (i + m)};
                        auto const sum = std::uint32_t{static_cast<std::uint32_t>(result.data_[result_idx]) + carry};
                        result.data_[result_idx] = static_cast<std::uint8_t>(sum & 0xFF);
                    }
                }
            }
            if constexpr (signedness == Signedness::Signed) {
                if (negative_result) {
                    result = -result;
                }
            }
            *this = result;
            return *this;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr bigint operator*(bigint<other_bits, other_signedness> const &other) const {
            auto result = bigint{*this};
            result *= other;
            return result;
        }

        template<std::integral T>
        constexpr bigint &operator-=(T const other) {
            *this -= bigint{other};
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] constexpr bigint operator-(T const other) const {
            auto result = bigint{*this};
            result -= other;
            return result;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        constexpr bigint &operator-=(bigint<other_bits, other_signedness> const &other) {
            static constexpr auto this_size = std::size_t{std::to_underlying(bits) / CHAR_BIT};
            static constexpr auto other_size = std::size_t{std::to_underlying(other_bits) / CHAR_BIT};

            auto fill = std::uint8_t{0};
            if constexpr (other_signedness == Signedness::Signed and other_size <= this_size) {
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
            for (auto const i: std::views::iota(0uz, this_size)) {
                if constexpr (std::endian::native == std::endian::little) {
                    auto const other_byte = std::uint16_t{i < other_size ? other.data_[i] : fill};
                    auto diff = static_cast<std::int16_t>(
                        static_cast<std::int16_t>(data_[i]) - static_cast<std::int16_t>(other_byte) - borrow
                    );
                    if (diff < 0) {
                        diff += 256;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    data_[i] = static_cast<std::uint8_t>(diff);
                } else {
                    auto idx = std::size_t{this_size - 1 - i};
                    auto const other_byte = std::uint16_t{i < other_size ? other.data_[other_size - 1 - i] : fill};
                    auto diff = static_cast<std::int16_t>(
                        static_cast<std::int16_t>(data_[idx]) - static_cast<std::int16_t>(other_byte) - borrow
                    );
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

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr bigint operator-(bigint<other_bits, other_signedness> const &other) const {
            auto result = bigint{*this};
            result -= other;
            return result;
        }

        template<std::integral T>
        constexpr bigint &operator/=(T const other) {
            *this /= bigint{other};
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] constexpr bigint operator/(T const other) const {
            auto result = bigint{*this};
            result /= other;
            return result;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        constexpr bigint &operator/=(bigint<other_bits, other_signedness> const &other) {
            if (other == std::int8_t{0}) {
                throw std::overflow_error("Division by zero");
            }

            auto quotient = bigint{};
            auto remainder = bigint{};
            static constexpr auto total_bits = std::to_underlying(bits);

            for (auto const i: std::views::reverse(std::views::iota(0uz, total_bits))) {
                remainder <<= std::int8_t{1};
                if (this->get_bit(i)) {
                    remainder += std::int8_t{1};
                }
                if (remainder >= other) {
                    remainder -= other;
                    quotient.set_bit(i, true);
                }
            }
            *this = quotient;
            return *this;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr bigint operator/(bigint<other_bits, other_signedness> const &other) const {
            auto result = bigint{*this};
            result /= other;
            return result;
        }

        template<std::integral T>
        constexpr bigint &operator%=(T const other) {
            *this %= bigint{other};
            return *this;
        }

        template<std::integral T>
        [[nodiscard]] constexpr bigint operator%(T const other) const {
            auto result = bigint{*this};
            result %= other;
            return result;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        constexpr bigint &operator%=(bigint<other_bits, other_signedness> const &other) {
            if (other == std::int8_t{0}) {
                throw std::overflow_error("Division by zero");
            }

            auto quotient = bigint{};
            auto remainder = bigint{};
            static constexpr auto total_bits = std::to_underlying(bits);

            for (auto const i: std::views::reverse(std::views::iota(0uz, total_bits))) {
                remainder <<= std::int8_t{1};
                if (this->get_bit(i)) {
                    remainder += std::int8_t{1};
                }

                if (remainder >= other) {
                    remainder -= other;
                }
            }
            *this = remainder;
            return *this;
        }

        template<BitWidth other_bits, Signedness other_signedness>
        [[nodiscard]] constexpr bigint operator%(bigint<other_bits, other_signedness> const &other) const {
            auto result = bigint{*this};
            result %= other;
            return result;
        }

        constexpr bigint &operator<<=(std::size_t const shift) {
            static constexpr auto n = std::size_t{std::to_underlying(bits) / CHAR_BIT};
            if (shift == 0) {
                return *this;
            }

            auto const byte_shift = std::size_t{shift / CHAR_BIT};
            auto const bit_shift = std::size_t{shift % CHAR_BIT};
            auto result = std::array<std::uint8_t, n>{};

            for (auto const i: std::views::iota(0uz, n)) {
                if constexpr (std::endian::native == std::endian::little) {
                    if (i + byte_shift < n) {
                        result[i + byte_shift] = data_[i];
                    }
                } else {
                    if (i >= byte_shift) {
                        result[i - byte_shift] = data_[i];
                    }
                }
            }

            auto carry = std::uint16_t{0};
            for (auto const i: std::views::iota(0uz, n)) {
                auto const temp = static_cast<std::uint16_t>(
                    (static_cast<std::uint16_t>(result[i]) << bit_shift) | carry);
                result[i] = static_cast<std::uint8_t>(temp & 0xFF);
                carry = temp >> 8;
            }

            data_ = result;
            return *this;
        }

        [[nodiscard]] constexpr bigint operator<<(std::size_t const shift) const {
            auto result = bigint{*this};
            result <<= shift;
            return result;
        }

        constexpr bigint &operator>>=(std::size_t const shift) {
            static constexpr auto n = std::size_t{std::to_underlying(bits) / CHAR_BIT};
            if (shift == 0) {
                return *this;
            }

            auto fill = std::uint8_t{0};
            if constexpr (signedness == Signedness::Signed) {
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

            if (shift >= std::to_underlying(bits)) {
                data_.fill(fill);
                return *this;
            }

            auto const byte_shift = std::size_t{shift / CHAR_BIT};
            auto const bit_shift = std::size_t{shift % CHAR_BIT};
            auto result = std::array<std::uint8_t, n>{};

            for (auto const i: std::views::iota(0uz, n)) {
                if constexpr (std::endian::native == std::endian::little) {
                    auto const lower = (i + byte_shift < n) ? data_[i + byte_shift] : fill;
                    auto const upper = (i + byte_shift + 1 < n) ? data_[i + byte_shift + 1] : fill;
                    if (bit_shift == 0) {
                        result[i] = lower;
                    } else {
                        result[i] = static_cast<std::uint8_t>((lower >> bit_shift) | (upper << (8 - bit_shift)));
                    }
                } else {
                    auto const src = static_cast<int>(i) - static_cast<int>(byte_shift);
                    auto const lower = (src >= 0 and static_cast<std::size_t>(src) < n) ? data_[src] : fill;
                    auto const src2 = src - 1;
                    auto const higher = (src2 >= 0 and static_cast<std::size_t>(src2) < n) ? data_[src2] : fill;
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

        [[nodiscard]] constexpr bigint operator>>(std::size_t const shift) const {
            auto result = bigint{*this};
            result >>= shift;
            return result;
        }

        [[nodiscard]] constexpr bigint operator+() const {
            return *this;
        }

        [[nodiscard]] constexpr bigint operator-() const {
            auto min_value = bigint{};
            if constexpr (std::endian::native == std::endian::little) {
                min_value.data_.back() = 0x80;
            } else {
                min_value.data_.front() = 0x80;
            }

            if (*this == min_value) {
                throw std::overflow_error("Negation overflow: minimum value cannot be negated");
            }

            auto result = ~*this;
            result += static_cast<std::int8_t>(1);
            return result;
        }

        constexpr bigint &operator++() {
            *this += 1;
            return *this;
        }

        constexpr bigint operator++(int) {
            auto result = bigint{*this};
            ++*this;
            return result;
        }

        constexpr bigint &operator--() {
            *this -= 1;
            return *this;
        }

        constexpr bigint operator--(int) {
            auto result = bigint{*this};
            --*this;
            return result;
        }

        [[nodiscard]] constexpr bigint operator&(bigint const &other) const {
            auto result = bigint{*this};
            result &= other;
            return result;
        }

        constexpr bigint &operator&=(bigint const &other) {
            for (auto const i: std::views::iota(0uz, data_.size())) {
                data_[i] &= other.data_[i];
            }
            return *this;
        }

        [[nodiscard]] constexpr bigint operator|(bigint const &other) const {
            auto result = bigint{*this};
            result |= other;
            return result;
        }

        constexpr bigint &operator|=(bigint const &other) {
            for (auto const i: std::views::iota(0uz, data_.size())) {
                data_[i] |= other.data_[i];
            }
            return *this;
        }

        [[nodiscard]] constexpr bigint operator^(bigint const &other) const {
            auto result = bigint{*this};
            result ^= other;
            return result;
        }

        constexpr bigint &operator^=(bigint const &other) {
            for (auto const i: std::views::iota(0uz, data_.size())) {
                data_[i] ^= other.data_[i];
            }
            return *this;
        }

        [[nodiscard]] constexpr bigint operator~() const {
            auto result = bigint{*this};
            for (auto &byte: result.data_) {
                byte = ~byte;
            }
            return result;
        }

        template<BitWidth other_bits, Signedness other_is_signed>
        friend class bigint;

#ifndef bigint_DISABLE_IO
        template<BitWidth other_bits, Signedness other_is_signed>
        friend constexpr std::ostream &print_hex(std::ostream &, bigint<other_bits, other_is_signed> const &, bool);

        template<BitWidth other_bits, Signedness other_is_signed>
        friend constexpr std::ostream &print_oct(std::ostream &, bigint<other_bits, other_is_signed> const &);

        template<BitWidth other_bits, Signedness other_is_signed>
        friend constexpr std::ostream &print_dec(std::ostream &, bigint<other_bits, other_is_signed> const &);

        template<BitWidth other_bits, Signedness other_is_signed>
        friend constexpr std::ostream &operator<<(std::ostream &, bigint<other_bits, other_is_signed> const &);

        template<BitWidth other_bits, Signedness other_is_signed>
        friend constexpr std::istream &operator>>(std::istream &, bigint<other_bits, other_is_signed> &);
#endif

        template<BitWidth other_bits, Signedness other_is_signed>
        friend constexpr bigint<other_bits, other_is_signed> byteswap(bigint<other_bits, other_is_signed> const &);

    private:
        [[nodiscard]] constexpr bool get_bit(std::size_t const pos) const {
            auto const byte_index = std::size_t{pos / CHAR_BIT};
            auto const bit_index = std::size_t{pos % CHAR_BIT};
            if constexpr (std::endian::native == std::endian::little) {
                return (data_[byte_index] >> bit_index) & 1;
            } else {
                auto const index = std::size_t{data_.size() - 1 - byte_index};
                return (data_[index] >> bit_index) & 1;
            }
        }

        constexpr void set_bit(std::size_t const pos, bool const value) {
            auto const byte_index = std::size_t{pos / CHAR_BIT};
            auto const bit_index = std::size_t{pos % CHAR_BIT};
            if constexpr (std::endian::native == std::endian::little) {
                if (value) {
                    data_[byte_index] |= static_cast<std::uint8_t>(1U << bit_index);
                } else {
                    data_[byte_index] &= static_cast<std::uint8_t>(~(1U << bit_index));
                }
            } else {
                auto const index = std::size_t{data_.size() - 1 - byte_index};
                if (value) {
                    data_[index] |= static_cast<std::uint8_t>(1U << bit_index);
                } else {
                    data_[index] &= static_cast<std::uint8_t>(~(1U << bit_index));
                }
            }
        }

        constexpr void multiply_by(std::uint32_t const multiplier) {
            auto carry = std::uint32_t{0};
            if constexpr (std::endian::native == std::endian::little) {
                for (auto const i: std::views::iota(0uz, data_.size())) {
                    auto const prod = std::uint32_t{static_cast<std::uint32_t>(data_[i]) * multiplier + carry};
                    data_[i] = static_cast<std::uint8_t>(prod & 0xFF);
                    carry = prod >> 8;
                }
            } else {
                for (auto const i: std::views::reverse(std::views::iota(1uz, data_.size() + 1))) {
                    auto const idx = std::size_t{i - 1};
                    auto const prod = std::uint32_t{static_cast<std::uint32_t>(data_[idx]) * multiplier + carry};
                    data_[idx] = static_cast<std::uint8_t>(prod & 0xFF);
                    carry = prod >> 8;
                }
            }
            if (carry != 0) {
                throw std::overflow_error("Overflow during multiplication");
            }
        }

        constexpr void add_value(std::uint8_t const value) {
            auto carry = std::uint16_t{value};
            if constexpr (std::endian::native == std::endian::little) {
                for (auto i = 0uz; i < data_.size() and carry; ++i) {
                    auto const sum = static_cast<std::uint16_t>(static_cast<std::uint16_t>(data_[i]) + carry);
                    data_[i] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                }
            } else {
                for (auto i = data_.size(); i > 0uz and carry; --i) {
                    auto const idx = std::size_t{i - 1};
                    auto const sum = static_cast<std::uint16_t>(static_cast<std::uint16_t>(data_[idx]) + carry);
                    data_[idx] = static_cast<std::uint8_t>(sum & 0xFF);
                    carry = sum >> 8;
                }
            }
            if (carry != 0) {
                throw std::overflow_error("Overflow during addition");
            }
        }

        constexpr void init_from_string_base(std::string_view const str, std::uint32_t const base) {
            data_.fill(0);
            for (auto const c: str) {
                if (c == '\'' or c == ' ') {
                    continue;
                }
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

        template<std::size_t max_size, Signedness other_signedness>
        constexpr bool spaceship_check_index(std::array<std::uint8_t, max_size> lhs_extended,
                                             std::array<std::uint8_t, max_size> rhs_extended,
                                             std::size_t const i, std::strong_ordering &value) const {
            if constexpr (signedness == Signedness::Unsigned and other_signedness == Signedness::Unsigned) {
                auto const a = lhs_extended[i];
                auto const b = rhs_extended[i];
                if (a < b) {
                    value = std::strong_ordering::less;
                    return true;
                }
                if (a > b) {
                    value = std::strong_ordering::greater;
                    return true;
                }
            } else if constexpr (signedness == Signedness::Unsigned and signedness == Signedness::Signed) {
                auto const a = lhs_extended[i];
                auto const b = static_cast<std::int8_t>(rhs_extended[i]);
                if (a < b) {
                    value = std::strong_ordering::less;
                    return true;
                }
                if (a > b) {
                    value = std::strong_ordering::greater;
                    return true;
                }
            } else if constexpr (signedness == Signedness::Signed and other_signedness == Signedness::Unsigned) {
                auto const a = static_cast<std::int8_t>(lhs_extended[i]);
                auto const b = rhs_extended[i];
                if (a < b) {
                    value = std::strong_ordering::less;
                    return true;
                }
                if (a > b) {
                    value = std::strong_ordering::greater;
                    return true;
                }
            } else {
                auto const a = static_cast<std::int8_t>(lhs_extended[i]);
                auto const b = static_cast<std::int8_t>(rhs_extended[i]);
                if (a < b) {
                    value = std::strong_ordering::less;
                    return true;
                }
                if (a > b) {
                    value = std::strong_ordering::greater;
                    return true;
                }
            }
            return false;
        }
    };

#ifndef bigint_DISABLE_IO
    template<BitWidth bits, Signedness signedness>
    constexpr std::ostream &print_hex(std::ostream &os, bigint<bits, signedness> const &data, bool const use_uppercase) {
        auto const &buf = data.data_;
        auto start = buf.size();
        while (start > 1 and buf[start - 1] == 0) {
            --start;
        }

        if constexpr (std::endian::native == std::endian::little) {
            for (auto const i: std::views::reverse(std::views::iota(0uz, start))) {
                auto local = std::array<char, 3>{};
                std::snprintf(
                    local.data(),
                    local.size(),
                    use_uppercase ? "%02X" : "%02x",
                    buf[i]
                );
                os.write(local.data(), local.size() - 1);
            }
        } else {
            for (auto const i: std::views::iota(0uz, start)) {
                auto local = std::array<char, 3>{};
                std::snprintf(
                    local.data(),
                    local.size(),
                    use_uppercase ? "%02X" : "%02x",
                    buf[i]
                );
                os.write(local.data(), local.size() - 1);
            }
        }

        return os;
    }

    template<BitWidth bits, Signedness signedness>
    constexpr std::ostream &print_oct(std::ostream &os, bigint<bits, signedness> const &data) {
        auto temp = data;

        if (temp == std::int8_t{0}) {
            os.put('0');
            return os;
        }

        constexpr auto max_oct_digits = std::size_t{(std::to_underlying(bits) / 3) + 2};
        auto buffer = std::array<char, max_oct_digits>{};
        auto pos = buffer.end();

        while (temp != std::int8_t{0}) {
            auto r = temp % std::int8_t{8};
            auto digit = r.data_[0];
            *--pos = static_cast<char>('0' + digit);
            temp /= std::int8_t{8};
        }

        auto const length = static_cast<std::size_t>(buffer.end() - pos);
        os.write(std::to_address(pos), length);
        return os;
    }

    template<BitWidth bits, Signedness signedness>
    constexpr std::ostream &print_dec(std::ostream &os, bigint<bits, signedness> const &data) {
        auto temp = data;

        if (temp == std::int8_t{0}) {
            os.put('0');
            return os;
        }

        auto negative = false;
        if constexpr (signedness == Signedness::Signed) {
            if (temp < std::int8_t{0}) {
                negative = true;
                temp = -temp;
            }
        }

        constexpr auto max_dec_digits = std::size_t{static_cast<std::size_t>(std::to_underlying(bits) * 0.3010299957) + 3}; //std::log10(2)
        auto buffer = std::array<char, max_dec_digits>{};
        auto pos = buffer.end();

        while (temp != std::int8_t{0}) {
            auto r = temp % std::int8_t{10};
            auto digit = r.data_[0];
            *--pos = static_cast<char>('0' + digit);
            temp /= std::int8_t{10};
        }

        if (negative) {
            os.put('-');
        }

        auto const length = static_cast<std::size_t>(buffer.end() - pos);
        os.write(std::to_address(pos), length);
        return os;
    }

    template<BitWidth bits, Signedness signedness>
    constexpr std::ostream &operator<<(std::ostream &os, bigint<bits, signedness> const &data) {
        auto const flags = os.flags();
        auto const base_flag = flags & std::ios_base::basefield;
        auto const use_uppercase = (flags & std::ios_base::uppercase) != 0;

        switch (base_flag) {
            case std::ios_base::hex:
                print_hex(os, data, use_uppercase);
                break;
            case std::ios_base::oct:
                print_oct(os, data);
                break;
            case std::ios_base::dec:
            default:
                print_dec(os, data);
                break;
        }

        return os;
    }

    template<BitWidth bits, Signedness signedness>
    constexpr std::istream &operator>>(std::istream &is, bigint<bits, signedness> &data) {
        auto token = std::string{};
        if (!(is >> token)) {
            return is;
        }

        auto const flags = is.flags();
        auto const base_flag = flags & std::ios_base::basefield;
        auto base = std::uint8_t{10};
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
                data = bigint<bits, signedness>();
                data.init_from_string_base(token, base);
            } else {
                data = token;
            }
        } catch (std::exception const &) {
            is.setstate(std::ios::failbit);
        }
        return is;
    }
#endif

    template<BitWidth bits, Signedness signedness>
    constexpr bigint<bits, signedness> byteswap(bigint<bits, signedness> const &data) {
        auto result = bigint<bits, signedness>{data};
        std::ranges::reverse(result.data_);
        return result;
    }

    template<BitWidth bits, Signedness signedness>
    constexpr bigint<bits, signedness> abs(bigint<bits, signedness> const &data) {
        if constexpr (signedness == Signedness::Unsigned) {
            return data;
        } else {
            auto result = bigint<bits, signedness>{data};
            if (result < static_cast<std::int8_t>(0)) {
                result = -result;
            }
            return result;
        }
    }
}
