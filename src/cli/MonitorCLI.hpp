#pragma once

#include "cli/CLI.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>

namespace nlm {

class MonitorCLI {
public:
    MonitorCLI();
    ~MonitorCLI();
    
    // Disable copying, enable moving
    MonitorCLI(const MonitorCLI&) = delete;
    MonitorCLI& operator=(const MonitorCLI&) = delete;
    MonitorCLI(MonitorCLI&&) noexcept;
    MonitorCLI& operator=(MonitorCLI&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Monitoring commands
    bool monitorCommand(const std::vector<std::string>& args);
    bool liveCommand(const std::vector<std::string>& args);
    bool statusCommand(const std::vector<std::string>& args);
    bool activityCommand(const std::vector<std::string>& args);
    bool networkCommand(const std::vector<std::string>& args);
    bool memoryCommand(const std::vector<std::string>& args);
    bool performanceCommand(const std::vector<std::string>& args);
    bool spikesCommand(const std::vector<std::string>& args);
    bool connectivityCommand(const std::vector<std::string>& args);
    
    // Visualization commands
    bool visualizeCommand(const std::vector<std::string>& args);
    bool plotCommand(const std::vector<std::string>& args);
    bool graphCommand(const std::vector<std::string>& args);
    bool chartCommand(const std::vector<std::string>& args);
    bool tableCommand(const std::vector<std::string>& args);
    
    // Real-time monitoring
    bool startMonitoring(const std::vector<std::string>& args);
    bool stopMonitoring(const std::vector<std::string>& args);
    bool pauseMonitoring(const std::vector<std::string>& args);
    bool resumeMonitoring(const std::vector<std::string>& args);
    
    // Data collection and analysis
    bool collectDataCommand(const std::vector<std::string>& args);
    bool analyzeDataCommand(const std::vector<std::string>& args);
    bool exportDataCommand(const std::vector<std::string>& args);
    bool clearDataCommand(const std::vector<std::string>& args);
    
    // Help and documentation
    void showHelp() const;
    void showCommandHelp(const std::string& command) const;
    
    // Integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    void setCLI(CLI* cli) { this->cli = cli; }
    
    // State
    bool isInitialized() const { return initialized; }
    bool isMonitoring() const { return monitoring; }
    bool isPaused() const { return paused; }
    
    // Monitoring control
    void startRealTimeMonitoring() { monitoring = true; }
    void stopRealTimeMonitoring() { monitoring = false; }
    void pauseMonitoring() { paused = true; }
    void resumeMonitoring() { paused = false; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool initialized;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    CLI* cli;
    
    // Monitoring state
    bool monitoring;
    bool paused;
    bool realTimeEnabled;
    std::chrono::steady_clock::time_point startTime;
    std::vector<std::chrono::steady_clock::time_point> timestamps;
    
    // Monitoring data
    struct MonitorData {
        std::chrono::steady_clock::time_point timestamp;
        double cpuUsage;
        double memoryUsage;
        size_t spikeCount;
        size_t activeNeuronCount;
        size_t totalNeuronCount;
        size_t totalSynapseCount;
        double networkActivity;
        double averageFiringRate;
        std::vector<float> recentSpikes;
        std::vector<std::pair<size_t, float>> neuronActivity;
        std::vector<std::pair<size_t, size_t>> connectionStats;
        double averageWeight;
        double weightVariance;
        double totalWeight;
        size_t strengthenedSynapses;
        size_t weakenedSynapses;
        size_t unchangedSynapses;
    } currentData;
    
    // Visualization data
    struct VisualizationData {
        std::vector<double> cpuHistory;
        std::vector<double> memoryHistory;
        std::vector<size_t> spikeHistory;
        std::vector<size_t> neuronHistory;
        std::vector<size_t> synapseHistory;
        std::vector<double> activityHistory;
        std::vector<double> firingRateHistory;
        std::chrono::steady_clock::time_point lastUpdate;
        size_t maxHistorySize;
    } vizData;
    
    // Helper methods
    void startRealTimeMonitoringThread();
    void monitoringWorker();
    void updateMonitoringData();
    void collectNetworkStatistics();
    void collectMemoryStatistics();
    void collectPerformanceStatistics();
    void collectSpikeStatistics();
    void collectConnectivityStatistics();
    void visualizeTextBased(const std::string& title, const MonitorData& data) const;
    void visualizeTableBased(const std::string& title, const MonitorData& data) const;
    void visualizeGraphBased(const std::string& title, const MonitorData& data) const;
    void printMonitorHeader(const std::string& title) const;
    void printStatusHeader() const;
    void printActivityHeader() const;
    void printNetworkHeader() const;
    void printMemoryHeader() const;
    void printPerformanceHeader() const;
    void printSpikesHeader() const;
    void printConnectivityHeader() const;
    void printVisualizationHeader() const;
    void displayMonitorData(const MonitorData& data, const std::string& format = "text") const;
    std::string formatMonitorData(const MonitorData& data, const std::string& format = "text") const;
    std::string formatMonitorDataAsText(const MonitorData& data) const;
    std::string formatMonitorDataAsCSV(const MonitorData& data) const;
    std::string formatMonitorDataAsJSON(const MonitorData& data) const;
    std::string formatMonitorDataAsHTML(const MonitorData& data) const;
    std::string getFormattedTimestamp(const std::chrono::steady_clock::time_point& timestamp) const;
    std::string getMemoryUsageString(double memoryMB) const;
    std::string getCPULoadString(double cpuPercent) const;
    std::string getActivityLevelString(double activity) const;
    double calculateNetworkActivity(const MonitorData& data) const;
    double calculateAverageFiringRate(const MonitorData& data) const;
    double calculateAverageWeight(const MonitorData& data) const;
    double calculateWeightVariance(const MonitorData& data) const;
    double calculateTotalWeight(const MonitorData& data) const;
    std::vector<std::pair<size_t, float>> getMostActiveNeurons(const MonitorData& data, size_t count = 10) const;
    std::vector<std::pair<size_t, size_t>> getConnectionStats(const MonitorData& data) const;
    void updateVisualizationData(const MonitorData& data);
    bool validateMonitoringParameters(const std::vector<std::string>& args) const;
    void setupMonitoringConfiguration() const;
    bool startDataCollection(const std::vector<std::string>& args);
    bool stopDataCollection(const std::vector<std::string>& args);
    bool exportMonitoringData(const std::vector<std::string>& args);
    bool clearMonitoringData(const std::vector<std::string>& args);
    bool saveMonitoringSnapshot(const std::string& filename) const;
    bool loadMonitoringSnapshot(const std::string& filename);
    std::string getMonitoringDataFilename(const std::string& prefix = "monitor") const;
    bool validateFilePath(const std::string& path) const;
    void ensureDataDirectory() const;
};
