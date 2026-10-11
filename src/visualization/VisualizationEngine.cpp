#include "VisualizationEngine.hpp"
#include "../brain/Brain.hpp"
#include "../debugging/DebugConsole.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <thread>

namespace nlm {

struct VisualizationEngine::Impl {
    VisualizationEngineConfig config;
    const Brain* brain;
    DebugConsole* debugConsole;
    
    // State
    bool active;
    bool paused;
    double lastUpdateTime;
    std::chrono::steady_clock::time_point lastFrameTime;
    
    // Visualization data
    NeuralNetworkData networkData;
    SpikeData spikeData;
    SynapticData synapticData;
    BrainActivityData brainActivityData;
    MemoryData memoryData;
    
    // Statistics
    VisualizationStats stats;
    
    // Performance
    std::atomic<bool> running;
    std::thread updateThread;
    std::mutex dataMutex;
    
    // Backend-specific data (placeholder)
    void* backendData;
    
    Impl(const VisualizationEngineConfig& cfg) : config(cfg), brain(nullptr), debugConsole(nullptr),
                                                 active(false), paused(false), lastUpdateTime(0.0),
                                                 running(false), backendData(nullptr) {
        // Initialize data structures
        networkData.spikes.clear();
        spikeData.spikes.clear();
        synapticData.synapses.clear();
        brainActivityData.currentStep = 0;
        memoryData.workingMemoryTrace.clear();
        memoryData.episodicMemoryItems.clear();
    }
    
    ~Impl() {
        shutdown();
    }
    
    void shutdown() {
        running = false;
        if (updateThread.joinable()) {
            updateThread.join();
        }
        
        // Cleanup backend resources
        if (backendData) {
            // TODO: Backend-specific cleanup
            backendData = nullptr;
        }
    }
};

VisualizationEngine::VisualizationEngine() : pImpl(std::make_unique<Impl>(VisualizationEngineConfig())) {}

VisualizationEngine::VisualizationEngine(const VisualizationEngineConfig& config) : pImpl(std::make_unique<Impl>(config)) {}

VisualizationEngine::~VisualizationEngine() = default;

bool VisualizationEngine::initialize() {
    if (pImpl->active) return true;
    
    // Validate configuration
    if (config.windows.empty()) {
        // Create default windows if none specified
        config.windows.push_back(VisualizationWindow("Network View", 0.0, 0.0, 0.5, 1.0, VisualizationMode::NetworkGraph));
        config.windows.push_back(VisualizationWindow("Activity View", 0.5, 0.0, 0.5, 0.5, VisualizationMode::BrainActivity));
        config.windows.push_back(VisualizationWindow("Spike Raster", 0.0, 0.5, 1.0, 0.5, VisualizationMode::SpikeRaster));
    }
    
    // Initialize backend (placeholder for now)
    pImpl->backendData = nullptr; // TODO: Initialize backend resources
    
    // Start update thread if not paused
    if (!pImpl->paused) {
        pImpl->running = true;
        pImpl->updateThread = std::thread(&VisualizationEngine::updateLoop, this);
    }
    
    pImpl->active = true;
    pImpl->lastUpdateTime = 0.0;
    pImpl->lastFrameTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("VisualizationEngine: Initialized successfully");
    return true;
}

void VisualizationEngine::update(const Brain* brain, double deltaTime) {
    if (!pImpl->brain || pImpl->paused) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->lastUpdateTime += deltaTime;
    
    // Update data structures
    updateNetworkData(brain);
    updateSpikeData(brain);
    updateSynapticData(brain);
    updateBrainActivityData(brain);
    updateMemoryData(brain);
    
    // Trigger update callbacks
    for (auto& callback : pImpl->config.updateCallbacks) {
        for (auto& window : pImpl->config.windows) {
            if (window.mode == callback.mode && window.visible) {
                try {
                    callback.callback(window, brain, deltaTime);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Visualization callback error: ") + e.what());
                }
            }
        }
    }
}

void VisualizationEngine::render() {
    if (!pImpl->active || pImpl->paused) return;
    
    auto now = std::chrono::steady_clock::now();
    auto frameTime = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->lastFrameTime).count();
    pImpl->lastFrameTime = now;
    
    // Update stats
    pImpl->stats.framesRendered++;
    pImpl->stats.lastFrameTime = frameTime;
    pImpl->stats.averageFrameTime = (pImpl->stats.averageFrameTime * (pImpl->stats.framesRendered - 1) + frameTime) / pImpl->stats.framesRendered;
    pImpl->stats.fps = 1.0 / std::max(frameTime, 0.001);
    
    // Render each window (placeholder implementation)
    for (auto& window : pImpl->config.windows) {
        if (!window.visible) continue;
        
        // TODO: Backend-specific rendering
        renderWindow(window);
    }
}

void VisualizationEngine::close() {
    pImpl->active = false;
    pImpl->shutdown();
    
    NLM_LOG_INFO("VisualizationEngine: Closed");
}

void VisualizationEngine::updateConfig(const VisualizationEngineConfig& config) {
    pImpl->config = config;
}

const VisualizationEngineConfig& VisualizationEngine::getConfig() const {
    return pImpl->config;
}

void VisualizationEngine::addWindow(const VisualizationWindow& window) {
    pImpl->config.windows.push_back(window);
}

void VisualizationEngine::removeWindow(const std::string& title) {
    auto it = std::remove_if(pImpl->config.windows.begin(), pImpl->config.windows.end(),
                            [&title](const VisualizationWindow& w) { return w.title == title; });
    pImpl->config.windows.erase(it, pImpl->config.windows.end());
}

void VisualizationEngine::clearWindows() {
    pImpl->config.windows.clear();
}

VisualizationWindow* VisualizationEngine::getWindow(const std::string& title) {
    for (auto& window : pImpl->config.windows) {
        if (window.title == title) return &window;
    }
    return nullptr;
}

const VisualizationWindow* VisualizationEngine::getWindow(const std::string& title) const {
    for (const auto& window : pImpl->config.windows) {
        if (window.title == title) return &window;
    }
    return nullptr;
}

void VisualizationEngine::addUpdateCallback(const VisualizationUpdateCallback& callback) {
    pImpl->config.updateCallbacks.push_back(callback);
}

void VisualizationEngine::removeUpdateCallback(VisualizationMode mode) {
    auto it = std::remove_if(pImpl->config.updateCallbacks.begin(), pImpl->config.updateCallbacks.end(),
                            [mode](const VisualizationUpdateCallback& cb) { return cb.mode == mode; });
    pImpl->config.updateCallbacks.erase(it, pImpl->config.updateCallbacks.end());
}

void VisualizationEngine::clearUpdateCallbacks() {
    pImpl->config.updateCallbacks.clear();
}

NeuralNetworkData VisualizationEngine::getNetworkData() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->networkData;
}

SpikeData VisualizationEngine::getSpikeData() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->spikeData;
}

SynapticData VisualizationEngine::getSynapticData() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->synapticData;
}

BrainActivityData VisualizationEngine::getBrainActivityData() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->brainActivityData;
}

MemoryData VisualizationEngine::getMemoryData() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->memoryData;
}

void VisualizationEngine::pause() {
    pImpl->paused = true;
    if (pImpl->running && pImpl->updateThread.joinable()) {
        pImpl->running = false;
        pImpl->updateThread.join();
    }
}

void VisualizationEngine::resume() {
    if (pImpl->paused) {
        pImpl->paused = false;
        pImpl->running = true;
        pImpl->updateThread = std::thread(&VisualizationEngine::updateLoop, this);
    }
}

bool VisualizationEngine::isPaused() const {
    return pImpl->paused;
}

const VisualizationStats& VisualizationEngine::getStats() const {
    return pImpl->stats;
}

bool VisualizationEngine::saveView(const std::string& filepath) {
    // Save window configuration
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement actual saving of view configuration
    // For now, just save basic info
    file << "Visualization view data" << std::endl;
    file << "Timestamp: " << std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count() << std::endl;
    
    file.close();
    return true;
}

bool VisualizationEngine::loadView(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement actual loading of view configuration
    file.close();
    return true;
}

bool VisualizationEngine::exportVisualization(const std::string& format, const std::string& filepath) {
    // Export visualization data in various formats
    std::stringstream ss;
    
    if (format == "json") {
        exportToJSON(ss);
    } else if (format == "csv") {
        exportToCSV(ss);
    } else if (format == "png") {
        // TODO: Implement PNG export
        return false;
    } else {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << ss.str();
    file.close();
    return true;
}

void VisualizationEngine::onMouseClick(double x, double y, int button) {
    // Handle mouse click events
    for (auto& window : pImpl->config.windows) {
        if (window.visible && x >= window.x && x <= window.x + window.width &&
            y >= window.y && y <= window.y + window.height) {
            // TODO: Handle click for this window
            break;
        }
    }
}

void VisualizationEngine::onMouseMove(double x, double y) {
    // Handle mouse move events
    for (auto& window : pImpl->config.windows) {
        if (window.visible && x >= window.x && x <= window.x + window.width &&
            y >= window.y && y <= window.y + window.height) {
            // TODO: Handle mouse move for this window
            break;
        }
    }
}

void VisualizationEngine::onKeyPress(int key, int mods) {
    // Handle key press events
    // TODO: Implement key handling
}

void VisualizationEngine::onScroll(double offsetX, double offsetY) {
    // Handle scroll events
    // TODO: Implement scroll handling
}

void VisualizationEngine::setDebugConsole(DebugConsole* console) {
    pImpl->debugConsole = console;
}

bool VisualizationEngine::isActive() const {
    return pImpl->active;
}

// Private methods
void VisualizationEngine::updateLoop() {
    double lastTime = 0.0;
    
    while (pImpl->running && pImpl->brain) {
        double currentTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
        double deltaTime = currentTime - lastTime;
        
        if (deltaTime >= config.updateInterval) {
            update(pImpl->brain, deltaTime);
            lastTime = currentTime;
        }
        
        // Sleep briefly to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void VisualizationEngine::updateNetworkData(const Brain* brain) {
    if (!brain) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->networkData.neurons.clear();
    pImpl->networkData.connections.clear();
    pImpl->networkData.excitatoryConnections.clear();
    pImpl->networkData.inhibitoryConnections.clear();
    pImpl->networkData.neuronActivations.clear();
    pImpl->networkData.synapticWeights.clear();
    
    // Add all neurons
    pImpl->networkData.neurons.reserve(brain->getTotalNeuronCount());
    for (const auto& regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (const auto* pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                pImpl->networkData.neurons.push_back(neuron->getId());
                
                // Calculate activation based on membrane potential
                float activation = (neuron->getMembranePotential() + 70.0f) / 120.0f; // Normalize to [0, 1]
                activation = std::max(0.0f, std::min(1.0f, activation));
                pImpl->networkData.neuronActivations[neuron->getId()] = activation;
            }
        }
    }
    
    // Add connections
    for (const auto& regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (const auto* synapse : region->getSynapses()) {
            auto connection = std::make_pair(synapse->getSourceNeuron(), synapse->getDestinationNeuron());
            pImpl->networkData.connections.push_back(connection);
            
            float weight = synapse->getWeight();
            pImpl->networkData.synapticWeights[connection] = weight;
            
            if (synapse->isExcitatory()) {
                pImpl->networkData.excitatoryConnections.push_back(connection);
            } else {
                pImpl->networkData.inhibitoryConnections.push_back(connection);
            }
        }
    }
    
    pImpl->stats.networkUpdated = true;
}

void VisualizationEngine::updateSpikeData(const Brain* brain) {
    if (!brain) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // Get current spike system data
    const auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) return;
    
    // Get current step and time
    pImpl->spikeData.startStep = pImpl->spikeData.currentStep;
    pImpl->spikeData.endStep = spikeSystem->getCurrentStep();
    
    // TODO: Get actual spike events from spike system
    // For now, this is a placeholder
    
    pImpl->stats.spikesUpdated = true;
}

void VisualizationEngine::updateSynapticData(const Brain* brain) {
    if (!brain) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->synapticData.synapses.clear();
    pImpl->synapticData.weights.clear();
    pImpl->synapticData.delays.clear();
    pImpl->synapticData.types.clear();
    
    // Add all synapses from all regions
    for (const auto& regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (!region) continue;
        
        for (const auto* synapse : region->getSynapses()) {
            pImpl->synapticData.synapses.push_back(synapse->getId());
            pImpl->synapticData.weights[synapse->getId()] = synapse->getWeight();
            pImpl->synapticData.delays[synapse->getId()] = synapse->getDelay();
            pImpl->synapticData.types[synapse->getId()] = synapse->getType();
        }
    }
    
    pImpl->stats.weightsUpdated = true;
}

void VisualizationEngine::updateBrainActivityData(const Brain* brain) {
    if (!brain) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->brainActivityData.currentStep = brain->getCurrentStep();
    pImpl->brainActivityData.currentTime = brain->getCurrentTime();
    pImpl->brainActivityData.totalNeurons = brain->getTotalNeuronCount();
    pImpl->brainActivityData.activeNeurons = brain->getActiveNeuronCount();
    pImpl->brainActivityData.firingNeurons = brain->getFiringNeuronCount();
    pImpl->brainActivityData.averageFiringRate = brain->getAverageFiringRate();
    pImpl->brainActivityData.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    
    // Update region activities
    pImpl->brainActivityData.regionActivities.clear();
    for (const auto& regionId : brain->getRegionIds()) {
        auto* region = brain->getRegion(regionId);
        if (region) {
            float activity = static_cast<float>(region->getActiveCount()) / std::max(1.0f, static_cast<float>(region->getTotalNeuronCount()));
            pImpl->brainActivityData.regionActivities[region->getName()] = activity;
        }
    }
    
    // TODO: Get neuromodulator levels
    pImpl->brainActivityData.neuromodulatorLevels.clear();
    
    pImpl->stats.activityUpdated = true;
}

void VisualizationEngine::updateMemoryData(const Brain* brain) {
    if (!brain) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // TODO: Get actual memory system data
    // For now, this is a placeholder
    
    pImpl->stats.memoryUpdated = true;
}

void VisualizationEngine::renderWindow(VisualizationWindow& window) {
    // Placeholder rendering implementation
    // TODO: Implement actual rendering based on window mode
    
    switch (window.mode) {
        case VisualizationMode::NetworkGraph:
            renderNetworkGraph(window);
            break;
        case VisualizationMode::SpikeRaster:
            renderSpikeRaster(window);
            break;
        case VisualizationMode::SynapticWeights:
            renderSynapticWeights(window);
            break;
        case VisualizationMode::BrainActivity:
            renderBrainActivity(window);
            break;
        case VisualizationMode::MemorySystem:
            renderMemorySystem(window);
            break;
        case VisualizationMode::Neuromodulation:
            renderNeuromodulation(window);
            break;
        case VisualizationMode::Development:
            renderDevelopment(window);
            break;
        default:
            renderCustom(window);
            break;
    }
}

void VisualizationEngine::renderNetworkGraph(VisualizationWindow& window) {
    // TODO: Implement network graph rendering
}

void VisualizationEngine::renderSpikeRaster(VisualizationWindow& window) {
    // TODO: Implement spike raster rendering
}

void VisualizationEngine::renderSynapticWeights(VisualizationWindow& window) {
    // TODO: Implement synaptic weights rendering
}

void VisualizationEngine::renderBrainActivity(VisualizationWindow& window) {
    // TODO: Implement brain activity rendering
}

void VisualizationEngine::renderMemorySystem(VisualizationWindow& window) {
    // TODO: Implement memory system rendering
}

void VisualizationEngine::renderNeuromodulation(VisualizationWindow& window) {
    // TODO: Implement neuromodulation rendering
}

void VisualizationEngine::renderDevelopment(VisualizationWindow& window) {
    // TODO: Implement development rendering
}

void VisualizationEngine::renderCustom(VisualizationWindow& window) {
    // TODO: Implement custom rendering
}

void VisualizationEngine::exportToJSON(std::stringstream& ss) {
    // Export visualization data as JSON
    // TODO: Implement JSON export
}

void VisualizationEngine::exportToCSV(std::stringstream& ss) {
    // Export visualization data as CSV
    // TODO: Implement CSV export
}

} // namespace nlm
