// Command-line interface for NLM (Neural Learning Machine)
// Provides comprehensive control over simulation parameters and execution

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

namespace nlm {

// Command-line option types
enum class OptionType {
    String,
    Integer,
    Float,
    Boolean,
    File
};

// Command-line argument structure
struct CLIArgument {
    std::string name;
    std::string shortName;
    std::string description;
    OptionType type;
    std::string defaultValue;
    bool required;
};

// Command-line interface class
class CLIInterface {
public:
    CLIInterface();
    ~CLIInterface();
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Get parsed values
    std::string getString(const std::string& key) const;
    int64_t getInt(const std::string& key) const;
    double getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::string getFile(const std::string& key) const;
    
    // Check if argument was provided
    bool hasArgument(const std::string& key) const;
    
    // Print help information
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Get configuration for NLM
    std::shared_ptr<Config> createNLMConfig() const;
    
    // Validate configuration
    bool validateConfig(const Config& config) const;
    
    // Set up logging based on command line options
    void setupLogging(const Config& config) const;
    
    // Get list of available experiment modes
    static std::vector<std::string> getAvailableExperiments();
    
    // Get list of available visualization modes
    static std::vector<std::string> getAvailableVisualizations();
    
    // Get list of performance modes
    static std::vector<std::string> getAvailablePerformanceModes();
    
private:
    // Define available command-line arguments
    static std::vector<CLIArgument> getAvailableArguments();
    
    // Parse and store arguments
    void parseArguments(const std::vector<std::string>& args);
    
    // Helper functions for argument parsing
    bool parseStringArgument(const std::string& arg, const std::string& value);
    bool parseIntArgument(const std::string& arg, const std::string& value);
    bool parseFloatArgument(const std::string& arg, const std::string& value);
    bool parseBoolArgument(const std::string& arg, const std::string& value);
    bool parseFileArgument(const std::string& arg, const std::string& value);
    
    // Store parsed values
    std::map<std::string, std::string> stringValues_;
    std::map<std::string, int64_t> intValues_;
    std::map<std::string, double> floatValues_;
    std::map<std::string, bool> boolValues_;
    std::map<std::string, std::string> fileValues_;
    
    // Error handling
    std::vector<std::string> errors_;
    
    // Configuration file path
    std::string configFile_;
};

// Enhanced main function with CLI support
int runNLMWithCLI(int argc, char** argv);

// Helper functions for CLI
namespace CLI {
    // Print formatted output
    void printInfo(const std::string& message);
    void printError(const std::string& message);
    void printWarning(const std::string& message);
    
    // Parse common simulation options
    struct SimulationOptions {
        double timestep;
        int64_t neuronCount;
        int64_t regionCount;
        float connectionProbability;
        std::string experimentMode;
        std::string visualizationMode;
        std::string performanceMode;
        bool enableVisualization;
        bool enableProfiling;
        bool enableDebugLogging;
        int logLevel;
        int threadCount;
    };
    
    SimulationOptions parseSimulationOptions(const std::map<std::string, std::string>& args);
    
    // Parse experiment-specific options
    struct ExperimentOptions {
        int64_t episodes;
        int64_t maxSteps;
        std::string rewardFunction;
        bool enableExploration;
        bool enableDevelopment;
        bool enableNeuromodulation;
        double rewardScale;
        double noveltyScale;
    };
    
    ExperimentOptions parseExperimentOptions(const std::map<std::string, std::string>& args);
    
    // Save simulation results
    bool saveResults(const std::string& prefix, const std::shared_ptr<Config>& config);
    
    // Load simulation results
    bool loadResults(const std::string& filename, std::shared_ptr<Config>& config);
    
    // Display simulation statistics
    void displayStatistics(const std::shared_ptr<Config>& config);
    
    // Check for command line option
    bool hasOption(int argc, char** argv, const std::string& option);
    
    // Get option value
    std::string getOptionValue(int argc, char** argv, const std::string& option);
    
    // Get option value with default
    std::string getOptionValue(int argc, char** argv, const std::string& option, const std::string& defaultValue);
    
    // Parse option with value
    bool parseOptionWithValue(int argc, char** argv, const std::string& option, std::string& value);
    
    // Parse option flag
    bool parseOptionFlag(int argc, char** argv, const std::string& option, bool& value);
}

} // namespace nlm
