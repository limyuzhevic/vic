#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <fstream>

namespace nlm {

// Output format enumeration for CLI output
enum class OutputFormat {
    HUMAN_READABLE,
    JSON,
    CSV,
    COMPACT,
    MINIMAL
};

// CLI Configuration options
struct CLIConfig {
    std::string configFile;
    OutputFormat outputFormat;
    std::string outputFile;
    bool verbose;
    bool quiet;
    bool help;
    bool version;
    bool validateConfig;
    bool exportMetrics;
    std::map<std::string, std::string> customArgs;
    
    // Interactive mode options
    bool interactive;
    bool pauseOnError;
    bool enableStepMode;
    
    // Visualization options
    bool enableVisualization;
    std::string visualizationType;
    std::string plotFile;
    
    CLIConfig() : outputFormat(OutputFormat::HUMAN_READABLE), verbose(false), quiet(false),
                  help(false), version(false), validateConfig(false), exportMetrics(false),
                  interactive(false), pauseOnError(true), enableStepMode(false),
                  enableVisualization(false), visualizationType("text") {}
};

// Unified CLI parser and processor
class UnifiedCLI {
public:
    UnifiedCLI();
    ~UnifiedCLI();
    
    // Parse command line arguments
    bool parseArgs(int argc, char* argv[]);
    
    // Get parsed configuration
    const CLIConfig& getConfig() const { return config_; }
    
    // Process configuration and load settings
    bool processConfig(std::shared_ptr<Config> config);
    
    // Print help information
    void printHelp(const std::string& programName);
    
    // Print version information
    void printVersion();
    
    // Print configuration summary
    void printConfigSummary(const std::shared_ptr<Config>& config) const;
    
    // Convert output format to string
    std::string outputFormatToString(OutputFormat format) const;
    
    // Convert human-readable output to target format
    void convertOutput(const std::string& humanOutput, OutputFormat targetFormat, std::ostream& output);
    
    // Validate configuration
    bool validateConfig(const std::shared_ptr<Config>& config) const;
    
    // Get common configuration options for demos
    void addCommonConfigOptions(std::shared_ptr<Config> config);
    
    // Parse standard demo arguments
    bool parseDemoArgs(int argc, char* argv[], int& numEpisodes, int& stepsPerEpisode,
                       int& numTrials, int& maxSteps);
    
private:
    CLIConfig config_;
    std::string programName_;
    
    // Helper methods
    void parseStandardArgs(int argc, char* argv[]);
    void parseKeyValueArgs(int argc, char* argv[]);
    OutputFormat parseOutputFormat(const std::string& format) const;
    
    // Validation methods
    bool validateConfigValues(const std::shared_ptr<Config>& config) const;
    bool validateCLIConfig() const;
};

} // namespace nlm