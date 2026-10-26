// CommandLineParser.h - Unified command-line interface for NLM
#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

namespace nlm {

// Command-line parsing modes
enum class CommandMode {
    Phase3Demo,      // Phase 3 integration test demo
    Phase4Demo,      // Phase 4 cognitive experiments demo
    Phase6Demo,      // Phase 6 integration test
    Custom,         // Custom experiment configuration
    Debug,          // Interactive debugging mode
    Batch,          // Batch processing mode
    Config,         // Configuration file operations
    Help,           // Show help information
    Version         // Show version information
};

// Command-line argument parser with subcommand support
class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser();
    
    // Disable copying
    CommandLineParser(const CommandLineParser&) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get the parsed mode
    CommandMode getMode() const { return mode; }
    
    // Get the parsed configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Get additional arguments
    const std::vector<std::string>& getArgs() const { return additionalArgs; }
    
    // Check if a flag is set
    bool hasFlag(const std::string& flag) const;
    
    // Get flag value
    std::string getFlag(const std::string& flag) const;
    
    // Get argument value by index
    std::string getArgument(size_t index) const;
    
    // Get help information
    static std::string getHelp(const std::string& programName);
    
    // Get version information
    static std::string getVersion();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    CommandMode mode;
    std::shared_ptr<Config> config;
    std::vector<std::string> additionalArgs;
    std::map<std::string, std::string> flags;
    
    // Internal parsing methods
    bool parseMode(const std::string& arg);
    bool parseConfig(const std::string& filepath);
    bool parseArguments(const std::vector<std::string>& args);
    void printHelp(const std::string& programName) const;
    void printVersion() const;
};

} // namespace nlm