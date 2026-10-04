#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Error/ErrorHandling.hpp"

namespace nlm {

// Command line interface for NLM
class CLI {
public:
    CLI() = default;
    ~CLI() = default;
    
    // Parse command line arguments
    bool parseArgs(int argc, char** argv);
    
    // Run the CLI application
    int run();
    
    // Get help text
    std::string getHelp() const;
    
    // Get version information
    std::string getVersion() const;
    
    // Get configuration from command line
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Execute a specific command
    int executeCommand(const std::string& command, const std::vector<std::string>& args);
    
private:
    // Command definition
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<int(const std::vector<std::string>&)> callback;
        std::vector<std::string> examples;
    };
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Available commands
    std::vector<Command> commands_;
    
    // Global options
    bool verbose_ = false;
    bool quiet_ = false;
    std::string logFile_;
    bool help_ = false;
    bool version_ = false;
    
    // Command line arguments
    std::vector<std::string> args_;
    
    // Initialize default commands
    void initializeCommands();
    
    // Parse global options
    bool parseGlobalOptions(int argc, char** argv);
    
    // Parse command and arguments
    std::pair<std::string, std::vector<std::string>> parseCommand(const std::string& arg);
    
    // Show help
    void showHelp(const std::string& command = "") const;
    
    // Show version
    void showVersion() const;
    
    // Command implementations
    int cmdRun(const std::vector<std::string>& args);
    int cmdConfig(const std::vector<std::string>& args);
    int cmdExperiment(const std::vector<std::string>& args);
    int cmdBenchmark(const std::vector<std::string>& args);
    int cmdInfo(const std::vector<std::string>& args);
    int cmdTest(const std::vector<std::string>& args);
    int cmdProfile(const std::vector<std::string>& args);
    int cmdReplay(const std::vector<std::string>& args);
    int cmdSave(const std::vector<std::string>& args);
    int cmdLoad(const std::vector<std::string>& args);
    
    // Configuration commands
    int configList(const std::vector<std::string>& args);
    int configSet(const std::vector<std::string>& args);
    int configGet(const std::vector<std::string>& args);
    int configReset(const std::vector<std::string>& args);
    
    // Experiment commands
    int experimentRun(const std::vector<std::string>& args);
    int experimentList(const std::vector<std::string>& args);
    int experimentStatus(const std::vector<std::string>& args);
    
    // Utility functions
    std::string formatTime(std::chrono::system_clock::time_point time) const;
    std::string formatBytes(size_t bytes) const;
    std::string formatDuration(std::chrono::milliseconds duration) const;
    
    // Validation functions
    bool validateArgs(const std::vector<std::string>& args) const;
    bool validateCommand(const std::string& command) const;
    
    // Error handling
    void handleError(const Error& error);
    
    // Logging setup
    void setupLogging();
    
    // Configuration setup
    void setupConfig();
};

// Advanced command-line utilities
class CLIUtils {
public:
    // Progress bar for long-running operations
    class ProgressBar {
    public:
        ProgressBar(size_t total, const std::string& label = "Progress");
        ~ProgressBar();
        
        void update(size_t current, const std::string& status = "");
        void increment(const std::string& status = "");
        void setStatus(const std::string& status);
        
        void finish();
        
    private:
        size_t total_;
        std::atomic<size_t> current_;
        std::string label_;
        std::string status_;
        std::mutex mutex_;
        bool finished_ = false;
        bool showPercentage_ = true;
    };
    
    // Table formatter for structured output
    class TableFormatter {
    public:
        TableFormatter(size_t columns, const std::vector<std::string>& headers);
        
        void addRow(const std::vector<std::string>& row);
        void addRow(const std::map<std::string, std::string>& row);
        
        std::string format() const;
        void saveToFile(const std::string& filepath) const;
        
    private:
        size_t columns_;
        std::vector<std::string> headers_;
        std::vector<std::vector<std::string>> rows_;
        std::vector<size_t> columnWidths_;
        
        void calculateColumnWidths();
        std::string padString(const std::string& str, size_t width, bool rightAlign = false) const;
    };
    
    // Configuration file utilities
    class ConfigFileUtils {
    public:
        // Generate sample configuration file
        static bool generateSampleConfig(const std::string& filepath);
        
        // Validate configuration file
        static bool validateConfig(const std::string& filepath, std::string& errorMessage);
        
        // Merge configuration files
        static bool mergeConfigs(const std::string& baseFile, const std::string& overrideFile,
                               const std::string& outputFile);
        
        // Export configuration to different formats
        static bool exportConfig(const std::string& inputFile, const std::string& outputFile,
                               const std::string& format = "yaml");
    };
    
    // Experiment utilities
    class ExperimentUtils {
    public:
        // Experiment parameter validation
        static bool validateExperimentParams(const std::map<std::string, std::string>& params,
                                            std::string& errorMessage);
        
        // Generate experiment configuration
        static std::map<std::string, std::string> generateExperimentConfig(
            const std::string& experimentType, const std::string& name);
        
        // List available experiments
        static std::vector<std::string> listExperiments();
        
        // Get experiment metadata
        static std::map<std::string, std::string> getExperimentMetadata(const std::string& experimentType);
    };
    
    // Performance monitoring utilities
    class PerformanceUtils {
    public:
        // Measure execution time
        template<typename Func, typename... Args>
        static auto measureTime(Func&& func, Args&&... args) {
            auto start = std::chrono::high_resolution_clock::now();
            auto result = func(std::forward<Args>(args)...);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            return std::make_pair(result, duration.count());
        }
        
        // Start performance profiling
        static void startProfiling(const std::string& sessionName);
        
        // Stop performance profiling
        static std::map<std::string, double> stopProfiling();
        
        // Generate performance report
        static std::string generatePerformanceReport(const std::map<std::string, double>& metrics);
    };
    
    // System utilities
    class SystemUtils {
    public:
        // Get system information
        static std::map<std::string, std::string> getSystemInfo();
        
        // Check system resources
        static std::map<std::string, double> getSystemResources();
        
        // Validate system requirements for NLM
        static bool validateSystemRequirements(std::string& errorMessage);
        
        // Get available memory
        static size_t getAvailableMemory();
        
        // Get CPU core count
        static size_t getCPUCoreCount();
    };
};

// Interactive CLI mode
class InteractiveCLI {
public:
    InteractiveCLI(std::shared_ptr<Config> config);
    ~InteractiveCLI();
    
    // Run interactive mode
    void run();
    
    // Show welcome message
    void showWelcome() const;
    
    // Show prompt
    void showPrompt() const;
    
    // Process command line input
    void processCommand(const std::string& command);
    
    // List available commands
    void listCommands() const;
    
    // Get command history
    std::vector<std::string> getCommandHistory() const { return history_; }
    
private:
    std::shared_ptr<Config> config_;
    std::vector<std::string> history_;
    size_t historyIndex_ = 0;
    bool running_ = true;
    
    // Command history management
    void addToHistory(const std::string& command);
    
    // Auto-completion
    std::vector<std::string> getAutoComplete(const std::string& partialCommand) const;
    
    // Command execution
    void executeInteractiveCommand(const std::string& command);
};

// Batch mode for automated execution
class BatchCLI {
public:
    BatchCLI(std::shared_ptr<Config> config);
    ~BatchCLI();
    
    // Execute batch commands from file
    int executeBatchFile(const std::string& filepath);
    
    // Execute batch commands from string
    int executeBatchString(const std::string& batchCommands);
    
    // Get execution results
    std::vector<std::string> getExecutionResults() const { return results_; }
    
private:
    std::shared_ptr<Config> config_;
    std::vector<std::string> results_;
    bool verbose_ = false;
    
    // Parse batch commands
    std::vector<std::pair<std::string, std::vector<std::string>>> parseBatchCommands(const std::string& batchCommands);
    
    // Execute single batch command
    int executeBatchCommand(const std::string& command, const std::vector<std::string>& args);
};

// Global CLI instance for backward compatibility
inline CLI& getCLI() {
    static CLI cli;
    return cli;
}

} // namespace nlm
