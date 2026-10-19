#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class EnhancedLogger;

class SimulationRunner {
public:
    SimulationRunner(std::shared_ptr<Config> config, EnhancedLogger& logger);
    ~SimulationRunner() = default;
    
    // Run a specific test/experiment
    bool runTest(const std::string& testName, const std::map<std::string, std::string>& params = {});
    
    // Run all enabled tests
    bool runAllTests(const std::map<std::string, std::string>& globalParams = {});
    
    // Get available tests
    static std::vector<std::string> getAvailableTests();
    
    // Get test parameters
    static std::map<std::string, std::vector<std::string>> getTestParameters(const std::string& testName);
    
    // Get test description
    static std::string getTestDescription(const std::string& testName);
    
    // Save results
    bool saveResults(const std::string& filepath) const;
    bool exportResults(const std::string& filepath, const std::string& format = "json") const;
    
private:
    std::shared_ptr<Config> config_;
    EnhancedLogger& logger_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    
    void initializeBrain();
    bool measureExecutionTime(std::function<bool()> func);
    
    bool runBasicConnectivityTest();
    bool runPlasticityExperiment();
    bool runStdpVerificationTest();
    bool runDevelopmentTest();
    bool runNeuromodulationTest();
    bool runMemoryTest();
};

} // namespace nlm