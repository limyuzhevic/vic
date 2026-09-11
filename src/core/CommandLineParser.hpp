#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * Command line argument parser for NLM
 * 
 * Supports:
 * - Experiment type selection (--phase2, --phase6)
 * - Config file (--config=file)
 * - Parameter overrides (--param=value)
 * - Help flag (-h/--help)
 * - Version (--version)
 */
class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv);
    ~CommandLineParser() = default;
    
    // Parse command line arguments
    bool parse();
    
    // Check for help flag
    bool showHelp() const { return showHelp_; }
    
    // Check for version flag
    bool showVersion() const { return showVersion_; }
    
    // Get experiment type
    std::string getExperimentType() const { return experimentType_; }
    
    // Get config file path
    std::string getConfigFile() const { return configFile_; }
    
    // Get parameter overrides
    std::unordered_map<std::string, std::string> getParameterOverrides() const { return parameterOverrides_; }
    
    // Get parsed configuration
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Print help message
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
private:
    int argc_;
    char** argv_;
    
    // Parsed values
    bool showHelp_;
    bool showVersion_;
    std::string experimentType_;
    std::string configFile_;
    std::unordered_map<std::string, std::string> parameterOverrides_;
    std::shared_ptr<Config> config_;
    
    // Internal parsing methods
    void parseArgs();
    void parseConfigFile();
    void applyParameterOverrides();
    void initializeDefaultConfig();
    
    // Argument detection
    static bool isFlag(const std::string& arg, const std::string& flag);
    static bool isConfigFlag(const std::string& arg);
    static bool isParameterOverride(const std::string& arg);
    
    // Help and version strings
    static constexpr const char* HELP_MESSAGE = R"(
NLM - Neural Learning Machine

Usage: nlm [--help] [--version] [--phase2|--phase6] [--config=file] [--param=value]...

Options:
  -h, --help           Show this help message
  --version            Show version information
  --phase2             Run Phase 2 (Real Neural Computation)
  --phase6             Run Phase 6 (Integration Test)
  --config=file        Load configuration from file (JSON or key=value format)
  --param=value        Override configuration parameter (e.g., --neuron_count=100)
  -p value             Alternative short form for parameter override

If no experiment type is specified, defaults to --phase2.

Examples:
  nlm --phase2
  nlm --phase6 --config=phase6_config.json --neuron_count=500
  nlm --phase2 --config=config.cfg --stdp_ltp_weight=0.03
  nlm --help
  nlm --version

Configuration files can be in JSON format (.json) or key=value format (.cfg).
Parameter values can be integers, floats, booleans (true/false), or strings.
)";
    
    static constexpr const char* VERSION_INFO = "NLM v2.0.0 - Neural Learning Machine\nPhase 2: Real Neural Computation";

} // namespace nlm
