#pragma once

/**
 * Command-line interface for NLM brain interaction
 * 
 * Provides command-line tools for:
 * - Running brain experiments
 * - Configuring brain parameters
 * - Saving/loading brain states
 * - Monitoring brain performance
 * - Visualizing brain state
 */

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>

namespace nlm {

/**
 * Command-line interface for brain interaction
 */
class BrainCLI {
public:
    BrainCLI();
    ~BrainCLI();
    
    /**
     * Run the CLI interface
     */
    int run(int argc, char* argv[]);
    
    /**
     * Display help information
     */
    void showHelp() const;
    
    /**
     * Display version information
     */
    void showVersion() const;
    
    /**
     * Display current brain status
     */
    void showBrainStatus() const;
    
    /**
     * Load brain configuration from file
     */
    bool loadConfig(const std::string& filename);
    
    /**
     * Save brain configuration to file
     */
    bool saveConfig(const std::string& filename) const;
    
    /**
     * Run a simple test
     */
    int runTest(const std::string& testName);
    
    /**
     * Run full experiment
     */
    int runExperiment(const std::string& experimentName);
    
    /**
     * Set configuration parameter
     */
    bool setParameter(const std::string& key, const std::string& value);
    
    /**
     * Get configuration parameter
     */
    std::string getParameter(const std::string& key) const;
    
    /**
     * Display configuration
     */
    void showConfiguration() const;
    
    /**
     * Save current brain state
     */
    bool saveBrainState(const std::string& filename) const;
    
    /**
     * Load brain state
     */
    bool loadBrainState(const std::string& filename);
    
    /**
     * Display help for a specific command
     */
    void showCommandHelp(const std::string& command) const;
    
    /**
     * Check if command exists
     */
    bool hasCommand(const std::string& command) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Command handling
    void processCommand(const std::string& command, const std::vector<std::string>& args);
    bool executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Command implementations
    bool handleHelp(const std::vector<std::string>& args);
    bool handleVersion(const std::vector<std::string>& args);
    bool handleStatus(const std::vector<std::string>& args);
    bool handleLoad(const std::vector<std::string>& args);
    bool handleSave(const std::vector<std::string>& args);
    bool handleSet(const std::vector<std::string>& args);
    bool handleGet(const std::vector<std::string>& args);
    bool handleConfig(const std::vector<std::string>& args);
    bool handleTest(const std::vector<std::string>& args);
    bool handleExperiment(const std::vector<std::string>& args);
    bool handleExit(const std::vector<std::string>& args);
    
    // Display formatting
    void displayBanner() const;
    void displayPrompt() const;
    void displayError(const std::string& error) const;
    void displayInfo(const std::string& info) const;
    void displaySuccess(const std::string& success) const;
    
    // Interactive mode
    void runInteractiveMode();
    
    // Batch mode
    void runBatchMode(const std::vector<std::string>& commands);
    
    // Configuration management
    void initializeConfiguration();
    
    // Brain management
    void initializeBrain();
    void cleanupBrain();
    
    // Command history
    void addToHistory(const std::string& command);
    void showHistory(size_t count = 10) const;
    
    // Command completion
    std::vector<std::string> getCommandCompletions(const std::string& partial) const;
};

/**
 * Command-line argument parser
 */
class CommandLineParser {
public:
    CommandLineParser(int argc, char* argv[]);
    
    /**
     * Get program name
     */
    const std::string& getProgramName() const { return programName_; }
    
    /**
     * Get positional arguments
     */
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs_; }
    
    /**
     * Get named arguments
     */
    std::string getNamedArg(const std::string& name) const;
    
    /**
     * Check if named argument exists
     */
    bool hasNamedArg(const std::string& name) const;
    
    /**
     * Get all named arguments
     */
    const std::map<std::string, std::string>& getAllNamedArgs() const { return namedArgs_; }
    
    /**
     * Display usage information
     */
    void showUsage() const;
    
    /**
     * Get command line help text
     */
    std::string getHelpText() const;
    
private:
    std::string programName_;
    std::vector<std::string> positionalArgs_;
    std::map<std::string, std::string> namedArgs_;
    
    void parseArguments(int argc, char* argv[]);
};

/**
 * Configuration file handler
 */
class ConfigFileHandler {
public:
    ConfigFileHandler();
    ~ConfigFileHandler();
    
    /**
     * Load configuration from file
     */
    bool load(const std::string& filename, std::map<std::string, std::string>& config);
    
    /**
     * Save configuration to file
     */
    bool save(const std::string& filename, const std::map<std::string, std::string>& config);
    
    /**
     * Validate configuration
     */
    bool validate(const std::map<std::string, std::string>& config, std::vector<std::string>& errors) const;
    
    /**
     * Get default configuration
     */
    static std::map<std::string, std::string> getDefaultConfiguration();
    
    /**
     * Format configuration for display
     */
    static std::string formatConfiguration(const std::map<std::string, std::string>& config);
    
    /**
     * Parse configuration value to appropriate type
     */
    template<typename T>
    static T parseValue(const std::string& value, const T& defaultValue = T());
    
    /**
     * Check if file exists
     */
    static bool fileExists(const std::string& filename);
};

/**
 * Interactive command prompt
 */
class CommandPrompt {
public:
    CommandPrompt();
    ~CommandPrompt();
    
    /**
     * Set prompt text
     */
    void setPrompt(const std::string& prompt) { prompt_ = prompt; }
    
    /**
     * Get prompt text
     */
    const std::string& getPrompt() const { return prompt_; }
    
    /**
     * Set prompt color
     */
    void setPromptColor(const std::string& color) { promptColor_ = color; }
    
    /**
     * Read command from input
     */
    std::string readLine();
    
    /**
     * Display completion suggestions
     */
    void showCompletions(const std::vector<std::string>& completions);
    
    /**
     * Set history file
     */
    void setHistoryFile(const std::string& filename) { historyFile_ = filename; }
    
    /**
     * Enable/disable history
     */
    void setHistoryEnabled(bool enabled) { historyEnabled_ = enabled; }
    
private:
    std::string prompt_;
    std::string promptColor_;
    std::string historyFile_;
    bool historyEnabled_;
    size_t historyPosition_;
};

/**
 * Output formatter for command-line output
 */
class OutputFormatter {
public:
    enum class Format {
        Plain,
        Table,
        JSON,
        CSV
    };
    
    OutputFormatter();
    ~OutputFormatter();
    
    /**
     * Set output format
     */
    void setFormat(Format format) { format_ = format; }
    
    /**
     * Print formatted output
     */
    void print(const std::string& text) const;
    void println(const std::string& text) const;
    
    /**
     * Print formatted table
     */
    void printTable(const std::vector<std::vector<std::string>>& rows,
                   const std::vector<std::string>& headers) const;
    
    /**
     * Print formatted metrics
     */
    void printMetrics(const std::map<std::string, double>& metrics) const;
    
    /**
     * Print formatted status
     */
    void printStatus(const std::map<std::string, std::string>& status) const;
    
    /**
     * Enable/disable colors
     */
    void setColorsEnabled(bool enabled) { colorsEnabled_ = enabled; }
    
    /**
     * Set output file
     */
    void setOutputFile(const std::string& filename);
    
    /**
     * Reset to default output
     */
    void reset();

private:
    Format format_;
    bool colorsEnabled_;
    std::ofstream outputFile_;
    bool fileOutputEnabled_;
    
    /**
     * Apply color codes
     */
    std::string applyColor(const std::string& text, const std::string& color) const;
    
    /**
     * Get color code for format
     */
    std::string getColorCode(const std::string& color) const;
};

/**
 * Command-line tool for running experiments
 */
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    /**
     * Run experiment with configuration
     */
    int run(const std::map<std::string, std::string>& config);
    
    /**
     * Get available experiments
     */
    std::vector<std::string> getAvailableExperiments() const;
    
    /**
     * Get experiment configuration template
     */
    std::map<std::string, std::string> getExperimentConfig(const std::string& experimentName) const;
    
    /**
     * Validate experiment configuration
     */
    bool validateExperimentConfig(const std::string& experimentName,
                                  const std::map<std::string, std::string>& config,
                                  std::vector<std::string>& errors) const;
    
    /**
     * Get experiment results formatter
     */
    static std::string formatResults(const std::map<std::string, double>& results,
                                     const std::string& format = "table");
    
    /**
     * Export experiment results
     */
    bool exportResults(const std::string& filename,
                      const std::map<std::string, double>& results,
                      const std::string& format = "csv");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * Load experiment-specific configuration
     */
    bool loadExperimentConfig(const std::string& experimentName,
                             std::map<std::string, std::string>& config);
    
    /**
     * Run experiment with given configuration
     */
    int executeExperiment(const std::string& experimentName,
                         const std::map<std::string, std::string>& config);
    
    /**
     * Monitor experiment progress
     */
    void monitorProgress(const std::map<std::string, double>& progress);
};

} // namespace nlm
