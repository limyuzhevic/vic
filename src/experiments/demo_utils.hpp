#pragma once

#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "agent/MotorCommand.hpp"
#include "development/DevelopmentalStage.hpp"
#include "agent/SensoryPercept.hpp"
#include <string>
#include <vector>

namespace nlm {

// Common demo utility functions
namespace demo_utils {

// Standardized CLI options for all demos
class CLIOptions {
public:
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    size_t numTrials = 50;
    size_t checkpointSteps = 100;
    bool verbose = true;
    
    // Parse command line arguments
    bool parse(int argc, char* argv[]);
    
    // Display usage information
    void printUsage() const;
    
    // Get string representation of a motor command
    static const char* motorCommandToString(MotorCommand cmd);
    
    // Get string representation of a developmental stage
    static const char* developmentalStageToString(DevelopmentalStage stage);
    
    // Create a default configuration with sensible Phase 2 values
    static std::shared_ptr<Config> makeDefaultConfig();
    
    // Create a standard percept with default values
    static SensoryPercept makeStandardPercept(const std::vector<float>& vision,
                                             const std::vector<float>& touch,
                                             const std::string& text = "");
    
    // Common header printing for demos
    static void printHeader(const std::string& title);
    
    // Common result printing for demos
    static void printResult(const std::string& name, float value, float max = 1.0f);
    
    // Common section printing for demos
    static void printSection(const std::string& title);
    
    // Validate configuration parameters
    static bool validateConfig(const std::shared_ptr<Config>& config);
    
    // Get configuration parameter information for help
    static std::string getConfigParameterInfo(const std::string& key);
}

} // namespace demo_utils

} // namespace nlm
