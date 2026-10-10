#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../profiling/Profiler.hpp"
#include "../profiling/PerformanceDashboard.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <deque>
#include <algorithm>

namespace nlm {

class ActivityMonitor {
public:
    ActivityMonitor(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                   std::shared_ptr<BrainSimulator> simulator);
    ~ActivityMonitor();
    
    // Disable copying, enable moving
    ActivityMonitor(const ActivityMonitor&) = delete;
    ActivityMonitor& operator=(const ActivityMonitor&) = delete;
    ActivityMonitor(ActivityMonitor&&) noexcept = default;
    ActivityMonitor& operator=(ActivityMonitor&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Real-time neural activity monitoring
    void startMonitoring(unsigned int updateIntervalMs = 100);
    void stopMonitoring();
    bool isMonitoring() const;
    void setUpdateInterval(unsigned int intervalMs);
    unsigned int getUpdateInterval() const;
    
    // Activity monitoring control
    void pauseMonitoring();
    void resumeMonitoring();
    void stopActivityMonitoring();
    bool isMonitoringPaused() const;
    bool isMonitoringRunning() const;
    
    // Neuron activity analysis
    void analyzeNeuronActivity();
    void analyzeNeuronActivity(NeuronId neuronId);
    void analyzeNeuronActivity(RegionId regionId);
    void analyzeNeuronActivity(const std::vector<NeuronId>& neuronIds);
    void analyzeNeuronActivity(const VisualizationFilter& filter);
    
    // Synapse activity analysis
    void analyzeSynapseActivity();
    void analyzeSynapseActivity(SynapseId synapseId);
    void analyzeSynapseActivity(RegionId sourceRegion, RegionId targetRegion);
    void analyzeSynapseActivity(const std::vector<SynapseId>& synapseIds);
    
    // Regional activity monitoring
    void monitorRegionalActivity();
    void monitorRegionalActivity(RegionId regionId);
    void monitorRegionalActivity(NeuronType neuronType);
    void monitorRegionalActivity(const std::vector<RegionId>& regionIds);
    void monitorRegionalActivity(const VisualizationFilter& filter);
    
    // Network-wide activity monitoring
    void monitorNetworkActivity();
    void monitorNetworkConnectivity();
    void monitorNetworkDynamics();
    void monitorNetworkStability();
    
    // Activity data collection
    struct ActivityData {
        double timestamp;
        SimulationStep step;
        double time;
        
        // Neuron statistics
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float maximumFiringRate;
        float minimumFiringRate;
        
        // Synapse statistics
        size_t activeSynapses;
        float averageSynapseWeight;
        float maximumSynapseWeight;
        float minimumSynapseWeight;
        
        // Regional statistics
        size_t activeRegions;
        float regionalActivityLevel;
        
        // Network statistics
        float networkActivityIndex;
        float networkStabilityIndex;
        float networkEfficiency;
        
        // Performance metrics
        double fps;
        double frameTime;
        size_t memoryUsage;
        size_t cpuUsage;
        
        ActivityData()
            : timestamp(0.0), step(0), time(0.0), activeNeurons(0), firingNeurons(0),
              averageFiringRate(0.0f), maximumFiringRate(0.0f), minimumFiringRate(0.0f),
              activeSynapses(0), averageSynapseWeight(0.0f), maximumSynapseWeight(0.0f),
              minimumSynapseWeight(0.0f), activeRegions(0), regionalActivityLevel(0.0f),
              networkActivityIndex(0.0f), networkStabilityIndex(1.0f), networkEfficiency(1.0f),
              fps(0.0), frameTime(0.0), memoryUsage(0), cpuUsage(0) {}
    };
    
    ActivityData getCurrentActivityData() const;
    std::vector<ActivityData> getActivityHistory(size_t maxHistory = 1000) const;
    
    // Activity state management
    void snapshotActivity(const std::string& name = "auto");
    bool restoreActivity(const std::string& name);
    std::vector<std::string> listActivitySnapshots() const;
    bool deleteActivitySnapshot(const std::string& name);
    
    // Activity analysis and statistics
    float calculateFiringRate(NeuronId neuronId) const;
    float calculateNetworkFiringRate() const;
    float calculateConnectionDensity() const;
    float calculateNetworkEfficiency() const;
    float calculateNetworkStability() const;
    float calculateActivityFluctuation(double timeWindow) const;
    float calculateNetworkSynchronization() const;
    
    // Activity thresholds and alerts
    void setFiringRateThreshold(float threshold);
    float getFiringRateThreshold() const;
    void setActivityLevelThreshold(float threshold);
    float getActivityLevelThreshold() const;
    void setConnectivityThreshold(float threshold);
    float getConnectivityThreshold() const;
    void setStabilityThreshold(float threshold);
    float getStabilityThreshold() const;
    
    // Activity alerts and notifications
    void enableActivityAlerts(bool enable);
    bool isActivityAlertsEnabled() const;
    void setActivityAlertCallback(std::function<void(const ActivityData&)> callback);
    bool checkActivityAlerts() const;
    std::vector<std::string> getActivityAlerts() const;
    void clearActivityAlerts();
    
    // Activity filtering and analysis
    void addActivityFilter(const VisualizationFilter& filter);
    void removeActivityFilter(const VisualizationFilter& filter);
    void clearActivityFilters();
    std::vector<VisualizationFilter> getActivityFilters() const;
    void setActivityFilterMode(FilterMode mode);
    FilterMode getActivityFilterMode() const;
    
    // Activity visualization and export
    void generateActivityReport(const std::string& filename = "activity_report.txt");
    void exportActivityData(const std::string& filename, const std::string& format = "csv");
    void generateActivityHeatmap(const std::string& filename = "activity_heatmap.txt");
    void generateActivityStatistics(const std::string& filename = "activity_statistics.txt");
    
    // Activity configuration
    void loadActivityConfiguration(const std::string& configFile);
    void saveActivityConfiguration() const;
    void resetActivityConfiguration();
    ActivityConfig getCurrentActivityConfiguration() const;
    
    // Integration with other systems
    void syncWithDebugger(std::shared_ptr<BrainDebugger> debugger);
    void syncWithSimulator(std::shared_ptr<BrainSimulator> simulator);
    void syncWithProfiler(std::shared_ptr<Profiler> profiler);
    void syncWithPerformanceDashboard(std::shared_ptr<PerformanceDashboard> dashboard);
    
    // Thread safety
    void lockActivityMonitor() const;
    void unlockActivityMonitor() const;
    bool tryLockActivityMonitor() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<BrainSimulator> simulator_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex activityMutex_;
    std::atomic<bool> isMonitoring_;
    std::atomic<bool> isPaused_;
    std::atomic<bool> isRunning_;
    std::thread monitoringThread_;
    
    // Monitoring state
    unsigned int updateIntervalMs_;
    double lastUpdateTime_;
    size_t updateCount_;
    
    // Activity data
    std::deque<ActivityData> activityHistory_;
    std::unordered_map<std::string, ActivityData> activitySnapshots_;
    std::deque<std::string> snapshotHistory_;
    
    // Activity configuration
    ActivityConfig activityConfig_;
    std::vector<VisualizationFilter> activityFilters_;
    FilterMode activityFilterMode_;
    
    // Activity thresholds and alerts
    float firingRateThreshold_;
    float activityLevelThreshold_;
    float connectivityThreshold_;
    float stabilityThreshold_;
    bool alertsEnabled_;
    
    // Activity alerts
    std::vector<std::string> activityAlerts_;
    std::function<void(const ActivityData&)> alertCallback_;
    
    // Performance monitoring
    double monitoringStartTime_;
    double monitoringEndTime_;
    double totalMonitoringTime_;
    
    // Internal helper methods
    void cleanup();
    void updateMonitoringState();
    void collectActivityData();
    void analyzeActivity();
    void generateActivityReports();
    void updateActivityHistory();
    void processActivityAlerts();
    void saveActivitySnapshot(const std::string& name);
    bool loadActivitySnapshot(const std::string& name);
    ActivityData captureCurrentActivity() const;
    void restoreActivityFromData(const ActivityData& data);
    void calculateNetworkStatistics(ActivityData& data) const;
    void calculateRegionalStatistics(ActivityData& data) const;
    void calculateNeuronStatistics(ActivityData& data) const;
    void calculateSynapseStatistics(ActivityData& data) const;
    void validateActivityData(ActivityData& data) const;
    void logActivityUpdate(const ActivityData& data);
    void exportActivityDataInternal() const;
    void generateActivityReportInternal() const;
    void generateActivityHeatmapInternal() const;
    void generateActivityStatisticsInternal() const;
    bool isActivityAlert(const ActivityData& data) const;
    std::string generateActivityAlertMessage(const ActivityData& data) const;
    double calculateFiringRateInternal(NeuronId neuronId) const;
    float calculateNetworkFiringRateInternal() const;
    float calculateConnectionDensityInternal() const;
    float calculateNetworkEfficiencyInternal() const;
    float calculateNetworkStabilityInternal() const;
    float calculateActivityFluctuationInternal(double timeWindow) const;
    float calculateNetworkSynchronizationInternal() const;
    void updatePerformanceMetrics();
    double calculateMonitoringTime() const;
    void handleActivityAlert(const ActivityData& data);
    void syncWithExternalSystems();
};

} // namespace nlm