#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "core/Config/Config.hpp"
#include "LearningExperiment.hpp"

namespace nlm {

class Brain;
class ConfigurationManager;

class TestRunner {
public:
    TestRunner(std::shared_ptr<ConfigurationManager> config);
    ~TestRunner() = default;
    
    // Run all configured tests
    int runAllTests(std::shared_ptr<Brain> brain);
    
    // Individual test methods
    bool runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain);
    bool runStdpVerification(std::shared_ptr<Brain> brain);
    
    // Get test statistics
    std::string getTestStatistics() const;
    
    // Enable/disable specific tests
    void setTestEnabled(const std::string& testName, bool enabled);
    
    // Get enabled tests
    std::vector<std::string> getEnabledTests() const;
    
private:
    std::shared_ptr<ConfigurationManager> config_;
    std::unordered_map<std::string, bool> testEnabled_;
    int testsRun_ = 0;
    int testsPassed_ = 0;
    int testsFailed_ = 0;
    
    // Test helper methods
    bool waitForSpikes(std::shared_ptr<Brain> brain, size_t expectedSpikes, size_t timeoutSteps = 1000);
    bool checkConvergence(std::vector<float>& weights, float tolerance = 0.01f);
};

} // namespace nlm