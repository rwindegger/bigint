//
// Created by Rene Windegger on 22/03/2025.
//

#include <bigint23/bigint23.h>
#include <gtest/gtest.h>

namespace {
    TEST(bigint23, compare_8bit_test) {
        using uint128_t = bigint23::bigint23<128, false>;
        using int128_t = bigint23::bigint23<128, true>;
        uint128_t const a = static_cast<uint8_t>(0x42);
        ASSERT_GT(static_cast<uint8_t>(0x43), a);
        ASSERT_LT(static_cast<uint8_t>(0x41), a);
        uint128_t const b = static_cast<uint16_t>(0x1042);
        ASSERT_LT(static_cast<uint8_t>(0x42), b);
        int128_t const c = static_cast<int8_t>(0xD6);
        ASSERT_LT(static_cast<int8_t>(0xD5), c);
        ASSERT_GT(static_cast<int8_t>(0xD7), c);
    }

    TEST(bigint23, compare_16bit_test) {
        using uint128_t = bigint23::bigint23<128, false>;
        using int128_t = bigint23::bigint23<128, true>;
        uint128_t const a = static_cast<uint16_t>(0x42);
        ASSERT_GT(static_cast<uint16_t>(0x43), a);
        ASSERT_LT(static_cast<uint16_t>(0x41), a);
        uint128_t const b = static_cast<uint32_t>(0x100042);
        ASSERT_LT(static_cast<uint16_t>(0x42), b);
        int128_t const c = static_cast<int16_t>(0xD6);
        ASSERT_LT(static_cast<int16_t>(0xD5), c);
        ASSERT_GT(static_cast<int16_t>(0xD7), c);
    }

    TEST(bigint23, compare_32bit_test) {
        using uint128_t = bigint23::bigint23<128, false>;
        using int128_t = bigint23::bigint23<128, true>;
        uint128_t const a = static_cast<uint32_t>(0x42);
        ASSERT_GT(static_cast<uint32_t>(0x43), a);
        ASSERT_LT(static_cast<uint32_t>(0x41), a);
        uint128_t const b = static_cast<uint64_t>(0x1000000042);
        ASSERT_LT(static_cast<uint32_t>(0x42), b);
        int128_t const c = static_cast<int32_t>(0xD6);
        ASSERT_LT(static_cast<int32_t>(0xD5), c);
        ASSERT_GT(static_cast<int32_t>(0xD7), c);
    }

    TEST(bigint23, complare_large_test) {
        using uint128_t = bigint23::bigint23<128, false>;
        using int128_t = bigint23::bigint23<128, true>;
        uint128_t const a = static_cast<uint64_t>(0x42);
        uint128_t const b = static_cast<uint64_t>(0x43);
        ASSERT_LT(a, b);
        ASSERT_GT(b, a);
        int128_t const c = static_cast<int64_t>(0xD6);
        ASSERT_GT(a, c);
        ASSERT_LT(c, a);
        int128_t const d = static_cast<int64_t>(0xD7);
        ASSERT_GT(d, c);
        ASSERT_LT(c, d);
        int128_t const e = -124592;
        ASSERT_LT(e, d);
        int128_t const f = static_cast<int64_t>(0x43);
        ASSERT_GT(f, a);
        ASSERT_LT(a, f);
    }
}