#include "VisualizationInterface.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cctype>

namespace nlm {

struct VisualizationInterface::Impl {
    VisualizationMode mode;
    std::string configFile;
    std::unordered_map<std::string, std::string> options;
    std::unordered_map<VisualizationEvent, std::vector<VisualizationEventHandler>> eventHandlers;
    std::unordered_map<VisualizationEvent, std::vector<std::function<bool(const VisualizationData&)>>> eventFilters;
    std::unordered_map<VisualizationEvent, std::function<void(const VisualizationData&)>> processedEvents;
    VisualizationData currentData;
    bool active;
    double updateRate;
    bool debugMode;
    bool performanceMonitoring;
    std::string recordingFile;
    std::ofstream recordingStream;
    bool isRecording;
    bool isReplaying;
    double replaySpeed;
    size_t replayIndex;
    std::vector<VisualizationData> replayData;
    std::chrono::high_resolution_clock::time_point lastUpdate;
    size_t frameCount;
    double startTime;
    
    Impl() : mode(VisualizationMode::Text), active(false), updateRate(30.0),
             debugMode(false), performanceMonitoring(false), isRecording(false),
             isReplaying(false), replaySpeed(1.0), replayIndex(0), frameCount(0),
             startTime(0.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {
    startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

VisualizationInterface::~VisualizationInterface() {
    if (pImpl->recordingStream.is_open()) {
        pImpl->recordingStream.close();
    }
}

void VisualizationInterface::configure(VisualizationMode mode, const std::string& configFile) {
    pImpl->mode = mode;
    pImpl->configFile = configFile;
    
    // Load configuration if provided
    if (!configFile.empty()) {
        std::ifstream file(configFile);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    pImpl->options[key] = value;
                }
            }
        }
    }
    
    // Apply mode-specific configuration
    switch (mode) {
        case VisualizationMode::Text:
            setOption("showNeuronDetails", "true");
            setOption("showPlasticity", "false");
            setOption("showMemory", "false");
            setOption("showNeuromodulation", "false");
            setOption("showPerformance", "true");
            break;
        case VisualizationMode::Simple:
            setOption("showBrainOutline", "true");
            setOption("showSynapses", "true");
            setOption("showNeurons", "true");
            setOption("showLabels", "false");
            break;
        case VisualizationMode::Advanced:
            setOption("enableHeatmap", "true");
            setOption("enableTimeSeries", "false");
            setOption("enableStatisticalAnalysis", "true");
            setOption("enablePatternDetection", "false");
            break;
        case VisualizationMode::Network:
            setOption("showConnections", "true");
            setOption("showNeuronTypes", "true");
            setOption("showSynapseWeights", "false");
            setOption("showPathways", "false");
            break;
        case VisualizationMode::TimeSeries:
            setOption("enableRealTimeUpdates", "true");
            setOption("updateInterval", "0.1");
            break;
        case VisualizationMode::3D:
            setOption("enable3D", "true");
            setOption("cameraPosition", "0,0,10");
            setOption("fieldOfView", "60");
            break;
        case VisualizationMode::Custom:
            // Custom mode - no preset options
            break;
    }
}

void VisualizationInterface::setOption(const std::string& option, const std::string& value) {
    pImpl->options[option] = value;
}

std::string VisualizationInterface::getOption(const std::string& option) const {
    auto it = pImpl->options.find(option);
    if (it != pImpl->options.end()) {
        return it->second;
    }
    return "";
}

void VisualizationInterface::addEventHandler(VisualizationEvent event, VisualizationEventHandler handler) {
    pImpl->eventHandlers[event].push_back(handler);
}

void VisualizationInterface::removeEventHandler(VisualizationEvent event) {
    pImpl->eventHandlers.erase(event);
}

void VisualizationInterface::clearEventHandlers() {
    pImpl->eventHandlers.clear();
}

void VisualizationInterface::update(const VisualizationData& data) {
    auto now = std::chrono::high_resolution_clock::now();
    double currentTime = std::chrono::duration<double>(now.time_since_epoch()).count();
    
    // Check if we need to update
    if (pImpl->mode == VisualizationMode::Text && currentTime - pImpl->lastUpdate < 1.0/30.0) {
        return; // Not enough time has passed
    }
    
    pImpl->lastUpdate = now;
    pImpl->currentData = data;
    pImpl->frameCount++;
    
    // Emit events for relevant changes
    emitEvent(VisualizationEvent::NeuronSpike, data);
    emitEvent(VisualizationEvent::SynapseActivation, data);
    emitEvent(VisualizationEvent::PlasticityChange, data);
    
    // Record if enabled
    if (pImpl->isRecording && pImpl->recordingStream.is_open()) {
        recordingStream << "Timestamp: " << data.timestamp << "\n";
        recordingStream << "SpikeCount: " << data.totalSpikeCount << "\n";
        recordingStream << "FiringNeurons: " << data.firingNeuronCount << "\n";
        recordingStream << "AverageFiringRate: " << data.averageFiringRate << "\n\n";
    }
    
    // Update replay if enabled
    if (pImpl->isReplaying && pImpl->replayIndex < pImpl->replayData.size()) {
        const VisualizationData& replayData = pImpl->replayData[pImpl->replayIndex];
        if (std::abs((now.time_since_epoch().count() - pImpl->startTime) / pImpl->replaySpeed - data.timestamp) < 0.001) {
            pImpl->replayIndex++;
        }
    }
}

void VisualizationInterface::emitEvent(VisualizationEvent event, const VisualizationData& data) {
    if (!pImpl->shouldFilterEvent(event, data)) {
        for (const auto& handler : pImpl->eventHandlers[event]) {
            handler(event, data);
        }
        
        // Process event
        pImpl->processedEvents[event] = data;
    }
}

bool VisualizationInterface::shouldFilterEvent(VisualizationEvent event, const VisualizationData& data) const {
    auto it = pImpl->eventFilters.find(event);
    if (it != pImpl->eventFilters.end()) {
        for (const auto& filter : it->second) {
            if (!filter(data)) {
                return true;
            }
        }
    }
    return false;
}

void VisualizationInterface::start() {
    pImpl->active = true;
    pImpl->startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void VisualizationInterface::stop() {
    pImpl->active = false;
}

bool VisualizationInterface::isRunning() const {
    return pImpl->active;
}

bool VisualizationInterface::saveVisualizationData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save data in a simple format
    file << "Visualization Data" << std::endl;
    file << "==================" << std::endl;
    file << "Timestamp: " << pImpl->currentData.timestamp << std::endl;
    file << "Neurons: " << pImpl->currentData.neuronStates.size() << std::endl;
    file << "Spikes: " << pImpl->currentData.totalSpikeCount << std::endl;
    file << "Firing neurons: " << pImpl->currentData.firingNeuronCount << std::endl;
    file << std::endl;
    
    return true;
}

bool VisualizationInterface::loadVisualizationData(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Timestamp: ") == 0) {
            pImpl->currentData.timestamp = std::stod(line.substr(12));
        } else if (line.find("Neurons: ") == 0) {
            // Parse neuron data (simplified)
        }
    }
    
    return true;
}

std::string VisualizationInterface::generateReport() const {
    std::ostringstream oss;
    oss << "=== Visualization Report ===" << std::endl;
    oss << "Active: " << (pImpl->active ? "Yes" : "No") << std::endl;
    oss << "Mode: " << static_cast<int>(pImpl->mode) << std::endl;
    oss << "Frames: " << pImpl->frameCount << std::endl;
    oss << "Duration: " << (pImpl->frameCount / pImpl->updateRate) << " seconds" << std::endl;
    
    if (pImpl->performanceMonitoring) {
        oss << "\n=== Performance Stats ===" << std::endl;
        oss << "Update rate: " << pImpl->updateRate << " Hz" << std::endl;
        oss << "Average frame time: " << (1000.0 / pImpl->updateRate) << " ms" << std::endl;
        oss << "Total events processed: " << pImpl->eventHandlers.size() << std::endl;
    }
    
    return oss.str();
}

bool VisualizationInterface::exportData(const std::string& format, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "csv") {
        file << "timestamp,neuron_count,spike_count,firing_rate" << std::endl;
        file << pImpl->currentData.timestamp << "," 
             << pImpl->currentData.neuronStates.size() << "," 
             << pImpl->currentData.totalSpikeCount << "," 
             << pImpl->currentData.averageFiringRate << std::endl;
    } else if (format == "json") {
        file << "{\n  \"timestamp\": " << pImpl->currentData.timestamp << ",\n"
             << "  \"neuron_count\": " << pImpl->currentData.neuronStates.size() << ",\n"
             << "  \"spike_count\": " << pImpl->currentData.totalSpikeCount << ",\n"
             << "  \"firing_rate\": " << pImpl->currentData.averageFiringRate << "\n"
             << "}\n";
    } else {
        return false;
    }
    
    return true;
}

void VisualizationInterface::enableDebugMode(bool enable) {
    pImpl->debugMode = enable;
    if (enable) {
        std::cout << "[DEBUG] Visualization debug mode enabled" << std::endl;
    }
}

bool VisualizationInterface::isDebugModeEnabled() const {
    return pImpl->debugMode;
}

void VisualizationInterface::addEventFilter(VisualizationEvent event, std::function<bool(const VisualizationData&)> filter) {
    pImpl->eventFilters[event].push_back(filter);
}

void VisualizationInterface::removeEventFilter(VisualizationEvent event) {
    pImpl->eventFilters.erase(event);
}

void VisualizationInterface::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoring = enable;
}

bool VisualizationInterface::isPerformanceMonitoringEnabled() const {
    return pImpl->performanceMonitoring;
}

std::string VisualizationInterface::getPerformanceStats() const {
    std::ostringstream oss;
    oss << "Performance monitoring: " << (pImpl->performanceMonitoring ? "enabled" : "disabled") << std::endl;
    oss << "Update rate: " << pImpl->updateRate << " Hz" << std::endl;
    oss << "Frames processed: " << pImpl->frameCount << std::endl;
    oss << "Average FPS: " << (pImpl->frameCount / (pImpl->frameCount > 0 ? pImpl->updateRate : 0)) << std::endl;
    return oss.str();
}

void VisualizationInterface::startRecording(const std::string& filepath) {
    pImpl->recordingFile = filepath;
    pImpl->recordingStream.open(filepath, std::ios::app);
    pImpl->isRecording = true;
}

void VisualizationInterface::stopRecording() {
    if (pImpl->recordingStream.is_open()) {
        pImpl->recordingStream.close();
    }
    pImpl->isRecording = false;
    pImpl->recordingFile = "";
}

bool VisualizationInterface::isRecording() const {
    return pImpl->isRecording;
}

bool VisualizationInterface::loadReplayData(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->replayData.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("VIS_DATA:") == 0) {
            // Parse saved visualization data
            // For simplicity, just read the entire file
            // In a real implementation, you would parse the saved data format
            pImpl->isReplaying = true;
            pImpl->replayIndex = ->replayData.size();
        }
    }
    
    return true;
}

void VisualizationInterface::startReplay(double speed) {
    pImpl->replaySpeed = speed;
    pImpl->isReplaying = true;
    pImpl->startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void VisualizationInterface::stopReplay() {
    pImpl->isReplaying = false;
    pImpl->replayIndex = 0;
}

bool VisualizationInterface::isReplaying() const {
    return pImpl->isReplaying;
}

double VisualizationInterface::getReplayTime() const {
    if (pImpl->isReplaying && !pImpl->replayData.empty()) {
        return pImpl->replayIndex * (1.0 / pImpl->replaySpeed);
    }
    return 0.0;
}

// Helper functions
std::string VisualizationInterface::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm

