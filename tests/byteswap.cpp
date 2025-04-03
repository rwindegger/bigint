//
// Created by Rene Windegger on 03/04/2025.
//

#include <bigint23/bigint23.h>
#include <gtest/gtest.h>

TEST(bigint23, byteswap_test) {
    bigint23::bigint23<128, false> expected;
    if constexpr (std::endian::native == std::endian::little) {
        expected = "0x78563412000000000000000000000000";
    } else {
        expected = "0x12345678";
    }
    bigint23::bigint23<128, false> const input = 0x12345678;
    auto actual = byteswap(input);
    auto const actual_ptr = reinterpret_cast<char const *>(std::addressof(actual));
    auto const expected_ptr = reinterpret_cast<char const *>(std::addressof(expected));
    for (int i = 0; i < sizeof(actual); i++) {
        ASSERT_EQ(expected_ptr[i], actual_ptr[i]);
    }
}
