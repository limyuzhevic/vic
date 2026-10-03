// TestSuite.hpp
#pragma once

#include "TestFramework.hpp"
#include <algorithm>

namespace nlm {
namespace test {

// Helper function for test comparison
inline bool testNameCompare(const std::unique_ptr<TestBase>& a, const std::unique_ptr<TestBase>& b) {
    return (*a) < (*b);
}

} // namespace test
} // namespace nlm