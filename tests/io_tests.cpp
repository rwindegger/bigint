//
// Created by Rene Windegger on 28/03/2025.
//

#include <bigint/bigint.h>
#include <gtest/gtest.h>
#include <sstream>

namespace {
    TEST(bigint, decimal_positive_os_test) {
        bigint::bigint<128, true> const a(123456789);
        std::ostringstream oss;
        oss << std::dec << a;
        ASSERT_EQ(oss.str(), "123456789");
    }

    TEST(bigint, decimal_negative_os_test) {
        bigint::bigint<128, true> const a(-123456789);
        std::ostringstream oss;
        oss << std::dec << a;
        ASSERT_EQ(oss.str(), "-123456789");
    }

    TEST(bigint, hex_unsigned_os_test) {
        bigint::bigint<128, false> const a(0x1A2B3C4D);
        std::ostringstream oss;
        oss << std::hex << std::nouppercase << a;
        ASSERT_EQ(oss.str(), "1a2b3c4d");
    }

    TEST(bigint, hex_signed_negative_os_test) {
        bigint::bigint<128, true> const a(-123456789);
        std::ostringstream oss;
        oss << std::hex << std::nouppercase << a;
        ASSERT_EQ(oss.str(), "fffffffffffffffffffffff8a432eb");
    }

    TEST(bigint, octal_unsigned_os_test) {
        bigint::bigint<128, false> const a(123456);
        std::ostringstream oss;
        oss << std::oct << a;
        ASSERT_EQ(oss.str(), "361100");
    }

    TEST(bigint, decimal_zero_os_test) {
        bigint::bigint<128, false> const a(0);
        std::ostringstream oss;
        oss << a;
        ASSERT_EQ(oss.str(), "0");
    }

    TEST(bigint, octal_zero_os_test) {
        bigint::bigint<128, false> const a(0);
        std::ostringstream oss;
        oss << std::oct << a;
        ASSERT_EQ(oss.str(), "0");
    }

    TEST(bigint, hexadecimal_zero_os_test) {
        bigint::bigint<128, false> const a(0);
        std::ostringstream oss;
        oss << std::hex << a;
        ASSERT_EQ(oss.str(), "00");
    }

    TEST(bigint, positive_decimal_is_test) {
        std::istringstream iss("123456789");
        bigint::bigint<128, true> a;
        iss >> a;
        ASSERT_EQ(a, 123456789);
    }

    TEST(bigint, negative_decimal_is_test) {
        std::istringstream iss("-987654321");
        bigint::bigint<128, true> a;
        iss >> a;
        ASSERT_EQ(a, -987654321);
    }

    TEST(bigint, hexadecimal_is_test) {
        std::istringstream iss("1a2b3c4d");
        iss >> std::hex;
        bigint::bigint<128, false> a;
        iss >> a;
        ASSERT_EQ(a, 0x1a2b3c4d);
    }

    TEST(bigint, octal_is_test) {
        std::istringstream iss("361100");
        iss >> std::oct;
        bigint::bigint<128, false> a;
        iss >> a;
        ASSERT_EQ(a, 0361100);
    }

    TEST(bigint, empty_steam_is_test) {
        std::istringstream iss("");
        bigint::bigint<128, false> a;
        iss >> a;
        ASSERT_EQ(a, 0);
    }

    TEST(bigint, negative_hexadecimal_fails_is_test) {
        std::istringstream iss("-987654321");
        iss >> std::hex;
        bigint::bigint<128, false> a;
        iss >> a;
        ASSERT_EQ(a, 0);
    }
}
