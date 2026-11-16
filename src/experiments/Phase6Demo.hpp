#pragma once

/**
 * Phase6Demo - Phase 6 Integration Demo
 * 
 * This demo runs the Phase 6 integration test to verify that all brain systems
 * are properly integrated into a coherent artificial brain.
 * 
 * The demo tests:
 * - Integration verification (all systems connected)
 * - Memory integration (working and episodic memory)
 * - Neuromodulation integration (dopamine, curiosity, novelty)
 * - Prediction system integration
 * - Development system integration
 * - Checkpoint save/load functionality
 * - Replay and consolidation functionality
 * 
 * This demo demonstrates that Phase 6 achieves its goal: "All systems
 * integrated into coherent brain loop that functions as a unified system"
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <memory>

namespace nlm {

/**
 * Configuration for Phase 6 demo
 */
struct Phase6DemoConfig {
    // Simulation parameters
    uint64_t maxSteps = 2000;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Feature flags
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    // File paths
    std::string checkpointPath = "./checkpoint_test.bin";
    
    // World configuration
    float worldWidth = 16.0f;
    float worldHeight = 16.0f;
    size_t visionWidth = 8;
    size_t visionHeight = 8;
    
    Phase6DemoConfig() = default;
};

/**
 * Phase 6 Integration Demo Runner
 * 
 * This class provides a complete Phase 6 demo with:
 * - Integration verification
 * - System-specific integration tests
 * - Full integration experiment with agent-world interaction
 * - Comprehensive result reporting
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run the complete Phase 6 demo
     */
    bool run(const Phase6DemoConfig& config);
    
    /**
     * Get the experiment result
     */
    const Phase6IntegrationResult& getResult() const { return result_; }
    
    /**
     * Print demo summary
     */
    void printSummary() const;
    
    /**
     * Check if demo completed successfully
     */
    bool success() const;
    
private:
    Phase6IntegrationResult result_;
    std::unique_ptr<Phase6IntegratedExperiment> experiment_;
    std::unique_ptr<Brain> brain_;
    std::unique_ptr<AgentBrain> agent_;
    std::unique_ptr<SimpleWorld> world_;
    
    /**
     * Initialize demo with configuration
     */
    bool initialize(const Phase6DemoConfig& config);
    
    /**
     * Print demo banner
     */
    void printBanner() const;
    
    /**
     * Print configuration summary
     */
    void printConfiguration(const Phase6DemoConfig& config) const;
    
    /**
     * Run integration verification
     */
    bool runIntegrationVerification();
    
    /**
     * Run system-specific integration tests
     */
    void runSystemIntegrationTests();
    
    /**
     * Run full integration experiment
     */
    void runFullIntegration(const Phase6DemoConfig& config);
    
    /**
     * Print detailed results
     */
    void printResults() const;
};

} // namespace nlm
