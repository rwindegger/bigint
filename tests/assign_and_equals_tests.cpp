//
// Created by Rene Windegger on 21/03/2025.
//

#include <bigint23/bigint.h>
#include <gtest/gtest.h>

namespace {
    TEST(bigint23, assign_and_equals_8bit_test) {
        using uint128_t = bigint::bigint<128, false>;
        using int128_t = bigint::bigint<128, true>;
        using u8 = bigint::bigint<8, false>;
        using i8 = bigint::bigint<8, true>;

        uint128_t const a = static_cast<std::uint8_t>(42);
        ASSERT_EQ(static_cast<std::uint8_t>(42), a);
        int128_t const b = static_cast<std::int8_t>(-42);
        ASSERT_EQ(static_cast<std::int8_t>(-42), b);
        uint128_t const c = static_cast<std::int8_t>(42);
        ASSERT_EQ(static_cast<std::uint8_t>(42), c);
        uint128_t d = static_cast<std::uint8_t>(128);
        ASSERT_EQ(static_cast<std::uint8_t>(128), d);
        d = static_cast<std::uint8_t>(70);
        ASSERT_EQ(static_cast<std::uint8_t>(70), d);
        uint128_t const e = static_cast<std::int8_t>(-42);
        ASSERT_NE(static_cast<std::int8_t>(-42), e);
        uint128_t const f = static_cast<std::int8_t>(42);
        ASSERT_EQ(static_cast<std::int8_t>(42), f);
        u8 const g = static_cast<std::int8_t>(-42);
        ASSERT_NE(static_cast<std::int8_t>(-42), g);
        i8 const h = static_cast<std::int8_t>(-42);
        ASSERT_EQ(static_cast<std::int8_t>(-42), h);
        u8 const i = static_cast<std::int8_t>(42);
        ASSERT_EQ(static_cast<std::int8_t>(42), i);
    }

    TEST(bigint23, assign_and_equals_16bit_test) {
        using uint128_t = bigint::bigint<128, false>;
        using int128_t = bigint::bigint<128, true>;
        using u16 = bigint::bigint<16, false>;
        using i16 = bigint::bigint<16, true>;

        uint128_t const a = static_cast<std::uint16_t>(42);
        ASSERT_EQ(static_cast<std::uint16_t>(42), a);
        int128_t const b = static_cast<std::int16_t>(-42);
        ASSERT_EQ(static_cast<std::int16_t>(-42), b);
        uint128_t const c = static_cast<std::int16_t>(42);
        ASSERT_EQ(static_cast<std::uint16_t>(42), c);
        uint128_t d = static_cast<std::uint16_t>(128);
        ASSERT_EQ(static_cast<std::uint16_t>(128), d);
        d = static_cast<std::uint16_t>(70);
        ASSERT_EQ(static_cast<std::uint16_t>(70), d);
        uint128_t const e = static_cast<std::int16_t>(-42);
        ASSERT_NE(static_cast<std::int16_t>(-42), e);
        uint128_t const f = static_cast<std::int16_t>(42);
        ASSERT_EQ(static_cast<std::int16_t>(42), f);
        u16 const g = static_cast<std::int16_t>(-42);
        ASSERT_NE(static_cast<std::int16_t>(-42), g);
        i16 const h = static_cast<std::int16_t>(-42);
        ASSERT_EQ(static_cast<std::int16_t>(-42), h);
        u16 const i = static_cast<std::int16_t>(42);
        ASSERT_EQ(static_cast<std::int16_t>(42), i);
    }

    TEST(bigint23, assign_and_equals_32bit_test) {
        using uint128_t = bigint::bigint<128, false>;
        using int128_t = bigint::bigint<128, true>;
        using u32 = bigint::bigint<32, false>;
        using i32 = bigint::bigint<32, true>;

        uint128_t const a = static_cast<std::uint32_t>(42);
        ASSERT_EQ(static_cast<std::uint32_t>(42), a);
        int128_t const b = static_cast<std::int32_t>(-42);
        ASSERT_EQ(static_cast<std::int32_t>(-42), b);
        uint128_t const c = static_cast<std::int32_t>(42);
        ASSERT_EQ(static_cast<std::uint32_t>(42), c);
        uint128_t d = static_cast<std::uint32_t>(128);
        ASSERT_EQ(static_cast<std::uint32_t>(128), d);
        d = static_cast<std::uint32_t>(70);
        ASSERT_EQ(static_cast<std::uint32_t>(70), d);
        uint128_t const e = static_cast<std::int32_t>(-42);
        ASSERT_NE(static_cast<std::int32_t>(-42), e);
        uint128_t const f = static_cast<std::int32_t>(42);
        ASSERT_EQ(static_cast<std::int32_t>(42), f);
        u32 const g = static_cast<std::int32_t>(-42);
        ASSERT_NE(static_cast<std::int32_t>(-42), g);
        i32 const h = static_cast<std::int32_t>(-42);
        ASSERT_EQ(static_cast<std::int32_t>(-42), h);
        u32 const i = static_cast<std::int32_t>(42);
        ASSERT_EQ(static_cast<std::int32_t>(42), i);
    }

    TEST(bigint23, assign_and_equals_64bit_test) {
        using uint128_t = bigint::bigint<128, false>;
        using int128_t = bigint::bigint<128, true>;
        using u64 = bigint::bigint<64, false>;
        using i64 = bigint::bigint<64, true>;

        uint128_t const a = static_cast<std::uint64_t>(42);
        ASSERT_EQ(static_cast<std::uint64_t>(42), a);
        int128_t const b = static_cast<std::int64_t>(-42);
        ASSERT_EQ(static_cast<std::int64_t>(-42), b);
        uint128_t const c = static_cast<std::int64_t>(42);
        ASSERT_EQ(static_cast<std::uint64_t>(42), c);
        uint128_t d = static_cast<std::uint64_t>(128);
        ASSERT_EQ(static_cast<std::uint64_t>(128), d);
        d = static_cast<std::uint64_t>(70);
        ASSERT_EQ(static_cast<std::uint64_t>(70), d);
        uint128_t const e = static_cast<std::int64_t>(-42);
        ASSERT_NE(static_cast<std::int64_t>(-42), e);
        uint128_t const f = static_cast<std::int64_t>(42);
        ASSERT_EQ(static_cast<std::int64_t>(42), f);
        u64 const g = static_cast<std::int64_t>(-42);
        ASSERT_NE(static_cast<std::int64_t>(-42), g);
        i64 const h = static_cast<std::int64_t>(-42);
        ASSERT_EQ(static_cast<std::int64_t>(-42), h);
        u64 const i = static_cast<std::int64_t>(42);
        ASSERT_EQ(static_cast<std::int64_t>(42), i);
    }

    TEST(bigint23, assign_and_equals_large_test) {
        using uint128_t = bigint::bigint<128, false>;
        using int128_t = bigint::bigint<128, true>;
        using uint256_t = bigint::bigint<256, false>;
        using int256_t = bigint::bigint<256, true>;
        using u16 = bigint::bigint<16, false>;
        using u32 = bigint::bigint<32, false>;
        using i16 = bigint::bigint<16, true>;
        using i32 = bigint::bigint<32, true>;

        uint128_t const pos_uint = static_cast<std::uint64_t>(42);
        uint256_t const b = pos_uint;
        ASSERT_EQ(pos_uint, b);
        uint128_t const c = pos_uint;
        ASSERT_EQ(c, b);
        int128_t const neg_int = static_cast<std::int64_t>(-42);
        uint256_t const e = neg_int;
        ASSERT_NE(neg_int, e);
        int256_t const f = neg_int;
        ASSERT_EQ(neg_int, f);
        uint256_t const g = static_cast<std::uint64_t>(70);
        ASSERT_NE(g, b);
        u16 const h = static_cast<std::uint16_t>(0x42);
        u32 const i = static_cast<std::uint32_t>(0x42120042);
        ASSERT_NE(i, h);
        i16 const j = static_cast<std::int16_t>(-42);
        i32 const k = static_cast<std::int32_t>(0xFFFFFFFFFFD6FFD6);
        ASSERT_NE(j, k);
        uint128_t const l = "0x5123019482ab";
        constexpr std::uint64_t m = 0x5123019482ab;
        ASSERT_EQ(m, l);
        uint128_t const n = "100'000'123";
        constexpr std::uint64_t o = 100'000'123;
        ASSERT_EQ(o, n);
        uint128_t const p = "0b10010010";
        constexpr std::uint64_t q = 0b10010010;
        ASSERT_EQ(q, p);
        uint128_t const r = "01274231277";
        constexpr std::uint64_t s = 01274231277;
        ASSERT_EQ(s, r);
        uint128_t const t = "0x1234567890abcdef1234567890abcdef";
        uint256_t const u = "0x1234567890ABCDEF1234567890ABCDEF";
        ASSERT_EQ(u, t);
        ASSERT_THROW(uint128_t const v = "0x1234567890abcdef1234567890abcdefabcdef", std::overflow_error);
        uint128_t x = "0x1234";
        constexpr std::uint64_t y = 0x1234;
        ASSERT_EQ(y, x);
        x = "0x2345";
        constexpr std::uint64_t z = 0x2345;
        ASSERT_EQ(z, x);
        int128_t const aa = "-42";
        ASSERT_EQ(-42, aa);
        ASSERT_THROW(uint128_t const ab = "-42", std::runtime_error);
        ASSERT_THROW(uint128_t const ac = "0xG2", std::runtime_error);
        ASSERT_THROW(uint128_t const ad = "081", std::runtime_error);

        int128_t const ae = std::string("-1234");
        ASSERT_EQ(ae, -1234);
        uint128_t const af = std::string("0x1234");
        ASSERT_EQ(af, 0x1234);
        uint128_t const ag = std::string("01234");
        ASSERT_EQ(ag, 01234);
        uint128_t const ah = std::string("0b10010101");
        ASSERT_EQ(ah, 0b10010101);
        uint128_t const ai = std::string("1234");
        ASSERT_EQ(ai, 1234);
        ASSERT_THROW(uint128_t const aj = std::string("-1234"), std::runtime_error);
        uint128_t ak = std::string("0b10010101");
        ASSERT_EQ(ak, 0b10010101);
        ak = std::string("1234");
        ASSERT_EQ(ak, 1234);
    }
}