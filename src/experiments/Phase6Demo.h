/**
 * NLM Phase 6 Integration Demo
 *
 * This is the main entry point for the Phase 6 integration test.
 * It runs a comprehensive test of all integrated brain systems.
 */

#pragma once

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>

namespace nlm {

/**
 * Phase 6 Integration Demo
 *
 * Runs the complete Phase 6 integration test to verify that all
 * brain systems are properly connected and functional.
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Run the Phase 6 integration demo
     */
    void run(const std::string& configPath = "configs/default.cfg");
    
    /**
     * Run with custom configuration
     */
    void run(const Phase6Config& config);
    
    /**
     * Run quick integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint functionality
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     */
    bool testReplay();
    
    /**
     * Get the experiment results
     */
    const Phase6IntegrationResult& getResults() const { return results_; }
    
    /**
     * Print demo banner
     */
    static void printBanner();
    
    /**
     * Print usage information
     */
    static void printUsage();

private:
    Phase6IntegrationResult results_;
    std::string configPath_;
    std::unique_ptr<Phase6IntegratedExperiment> experiment_;
};

} // namespace nlm


} // namespace nlm
