#pragma once

#include "brain/Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

class TestRunner {
public:
    TestRunner();
    ~TestRunner() = default();
    
    // Disable copying
    TestRunner(const TestRunner&) = delete;
    TestRunner& operator=(const TestRunner&) = delete;
    
    // Run all tests
    bool runAllTests(std::shared_ptr<Brain> brain);
    
    // Individual test methods
    bool runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain);
    bool runStdpVerification(std::shared_ptr<Brain> brain);
    
    // Get test results
    std::string getTestResults() const { return testResults; }
    
private:
    std::string testResults;
    
    // Helper methods
    void logTestHeader(const std::string& testName);
    void logTestResult(bool passed, const std::string& testName, const std::string& message = "");
    void logTestSummary() const;
};

} // namespace nlm