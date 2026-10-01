#pragma once

#include <string>

namespace nlm {
namespace tests {

// Test result tracking
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    
    TestResult(const std::string& testName, bool testPassed, const std::string& testMessage)
        : name(testName), passed(testPassed), message(testMessage) {}
};

// Global test counters
int g_configTestsPassed = 0;
int g_configTestsFailed = 0;
int g_plasticityTestsPassed = 0;
int g_plasticityTestsFailed = 0;
int g_memoryTestsPassed = 0;
int g_memoryTestsFailed = 0;
int g_debugTestsPassed = 0;
int g_debugTestsFailed = 0;

// Getters for test results
int getConfigPassedCount() { return g_configTestsPassed; }
int getConfigFailedCount() { return g_configTestsFailed; }
int getPlasticityPassedCount() { return g_plasticityTestsPassed; }
int getPlasticityFailedCount() { return g_plasticityTestsFailed; }
int getMemoryPassedCount() { return g_memoryTestsPassed; }
int getMemoryFailedCount() { return g_memoryTestsFailed; }
int getDebugPassedCount() { return g_debugTestsPassed; }
int getDebugFailedCount() { return g_debugTestsFailed; }

// Integration test runner interface
void runAllIntegrationTests();

// Individual test suites
int runConfigTests();
int runPlasticityTests();
int runMemoryTests();
int runDebugTests();

} // namespace tests
} // namespace nlm