// CommandLineOptions.hpp
// CLI argument parsing and validation for NLM

#ifndef NLM_COMMANDLINEOPTIONS_HPP
#define NLM_COMMANDLINEOPTIONS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace nlm {

// Command line options structure for NLM
struct CommandLineOptions {
    // Basic options
    bool helpRequested = false;                    // --help flag
    bool versionRequested = false;                 // --version flag
    bool verbose = false;                         // --verbose flag
    bool benchmark = false;                       // --benchmark flag
    bool quiet = false;                            // --quiet flag
    bool debug = false;                           // --debug flag
    
    // Configuration
    std::string configFile = "configs/default.cfg";  // --config file
    
    // Test execution options
    std::string testMode = "all";                 // --test-mode: all/connectivity/plasticity/stdp
    bool runConnectivityTest = false;              // --connectivity
    bool runPlasticityTest = false;                // --plasticity
    bool runStdpTest = false;                     // --stdp
    bool runLearningTest = false;                  // --learning
    
    // Performance and simulation options
    uint64_t neuronCount = 500;                   // --neurons
    uint64_t maxSteps = 1000;                     // --steps
    double simulationTimestep = 0.001;            // --timestep
    uint64_t randomSeed = 42;                     // --seed
    
    // Plasticity options
    bool enableHebbian = true;                    // --hebbian
    bool enableStdp = true;                       // --stdp
    bool enableStructuralPlasticity = true;        // --structural
    
    // Development options
    bool enableDevelopment = false;                // --development
    float synaptogenesisRate = 0.0001f;           // --synaptogenesis-rate
    float pruningRate = 0.00001f;                 // --pruning-rate
    
    // Output options
    std::string outputFile;                       // --output
    std::string benchmarkFile;                    // --benchmark-output
    bool jsonOutput = false;                      // --json
    bool csvOutput = false;                       // --csv
    
    // Validation flags
    bool validateConfig = false;                   // --validate-config
    bool listConfigs = false;                      // --list-configs
    
    // Parse command line arguments
    static CommandLineOptions parse(int argc, char** argv);
    
    // Validate option combinations
    bool validate() const;
    
    // Print help information
    static void printHelp(const std::string& programName);
    
    // Print version information
    static void printVersion();
    
    // Get test mode name
    std::string getTestModeName() const;
    
    // Check if a specific test should be run
    bool shouldRunTest(const std::string& testName) const;
    
    // Get list of tests to run based on mode
    std::vector<std::string> getTestsToRun() const;
    
private:
    // Parse test mode string
    void parseTestMode(const std::string& mode);
    
    // Parse configuration file
    void parseConfigFile(const std::string& filename);
    
    // Load configuration from environment variables
    void parseEnvironmentVariables();
    
    // Helper functions
    std::string toLower(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
};

} // namespace nlm

#endif // NLM_COMMANDLINEOPTIONS_HPP