// NLM Advanced Debugging Interface
// Provides comprehensive debugging, profiling, and analysis capabilities for NLM

#ifndef NLM_DEBUG_INTERFACE_HPP
#define NLM_DEBUG_INTERFACE_HPP

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"

namespace nlm {

// Debug command types
enum class DebugCommand {
    Help,
    Status,
    Performance,
    Memory,
    Network,
    Neurons,
    Synapses,
    Regions,
    Populations,
    Development,
    Neuromodulation,
    Prediction,
    MemoryUsage,
    EnergyBudget,
    VisualInput,
    MotorOutput,
    AgentState,
    Configuration,
    ExportData,
    ImportData,
    Benchmark,
    RunExperiment,
    SetParameter,
    GetParameter,
    ValidateConfig,
    ClearLog,
    EnableLog,
    DisableLog,
    ShowLog,
    SaveState,
    LoadState,
    Replay,
    Quit
};

// Performance metrics
struct PerformanceMetrics {
    std::chrono::steady_clock::time_point startTime;
    double totalCPUTime;
    double totalWallTime;
    size_t totalSteps;
    double averageFiringRate;
    double averageMemoryUsage;
    double averageNetworkLatency;
    std::vector<double> stepTimes;
    std::vector<double> memoryUsageSamples;
    std::vector<double> firingRateSamples;
    
    PerformanceMetrics() : totalCPUTime(0.0), totalWallTime(0.0), totalSteps(0),
                           averageFiringRate(0.0), averageMemoryUsage(0.0),
                           averageNetworkLatency(0.0) {}
};

// Memory profile
struct MemoryProfile {
    size_t totalAllocated;
    size_t totalDeallocated;
    size_t currentUsage;
    size_t peakUsage;
    std::unordered_map<std::string, size_t> categoryUsage;
    std::vector<std::pair<double, size_t>> usageTimeline;
    
    MemoryProfile() : totalAllocated(0), totalDeallocated(0), currentUsage(0), peakUsage(0) {}
};

// Network statistics
struct NetworkStats {
    size_t packetsSent;
    size_t packetsReceived;
    double averageLatency;
    double packetLossRate;
    double bandwidthUsage;
    std::vector<double> latencyHistory;
    std::vector<size_t> packetSizeHistory;
    
    NetworkStats() : packetsSent(0), packetsReceived(0), averageLatency(0.0),
                     packetLossRate(0.0), bandwidthUsage(0.0) {}
};

// Debug interface class
class DebugInterface {
public:
    DebugInterface(std::shared_ptr<Config> config,
                   std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agent,
                   std::shared_ptr<AgentBody> body,
                   std::shared_ptr<SimpleWorld> world);
    ~DebugInterface();
    
    // Command processing
    bool processCommand(const std::string& input);
    void processCommand(DebugCommand cmd);
    
    // Command registration
    void registerCommand(DebugCommand cmd, const std::string& name, const std::string& description,
                         std::function<void()> handler);
    
    // Command help and completion
    std::vector<std::string> getCommandCompletions(const std::string& input) const;
    std::vector<std::string> getCommandHelp(DebugCommand cmd) const;
    
    // Debug command handlers
    void cmdHelp();
    void cmdStatus();
    void cmdPerformance();
    void cmdMemory();
    void cmdNetwork();
    void cmdNeurons();
    void cmdSynapses();
    void cmdRegions();
    void cmdPopulations();
    void cmdDevelopment();
    void cmdNeuromodulation();
    void cmdPrediction();
    void cmdMemoryUsage();
    void cmdEnergyBudget();
    void cmdVisualInput();
    void cmdMotorOutput();
    void cmdAgentState();
    void cmdConfiguration();
    void cmdExportData();
    void cmdImportData();
    void cmdBenchmark();
    void cmdRunExperiment();
    void cmdSetParameter();
    void cmdGetParameter();
    void cmdValidateConfig();
    void cmdClearLog();
    void cmdEnableLog();
    void cmdDisableLog();
    void cmdShowLog();
    void cmdSaveState();
    void cmdLoadState();
    void cmdReplay();
    void cmdQuit();
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    void recordStepTime(double time);
    void recordMemoryUsage(double usage);
    void recordFiringRate(double rate);
    void recordNetworkLatency(double latency);
    
    // Data export/import
    bool exportData(const std::string& filename, const std::string& format = "text");
    bool importData(const std::string& filename, const std::string& format = "text");
    
    // State management
    bool saveState(const std::string& filename) const;
    bool loadState(const std::string& filename);
    
    // Log management
    void enableLogging(const std::string& logFile = "");
    void disableLogging();
    void clearLogFile();
    std::string getLogFile() const;
    bool isLoggingEnabled() const;
    
    // Parameter management
    bool setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
    std::vector<std::string> getParameterKeys() const;
    
    // Validation
    std::vector<std::string> validateConfiguration() const;
    
    // Visualization
    std::string visualizeNeurons() const;
    std::string visualizeSynapses() const;
    std::string visualizeRegions() const;
    std::string visualizePopulations() const;
    std::string visualizeNetworkGraph() const;
    
    // Benchmarking
    void runBenchmark(const std::string& benchmarkName, size_t neuronCount, size_t steps);
    
    // Experiment running
    void runExperiment(const std::string& experimentName, const std::string& configFile = "");
    
    // Utility functions
    std::string formatMemorySize(size_t bytes) const;
    std::string formatTime(double seconds) const;
    std::string formatRate(double rate) const;
    
    // Getters
    const PerformanceMetrics& getPerformanceMetrics() const { return performanceMetrics_; }
    const MemoryProfile& getMemoryProfile() const { return memoryProfile_; }
    const NetworkStats& getNetworkStats() const { return networkStats_; }
    std::shared_ptr<Config> getConfig() const { return config_; }
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<AgentBrain> getAgent() const { return agent_; }
    std::shared_ptr<AgentBody> getBody() const { return body_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    // Static command parsing
    static DebugCommand parseCommand(const std::string& cmd);
    static std::string commandToString(DebugCommand cmd);
    static std::vector<std::string> getAllCommandNames();
    
private:
    // Internal helpers
    void log(const std::string& message);
    void error(const std::string& message);
    void warning(const std::string& message);
    std::string trim(const std::string& str) const;
    bool stringToBool(const std::string& str) const;
    double stringToDouble(const std::string& str) const;
    int stringToInt(const std::string& str) const;
    
    // Command execution
    void executeCommand(DebugCommand cmd);
    
    // Command completion
    std::vector<std::string> completeCommand(const std::string& input) const;
    
    // Component references
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<AgentBody> body_;
    std::shared_ptr<SimpleWorld> world_;
    
    // State
    std::unordered_map<DebugCommand, std::pair<std::string, std::function<void()>>> commands_;
    PerformanceMetrics performanceMetrics_;
    MemoryProfile memoryProfile_;
    NetworkStats networkStats_;
    bool loggingEnabled_;
    std::string logFile_;
    std::ofstream logStream_;
    std::vector<std::string> logBuffer_;
    bool performanceMonitoringActive_;
    std::chrono::steady_clock::time_point performanceStartTime_;
};

} // namespace nlm

#endif // NLM_DEBUG_INTERFACE_HPP