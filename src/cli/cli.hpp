#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

namespace nlm {

// Command-line options structure
struct CLIOptions {
    // Verbosity level
    int verbosity = 0;
    
    // Output format
    std::string outputFormat = "console";
    
    // Execution mode
    std::string mode = "run";
    
    // Configuration file
    std::string configFile;
    
    // Test selection
    std::vector<std::string> testNames;
    std::vector<std::string> testCategories;
    std::string testPattern;
    
    // Benchmark parameters
    bool benchmark = false;
    std::string benchmarkMetrics;
    std::string benchmarkOutput;
    
    // Profile parameters
    bool profile = false;
    std::string profileOutput;
    
    // Validation parameters
    bool validate = false;
    std::string validationSchema;
    
    // Execution parameters
    int maxSteps = 1000;
    double timeLimit = 0.0;
    int64_t randomSeed = 42;
    
    // Output options
    bool quiet = false;
    bool progress = false;
    bool verbose = false;
    
    // Error handling
    bool continueOnError = false;
    std::string errorOutput;
    
    // Help and version
    bool help = false;
    bool version = false;
    bool listTests = false;
    bool listCategories = false;
};

// Test information structure
struct TestInfo {
    std::string name;
    std::string category;
    std::string description;
    std::string command;
};

// Command-line parser class
class CommandLineParser {
public:
    CommandLineParser() = default;
    ~CommandLineParser() = default;
    
    // Parse command line arguments
    bool parse(int argc, char** argv, CLIOptions& options);
    
    // Get help text
    std::string getHelp() const;
    
    // Get version info
    std::string getVersion() const;
    
    // Get list of available tests
    std::vector<TestInfo> getAvailableTests() const;
    
    // Get list of available categories
    std::vector<std::string> getAvailableCategories() const;
    
    // Validate options
    bool validateOptions(const CLIOptions& options) const;
    
    // Process options and prepare execution
    bool prepareExecution(const CLIOptions& options);
    
private:
    // Internal parsing methods
    void setupCommands();
    void setupOptions();
    void setupTests();
    
    // Argument parsing helpers
    bool parseVerbosity(const std::string& arg);
    bool parseOutputFormat(const std::string& arg);
    bool parseMode(const std::string& arg);
    bool parseConfigFile(const std::string& arg);
    bool parseTest(const std::string& arg);
    bool parseTestCategory(const std::string& arg);
    bool parseTestPattern(const std::string& arg);
    bool parseBenchmark(const std::string& arg);
    bool parseProfile(const std::string& arg);
    bool parseValidate(const std::string& arg);
    
    // Error handling
    void handleError(const std::string& error);
    void handleWarning(const std::string& warning);
    
    // Option storage
    std::unordered_map<std::string, std::string> optionValues;
    std::vector<std::string> positionalArgs;
    
    // Metadata
    std::string programName;
    std::string versionInfo;
    std::vector<TestInfo> availableTests;
    std::vector<std::string> availableCategories;
};

// Result structure for command execution
struct CommandResult {
    int exitCode;
    std::string output;
    bool success;
    std::string errorMessage;
    double executionTime;
};

// CLI executor class
class CLIExecutor {
public:
    CLIExecutor() = default;
    ~CLIExecutor() = default;
    
    // Execute command based on options
    CommandResult execute(const CLIOptions& options);
    
    // Execute single test
    CommandResult executeTest(const std::string& testName);
    
    // Execute benchmark
    CommandResult executeBenchmark(const CLIOptions& options);
    
    // Execute profile
    CommandResult executeProfile(const CLIOptions& options);
    
    // Execute validation
    CommandResult executeValidation(const CLIOptions& options);
    
    // Output formatter
    class OutputFormatter {
    public:
        static std::string format(const std::string& format, const std::string& data);
        static std::string formatJSON(const std::string& data);
        static std::string formatCSV(const std::string& data);
        static std::string formatHTML(const std::string& data);
        static std::string formatConsole(const std::string& data);
    };
    
private:
    // Execute test run mode
    CommandResult executeRunMode(const CLIOptions& options);
    
    // Execute benchmark mode
    CommandResult executeBenchmarkMode(const CLIOptions& options);
    
    // Execute profile mode
    CommandResult executeProfileMode(const CLIOptions& options);
    
    // Execute validation mode
    CommandResult executeValidationMode(const CLIOptions& options);
    
    // Load tests based on options
    std::vector<std::string> loadTests(const CLIOptions& options);
    
    // Filter tests
    std::vector<std::string> filterTests(const std::vector<std::string>& allTests, 
                                        const CLIOptions& options) const;
};

} // namespace nlm