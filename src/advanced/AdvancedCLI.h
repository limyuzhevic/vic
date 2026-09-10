#pragma once

#include <string>
#include <map>

// Advanced Command-Line Interface for NLM Phase 6
// Provides enhanced control over brain configuration and system parameters

class AdvancedCommandLineInterface {
public:
    AdvancedCommandLineInterface();
    
    // Parse advanced command-line options
    bool parseAdvancedOptions(const std::vector<std::string>& args, 
                              std::map<std::string, std::string>& configOptions,
                              std::map<std::string, std::string>& systemOptions);
    
    // Generate advanced usage documentation
    void printAdvancedUsage();
};