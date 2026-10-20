#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <memory>
#include <cassert>
#include <functional>
#include <thread>
#include <random>

namespace test_framework {

enum class TestCategory {
    Unit,
    Integration,
    Performance,
    Configuration,
    AgentBehavior,
    WorldSimulation,
    Stress,
    Fuzz,
    PropertyBased
};

enum class TestStatus {
    Pending,
    Running,
    Passed,
    Failed,
    Skipped,
    Error
};

struct TestResult {
    std::string name;
    TestCategory category;
    TestStatus status;
    std::string message;
    double executionTimeMs;
    uint64_t memoryUsage;
    std::string errorDetails;
    bool parameterized;
    std::vector<std::string> tags;
};

class TestBase {
protected:
    std::string name_;
    TestCategory category_;
    std::vector<std::string> tags_;
    bool enabled_ = true;
    
public:
    TestBase(const std::string& name, TestCategory category) 
        : name_(name), category_(category) {}
    
    virtual ~TestBase() = default;
    
    const std::string& getName() const { return name_; }
    TestCategory getCategory() const { return category_; }
    const std::vector<std::string>& getTags() const { return tags_; }
    bool isEnabled() const { return enabled_; }
    
    void addTag(const std::string& tag) { tags_.push_back(tag); }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    virtual void setup() {}
    virtual void teardown() {}
    
    virtual void run() = 0;
    
    template<typename TestClass>
    static void addParameterizedTest(
        const std::string& baseName,
        TestCategory category,
        const std::vector<typename TestClass::ParamType>& params,
        void (TestClass::*testMethod)(const typename TestClass::ParamType&),
        TestClass& instance)
    {
        for (size_t i = 0; i < params.size(); ++i) {
            auto paramTest = new ParameterizedTest<typename TestClass::ParamType>(
                baseName + "_param_" + std::to_string(i),
                category,
                params[i],
                testMethod,
                instance
            );
            instance.registerTest(paramTest);
        }
    }
    
    virtual void execute() {
        std::cout << "  Running test: " << name_ << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        setup();
        try {
            run();
            teardown();
            
            auto end = std::chrono::high_resolution_clock::now();
            executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
            status_ = TestStatus::Passed;
            std::cout << "  PASSED: " << name_ << " (" << executionTimeMs << "ms)" << std::endl;
        } catch (const std::exception& e) {
            teardown();
            auto end = std::chrono::high_resolution_clock::now();
            executionTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
            status_ = TestStatus::Failed;
            errorDetails_ = e.what();
            std::cout << "  FAILED: " << name_ << " - " << e.what() << std::endl;
            throw;
        }
    }
    
    virtual TestResult getResult() const {
        return result_;
    }
    
protected:
    void setResult(const TestResult& result) { result_ = result; }
    
private:
    TestResult result_;
    TestStatus status_ = TestStatus::Pending;
    double executionTimeMs = 0.0;
    uint64_t memoryUsage = 0;
    std::string errorDetails_;
};

template<typename T>
class ParameterizedTest : public TestBase {
private:
    T param_;
    std::function<void(const T&)> testMethod_;
    void* instance_;
    
public:
    ParameterizedTest(const std::string& name, TestCategory category,
                     const T& param,
                     std::function<void(const T&)> testMethod,
                     void* instance)
        : TestBase(name, category), param_(param), 
          testMethod_(testMethod), instance_(instance) {}
    
    void run() override {
        testMethod_(param_);
    }
};

class TestSuite {
private:
    std::string name_;
    std::vector<std::unique_ptr<TestBase>> tests_;
    std::map<std::string, std::vector<TestBase*>> categorizedTests_;
    bool verbose_ = false;
    
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    void addTest(std::unique_ptr<TestBase> test) {
        tests_.push_back(std::move(test));
        auto& testPtr = tests_.back();
        
        const auto& category = testPtr->getCategory();
        categorizedTests_[testCategoryToString(category)].push_back(testPtr.get());
    }
    
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    void runAll() {
        std::cout << "Running test suite: " << name_ << std::endl;
        std::cout << "==========================================" << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, error = 0;
        
        for (auto& test : tests_) {
            if (!test->isEnabled()) {
                if (verbose_) std::cout << "  Skipping: " << test->getName() << std::endl;
                continue;
            }
            
            try {
                test->execute();
                passed++;
            } catch (...) {
                failed++;
            }
        }
        
        std::cout << "==========================================" << std::endl;
        std::cout << "Test Suite Results:" << std::endl;
        std::cout << "  Passed: " << passed << std::endl;
        std::cout << "  Failed: " << failed << std::endl;
        std::cout << "  Total:  " << (passed + failed) << std::endl;
    }
    
    void runCategory(TestCategory category) {
        const std::string categoryStr = testCategoryToString(category);
        if (categorizedTests_.find(categoryStr) == categorizedTests_.end()) return;
        
        std::cout << "\nRunning " << categoryStr << " tests:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        for (auto* test : categorizedTests_[categoryStr]) {
            if (!test->isEnabled()) continue;
            try {
                test->execute();
            } catch (...) {}
        }
    }
    
    static std::string testCategoryToString(TestCategory category) {
        switch (category) {
            case TestCategory::Unit: return "Unit";
            case TestCategory::Integration: return "Integration";
            case TestCategory::Performance: return "Performance";
            case TestCategory::Configuration: return "Configuration";
            case TestCategory::AgentBehavior: return "AgentBehavior";
            case TestCategory::WorldSimulation: return "WorldSimulation";
            case TestCategory::Stress: return "Stress";
            case TestCategory::Fuzz: return "Fuzz";
            case TestCategory::PropertyBased: return "PropertyBased";
            default: return "Unknown";
        }
    }
    
    void runBenchmark(const std::string& benchmarkName, std::function<void()> testFunc, 
                     size_t iterations = 100) {
        std::cout << "\nRunning benchmark: " << benchmarkName << std::endl;
        
        std::vector<double> times;
        for (size_t i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            testFunc();
            auto end = std::chrono::high_resolution_clock::now();
            times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
        }
        
        double minTime = *std::min_element(times.begin(), times.end());
        double maxTime = *std::max_element(times.begin(), times.end());
        double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        
        std::cout << "  Min time:  " << minTime << "ms" << std::endl;
        std::cout << "  Max time:  " << maxTime << "ms" << std::endl;
        std::cout << "  Avg time:  " << avgTime << "ms" << std::endl;
        std::cout << "  Iterations: " << iterations << std::endl;
    }
};

class TestFramework {
private:
    std::vector<std::unique_ptr<TestSuite>> suites_;
    std::vector<TestResult> allResults_;
    bool verbose_ = false;
    
public:
    TestFramework() {}
    
    void addSuite(std::unique_ptr<TestSuite> suite) {
        suites_.push_back(std::move(suite));
    }
    
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    void runAll() {
        std::cout << "=== NLM Test Framework ===" << std::endl;
        
        for (auto& suite : suites_) {
            suite->runAll();
        }
        
        std::cout << "\n=== Test Framework Summary ===" << std::endl;
        std::cout << "Total suites: " << suites_.size() << std::endl;
    }
    
    void runCategories(const std::vector<TestCategory>& categories) {
        for (const auto& suite : suites_) {
            for (const auto& category : categories) {
                suite->runCategory(category);
            }
        }
    }
};

} // namespace test_framework
