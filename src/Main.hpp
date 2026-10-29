#pragma once

#include <memory>
#include <string>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"

namespace nlm {

/**
 * @brief Main application class with unified CLI interface
 * 
 * Provides a comprehensive command-line interface for running NLM simulations,
 * including configuration management, experiment execution, and debugging features.
 */
class NLMApplication {
public:
    /**
     * @brief Constructor
     */
    NLMApplication();
    
    /**
     * @brief Destructor
     */
    ~NLMApplication();
    
    /**
     * @brief Run the application with provided arguments
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return Exit code (0 for success, non-zero for error)
     */
    int run(int argc, char** argv);
    
    /**
     * @brief Run all Phase 2 experiments
     * 
     * Executes the three main Phase 2 tests: basic connectivity, plasticity learning, and STDP verification.
     * 
     * @param config Configuration to use
     * @return true if all tests pass, false on error
     */
    bool runPhase2Experiments(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Run specific experiment or mode
     * 
     * @param mode Experiment mode
     * @param config Configuration to use
     * @param steps Number of simulation steps
     * @return true if successful, false on error
     */
    bool runExperiment(const std::string& mode, const std::shared_ptr<Config>& config, int steps);
    
    /**
     * @brief Initialize logging system
     * 
     * @param config Configuration containing logging settings
     */
    void initializeLogging(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Create and initialize brain
     * 
     * @param config Configuration to use
     * @return Shared pointer to initialized brain
     */
    std::shared_ptr<Brain> createAndInitializeBrain(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Display application information and help
     */
    void showInfo() const;

private:
    std::shared_ptr<Logger> logger;
    bool verboseOutput;
};

} // namespace nlm