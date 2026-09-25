// test_config.hpp - Test configuration system for Phase 6 integration tests
// Author: Kilo
// Purpose: Centralized configuration management for all Phase 6 tests

#pragma once

#include "brain_test_common.hpp"
#include <map>
#include <string>

namespace nlm {
namespace test {

/**
 * Test scenario configurations
 */
class TestConfigManager {
public:
    TestConfigManager() {
        initializeDefaultConfigs();
    }
    
    /**
     * Get test configuration for a specific scenario
     */
    const Phase6TestConfig& getConfig(const std::string& scenarioName) const {
        auto it = scenarioConfigs.find(scenarioName);
        if (it != scenarioConfigs.end()) {
            return it->second;
        }
        // Return default config if scenario not found
        static Phase6TestConfig defaultConfig;
        return defaultConfig;
    }
    
    /**
     * Register a custom test configuration
     */
    void registerScenario(const std::string& name, const Phase6TestConfig& config) {
        scenarioConfigs[name] = config;
    }
    
    /**
     * Get all available scenario names
     */
    std::vector<std::string> getScenarioNames() const {
        std::vector<std::string> names;
        for (const auto& pair : scenarioConfigs) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * Print all available scenarios
     */
    void printAvailableScenarios() const {
        std::cout << "Available test scenarios:" << std::endl;
        for (const auto& pair : scenarioConfigs) {
            std::cout << "  " << pair.first << std::endl;
        }
    }
    
private:
    void initializeDefaultConfigs() {
        // Basic integration test
        Phase6TestConfig basicConfig;
        basicConfig.maxSteps = 1000;
        basicConfig.neuronCount = 500;
        basicConfig.regionCount = 1;
        basicConfig.connectionProbability = 0.1f;
        basicConfig.enableCheckpointing = false;
        basicConfig.enableReplay = false;
        basicConfig.enableDevelopment = false;
        scenarioConfigs["basic_integration"] = basicConfig;
        
        // Memory-intensive test
        Phase6TestConfig memoryConfig;
        memoryConfig.maxSteps = 2000;
        memoryConfig.neuronCount = 1000;
        memoryConfig.regionCount = 2;
        memoryConfig.connectionProbability = 0.15f;
        memoryConfig.enableCheckpointing = false;
        memoryConfig.enableReplay = true;
        memoryConfig.enableDevelopment = true;
        scenarioConfigs["memory_intensive"] = memoryConfig;
        
        // Neuromodulation-heavy test
        Phase6TestConfig neuromodConfig;
        neuromodConfig.maxSteps = 1500;
        neuromodConfig.neuronCount = 750;
        neuromodConfig.regionCount = 1;
        neuromodConfig.connectionProbability = 0.12f;
        neuromodConfig.enableCheckpointing = false;
        neuromodConfig.enableReplay = false;
        neuromodConfig.enableDevelopment = true;
        scenarioConfigs["neuromodulation_heavy"] = neuromodConfig;
        
        // Cognitive-heavy test
        Phase6TestConfig cognitionConfig;
        cognitionConfig.maxSteps = 2500;
        cognitionConfig.neuronCount = 1200;
        cognitionConfig.regionCount = 2;
        cognitionConfig.connectionProbability = 0.18f;
        cognitionConfig.enableCheckpointing = false;
        cognitionConfig.enableReplay = true;
        cognitionConfig.enableDevelopment = true;
        scenarioConfigs["cognition_heavy"] = cognitionConfig;
        
        // Stress test
        Phase6TestConfig stressConfig;
        stressConfig.maxSteps = 5000;
        stressConfig.neuronCount = 2000;
        stressConfig.regionCount = 3;
        stressConfig.connectionProbability = 0.2f;
        stressConfig.enableCheckpointing = true;
        stressConfig.enableReplay = true;
        stressConfig.enableDevelopment = true;
        scenarioConfigs["stress_test"] = stressConfig;
        
        // Persistence test
        Phase6TestConfig persistenceConfig;
        persistenceConfig.maxSteps = 1000;
        persistenceConfig.neuronCount = 300;
        persistenceConfig.regionCount = 1;
        persistenceConfig.connectionProbability = 0.08f;
        persistenceConfig.enableCheckpointing = true;
        persistenceConfig.enableReplay = false;
        persistenceConfig.enableDevelopment = false;
        persistenceConfig.checkpointPath = "/tmp/nlm_persistence_test.bin";
        scenarioConfigs["persistence_test"] = persistenceConfig;
    }
    
    std::map<std::string, Phase6TestConfig> scenarioConfigs;
};

} // namespace test
} // namespace nlm