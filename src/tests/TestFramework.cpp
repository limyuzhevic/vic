#include "TestFramework.hpp"
#include <iostream>

namespace nlm {

namespace tests {

// TestFramework implementation
void TestFramework::logTestResult(const TestResult& result) {
    std::cout << std::left << std::setw(40) << result.name << " ";
    
    switch (result.status) {
        case TestResult::Status::PASSED:
            std::cout << "[PASS]";
            break;
        case TestResult::Status::FAILED:
            std::cout << "[FAIL]";
            break;
        case TestResult::Status::SKIPPED:
            std::cout << "[SKIP]";
            break;
        case TestResult::Status::ERROR:
            std::cout << "[ERROR]";
            break;
    }
    
    if (!result.message.empty()) {
        std::cout << " - " << result.message;
    }
    
    if (result.duration.count() > 0) {
        std::cout << " (" << result.duration.count() << "ms)";
    }
    
    std::cout << std::endl;
}

// Test helper implementations
TestResult TestBase::executeWithTimeout(Func&& func, const std::string& operation,
                                        double timeoutSeconds) {
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        auto result = func();
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (duration.count() > timeoutSeconds * 1000) {
            return TestResult(operation, TestResult::Status::FAILED,
                             "Operation timed out after " + std::to_string(duration.count()) + "ms");
        }
        
        return TestResult(operation, TestResult::Status::PASSED, "", duration);
        
    } catch (const std::exception& e) {
        return TestResult(operation, TestResult::Status::ERROR, 
                         std::string("Exception: ") + e.what());
    }
}

} // namespace tests

} // namespace nlm
