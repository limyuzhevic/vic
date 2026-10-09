#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

// Forward declarations for test categories
namespace nlm_test {
    // Test result structure
    struct TestResult {
        std::string testName;
        std::string category;
        bool passed;
        std::string message;
        double executionTimeMs;
    };
    
    // Base test class
    class Test {
    public:
        virtual ~Test() = default;
        virtual std::string getName() const = 0;
        virtual std::string getCategory() const = 0;
        virtual void setup() {}  // Setup before test
        virtual void cleanup() {}  // Cleanup after test
        virtual void run() = 0;  // The actual test
        virtual bool shouldSkip() const { return false; }
        
        TestResult execute() {
            auto startTime = std::chrono::high_resolution_clock::now();
            bool passed = false;
            std::string message;
            
            try {
                if (shouldSkip()) {
                    message = "SKIPPED";
                    passed = true;  // Skip is not a failure
                } else {
                    setup();
                    run();
                    passed = true;
                    message = "PASSED";
                }
            } catch (const std::exception& e) {
                passed = false;
                message = "FAILED: " + std::string(e.what());
            } catch (...) {
                passed = false;
                message = "FAILED: Unknown exception";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            return {
                getName(),
                getCategory(),
                passed,
                message,
                static_cast<double>(duration.count())
            };
        }
    };
    
    // Test runner class
    class TestRunner {
    private:
        std::vector<std::unique_ptr<Test>> tests;
        
    public:
        void addTest(std::unique_ptr<Test> test) {
            tests.push_back(std::move(test));
        }
        
        std::vector<TestResult> runAllTests() {
            std::vector<TestResult> results;
            results.reserve(tests.size());
            
            for (auto& test : tests) {
                results.push_back(test->execute());
            }
            
            return results;
        }
        
        void printResults(const std::vector<TestResult>& results) {
            std::cout << "=== NLM INTEGRATED TEST SUITE ===" << std::endl;
            std::cout << "Testing Real Neural Computation (Phase 2)" << std::endl;
            std::cout << std::endl;
            
            // Group tests by category
            std::unordered_map<std::string, std::vector<const TestResult*>> categories;
            for (const auto& result : results) {
                categories[result.category].push_back(&result);
            }
            
            // Print results by category
            bool allPassed = true;
            for (const auto& pair : categories) {
                const std::string& category = pair.first;
                const std::vector<const TestResult*>& categoryTests = pair.second;
                
                std::cout << "=== " << category << " ===" << std::endl;
                
                for (const TestResult* result : categoryTests) {
                    std::cout << "  " << result->testName << ": ";
                    
                    if (result->passed) {
                        if (result->message.find("SKIPPED") != std::string::npos) {
                            std::cout << "SKIP";
                        } else {
                            std::cout << "PASS";
                        }
                        std::cout << " (" << std::fixed << std::setprecision(2) 
                                  << result->executionTimeMs << " ms)";
                    } else {
                        std::cout << "FAIL: " << result->message;
                        allPassed = false;
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
            
            // Summary
            std::cout << "=== TEST SUMMARY ===" << std::endl;
            int totalTests = results.size();
            int passedTests = 0;
            int failedTests = 0;
            int skippedTests = 0;
            double totalTime = 0.0;
            
            for (const TestResult& result : results) {
                if (result.passed) {
                    if (result.message.find("SKIPPED") != std::string::npos) {
                        skippedTests++;
                    } else {
                        passedTests++;
                    }
                } else {
                    failedTests++;
                }
                totalTime += result.executionTimeMs;
            }
            
            std::cout << "Total Tests: " << totalTests << std::endl;
            std::cout << "  Passed: " << passedTests << std::endl;
            std::cout << "  Failed: " << failedTests << std::endl;
            std::cout << "  Skipped: " << skippedTests << std::endl;
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Total Time: " << totalTime << " ms" << std::endl;
            
            if (allPassed) {
                std::cout << "=== ALL TESTS PASSED ===" << std::endl;
                return 0;
            } else {
                std::cout << "=== SOME TESTS FAILED ===" << std::endl;
                return 1;
            }
        }
    };
    
    // Base test implementation
    template<typename TestClass>
    class TestImplementation : public Test {
    protected:
        TestClass& getFixture() {
            if (!fixture_) {
                fixture_ = std::make_unique<TestClass>();
            }
            return *fixture_;
        }
        
    private:
        std::unique_ptr<TestClass> fixture_;
    };
    
    // Convenience macros for test creation
    #define NLM_TEST_CASE(Category, Name) \
        class Category##_##Name##_Test : public Test { \
        public: \
            std::string getName() const override { return #Name; } \
            std::string getCategory() const override { return Category; } \
            void run() override; \
        }; \
        void Category##_##Name##_Test::run()
    
    #define NLM_FIXTURE_TEST_CASE(Category, FixtureClass, Name) \
        class Category##_##FixtureClass##_##Name##_Test : public TestImplementation<FixtureClass> { \
        public: \
            std::string getName() const override { return #Name; } \
            std::string getCategory() const override { return Category; } \
            void setup() override { getFixture().setup(); } \
            void cleanup() override { getFixture().cleanup(); } \
            void run() override; \
        }; \
        void Category##_##FixtureClass##_##Name##_Test::run()
} // namespace nlm_test