// NLM Improved Command-Line Interface
// Phase 6: Enhanced Command-Line Argument Parsing
// Provides comprehensive configuration options for both users and advanced users

#ifndef CMD_INTERFACE_HPP
#define CMD_INTERFACE_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

// Command-line interface configuration
struct CLIConfig {
    std::string configFile;           // Configuration file path
    bool verbose;                     // Enable verbose logging
    bool quiet;                       // Suppress non-essential output
    std::string logFile;              // Log file path
    int logLevel;                     // Logging level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)
    std::string outputDir;            // Output directory for results
    bool saveResults;                 // Save simulation results
    std::string resultFormat;         // Result format: json, txt, csv
    int maxSteps;                     // Maximum simulation steps
    double timeLimit;                 // Time limit in seconds (0 = no limit)
    bool showProgress;                // Show progress updates
    std::string experimentType;       // Type of experiment: "demo", "integration", "full"
    std::string environment;          // Environment type: "simple", "complex", "custom"
    
    // Advanced options
    bool enableRewardModulation;      // Enable reward-based learning
    bool enableStructuralPlasticity;  // Enable synaptic growth/pruning
    bool enableDevelopment;           // Enable developmental processes
    bool enableCuriosity;             // Enable curiosity-driven exploration
    bool enableNoveltyDetection;      // Enable novelty detection
    bool enablePrediction;            // Enable predictive coding
    bool enableCheckpointing;         // Enable checkpoint saving
    bool enableReplay;                // Enable memory replay
    
    // Performance tuning
    int numThreads;                   // Number of threads for parallel processing
    double threadGranularity;         // Thread granularity for work distribution
    bool useSIMD;                     // Enable SIMD optimizations
    
    // Advanced neural parameters
    float plasticityLearningRate;     // General plasticity learning rate
    float stdpLTPWeight;              // STDP LTP weight
    float stdpLTDWeight;              // STDP LTD weight
    float stdpTimeConstant;           // STDP time constant
    float developmentSynaptogenesisRate; // Synaptogenesis rate
    float developmentPruningRate;     // Pruning rate
    
    // Memory parameters
    size_t workingMemoryCapacity;     // Working memory capacity
    size_t episodicMemoryMaxEpisodes; // Maximum episodic memory episodes
    
    // Neuromodulation parameters
    float dopamineBaseline;           // Baseline dopamine level
    float dopamineBaselineCuriosity;  // Curiosity-related baseline
    float dopamineBaselineNovelty;    // Novelty-related baseline
    
    // Reward parameters
    float rewardDiscountFactor;       // Reward discount factor
    float explorationBonus;           // Exploration bonus for novel states
    float exploitationPenalty;       // Penalty for over-exploitation
    
    // Environment parameters
    std::string environmentName;      // Environment name
    int environmentWidth;             // Environment width
    int environmentHeight;            // Environment height
    float visionWidth;                // Vision width
    float visionHeight;               // Vision height
    
    CLIConfig() : verbose(false), quiet(false), logLevel(2), // Default INFO
                  saveResults(false), maxSteps(10000), timeLimit(0.0),
                  showProgress(false), experimentType("demo"),
                  environment("simple"), enableRewardModulation(false),
                  enableStructuralPlasticity(false), enableDevelopment(false),
                  enableCuriosity(false), enableNoveltyDetection(false),
                  enablePrediction(false), enableCheckpointing(false),
                  enableReplay(false), numThreads(0), threadGranularity(0.1),
                  useSIMD(true), plasticityLearningRate(0.01f),
                  stdpLTPWeight(0.01f), stdpLTDWeight(0.012f),
                  stdpTimeConstant(20.0f), developmentSynaptogenesisRate(0.001f),
                  developmentPruningRate(0.0001f), workingMemoryCapacity(100),
                  episodicMemoryMaxEpisodes(1000), dopamineBaseline(0.1f),
                  dopamineBaselineCuriosity(0.0f), dopamineBaselineNovelty(0.0f),
                  rewardDiscountFactor(0.99f), explorationBonus(0.1f),
                  exploitationPenalty(0.01f), environmentWidth(10),
                  environmentHeight(10), visionWidth(8.0f), visionHeight(8.0f) {}
};

// Enhanced command-line parser
class CLIInterface {
public:
    CLIInterface(int argc, char** argv);
    ~CLIInterface();
    
    // Parse command line arguments
    bool parseArguments();
    
    // Get the final configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    CLIConfig getCLIConfig() const { return cliConfig; }
    
    // Get help text
    static std::string getHelpText();
    static std::string getAdvancedHelpText();
    
    // Get available configuration keys
    static std::vector<std::string> getAvailableKeys();
    static std::unordered_map<std::string, std::string> getKeyDescriptions();
    
    // Validation
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    int argc;
    char** argv;
    std::shared_ptr<Config> config;
    CLIConfig cliConfig;
    
    // Helper functions
    bool parseStandardArgs();
    bool parseAdvancedArgs();
    bool parseFileArgs();
    void setDefaults();
    bool loadConfigFile();
    bool applyAdvancedOptions();
    
    // Argument parsing helpers
    bool parseStringArg(const std::string& arg, std::string& value);
    bool parseIntArg(const std::string& arg, int& value);
    bool parseFloatArg(const std::string& arg, float& value);
    bool parseBoolArg(const std::string& arg, bool& value);
    bool parseKeyValueArg(const std::string& arg, std::string& key, std::string& value);
    
    // Option parsing
    bool parseShortOption(const std::string& arg);
    bool parseLongOption(const std::string& arg);
    
    // Configuration helpers
    void applyConfigToCLI(const std::shared_ptr<Config>& cfg);
    void applyCLIToConfig(std::shared_ptr<Config>& cfg);
    
    // Error handling
    void printError(const std::string& error) const;
    void printWarning(const std::string& warning) const;
    void printInfo(const std::string& info) const;
};

} // namespace nlm

#endif // CMD_INTERFACE_HPP