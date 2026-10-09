// NLM Test Framework Implementation
// Implements the improved test runner with better reporting

#include "nlm/TestFramework.hpp"

namespace nlm {
    // Test runner instance for automatic registration
    static TestRunner& getDefaultTestRunner() {
        static TestRunner runner;
        return runner;
    }
}

// Helper function to run all tests automatically
void runNLMTests() {
    nlm::getDefaultTestRunner().runAll();
}
