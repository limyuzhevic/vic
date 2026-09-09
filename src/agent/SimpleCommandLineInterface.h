// SimpleCommandLineInterface.h - Command-line interface for NLM

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

/**
 * Command-line interface for NLM (Neural Learning Machine)
 * 
 * Provides a unified interface for:
 * - Running simulations
 * - Loading/saving brain states
 * - Configuration management
 * - Experiment control
 */

class SimpleCommandLineInterface {
public:
    SimpleCommandLineInterface();
    ~SimpleCommandLineInterface();
    
    /**
     * Parse command line arguments and run appropriate actions
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument strings
     * @return Exit code (0 for success, non-zero for error)
     */
    int run(int argc, char* argv[]);
    
    /**
     * Print help information
     */
    void printHelp() const;
    
    /**
     * Print version information
     */
    void printVersion() const;
    
    /**
     * Load configuration from file or string
     */
    bool loadConfiguration(const std::string& configPath);
    
    /**
     * Save configuration to file
     */
    bool saveConfiguration(const std::string& configPath) const;
    
    /**
     * Run simulation with current configuration
     */
    bool runSimulation(int steps);
    
    /**
     * Save current brain state
     */
    bool saveBrainState(const std::string& filepath) const;
    
    /**
     * Load brain state
     */
    bool loadBrainState(const std::string& filepath);
    
    /**
     * Run integration test
     */
    bool runIntegrationTest();
    
    /**
     * Run learning experiment
     */
    bool runLearningExperiment();
    
    /**
     * Get current configuration
     */
    std::shared_ptr<Config> getConfiguration() const { return config_; }
    
    /**
     * Set configuration
     */
    void setConfiguration(std::shared_ptr<Config> config) { config_ = config; }
    
private:
    /**
     * Parse common options (help, version, config file)
     */
    bool parseCommonOptions(int& currentArg);
    
    /**
     * Parse simulation options
     */
    bool parseSimulationOptions(int& currentArg);
    
    /**
     * Parse experiment options
     */
    bool parseExperimentOptions(int& currentArg);
    
    /**
     * Print simulation results
     */
    void printSimulationResults(int steps) const;
    
    /**
     * Validate configuration
     */
    bool validateConfiguration() const;
    
    // Current configuration
    std::shared_ptr<Config> config_;
    
    // Current brain (if loaded)
    std::shared_ptr<Brain> currentBrain_;
    
    // Command line options
    std::string configFile_;
    std::string brainFile_;
    std::string outputFile_;
    int simulationSteps_;
    bool verboseMode_;
    bool quietMode_;
    bool saveCheckpoints_;
    bool loadCheckpoint_;
};

} // namespace nlm
