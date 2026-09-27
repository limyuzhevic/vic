#pragma once

/**
 * Test runner framework
 */
#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace nlm {
namespace testing {

class TestRunner {
public:
    void runAllTests();
    void registerTestSuite(std::function<void()> suite);
    void generateReport();
};

} // namespace testing
} // namespace nlm