#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <functional>

namespace nlm {

// Command-line argument parsing result
struct ParsedArgs {
    std::string experimentType;
    std::string configFile;
    std::string saveFile;
    std::string loadFile;
    int64_t maxSteps = -1;
    uint64_t seed = 42;
    bool enableBatch = false;
    bool enableInteractive = false;
    bool enableHelp = false;
    bool enableMetrics = false;
    std::vector<std::string> customArgs;
    std::map<std::string, std::string> optionMap;
};

// Simple command-line argument parser
class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv);
    
    // Parse arguments
    ParsedArgs parse() const;
    
    // Add option definition
    void addOption(const std::string& longName, char shortName = 0,
                  const std::string& description = "", 
                  bool requiresValue = false);
    
    // Get help text
    std::string getHelp() const;
    
    // Check if option exists
    bool hasOption(const std::string& name) const;
    
    // Get option value
    std::string getOptionValue(const std::string& name) const;
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm