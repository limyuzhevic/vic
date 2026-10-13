#pragma once

#include "core/Config/ConfigurationManager.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Exceptions/NLMErrors.hpp"
#include "experiments/ExperimentBase.hpp"

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include <functional>
#include <optional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <regex>

namespace nlm {

// Forward declarations
class CommandLineParser;
class SimulationController;
class OutputFormatter;
class ExportImportManager;
class PerformanceMonitor;
class VisualizationController;
class LoggingController;
class ExperimentManager;

class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();

    // Initialize with configuration
    bool initialize(int argc, char** argv);
    void shutdown();

    // Command processing
    bool processCommand(const std::string& commandLine);
    bool runCommand(const std::string& command);

    // Interactive mode
    void startInteractiveMode();
    void stopInteractiveMode();
    bool isInteractiveMode() const;

    // Scripting mode
    bool runScript(const std::string& scriptPath);
    void enableScriptingMode(bool enable);
    bool isScriptingMode() const;

    // Command history and completion
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory() const;
    std::vector<std::string> getCommandCompletions(const std::string& prefix) const;

    // Error handling and recovery
    std::vector<std::string> getErrorHistory() const;
    bool hasErrors() const;
    void clearErrors();

    // Status and monitoring
    std::string getStatus() const;
    std::string getSystemInfo() const;
    double getUptime() const;

    // Configuration access
    ConfigurationManager& getConfigManager();
    const ConfigurationManager& getConfigManager() const;

    // Simulation control
    bool startSimulation(const std::string& simulationType, const std::string& configFile = "");
    bool stopSimulation();
    bool pauseSimulation();
    bool resumeSimulation();
    bool stepSimulation();
    bool isSimulationRunning() const;

    // Output formatting
    std::string formatOutput(const std::string& data, const std::string& format = "text");
    bool saveOutput(const std::string& data, const std::string& format, const std::string& filepath);

    // Export/Import
    bool exportBrainState(const std::string& filepath, const std::string& format = "binary");
    bool importBrainState(const std::string& filepath, const std::string& format = "binary");
    bool exportExperimentResults(const std::string& filepath, const std::string& format = "csv");
    bool importExperimentResults(const std::string& filepath, const std::string& format = "csv");

    // Performance monitoring
    std::string getPerformanceReport() const;
    bool startPerformanceMonitoring(double intervalMs = 1000.0);
    bool stopPerformanceMonitoring();
    bool isPerformanceMonitoring() const;

    // Visualization
    bool showVisualization(const std::string& type, const std::string& config = "");
    bool updateVisualization(const std::string& data);
    void closeVisualization();

    // Logging control
    bool setLogLevel(const std::string& level);
    bool addLogFile(const std::string& filepath);
    bool removeLogFile(const std::string& filepath);
    std::vector<std::string> getLogFiles() const;

    // Experiment management
    bool runExperiment(const std::string& experimentType, const std::string& configFile = "");
    bool compareExperiments(const std::string& experiment1, const std::string& experiment2);
    bool analyzeExperimentResults(const std::string& experimentFile);

    // Batch operations
    bool runBatch(const std::string& batchFile);
    bool scheduleExperiment(const std::string& experimentName, const std::string& scheduleConfig);
    bool cancelScheduledExperiment(const std::string& experimentName);

    // Benchmarking
    std::string runBenchmark(const std::string& benchmarkType = "default", const std::string& config = "");
    bool runPerformanceTest(const std::string& testType = "stress");

    // Help and documentation
    std::string getHelp(const std::string& command = "") const;
    std::string getVersion() const;
    std::vector<std::string> getAvailableCommands() const;
    std::vector<std::string> getAvailableExperiments() const;

    // Thread safety
    bool setThreadSafe(bool enable);
    bool isThreadSafe() const;

    // Error recovery
    bool recoverFromError(const std::string& errorContext);
    bool validateSystemState() const;

    // Serialization
    std::string serializeState() const;
    bool deserializeState(const std::string& stateData);

    // Configuration templates
    bool loadConfigTemplate(const std::string& templateName);
    bool saveConfigTemplate(const std::string& templateName);
    std::vector<std::string> getAvailableConfigTemplates() const;

    // Real-time monitoring
    std::string getRealTimeStatus() const;
    bool startRealTimeMonitoring(double intervalMs = 100.0);
    bool stopRealTimeMonitoring();

    // Command aliases and shortcuts
    bool addCommandAlias(const std::string& alias, const std::string& command);
    bool removeCommandAlias(const std::string& alias);
    std::string resolveCommandAlias(const std::string& alias) const;

    // Event handling
    void setCommandCallback(const std::function<void(const std::string&)>& callback);
    void setErrorCallback(const std::function<void(const std::string&)>& callback);
    void setStatusCallback(const std::function<void(const std::string&)>& callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Internal helper methods
    bool parseArguments(const std::vector<std::string>& args);
    std::string trim(const std::string& str) const;
    bool isValidCommand(const std::string& command) const;
    std::string getCurrentWorkingDirectory() const;
    std::string formatError(const std::string& error) const;
};

} // namespace nlm
