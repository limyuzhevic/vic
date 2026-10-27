// NLM Testing Framework Implementation
// Implementation of the enhanced Agent testing framework
// Provides comprehensive testing capabilities for NLM components

#include "TestingFramework.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace nlm {
namespace testing {

// Helper functions for JSON serialization
namespace json {
    std::string escapeString(const std::string& str) {
        std::string escaped;
        for (char c : str) {
            switch (c) {
                case '\"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default:
                    if (c < ' ' || c > '~') {
                        std::stringstream ss;
                        ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)(unsigned char)c;
                        escaped += ss.str();
                    } else {
                        escaped += c;
                    }
            }
        }
        return escaped;
    }
}

// AgentTest implementation
void AgentTest::addResult(const TestResult& result) {
    results_.push_back(result);
}

// TestingFramework implementation
AgentTestSuite::AgentTestSuite(const TestConfig& config) : config_(config) {
    // Set random seed if provided
    if (config_.randomSeed > 0) {
        std::srand(config_.randomSeed);
    }
}

void AgentTestSuite::addTest(std::shared_ptr<AgentTest> test) {
    tests_.push_back(test);
}

void AgentTestSuite::addDefaultTests() {
    tests_.push_back(std::make_shared<SensoryProcessingTest>());
    tests_.push_back(std::make_shared<MotorDecodingTest>());
    tests_.push_back(std::make_shared<NeuromodulationTest>());
    tests_.push_back(std::make_shared<DevelopmentProgressionTest>());
    tests_.push_back(std::make_shared<CuriosityDrivenExplorationTest>());
    tests_.push_back(std::make_shared<RewardLearningTest>());
}

void AgentTestSuite::runAll() {
    std::cout << "=== NLM Agent Testing Framework ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  - Verbose: " << (config_.verbose ? "Yes" : "No") << std::endl;
    std::cout << "  - Continue on failure: " << (config_.continueOnFailure ? "Yes" : "No") << std::endl;
    std::cout << "  - Timeout: " << config_.timeout << " seconds" << std::endl;
    std::cout << "  - Random seed: " << config_.randomSeed << std::endl;
    std::cout << std::endl;
    
    int testsRun = 0;
    int testsPassed = 0;
    
    for (const auto& test : tests_) {
        if (!test->shouldRun()) continue;
        
        std::cout << "Running test: " << test->getTestName() << "..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        TestResult result = test->run();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        result.executionTime = elapsed;
        
        results_.push_back(result);
        
        if (result.passed) {
            testsPassed++;
            std::cout << "  PASSED (" << std::fixed << std::setprecision(3) << elapsed << "s)" << std::endl;
        } else {
            std::cout << "  FAILED (" << std::fixed << std::setprecision(3) << elapsed << "s)" << std::endl;
            std::cout << "  Message: " << result.message << std::endl;
            
            if (!config_.continueOnFailure) {
                std::cout << "Stopping test suite (continue-on-failure disabled)." << std::endl;
                break;
            }
        }
        
        testsRun++;
    }
    
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Tests run: " << testsRun << std::endl;
    std::cout << "Tests passed: " << testsPassed << std::endl;
    std::cout << "Tests failed: " << (testsRun - testsPassed) << std::endl;
    std::cout << "Success rate: " << (testsRun > 0 ? (testsPassed * 100.0 / testsRun) : 0.0) << "%" << std::endl;
    
    // Save results if configured
    if (config_.outputFile != "") {
        saveResults();
    }
}

void AgentTestSuite::saveResults() {
    // Simple JSON output for now
    std::ofstream file(config_.outputFile);
    if (!file.is_open()) {
        std::cout << "Warning: Could not open output file: " << config_.outputFile << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"test_suite\": \"NLM Agent Testing Framework\",\n";
    file << "  \"configuration\": {\n";
    file << "    \"verbose\": " << (config_.verbose ? "true" : "false") << ",\n";
    file << "    \"continue_on_failure\": " << (config_.continueOnFailure ? "true" : "false") << ",\n";
    file << "    \"timeout\": " << config_.timeout << ",\n";
    file << "    \"random_seed\": " << config_.randomSeed << ",\n";
    file << "    \"agent_params\": {\n";
    file << "      \"num_steps\": " << config_.agentParams.numSteps << ",\n";
    file << "      \"plasticity_strength\": " << config_.agentParams.plasticityStrength << ",\n";
    file << "      \"novelty_threshold\": " << config_.agentParams.noveltyThreshold << ",\n";
    file << "      \"enable_curiosity\": " << (config_.agentParams.enableCuriosity ? "true" : "false") << ",\n";
    file << "      \"enable_development\": " << (config_.agentParams.enableDevelopment ? "true" : "false") << ",\n";
    file << "      \"neuron_count\": " << config_.agentParams.neuronCount << ",\n";
    file << "      \"connection_probability\": " << config_.agentParams.connectionProbability << "\n";
    file << "    }\n";
    file << "  },\n";
    file << "  \"results\": [\n";
    
    for (size_t i = 0; i < results_.size(); ++i) {
        const auto& result = results_[i];
        file << "    {\n";
        file << "      \"test_name\": \"" << result.testName << "\",\n";
        file << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
        file << "      \"execution_time\": " << result.executionTime << ",\n";
        file << "      \"message\": \"" << json::escapeString(result.message) << "\"";
        if (!result.metrics.empty()) {
            file << ",\n      \"metrics\": {\n";
            bool firstMetric = true;
            for (const auto& metric : result.metrics) {
                if (!firstMetric) file << ",\n";
                file << "        \"" << metric.first << "\": \"" << json::escapeString(metric.second) << "\"";
                firstMetric = false;
            }
            file << "\n      }\n";
        } else {
            file << "\n";
        }
        
        if (i < results_.size() - 1) {
            file << "    },\n";
        } else {
            file << "    }\n";
        }
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    std::cout << "Results saved to: " << config_.outputFile << std::endl;
}

} // namespace testing
} // namespace nlm
