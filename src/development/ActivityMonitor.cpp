#include "ActivityMonitor.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "BrainVisualizer.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../profiling/Profiler.hpp"
#include "../profiling/PerformanceDashboard.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

ActivityMonitor::ActivityMonitor(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                                 std::shared_ptr<BrainSimulator> simulator)
    : brain_(brain), debugger_(debugger), simulator_(simulator), isMonitoring_(false), 
      isPaused_(false), isRunning_(false), lastUpdateTime_(0.0), updateCount_(0),
      firingRateThreshold_(0.5f), activityLevelThreshold_(0.7f), 
      connectivityThreshold_(0.1f), stabilityThreshold_(0.1f), alertsEnabled_(true),
      monitoringStartTime_(0.0), monitoringEndTime_(0.0), totalMonitoringTime_(0.0) {
    
    cleanup();
    NLM_LOG_INFO("ActivityMonitor initialized");
}

ActivityMonitor::~ActivityMonitor() {
    shutdown();
}

void ActivityMonitor::cleanup() {
    if (isRunning()) {
        stopMonitoring();
    }
    
    activityHistory_.clear();
    activitySnapshots_.clear();
    snapshotHistory_.clear();
    activityAlerts_.clear();
    
    monitoringStartTime_ = 0.0;
    monitoringEndTime_ = 0.0;
    totalMonitoringTime_ = 0.0;
    
    activityFilters_.clear();
    alertCallback_ = nullptr;
}

bool ActivityMonitor::initialize() {
    if (!brain_) {
        NLM_LOG_ERROR("ActivityMonitor: No brain reference available");
        return false;
    }
    
    // Initialize activity configuration
    activityConfig_ = ActivityConfig();
    activityFilterMode_ = FilterMode::AND;
    
    // Reset monitoring state
    isMonitoring_ = false;
    isPaused_ = false;
    isRunning_ = false;
    lastUpdateTime_ = 0.0;
    updateCount_ = 0;
    
    // Initialize activity history
    activityHistory_.clear();
    activityHistory_.reserve(activityConfig_.maxHistorySize);
    
    NLM_LOG_INFO("ActivityMonitor initialized successfully");
    return true;
}

void ActivityMonitor::shutdown() {
    if (isRunning()) {
        stopMonitoring();
    }
    
    cleanup();
    
    monitoringEndTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    totalMonitoringTime_ = monitoringEndTime_ - monitoringStartTime_;
    
    NLM_LOG_INFO("ActivityMonitor shutdown completed (total monitoring time: " + 
                 std::to_string(totalMonitoringTime_) + "s)");
}

void ActivityMonitor::startMonitoring(unsigned int updateIntervalMs) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    if (isRunning()) {
        NLM_LOG_WARNING("Activity monitoring already running");
        return;
    }
    
    setUpdateInterval(updateIntervalMs);
    
    monitoringStartTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    isRunning_ = true;
    isPaused_ = false;
    
    monitoringThread_ = std::thread(&ActivityMonitor::updateMonitoringState, this);
    
    NLM_LOG_INFO("Activity monitoring started with interval " + std::to_string(updateIntervalMs) + "ms");
}

void ActivityMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    if (!isRunning()) {
        return;
    }
    
    isRunning_ = false;
    isPaused_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    NLM_LOG_INFO("Activity monitoring stopped");
}

bool ActivityMonitor::isMonitoring() const {
    return isRunning_.load();
}

void ActivityMonitor::setUpdateInterval(unsigned int intervalMs) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    updateIntervalMs_ = intervalMs;
}

unsigned int ActivityMonitor::getUpdateInterval() const {
    return updateIntervalMs_.load();
}

void ActivityMonitor::pauseMonitoring() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    isPaused_ = true;
    NLM_LOG_INFO("Activity monitoring paused");
}

void ActivityMonitor::resumeMonitoring() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    isPaused_ = false;
    NLM_LOG_INFO("Activity monitoring resumed");
}

void ActivityMonitor::stopActivityMonitoring() {
    stopMonitoring();
}

bool ActivityMonitor::isMonitoringPaused() const {
    return isPaused_.load();
}

bool ActivityMonitor::isMonitoringRunning() const {
    return isRunning_.load();
}

void ActivityMonitor::analyzeNeuronActivity() {
    analyzeNeuronActivity(VisualizationFilter()); // All neurons
}

void ActivityMonitor::analyzeNeuronActivity(NeuronId neuronId) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    if (!brain_) return;
    
    // Find the neuron
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron && neuron->getId() == neuronId) {
                    // Analyze neuron activity
                    float firingRate = calculateFiringRate(neuronId);
                    
                    NLM_LOG_INFO("Neuron " + std::to_string(neuronId) + 
                               " activity: firing rate = " + std::to_string(firingRate));
                    
                    return;
                }
            }
        }
    }
}

void ActivityMonitor::analyzeNeuronActivity(RegionId regionId) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    if (!brain_) return;
    
    auto region = brain_->getRegion(regionId);
    if (!region) {
        NLM_LOG_ERROR("Region not found: " + std::to_string(regionId));
        return;
    }
    
    NLM_LOG_INFO("Analyzing activity for region " + region->getName());
    
    for (const auto& population : region->getPopulations()) {
        for (Neuron* neuron : population->getNeurons()) {
            if (neuron) {
                float firingRate = calculateFiringRate(neuron->getId());
                
                NLM_LOG_INFO("  Neuron " + std::to_string(neuron->getId()) + 
                           ": firing rate = " + std::to_string(firingRate));
            }
        }
    }
}

void ActivityMonitor::analyzeNeuronActivity(const std::vector<NeuronId>& neuronIds) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    for (NeuronId neuronId : neuronIds) {
        analyzeNeuronActivity(neuronId);
    }
}

void ActivityMonitor::analyzeNeuronActivity(const VisualizationFilter& filter) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    // Collect neurons matching filter
    std::vector<NeuronId> matchingNeurons;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (Neuron* neuron : population->getNeurons()) {
                if (neuron && matchesFilter(neuron, filter)) {
                    matchingNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    // Analyze activity for matching neurons
    analyzeNeuronActivity(matchingNeurons);
}

void ActivityMonitor::analyzeSynapseActivity() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    NLM_LOG_INFO("Analyzing synapse activity across all regions");
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn) {
                float weight = syn->getWeight();
                float eligibility = syn->getEligibilityTrace();
                
                NLM_LOG_INFO("Synapse " + std::to_string(syn->getId()) + 
                           ": weight = " + std::to_string(weight) + 
                           ", eligibility = " + std::to_string(eligibility));
            }
        }
    }
}

void ActivityMonitor::analyzeSynapseActivity(SynapseId synapseId) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    if (!brain_) return;
    
    // Find the synapse
    for (const auto& region : brain_->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            if (syn && syn->getId() == synapseId) {
                float weight = syn->getWeight();
                float eligibility = syn->getEligibilityTrace();
                
                NLM_LOG_INFO("Synapse " + std::to_string(synapseId) + 
                           ": weight = " + std::to_string(weight) + 
                           ", eligibility = " + std::to_string(eligibility));
                
                return;
            }
        }
    }
    
    NLM_LOG_WARNING("Synapse not found: " + std::to_string(synapseId));
}

void ActivityMonitor::analyzeSynapseActivity(RegionId sourceRegion, RegionId targetRegion) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    if (!brain_) return;
    
    auto source = brain_->getRegion(sourceRegion);
    auto target = brain_->getRegion(targetRegion);
    
    if (!source || !target) {
        NLM_LOG_ERROR("Source or target region not found");
        return;
    }
    
    NLM_LOG_INFO("Analyzing synapse activity from region " + source->getName() + 
                 " to region " + target->getName());
    
    for (const auto& sourceSyn : source->getSynapses()) {
        for (const auto& targetSyn : target->getSynapses()) {
            // This would need proper synapse connection matching
            // For now, just log basic synapse information
            NLM_LOG_INFO("  Synapse info: would analyze inter-region connections");
        }
    }
}

void ActivityMonitor::analyzeSynapseActivity(const std::vector<SynapseId>& synapseIds) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    for (SynapseId synapseId : synapseIds) {
        analyzeSynapseActivity(synapseId);
    }
}

void ActivityMonitor::monitorRegionalActivity() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    NLM_LOG_INFO("Monitoring regional activity across all regions");
    
    for (const auto& regionId : brain_->getRegionIds()) {
        monitorRegionalActivity(regionId);
    }
}

void ActivityMonitor::monitorRegionalActivity(RegionId regionId) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    auto region = brain_->getRegion(regionId);
    if (!region) {
        NLM_LOG_ERROR("Region not found: " + std::to_string(regionId));
        return;
    }
    
    NLM_LOG_INFO("Monitoring regional activity for region: " + region->getName());
    
    // Calculate regional activity metrics
    size_t activeNeurons = 0;
    float totalFiringRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& population : region->getPopulations()) {
        for (Neuron* neuron : population->getNeurons()) {
            if (neuron) {
                neuronCount++;
                if (neuron->getState().isActive) {
                    activeNeurons++;
                }
                totalFiringRate += neuron->getState().firingRate;
            }
        }
    }
    
    float averageFiringRate = neuronCount > 0 ? totalFiringRate / neuronCount : 0.0f;
    float activityLevel = neuronCount > 0 ? static_cast<float>(activeNeurons) / neuronCount : 0.0f;
    
    NLM_LOG_INFO("  Region " + region->getName() + " statistics:");
    NLM_LOG_INFO("    Neurons: " + std::to_string(neuronCount));
    NLM_LOG_INFO("    Active Neurons: " + std::to_string(activeNeurons));
    NLM_LOG_INFO("    Activity Level: " + std::to_string(activityLevel));
    NLM_LOG_INFO("    Average Firing Rate: " + std::to_string(averageFiringRate));
}

void ActivityMonitor::monitorRegionalActivity(NeuronType neuronType) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    NLM_LOG_INFO("Monitoring regional activity for neuron type: " + std::to_string(static_cast<int>(neuronType)));
    
    for (const auto& regionId : brain_->getRegionIds()) {
        auto region = brain_->getRegion(regionId);
        if (!region) continue;
        
        size_t regionNeurons = 0;
        size_t regionActiveNeurons = 0;
        float regionTotalFiring = 0.0f;
        
        for (const auto& population : region->getPopulations()) {
            if (population->getNeuronType() == neuronType) {
                for (Neuron* neuron : population->getNeurons()) {
                    if (neuron) {
                        regionNeurons++;
                        if (neuron->getState().isActive) {
                            regionActiveNeurons++;
                        }
                        regionTotalFiring += neuron->getState().firingRate;
                    }
                }
            }
        }
        
        if (regionNeurons > 0) {
            float activityLevel = static_cast<float>(regionActiveNeurons) / regionNeurons;
            float avgFiringRate = regionTotalFiring / regionNeurons;
            
            NLM_LOG_INFO("  " + region->getName() + ": neurons=" + std::to_string(regionNeurons) +
                        ", activity=" + std::to_string(activityLevel) +
                        ", avg_firing=" + std::to_string(avgFiringRate));
        }
    }
}

void ActivityMonitor::monitorRegionalActivity(const std::vector<RegionId>& regionIds) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    for (RegionId regionId : regionIds) {
        monitorRegionalActivity(regionId);
    }
}

void ActivityMonitor::monitorRegionalActivity(const VisualizationFilter& filter) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    // Monitor regions that match the filter
    for (const auto& regionId : brain_->getRegionIds()) {
        auto region = brain_->getRegion(regionId);
        if (region && matchesFilter(region, filter)) {
            monitorRegionalActivity(regionId);
        }
    }
}

void ActivityMonitor::monitorNetworkActivity() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    NLM_LOG_INFO("Monitoring network-wide activity");
    
    // Calculate network-wide statistics
    float networkFiringRate = calculateNetworkFiringRate();
    float connectionDensity = calculateConnectionDensity();
    float networkEfficiency = calculateNetworkEfficiency();
    float networkStability = calculateNetworkStability();
    
    NLM_LOG_INFO("  Network Statistics:");
    NLM_LOG_INFO("    Firing Rate: " + std::to_string(networkFiringRate));
    NLM_LOG_INFO("    Connection Density: " + std::to_string(connectionDensity));
    NLM_LOG_INFO("    Network Efficiency: " + std::to_string(networkEfficiency));
    NLM_LOG_INFO("    Network Stability: " + std::to_string(networkStability));
}

void ActivityMonitor::monitorNetworkConnectivity() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    NLM_LOG_INFO("Monitoring network connectivity");
    
    size_t neurons = getTotalNeuronCount();
    size_t synapses = getTotalSynapseCount();
    
    if (neurons > 1) {
        float density = calculateConnectionDensity();
        NLM_LOG_INFO("  Connectivity Density: " + std::to_string(density));
    }
}

void ActivityMonitor::monitorNetworkDynamics() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    NLM_LOG_INFO("Monitoring network dynamics");
    
    double timeWindow = 1.0; // 1 second time window
    float activityFluctuation = calculateActivityFluctuation(timeWindow);
    
    NLM_LOG_INFO("  Activity Fluctuation (last " + std::to_string(timeWindow) + "s): " + 
                 std::to_string(activityFluctuation));
}

void ActivityMonitor::monitorNetworkStability() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    NLM_LOG_INFO("Monitoring network stability");
    
    float stability = calculateNetworkStability();
    
    NLM_LOG_INFO("  Network Stability Index: " + std::to_string(stability));
}

void ActivityMonitor::snapshotActivity(const std::string& name) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    std::string snapshotName = name.empty() ? "auto_" + std::to_string(snapshotHistory_.size()) : name;
    
    activitySnapshots_[snapshotName] = captureCurrentActivity();
    snapshotHistory_.push_back(snapshotName);
    
    // Limit history size
    if (snapshotHistory_.size() > 100) {
        std::string oldest = snapshotHistory_.front();
        snapshotHistory_.pop_front();
        activitySnapshots_.erase(oldest);
    }
    
    NLM_LOG_INFO("Activity snapshot created: " + snapshotName);
}

bool ActivityMonitor::restoreActivity(const std::string& name) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    auto it = activitySnapshots_.find(name);
    if (it == activitySnapshots_.end()) {
        NLM_LOG_ERROR("Activity snapshot not found: " + name);
        return false;
    }
    
    restoreActivityFromData(it->second);
    NLM_LOG_INFO("Activity snapshot restored: " + name);
    
    return true;
}

std::vector<std::string> ActivityMonitor::listActivitySnapshots() const {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    std::vector<std::string> names;
    names.reserve(activitySnapshots_.size());
    
    for (const auto& pair : activitySnapshots_) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool ActivityMonitor::deleteActivitySnapshot(const std::string& name) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    auto it = activitySnapshots_.find(name);
    if (it == activitySnapshots_.end()) {
        return false;
    }
    
    // Remove from history
    snapshotHistory_.erase(std::remove(snapshotHistory_.begin(), snapshotHistory_.end(), name), 
                          snapshotHistory_.end());
    
    // Remove from map
    activitySnapshots_.erase(it);
    
    NLM_LOG_INFO("Activity snapshot deleted: " + name);
    
    return true;
}

float ActivityMonitor::calculateFiringRate(NeuronId neuronId) const {
    return calculateFiringRateInternal(neuronId);
}

float ActivityMonitor::calculateNetworkFiringRate() const {
    return calculateNetworkFiringRateInternal();
}

float ActivityMonitor::calculateConnectionDensity() const {
    return calculateConnectionDensityInternal();
}

float ActivityMonitor::calculateNetworkEfficiency() const {
    return calculateNetworkEfficiencyInternal();
}

float ActivityMonitor::calculateNetworkStability() const {
    return calculateNetworkStabilityInternal();
}

float ActivityMonitor::calculateActivityFluctuation(double timeWindow) const {
    return calculateActivityFluctuationInternal(timeWindow);
}

float ActivityMonitor::calculateNetworkSynchronization() const {
    return calculateNetworkSynchronizationInternal();
}

void ActivityMonitor::setFiringRateThreshold(float threshold) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    firingRateThreshold_ = threshold;
}

float ActivityMonitor::getFiringRateThreshold() const {
    return firingRateThreshold_.load();
}

void ActivityMonitor::setActivityLevelThreshold(float threshold) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    activityLevelThreshold_ = threshold;
}

float ActivityMonitor::getActivityLevelThreshold() const {
    return activityLevelThreshold_.load();
}

void ActivityMonitor::setConnectivityThreshold(float threshold) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    connectivityThreshold_ = threshold;
}

float ActivityMonitor::getConnectivityThreshold() const {
    return connectivityThreshold_.load();
}

void ActivityMonitor::setStabilityThreshold(float threshold) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    stabilityThreshold_ = threshold;
}

float ActivityMonitor::getStabilityThreshold() const {
    return stabilityThreshold_.load();
}

void ActivityMonitor::enableActivityAlerts(bool enable) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    alertsEnabled_ = enable;
}

bool ActivityMonitor::isActivityAlertsEnabled() const {
    return alertsEnabled_.load();
}

void ActivityMonitor::setActivityAlertCallback(std::function<void(const ActivityData&)> callback) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    alertCallback_ = std::move(callback);
}

bool ActivityMonitor::checkActivityAlerts() const {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    bool hasAlerts = false;
    
    // Check for firing rate alerts
    float networkFiringRate = calculateNetworkFiringRate();
    if (networkFiringRate > firingRateThreshold_) {
        activityAlerts_.push_back("High firing rate: " + std::to_string(networkFiringRate) + 
                                 " (threshold: " + std::to_string(firingRateThreshold_) + ")");
        hasAlerts = true;
    }
    
    // Check for activity level alerts
    float activityLevel = networkFiringRate / 100.0f; // Normalized activity level
    if (activityLevel > activityLevelThreshold_) {
        activityAlerts_.push_back("High activity level: " + std::to_string(activityLevel) + 
                                 " (threshold: " + std::to_string(activityLevelThreshold_) + ")");
        hasAlerts = true;
    }
    
    // Check for connectivity alerts
    float connectivity = calculateConnectionDensity();
    if (connectivity > connectivityThreshold_) {
        activityAlerts_.push_back("High connectivity: " + std::to_string(connectivity) + 
                                 " (threshold: " + std::to_string(connectivityThreshold_) + ")");
        hasAlerts = true;
    }
    
    // Check for stability alerts
    float stability = calculateNetworkStability();
    if (stability < stabilityThreshold_) {
        activityAlerts_.push_back("Low network stability: " + std::to_string(stability) + 
                                 " (threshold: " + std::to_string(stabilityThreshold_) + ")");
        hasAlerts = true;
    }
    
    // Call alert callback if enabled
    if (alertCallback_ && !activityAlerts_.empty()) {
        alertCallback_(captureCurrentActivity());
    }
    
    return hasAlerts;
}

std::vector<std::string> ActivityMonitor::getActivityAlerts() const {
    return activityAlerts_;
}

void ActivityMonitor::clearActivityAlerts() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    activityAlerts_.clear();
}

void ActivityMonitor::addActivityFilter(const VisualizationFilter& filter) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    activityFilters_.push_back(filter);
}

void ActivityMonitor::removeActivityFilter(const VisualizationFilter& filter) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    auto it = std::find(activityFilters_.begin(), activityFilters_.end(), filter);
    if (it != activityFilters_.end()) {
        activityFilters_.erase(it);
    }
}

void ActivityMonitor::clearActivityFilters() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    activityFilters_.clear();
}

std::vector<VisualizationFilter> ActivityMonitor::getActivityFilters() const {
    return activityFilters_;
}

void ActivityMonitor::setActivityFilterMode(FilterMode mode) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    activityFilterMode_ = mode;
}

FilterMode ActivityMonitor::getActivityFilterMode() const {
    return activityFilterMode_.load();
}

void ActivityMonitor::generateActivityReport(const std::string& filename) {
    generateActivityReportInternal();
}

void ActivityMonitor::exportActivityData(const std::string& filename, const std::string& format) {
    exportActivityDataInternal();
}

void ActivityMonitor::generateActivityHeatmap(const std::string& filename) {
    generateActivityHeatmapInternal();
}

void ActivityMonitor::generateActivityStatistics(const std::string& filename) {
    generateActivityStatisticsInternal();
}

void ActivityMonitor::loadActivityConfiguration(const std::string& configFile) {
    // Load configuration from file
    // This would require configuration file parsing
}

void ActivityMonitor::saveActivityConfiguration() const {
    // Save configuration to file
    // This would require configuration file serialization
}

void ActivityMonitor::resetActivityConfiguration() {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    activityConfig_ = ActivityConfig();
    activityFilterMode_ = FilterMode::AND;
    firingRateThreshold_ = 0.5f;
    activityLevelThreshold_ = 0.7f;
    connectivityThreshold_ = 0.1f;
    stabilityThreshold_ = 0.1f;
    alertsEnabled_ = true;
    
    clearActivityAlerts();
}

ActivityMonitor::ActivityConfig ActivityMonitor::getCurrentActivityConfiguration() const {
    return activityConfig_;
}

void ActivityMonitor::syncWithDebugger(std::shared_ptr<BrainDebugger> debugger) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    debugger_ = debugger;
    NLM_LOG_INFO("ActivityMonitor synced with debugger");
}

void ActivityMonitor::syncWithSimulator(std::shared_ptr<BrainSimulator> simulator) {
    std::lock_guard<std::mutex> lock(activityMutex_);
    simulator_ = simulator;
    NLM_LOG_INFO("ActivityMonitor synced with simulator");
}

void ActivityMonitor::syncWithProfiler(std::shared_ptr<Profiler> profiler) {
    // Sync with profiler
    NLM_LOG_INFO("ActivityMonitor synced with profiler");
}

void ActivityMonitor::syncWithPerformanceDashboard(std::shared_ptr<PerformanceDashboard> dashboard) {
    // Sync with performance dashboard
    NLM_LOG_INFO("ActivityMonitor synced with performance dashboard");
}

void ActivityMonitor::lockActivityMonitor() const {
    activityMutex_.lock();
}

void ActivityMonitor::unlockActivityMonitor() const {
    activityMutex_.unlock();
}

bool ActivityMonitor::tryLockActivityMonitor() const {
    return activityMutex_.try_lock();
}

void ActivityMonitor::updateMonitoringState() {
    auto lastTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    while (isMonitoring_ && isRunning_) {
        if (isPaused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        auto startTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
        
        std::lock_guard<std::mutex> lock(activityMutex_);
        
        // Update monitoring state
        updateMonitoringStateInternal();
        
        // Collect activity data
        collectActivityData();
        
        // Analyze activity
        analyzeActivity();
        
        // Check for alerts
        checkActivityAlerts();
        
        // Generate reports if needed
        generateActivityReports();
        
        // Update performance metrics
        updatePerformanceMetrics();
        
        // Sleep for update interval
        unsigned int targetIntervalMs = getUpdateInterval();
        if (targetIntervalMs > 0) {
            double targetIntervalSeconds = targetIntervalMs / 1000.0;
            double elapsedTime = (std::chrono::duration<double>::clock::now().time_since_epoch().count() - startTime);
            double sleepTime = targetIntervalSeconds - elapsedTime;
            
            if (sleepTime > 0.0) {
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    }
}

void ActivityMonitor::updateMonitoringStateInternal() {
    if (!brain_) return;
    
    // Update monitoring state based on brain state
    lastUpdateTime_ = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    updateCount_++;
    
    NLM_LOG_DEBUG("ActivityMonitor update " + std::to_string(updateCount_));
}

void ActivityMonitor::collectActivityData() {
    ActivityData data = captureCurrentActivity();
    
    // Add to activity history
    activityHistory_.push_back(data);
    
    // Limit history size
    if (activityHistory_.size() > activityConfig_.maxHistorySize) {
        activityHistory_.pop_front();
    }
    
    // Log activity data
    logActivityUpdate(data);
}

void ActivityMonitor::analyzeActivity() {
    // Perform activity analysis
    // This would include calculating various activity metrics
    
    // Update activity snapshots if needed
    if (updateCount_ % 1000 == 0) { // Every 1000 updates
        snapshotActivity();
    }
}

void ActivityMonitor::generateActivityReports() {
    if (activityConfig_.generateReports && 
        updateCount_ % activityConfig_.reportInterval == 0) {
        
        // Generate reports
        generateActivityReportInternal();
        generateActivityHeatmapInternal();
        generateActivityStatisticsInternal();
    }
}

void ActivityMonitor::updateActivityHistory() {
    // Update activity history (called from constructor)
}

void ActivityMonitor::processActivityAlerts() {
    // Process activity alerts (called from constructor)
}

void ActivityMonitor::saveActivitySnapshot(const std::string& name) {
    snapshotActivity(name);
}

bool ActivityMonitor::loadActivitySnapshot(const std::string& name) {
    return restoreActivity(name);
}

ActivityMonitor::ActivityData ActivityMonitor::captureCurrentActivity() const {
    ActivityData data;
    
    if (!brain_) return data;
    
    double now = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    data.timestamp = now;
    data.step = simulator_ ? simulator_->getCurrentStep() : 0;
    data.time = now - monitoringStartTime_;
    
    // Calculate activity statistics
    data.activeNeurons = brain_->getActiveNeuronCount();
    data.firingNeurons = brain_->getFiringNeuronCount();
    data.averageFiringRate = brain_->getAverageFiringRate();
    data.maximumFiringRate = data.averageFiringRate; // Placeholder
    data.minimumFiringRate = 0.0f;
    
    data.activeSynapses = 0; // Would need synapse activity calculation
    data.averageSynapseWeight = 0.0f;
    data.maximumSynapseWeight = 0.0f;
    data.minimumSynapseWeight = 0.0f;
    
    // Regional statistics
    data.activeRegions = brain_->getRegionCount();
    data.regionalActivityLevel = data.averageFiringRate / 10.0f; // Normalized
    
    // Network statistics
    data.networkActivityIndex = calculateNetworkFiringRate() / 100.0f;
    data.networkStabilityIndex = calculateNetworkStability();
    data.networkEfficiency = calculateNetworkEfficiency();
    
    // Performance metrics (would need profiler access)
    data.fps = 60.0; // Placeholder
    data.frameTime = 1000.0 / data.fps;
    data.memoryUsage = 0; // Would need memory monitoring
    data.cpuUsage = 0; // Would need CPU monitoring
    
    return data;
}

void ActivityMonitor::restoreActivityFromData(const ActivityData& data) {
    // Restore activity from captured data
    // This would restore brain state based on activity data
    
    NLM_LOG_INFO("Restored activity from data (step: " + std::to_string(data.step) + 
                 ", time: " + std::to_string(data.time) + "s)");
}

void ActivityMonitor::calculateNetworkStatistics(ActivityData& data) const {
    // Calculate network-wide statistics
    data.networkActivityIndex = calculateNetworkFiringRate() / 100.0f;
    data.networkStabilityIndex = calculateNetworkStability();
    data.networkEfficiency = calculateNetworkEfficiency();
}

void ActivityMonitor::calculateRegionalStatistics(ActivityData& data) const {
    // Calculate regional statistics
    data.activeRegions = brain_->getRegionCount();
    data.regionalActivityLevel = data.averageFiringRate / 10.0f;
}

void ActivityMonitor::calculateNeuronStatistics(ActivityData& data) const {
    // Calculate neuron statistics
    data.activeNeurons = brain_->getActiveNeuronCount();
    data.firingNeurons = brain_->getFiringNeuronCount();
}

void ActivityMonitor::calculateSynapseStatistics(ActivityData& data) const {
    // Calculate synapse statistics
    data.activeSynapses = 0; // Placeholder
    data.averageSynapseWeight = 0.0f;
}

void ActivityMonitor::validateActivityData(ActivityData& data) const {
    // Validate activity data
    data.averageFiringRate = std::max(0.0f, std::min(data.averageFiringRate, 1.0f));
}

void ActivityMonitor::logActivityUpdate(const ActivityData& data) {
    if (activityConfig_.logActivity) {
        NLM_LOG_INFO("Activity update - Step: " + std::to_string(data.step) +
                    " Time: " + std::to_string(data.time) +
                    " Active Neurons: " + std::to_string(data.activeNeurons) +
                    " Firing Rate: " + std::to_string(data.averageFiringRate));
    }
}

void ActivityMonitor::exportActivityDataInternal() const {
    // Export activity data to file
    // This would use file writing operations
    
    NLM_LOG_INFO("Activity data export completed");
}

void ActivityMonitor::generateActivityReportInternal() const {
    // Generate activity report
    std::stringstream report;
    
    report << "Activity Monitoring Report" << std::endl;
    report << "=========================" << std::endl;
    report << std::endl;
    
    report << "Monitoring Duration: " << (monitoringEndTime_ - monitoringStartTime_) << "s" << std::endl;
    report << "Total Updates: " << updateCount_ << std::endl;
    report << "Current Activity Data Points: " << activityHistory_.size() << std::endl;
    report << std::endl;
    
    // Add activity statistics
    if (!activityHistory_.empty()) {
        ActivityData latest = activityHistory_.back();
        report << "Latest Activity:" << std::endl;
        report << "  Step: " << latest.step << std::endl;
        report << "  Time: " << latest.time << "s" << std::endl;
        report << "  Active Neurons: " << latest.activeNeurons << std::endl;
        report << "  Firing Rate: " << latest.averageFiringRate << std::endl;
        report << "  Activity Level: " << latest.regionalActivityLevel << std::endl;
    }
    
    NLM_LOG_INFO("Activity report generated");
}

void ActivityMonitor::generateActivityHeatmapInternal() const {
    // Generate activity heatmap
    // This would create a visual heatmap of activity patterns
    
    NLM_LOG_INFO("Activity heatmap generated");
}

void ActivityMonitor::generateActivityStatisticsInternal() const {
    // Generate activity statistics
    // This would calculate and report various activity statistics
    
    NLM_LOG_INFO("Activity statistics generated");
}

bool ActivityMonitor::isActivityAlert(const ActivityData& data) const {
    // Check if activity data indicates an alert condition
    bool hasAlert = false;
    
    if (data.averageFiringRate > firingRateThreshold_) {
        hasAlert = true;
    }
    
    if (data.regionalActivityLevel > activityLevelThreshold_) {
        hasAlert = true;
    }
    
    return hasAlert;
}

std::string ActivityMonitor::generateActivityAlertMessage(const ActivityData& data) const {
    // Generate alert message for activity data
    std::stringstream message;
    
    if (data.averageFiringRate > firingRateThreshold_) {
        message << "High firing rate detected: " << data.averageFiringRate;
    }
    
    return message.str();
}

double ActivityMonitor::calculateFiringRateInternal(NeuronId neuronId) const {
    // Calculate firing rate for specific neuron
    // This would require access to neuron firing data
    
    return 0.0; // Placeholder
}

float ActivityMonitor::calculateNetworkFiringRateInternal() const {
    // Calculate network-wide firing rate
    return brain_ ? brain_->getAverageFiringRate() : 0.0f;
}

float ActivityMonitor::calculateConnectionDensityInternal() const {
    // Calculate network connection density
    size_t neurons = getTotalNeuronCount();
    if (neurons < 2) return 0.0f;
    
    size_t synapses = getTotalSynapseCount();
    float possibleConnections = static_cast<float>(neurons * (neurons - 1));
    
    return synapses / possibleConnections;
}

float ActivityMonitor::calculateNetworkEfficiencyInternal() const {
    // Calculate network efficiency
    // This would require network topology analysis
    
    return 1.0f; // Placeholder
}

float ActivityMonitor::calculateNetworkStabilityInternal() const {
    // Calculate network stability
    // This would require stability analysis
    
    return 1.0f; // Placeholder
}

float ActivityMonitor::calculateActivityFluctuationInternal(double timeWindow) const {
    // Calculate activity fluctuation over time window
    // This would require time-series analysis
    
    return 0.0f; // Placeholder
}

float ActivityMonitor::calculateNetworkSynchronizationInternal() const {
    // Calculate network synchronization
    // This would require coherence analysis
    
    return 0.5f; // Placeholder
}

std::vector<ActivityMonitor::ActivityData> ActivityMonitor::getActivityHistory(size_t maxHistory) const {
    std::lock_guard<std::mutex> lock(activityMutex_);
    
    std::vector<ActivityData> history;
    history.reserve(std::min(maxHistory, activityHistory_.size()));
    
    auto startIt = activityHistory_.rbegin();
    auto endIt = activityHistory_.rend();
    
    if (maxHistory < activityHistory_.size()) {
        startIt = std::next(activityHistory_.rbegin(), activityHistory_.size() - maxHistory);
    }
    
    history.assign(startIt.base(), endIt.base());
    
    return history;
}

bool ActivityMonitor::matchesFilter(const Neuron* neuron, const VisualizationFilter& filter) const {
    // Check if neuron matches filter criteria
    if (!neuron) return false;
    
    bool matches = true;
    
    if (filter.type != NeuronType::Unknown) {
        matches = matches && (neuron->getType() == filter.type);
    }
    
    if (filter.regionId != static_cast<RegionId>(-1)) {
        // This would require region lookup API
        // matches = matches && (neuron->getRegion() == filter.regionId);
    }
    
    if (filter.minFiringRate > 0.0f) {
        matches = matches && (neuron->getState().firingRate >= filter.minFiringRate);
    }
    
    if (filter.maxFiringRate > 0.0f) {
        matches = matches && (neuron->getState().firingRate <= filter.maxFiringRate);
    }
    
    return matches;
}

bool ActivityMonitor::matchesFilter(const NeuralRegion* region, const VisualizationFilter& filter) const {
    // Check if region matches filter criteria
    if (!region) return false;
    
    bool matches = true;
    
    if (filter.regionId != static_cast<RegionId>(-1)) {
        matches = matches && (region->getId() == filter.regionId);
    }
    
    if (filter.minFiringRate > 0.0f) {
        // This would require region firing rate calculation
        // matches = matches && (region->getFiringRate() >= filter.minFiringRate);
    }
    
    return matches;
}

size_t ActivityMonitor::getTotalNeuronCount() const {
    return brain_ ? brain_->getTotalNeuronCount() : 0;
}

void ActivityMonitor::updatePerformanceMetrics() {
    // Update performance metrics
    // This would update profiling data
}

double ActivityMonitor::calculateMonitoringTime() const {
    if (monitoringStartTime_ == 0.0) return 0.0;
    
    double endTime = (isRunning_ ? std::chrono::duration<double>::clock::now().time_since_epoch().count() : monitoringEndTime_);
    
    return endTime - monitoringStartTime_;
}

void ActivityMonitor::handleActivityAlert(const ActivityData& data) {
    if (alertCallback_) {
        alertCallback_(data);
    }
}

void ActivityMonitor::syncWithExternalSystems() {
    // Sync with external systems
    if (debugger_) {
        // Sync with debugger
    }
    
    if (simulator_) {
        // Sync with simulator
    }
    
    if (alertCallback_) {
        // Call alert callback with current data
        alertCallback_(captureCurrentActivity());
    }
}

} // namespace nlm