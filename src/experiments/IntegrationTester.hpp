#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "ExperimentConstants.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * Integration testing and system verification
 * Handles verification of brain system integration
 */
class IntegrationTester {
public:
    IntegrationTester() = default;
    ~IntegrationTester() = default;
    
    /**
     * Test all brain systems integration
     */
    bool testAllSystems(std::shared_ptr<Brain> brain);
    
    /**
     * Test memory systems integration
     */
    bool testMemoryIntegration(std::shared_ptr<Brain> brain);
    
    /**
     * Test neuromodulation integration
     */
    bool testNeuromodulationIntegration(std::shared_ptr<Brain> brain);
    
    /**
     * Test prediction system integration
     */
    bool testPredictionIntegration(std::shared_ptr<Brain> brain);
    
    /**
     * Test development system integration
     */
    bool testDevelopmentIntegration(std::shared_ptr<Brain> brain);
    
    /**
     * Run basic connectivity tests
     */
    bool testBasicConnectivity(std::shared_ptr<Brain> brain);
    
    /**
     * Get integration test results
     */
    struct IntegrationTestResults {
        bool memoryWorkingMemoryIntegrated = false;
        bool memoryEpisodicMemoryIntegrated = false;
        bool neuromodulationIntegrated = false;
        bool predictionIntegrated = false;
        bool developmentIntegrated = false;
        bool basicConnectivity = false;
        std::vector<std::string> errors;
        
        bool isAllSystemsIntegrated() const {
            return memoryWorkingMemoryIntegrated && memoryEpisodicMemoryIntegrated &&
                   neuromodulationIntegrated && predictionIntegrated &&
                   developmentIntegrated && basicConnectivity;
        }
    };
    
    IntegrationTestResults getTestResults() const { return testResults_; }
    
private:
    IntegrationTestResults testResults_;
};

} // namespace nlm
