#include "framework/test_base.hpp"
#include <unordered_set>
#include <set>
#include <sstream>

// Static member definitions
std::unordered_map<void*, std::string> test::MemoryTracker::allocations;

namespace test {

// Global test runner instance
TestRunner& getTestRunner() {
    static TestRunner runner;
    return runner;
}

// Test suite implementations
TestSuite::TestSuite() {}

TestSuite::~TestSuite() {}

// Parameterized test support
template<typename TestClass>
void registerParameterizedTest(const std::string& name, TestClass* test) {
    getTestRunner().registerTestSuite(test);
}

// Test registration macros
#define REGISTER_TEST_SUITE(suiteName, suiteInstance) \
    namespace { \
        TestSuite* suite##_registration##_ = register_test_suite(suiteName, suiteInstance); \
    }

// Utility functions
std::string generateTestName(const std::string& testName, int param) {
    std::stringstream ss;
    ss << testName << "_param_" << param;
    return ss.str();
}

// Assertion utilities
void printAssertionFailure(const std::string& file, int line, const std::string& message) {
    std::cerr << "FAILED: " << file << ":" << line << " " << message << std::endl;
}

} // namespace test

// Global initialization
void initializeTestFramework() {
    // Register built-in test suites
    // These would be automatically discovered
    test::getTestRunner();
}

void shutdownTestFramework() {
    test::MemoryTracker::reportLeaks();
}