#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>

namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Get parsed options
    const std::map<std::string, std::string>& getOptions() const { return parsedOptions_; }
    
    // Get boolean flags
    const std::map<std::string, bool>& getFlags() const { return parsedFlags_; }
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs_; }
    
    // Check if a specific option is set
    bool hasOption(const std::string& option) const;
    
    // Check if a specific flag is set
    bool isFlagSet(const std::string& flag) const;
    
    // Get option value as string
    std::string getStringOption(const std::string& option, const std::string& defaultValue = "") const;
    
    // Get option value with type conversion
    template<typename T>
    std::optional<T> getOption(const std::string& option) const;
    
    // Get flag value
    bool getFlag(const std::string& flag, bool defaultValue = false) const;
    
    // Print help information
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Print configuration summary
    void printConfigSummary() const;
    
    // Load configuration from file
    bool loadConfig(const std::string& filename);
    
    // Save configuration to file
    bool saveConfig(const std::string& filename) const;
    
    // Enable/disable verbose output
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // Enable/disable debug output
    void setDebug(bool debug) { debug_ = debug; }
    
    // Enable/disable profiling
    void setProfiling(bool profiling) { profiling_ = profiling; }
    
    // Get performance statistics
    std::string getPerformanceStats() const;
    
    // Execute simulation with current configuration
    int executeSimulation() const;
    
protected:
    // Argument parsing methods
    bool parseShortOptions(char** argv, int& currentIndex);
    bool parseLongOptions(char** argv, int& currentIndex);
    bool parseOption(const std::string& arg);
    bool parseFlag(const std::string& arg);
    
    // Option/flag handlers
    void registerStandardOptions();
    void registerDebugOptions();
    void registerPerformanceOptions();
    void registerOutputOptions();
    
    // Error handling
    void printError(const std::string& message) const;
    void printWarning(const std::string& message) const;
    
    // Validation
    bool validateArguments() const;
    bool validateConfig() const;
    
private:
    std::shared_ptr<Config> config_;
    std::map<std::string, std::string> parsedOptions_;
    std::map<std::string, bool> parsedFlags_;
    std::vector<std::string> positionalArgs_;
    
    bool verbose_;
    bool debug_;
    bool profiling_;
    std::string configFile_;
    std::string outputFile_;
    bool helpRequested_;
    bool versionRequested_;
    bool benchmarkRequested_;
    
    // Performance tracking
    double startTime_;
    double endTime_;
    size_t stepsCompleted_;
    size_t neuronsFired_;
};

} // namespace nlm