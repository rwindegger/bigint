//
// Created by Rene Windegger on 27/03/2025.
//

#include <bigint23/bigint.hpp>
#include <gtest/gtest.h>

namespace {
    TEST(bigint23, binary_and_test) {
        using U32 = bigint::bigint<bigint::BitWidth{32}, bigint::Signedness::Unsigned>;
        U32 const a = "0x5a3b3216";
        ASSERT_EQ(a & 0x415185ab, 0x40110002);
    }

    TEST(bigint23, binary_negation_test) {
        using U32 = bigint::bigint<bigint::BitWidth{32}, bigint::Signedness::Unsigned>;
        U32 const a = "0";
        ASSERT_EQ(~a, 0xFFFFFFFF);
        U32 const b = "0x41ED7899";
        ASSERT_EQ(~b, 0xBE128766);
    }

    TEST(bigint23, binary_or_test) {
        using U32 = bigint::bigint<bigint::BitWidth{32}, bigint::Signedness::Unsigned>;
        U32 const a = "0x5a3b3216";
        ASSERT_EQ(a | 0x415185ab, 0x5B7BB7BF);
    }

    TEST(bigint23, binary_xor_test) {
        using U32 = bigint::bigint<bigint::BitWidth{32}, bigint::Signedness::Unsigned>;
        U32 const a = "0x5a3b3216";
        ASSERT_EQ(a ^ 0x415185ab, 0x1B6AB7BD);
    }
}