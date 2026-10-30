#include "VisualizationInterface.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    VisualizationMode mode;
    VisualizationOutput output;
    
    // Backend information
    bool glfwAvailable;
    bool sdlAvailable;
    bool webAvailable;
    
    // Window and display information
    int windowWidth;
    int windowHeight;
    std::string windowTitle;
    
    // Threading for real-time updates
    std::atomic<bool> updateThreadRunning;
    std::thread updateThread;
    std::mutex updateMutex;
    std::condition_variable updateCondition;
    std::queue<double> updateQueue;
    
    // Visualization data cache
    std::vector<std::pair<size_t, size_t>> cachedConnections;
    std::vector<double> cachedActivityLevels;
    std::vector<std::pair<double, size_t>> cachedSpikeTimes;
    std::vector<std::vector<double>> cachedWeights;
    
    Impl() : active(false), updateRate(30.0), mode(VisualizationMode::None), 
             output(VisualizationOutput::Console), glfwAvailable(false), 
             sdlAvailable(false), webAvailable(false), windowWidth(1024), 
             windowHeight(768), updateThreadRunning(false) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl()) {
    // Detect available backends
    pImpl->glfwAvailable = isGLFWAvailable();
    pImpl->sdlAvailable = isSDLAvailable();
    pImpl->webAvailable = isWebAssemblyAvailable();
}

VisualizationInterface::~VisualizationInterface() {
    close();
}

VisualizationInterface::VisualizationInterface(VisualizationInterface&& other) noexcept 
    : pImpl(std::move(other.pImpl)) {}

VisualizationInterface& VisualizationInterface::operator=(VisualizationInterface&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool VisualizationInterface::initialize(VisualizationOutput output, VisualizationMode mode) {
    pImpl->output = output;
    pImpl->mode = mode;
    pImpl->active = true;
    
    // Initialize based on output format
    switch (output) {
        case VisualizationOutput::Console:
            std::cout << "Initializing console visualization..." << std::endl;
            break;
        case VisualizationOutput::File:
            std::cout << "Initializing file visualization..." << std::endl;
            break;
        case VisualizationOutput::Live:
            if (pImpl->glfwAvailable) {
                return initializeGLFW();
            } else if (pImpl->sdlAvailable) {
                return initializeSDL();
            } else {
                std::cout << "Warning: No live display backend available" << std::endl;
                pImpl->active = false;
                return false;
            }
            break;
        case VisualizationOutput::Web:
            if (pImpl->webAvailable) {
                return launchWebVisualization();
            } else {
                std::cout << "Warning: WebAssembly not available" << std::endl;
                pImpl->active = false;
                return false;
            }
            break;
        case VisualizationOutput::Both:
            std::cout << "Initializing dual output visualization (console + file)..." << std::endl;
            break;
    }
    
    // Start update thread for real-time data
    if (mode != VisualizationMode::None) {
        updateThreadRunning = true;
        updateThread = std::thread(&VisualizationInterface::updateThreadFunc, this);
    }
    
    return true;
}

bool VisualizationInterface::initializeGLFW(int width, int height, const std::string& title) {
    if (!pImpl->glfwAvailable) {
        std::cout << "Warning: GLFW not available for visualization" << std::endl;
        return false;
    }
    
    // TODO: Implement GLFW initialization
    // This is a placeholder for the actual GLFW implementation
    std::cout << "Initializing GLFW window: " << width << "x" << height << " - " << title << std::endl;
    std::cout << "GLFW visualization backend available but not fully implemented" << std::endl;
    
    pImpl->windowWidth = width;
    pImpl->windowHeight = height;
    pImpl->windowTitle = title;
    
    // For now, just enable console output for GLFW
    if (pImpl->output != VisualizationOutput::Live) {
        std::cout << "GLFW visualization running in console mode (placeholder)" << std::endl;
    }
    
    return true;
}

bool VisualizationInterface::initializeSDL(int width, int height, const std::string& title) {
    if (!pImpl->sdlAvailable) {
        std::cout << "Warning: SDL not available for visualization" << std::endl;
        return false;
    }
    
    // TODO: Implement SDL initialization
    std::cout << "Initializing SDL window: " << width << "x" << height << " - " << title << std::endl;
    std::cout << "SDL visualization backend available but not fully implemented" << std::endl;
    
    pImpl->windowWidth = width;
    pImpl->windowHeight = height;
    pImpl->windowTitle = title;
    
    return true;
}

void VisualizationInterface::update(double timeStep) {
    if (!pImpl->active) return;
    
    std::lock_guard<std::mutex> lock(pImpl->updateMutex);
    pImpl->updateQueue.push(timeStep);
    pImpl->updateCondition.notify_one();
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    switch (pImpl->output) {
        case VisualizationOutput::Console:
            renderConsole();
            break;
        case VisualizationOutput::File:
            renderToFile();
            break;
        case VisualizationOutput::Live:
            renderLive();
            break;
        case VisualizationOutput::Web:
            renderWeb();
            break;
        case VisualizationOutput::Both:
            renderConsole();
            renderToFile();
            break;
    }
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        pImpl->active = false;
        
        // Stop update thread
        if (updateThreadRunning) {
            updateCondition.notify_one();
            if (updateThread.joinable()) {
                updateThread.join();
            }
            updateThreadRunning = false;
        }
        
        // Cleanup backend
        cleanupBackend();
        
        std::cout << "Visualization closed" << std::endl;
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setVisualizationMode(VisualizationMode mode) {
    pImpl->mode = mode;
}

VisualizationMode VisualizationInterface::getVisualizationMode() const {
    return pImpl->mode;
}

void VisualizationInterface::setOutputFormat(VisualizationOutput output) {
    pImpl->output = output;
}

VisualizationOutput VisualizationInterface::getOutputFormat() const {
    return pImpl->output;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    prepareNetworkData(brain, pImpl->cachedConnections);
    if (pImpl->output == VisualizationOutput::Console || pImpl->output == VisualizationOutput::Both) {
        std::cout << "Network visualization prepared: " << pImpl->cachedConnections.size() << " connections" << std::endl;
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    prepareActivityData(brain, pImpl->cachedActivityLevels);
    if (pImpl->output == VisualizationOutput::Console || pImpl->output == VisualizationOutput::Both) {
        std::cout << "Activity visualization prepared: " << pImpl->cachedActivityLevels.size() << " activity samples" << std::endl;
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    prepareSpikeData(spikes, pImpl->cachedSpikeTimes);
    if (pImpl->output == VisualizationOutput::Console || pImpl->output == VisualizationOutput::Both) {
        std::cout << "Spike visualization prepared: " << pImpl->cachedSpikeTimes.size() << " spike events" << std::endl;
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    prepareWeightData(brain, pImpl->cachedWeights);
    if (pImpl->output == VisualizationOutput::Console || pImpl->output == VisualizationOutput::Both) {
        std::cout << "Weight visualization prepared: " << pImpl->cachedWeights.size() << "x" << (pImpl->cachedWeights.empty() ? 0 : pImpl->cachedWeights[0].size()) << " matrix" << std::endl;
    }
}

void VisualizationInterface::visualize3D(const Brain& brain) {
    // Enhanced 3D visualization preparation
    visualizeNetwork(brain);
    visualizeActivity(brain);
    visualizeWeights(brain);
    
    std::cout << "3D visualization prepared with network, activity, and weight data" << std::endl;
}

bool VisualizationInterface::exportVisualization(const std::string& filename, VisualizationOutput format) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << "NLM Visualization Export" << std::endl;
    file << "=========================" << std::endl;
    file << "Generated: " << getCurrentTimestamp() << std::endl;
    file << "Format: " << static_cast<int>(format) << std::endl;
    file << "Mode: " << static_cast<int>(pImpl->mode) << std::endl;
    file << std::endl;
    
    // Export cached data
    file << "Network Connections: " << pImpl->cachedConnections.size() << std::endl;
    for (const auto& conn : pImpl->cachedConnections) {
        file << conn.first << " -> " << conn.second << std::endl;
    }
    file << std::endl;
    
    file << "Activity Levels: " << pImpl->cachedActivityLevels.size() << " samples" << std::endl;
    for (double level : pImpl->cachedActivityLevels) {
        file << std::fixed << std::setprecision(3) << level << std::endl;
    }
    file << std::endl;
    
    file << "Spike Events: " << pImpl->cachedSpikeTimes.size() << std::endl;
    for (const auto& spike : pImpl->cachedSpikeTimes) {
        file << "Time: " << std::fixed << std::setprecision(3) << spike.first 
             << ", Neuron: " << spike.second << std::endl;
    }
    
    file << std::endl;
    file << "Weight Matrix: " << pImpl->cachedWeights.size() << "x" << 
        (pImpl->cachedWeights.empty() ? 0 : pImpl->cachedWeights[0].size()) << std::endl;
    
    file.close();
    std::cout << "Visualization exported to: " << filename << std::endl;
    return true;
}

bool VisualizationInterface::launchWebVisualization(int port) {
    if (!pImpl->webAvailable) {
        std::cout << "Warning: Web visualization not available" << std::endl;
        return false;
    }
    
    // TODO: Implement web visualization
    std::cout << "Web visualization available on port " << port << std::endl;
    std::cout << "Web visualization backend not fully implemented" << std::endl;
    
    return true;
}

std::string VisualizationInterface::getStatistics(const Brain& brain) const {
    std::ostringstream oss;
    oss << "Visualization Statistics:" << std::endl;
    oss << "  Active: " << (pImpl->active ? "Yes" : "No") << std::endl;
    oss << "  Update Rate: " << std::fixed << std::setprecision(1) << pImpl->updateRate << " Hz" << std::endl;
    oss << "  Mode: " << static_cast<int>(pImpl->mode) << std::endl;
    oss << "  Output: " << static_cast<int>(pImpl->output) << std::endl;
    oss << "  Available Backends: GLFW=" << (pImpl->glfwAvailable ? "Yes" : "No") 
        << " SDL=" << (pImpl->sdlAvailable ? "Yes" : "No") 
        << " Web=" << (pImpl->webAvailable ? "Yes" : "No") << std::endl;
    oss << "  Cached Data:" << std::endl;
    oss << "    Connections: " << pImpl->cachedConnections.size() << std::endl;
    oss << "    Activity Samples: " << pImpl->cachedActivityLevels.size() << std::endl;
    oss << "    Spike Events: " << pImpl->cachedSpikeTimes.size() << std::endl;
    oss << "    Weight Matrix: " << pImpl->cachedWeights.size() << "x" << 
        (pImpl->cachedWeights.empty() ? 0 : pImpl->cachedWeights[0].size()) << std::endl;
    
    return oss.str();
}

bool VisualizationInterface::isGLFWAvailable() {
    // TODO: Implement actual GLFW availability check
    // For now, return false to simulate missing GLFW
    return false;
}

bool VisualizationInterface::isSDLAvailable() {
    // TODO: Implement actual SDL availability check
    // For now, return false to simulate missing SDL
    return false;
}

bool VisualizationInterface::isWebAssemblyAvailable() {
    // TODO: Implement actual WebAssembly availability check
    // For now, return false to simulate missing WebAssembly
    return false;
}

std::vector<std::string> VisualizationInterface::getAvailableBackends() {
    std::vector<std::string> backends;
    if (isGLFWAvailable()) backends.push_back("GLFW");
    if (isSDLAvailable()) backends.push_back("SDL");
    if (isWebAssemblyAvailable()) backends.push_back("WebAssembly");
    return backends;
}

void VisualizationInterface::setupGLFWWindow(int width, int height, const std::string& title) {
    // GLFW window setup - placeholder
    pImpl->windowWidth = width;
    pImpl->windowHeight = height;
    pImpl->windowTitle = title;
}

void VisualizationInterface::setupSDLWindow(int width, int height, const std::string& title) {
    // SDL window setup - placeholder
    pImpl->windowWidth = width;
    pImpl->windowHeight = height;
    pImpl->windowTitle = title;
}

void VisualizationInterface::cleanupBackend() {
    // Backend cleanup - placeholder
}

void VisualizationInterface::prepareNetworkData(const Brain& brain, std::vector<std::pair<size_t, size_t>>& connections) const {
    connections.clear();
    
    // Get all regions and their neurons
    for (RegionId regionId = 0; regionId < brain.getRegionCount(); ++regionId) {
        if (auto* region = brain.getRegion(regionId)) {
            for (const auto& neuron : region->getNeurons()) {
                // This is a simplified version - in practice you'd need proper neuron IDs
                // For now, just create some sample connections
                for (size_t i = 0; i < 5; ++i) {
                    connections.emplace_back(static_cast<size_t>(neuron->getId()), i % 10);
                }
            }
        }
    }
}

void VisualizationInterface::prepareActivityData(const Brain& brain, std::vector<double>& activityLevels) const {
    activityLevels.clear();
    
    // Sample activity levels from different neurons
    for (NeuronId i = 0; i < brain.getTotalNeuronCount(); i += std::max(1, brain.getTotalNeuronCount() / 100)) {
        if (auto* neuron = brain.getNeuron(i)) {
            double voltage = neuron->getVoltage();
            double activity = (voltage + 60.0) / 120.0; // Normalize to [0, 1]
            activityLevels.push_back(activity);
        }
    }
}

void VisualizationInterface::prepareSpikeData(const Brain& brain, std::vector<std::pair<double, size_t>>& spikeTimes) const {
    spikeTimes.clear();
    
    // This would normally track actual spike events
    // For now, generate some sample data
    for (size_t i = 0; i < 50; ++i) {
        double time = static_cast<double>(i * 10) / 1000.0; // Time in seconds
        spikeTimes.emplace_back(time, i % 20); // Neuron ID
    }
}

void VisualizationInterface::prepareWeightData(const Brain& brain, std::vector<std::vector<double>>& weights) const {
    weights.clear();
    
    // Create a sample weight matrix
    int rows = 10;
    int cols = 10;
    weights.resize(rows, std::vector<double>(cols));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Generate sample weights with some pattern
            weights[i][j] = 0.1 + 0.4 * (static_cast<double>(rand()) / RAND_MAX);
        }
    }
}

void VisualizationInterface::updateThreadFunc() {
    while (updateThreadRunning) {
        std::unique_lock<std::mutex> lock(pImpl->updateMutex);
        
        if (pImpl->updateQueue.empty()) {
            if (!updateCondition.wait_for(lock, std::chrono::milliseconds(100), 
                                        [this] { return !updateThreadRunning || !pImpl->updateQueue.empty(); })) {
                continue;
            }
        }
        
        double timeStep = pImpl->updateQueue.front();
        pImpl->updateQueue.pop();
        lock.unlock();
        
        // Process update
        if (pImpl->active) {
            // Real-time visualization updates would happen here
        }
    }
}

void VisualizationInterface::renderConsole() {
    if (!pImpl->active) return;
    
    std::cout << "=== NLM Visualization Console ===" << std::endl;
    std::cout << "Mode: " << static_cast<int>(pImpl->mode) << std::endl;
    std::cout << "Data Cache:" << std::endl;
    std::cout << "  Connections: " << pImpl->cachedConnections.size() << std::endl;
    std::cout << "  Activity Samples: " << pImpl->cachedActivityLevels.size() << std::endl;
    std::cout << "  Spike Events: " << pImpl->cachedSpikeTimes.size() << std::endl;
    std::cout << "  Weight Matrix: " << pImpl->cachedWeights.size() << "x" << 
        (pImpl->cachedWeights.empty() ? 0 : pImpl->cachedWeights[0].size()) << std::endl;
    
    if (pImpl->mode == VisualizationMode::NetworkGraph) {
        std::cout << "Network Graph: Connections between neurons" << std::endl;
    } else if (pImpl->mode == VisualizationMode::Activity) {
        std::cout << "Activity Heatmap: Neural activity over time" << std::endl;
    } else if (pImpl->mode == VisualizationMode::Spikes) {
        std::cout << "Spike Raster: Spike timing visualization" << std::endl;
    } else if (pImpl->mode == VisualizationMode::Weights) {
        std::cout << "Weight Matrix: Synaptic connection strengths" << std::endl;
    } else if (pImpl->mode == VisualizationMode::3D) {
        std::cout << "3D Architecture: Complete neural structure" << std::endl;
    }
    
    std::cout << "=== End Visualization ===" << std::endl;
}

void VisualizationInterface::renderToFile() {
    if (!pImpl->active) return;
    
    // Export to a simple text format
    std::string filename = "nlm_visualization_" + getCurrentTimestamp() + ".txt";
    exportVisualization(filename, VisualizationOutput::File);
}

void VisualizationInterface::renderLive() {
    // Live rendering would happen here
    // For now, just show status
    std::cout << "Live visualization running (GLFW/SDL placeholder)" << std::endl;
}

void VisualizationInterface::renderWeb() {
    // Web rendering would happen here
    std::cout << "Web visualization available (not implemented)" << std::endl;
}

std::string VisualizationInterface::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

} // namespace nlm
