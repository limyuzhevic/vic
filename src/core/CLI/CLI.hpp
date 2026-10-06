#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <iomanip>
#include <sstream>

#include "core/Config/Config.hpp"

namespace nlm {

class CLI {
public:
    CLI();
    ~CLI();

    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed options
    std::string getConfigFile() const { return configFile; }
    bool isHelpRequested() const { return helpRequested; }
    bool isVersionRequested() const { return versionRequested; }
    bool isInteractiveMode() const { return interactiveMode; }
    bool isSimulationControl() const { return simulationControl; }
    bool isDebugMode() const { return debugMode; }
    bool isExportMode() const { return exportMode; }
    bool isImportMode() const { return importMode; }
    std::string getExportFile() const { return exportFile; }
    std::string getImportFile() const { return importFile; }
    std::string getLogLevel() const { return logLevel; }
    std::string getEnvironment() const { return environment; }
    double getTimeStep() const { return timeStep; }
    int64_t getNeuronCount() const { return neuronCount; }
    int64_t getRegionCount() const { return regionCount; }
    float getConnectionProbability() const { return connectionProbability; }
    bool getPlasticityEnabled() const { return plasticityEnabled; }
    bool getStdpEnabled() const { return stdpEnabled; }
    bool getHebbianEnabled() const { return hebbianEnabled; }
    bool getDevelopmentEnabled() const { return developmentEnabled; }
    bool getNeuromodulationEnabled() const { return neuromodulationEnabled; }
    bool getVisualizationEnabled() const { return visualizationEnabled; }
    bool getMemoryEnabled() const { return memoryEnabled; }
    bool getPredictionEnabled() const { return predictionEnabled; }
    bool getPlanningEnabled() const { return planningEnabled; }
    
    // Execute the requested operation
    int execute();
    
    // Print help and usage information
    void printHelp() const;
    void printVersion() const;
    void printExamples() const;

private:
    // Command line argument definitions
    std::string configFile = "configs/default.cfg";
    bool helpRequested = false;
    bool versionRequested = false;
    bool interactiveMode = false;
    bool simulationControl = false;
    bool debugMode = false;
    bool exportMode = false;
    bool importMode = false;
    std::string exportFile = "";
    std::string importFile = "";
    std::string logLevel = "INFO";
    std::string environment = "GridWorld";
    double timeStep = 0.001;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    
    // Feature toggles
    bool plasticityEnabled = true;
    bool stdpEnabled = true;
    bool hebbianEnabled = true;
    bool developmentEnabled = false;
    bool neuromodulationEnabled = false;
    bool visualizationEnabled = false;
    bool memoryEnabled = false;
    bool predictionEnabled = false;
    bool planningEnabled = false;
    
    // Simulation control
    bool runConnectivityTest = false;
    bool runPlasticityTest = false;
    bool runStdpTest = false;
    bool runLearningExperiment = false;
    SimulationStep maxSteps = 1000;
    double timeLimit = 0.0;
    
    // Export/Import
    std::string brainExportFile = "brain_export.json";
    std::string configExportFile = "config_export.cfg";
    
    // Real-time monitoring
    bool realTimeMonitoring = false;
    int monitoringInterval = 100; // milliseconds
    std::atomic<bool> monitoringActive{false};
    std::thread monitoringThread;
    std::mutex monitoringMutex;
    std::condition_variable monitoringCV;
    
    // Internal help and validation
    std::map<std::string, std::string> optionDescriptions;
    std::map<std::string, std::function<bool(const std::string&)>> validators;
    
    void initializeOptionDescriptions();
    void initializeValidators();
    void parseEnvironment(const std::string& env);
    void executeInteractiveMode();
    void executeSimulationMode();
    void executeExportMode();
    void executeImportMode();
    void executeMonitoringMode();
    void executeDebugMode();
    void printConfiguration() const;
    void validateConfiguration() const;
    bool validateOption(const std::string& key, const std::string& value) const;
    void exportBrainData() const;
    void importBrainData();
    void startMonitoring();
    void stopMonitoring();
    void monitoringLoop();
    void printBrainStatus() const;
    void printMemoryStatus() const;
    void printNeuromodulationStatus() const;
    void printDevelopmentStatus() const;
};

} // namespace nlm
