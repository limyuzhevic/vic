#ifndef NLM_CLI_FRAMEWORK_H
#define NLM_CLI_FRAMEWORK_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <climits>
#include <cfloat>

namespace nlm {

/**
 * @brief Exception class for CLI-related errors
 */
class CLIException : public std::runtime_error {
public:
    explicit CLIException(const std::string& message) 
        : std::runtime_error("CLI Error: " + message) {}
};

/**
 * @brief Structure to hold CLI options
 */
struct NLMCliOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputDir = ".";
    std::string logLevel = "INFO";
    bool verbose = false;
    bool debug = false;
    std::string mode = "full";
    bool saveResults = false;
    int maxSteps = 10000;
    std::string checkpointPath = "checkpoint.bin";
    bool enableCheckpointing = false;
    bool enableReplay = false;
    bool enableDevelopment = false;
    std::string experimentMode = "default";
    int neuronCount = 500;
    int regionCount = 2;
    float connectionProbability = 0.1f;
    std::string randomSeed = "42";
    float stdpLTPWeight = 0.01f;
    float stdpLTDWeight = 0.012f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Phase 3 specific
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    
    // Phase 4 specific
    int numTrials = 50;
    
    // Phase 6 specific
    int demoSteps = 2000;
    
    // Help/usage
    bool showHelp = false;
    bool showVersion = false;
};

/**
 * @brief Parse command line arguments into NLMCliOptions
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return Parsed NLMCliOptions
 * @throws CLIException on parsing errors
 */
NLMCliOptions parseCommandLine(int argc, char** argv);

/**
 * @brief Print help information for CLI usage
 */
void printHelp(const std::string& programName = "nlm");

/**
 * @brief Print version information
 */
void printVersion();

/**
 * @brief Validate CLI options for consistency
 * 
 * @param options Options to validate
 * @throws CLIException on validation errors
 */
void validateOptions(const NLMCliOptions& options);

/**
 * @brief Format a key-value pair for output
 * 
 * @param key The key
 * @param value The value
 * @return Formatted string
 */
std::string formatOption(const std::string& key, const std::string& value);

/**
 * @brief Format a key-value pair for output (numeric)
 * 
 * @param key The key
 * @param value The value
 * @param precision Decimal precision
 * @return Formatted string
 */
template<typename T>
std::string formatOption(const std::string& key, T value, int precision = 3) {
    std::stringstream ss;
    ss << std::left << std::setw(30) << key << ": " 
       << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

/**
 * @brief Safe string to integer conversion
 * 
 * @param str String to convert
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return Converted integer
 * @throws CLIException on conversion error or value out of range
 */
int parseInt(const std::string& str, int min = INT_MIN, int max = INT_MAX);

/**
 * @brief Safe string to float conversion
 * 
 * @param str String to convert
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return Converted float
 * @throws CLIException on conversion error or value out of range
 */
float parseFloat(const std::string& str, float min = -FLT_MAX, float max = FLT_MAX);

/**
 * @brief Parse boolean flag from string
 * 
 * @param str String to parse
 * @return Parsed boolean
 */
bool parseBool(const std::string& str);

/**
 * @brief Load configuration from file with error handling
 * 
 * @param configFile Path to configuration file
 * @return true if loaded successfully, false otherwise
 */
bool loadConfigFile(const std::string& configFile);

/**
 * @brief Log CLI options for debugging
 * 
 * @param options Options to log
 */
void logOptions(const NLMCliOptions& options);

/**
 * @brief Create output directory if it doesn't exist
 * 
 * @param path Directory path
 * @return true if directory exists or was created, false on error
 */
bool ensureOutputDirectory(const std::string& path);

} // namespace nlm

#endif // NLM_CLI_FRAMEWORK_H