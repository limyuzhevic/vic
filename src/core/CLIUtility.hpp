// CLI Utility for NLM Demo Executables
// Provides enhanced command-line argument parsing, help system, and configuration management

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <memory>
#include "core/ConfigurationManager.hpp"

namespace nlm {

// Command-line argument definition
struct CLIArgument {
    std::string name;
    char shortOption;
    std::string longOption;
    std::string description;
    std::string defaultValue;
    std::string type;  // "flag", "string", "int", "float", "choice"
    std::vector<std::string> choices;
    bool required;
    
    CLIArgument() : shortOption(0), required(false) {}
    
    CLIArgument(const std::string& name, char shortOpt, const std::string& longOpt,
                const std::string& desc, const std::string& def = "",
                const std::string& typeName = "string", bool req = false)
        : name(name), shortOption(shortOpt), longOption(longOpt), 
          description(desc), defaultValue(def), type(typeName), required(req) {}
};

// Command-line option parser
class CLIParser {
public:
    CLIParser(int argc, char** argv);
    
    // Add argument definition
    void addArgument(const CLIArgument& arg);
    
    // Parse arguments
    bool parse();
    
    // Get argument value
    template<typename T>
    T getArgument(const std::string& name, const T& defaultValue = T()) const;
    
    // Check if argument is present
    bool hasArgument(const std::string& name) const;
    
    // Get all argument names
    std::vector<std::string> getArgumentNames() const;
    
    // Help and usage
    void printHelp() const;
    void printUsage(const std::string& programName) const;
    
    // Configuration integration
    bool applyToConfiguration(ConfigurationManager& configManager) const;
    
    // Validation
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Statistics
    size_t getArgumentCount() const;
    size_t getUnknownArgsCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void parseArguments();
    void printUnknownArgsWarning() const;
};

// Common CLI options for NLM demos
namespace DemoOptions {
    // Basic options
    extern const CLIArgument Help;
    extern const CLIArgument Version;
    extern const CLIArgument LoadConfig;
    extern const CLIArgument SaveConfig;
    
    // Configuration presets
    extern const CLIArgument Preset;
    extern const CLIArgument NeuronCount;
    extern const CLIArgument RegionCount;
    extern const CLIArgument ConnectionProbability;
    extern const CLIArgument MaxSteps;
    extern const CLIArgument SimulationTimestep;
    
    // Performance options
    extern const CLIArgument EnableCheckpointing;
    extern const CLIArgument EnableReplay;
    extern const CLIArgument EnableDevelopment;
    extern const CLIArgument EnableNeuromodulation;
    
    // Output options
    extern const CLIArgument Verbose;
    extern const CLIArgument Quiet;
    extern const CLIArgument OutputFile;
    extern const CLIArgument OutputFormat;
    
    // Benchmark options
    extern const CLIArgument BenchmarkType;
    extern const CLIArgument BenchmarkDuration;
    extern const CLIArgument BenchmarkIterations;
    
    // Environment options
    extern const CLIArgument RandomSeed;
    extern const CLIArgument EnvironmentWidth;
    extern const CLIArgument EnvironmentHeight;
}

// Demo-specific option builders
class DemoOptionsBuilder {
public:
    static std::vector<CLIArgument> buildCommonOptions();
    static std::vector<CLIArgument> buildPhase3Options();
    static std::vector<CLIArgument> buildPhase4Options();
    static std::vector<CLIArgument> buildPhase5Options();
    static std::vector<CLIArgument> buildPhase6Options();
    static std::vector<CLIArgument> buildBenchmarkOptions();
};

// Enhanced main function wrapper
int enhancedMain(int argc, char** argv,
                 std::function<int(CLIParser&)> demoMainFunction,
                 const std::string& programDescription = "");

// Common demo entry point
int demoEntryPoint(int argc, char** argv,
                   std::function<int(CLIParser&)> runDemo,
                   const std::string& demoName,
                   const std::string& version = "1.0.0");

// Configuration preset shortcuts
namespace Presets {
    void quickTest(CLIParser& parser);
    void fullTest(CLIParser& parser);
    void performance(CLIParser& parser);
    void development(CLIParser& parser);
    void production(CLIParser& parser);
}

// Output formatters
class OutputFormatter {
public:
    static void printBanner(const std::string& title);
    static void printSection(const std::string& title);
    static void printResult(const std::string& name, float value, float max = 1.0f);
    static void printMetrics(const std::map<std::string, float>& metrics);
    static void printProgress(const std::string& message, double progress);
};

// Progress reporting
class ProgressReporter {
public:
    ProgressReporter(const std::string& title, int totalSteps);
    
    void update(int currentStep, const std::string& status = "");
    void updateProgress(double progress, const std::string& status = "");
    void setMessage(const std::string& message);
    void finish();
    
    bool isActive() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Result collector for benchmarks and experiments
struct ExperimentResult {
    std::string name;
    double duration;
    std::map<std::string, float> metrics;
    std::map<std::string, std::string> parameters;
    std::string status;
    std::chrono::steady_clock::time_point timestamp;
    
    ExperimentResult(const std::string& n) : name(n), timestamp(std::chrono::steady_clock::now()) {}
};

// Experiment runner with enhanced reporting
class ExperimentRunner {
public:
    ExperimentRunner(const std::string& name);
    
    void runExperiment(const std::function<void()>& experimentFunc);
    void addResult(const ExperimentResult& result);
    void printResults(bool verbose = false) const;
    void exportResults(const std::string& filename) const;
    
    const std::vector<ExperimentResult>& getResults() const;
    
private:
    std::vector<ExperimentResult> results;
};

// Global CLI instance
CLIParser& getGlobalCLI();

// Convenience functions
inline void printDemoHelp(const std::string& demoName) {
    std::cout << "=== " << demoName << " Help ===" << std::endl;
    std::cout << "Use --help for detailed usage information." << std::endl;
}

inline void printConfigurationHelp() {
    nlm::getConfigurationManager().printHelp();
}

inline bool isValidConfiguration(ConfigurationManager& config) {
    auto validation = config.validate();
    return validation.valid;
}

} // namespace nlm
