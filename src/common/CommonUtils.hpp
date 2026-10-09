// NLM Common Utilities
// Common functionality for all NLM executables
// Provides: CLI argument parsing, configuration handling, output management

#ifndef NLM_COMMON_UTILS_HPP
#define NLM_COMMON_UTILS_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cctype>

namespace nlm {

// Command line argument parsing structure
struct CLIArgs {
    std::string configFile;
    std::string outputFile;
    std::string logLevel;
    std::string mode;
    bool verbose;
    bool help;
    bool version;
    bool quiet;
    
    // Executable-specific arguments
    std::map<std::string, std::string> customArgs;
    
    // Default constructor
    CLIArgs() : verbose(false), help(false), version(false), quiet(false) {}
};

// Configuration override from CLI
struct ConfigOverride {
    std::string key;
    std::string value;
    std::string type; // "int", "float", "string", "bool"
};

// Output formats
enum class OutputFormat {
    CONSOLE,
    JSON,
    CSV,
    TEXT
};

// Utility functions for CLI parsing
class CLIUtils {
public:
    static void printHelp(const std::string& programName, const std::string& description);
    static void printVersion();
    static CLIArgs parseArgs(int argc, char* argv[], 
                           const std::vector<std::string>& allowedFlags,
                           const std::map<std::string, std::string>& flagDescriptions);
    static bool parseFlag(int& i, int argc, char* argv[], 
                         const std::string& flag, std::string& value);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static OutputFormat parseOutputFormat(const std::string& formatStr);
    static std::string formatTime(const std::chrono::system_clock::time_point& time);
    static std::string getCurrentTimestamp();
};

// Output management class
class OutputManager {
public:
    static void setOutputFormat(OutputFormat format);
    static OutputFormat getOutputFormat();
    static void setOutputFile(const std::string& filename);
    static std::ostream& getOutputStream();
    static void printHeader(const std::string& title);
    static void printSection(const std::string& title);
    static void printResult(const std::string& name, float value, float max = 1.0f);
    static void printResult(const std::string& name, double value, double max = 1.0f);
    static void printResult(const std::string& name, int64_t value);
    static void printResult(const std::string& name, const std::string& value);
    static void printProgress(const std::string& message, int current, int total);
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printDebug(const std::string& message);
    
private:
    static OutputFormat currentFormat;
    static std::ofstream outputFileStream;
    static std::ostream& getOutputStreamInternal();
};

// Performance timing utilities
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    void reset() { startTime = std::chrono::high_resolution_clock::now(); }
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    double elapsedMilliseconds() const { return elapsed() * 1000.0; }
    double elapsedSeconds() const { return elapsed(); }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// String formatting utilities
class StringUtils {
public:
    static std::string format(const std::string& format, ...);
    static std::string leftAlign(const std::string& str, size_t width);
    static std::string rightAlign(const std::string& str, size_t width);
    static std::string centerAlign(const std::string& str, size_t width);
    static std::string toString(float value, int precision = 3);
    static std::string toString(double value, int precision = 3);
    static std::string toString(int64_t value);
    static std::string toString(bool value);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static bool contains(const std::string& str, const std::string& substring);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static std::string escapeJSON(const std::string& str);
};

// Experiment result structure for common use
struct ExperimentResult {
    std::string name;
    double startTime;
    double endTime;
    double duration;
    float totalReward;
    float averageReward;
    float successRate;
    std::map<std::string, std::string> metadata;
    
    std::string toJSON() const;
    std::string summary() const;
};

// Common configuration loading utilities
class ConfigUtils {
public:
    static bool loadFromFile(std::shared_ptr<Config> config, const std::string& filename);
    static bool loadFromCLI(std::shared_ptr<Config> config, const std::map<std::string, std::string>& cliArgs);
    static std::string getValueAsString(const std::shared_ptr<Config>& config, 
                                       const std::string& key,
                                       const std::string& defaultValue = "");
    static int64_t getValueAsInt(const std::shared_ptr<Config>& config,
                                const std::string& key,
                                int64_t defaultValue = 0);
    static double getValueAsDouble(const std::shared_ptr<Config>& config,
                                  const std::string& key,
                                  double defaultValue = 0.0);
    static float getValueAsFloat(const std::shared_ptr<Config>& config,
                                const std::string& key,
                                float defaultValue = 0.0f);
    static bool getValueAsBool(const std::shared_ptr<Config>& config,
                              const std::string& key,
                              bool defaultValue = false);
};

// Main execution mode enumeration
enum class ExecutionMode {
    UNKNOWN,
    HELP,
    VERSION,
    TEST,      // Run quick validation tests
    RUN,       // Run full demonstration
    BENCHMARK, // Performance benchmarking
    BATCH,     // Batch processing
    PROFILE    // Profiling mode
};

} // namespace nlm

#endif // NLM_COMMON_UTILS_HPP
