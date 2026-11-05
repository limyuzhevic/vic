// NLM Common CLI Utilities
// Provides unified command line interface for all NLM demo executables

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>

namespace nlm {

struct CLIResult {
    bool success;
    std::string message;
    std::string error;
};

struct CLICommand {
    std::string name;
    std::string description;
    std::string longDescription;
    std::function<std::string()> help;
    std::function<bool(const std::string&)> validate;
    std::function<std::string(const std::string&)> transform;
};

class CLIParser {
public:
    CLIParser(const std::string& programName);
    
    // Add a command line argument
    void addArgument(const std::string& name, const std::string& description, 
                    const std::string& longDescription = "",
                    const std::string& defaultValue = "",
                    bool required = false,
                    std::function<bool(const std::string&)> validator = nullptr);
    
    // Add a flag (boolean)
    void addFlag(const std::string& name, const std::string& description,
                 const std::string& longDescription = "");
    
    // Parse command line arguments
    CLIResult parse(int argc, char* argv[]);
    
    // Get parsed value
    std::string getValue(const std::string& name) const;
    bool getFlag(const std::string& name) const;
    
    // Check if argument was provided
    bool hasArgument(const std::string& name) const;
    
    // Get help text
    std::string generateHelp() const;
    
    // Print help to stdout
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Get program name
    std::string getProgramName() const { return programName; }
    
    // Set version string
    void setVersion(const std::string& version) { versionInfo = version; }
    
    // Set description
    void setDescription(const std::string& desc) { description = desc; }

private:
    std::string programName;
    std::string versionInfo;
    std::string description;
    std::map<std::string, std::string> arguments;
    std::map<std::string, bool> flags;
    std::map<std::string, std::string> argumentDescriptions;
    std::map<std::string, std::string> argumentLongDescriptions;
    std::map<std::string, std::string> argumentDefaults;
    std::map<std::string, bool> argumentRequired;
    std::map<std::string, std::function<bool(const std::string&)>> argumentValidators;
    std::map<std::string, std::function<std::string(const std::string&)>> argumentTransformers;
    std::map<std::string, std::string> flagDescriptions;
    std::map<std::string, std::string> flagLongDescriptions;
    
    void printError(const std::string& error) const;
    std::string formatHelp(const std::string& arg, const std::string& desc, 
                          const std::string& longDesc, const std::string& defaultVal,
                          bool required) const;
};

// Common NLM options that all demos can use
struct NLMCommonOptions {
    std::string configFile;
    std::string outputFile;
    std::string logLevel;
    bool verbose;
    bool quiet;
    bool help;
    bool version;
    int numEpisodes;
    int stepsPerEpisode;
    int numTrials;
    double timestep;
    std::string randomSeed;
    bool enablePlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableRewardModulation;
    std::string experimentType;
    int maxSteps;
    bool enableCheckpointing;
    bool enableReplay;
    std::string checkpointFile;
};

// Parse common NLM options from command line
NLMCommonOptions parseCommonOptions(int argc, char* argv[], const std::string& programName);

// Print common NLM options help
void printCommonOptionsHelp(const std::string& programName);

// Common logging configuration
void setupCommonLogging(const std::string& logLevel, bool verbose, bool quiet);

// Common configuration loading
bool loadCommonConfig(const std::string& configFile, std::map<std::string, std::string>& configValues);

// Format a value with units for display
std::string formatValue(double value, const std::string& unit = "");

// Format time for display
std::string formatTime(double seconds);

// Common simulation loop with metrics collection
struct SimulationMetrics {
    double startTime;
    double endTime;
    uint64_t totalSteps;
    uint64_t totalSpikes;
    double averageFiringRate;
    double energyConsumed;
    double rewardReceived;
    uint64_t explorationActions;
    uint64_t exploitationActions;
    uint64_t totalActions;
    double developmentProgress;
    double memoryUtilization;
};

SimulationMetrics runSimulationLoop(std::function<void(uint64_t, double)> stepFunction,
                                   uint64_t numSteps,
                                   double timestep = 0.001,
                                   std::function<void(double, const SimulationMetrics&)> progressCallback = nullptr);

// Export metrics to file
bool exportMetrics(const std::string& filename, const SimulationMetrics& metrics,
                   const std::map<std::string, double>& additionalMetrics = {});

} // namespace nlm

#endif // NLM_CLI_H