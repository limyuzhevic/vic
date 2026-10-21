#pragma once

#include "brain/Brain.hpp"
#include "core/ConfigManager.hpp"
#include "core/TestRunner.hpp"
#include <string>
#include <memory>

namespace nlm {

class ExperimentController {
public:
    ExperimentController();
    ~ExperimentController() = default();
    
    // Disable copying
    ExperimentController(const ExperimentController&) = delete;
    ExperimentController& operator=(const ExperimentController&) = delete;
    
    // Initialize and run experiment
    bool runExperiment(const std::string& configFile, int argc, char** argv);
    
    // Get experiment status
    std::string getExperimentStatus() const { return statusMessage; }
    
    // Get experiment results
    std::string getExperimentResults() const { return experimentResults; }
    
private:
    std::string statusMessage;
    std::string experimentResults;
    std::unique_ptr<ConfigManager> configManager;
    std::unique_ptr<TestRunner> testRunner;
    
    // Setup phase
    bool setupPhase(const std::string& configFile, int argc, char** argv);
    
    // Run phase
    bool runPhase(std::shared_ptr<Brain> brain);
    
    // Cleanup phase
    void cleanupPhase();
    
    // Log startup information
    void logStartupInfo() const;
    
    // Log completion information
    void logCompletionInfo(std::shared_ptr<Brain> brain) const;
};

} // namespace nlm