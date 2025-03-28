//
// Created by Rene Windegger on 28/03/2025.
//

#include <bigint/bigint.h>
#include <gtest/gtest.h>
#include <sstream>

namespace {
    TEST(bigint, decimal_positive_os_test) {
        bigint::bigint<128, true> a(123456789);
        std::ostringstream oss;
        oss << std::dec << a;
        ASSERT_EQ(oss.str(), "123456789");
    }

    TEST(bigint, decimal_negative_os_test) {
        bigint::bigint<128, true> a(-123456789);
        std::ostringstream oss;
        oss << std::dec << a;
        ASSERT_EQ(oss.str(), "-123456789");
    }

    TEST(bigint, hex_unsigned_os_test) {
        bigint::bigint<128, false> a(0x1A2B3C4D);
        std::ostringstream oss;
        oss << std::hex << std::nouppercase << a;
        ASSERT_EQ(oss.str(), "1a2b3c4d");
    }

    TEST(bigint, hex_signed_negative_os_test) {
        bigint::bigint<128, true> a(-123456789);
        std::ostringstream oss;
        oss << std::hex << std::nouppercase << a;
        ASSERT_EQ(oss.str(), "fffffffffffffffffffffff8a432eb");
    }

    TEST(bigint, octal_unsigned_os_test) {
        bigint::bigint<128, false> a(123456);
        std::ostringstream oss;
        oss << std::oct << a;
        ASSERT_EQ(oss.str(), "361100");
    }

    TEST(bigint, positive_decimal_is_test) {
        std::istringstream iss("123456789");
        bigint::bigint<128, true> a;
        iss >> a;
        ASSERT_EQ(a, 123456789);
    }

    TEST(bigint, NegativeDecimal) {
        std::istringstream iss("-987654321");
        bigint::bigint<128, true> a;
        iss >> a;
        ASSERT_EQ(a, -987654321);
    }

    TEST(bigint, Hexadecimal) {
        std::istringstream iss("1a2b3c4d");
        iss >> std::hex;
        bigint::bigint<128, false> a;
        iss >> a;
        ASSERT_EQ(a, 0x1a2b3c4d);
    }

    TEST(bigint, Octal) {
        std::istringstream iss("361100");
        iss >> std::oct;
        bigint::bigint<128, false> a;
        iss >> a;
        ASSERT_EQ(a, 0361100);
    }
}
