#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    VisualizationMode mode;
    OutputFormat outputFormat;
    bool realTimeEnabled;
    bool debugOverlayEnabled;
    bool interactiveMode;
    double frameRateLimit;
    uint64_t frameCount;
    std::chrono::steady_clock::time_point lastFrameTime;
    
    // Data storage
    std::vector<TimeSeriesDataPoint> timeSeriesData;
    std::vector<DebugInfo> debugInfo;
    std::vector<HeatMapData> heatMaps;
    std::chrono::steady_clock::time_point recordingStartTime;
    bool isRecording;
    std::string recordingFilename;
    
    // Callbacks
    std::function<void(const std::string&, const std::vector<double>&)> dataCallback;
    std::function<void(const std::string&, const std::string&)> eventCallback;
    
    // Performance tracking
    std::chrono::steady_clock::time_point lastUpdateTime;
    uint64_t updateCount;
    double totalUpdateTime;
    
    Impl() : active(false), updateRate(30.0), mode(VisualizationMode::RealTime),
             outputFormat(OutputFormat::Text), realTimeEnabled(true), 
             debugOverlayEnabled(true), interactiveMode(false), frameRateLimit(60.0),
             frameCount(0), isRecording(false), recordingStartTime(), updateCount(0),
             totalUpdateTime(0.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() {
    close();
}

void VisualizationInterface::initializeBackend() {
    // Default implementation - no backend
    pImpl->active = false;
}

void VisualizationInterface::cleanupBackend() {
    // Default implementation - no cleanup
    pImpl->active = false;
}

bool VisualizationInterface::isBackendInitialized() const {
    return pImpl->active;
}

void VisualizationInterface::enableRealTimeUpdates(bool enable) {
    pImpl->realTimeEnabled = enable;
    if (enable) {
        pImpl->lastFrameTime = std::chrono::steady_clock::now();
    }
}

bool VisualizationInterface::areRealTimeUpdatesEnabled() const {
    return pImpl->realTimeEnabled;
}

void VisualizationInterface::setFrameRateLimit(double maxFps) {
    pImpl->frameRateLimit = std::max(1.0, maxFps);
}

void VisualizationInterface::captureScreenshot(const std::string& filename) {
    NLM_LOG_INFO("Screenshot captured to: " + filename);
    // Default implementation - just log
}

void VisualizationInterface::startRecording(const std::string& filename) {
    pImpl->isRecording = true;
    pImpl->recordingFilename = filename;
    pImpl->recordingStartTime = std::chrono::steady_clock::now();
    NLM_LOG_INFO("Started recording to: " + filename);
}

void VisualizationInterface::stopRecording() {
    if (pImpl->isRecording) {
        pImpl->isRecording = false;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - pImpl->recordingStartTime
        );
        NLM_LOG_INFO("Stopped recording. Duration: " + std::to_string(duration.count()) + "s");
    }
}

void VisualizationInterface::setInteractiveMode(bool interactive) {
    pImpl->interactiveMode = interactive;
    if (interactive) {
        NLM_LOG_INFO("Interactive mode enabled");
    }
}

bool VisualizationInterface::isInteractiveMode() const {
    return pImpl->interactiveMode;
}

void VisualizationInterface::handleInput(const std::string& input) {
    // Default implementation - just log
    NLM_LOG_INFO("Input received: " + input);
}

std::string VisualizationInterface::getHelpText() const {
    return "Available commands: help, quit, visualize [brain|world], debug [info|memory|perf], export [filename], record [start|stop]";
}

void VisualizationInterface::enableDebugOverlay(bool enable) {
    pImpl->debugOverlayEnabled = enable;
    if (enable) {
        NLM_LOG_INFO("Debug overlay enabled");
    } else {
        NLM_LOG_INFO("Debug overlay disabled");
    }
}

bool VisualizationInterface::isDebugOverlayEnabled() const {
    return pImpl->debugOverlayEnabled;
}

void VisualizationInterface::addDebugInfo(const DebugInfo& debugInfo) {
    pImpl->debugInfo.push_back(debugInfo);
    // Keep only last 100 debug entries
    if (pImpl->debugInfo.size() > 100) {
        pImpl->debugInfo.erase(pImpl->debugInfo.begin());
    }
}

std::vector<DebugInfo> VisualizationInterface::getDebugInfo() const {
    return pImpl->debugInfo;
}

void VisualizationInterface::loadConfiguration(const std::string& configFile) {
    // Default implementation - just log
    NLM_LOG_INFO("Loading configuration from: " + configFile);
}

void VisualizationInterface::saveConfiguration(const std::string& configFile) const {
    NLM_LOG_INFO("Saving configuration to: " + configFile);
}

double VisualizationInterface::getLastFrameTime() const {
    if (pImpl->frameCount == 0) return 0.0;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - pImpl->lastFrameTime
    );
    return duration.count() / 1000.0; // Convert to seconds
}

uint64_t VisualizationInterface::getTotalFramesRendered() const {
    return pImpl->frameCount;
}

float VisualizationInterface::getFPS() const {
    double frameTime = getLastFrameTime();
    if (frameTime <= 0.0) return 0.0f;
    return 1.0f / static_cast<float>(frameTime);
}

void VisualizationInterface::resetPerformanceCounters() {
    pImpl->frameCount = 0;
    pImpl->updateCount = 0;
    pImpl->totalUpdateTime = 0.0;
    NLM_LOG_INFO("Performance counters reset");
}

void VisualizationInterface::setDataCallback(std::function<void(const std::string&, const std::vector<double>&)> callback) {
    pImpl->dataCallback = callback;
}

void VisualizationInterface::setEventCallback(std::function<void(const std::string&, const std::string&)> callback) {
    pImpl->eventCallback = callback;
}

// Default implementations for visualization methods
void VisualizationInterface::visualizeBrain(const Brain& brain) {
    visualizeNetwork(brain, true);
}

void VisualizationInterface::visualizeActivity(const Brain& brain, const std::vector<NeuronActivityData>& activityData) {
    NLM_LOG_INFO("Visualizing activity for " + std::to_string(activityData.size()) + " neurons");
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes, SimulationStep step) {
    NLM_LOG_INFO("Visualizing " + std::to_string(spikes.size()) + " spikes at step " + std::to_string(step));
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    NLM_LOG_INFO("Visualizing weights in brain");
}

void VisualizationInterface::visualizePopulations(const Brain& brain) {
    NLM_LOG_INFO("Visualizing populations in brain");
}

void VisualizationInterface::visualizeConnections(const Brain& brain, bool highlightPlastic) {
    NLM_LOG_INFO("Visualizing connections (plastic=" + std::string(highlightPlastic ? "true" : "false") + ")");
}

void VisualizationInterface::visualizeSynapses(const std::vector<SynapseActivityData>& synapseData) {
    NLM_LOG_INFO("Visualizing " + std::to_string(synapseData.size()) + " synapses");
}

void VisualizationInterface::visualizeWorld(const SimpleWorld& world) {
    NLM_LOG_INFO("Visualizing world: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + " units");
}

void VisualizationInterface::visualizeAgent(const AgentBrain& agent, const SimpleWorld& world) {
    NLM_LOG_INFO("Visualizing agent in world");
}

void VisualizationInterface::visualizeEnvironment(const std::vector<std::vector<float>>& environment) {
    NLM_LOG_INFO("Visualizing environment map: " + std::to_string(environment.size()) + "x" + std::to_string(environment[0].size()) + " units");
}

void VisualizationInterface::visualizeTimeSeries(const std::vector<TimeSeriesDataPoint>& data) {
    NLM_LOG_INFO("Visualizing time series with " + std::to_string(data.size()) + " data points");
}

void VisualizationInterface::addTimeSeriesMetric(const std::string& name, double value) {
    TimeSeriesDataPoint point;
    point.timestamp = std::chrono::steady_clock::now();
    point.metricName = name;
    point.value = value;
    pImpl->timeSeriesData.push_back(point);
    
    // Keep only last 1000 data points
    if (pImpl->timeSeriesData.size() > 1000) {
        pImpl->timeSeriesData.erase(pImpl->timeSeriesData.begin());
    }
    
    // Call data callback if set
    if (pImpl->dataCallback) {
        std::vector<double> values;
        for (const auto& dp : pImpl->timeSeriesData) {
            if (dp.metricName == name) {
                values.push_back(dp.value);
            }
        }
        pImpl->dataCallback(name, values);
    }
}

std::vector<TimeSeriesDataPoint> VisualizationInterface::getTimeSeriesData(const std::string& metricName) const {
    std::vector<TimeSeriesDataPoint> result;
    for (const auto& point : pImpl->timeSeriesData) {
        if (point.metricName == metricName) {
            result.push_back(point);
        }
    }
    return result;
}

HeatMapData VisualizationInterface::generateActivityHeatMap(const Brain& brain) {
    HeatMapData heatMap;
    heatMap.width = 100;
    heatMap.height = 100;
    heatMap.minValue = 0.0f;
    heatMap.maxValue = 1.0f;
    heatMap.colorScheme = "thermal";
    heatMap.heatValues = std::vector<std::vector<float>>(heatMap.height, std::vector<float>(heatMap.width, 0.0f));
    
    // Simple implementation - just fill with zeros for now
    NLM_LOG_INFO("Generated activity heat map with dimensions " + std::to_string(heatMap.width) + "x" + std::to_string(heatMap.height));
    return heatMap;
}

HeatMapData VisualizationInterface::generateWorldHeatMap(const SimpleWorld& world) {
    HeatMapData heatMap;
    heatMap.width = world.getWidth();
    heatMap.height = world.getHeight();
    heatMap.minValue = 0.0f;
    heatMap.maxValue = 1.0f;
    heatMap.colorScheme = "viridis";
    heatMap.heatValues = std::vector<std::vector<float>>(heatMap.height, std::vector<float>(heatMap.width, 0.0f));
    
    NLM_LOG_INFO("Generated world heat map with dimensions " + std::to_string(heatMap.width) + "x" + std::to_string(heatMap.height));
    return heatMap;
}

} // namespace nlm