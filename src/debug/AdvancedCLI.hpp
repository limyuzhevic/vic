// Enhanced CLI interface for NLM v2.0+
// This file provides advanced command-line argument parsing and validation

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <chrono>

namespace nlm {

// Advanced CLI option definitions
struct SimulationControlOptions {
    int maxSteps = 1000;
    double timestep = 0.001;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    double temperature = 1.0;
    double learningRate = 1.0;
    bool plasticityEnabled = true;
    bool neuromodulationEnabled = true;
    bool developmentEnabled = true;
    std::string connectionProbability = "0.15";
    std::string stdpLtpWeight = "0.02";
    std::string stdpLtdWeight = "0.015";
    std::string stdpTau = "20.0";
    std::string synaptogenesisRate = "0.0001";
    std::string pruningRate = "0.00001";
};

struct MemorySystemOptions {
    int64_t workingMemorySize = 1024;
    int64_t episodicMemorySize = 5000;
    bool memoryReplay = false;
    int replayFrequency = 10;
};

struct NeuromodulationOptions {
    double dopamineLevel = 0.5;
    double curiosityLevel = 0.3;
    double noveltyThreshold = 0.5;
    double rewardScale = 1.0;
    // Additional neuromodulators
    double achLevel = 0.5;
    double neLevel = 0.5;
    double fiveHTLevel = 0.5;
    double nicotineLevel = 0.5;
    double orexinLevel = 0.5;
};

struct DebugOptions {
    int debugLevel = 0; // 0=none, 1=basic, 2=detailed, 3=trace
    std::vector<std::string> traceTargets; // neuron, synapse, memory, neuromodulator
    std::vector<std::string> profileTargets; // simulation, memory, neuromodulation
    std::optional<std::string> checkpointFile;
    std::vector<std::string> visualizationTargets; // network, spikes, memory
};

struct LoggingOptions {
    std::string logLevel = "info"; // debug, info, warning, error
    std::optional<std::string> logFile;
    std::string logFormat = "text"; // text, json, csv
    bool eventLog = false;
    bool traceSim = false;
    bool structuredLog = false;
    bool logMetrics = false;
    bool logNeuralActivity = false;
};

struct ExportOptions {
    std::string exportNetworkFormat = "none"; // dot, gexf, graphml
    std::optional<std::string> exportNetworkPath;
    std::string exportSpikesFormat = "none"; // txt, csv
    std::optional<std::string> exportSpikesPath;
    std::string exportMemoryFormat = "none"; // txt, json
    std::optional<std::string> exportMemoryPath;
    std::string exportBehaviorFormat = "none"; // txt, csv
    std::optional<std::string> exportBehaviorPath;
    bool liveViz = false;
    int vizPort = 8080;
    int vizUpdateInterval = 1000;
};

struct PerformanceOptions {
    std::vector<std::string> measurementTargets; // cpu, memory, gpu, network
    std::string benchmarkMode = "none"; // basic, detailed, stress
    int profileInterval = 100;
    bool performanceMetrics = false;
    bool memoryUsage = false;
    bool neuronFiringRate = false;
};

struct SystemOptions {
    int threads = 0; // 0 = auto-detect
    bool simd = false;
    bool memoryPools = false;
    std::string checkpointCompression = "gzip"; // none, gzip, zlib
};

struct EnvironmentOptions {
    // Environment variable overrides
    std::optional<std::string> nlmLogLevel;
    std::optional<std::string> nlmConfigPath;
    std::optional<std::string> nlmDataPath;
    bool nlmPythonBindings = false;
};

struct InteractiveOptions {
    bool interactive = false;
    std::optional<std::string> scriptFile;
    bool monitor = false;
    std::vector<std::string> memoryInspectionTargets; // working, episodic, semantic, procedural, associative
    std::vector<std::string> neuromodulatorInspectionTargets; // dopamine, curiosity, novelty, ACh, NE, 5-HT
};

// Configuration file path management
struct ConfigFileOptions {
    std::vector<std::string> configFiles; // --config1, --config2, etc.
    std::map<std::string, std::string> varRefs; // --var-ref=config_key
};

// Advanced configuration format options
struct ConfigFormatOptions {
    std::map<std::string, std::string> setOptions; // --set=key=value, --set=neuron_count,500
    std::vector<std::string> enableOptions; // --enable=plasticity, --enable=neuromodulation
    std::map<std::string, std::string> overrideOptions; // --override=simulation_timestep,0.0001
};

// Main enhanced CLI options structure
struct AdvancedCLIOptions {
    SimulationControlOptions simulation;
    MemorySystemOptions memory;
    NeuromodulationOptions neuromodulation;
    DebugOptions debug;
    LoggingOptions logging;
    ExportOptions export;
    PerformanceOptions performance;
    SystemOptions system;
    EnvironmentOptions environment;
    InteractiveOptions interactive;
    ConfigFileOptions configFiles;
    ConfigFormatOptions configFormat;
    
    // Legacy options (for backward compatibility)
    std::optional<std::string> legacyConfigFile;
    std::vector<std::string> legacyArgs;
};

// Enhanced CLI parser class
class AdvancedCLI {
public:
    AdvancedCLI();
    ~AdvancedCLI() = default;
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed options
    const AdvancedCLIOptions& getOptions() const { return options; }
    
    // Convert to NLM configuration
    void applyToConfig(std::shared_ptr<Config> config) const;
    
    // Validate options
    std::vector<std::string> validate() const;
    
    // Get help text
    static std::string getHelpText();
    
    // Get version info
    static std::string getVersionInfo();
    
    // Check if specific option was provided
    bool hasOption(const std::string& option) const;
    
    // Get help for specific option
    std::string getOptionHelp(const std::string& option) const;
    
private:
    AdvancedCLIOptions options;
    std::unordered_map<std::string, std::function<bool(const std::string&)>> optionParsers;
    
    // Initialize option parsers
    void initializeOptionParsers();
    
    // Parse individual option types
    bool parseSimulationOptions(const std::string& arg);
    bool parseMemoryOptions(const std::string& arg);
    bool parseNeuromodulationOptions(const std::string& arg);
    bool parseDebugOptions(const std::string& arg);
    bool parseLoggingOptions(const std::string& arg);
    bool parseExportOptions(const std::string& arg);
    bool parsePerformanceOptions(const std::string& arg);
    bool parseSystemOptions(const std::string& arg);
    bool parseInteractiveOptions(const std::string& arg);
    bool parseConfigOptions(const std::string& arg);
    bool parseEnvironmentOptions(const std::string& arg);
    
    // Parse special formats
    bool parseSetOption(const std::string& arg);
    bool parseEnableOption(const std::string& arg);
    bool parseOverrideOption(const std::string& arg);
    bool parseVarRefOption(const std::string& arg);
    
    // Parse boolean values
    bool parseBoolValue(const std::string& value) const;
    
    // Parse numeric values
    double parseDoubleValue(const std::string& value) const;
    int64_t parseIntValue(const std::string& value) const;
    
    // Parse comma-separated lists
    std::vector<std::string> parseStringList(const std::string& value) const;
    
    // Parse vector values
    std::vector<double> parseDoubleList(const std::string& value) const;
    
    // Handle environment variable substitution
    std::string resolveVariables(const std::string& value) const;
    
    // Configuration file path parsing
    void parseConfigFileOption(const std::string& arg);
    
    // Legacy argument parsing (for backward compatibility)
    void parseLegacyArguments(int argc, char** argv);
    
    // Option normalization
    std::string normalizeOptionName(const std::string& option) const;
};

} // namespace nlm
