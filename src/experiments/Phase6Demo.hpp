/**
 * Phase 6 Demo - Comprehensive Integration Test Framework
 * 
 * This header defines the improved Phase 6 demo system with comprehensive
 * integration testing, validation, and reporting capabilities.
 */

#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Command-line argument parser for Phase 6 demo
 */
class DemoArgs {
public:
    size_t numTrials = 1;
    size_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_demo.bin";
    bool verbose = false;
    std::string outputFile = "";
    bool runOnlyVerification = false;
    
    DemoArgs() = default;
    
    void setTrials(size_t trials) { numTrials = trials; }
    void setNeurons(size_t neurons) { neuronCount = neurons; }
    void setSteps(uint64_t steps) { maxSteps = steps; }
    void setCheckpointing(bool enable) { enableCheckpointing = enable; }
    void setReplay(bool enable) { enableReplay = enable; }
    void setDevelopment(bool enable) { enableDevelopment = enable; }
    void setCheckpointPath(const std::string& path) { checkpointPath = path; }
    void setVerbose(bool verboseFlag) { verbose = verboseFlag; }
    void setOutputFile(const std::string& file) { outputFile = file; }
    void setVerifyOnly(bool verify) { runOnlyVerification = verify; }
    
    void printHelp() const;
};

/**
 * Comprehensive integration test runner for Phase 6
 */
class Phase6Demo {
public:
    Phase6Demo();
    
    /**
     * Run the complete demo with all tests and validation
     */
    bool run(const DemoArgs& args);
    
    /**
     * Run only integration verification tests
     */
    bool runVerificationOnly(const DemoArgs& args);
    
    /**
     * Run single integration test with detailed reporting
     */
    void runSingleTest(const DemoArgs& args);
    
private:
    Phase6IntegratedExperiment experiment;
    
    /**
     * Configure experiment from demo arguments
     */
    void configureFromArgs(Phase6Config& config, const DemoArgs& args);
    
    /**
     * Save results to file
     */
    void saveResultsToFile(const std::string& filename, 
                          const std::vector<Phase6IntegrationResult>& allResults,
                          const Phase6IntegrationResult& finalSummary);
    
    /**
     * Get current timestamp for file headers
     */
    std::string getCurrentTimestamp() const;
    
    /**
     * Summarize results across multiple trials
     */
    Phase6IntegrationResult summarizeResults(const std::vector<Phase6IntegrationResult>& allResults);
    
    /**
     * Print detailed trial results
     */
    void printTrialResults(const Phase6IntegrationResult& result, size_t trialNum);
};

} // namespace nlm