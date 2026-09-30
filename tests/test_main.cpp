// Main Tests
// Test main functionality and integration

#include <gtest/gtest.h>

namespace nlm {

TEST(MainTest, TestFramework) {
    EXPECT_TRUE(true) << "Basic test framework functionality";
}

TEST(MainTest, MultipleTests) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}

} // namespace nlm

