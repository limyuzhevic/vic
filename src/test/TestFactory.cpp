// TestFactory.cpp
#include "TestFactory.hpp"
#include <fstream>
#include <sstream>
#include <regex>

namespace nlm {
namespace test {

bool TestFactory::discoverTests(const std::string& pattern) {
    // Discover test classes from files matching the pattern
    // This is a simplified implementation
    return false;
}

bool TestFactory::discoverSuites(const std::string& pattern) {
    // Discover test suites from files matching the pattern
    // This is a simplified implementation
    return false;
}

std::string TestFactory::exportTestInfo() const {
    std::stringstream ss;
    ss << "Registered Tests:\n";
    for (const auto& pair : testFactories_) {
        const auto& metadata = pair.second.metadata;
        ss << "  - " << metadata.name << ": " << metadata.description
           << " (Category: " << metadata.category << ")\n";
    }
    return ss.str();
}

std::string TestFactory::exportSuiteInfo() const {
    std::stringstream ss;
    ss << "Registered Suites:\n";
    for (const auto& pair : suiteFactories_) {
        const auto& metadata = pair.second.metadata;
        ss << "  - " << metadata.name << ": " << metadata.description
           << " (Category: " << metadata.category << ")\n";
    }
    return ss.str();
}

std::vector<std::unique_ptr<TestBase>> TestFactory::createTestsFromConfig(const TestConfiguration& config) {
    std::vector<std::unique_ptr<TestBase>> tests;
    // Create tests based on configuration
    return tests;
}

std::vector<std::unique_ptr<TestSuite>> TestFactory::createSuitesFromConfig(const TestConfiguration& config) {
    std::vector<std::unique_ptr<TestSuite>> suites;
    // Create suites based on configuration
    return suites;
}

} // namespace test
} // namespace nlm