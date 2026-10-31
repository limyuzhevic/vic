#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <iomanip>

namespace nlm {

class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Parse and execute command line arguments
    bool run(int argc, char** argv);
    
    // Show help and version information
    void showHelp() const;
    void showVersion() const;
    
    // Configuration management
    void loadConfigurationFromFile(Config& config, const std::string& filepath);
    void applyPhaseDefaults(Config& config, const std::string& phase, 
                           const std::unordered_map<std::string, std::any>& cliArgs);
    void applyCommandLineOverrides(Config& config, 
                                   const std::unordered_map<std::string, std::any>& cliArgs);
    
    // Demonstration execution
    bool runPhase2Demo(const Config& config);
    bool runPhase6Demo(const Config& config);
    bool runBenchmark(const Config& config, 
                     const std::vector<size_t>& neuronCounts);
    
    // Individual tests
    void runBasicConnectivityTest(Brain& brain);
    void runPlasticityExperiment(Brain& brain);
    void runSTDPVerification(Brain& brain);
    
    // Output and logging
    void logConfiguration(const Config& config) const;
    void logResults(const std::string& title, 
                   const std::unordered_map<std::string, std::any>& metrics) const;
    
    // Utility functions
    std::vector<size_t> parseNeuronCounts(const std::string& input) const;
    std::string formatScientificNotation(double value) const;
    std::string getPhaseDescription(const std::string& phase) const;
    
protected:
    std::shared_ptr<Logger> logger;
    std::unordered_map<std::string, std::any> commandLineArgs;
    
private:
    // Parse command line arguments into key-value pairs
    void parseCommandLineArguments(int argc, char** argv);
    
    // Initialize configuration with phase-specific defaults
    void initializeDefaultConfiguration(Config& config, const std::string& phase);
    
    // Setup logging system
    void setupLoggingSystem(const std::string& level);
    
    // Helper methods for individual tests
    void printTestHeader(const std::string& testName) const;
    void printTestResult(const std::string& testName, bool success, 
                        const std::string& details = "") const;
    
    // Configuration file format detection and loading
    bool loadConfigurationFile(Config& config, const std::string& filepath);
    bool isJSONFile(const std::string& filepath) const;
    bool isYAMLError(const std::string& filepath) const;
};

} // namespace nlm
