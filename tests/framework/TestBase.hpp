"""
Base test class for the NLM test framework
"""

#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <functional>
#include "test_framework/TestResult.hpp"

namespace nlm {

class TestBase {
public:
    enum class Category {
        Unit,           // Individual component tests
        Integration,    // Component interaction tests
        Performance,    // Performance benchmarks
        Stress,         // Stress/stability tests
        Regression,     // Regression detection
        System          // End-to-end system tests
    };

    enum class Priority {
        Low = 0,
        Normal = 1,
        High = 2,
        Critical = 3
    };

    struct TestMetadata {
        std::string name;
        std::string description;
        Category category;
        Priority priority;
        std::vector<std::string> tags;
        std::chrono::milliseconds estimated_duration;
        bool enabled = true;
    };

    TestBase(const TestMetadata& metadata);
    virtual ~TestBase() = default;

    // Run the test
    TestResult run();

    // Test lifecycle
    virtual void setup() {}
    virtual void teardown() {}
    virtual void setupFixture() {}
    virtual void teardownFixture() {}

    // Test execution
    virtual void execute() = 0;

    // Metadata accessors
    const std::string& getName() const { return metadata.name; }
    const std::string& getDescription() const { return metadata.description; }
    Category getCategory() const { return metadata.category; }
    Priority getPriority() const { return metadata.priority; }
    const std::vector<std::string>& getTags() const { return metadata.tags; }
    bool isEnabled() const { return metadata.enabled; }

    // Comparison and ordering
    bool operator<(const TestBase& other) const {
        return metadata.priority > other.metadata.priority; // Higher priority first
    }

    // Time measurement
    template<typename F>
    std::chrono::milliseconds timeExecution(F&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    }

protected:
    TestMetadata metadata;
    TestResult result;
};

// Test fixture for setup/teardown
class TestFixture {
public:
    virtual ~TestFixture() = default;
    virtual void setup() {}
    virtual void teardown() {}
};

// Helper macros for test registration
#define REGISTER_TEST(test_class, category, priority, tags, description) \
    namespace { \
        class test_class##_registrar { \
        public: \
            test_class##_registrar() { \
                nlm::TestRegistry::getInstance().registerTest( \
                    std::make_unique<test_class>(), \
                    #test_class, \
                    category, \
                    priority, \
                    tags, \
                    description \
                ); \
            } \
        }; \
        static test_class##_registrar test_class##_instance; \
    } \

#define NLM_TEST_CLASS(test_class) \
    class test_class : public ::nlm::TestBase \

#define NLM_TEST_FIXTURE(test_fixture) \
    class test_fixture : public ::nlm::TestFixture

#define TEST_CATEGORY(category, priority, tags, description) \
    TestBase::TestMetadata metadata = { #category "_" #priority, description, TestBase::Category::category, TestBase::Priority::priority, tags }

#define TEST_METADATA(name, category, priority, tags, description) \
    TestBase::TestMetadata metadata = { name, description, TestBase::Category::category, TestBase::Priority::priority, tags }

} // namespace nlm

// Test Registry for test discovery
namespace nlm {
    class TestRegistry {
    public:
        static TestRegistry& getInstance() {
            static TestRegistry instance;
            return instance;
        }

        void registerTest(std::unique_ptr<TestBase> test,
                         const std::string& name,
                         TestBase::Category category,
                         TestBase::Priority priority,
                         const std::vector<std::string>& tags,
                         const std::string& description);

        const std::vector<std::unique_ptr<TestBase>>& getAllTests() const { return tests; }
        const std::vector<std::unique_ptr<TestBase>>& getTestsByCategory(TestBase::Category category) const;
        const std::vector<std::unique_ptr<TestBase>>& getTestsByTag(const std::string& tag) const;

        void filterTests(const std::function<bool(const TestBase&)>& predicate);
        void sortTests();

        std::vector<std::string> getAllTags() const;
        std::vector<std::string> getAllCategories() const;

    private:
        TestRegistry() = default;
        std::vector<std::unique_ptr<TestBase>> tests;
    };
}