//
// Created by Rene Windegger on 03/04/2025.
//

#include <bigint23/bigint.hpp>
#include <gtest/gtest.h>

TEST(bigint23, byteswap_test) {
    bigint::bigint<bigint::BitWidth{128}, bigint::Signedness::Unsigned> expected;
    if constexpr (std::endian::native == std::endian::little) {
        expected = "0x78563412000000000000000000000000";
    } else {
        expected = "0x12345678";
    }
    bigint::bigint<bigint::BitWidth{128}, bigint::Signedness::Unsigned> const input = 0x12345678;
    auto actual = byteswap(input);
    auto const actual_ptr = reinterpret_cast<char const *>(std::addressof(actual));
    auto const expected_ptr = reinterpret_cast<char const *>(std::addressof(expected));
    for (int i = 0; i < sizeof(actual); i++) {
        ASSERT_EQ(expected_ptr[i], actual_ptr[i]);
    }
}

TEST(bigint23, abs_test) {
    bigint::bigint<bigint::BitWidth{128}, bigint::Signedness::Unsigned> const expected = 1234567890;
    bigint::bigint<bigint::BitWidth{128}, bigint::Signedness::Signed> const input = -1234567890;
    auto actual1 = abs(input);
    ASSERT_EQ(actual1, expected);
    auto actual2 = abs(expected);
    ASSERT_EQ(actual2, expected);
}
