// NLM Command-Line Tool
// Advanced command-line interface for NLM (熙然) neural brain simulation
// Version: 0.1.0
// Provides advanced functionality for configuration management, 
// experiment control, and system monitoring

#ifndef NLM_CMD_H
#define NLM_CMD_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

namespace nlm {
namespace cmd {

// Forward declarations
class Config;
class Brain;
class Logger;

// Command-line tool options
struct ToolOptions {
    std::string configFile;
    std::string outputFile;
    std::string checkpointFile;
    std::string logFile;
    std::string experimentType;
    std::string mode;
    
    // Simulation parameters
    int simulationSteps;
    double timeStep;
    int neuronCount;
    int regionCount;
    
    // Learning parameters
    bool enablePlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableRewardModulation;
    
    // Debug and profiling
    bool enableDebug;
    bool enableProfiling;
    bool verbose;
    
    // Output options
    bool quiet;
    bool jsonOutput;
    bool metricsOnly;
    
    ToolOptions() : configFile("configs/default.cfg"),
                   outputFile(""),
                   checkpointFile(""),
                   logFile(""),
                   experimentType("basic"),
                   mode("run"),
                   simulationSteps(1000),
                   timeStep(0.001),
                   neuronCount(1000),
                   regionCount(1),
                   enablePlasticity(true),
                   enableDevelopment(true),
                   enableCuriosity(true),
                   enableRewardModulation(true),
                   enableDebug(false),
                   enableProfiling(false),
                   verbose(true),
                   quiet(false),
                   jsonOutput(false),
                   metricsOnly(false) {}
};

// Main command-line tool class
class NLMCommandTool {
private:
    ToolOptions options;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Logger> logger;
    
    // Command-line argument parsing
    void parseArguments(int argc, char** argv);
    void parseOption(const std::string& key, const std::string& value);
    void printHelp() const;
    void printVersion() const;
    
    // Configuration management
    bool loadConfiguration();
    bool saveConfiguration() const;
    void applyOptionsToConfig();
    
    // Brain management
    bool initializeBrain();
    void cleanupBrain();
    void runExperiment();
    void runMonitoringSession();
    void runValidationSuite();
    
    // Output and reporting
    void generateMetricsReport() const;
    void generateJSONReport() const;
    void generateDebugOutput() const;
    void writeCheckpoint() const;
    bool loadCheckpoint();
    
    // Utility functions
    void setupLogging() const;
    void printBanner() const;
    void printConfiguration() const;
    static std::string getTimestamp();
    double getSystemMemoryUsage() const;
    
    // Test functions (declared for external access)
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
    
public:
    NLMCommandTool(int argc, char** argv);
    ~NLMCommandTool() = default;
    
    bool run() {
        try {
            if (!loadConfiguration()) {
                std::cerr << "Error: Failed to load configuration" << std::endl;
                return false;
            }
            
            applyOptionsToConfig();
            
            if (!initializeBrain()) {
                std::cerr << "Error: Failed to initialize brain" << std::endl;
                return false;
            }
            
            if (options.mode == "run") {
                runExperiment();
            } else if (options.mode == "monitor") {
                runMonitoringSession();
            } else if (options.mode == "validate") {
                runValidationSuite();
            } else {
                std::cerr << "Error: Unknown mode: " << options.mode << std::endl;
                return false;
            }
            
            if (!options.checkpointFile.empty() && options.mode != "validate") {
                writeCheckpoint();
            }
            
            generateMetricsReport();
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }
};

} // namespace cmd
} // namespace nlm

#endif // NLM_CMD_H
