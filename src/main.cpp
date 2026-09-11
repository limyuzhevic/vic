# Phase 6 Integration Validation Suite
// Main test driver for NLM Phase 6 integration
// Tests all improvements made to Phase 6 integration

#include "Phase6Validator.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

int main(int argc, char** argv) {
    // Set up validation environment
    std::cout << "NLM Phase 6 Integration Validation Suite" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
    
    // Print Phase 6 Final Audit reference
    std::cout << "Phase 6 Final Audit Status:" << std::endl;
    std::cout << "  - Score: 43/120 (35.8%)" << std::endl;
    std::cout << "  - Critical gaps identified in integration" << std::endl;
    std::cout << "  - Memory systems disconnected from neural processing" << std::endl;
    std::cout << "  - Prediction system never trained or used" << std::endl;
    std::cout << "  - Persistence systems as stubs instead of functional" << std::endl;
    std::cout << "  - Development only affected structural plasticity" << std::endl;
    std::cout << std::endl;
    
    // Create and run validation suite
    nlm::Phase6Validator validator;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run all validation tests
    validator.runAllTests();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Print summary report
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "VALIDATION SUMMARY" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Display individual test results
    validator.printResults();
    
    // Print validation statistics
    auto stats = validator.getValidationStats();
    
    std::cout << std::endl;
    std::cout << "VALIDATION STATISTICS:" << std::endl;
    std::cout << "  - Total tests executed: " << stats.totalTests << std::endl;
    std::cout << "  - Total execution time: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << "  - Tests passed: " << stats.passedTests << std::endl;
    std::cout << "  - Tests partially passed: " << stats.partialTests << std::endl;
    std::cout << "  - Tests failed: " << stats.failedTests << std::endl;
    std::cout << "  - Overall pass rate: " << (stats.passedTests * 100.0 / stats.totalTests) << "%" << std::endl;
    std::cout << std::endl;
    
    // Print assessment report
    validator.printAssessmentReport();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Phase 6 Integration Validation Complete" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    if (stats.failedTests == 0 && stats.partialTests == 0) {
        std::cout << "✓ ALL TESTS PASSED - Phase 6 integration complete!" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM Phase 6 integration has been successfully validated." << std::endl;
        std::cout << "All previously disconnected systems are now operational and working together" << std::endl;
        std::cout << "as a coherent artificial brain system, achieving the Phase 6 objectives." << std::endl;
        return 0;
    } else if (stats.failedTests == 0) {
        std::cout << "⚠ MOST TESTS PASSED - Minor issues identified but overall functional." << std::endl;
        return 1;
    } else {
        std::cout << "✗ TESTS FAILED - Critical issues remain in Phase 6 integration." << std::endl;
        return 2;
    }
}