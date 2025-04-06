//
// Created by Rene Windegger on 21/03/2025.
//

#include <bigint23/bigint23.h>
#include <gtest/gtest.h>

namespace {
    TEST(bigint23, addition_with_integral_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const c = "0xffffffff";
        ASSERT_EQ(c + 1, 0);
        ASSERT_EQ(c + 2, 1);
        i32 const f = "-42";
        ASSERT_EQ(f + (-1), -43);
        ASSERT_EQ(f + 1, -41);
    }

    TEST(bigint23, addition_with_bigint23_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const c = "0xffffffff";
        u32 const d = "0x1";
        ASSERT_EQ(c + d, 0);
        u32 const e = "0x2";
        ASSERT_EQ(c + e, 1);
        i32 const f = "-42";
        i32 const g = -1;
        ASSERT_EQ(f + g, -43);
        ASSERT_EQ(f + d, -41);
    }

    TEST(bigint23, multiplication_with_integral_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 42;
        ASSERT_EQ(a * 43, 1806);
        i32 const c = 2018238;
        ASSERT_EQ(c * 42, 84765996);
        u32 const d = 0xFFFFFFFF;
        ASSERT_EQ(d * 2, 0xFFFFFFFE);
        i32 const e = 42;
        ASSERT_EQ(e * -2, -84);
    }

    TEST(bigint23, multiplication_with_bigint23_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        using i16 = bigint23::bigint23<16, true>;
        using u16 = bigint23::bigint23<16, false>;
        u32 const a = 42;
        u32 const b = 43;
        ASSERT_EQ(a * b, 1806);
        i32 const c = 2018238;
        ASSERT_EQ(c * a, 84765996);
        u32 const d = 0xFFFFFFFF;
        ASSERT_EQ(d * 2, 0xFFFFFFFE);
        i32 const e = 42;
        ASSERT_EQ(e * -2, -84);
        i16 const f = static_cast<std::int16_t>(-42);
        i16 const g = static_cast<std::int16_t>(-42);
        ASSERT_EQ(e * f, -1764);
        ASSERT_EQ(f * g, static_cast<std::int16_t>(1764));
    }

    TEST(bigint23, subtraction_with_integral_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 42;
        ASSERT_EQ(a - 1, 41);
        ASSERT_EQ(a - 2, 40);
        i32 const c = -42;
        ASSERT_EQ(c - 1, -43);
        ASSERT_EQ(c - (-1), -41);
        u32 const d = 0;
        ASSERT_EQ(d - 1, 0xFFFFFFFF);
    }

    TEST(bigint23, subtraction_with_bigint23_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 42;
        u32 const b = 1;
        ASSERT_EQ(a - b, 41);
        u32 const c = 2;
        ASSERT_EQ(a - c, 40);
        i32 const d = -42;
        ASSERT_EQ(d - b, -43);
        i32 const e = -1;
        ASSERT_EQ(d - e, -41);
        u32 const f = 0;
        ASSERT_EQ(f - b, 0xFFFFFFFF);
    }

    TEST(bigint23, division_with_integral_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 15;
        ASSERT_EQ(a / 3, 5);
        ASSERT_EQ(a / 2, 7);
        i32 const c = 15;
        ASSERT_EQ(c / -3, -5);
        ASSERT_THROW(auto d = c / 0, std::overflow_error);
    }

    TEST(bigint23, division_with_bigint23_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 15;
        u32 const b = 3;
        ASSERT_EQ(a / b, 5);
        u32 const c = 2;
        ASSERT_EQ(a / c, 7);
        i32 const d = 15;
        i32 const e = -3;
        ASSERT_EQ(d / e, -5);
        u32 const f = 0;
        ASSERT_THROW(auto g = d / f, std::overflow_error);
    }

    TEST(bigint23, modulo_with_integral_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 15;
        ASSERT_EQ(a % 3, 0);
        ASSERT_EQ(a % 2, 1);
        i32 const c = 15;
        ASSERT_EQ(c % -3, 0);
        ASSERT_THROW(auto d = a % 0, std::overflow_error);
    }

    TEST(bigint23, modulo_with_bigint23_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 15;
        u32 const b = 3;
        ASSERT_EQ(a % b, 0);
        u32 const c = 2;
        ASSERT_EQ(a % c, 1);
        i32 const d = 15;
        i32 const e = -3;
        ASSERT_EQ(d % e, 0);
        u32 const f = 0;
        ASSERT_THROW(auto g = d % f, std::overflow_error);
    }

    TEST(bigint23, shift_test) {
        using i32 = bigint23::bigint23<32, true>;
        using u32 = bigint23::bigint23<32, false>;
        u32 const a = 42;
        ASSERT_EQ(a >> 2, 10);
        ASSERT_EQ(a << 2, 168);
        ASSERT_EQ(a >> 0, 42);
        ASSERT_EQ(a << 0, 42);
        ASSERT_EQ(a >> 64, 0);
        ASSERT_EQ(a << 8, 10752);
        u32 const b = 10752;
        ASSERT_EQ(b >> 8, 42);
        i32 const c = -42;
        ASSERT_EQ(c >> 2, -11);
        ASSERT_EQ(c << 2, -168);
        ASSERT_EQ(c >> 0, -42);
        ASSERT_EQ(c << 0, -42);
        ASSERT_EQ(c >> 64, 0xFFFFFFFF);
    }

    TEST(bigint23, increment_test) {
        using i32 = bigint23::bigint23<32, true>;
        i32 a = 42;
        ASSERT_EQ(++a, 43);
        ASSERT_EQ(a++, 43);
        ASSERT_EQ(a, 44);
    }

    TEST(bigint23, decrement_test) {
        using i32 = bigint23::bigint23<32, true>;
        i32 a = 42;
        ASSERT_EQ(--a, 41);
        ASSERT_EQ(a--, 41);
        ASSERT_EQ(a, 40);
    }

    TEST(bigint23, unary_minus_test) {
        using i8 = bigint23::bigint23<8, true>;
        using u8 = bigint23::bigint23<8, false>;
        i8 const a = static_cast<int8_t>(0x80);
        ASSERT_THROW(auto test = -a, std::overflow_error);
        i8 const b = static_cast<int8_t>(-15);
        ASSERT_EQ(static_cast<int8_t>(15), -b);
        i8 const c = static_cast<int8_t>(15);
        ASSERT_EQ(static_cast<int8_t>(-15), -c);
        constexpr uint8_t expected = 15;
        u8 const actual = expected;
        ASSERT_EQ(-actual, static_cast<uint8_t>(-expected));
    }
}
