#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>

namespace nlm {

class CommandLineParser {
public:
    struct Option {
        std::string flag;
        std::string value;
        std::string description;
    };
    
    CommandLineParser(int argc, char** argv);
    
    // Get a specific option value
    std::string getOption(const std::string& flag, const std::string& defaultValue = "") const;
    
    // Get boolean option (presence only)
    bool hasOption(const std::string& flag) const;
    
    // Get all options
    const std::map<std::string, std::string>& getAllOptions() const { return options_; }
    
    // Help and usage
    void printHelp() const;
    
    // Parse and setup configuration
    void applyToConfig(std::shared_ptr<Config> config) const;
    
    // List available experiments
    static void printAvailableExperiments();
    
private:
    std::map<std::string, std::string> options_;
    std::vector<Option> positionalArgs_;
};

} // namespace nlm