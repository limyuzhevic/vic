// Test file for error handling improvements
// Demonstrates all aspects of the new error handling system

#include "ErrorHandling.hpp"
#include <iostream>
#include <stdexcept>

void demonstrateErrorHandling() {
    std::cout << "=== NLM Error Handling Demonstration ===\n" << std::endl;
    
    // Initialize error handling
    nlm::initializeErrorHandling();
    
    std::cout << "1. Testing basic exception handling...\n";
    try {
        NLM_THROW_ERROR(nlm::ErrorCategory::CONFIGURATION,
                       nlm::ErrorCode::CONFIG_INVALID_VALUE,
                       "Test configuration error with full context");
    } catch (const nlm::NLMException& e) {
        std::cout << "   Caught error: " << e.what() << "\n";
        std::cout << "   Category: " << nlm::errorCategoryToString(e.getCategory()) << "\n";
        std::cout << "   Severity: " << static_cast<int>(e.getSeverity()) << "\n";
    }
    
    std::cout << "\n2. Testing neural network error handling...\n";
    try {
        auto error = nlm::Errors::neuralNetwork(
            nlm::ErrorCode::NN_INITIALIZATION_FAILED,
            "Failed to initialize neural network layer: size_mismatch"
        );
        nlm::getGlobalErrorManager().recordError(error);
        std::cout << "   Recorded neural network error: " << error->what() << "\n";
    } catch (...) {
        std::cout << "   Error recorded successfully\n";
    }
    
    std::cout << "\n3. Testing memory error handling...\n";
    try {
        auto error = nlm::Errors::memoryAllocation(1024 * 1024 * 1024, "Out of memory"); // 1GB allocation
        nlm::getGlobalErrorManager().recordError(error);
        std::cout << "   Recorded memory error: " << error->what() << "\n";
    } catch (...) {
        std::cout << "   Error recorded successfully\n";
    }
    
    std::cout << "\n4. Testing performance error handling...\n";
    try {
        auto error = nlm::Errors::performanceTimeout(
            "brain_simulation",
            std::chrono::milliseconds(5000),
            std::chrono::milliseconds(1000)
        );
        nlm::getGlobalErrorManager().recordError(error);
        std::cout << "   Recorded performance error: " << error->what() << "\n";
    } catch (...) {
        std::cout << "   Error recorded successfully\n";
    }
    
    std::cout << "\n5. Testing input validation...\n";
    nlm::ErrorPrevention::getGlobalPreventionSystem().registerPreCondition("test_precondition",
        []() { return false; /* Simulate precondition failure */ });
    
    nlm::ErrorPrevention::getGlobalPreventionSystem().registerInputValidator("test_input",
        []() { return false; /* Simulate input validation failure */ });
    
    std::vector<std::string> failures = nlm::ErrorPrevention::getGlobalPreventionSystem().runAllChecks();
    if (!failures.empty()) {
        std::cout << "   Validation failures detected:\n";
        for (const auto& failure : failures) {
            std::cout << "     - " << failure << "\n";
        }
    } else {
        std::cout << "   No validation failures\n";
    }
    
    std::cout << "\n6. Testing error reporting...\n";
    // Record some errors
    auto configError = nlm::Errors::configInvalid("learning_rate", "positive_float", "-1.0");
    nlm::getGlobalErrorManager().recordError(configError);
    
    nlm::ErrorReporting::getGlobalReporter().recordError(configError);
    
    // Generate report
    std::string report = nlm::ErrorReporting::getGlobalReporter().generateReport();
    std::cout << "   Error report generated (showing first 500 chars):\n";
    if (report.length() > 500) {
        std::cout << "   " << report.substr(0, 500) << "...\n";
    } else {
        std::cout << "   " << report << "\n";
    }
    
    std::cout << "\n7. Testing thread-safe error handling...\n";
    // Simulate multiple threads recording errors
    nlm::ThreadSafeErrorHandling::getGlobalThreadSafeManager().recordThreadError(
        nlm::Errors::systemError("thread_operation", "Thread-specific error")
    );
    
    std::cout << "   Thread-safe error recorded\n";
    
    std::cout << "\n8. Testing error statistics...\n";
    nlm::ErrorManager::ErrorStats stats = nlm::getGlobalErrorManager().getStats();
    std::cout << "   Error statistics:\n";
    std::cout << "     Total errors: " << stats.totalErrors << "\n";
    std::cout << "     Recoveries: " << stats.recoveryCount << "\n";
    std::cout << "     Last error time: " << std::chrono::system_clock::to_time_t(stats.lastErrorTime) << "\n";
    
    std::cout << "\n=== Error Handling Demonstration Complete ===\n" << std::endl;
}

int main() {
    demonstrateErrorHandling();
    return 0;
}
