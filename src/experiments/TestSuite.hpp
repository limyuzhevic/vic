// TestSuite class for NLM Phase 2 Real Neural Computation
// Provides organized test execution with proper error handling and reporting
// Replaces the monolithic test functions in main.cpp

#ifndef NLM_TEST_SUITE_HPP
#define NLM_TEST_SUITE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include "brain/Brain.hpp"
#include "experiments/LearningExperiment.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class TestSuite {
public:
    TestSuite() = default;
    ~TestSuite() = default;
    
    // Disable copying
    TestSuite(const TestSuite&) = delete;
    TestSuite& operator=(const TestSuite&) = delete;
    
    // Test execution methods
    bool runBasicConnectivityTest(std::shared_ptr<Brain> brain) const;
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain) const;
    bool runStdpVerification(std::shared_ptr<Brain> brain) const;
    bool runAllTests(std::shared_ptr<Brain> brain) const;
    
    // Helper methods for test execution
    static void printTestBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Test Suite - Organized Tests                              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    static void logTestHeader(const std::string& testName) {
        std::cout << "" << std::endl;
        std::cout << "=== " << testName << " ===" << std::endl;
        std::cout << "" << std::endl;
    }
    
    static void logTestResult(const std::string& testName, bool passed, const std::string& message = "") {
        if (passed) {
            std::cout << "✓ " << testName << " PASSED" << std::endl;
            if (!message.empty()) {
                std::cout << "  " << message << std::endl;
            }
        } else {
            std::cout << "✗ " << testName << " FAILED" << std::endl;
            if (!message.empty()) {
                std::cout << "  " << message << std::endl;
            }
        }
    }
    
    static void logProgress(const std::string& message) {
        std::cout << "  " << message << std::endl;
    }
    
private:
    // Helper methods for individual tests
    bool runBasicConnectivityTestImpl(std::shared_ptr<Brain> brain) const;
    bool runPlasticityExperimentImpl(std::shared_ptr<Brain> brain) const;
    bool runStdpVerificationImpl(std::shared_ptr<Brain> brain) const;
};

} // namespace nlm

#endif // NLM_TEST_SUITE_HPP