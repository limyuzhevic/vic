#include "BrainMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/MonitoringManager.hpp"
#include "../performance/VisualizationEngine.hpp"
#include "../performance/ExportManager.hpp"
#include "../web/WebInterface.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace nlm {

struct BrainMonitor::Impl {
    std::shared_ptr<Brain> brain;
    IntegrationConfig config;
    BrainIntegrationState state;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastExportTime;
    std::chrono::milliseconds exportInterval;
    std::string exportDirectory;
    bool exportInitialized;
};

BrainMonitor::BrainMonitor() : running_(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->exportInitialized = false;
    pImpl->exportInterval = std::chrono::milliseconds(60000); // 1 minute default
}

BrainMonitor::~BrainMonitor() {
    stop();
}

bool BrainMonitor::initialize(const IntegrationConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    this->config_ = config;
    pImpl->config = config;
    pImpl->exportDirectory = config_.exportDirectory;
    pImpl->exportInterval = std::chrono::milliseconds(60000); // Default 1 minute
    
    // Setup export directory
    setupExportDirectory();
    
    // Setup notification callback
    setupNotificationCallback();
    
    NLM_LOG_INFO("BrainMonitor initialized");
    return true;
}

void BrainMonitor::start() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (!running_) {
        running_ = true;
        NLM_LOG_INFO("BrainMonitor started");
    }
}

void BrainMonitor::stop() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (running_) {
        running_ = false;
        NLM_LOG_INFO("BrainMonitor stopped");
    }
}

void BrainMonitor::configure(const IntegrationConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    this->config_ = config;
    pImpl->config = config;
    NLM_LOG_INFO("BrainMonitor configured");
}

void BrainMonitor::monitorBrain(const std::shared_ptr<Brain>& brain) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot monitor null brain");
        return;
    }
    
    // Cleanup previous brain if any
    if (pImpl->brain) {
        cleanupBrain(pImpl->brain);
    }
    
    // Initialize new brain
    pImpl->brain = brain;
    initializeBrain(pImpl->brain);
    
    NLM_LOG_INFO("Started monitoring brain with " + std::to_string(brain->getTotalNeuronCount()) + 
                " neurons and " + std::to_string(brain->getTotalSynapseCount()) + " synapses");
}

void BrainMonitor::monitorBrain(std::shared_ptr<Brain>&& brain) {
    monitorBrain(std::move(brain));
}

std::string BrainMonitor::getBrainStatus() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return generateBrainStatus();
}

std::string BrainMonitor::getIntegrationStatus() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return generateIntegrationStatus();
}

std::string BrainMonitor::getPerformanceStats() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return generatePerformanceStats();
}

bool BrainMonitor::exportBrainData(const std::string& filename, const std::string& format) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    return exportBrainMetrics(filename, format);
}

bool BrainMonitor::exportAllData(const std::string& prefix) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::vector<std::string> formats = {"csv", "json", "html"};
    std::vector<ExportResult> results;
    
    for (const auto& format : formats) {
        std::string filename = prefix + "." + format;
        ExportResult result = exportBrainMetrics(filename, format);
        results.push_back(result);
        
        if (!result.success) {
            NLM_LOG_WARNING("Export failed for " + format + ": " + result.message);
        }
    }
    
    // Count successful exports
    size_t successCount = std::count_if(results.begin(), results.end(),
                                       [](const ExportResult& r) { return r.success; });
    
    if (successCount > 0) {
        NLM_LOG_INFO("Exported brain data to " + std::to_string(successCount) + 
                    " formats: " + prefix);
        return true;
    }
    
    NLM_LOG_ERROR("All brain data exports failed");
    return false;
}

void BrainMonitor::updateBrainState() {
    if (!pImpl->brain) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->state.lastUpdate);
    
    if (elapsed < config_.updateInterval) {
        return; // Skip update
    }
    
    pImpl->state.lastUpdate = now;
    pImpl->state.lastStep = pImpl->brain->getTotalSpikeCount(); // Simplified
    pImpl->state.lastTime = pImpl->brain->getAverageFiringRate(); // Simplified
}

void BrainMonitor::updateMonitoring() {
    if (!pImpl->brain || !config_.enableBrainMonitoring) return;
    
    // Update MonitoringManager with brain data
    MonitoringManager::getInstance().update(*pImpl->brain, pImpl->state.lastTime, pImpl->state.lastStep);
}

void BrainMonitor::updateVisualization() {
    if (!pImpl->brain || !config_.enableVisualization) return;
    
    // Update VisualizationEngine with brain data
    VisualizationEngine::getInstance().update(*pImpl->brain, pImpl->state.lastTime, pImpl->state.lastStep);
}

void BrainMonitor::updateExport() {
    if (!pImpl->brain || !config_.enableExport) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastExportTime);
    
    if (elapsed < pImpl->exportInterval) {
        return; // Skip export
    }
    
    // Export data periodically
    std::string filename = pImpl->exportDirectory + "/brain_export_" + 
                          std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                              now.time_since_epoch()).count()) + ".csv";
    
    exportBrainMetrics(filename, "csv");
    pImpl->lastExportTime = now;
}

void BrainMonitor::updateWebInterface() {
    if (!pImpl->brain || !config_.enableWebInterface) return;
    
    // Update WebInterface if running
    if (WebInterface::getInstance().isRunning()) {
        WebInterface::updateGlobal(*pImpl->brain, pImpl->state.lastTime, pImpl->state.lastStep);
    }
}

void BrainMonitor::processBrainUpdates() {
    if (!pImpl->brain) return;
    
    // Process brain state updates
    updateBrainState();
    
    // Update monitoring and visualization systems
    updateMonitoring();
    updateVisualization();
    updateExport();
    updateWebInterface();
}

void BrainMonitor::handleBrainEvents() {
    if (!pImpl->brain) return;
    
    // Handle brain-specific events
    std::string status = getBrainStatus();
    
    if (pImpl->state.lastStatus != status) {
        fireBrainEvent("brain_state_changed", "Brain state changed: " + status);
        pImpl->state.lastStatus = status;
    }
}

void BrainMonitor::initializeBrain(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    pImpl->state.isInitialized = true;
    
    // Trigger integration event
    fireBrainEvent("brain_initialized", "Brain initialization complete");
}

void BrainMonitor::cleanupBrain(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    pImpl->state.isInitialized = false;
    pImpl->state.lastStatus.clear();
    
    // Trigger integration event
    fireBrainEvent("brain_cleaned_up", "Brain cleanup complete");
}

void BrainMonitor::updateBrainMetrics(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    // Update metrics based on brain state
    double neuronCount = static_cast<double>(brain->getTotalNeuronCount());
    double synapseCount = static_cast<double>(brain->getTotalSynapseCount());
    double spikeCount = static_cast<double>(brain->getTotalSpikeCount());
    double firingRate = brain->getAverageFiringRate();
    
    // Record metrics
    MetricsRecorder::getInstance().record("brain_neuron_count", neuronCount);
    MetricsRecorder::getInstance().record("brain_synapse_count", synapseCount);
    MetricsRecorder::getInstance().record("brain_spike_count", spikeCount);
    MetricsRecorder::getInstance().record("brain_firing_rate", firingRate);
}

void BrainMonitor::updateBrainVisualization(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    // Update visualization with brain data
    VisualizationEngine::getInstance().update(*brain, pImpl->state.lastTime, pImpl->state.lastStep);
}

bool BrainMonitor::exportBrainMetrics(const std::string& filename, const std::string& format) {
    if (!pImpl->brain) {
        return false;
    }
    
    std::string actualFilename = filename;
    if (actualFilename.empty()) {
        actualFilename = pImpl->exportDirectory + "/brain_export_" + 
                        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::high_resolution_clock::now().time_since_epoch()).count()) + "." + format;
    }
    
    try {
        // Create directory if it doesn't exist
        std::filesystem::create_directories(pImpl->exportDirectory);
        
        // Export metrics
        bool success = true;
        
        if (format == "csv") {
            std::ofstream file(actualFilename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for export: " + actualFilename);
                return false;
            }
            
            file << "Timestamp,Metric,Value\n";
            file << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count() 
                 << ",brain_neuron_count," << pImpl->brain->getTotalNeuronCount() << "\n";
            file << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count() 
                 << ",brain_synapse_count," << pImpl->brain->getTotalSynapseCount() << "\n";
            file << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count() 
                 << ",brain_spike_count," << pImpl->brain->getTotalSpikeCount() << "\n";
            file << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count() 
                 << ",brain_firing_rate," << pImpl->brain->getAverageFiringRate() << "\n";
            
            file.close();
            
        } else if (format == "json") {
            nlohmann::json jsonData;
            jsonData["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            jsonData["neuron_count"] = pImpl->brain->getTotalNeuronCount();
            jsonData["synapse_count"] = pImpl->brain->getTotalSynapseCount();
            jsonData["spike_count"] = pImpl->brain->getTotalSpikeCount();
            jsonData["firing_rate"] = pImpl->brain->getAverageFiringRate();
            
            std::ofstream file(actualFilename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for export: " + actualFilename);
                return false;
            }
            
            file << jsonData.dump(2);
            file.close();
        }
        
        NLM_LOG_INFO("Brain data exported to " + actualFilename);
        return success;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting brain data: " + std::string(e.what()));
        return false;
    }
}

bool BrainMonitor::exportBrainVisualization(const std::string& filename, const std::string& format) {
    if (!pImpl->brain) {
        return false;
    }
    
    std::string actualFilename = filename.empty() ? "brain_visualization." + format : filename;
    
    return VisualizationEngine::getInstance().exportVisualization(actualFilename, 
                                                                   static_cast<ExportFormat>(0)); // Simplified
}

bool BrainMonitor::exportBrainState(const std::string& filename, const std::string& format) {
    if (!pImpl->brain) {
        return false;
    }
    
    std::string actualFilename = filename.empty() ? "brain_state." + format : filename;
    
    try {
        std::filesystem::create_directories(pImpl->exportDirectory);
        
        if (format == "txt") {
            std::ofstream file(actualFilename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open file for state export: " + actualFilename);
                return false;
            }
            
            file << "Brain State Export\n";
            file << "===================\n\n";
            file << "Timestamp: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count() << " ms\n";
            file << "Neuron Count: " << pImpl->brain->getTotalNeuronCount() << "\n";
            file << "Synapse Count: " << pImpl->brain->getTotalSynapseCount() << "\n";
            file << "Active Neurons: " << pImpl->brain->getActiveNeuronCount() << "\n";
            file << "Firing Neurons: " << pImpl->brain->getFiringNeuronCount() << "\n";
            file << "Total Spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
            file << "Average Firing Rate: " << pImpl->brain->getAverageFiringRate() << " Hz\n";
            file << "E/I Ratio: " << pImpl->brain->getExcitationInhibitionRatio() << "\n";
            
            file.close();
        }
        
        NLM_LOG_INFO("Brain state exported to " + actualFilename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting brain state: " + std::string(e.what()));
        return false;
    }
}

void BrainMonitor::setupDefaultConfiguration() {
    if (!pImpl->exportInitialized) {
        pImpl->exportDirectory = "./brain_exports";
        pImpl->exportInitialized = true;
        NLM_LOG_INFO("BrainMonitor default configuration set up");
    }
}

void BrainMonitor::setupExportDirectory() {
    try {
        std::filesystem::create_directories(pImpl->exportDirectory);
        NLM_LOG_INFO("Export directory created: " + pImpl->exportDirectory);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create export directory: " + std::string(e.what()));
    }
}

void BrainMonitor::setupNotificationCallback() {
    if (!config_.notificationCallback.empty()) {
        // In a real implementation, this would set up a callback
        NLM_LOG_INFO("Notification callback configured: " + config_.notificationCallback);
    }
}

void BrainMonitor::fireBrainEvent(const std::string& eventType, const std::string& message) {
    NLM_LOG_INFO("[Brain Event] " + eventType + ": " + message);
    
    // Trigger notification callback if configured
    if (!config_.notificationCallback.empty()) {
        // In a real implementation, this would trigger the callback
        NLM_LOG_INFO("[Notification] " + message);
    }
}

void BrainMonitor::cleanOldExports(const std::chrono::milliseconds& age) const {
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoff = now - age;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(pImpl->exportDirectory)) {
            if (entry.is_regular_file()) {
                auto fileTime = std::chrono::high_resolution_clock::now(); // Simplified - would get actual file time
                if (fileTime < cutoff) {
                    std::filesystem::remove(entry.path());
                    NLM_LOG_INFO("Cleaned old export file: " + entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error cleaning old exports: " + std::string(e.what()));
    }
}

size_t BrainMonitor::getExportDirectorySize() const {
    size_t totalSize = 0;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(pImpl->exportDirectory)) {
            if (entry.is_regular_file()) {
                totalSize += entry.file_size();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error calculating export directory size: " + std::string(e.what()));
    }
    
    return totalSize;
}

std::vector<std::string> BrainMonitor::getExportFiles() const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(pImpl->exportDirectory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting export files: " + std::string(e.what()));
    }
    
    return files;
}

std::string BrainMonitor::generateBrainStatus() const {
    std::ostringstream oss;
    
    if (!pImpl->brain) {
        return "No brain is currently being monitored";
    }
    
    oss << "=== Brain Monitor Status ===\n";
    oss << "Brain: " << pImpl->brain->getTotalNeuronCount() << " neurons, " 
        << pImpl->brain->getTotalSynapseCount() << " synapses\n";
    oss << "Active: " << pImpl->brain->getActiveNeuronCount() << " neurons, " 
        << pImpl->brain->getFiringNeuronCount() << " firing\n";
    oss << "Performance: " << pImpl->brain->getAverageFiringRate() << " Hz firing rate, " 
        << pImpl->brain->getExcitationInhibitionRatio() << " E/I ratio\n";
    oss << "Total Spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
    oss << "Export Directory: " << pImpl->exportDirectory << "\n";
    oss << "Export Files: " << getExportFiles().size() << "\n";
    oss << "Export Size: " << getExportDirectorySize() / 1024 << " KB\n";
    oss << "===================================\n";
    
    return oss.str();
}

std::string BrainMonitor::generateIntegrationStatus() const {
    std::ostringstream oss;
    
    oss << "=== Integration Status ===\n";
    oss << "Brain Monitoring: " << (config_.enableBrainMonitoring ? "Enabled" : "Disabled") << "\n";
    oss << "Visualization: " << (config_.enableVisualization ? "Enabled" : "Disabled") << "\n";
    oss << "Export: " << (config_.enableExport ? "Enabled" : "Disabled") << "\n";
    oss << "Web Interface: " << (config_.enableWebInterface ? "Enabled" : "Disabled") << "\n";
    oss << "Update Interval: " << config_.updateInterval.count() << " ms\n";
    oss << "====================================\n";
    
    return oss.str();
}

std::string BrainMonitor::generatePerformanceStats() const {
    std::ostringstream oss;
    
    oss << "=== Performance Statistics ===\n";
    oss << "System uptime: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - pImpl->startTime).count() << " seconds\n";
    oss << "Brain monitoring active: " << (pImpl->brain ? "Yes" : "No") << "\n";
    oss << "Last update: " << (pImpl->state.lastUpdate.time_since_epoch().count() > 0 ? 
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - pImpl->state.lastUpdate).count()) + " ms ago" : 
        "Never") << "\n";
    oss << "=================================\n";
    
    return oss.str();
}

void BrainMonitor::monitorGlobalBrain(const std::shared_ptr<Brain>& brain) {
    getInstance().monitorBrain(brain);
}

void BrainMonitor::startGlobalMonitoring() {
    getInstance().start();
}

void BrainMonitor::stopGlobalMonitoring() {
    getInstance().stop();
}

bool BrainMonitor::isGlobalMonitoringRunning() {
    return getInstance().isRunning();
}

} // namespace nlm
