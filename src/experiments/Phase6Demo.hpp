#pragma once

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected. This is the
 * recommended entry point for NLM Phase 6 experiments.
 */

#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * Phase 6 demo configuration
 */
struct Phase6DemoConfig {
    // Experiment parameters
    uint64_t maxSteps = 2000;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Feature flags
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    // File paths
    std::string checkpointPath = "./nlm_phase6_checkpoint.bin";
    std::string logFile = "nlm_phase6_demo.log";
    
    // Performance
    bool enableVectorization = true;
    bool enableParallelProcessing = false;
    int maxThreads = 4;
    
    Phase6DemoConfig() = default;
};

/**
 * Phase 6 demo runner - the recommended entry point for Phase 6 experiments
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run the Phase 6 demo with the given configuration
     */
    bool run(const Phase6DemoConfig& config = Phase6DemoConfig());
    
    /**
     * Run quick integration verification
     */
    bool verifyIntegration();
    
    /**
     * Run individual system tests
     */
    bool testMemorySystem();
    bool testNeuromodulationSystem();
    bool testPredictionSystem();
    bool testDevelopmentSystem();
    bool testCheckpointSystem();
    bool testReplaySystem();
    
    /**
     * Set configuration values
     */
    void setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * Get configuration value
     */
    std::string getConfigValue(const std::string& key) const;
    
    /**
     * Print demo results
     */
    void printResults() const;
    
    /**
     * Save results to file
     */
    bool saveResults(const std::string& filename) const;
    
    /**
     * Load results from file
     */
    bool loadResults(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    bool setupLogging(const std::string& logFile) const;
    bool validateConfig(const Phase6DemoConfig& config) const;
    void printSystemStatus() const;
    void cleanup() noexcept;
};

// Convenience function for easy usage
inline bool runPhase6Demo(const Phase6DemoConfig& config = Phase6DemoConfig()) {
    Phase6Demo demo;
    return demo.run(config);
}

} // namespace nlm
