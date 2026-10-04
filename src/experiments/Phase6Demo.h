#pragma once

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <iostream>

namespace nlm {

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    // Initialize demo with optional configuration
    bool initialize(int argc, char* argv[]);
    
    // Run the Phase 6 integration demo
    int run();
    
    // Run quick verification only
    bool verifyIntegrationOnly();
    
    // Test individual system integrations
    bool runMemoryIntegrationTest();
    bool runNeuromodulationIntegrationTest();
    bool runCheckpointingTest();
    bool runReplayTest();
    
    // Get demo results
    const Phase6IntegrationResult& getResults() const { return results_; }
    bool wasSuccessful() const { return success_; }
    
    // Configuration getters
    const std::string& getCheckpointPath() const { return checkpointPath_; }
    size_t getNeuronCount() const { return neuronCount_; }
    size_t getMaxSteps() const { return maxSteps_; }
    
protected:
    // Protected constructor - use createInstance()
    Phase6Demo(const Phase6Demo&) = delete;
    Phase6Demo& operator=(const Phase6Demo&) = delete;
    
    static std::unique_ptr<Phase6Demo> createInstance(int argc, char* argv[]);
    
private:
    // Core experiment
    Phase6IntegratedExperiment experiment_;
    Phase6IntegrationResult results_;
    
    // Configuration
    std::string checkpointPath_;
    size_t neuronCount_;
    size_t maxSteps_;
    bool enableCheckpointing_;
    bool enableReplay_;
    bool enableDevelopment_;
    
    // State
    bool initialized_;
    bool success_;
    
    // Helper methods
    void printBanner() const;
    void printResults() const;
    bool parseCommandLine(int argc, char* argv[]);
    void setupConfiguration();
    void cleanup();
    
    friend class Phase6DemoFactory;
};

// Factory function for creating demo instances
inline std::unique_ptr<Phase6Demo> createPhase6Demo(int argc, char* argv[]) {
    return Phase6Demo::createInstance(argc, argv);
}

} // namespace nlm
