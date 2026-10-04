#include "StatusManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <filesystem>

namespace nlm {

struct StatusManager::Impl {
    // Configuration
    StatusConfig config;
    
    // Current status
    BrainStatus brainStatus;
    SystemStatus systemStatus;
    ExperimentStatus experimentStatus;
    StatusLevel overallStatusLevel{StatusLevel::Offline};
    
    // Status tracking
    std::vector<StatusSnapshot> statusHistory;
    std::vector<StatusEvent> statusEvents;
    std::chrono::system_clock::time_point lastStatusUpdate;
    bool statusMonitoringActive{false};
    
    // Output streams
    std::ofstream statusExportFile;
    std::ofstream snapshotFile;
    
    // Constructor
    Impl() : lastStatusUpdate(std::chrono::system_clock::now()) {}
    
    // Destructor
    ~Impl() {
        if (statusExportFile.is_open()) {
            statusExportFile.close();
        }
        if (snapshotFile.is_open()) {
            snapshotFile.close();
        }
    }
};

StatusManager::StatusManager(const StatusConfig& config) : pImpl(std::make_unique<Impl>()) {
    configure(config);
    
    // Open export file if specified
    if (!config.statusExportFile.empty()) {
        pImpl->statusExportFile.open(config.statusExportFile, std::ios::app);
    }
    
    // Open snapshot file if specified
    if (!config.statusSnapshotFile.empty()) {
        pImpl->snapshotFile.open(config.statusSnapshotFile, std::ios::app);
    }
}

StatusManager::~StatusManager() = default;

void StatusManager::configure(const StatusConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->config = config;
    
    // Start/stop status monitoring based on config
    if (config.enableRealTimeUpdates && !pImpl->statusMonitoringActive) {
        pImpl->statusMonitoringActive = true;
    } else if (!config.enableRealTimeUpdates && pImpl->statusMonitoringActive) {
        pImpl->statusMonitoringActive = false;
    }
}

void StatusManager::updateBrainStatus(const BrainStatus& status) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->brainStatus = status;
    
    if (pImpl->config.enableStatusLogging) {
        Logger::getGlobal().info("Brain status updated: " + std::to_string(status.currentStep) + " steps, " +
                               std::to_string(status.neuronCount) + " neurons, " +
                               std::to_string(status.averageFiringRate) + "% firing rate");
    }
    
    updateStatusLevel();
}

void StatusManager::updateSystemStatus(const SystemStatus& status) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->systemStatus = status;
    
    if (pImpl->config.enableStatusLogging) {
        Logger::getGlobal().info("System status updated: " + std::to_string(status.cpuUsage) + "% CPU usage, " +
                               std::to_string(status.memoryUsed / 1024 / 1024) + " MB memory used");
    }
    
    updateStatusLevel();
}

void StatusManager::updateExperimentStatus(const ExperimentStatus& status) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->experimentStatus = status;
    
    if (pImpl->config.enableStatusLogging) {
        Logger::getGlobal().info("Experiment status updated: " + status.name + " - " +
                               std::to_string(status.completionPercentage) + "% complete");
    }
    
    updateStatusLevel();
}

BrainStatus StatusManager::getBrainStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->brainStatus;
}

SystemStatus StatusManager::getSystemStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->systemStatus;
}

ExperimentStatus StatusManager::getExperimentStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->experimentStatus;
}

StatusLevel StatusManager::getOverallStatusLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->overallStatusLevel;
}

std::string StatusManager::getStatusText() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    std::stringstream ss;
    
    switch (pImpl->overallStatusLevel) {
        case StatusLevel::Offline:
            ss << "System is offline";
            break;
        case StatusLevel::Initializing:
            ss << "System is initializing";
            break;
        case StatusLevel::Ready:
            ss << "System is ready for operations";
            break;
        case StatusLevel::Running:
            ss << "System is running simulation";
            break;
        case StatusLevel::Paused:
            ss << "System is paused";
            break;
        case StatusLevel::Saving:
            ss << "System is saving checkpoint";
            break;
        case StatusLevel::Loading:
            ss << "System is loading checkpoint";
            break;
        case StatusLevel::Error:
            ss << "System encountered an error";
            break;
        case StatusLevel::Completed:
            ss << "System completed successfully";
            break;
    }
    
    return ss.str();
}

StatusSnapshot StatusManager::createStatusSnapshot() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    StatusSnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.level = pImpl->overallStatusLevel;
    snapshot.brainStatus = pImpl->brainStatus;
    snapshot.systemStatus = pImpl->systemStatus;
    snapshot.experimentStatus = pImpl->experimentStatus;
    snapshot.statusText = getStatusText();
    
    // Check for alerts and warnings
    std::vector<std::string> alerts;
    std::vector<std::string> warnings;
    
    // Check for critical alerts
    if (snapshot.brainStatus.averageFiringRate > 200.0) {
        alerts.push_back("High firing rate detected: " + std::to_string(snapshot.brainStatus.averageFiringRate) + "%");
    }
    
    if (snapshot.brainStatus.totalSpikeCount > 1000000) {
        alerts.push_back("High spike count: " + std::to_string(snapshot.brainStatus.totalSpikeCount) + " spikes");
    }
    
    if (snapshot.systemStatus.cpuUsage > 90.0) {
        warnings.push_back("High CPU usage: " + std::to_string(snapshot.systemStatus.cpuUsage) + "%");
    }
    
    if (snapshot.systemStatus.getMemoryUsagePercentage() > 85.0) {
        warnings.push_back("High memory usage: " + std::to_string(snapshot.systemStatus.getMemoryUsagePercentage()) + "%");
    }
    
    if (snapshot.experimentStatus.isRunning && 
        snapshot.experimentStatus.completionPercentage > 90.0 &&
        snapshot.experimentStatus.completionPercentage < 100.0) {
        warnings.push_back("Experiment nearing completion: " + std::to_string(snapshot.experimentStatus.completionPercentage) + "%");
    }
    
    snapshot.activeAlerts = alerts;
    snapshot.warningMessages = warnings;
    
    return snapshot;
}

bool StatusManager::saveStatusSnapshot(const std::string& filepath, const std::string& format) {
    StatusSnapshot snapshot = createStatusSnapshot();
    
    if (format == "json" || format == "JSON") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << snapshot.toJSON();
        file.close();
    } else {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << snapshot.toText();
        file.close();
    }
    
    return true;
}

bool StatusManager::loadStatusSnapshot(const StatusSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Update status with snapshot data
    pImpl->brainStatus = snapshot.brainStatus;
    pImpl->systemStatus = snapshot.systemStatus;
    pImpl->experimentStatus = snapshot.experimentStatus;
    
    // Add status event for loading
    addStatusEvent("StatusManager", "Status snapshot loaded from " + 
                  std::to_string(std::chrono::system_clock::to_time_t(snapshot.timestamp)),
                  StatusLevel::Ready);
    
    return true;
}

std::vector<StatusSnapshot> StatusManager::getStatusHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Return last N snapshots based on config
    size_t maxSnapshots = pImpl->config.maxTimelineEvents;
    if (maxSnapshots == 0) {
        return pImpl->statusHistory;
    }
    
    std::vector<StatusSnapshot> result;
    size_t startIdx = pImpl->statusHistory.size() > maxSnapshots ? 
                     pImpl->statusHistory.size() - maxSnapshots : 0;
    
    for (size_t i = startIdx; i < pImpl->statusHistory.size(); ++i) {
        result.push_back(pImpl->statusHistory[i]);
    }
    
    return result;
}

std::vector<StatusEvent> StatusManager::getStatusEvents() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->statusEvents;
}

void StatusManager::addStatusEvent(const std::string& source,
                                 const std::string& message,
                                 StatusLevel level,
                                 const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    StatusEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.source = source;
    event.message = message;
    event.level = level;
    event.metadata = metadata;
    
    pImpl->statusEvents.push_back(event);
    
    // Add to status history if needed
    if (pImpl->config.enableStatusSnapshots) {
        StatusSnapshot snapshot = createStatusSnapshot();
        pImpl->statusHistory.push_back(snapshot);
        
        // Limit history size
        if (pImpl->statusHistory.size() > pImpl->config.maxTimelineEvents) {
            pImpl->statusHistory.erase(pImpl->statusHistory.begin());
        }
    }
    
    // Log if enabled
    if (pImpl->config.enableStatusLogging) {
        switch (level) {
            case StatusLevel::Error:
                Logger::getGlobal().error(message);
                break;
            case StatusLevel::Warning:
                Logger::getGlobal().warning(message);
                break;
            case StatusLevel::Info:
                Logger::getGlobal().info(message);
                break;
            case StatusLevel::Debug:
                Logger::getGlobal().debug(message);
                break;
            default:
                Logger::getGlobal().info(message);
                break;
        }
    }
}

std::vector<std::string> StatusManager::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    StatusSnapshot snapshot = createStatusSnapshot();
    return snapshot.activeAlerts;
}

std::vector<std::string> StatusManager::getWarningMessages() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    StatusSnapshot snapshot = createStatusSnapshot();
    return snapshot.warningMessages;
}

bool StatusManager::hasIssues() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    StatusSnapshot snapshot = createStatusSnapshot();
    
    // Check for any alerts or warnings
    return !snapshot.activeAlerts.empty() || !snapshot.warningMessages.empty();
}

size_t StatusManager::getIssueCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    StatusSnapshot snapshot = createStatusSnapshot();
    return snapshot.activeAlerts.size() + snapshot.warningMessages.size();
}

void StatusManager::updateStatusLevel() {
    // Determine overall status level based on component status
    StatusLevel newLevel = StatusLevel::Ready;
    
    if (pImpl->experimentStatus.isRunning) {
        if (pImpl->experimentStatus.isPaused) {
            newLevel = StatusLevel::Paused;
        } else {
            newLevel = StatusLevel::Running;
        }
    }
    
    if (!pImpl->brainStatus.getFiringNeuronCount()) {
        newLevel = StatusLevel::Ready;
    }
    
    if (hasIssues()) {
        newLevel = StatusLevel::Error;
    }
    
    if (pImpl->experimentStatus.completionPercentage >= 100.0) {
        newLevel = StatusLevel::Completed;
    }
    
    pImpl->overallStatusLevel = newLevel;
}

void StatusManager::createStatusSnapshotIfNeeded() {
    if (!pImpl->config.enableStatusSnapshots) {
        return;
    }
    
    // Create snapshot at regular intervals
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastStatusUpdate);
    
    if (elapsed.count() >= pImpl->config.statusInterval.count()) {
        StatusSnapshot snapshot = createStatusSnapshot();
        
        // Save to file if configured
        if (!pImpl->config.statusSnapshotFile.empty()) {
            std::ofstream file(pImpl->config.statusSnapshotFile, std::ios::app);
            if (file.is_open()) {
                file << snapshot.toText() << "\n---\n";
                file.close();
            }
        }
        
        pImpl->lastStatusUpdate = now;
    }
}

void StatusManager::logStatusIfNeeded() {
    if (!pImpl->config.enableStatusLogging) {
        return;
    }
    
    std::stringstream ss;
    ss << "Status Report - " << getStatusText() << "\n";
    ss << "Brain: " << pImpl->brainStatus.currentStep << " steps, " 
       << pImpl->brainStatus.neuronCount << " neurons\n";
    ss << "System: " << pImpl->systemStatus.cpuUsage << "% CPU, "
       << pImpl->systemStatus.getMemoryUsagePercentage() << "% memory\n";
    ss << "Experiment: " << pImpl->experimentStatus.completionPercentage << "% complete\n";
    
    if (hasIssues()) {
        ss << "ISSUES DETECTED: " << getIssueCount() << " issues found\n";
    }
    
    Logger::getGlobal().info(ss.str());
}

void StatusManager::exportStatusIfNeeded() {
    if (!pImpl->config.enableStatusExport) {
        return;
    }
    
    // Export status periodically or when explicitly requested
    saveStatusSnapshot(pImpl->config.statusExportFile, "json");
}

bool StatusManager::exportStatus(const std::string& filepath, const std::string& format) {
    return saveStatusSnapshot(filepath, format);
}

bool StatusManager::importStatus(const std::string& filepath, const std::string& format) {
    // For now, just log the attempt
    std::string message = "Attempting to import status from " + filepath;
    
    switch (format.empty() ? 't' : format[0]) {
        case 'j':
        case 'J':
            message += " (JSON format)";
            break;
        case 't':
        case 'T':
            message += " (Text format)";
            break;
        default:
            message += " (unknown format)";
            break;
    }
    
    addStatusEvent("StatusManager", message, StatusLevel::Info);
    return true; // For now, always return true
}

StatusManager& StatusManager::getGlobalStatusManager() {
    static std::shared_ptr<StatusManager> globalStatusManager;
    if (!globalStatusManager) {
        globalStatusManager = std::make_shared<StatusManager>();
    }
    return *globalStatusManager;
}

void StatusManager::setGlobalStatusManager(std::shared_ptr<StatusManager> manager) {
    static std::shared_ptr<StatusManager> globalStatusManager;
    globalStatusManager = manager;
}

} // namespace nlm