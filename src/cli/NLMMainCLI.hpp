#ifndef NLM_MAIN_CLI_H
#define NLM_MAIN_CLI_H

#include "main.cpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace nlm {

class NLMMainCLI {
public:
    NLMMainCLI();
    ~NLMMainCLI();
    
    // Main entry point
    int run(int argc, char** argv);
    
    // Parse arguments and execute commands
    int parseAndExecute(int argc, char** argv);
    
    // Command implementations
    int executeConfig(const std::vector<std::string>& args);
    int executeRun(const std::vector<std::string>& args);
    int executeSave(const std::vector<std::string>& args);
    int executeLoad(const std::vector<std::string>& args);
    int executeReset(const std::vector<std::string>& args);
    int executeStatus(const std::vector<std::string>& args);
    int executeTest(const std::vector<std::string>& args);
    int executeExplore(const std::vector<std::string>& args);
    int executeLog(const std::vector<std::string>& args);
    
    // Helper functions
    int runBrainSimulation(const std::map<std::string, std::string>& options);
    int runTests(const std::string& testType);
    int saveBrainState(const std::string& filename);
    int loadBrainState(const std::string& filename);
    int printBrainStatus();
    int printBrainInfo();
    int exploreBrainState();
    int configureLogging(const std::string& level, const std::string& filename);
    
    // Configuration validation
    bool validateBrainConfig(const std::map<std::string, std::string>& options);
    
    // Display functions
    void printBanner() const;
    void printHelp() const;
    void printVersion() const;
    void printBrainSummary() const;
    
private:
    // Global state
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Logger> logger;
    bool verbose;
    bool quiet;
    bool interactive;
    std::string configFile;
    std::string outputFile;
    std::string logLevel;
    std::string logFile;
    bool brainInitialized;
    
    // Command line options
    struct CLIOptions {
        std::string command;
        std::vector<std::string> args;
        std::map<std::string, std::string> flags;
        std::map<std::string, std::string> options;
        bool help;
        bool version;
        bool verbose;
        bool quiet;
        bool interactive;
        std::string configFile;
        std::string outputFile;
        std::string logLevel;
        std::string logFile;
    };
    
    CLIOptions options;
    
    // Initialize global components
    void initializeGlobalComponents();
    void cleanupGlobalComponents();
    
    // Configuration file handling
    bool loadConfigurationFromFile(const std::string& filename);
    bool saveConfigurationToFile(const std::string& filename);
    
    // Command parsing
    void parseArguments(int argc, char** argv);
    void parseFlags(const std::vector<std::string>& args);
    void parseOptions(const std::vector<std::string>& args);
    
    // Validation
    bool validateOptions() const;
    bool validateBrainParameters() const;
    
    // Output formatting
    void printSuccess(const std::string& message) const;
    void printError(const std::string& message) const;
    void printWarning(const std::string& message) const;
    void printInfo(const std::string& message) const;
    void printDebug(const std::string& message) const;
    
    // File operations
    bool fileExists(const std::string& filename) const;
    std::string getAbsolutePath(const std::string& filename) const;
    
    // Configuration management
    void setDefaultConfig();
    void updateConfigFromOptions();
    
    // Brain management
    bool initializeBrain();
    void resetBrain();
    bool saveBrainToFile(const std::string& filename);
    bool loadBrainFromFile(const std::string& filename);
    
    // Test execution
    int runBasicTests();
    int runPlasticityTests();
    int runSTDPTests();
    int runIntegrationTests();
    
    // Configuration commands
    int handleConfigCommand(const std::vector<std::string>& args);
    int handleConfigSet(const std::vector<std::string>& args);
    int handleConfigGet(const std::vector<std::string>& args);
    int handleConfigList();
    int handleConfigSave(const std::vector<std::string>& args);
    int handleConfigLoad(const std::vector<std::string>& args);
    
    // Output commands
    int handleOutputCommand(const std::vector<std::string>& args);
    int handleOutputStatus();
    int handleOutputInfo();
    int handleOutputSummary();
    
    // Test commands
    int handleTestCommand(const std::vector<std::string>& args);
    int handleTestBasic();
    int handleTestPlasticity();
    int handleTestSTDP();
    int handleTestIntegration();
    int handleTestAll();
    
    // Exploration commands
    int handleExploreCommand(const std::vector<std::string>& args);
    int handleExploreBrain();
    int handleExploreStats();
    int handleExploreNetwork();
    int handleExploreConnections();
    
    // Logging commands
    int handleLogCommand(const std::vector<std::string>& args);
    int handleLogSetLevel(const std::string& level);
    int handleLogSetFile(const std::string& filename);
    int handleLogSetOutput(const std::string& output);
    int handleLogGetStatus();
    
    // Interactive mode
    int runInteractiveMode();
    void printInteractivePrompt() const;
    std::string getInteractiveInput() const;
    
    // Utility functions
    std::string trim(const std::string& str) const;
    bool stringToBool(const std::string& str) const;
    double stringToDouble(const std::string& str) const;
    int64_t stringToInt64(const std::string& str) const;
    float stringToFloat(const std::string& str) const;
    
    // Color output for terminal
    static const std::string COLOR_RED;
    static const std::string COLOR_GREEN;
    static const std::string COLOR_YELLOW;
    static const std::string COLOR_BLUE;
    static const std::string COLOR_MAGENTA;
    static const std::string COLOR_CYAN;
    static const std::string COLOR_WHITE;
    static const std::string COLOR_RESET;
    static const std::string COLOR_BOLD;
    
    // ANSI escape codes
    static const std::string ANSI_RED;
    static const std::string ANSI_GREEN;
    static const std::string ANSI_YELLOW;
    static const std::string ANSI_BLUE;
    static const std::string ANSI_MAGENTA;
    static const std::string ANSI_CYAN;
    static const std::string ANSI_WHITE;
    static const std::string ANSI_BOLD;
    static const std::string ANSI_RESET;
    
    // Print colored output
    void printColored(const std::string& color, const std::string& message) const;
    void printSuccessColored(const std::string& message) const;
    void printErrorColored(const std::string& message) const;
    void printWarningColored(const std::string& message) const;
    void printInfoColored(const std::string& message) const;
    void printDebugColored(const std::string& message) const;
};

} // namespace nlm

#endif // NLM_MAIN_CLI_H