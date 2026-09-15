#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <atomic>

namespace nlm {

// CLI argument structure
struct CLIArgument {
    std::string name;
    std::string shortName;
    std::string description;
    std::string defaultValue;
    bool required;
    bool hasValue;
    std::string valueType;
};

// CLI options structure
struct CLIOptions {
    std::string configFile;
    std::string logLevel;
    std::string logFile;
    bool verbose;
    bool quiet;
    bool debug;
    bool jsonOutput;
    bool yamlOutput;
    bool help;
    bool version;
    std::string outputFormat;
    int performanceMode; // 0=basic, 1=detailed, 2=full
    int maxMemory;
    double timeout;
    std::string experimentName;
    std::vector<std::string> plugins;
    std::map<std::string, std::string> customArgs;
};

// CLI parser class
class CLIParser {
public:
    CLIParser();
    virtual ~CLIParser() = default;
    
    // Parse command line arguments
    bool parse(int argc, char** argv, CLIOptions& options);
    
    // Display help information
    void printHelp(const std::string& programName);
    
    // Display version information
    void printVersion();
    
    // Validate options
    bool validateOptions(const CLIOptions& options);
    
    // Get error messages
    std::string getError() const { return errorMessage; }
    
protected:
    std::string errorMessage;
    
    // Built-in option definitions
    std::vector<CLIArgument> builtinArguments;
    
    // Custom argument handlers
    std::map<std::string, std::function<bool(const std::string&)>> argumentHandlers;
    
    // Help text templates
    const std::string helpText;
    const std::string versionText;
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    PerformanceMonitor();
    virtual ~PerformanceMonitor() = default;
    
    // Start timing
    void startTimer(const std::string& name);
    
    // Stop timing and record
    double stopTimer(const std::string& name);
    
    // Get elapsed time for named timer
    double getElapsed(const std::string& name) const;
    
    // Print performance report
    void printReport() const;
    
    // Get memory usage
    size_t getMemoryUsage() const;
    
    // Get CPU usage
    double getCPUUsage() const;
    
    // Check if monitoring is enabled
    bool isEnabled() const { return enabled; }
    
    // Set monitoring mode
    void setMode(int mode) { this->mode = mode; }
    
    // Set output format
    void setOutputFormat(const std::string& format) { this->outputFormat = format; }
    
private:
    bool enabled;
    int mode; // 0=basic, 1=detailed, 2=full
    std::string outputFormat;
    
    struct TimerInfo {
        std::chrono::high_resolution_clock::time_point start;
        double elapsed;
        bool running;
    };
    
    std::map<std::string, TimerInfo> timers;
    
    // Implementation details...
};

// Configuration diff tool
class ConfigDiff {
public:
    ConfigDiff();
    virtual ~ConfigDiff() = default;
    
    // Compare two configuration objects
    std::string compare(const std::string& file1, const std::string& file2,
                        const std::string& format = "auto");
    
    // Generate configuration diff report
    std::string generateDiffReport(const std::map<std::string, std::string>& changes);
    
    // Highlight differences in output
    std::string highlightChanges(const std::string& content,
                                 const std::vector<std::pair<size_t, size_t>>& diffs);
    
    // Export configuration
    bool exportConfig(const std::string& filepath, const CLIOptions& options);
    
    // Import configuration
    bool importConfig(const std::string& filepath, CLIOptions& options);
    
    // Configuration validation
    bool validateConfig(const std::string& filepath, const std::string& format = "auto");
};

// Output formatter
class OutputFormatter {
public:
    OutputFormatter();
    virtual ~OutputFormatter() = default;
    
    // Format for console output
    std::string formatConsole(const std::string& content, const CLIOptions& options);
    
    // Format for JSON output
    std::string formatJSON(const std::string& content, const CLIOptions& options);
    
    // Format for YAML output
    std::string formatYAML(const std::string& content, const CLIOptions& options);
    
    // Format for human-readable output
    std::string formatHumanReadable(const std::string& content, const CLIOptions& options);
    
    // Format for progress indicators
    std::string formatProgress(double progress, const std::string& message = "");
    
    // Format for performance metrics
    std::string formatMetrics(const PerformanceMonitor& monitor);
};

// Configuration preset manager
class ConfigPresetManager {
public:
    ConfigPresetManager();
    virtual ~ConfigPresetManager() = default;
    
    // Load preset configuration
    bool loadPreset(const std::string& presetName, CLIOptions& options);
    
    // Save preset configuration
    bool savePreset(const std::string& presetName, const CLIOptions& options);
    
    // List available presets
    std::vector<std::string> listPresets() const;
    
    // Delete preset
    bool deletePreset(const std::string& presetName);
    
    // Apply default configuration
    void applyDefaults(CLIOptions& options);
    
private:
    std::string presetsDir;
};

} // namespace nlm
