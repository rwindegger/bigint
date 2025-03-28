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
        EXPECT_EQ(oss.str(), "123456789");
    }

    TEST(bigint, decimal_negative_os_test) {
        bigint::bigint<128, true> a(-123456789);
        std::ostringstream oss;
        oss << std::dec << a;
        EXPECT_EQ(oss.str(), "-123456789");
    }

    TEST(bigint, hex_unsigned_os_test) {
        bigint::bigint<128, false> a(0x1A2B3C4D);
        std::ostringstream oss;
        oss << std::hex << std::nouppercase << a;
        EXPECT_EQ(oss.str(), "1a2b3c4d");
    }

    TEST(bigint, hex_signed_negative_os_test) {
        bigint::bigint<128, true> a(-123456789);
        std::ostringstream oss;
        oss << std::hex << std::nouppercase << a;
        EXPECT_EQ(oss.str(), "fffffffffffffffffffffff8a432eb");
    }

    TEST(bigint, octal_unsigned_os_test) {
        bigint::bigint<128, false> a(123456);
        std::ostringstream oss;
        oss << std::oct << a;
        EXPECT_EQ(oss.str(), "361100");
    }
}
