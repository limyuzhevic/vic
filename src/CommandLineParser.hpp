// NLM (熙然) - Neural Learning Machine
// CommandLineParser - Modular CLI Argument Parsing
//
// Handles all command-line arguments for NLM Phase 2 simulation

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>
#include <functional>
#include <map>

namespace nlm {

class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv);
    ~CommandLineParser() = default;
    
    // Disable copying, enable moving
    CommandLineParser(const CommandLineParser&) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;
    CommandLineParser(CommandLineParser&&) noexcept = default;
    CommandLineParser& operator=(CommandLineParser&&) noexcept = default;
    
    // Help system
    bool hasHelp() const { return hasHelpFlag_; }
    const std::string& getHelpText() const { return helpText_; }
    
    // Verbosity control
    bool getVerbose() const { return verbose_; }
    
    // Configuration
    const std::string& getConfigFile() const { return configFile_; }
    bool hasConfig() const { return !configFile_.empty(); }
    
    // Test selection
    std::vector<std::string> getTestNames() const { return testNames_; }
    bool shouldRunAllTests() const { return runAllTests_; }
    std::unordered_set<std::string> getTestSet() const { return testSet_; }
    
    // Output configuration
    const std::string& getOutputFile() const { return outputFile_; }
    bool hasOutputFile() const { return !outputFile_.empty(); }
    
    // Experiment parameters
    int64_t getSeed() const { return seed_; }
    bool hasSeed() const { return hasSeed_; }
    
    // Custom overrides
    int getStepsOverride() const { return stepsOverride_; }
    bool hasStepsOverride() const { return hasStepsOverride_; }
    
    float getCurrentOverride() const { return currentOverride_; }
    bool hasCurrentOverride() const { return hasCurrentOverride_; }
    
    // Advanced options
    const std::string& getExperimentType() const { return experimentType_; }
    bool enableStatsAnalysis() const { return enableStatsAnalysis_; }
    bool enableBenchmark() const { return enableBenchmark_; }
    bool enableProfile() const { return enableProfile_; }
    
    // Help generation
    void setHelpText(const std::string& text) { helpText_ = text; }
    
private:
    // Parse arguments
    void parseArguments(int argc, char** argv);
    
    // Validate arguments
    void validateArguments();
    
    // Flag management
    void registerFlag(const std::string& flag, bool& flagVar, const std::string& shortForm = "");
    void registerStringFlag(const std::string& flag, std::string& flagVar, const std::string& defaultValue = "", const std::string& shortForm = "");
    void registerIntFlag(const std::string& flag, int64_t& flagVar, const std::string& shortForm = "");
    void registerFloatFlag(const std::string& flag, float& flagVar, const std::string& shortForm = "");
    
    // Test name parsing
    void parseTestNames(const std::string& testNamesStr);
    void parseTestNamesShort(const std::string& testNamesStr);
    
    // Argument extraction
    std::string extractFlagValue(const std::string& arg) const;
    bool extractFlag(const std::string& arg) const;
    
    // Member variables
    bool hasHelpFlag_ = false;
    std::string helpText_ = "";
    bool verbose_ = false;
    std::string configFile_ = "configs/default.cfg";
    std::vector<std::string> testNames_;
    std::unordered_set<std::string> testSet_;
    bool runAllTests_ = false;
    std::string outputFile_ = "";
    int64_t seed_ = 42;
    bool hasSeed_ = false;
    int stepsOverride_ = 0;
    bool hasStepsOverride_ = false;
    float currentOverride_ = 0.0f;
    bool hasCurrentOverride_ = false;
    std::string experimentType_ = "";
    bool enableStatsAnalysis_ = false;
    bool enableBenchmark_ = false;
    bool enableProfile_ = false;
};
