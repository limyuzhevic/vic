#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/PerformanceProfiler.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

namespace nlm {

struct VisualizationInterface::Impl {
    bool initialized;
    bool active;
    double updateRate;
    bool headlessMode;
    std::string configPath;
    
    // Visualization state
    bool monitoring;
    ProgressReport currentProgress;
    PerformanceMetrics lastMetrics;
    std::string currentRenderMode;
    std::string currentColorMap;
    std::string currentLayout;
    
    // Performance profiler integration
    PerformanceProfiler* profiler;
    CheckpointManager* checkpointManager;
    
    // Network visualization data
    std::vector<NeuronId> neuronIds;
    std::vector<SynapseId> synapseIds;
    std::map<NeuronId, std::vector<SynapseId>> neuronToSynapses;
    std::map<SynapseId, std::vector<NeuronId>> synapseToNeurons;
    
    // Activity visualization data
    std::deque<double> activityHistory;
    std::vector<double> neuronActivities;
    double maxActivity;
    
    // Performance metrics history
    std::deque<PerformanceMetrics> metricsHistory;
    
    // Event handling
    VisualizationEventCallback eventCallback;
    
    // Rendering thread
    std::thread renderThread;
    std::atomic<bool> renderRunning;
    std::mutex dataMutex;
    
    Impl() : initialized(false), active(false), updateRate(30.0),
             headlessMode(false), monitoring(false), profiler(nullptr),
             checkpointManager(nullptr), maxActivity(1.0),
             renderRunning(false) {
        activityHistory.resize(1000, 0.0);
        neuronActivities.reserve(10000);
        metricsHistory.reserve(1000);
    }
    
    ~Impl() {
        if (renderRunning) {
            stopRendering();
        }
    }
    
    void initializeThread() {
        renderRunning = true;
        renderThread = std::thread(&Impl::renderingLoop, this);
    }
    
    void stopRendering() {
        renderRunning = false;
        if (renderThread.joinable()) {
            renderThread.join();
        }
    }
    
    void renderingLoop() {
        auto nextFrameTime = std::chrono::steady_clock::now();
        
        while (renderRunning) {
            auto now = std::chrono::steady_clock::now();
            
            if (now >= nextFrameTime) {
                renderFrame();
                
                // Calculate next frame time
                double frameTime = 1.0 / updateRate;
                nextFrameTime = now + std::chrono::duration<double>(frameTime);
                
                // Sleep if we overslept
                if (std::chrono::steady_clock::now() < nextFrameTime) {
                    std::this_thread::sleep_until(nextFrameTime);
                }
            } else {
                // Sleep until next frame
                std::this_thread::sleep_until(nextFrameTime);
            }
        }
    }
    
    void renderFrame() {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        if (!active) return;
        
        // Update activity history
        if (!neuronActivities.empty()) {
            double totalActivity = std::accumulate(neuronActivities.begin(),
                                                   neuronActivities.end(), 0.0);
            double avgActivity = totalActivity / neuronActivities.size();
            
            activityHistory.pop_front();
            activityHistory.push_back(avgActivity);
            
            maxActivity = *std::max_element(neuronActivities.begin(),
                                           neuronActivities.end());
        }
        
        // Update metrics history
        if (profiler) {
            lastMetrics = profiler->getCurrentMetrics();
            metricsHistory.push_back(lastMetrics);
            if (metricsHistory.size() > 1000) {
                metricsHistory.pop_front();
            }
        }
    }
    
    void extractNetworkData(const Brain& brain) {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        neuronIds.clear();
        synapseIds.clear();
        neuronToSynapses.clear();
        synapseToNeurons.clear();
        
        // Extract neuron data
        for (const auto& region : brain.getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    neuronIds.push_back(neuron->getId());
                }
            }
        }
        
        // Extract synapse data
        for (const auto& region : brain.getRegions()) {
            for (const auto* synapse : region->getSynapses()) {
                synapseIds.push_back(synapse->getId());
                neuronToSynapses[synapse->getSourceNeuron()].push_back(synapse->getId());
                synapseToNeurons[synapse->getId()].push_back(synapse->getSourceNeuron());
                synapseToNeurons[synapse->getId()].push_back(synapse->getDestinationNeuron());
            }
        }
        
        neuronActivities.resize(neuronIds.size(), 0.0);
    }
    
    void updateActivityData(const Brain& brain) {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        if (neuronActivities.empty()) return;
        
        // Update neuron activities based on membrane potential
        size_t idx = 0;
        for (const auto& region : brain.getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronActivities[idx] = std::abs(state.membranePotential - state.restingPotential);
                    idx++;
                }
            }
        }
    }
    
    void generateHeatmapData(std::vector<float>& heatmapData, int regionCount, int neuronsPerRegion) {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        heatmapData.clear();
        heatmapData.resize(regionCount * neuronsPerRegion, 0.0f);
        
        if (neuronActivities.empty() || neuronActivities.size() != static_cast<size_t>(regionCount * neuronsPerRegion)) {
            return;
        }
        
        double minActivity = *std::min_element(neuronActivities.begin(), neuronActivities.end());
        double maxActivity = *std::max_element(neuronActivities.begin(), neuronActivities.end());
        double range = maxActivity - minActivity;
        
        if (range > 0.0) {
            for (size_t i = 0; i < neuronActivities.size(); ++i) {
                float normalized = static_cast<float>((neuronActivities[i] - minActivity) / range);
                heatmapData[i] = normalized;
            }
        }
    }
    
    void saveScreenshot(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            NLM_LOG_ERROR("Failed to save screenshot: " + filepath);
            return;
        }
        
        // Write simple PPM format for debugging
        file << "P6\n" << "512 512\n" << "255\n";
        
        // Write some placeholder pixel data
        for (int y = 0; y < 512; ++y) {
            for (int x = 0; x < 512; ++x) {
                uint8_t r = static_cast<uint8_t>((x + y) % 256);
                uint8_t g = static_cast<uint8_t>((x * 2) % 256);
                uint8_t b = static_cast<uint8_t>((y * 2) % 256);
                file.write(reinterpret_cast<const char*>(&r), 1);
                file.write(reinterpret_cast<const char*>(&g), 1);
                file.write(reinterpret_cast<const char*>(&b), 1);
            }
        }
        
        NLM_LOG_INFO("Screenshot saved: " + filepath);
    }
    
    void exportAnimation(const std::string& filepath, double fps, int durationSeconds) {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        NLM_LOG_INFO("Animation export not yet implemented: " + filepath);
    }
    
    void notifyEvent(const std::string& event) {
        if (eventCallback) {
            eventCallback(event);
        }
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(bool headless, const std::string& configPath) {
    if (pImpl->initialized) {
        return true;
    }
    
    pImpl->headlessMode = headless;
    pImpl->configPath = configPath;
    
    pImpl->initialized = true;
    pImpl->active = !headless;
    
    NLM_LOG_INFO("VisualizationInterface initialized (headless: " + 
                 std::string(headless ? "true" : "false") + ")");
    
    if (!headless) {
        pImpl->initializeThread();
    }
    
    notifyEvent("initialized");
    return true;
}

void VisualizationInterface::update(const Brain& brain, PerformanceProfiler* profiler,
                                    CheckpointManager* checkpointManager) {
    if (!pImpl->initialized) return;
    
    pImpl->profiler = profiler;
    pImpl->checkpointManager = checkpointManager;
    
    // Extract network data
    pImpl->extractNetworkData(brain);
    
    // Update activity visualization data
    pImpl->updateActivityData(brain);
    
    // Update metrics
    if (pImpl->profiler) {
        pImpl->lastMetrics = pImpl->profiler->getCurrentMetrics();
    }
}

void VisualizationInterface::render() {
    if (!pImpl->initialized || !pImpl->active) return;
    
    // Rendering is handled in the render thread
    notifyEvent("render_requested");
}

void VisualizationInterface::close() {
    if (!pImpl->initialized) return;
    
    pImpl->stopRendering();
    pImpl->active = false;
    pImpl->initialized = false;
    
    notifyEvent("closed");
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = std::max(0.1, hz);
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

bool VisualizationInterface::setRenderMode(const std::string& mode) {
    if (mode == "network" || mode == "activity" || mode == "weights" || mode == "spikes") {
        pImpl->currentRenderMode = mode;
        notifyEvent("render_mode_changed:" + mode);
        return true;
    }
    return false;
}

bool VisualizationInterface::setColorMap(const std::string& colormap) {
    if (colormap == "thermal" || colormap == "viridis" || colormap == "plasma" || colormap == "gray") {
        pImpl->currentColorMap = colormap;
        notifyEvent("colormap_changed:" + colormap);
        return true;
    }
    return false;
}

bool VisualizationInterface::setNetworkLayout(const std::string& layout) {
    if (layout == "2d" || layout == "3d" || layout == "circular" || layout == "force-directed") {
        pImpl->currentLayout = layout;
        notifyEvent("layout_changed:" + layout);
        return true;
    }
    return false;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    setRenderMode("network");
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    setRenderMode("activity");
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    setRenderMode("spikes");
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    setRenderMode("weights");
}

void VisualizationInterface::visualizeProgress(double progress, const std::string& message) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->currentProgress = ProgressReport(progress, message);
    
    std::string progressMsg = "Progress: " + std::to_string(static_cast<int>(progress * 100)) + "%";
    if (!message.empty()) {
        progressMsg += " - " + message;
    }
    
    NLM_LOG_INFO(progressMsg);
    notifyEvent("progress_update:" + std::to_string(progress * 100) + ":" + message);
}

void VisualizationInterface::visualizeMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->lastMetrics = metrics;
    
    NLM_LOG_INFO("Metrics - CPU: " + std::to_string(metrics.cpuUsage) + "%, " +
                 "Memory: " + std::to_string(metrics.memoryUsed / 1024 / 1024) + " MB, " +
                 "Spikes: " + std::to_string(metrics.totalSpikes) + ", " +
                 "Active neurons: " + std::to_string(metrics.activeNeurons));
}

void VisualizationInterface::visualizeCheckpointStatus(const std::string& checkpointPath,
                                                       double progress) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    NLM_LOG_INFO("Checkpoint status: " + checkpointPath + " (" +
                 std::to_string(static_cast<int>(progress * 100)) + "%)");
}

bool VisualizationInterface::setCameraPosition(double x, double y, double z) {
    NLM_LOG_INFO("Camera position set to: (" + std::to_string(x) + ", " +
                 std::to_string(y) + ", " + std::to_string(z) + ")");
    return true;
}

bool VisualizationInterface::setZoomLevel(double level) {
    NLM_LOG_INFO("Zoom level set to: " + std::to_string(level));
    return true;
}

bool VisualizationInterface::resetView() {
    NLM_LOG_INFO("View reset");
    return true;
}

bool VisualizationInterface::saveScreenshot(const std::string& filepath) {
    pImpl->saveScreenshot(filepath);
    return true;
}

bool VisualizationInterface::exportAnimation(const std::string& filepath, double fps,
                                             int durationSeconds) {
    pImpl->exportAnimation(filepath, fps, durationSeconds);
    return true;
}

void VisualizationInterface::startMonitoring() {
    if (!pImpl->initialized) return;
    
    pImpl->monitoring = true;
    notifyEvent("monitoring_started");
}

void VisualizationInterface::stopMonitoring() {
    if (!pImpl->initialized) return;
    
    pImpl->monitoring = false;
    notifyEvent("monitoring_stopped");
}

bool VisualizationInterface::isMonitoring() const {
    return pImpl->monitoring;
}

void VisualizationInterface::setEventCallback(VisualizationEventCallback callback) {
    pImpl->eventCallback = callback;
}

} // namespace nlm