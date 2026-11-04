// test_framework.cpp - Implementation of the testing framework

#include "test_framework.hpp"
#include <iostream>

using namespace nlm;

// TestSuite implementation
void TestSuite::saveTestResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        logError("Failed to open test results file: " + filename);
        return;
    }
    
    file << "{\n";
    file << "  \"testSuite\": \"" << name_ << "\",\n";
    file << "  \"description\": \"" << description_ << "\",\n";
    
    int totalTests = tests_.size();
    int passedTests = 0;
    
    for (size_t i = 0; i < tests_.size(); ++i) {
        auto& test = tests_[i];
        bool testPassed = true;
        
        for (auto& result : test->getResults()) {
            if (!result.passed) {
                testPassed = false;
                break;
            }
        }
        
        if (testPassed) passedTests++;
        
        file << "  \"tests\": [\n";
        for (size_t j = 0; j < test->getResults().size(); ++j) {
            auto& result = test->getResults()[j];
            file << "    {\n";
            file << "      \"name\": \"" << result.name << "\",\n";
            file << "      \"description\": \"" << result.description << "\",\n";
            file << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
            file << "      \"message\": \"" << result.message << "\",\n";
            file << "      \"executionTime\": " << std::fixed << std::setprecision(6) << result.executionTime << ",\n";
            file << "      \"timestamp\": \"" << std::chrono::system_clock::to_time_t(result.timestamp) << "\"\n";
            
            if (j < test->getResults().size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        file << "  ]\n";
        
        if (i < tests_.size() - 1) {
            file << "  },\n";
        } else {
            file << "  }\n";
        }
    }
    
    file << "}\n";
    file.close();
    
    logInfo("Test results saved to: " + filename);
}
