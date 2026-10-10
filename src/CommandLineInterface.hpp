#pragma once

#include <string>
#include <vector>
#include <map>

namespace nlm {

// Enhanced command line interface for NLM
// Supports configuration file parsing, option handling, and help system
class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Disable copying, enable moving
    CommandLineInterface(const CommandLineInterface&) = delete;
    CommandLineInterface& operator=(const CommandLineInterface&) = delete;
    CommandLineInterface(CommandLineInterface&&) noexcept;
    CommandLineInterface& operator=(CommandLineInterface&&) noexcept;
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Display help information
    void printHelp() const;
    
    // Display version information
    void printVersion() const;
    
    // Display startup banner
    void printStartupBanner() const;
    
    // Display configuration summary
    void printConfigurationSummary() const;
    
    // Get parsed options and arguments
    const std::map<std::string, std::string>& getOptions() const;
    const std::vector<std::string>& getArgs() const;
    
    // Check for special flags
    bool isHelpRequested() const;
    bool isVersionRequested() const;
    bool isVerboseMode() const;
    
    // Get configuration values
    const std::string& getConfigFile() const;
    const std::string& getOutputFile() const;
    const std::string& getLogLevel() const;
    
    // Get numeric values
    uint64_t getSeed() const;
    int64_t getNeuronCount() const;
    int64_t getRegionCount() const;
    float getConnectionProbability() const;
    
    // Set/get arbitrary options
    void setOption(const std::string& key, const std::string& value);
    std::string getOption(const std::string& key) const;
    bool hasOption(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
