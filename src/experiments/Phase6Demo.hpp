#pragma once

/**
 * Phase6Demo - Phase 6 Integration Test Demo
 * 
 * This demo runs the Phase 6 integration test to verify that all brain systems
 * are properly connected and working together.
 * 
 * The demo provides a simple command-line interface for running the integration
 * test with customizable parameters and comprehensive output.
 * 
 * @note This is Phase 6 of the NLM project (FINAL INTEGRATION)
 * @author NLM Research Team
 * @version 0.1.0
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace nlm {

/**
 * Command-line options for Phase 6 demo
 */
struct DemoOptions {
    uint64_t neuronCount = 500;           // Number of neurons
    uint64_t maxSteps = 2000;             // Maximum simulation steps
    bool enableCheckpointing = true;      // Enable checkpoint save/load
    bool enableReplay = true;             // Enable replay functionality
    bool enableDevelopment = true;        // Enable developmental processes
    std::string checkpointPath = "./checkpoint.bin"; // Checkpoint file path
    bool verbose = false;                 // Enable verbose logging
    std::string configFile = "configs/default.cfg"; // Configuration file
    uint64_t customSeed = 42;             // Random seed for reproducibility
};

/**
 * Phase 6 integration test demonstration
 * 
 * This class provides a command-line interface for running the Phase 6
 * integration test with customizable parameters and comprehensive
 * output reporting.
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    /**
     * Parse command-line arguments
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument strings
     * @return DemoOptions structure with parsed configuration
     */
    DemoOptions parseArguments(int argc, char* argv[]);
    
    /**
     * Print demo help information
     */
    void printHelp() const;
    
    /**
     * Run the Phase 6 integration demo
     * 
     * @param options Configuration options parsed from command line
     * @return Exit code (0 for success, non-zero for failure)
     */
    int run(const DemoOptions& options);
    
    /**
     * Print configuration summary
     * 
     * @param options Configuration options to display
     */
    void printConfiguration(const DemoOptions& options) const;
    
    /**
     * Run the integration test with timing and metrics
     * 
     * @param experiment Phase 6 integrated experiment instance
     * @param options Demo configuration options
     * @return Integration test results
     */
    Phase6IntegrationResult runIntegrationTest(Phase6IntegratedExperiment& experiment, const DemoOptions& options);
    
    /**
     * Print formatted results
     * 
     * @param result Integration test results
     */
    void printResults(const Phase6IntegrationResult& result) const;
    
    /**
     * Validate options and configuration
     * 
     * @param options Options to validate
     * @return true if valid, false otherwise
     */
    bool validateOptions(const DemoOptions& options) const;

private:
    // Logging level mapping
    std::map<std::string, Logger::Level> logLevelMap;
    
    /**
     * Initialize logging system
     */
    void initializeLogging();
    
    /**
     * Setup experiment configuration
     * 
     * @param options Demo options
     * @return Configuration for the experiment
     */
    Phase6Config setupExperimentConfig(const DemoOptions& options) const;
    
    /**
     * Display progress information during experiment
     * 
     * @param step Current simulation step
     * @param maxSteps Maximum simulation steps
     * @param startTime Simulation start time
     */
    void displayProgress(size_t step, size_t maxSteps, time_t startTime) const;
    
    /**
     * Calculate elapsed time
     * 
     * @param startTime Start time (seconds since epoch)
     * @return Elapsed time in seconds
     */
    double calculateElapsedTime(time_t startTime) const;
};

} // namespace nlm
