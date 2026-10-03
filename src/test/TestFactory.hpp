// TestFactory.hpp
#pragma once

#include "TestFramework.hpp"
#include "TestConfiguration.hpp"
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace test {

// Test factory function type
using TestFactoryFunction = std::function<std::unique_ptr<TestBase>()>;
using TestSuiteFactoryFunction = std::function<std::unique_ptr<TestSuite>()>;

// Test metadata for factory registration
struct TestMetadata {
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    TestSeverity severity;
    std::string file;
    int line;
    std::unordered_map<std::string, std::any> metadata;

    TestMetadata() : severity(TestSeverity::Medium) {}
    TestMetadata(const std::string& name, const std::string& description,
                 const std::string& category, TestSeverity severity = TestSeverity::Medium)
        : name(name), description(description), category(category), severity(severity) {}
};

// Test factory for dynamic test creation
class TestFactory {
public:
    static TestFactory& getInstance() {
        static TestFactory instance;
        return instance;
    }

    // Register a test factory function
    void registerTest(const std::string& name, TestFactoryFunction factory, const TestMetadata& metadata = TestMetadata()) {
        testFactories_[name] = {factory, metadata};
    }

    // Register a suite factory function
    void registerSuite(const std::string& name, TestSuiteFactoryFunction factory, const TestMetadata& metadata = TestMetadata()) {
        suiteFactories_[name] = {factory, metadata};
    }

    // Create a test instance by name
    std::unique_ptr<TestBase> createTest(const std::string& name) {
        auto it = testFactories_.find(name);
        if (it != testFactories_.end()) {
            return it->second.factory();
        }
        return nullptr;
    }

    // Create a suite instance by name
    std::unique_ptr<TestSuite> createSuite(const std::string& name) {
        auto it = suiteFactories_.find(name);
        if (it != suiteFactories_.end()) {
            return it->second.factory();
        }
        return nullptr;
    }

    // Get test metadata
    const TestMetadata& getTestMetadata(const std::string& name) const {
        static TestMetadata emptyMetadata;
        auto it = testFactories_.find(name);
        if (it != testFactories_.end()) {
            return it->second.metadata;
        }
        return emptyMetadata;
    }

    // Get suite metadata
    const TestMetadata& getSuiteMetadata(const std::string& name) const {
        static TestMetadata emptyMetadata;
        auto it = suiteFactories_.find(name);
        if (it != suiteFactories_.end()) {
            return it->second.metadata;
        }
        return emptyMetadata;
    }

    // Get all registered test names
    std::vector<std::string> getRegisteredTestNames() const {
        std::vector<std::string> names;
        names.reserve(testFactories_.size());
        for (const auto& pair : testFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }

    // Get all registered suite names
    std::vector<std::string> getRegisteredSuiteNames() const {
        std::vector<std::string> names;
        names.reserve(suiteFactories_.size());
        for (const auto& pair : suiteFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }

    // Clear all registrations
    void clear() {
        testFactories_.clear();
        suiteFactories_.clear();
    }

    // Discover and register tests from files
    bool discoverTests(const std::string& pattern = "test_*.cpp");
    bool discoverSuites(const std::string& pattern = "test_*.cpp");

    // Export test information
    std::string exportTestInfo() const;
    std::string exportSuiteInfo() const;

    // Configuration-based test creation
    std::vector<std::unique_ptr<TestBase>> createTestsFromConfig(const TestConfiguration& config);
    std::vector<std::unique_ptr<TestSuite>> createSuitesFromConfig(const TestConfiguration& config);

private:
    TestFactory() = default;

    struct FactoryEntry {
        TestFactoryFunction factory;
        TestMetadata metadata;
    };

    struct SuiteFactoryEntry {
        TestSuiteFactoryFunction factory;
        TestMetadata metadata;
    };

    std::unordered_map<std::string, FactoryEntry> testFactories_;
    std::unordered_map<std::string, SuiteFactoryEntry> suiteFactories_;
};

// Macro for registering tests with the factory
#define REGISTER_TEST(name, testClass) \
    namespace { \
        struct testClass##Registrar { \
            testClass##Registrar() { \
                TestFactory::getInstance().registerTest( #name, []() { return std::make_unique<testClass>(); }, \
                    TestMetadata(#name, "Auto-registered test", "auto", TestSeverity::Medium)); \
            } \
        }; \
        static testClass##Registrar testClass##instance; \
    }

// Macro for registering test suites
#define REGISTER_SUITE(name, suiteClass) \
    namespace { \
        struct suiteClass##Registrar { \
            suiteClass##Registrar() { \
                TestFactory::getInstance().registerSuite( #name, []() { return std::make_unique<suiteClass>(); }, \
                    TestMetadata(#name, "Auto-registered suite", "auto", TestSeverity::Medium)); \
            } \
        }; \
        static suiteClass##Registrar suiteClass##instance; \
    }

} // namespace test
} // namespace nlm