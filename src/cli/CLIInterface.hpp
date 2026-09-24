#pragma once

#include <string>

namespace nlm {

class CLIInterface {
public:
    CLIInterface();
    ~CLIInterface() = default;
    
    // Disable copying
    CLIInterface(const CLIInterface&) = delete;
    CLIInterface& operator=(const CLIInterface&) = delete;
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Display help information
    void showHelp() const;
    
    // Display version information
    void showVersion() const;
    
    // List available experiments
    void listExperiments() const;
    
    // Run standard simulation
    bool runSimulation();
    
    // Run specific experiment
    bool runExperiment(const std::string& experimentName, size_t steps);
    
    // Run in interactive mode
    bool runInteractiveMode();
    
private:
    void showInteractiveHelp() const;
    
    // Configuration options
    bool verbose;
    bool quiet;
    bool enableVerboseLogging;
    bool showStatistics;
    bool runInInteractiveMode;
    
    std::string configFile;
    std::string checkpointFile;
    std::string loadFile;
    std::string experimentName;
    
    uint64_t randomSeed;
    size_t neuronCount;
    size_t regionCount;
    size_t maxSteps;
    
    float stdpLtp;
    float stdpLtd;
    float stdpTau;
};

} // namespace nlm
