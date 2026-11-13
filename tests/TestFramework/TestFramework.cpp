#include "TestFramework.hpp"

namespace nlm {
namespace test {

TestFramework::TestFramework() {
    std::cout << "=== NLM Test Framework ===" << std::endl;
}

void TestFramework::addCategory(std::shared_ptr<TestCategory> category) {
    categories.push_back(category);
}

void TestFramework::runAllTests() {
    totalTests = 0;
    passedTests = 0;
    failedTests = 0;
    
    for (const auto& category : categories) {
        std::cout << std::endl << "Running " << category->getName() << " tests..." << std::endl;
        try {
            category->runAll();
            auto results = category->getResults();
            
            for (const auto& result : results) {
                totalTests++;
                if (result.passed) {
                    passedTests++;
                    std::cout << "  ✓ " << result.name << " (" << result.category << ")" << std::endl;
                } else {
                    failedTests++;
                    std::cout << "  ✗ " << result.name << " (" << result.category << ") - " 
                             << result.message << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "  ✗ " << category->getName() << " tests crashed: " << e.what() << std::endl;
            failedTests++;
        }
    }
    
    printSummary();
}

void TestFramework::printSummary() const {
    std::cout << std::endl << "=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << totalTests << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    
    if (failedTests == 0) {
        std::cout << std::endl << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl << "✗ " << failedTests << " tests failed." << std::endl;
        return 1;
    }
}

int TestFramework::getFailureCount() const {
    return failedTests;
}

} // namespace test
} // namespace nlm
