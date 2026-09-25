// NLM Advanced CLI Interface
// 
// Enhanced command-line interface for the NLM (Neural Learning Machine) simulator
// Provides comprehensive control over simulation parameters, logging, and output.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace nlm {

// Command-line option definitions
struct CLIOptions {
    // === Basic Simulation Options ===
    bool helpRequested = false;                    // -h, --help
    bool versionRequested = false;                 // -v, --version
    std::string configFile = "configs/default.cfg"; // -c, --config
    std::string outputFile = "";
    std::string logFile = "nlm.log";
    std::string checkpointFile = "";
    
    // === Simulation Parameters ===
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    
    // === Brain Configuration ===
    std::string brainType = "phase6";               // phase2, phase3, phase4, phase5, phase6
    bool enableSTDP = true;
    bool enableHebbian = true;
    bool enableStructuralPlasticity = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    
    // === Neuromodulation ===
    bool enableDopamine = true;
    bool enablePredictionError = true;
    float dopamineBaseline = 0.05f;
    float dopaminePeak = 1.0f;
    
    // === Memory Systems ===
    bool enableWorkingMemory = true;
    bool enableEpisodicMemory = true;
    bool enableAssociativeMemory = true;
    size_t maxWorkingMemoryCapacity = 1000;
    size_t maxEpisodicMemoryEpisodes = 1000;
    
    // === Experiment Control ===
    bool runBasicTests = true;                      // Test 1: Basic connectivity
    bool runPlasticityTests = true;                  // Test 2: Plasticity learning
    bool runStdpVerification = true;                // Test 3: STDP verification
    bool runIntegrationTest = false;                 // Run full integration test
    size_t testSteps = 1000;
    
    // === Output and Logging ===
    std::string logLevel = "INFO";                 // DEBUG, INFO, WARN, ERROR
    bool verboseOutput = false;
    bool quietMode = false;
    bool generateReport = true;
    std::string reportFormat = "text";             // text, json, csv
    
    // === Performance and Monitoring ===
    bool enableProfiling = false;
    bool enableTiming = true;
    bool showProgress = true;
    size_t progressInterval = 100;
    
    // === Checkpoint and Recovery ===
    bool enableCheckpointing = false;
    size_t checkpointInterval = 1000;
    size_t maxCheckpoints = 10;
    
    // === Advanced Features ===
    bool enableReplay = true;
    bool enableConsolidation = true;
    float replayInterval = 100.0f;
    float consolidationInterval = 1000.0f;
    
    // === Visualization ===
    bool enableVisualization = false;
    std::string visualizationType = "minimal";    // none, minimal, detailed
    bool saveVisualizations = false;
    
    // === Custom Parameters ===
    std::map<std::string, std::string> customParams;
    
    // === Help and Usage ===
    std::vector<std::string> experimentList = {
        "basic", "connectivity", "plasticity", "stdp", "integration", "full"
    };
    
    std::vector<std::string> logLevelList = {"DEBUG", "INFO", "WARN", "ERROR"};
    std::vector<std::string> brainTypeList = {"phase2", "phase3", "phase4", "phase5", "phase6"};
    std::vector<std::string> reportFormatList = {"text", "json", "csv"};
};

// Advanced CLI parser and handler
class AdvancedCLI {
public:
    AdvancedCLI() {
        initializeOptions();
        parseArguments();
    }
    
    ~AdvancedCLI() = default;
    
    // === Option Access ===
    const CLIOptions& getOptions() const { return options_; }
    CLIOptions& getOptions() { return options_; }
    
    // === Configuration Loading ===
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // === Command Processing ===
    bool processCommand(const std::string& command);
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // === Help and Documentation ===
    void showHelp() const;
    void showVersion() const;
    void showExperimentInfo(const std::string& experiment) const;
    void showConfigurationSummary() const;
    
    // === Option Management ===
    void setOption(const std::string& option, const std::string& value);
    std::string getOption(const std::string& option) const;
    bool hasOption(const std::string& option) const;
    
    // === Validation ===
    bool validateOptions() const;
    std::vector<std::string> getValidationErrors() const;
    
    // === Statistics and Reporting ===
    void logStatistics(const std::chrono::high_resolution_clock::duration& duration) const;
    std::string generateReport(const std::string& format) const;
    
    // === Convenience Methods ===
    std::string getExperimentCommand(const std::string& experiment) const;
    std::string getBrainCommand(const std::string& brainType) const;
    
protected:
    void initializeOptions();
    void parseArguments();
    void parseFromEnvironment();
    void setupOptionHandlers();
    
    // Command handlers
    bool handleHelpCommand(const std::vector<std::string>& args);
    bool handleVersionCommand(const std::vector<std::string>& args);
    bool handleConfigCommand(const std::vector<std::string>& args);
    bool handleExperimentCommand(const std::vector<std::string>& args);
    bool handleOutputCommand(const std::vector<std::string>& args);
    bool handleLogCommand(const std::vector<std::string>& args);
    bool handleMemoryCommand(const std::vector<std::string>& args);
    bool handleNeuromodCommand(const std::vector<std::string>& args);
    bool handleCheckpointCommand(const std::vector<std::string>& args);
    bool handleVisualizationCommand(const std::vector<std::string>& args);
    
    // Option setters
    void setBasicOptions(const std::vector<std::string>& args);
    void setBrainOptions(const std::vector<std::string>& args);
    void setMemoryOptions(const std::vector<std::string>& args);
    void setNeuromodOptions(const std::vector<std::string>& args);
    void setExperimentOptions(const std::vector<std::string>& args);
    void setOutputOptions(const std::vector<std::string>& args);
    void setPerformanceOptions(const std::vector<std::string>& args);
    void setAdvancedOptions(const std::vector<std::string>& args);
    
    // Validation helpers
    bool validateLogLevel(const std::string& level) const;
    bool validateBrainType(const std::string& type) const;
    bool validateReportFormat(const std::string& format) const;
    bool validateExperiment(const std::string& experiment) const;
    
    // String utilities
    static std::string toLower(const std::string& str);
    static bool stringStartsWith(const std::string& str, const std::string& prefix);
    static bool stringContains(const std::string& str, const std::string& substring);
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Option validation
    static bool isValidFile(const std::string& filename);
    static bool isValidParameter(const std::string& param);
    
private:
    CLIOptions options_;
    std::map<std::string, std::function<bool(const std::vector<std::string>&)>> commandHandlers_;
    std::map<std::string, std::function<void(const std::string&)>> optionSetters_;
    std::map<std::string, std::string> optionValues_;
    std::vector<std::string> validationErrors_;
};

// === Global CLI instance ===
inline AdvancedCLI& getCLI() {
    static AdvancedCLI cli;
    return cli;
}

// === Convenience functions ===

// Parse command line arguments
inline bool parseArguments(int argc, char* argv[]) {
    return getCLI().processCommand(std::string(argv[0]));
}

// Get CLI options
inline const CLIOptions& getCLIOptions() {
    return getCLI().getOptions();
}

// Set a CLI option
inline void setCLIOption(const std::string& option, const std::string& value) {
    getCLI().setOption(option, value);
}

// Check if CLI option is set
inline bool hasCLIOption(const std::string& option) {
    return getCLI().hasOption(option);
}

// Execute a CLI command
inline bool executeCLICommand(const std::string& command, 
                             const std::vector<std::string>& args = {}) {
    return getCLI().executeCommand(command, args);
}

// Show help
inline void showHelp() {
    getCLI().showHelp();
}

// Show version
inline void showVersion() {
    getCLI().showVersion();
}

// Validate CLI options
inline bool validateCLI() {
    return getCLI().validateOptions();
}

// Get validation errors
inline std::vector<std::string> getCLIValidationErrors() {
    return getCLI().getValidationErrors();
}

// Generate CLI report
inline std::string generateCLIOutput(const std::string& format = "text") {
    return getCLI().generateReport(format);
}

} // namespace nlm