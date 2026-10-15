#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

namespace nlm {

class CLIParser {
public:
    struct TestOptions {
        // Test selection and filtering
        std::vector<std::string> testNames;
        std::vector<std::string> testCategories;
        std::vector<std::string> testTags;
        std::vector<std::string> testPhases;
        std::vector<std::string> excludeTests;
        std::string testOrder; // "alphabetical", "category", "phase", "custom"
        std::string executionMode; // "sequential", "parallel"
        
        // Execution control
        int maxParallelTests;
        int timeoutSeconds;
        bool runAllTests;
        bool skipDisabledTests;
        
        // Configuration
        std::string configFile;
        std::map<std::string, std::string> runtimeConfig;
        
        // Output and reporting
        std::string outputDirectory;
        std::string outputFormat; // "json", "xml", "csv", "plaintext"
        std::string reportFormat;
        bool verbose;
        bool quiet;
        bool quietProgress;
        int progressInterval;
        
        // Brain and simulation
        bool resetBrainBetweenTests;
        bool saveBrainState;
        std::string brainSavePath;
        bool restoreBrainState;
        std::string brainLoadPath;
        
        // Plugin system
        std::vector<std::string> enabledPlugins;
        std::vector<std::string> disabledPlugins;
        std::map<std::string, std::string> pluginConfig;
        
        // Scenario and batch execution
        std::string scenarioFile;
        std::string scenarioName;
        bool runScenarios;
        bool dryRun;
        
        // Miscellaneous
        bool showHelp;
        bool showVersion;
        bool validateConfig;
        std::string logLevel;
        std::string logFile;
        
        // Default constructor
        TestOptions() : 
            maxParallelTests(1),
            timeoutSeconds(0),
            runAllTests(false),
            skipDisabledTests(true),
            verbose(false),
            quiet(false),
            quietProgress(false),
            progressInterval(100),
            resetBrainBetweenTests(true),
            saveBrainState(false),
            restoreBrainState(false),
            runScenarios(false),
            dryRun(false),
            showHelp(false),
            showVersion(false),
            validateConfig(false) {}
    };
    
    // Parse command line arguments
    static TestOptions parse(int argc, char** argv);
    
    // Display help information
    static void printHelp(const std::string& programName);
    static void printVersion();
    
    // Validate parsed options
    static bool validateOptions(const TestOptions& options, std::vector<std::string>& errors);
    
    // Get option descriptions
    static std::map<std::string, std::string> getOptionDescriptions();
    static std::map<std::string, std::string> getTestOptionDescriptions();
    
private:
    static void registerOptions();
    static void setupArgumentParser();
};

} // namespace nlm
