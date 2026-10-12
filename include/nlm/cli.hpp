#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>
#include <filesystem>
#include <unordered_map>
#include <typeindex>
#include <regex>

namespace nlm {

// File format enumeration (from existing Config system)
enum class FileFormat {
    UNKNOWN,
    SIMPLE,
    JSON,
    YAML
};

// Configuration value types (simplified)
using ConfigValue = std::variant<int, int64_t, double, bool, std::string>;

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// CLI application configuration
struct CLIConfig {
    std::string configFile;
    std::string outputFile;
    int steps = 0;
    double timeLimit = 0.0;
    double pauseDuration = 0.0;
    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    bool test = false;
    bool validate = false;
    std::string infoType; // "config" or "brain"
    std::string brainFile;
    std::string infoField;
    std::string benchmarkType; // "time" or "iterations"
    int benchmarkDuration = 0;
    int benchmarkIterations = 0;
    std::string createType; // "brain" or "world"
    std::string createOutput;
    std::unordered_map<std::string, std::string> createOptions;
    bool strictValidation = false;
};

// Output format enum
enum class OutputFormat {
    JSON,
    CSV,
    CONSOLE,
    YAML
};

// CLI error types
enum class CLIErrorType {
    None,
    FileNotFound,
    InvalidFormat,
    ValidationError,
    ConfigurationError,
    CommandError,
    RuntimeError
};

// CLI result class
struct CLIResult {
    bool success = true;
    CLIErrorType errorType = CLIErrorType::None;
    std::string errorMessage;
    std::string output;
    std::vector<std::string> validationErrors;
    std::vector<std::string> validationWarnings;
};

// Validation result (from existing Config system)
struct ValidationResult {
    bool isError() const { return false; }
    bool isWarning() const { return false; }
    bool isCritical() const { return false; }
};

// CLI Application interface
class CLIApp {
public:
    virtual ~CLIApp() = default;
    
    // Add subcommand
    virtual void addSubcommand(const std::string& name, const std::string& description, std::function<bool(const CLIConfig&)>) = 0;
    
    // Set default options
    virtual void addOption(const std::string& name, const std::string& description, std::function<void(CLIConfig&)>) = 0;
    
    // Parse arguments
    virtual bool parse(int argc, char* argv[]) = 0;
    
    // Run the application
    virtual int run() = 0;
    
    // Get configuration
    virtual const CLIConfig& getConfig() const = 0;
    
    // Get result
    virtual const CLIResult& getResult() const = 0;
};

// Configuration entry structure (simplified)
struct ConfigEntry {
    std::string key;
    ConfigValue value;
};

// Validation result
using ValidationResult = std::vector<ValidationResult>;

// CLI utility functions
class CLIUtils {
public:
    // File format detection
    static FileFormat detectFileFormat(const std::string& filepath);
    
    // Configuration file creation
    static bool createConfigFile(const std::string& type, const std::string& output, const std::unordered_map<std::string, std::string>& options);
    
    // Configuration validation
    static CLIResult validateConfig(const std::string& filepath, bool strict);
    
    // Configuration information display
    static CLIResult displayConfigInfo(const std::string& filepath, const std::string& field = "");
    
    // Brain information display
    static CLIResult displayBrainInfo(const std::string& filepath);
    
    // Output formatting
    static std::string formatOutput(const std::string& data, OutputFormat format);
    
    // Progress reporting
    static void updateProgress(int current, int total, const std::string& message);
    
    // Error handling
    static std::string errorToString(CLIErrorType error);
    
    // Command line argument parsing
    static std::unordered_map<std::string, std::string> parseArguments(int argc, char* argv[]);
};

// Create CLI application
CLIApp& createCLIApp();

} // namespace nlm