#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <iostream>

// Include log level from logger
#include "../Logger/Logger.hpp"
#include "../Config/Config.hpp"
struct CLIOptions {
    // Basic simulation settings
    std::string configFile = "configs/default.cfg";
    uint64_t simulationSteps = 1000;
    uint64_t neuronCount = 500;
    uint64_t regionCount = 1;
    float connectionProbability = 0.15f;
    
    // Plasticity settings
    float stdpLTPWeight = 0.02f;
    float stdpLTDWeight = 0.015f;
    float stdpTau = 20.0f;
    
    // Structural plasticity settings
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Logging and output
    std::string logLevel = "info";
    bool verbose = false;
    bool quiet = false;
    std::string outputFile = "";
    
    // Testing modes
    bool runTests = false;
    bool runBenchmark = false;
    bool runPhase3Demo = false;
    bool runPhase4Demo = false;
    bool runPhase5Demo = false;
    bool runPhase6Demo = false;
    
    // Development and debugging
    bool enableDebug = false;
    bool profile = false;
    uint64_t checkpointInterval = 100;
    std::string checkpointPath = "";
    
    // Advanced options
    double timestep = 0.001;
    bool useGPU = false;
    uint32_t maxMemoryGB = 4;
    
    // Display help
    bool showHelp = false;
    bool showVersion = false;
};

// CLI argument parser
class CLIParser {
public:
    CLIParser(int argc, char** argv);
    ~CLIParser() = default;
    
    // Parse arguments
    CLIOptions parse();
    
    // Get the help text
    static std::string getHelpText();
    
    // Get version information
    static std::string getVersion();
    
    // Validate options
    static bool validate(const CLIOptions& options, std::string& errorMessage);
    
    // Load options from file
    static bool loadOptionsFromFile(const std::string& filepath, CLIOptions& options);
    
    // Save options to file
    static bool saveOptionsToFile(const std::string& filepath, const CLIOptions& options);
    
private:
    int argc_;
    char** argv_;
    
    // Parse basic simulation options
    void parseSimulationOptions(const std::string& arg, CLIOptions& options);
    
    // Parse plasticity options
    void parsePlasticityOptions(const std::string& arg, CLIOptions& options);
    
    // Parse test/demo options
    void parseDemoOptions(const std::string& arg, CLIOptions& options);
    
    // Parse logging/output options
    void parseOutputOptions(const std::string& arg, CLIOptions& options);
    
    // Parse development/debug options
    void parseDebugOptions(const std::string& arg, CLIOptions& options);
    
    // Convert string to log level
    static LogLevel stringToLogLevel(const std::string& level);
    
    // Convert log level to string
    static std::string logLevelToString(LogLevel level);
    
    // Trim whitespace from string
    static std::string trim(const std::string& str);
    
    // Convert string to lowercase
    static std::string toLower(const std::string& str);
};