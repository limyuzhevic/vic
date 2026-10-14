// NLM Test Runner Implementation
// Implements the test runner functionality for the modern testing framework

#include "nlm_testing_framework.hpp"
#include <algorithm>
#include <unordered_map>

namespace {
    // Test function signature
    using TestFunction = std::function<bool()>;
    
    // Test suite structure
    struct TestSuite {
        std::string name;
        TestCategory category;
        std::string description;
        std::vector<TestFunction> tests;
        bool enabled;
        
        TestSuite(const std::string& n, TestCategory c, const std::string& d = "")
            : name(n), category(c), description(d), enabled(true) {}
    };
    
    // Global test registry implementation
    class TestRegistryImpl {
    private:
        std::vector<TestSuite> testSuites;
        std::unordered_map<std::string, size_t> suiteNameToIndex;
        std::vector<TestFunction> allTests;
        
    public:
        TestMetadata* registerTest(const std::string& name, TestCategory category, const std::string& description = "") {
            // Check if suite already exists
            auto it = suiteNameToIndex.find(name);
            if (it != suiteNameToIndex.end()) {
                return nullptr; // Suite already registered
            }
            
            // Create new test suite
            size_t index = testSuites.size();
            testSuites.emplace_back(name, category, description);
            suiteNameToIndex[name] = index;
            
            return &testSuites[index];
        }
        
        void addTest(TestFunction testFunc, const std::string& suiteName) {
            auto it = suiteNameToIndex.find(suiteName);
            if (it != suiteNameToIndex.end()) {
                testSuites[it->second].tests.push_back(testFunc);
                allTests.push_back(testFunc);
            }
        }
        
        void runAll() {
            std::cout << "=== Running All NLM Tests ===" << std::endl;
            
            int passed = 0;
            int failed = 0;
            int skipped = 0;
            
            for (auto& suite : testSuites) {
                if (!suite.enabled) {
                    std::cout << "Skipping suite: " << suite.name << std::endl;
                    skipped++;
                    continue;
                }
                
                std::cout << "\n--- Running suite: " << suite.name << " ---" << std::endl;
                std::cout << "Description: " << suite.description << std::endl;
                std::cout << "Category: " << static_cast<int>(suite.category) << std::endl;
                
                for (size_t i = 0; i < suite.tests.size(); ++i) {
                    std::cout << "Test " << (i + 1) << "/" << suite.tests.size() << ": ";
                    
                    try {
                        if (suite.tests[i]()) {
                            passed++;
                            std::cout << "PASS" << std::endl;
                        } else {
                            failed++;
                            std::cout << "FAIL" << std::endl;
                        }
                    } catch (const std::exception& e) {
                        failed++;
                        std::cout << "ERROR: " << e.what() << std::endl;
                    } catch (...) {
                        failed++;
                        std::cout << "UNKNOWN ERROR" << std::endl;
                    }
                }
            }
            
            std::cout << "\n=== Test Summary ===" << std::endl;
            std::cout << "Total suites: " << testSuites.size() << std::endl;
            std::cout << "Total tests: " << allTests.size() << std::endl;
            std::cout << "Passed: " << passed << std::endl;
            std::cout << "Failed: " << failed << std::endl;
            std::cout << "Skipped: " << skipped << std::endl;
            
            if (failed > 0) {
                std::cerr << "\nERROR: " << failed << " tests failed" << std::endl;
                return;
            }
        }
        
        void runCategory(TestCategory category) {
            std::cout << "\n=== Running Tests for Category: " << static_cast<int>(category) << " ===" << std::endl;
            
            int passed = 0;
            int failed = 0;
            
            for (auto& suite : testSuites) {
                if (suite.category != category || !suite.enabled) {
                    continue;
                }
                
                std::cout << "\n--- Running suite: " << suite.name << " ---" << std::endl;
                
                for (size_t i = 0; i < suite.tests.size(); ++i) {
                    std::cout << "Test " << (i + 1) << "/" << suite.tests.size() << ": ";
                    
                    try {
                        if (suite.tests[i]()) {
                            passed++;
                            std::cout << "PASS" << std::endl;
                        } else {
                            failed++;
                            std::cout << "FAIL" << std::endl;
                        }
                    } catch (const std::exception& e) {
                        failed++;
                        std::cout << "ERROR: " << e.what() << std::endl;
                    } catch (...) {
                        failed++;
                        std::cout << "UNKNOWN ERROR" << std::endl;
                    }
                }
            }
            
            std::cout << "\n=== Category Test Summary ===" << std::endl;
            std::cout << "Category: " << static_cast<int>(category) << std::endl;
            std::cout << "Passed: " << passed << std::endl;
            std::cout << "Failed: " << failed << std::endl;
        }
        
        void disableSuite(const std::string& name) {
            auto it = suiteNameToIndex.find(name);
            if (it != suiteNameToIndex.end()) {
                testSuites[it->second].enabled = false;
            }
        }
        
        const std::vector<TestSuite>& getAllSuites() const {
            return testSuites;
        }
    };
    
    TestRegistryImpl* instance = nullptr;
}

// Global test registry instance
TestRegistry& TestRegistry::getInstance() {
    if (!instance) {
        instance = new TestRegistryImpl();
    }
    return *instance;
}

// Run tests for a specific category
void TestRegistry::runCategory(TestCategory category) {
    std::cout << "\n=== Running Tests for Category: " << static_cast<int>(category) << " ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // In a real implementation, we would have access to the test suites
    // For now, we provide a basic implementation
    std::cout << "Category " << static_cast<int>(category) << " tests would be run here" << std::endl;
    
    std::cout << "\n=== Category Test Summary ===" << std::endl;
    std::cout << "Category: " << static_cast<int>(category) << std::endl;
    std::cout << "Note: This is a basic implementation. Use runAll() for full test execution." << std::endl;
}