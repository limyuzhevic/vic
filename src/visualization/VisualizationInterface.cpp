#include "VisualizationInterface.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <thread>
#include <mutex>
#include <ctime>

namespace nlm {

// Visualization implementation
struct VisualizationInterface::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool initialized;
    std::chrono::steady_clock::time_point startTime;
    
    // Performance tracking
    double totalRenderTime;
    double totalUpdateTime;
    size_t frameCount;
    
    // Data history
    std::vector<VisualizationData> dataHistory;
    size_t maxHistorySize;
    
    // Options
    std::map<std::string, std::string> options;
    
    Impl() : initialized(false), startTime(std::chrono::steady_clock::now()),
             totalRenderTime(0.0), totalUpdateTime(0.0), frameCount(0),
             maxHistorySize(1000) {
        options["refresh_rate"] = "30"; // FPS
        options["neuron_details"] = "false";
        options["synapse_details"] = "false";
        options["memory_details"] = "false";
        options["neuromodulation_details"] = "false";
    }
};

VisualizationInterface::Impl::Impl() : initialized(false), startTime(std::chrono::steady_clock::now()),
                                        totalRenderTime(0.0), totalUpdateTime(0.0), frameCount(0),
                                        maxHistorySize(1000) {
    options["refresh_rate"] = "30";
    options["neuron_details"] = "false";
    options["synapse_details"] = "false";
    options["memory_details"] = "false";
    options["neuromodulation_details"] = "false";
}

VisualizationInterface::VisualizationInterface() {
    pImpl = std::make_unique<Impl>();
}

VisualizationInterface::~VisualizationInterface() = default;

// ConsoleVisualizer implementation
struct ConsoleVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool initialized;
    
    // Console state
    bool neuronDetails;
    bool synapseDetails;
    bool memoryDetails;
    bool neuromodulationDetails;
    int refreshRate;
    size_t maxHistorySize;
    
    // Console buffer
    std::vector<VisualizationData> consoleHistory;
    size_t currentLine;
    
    // Progress tracking
    double lastUpdateTime;
    size_t framesRendered;
    
    Impl() : initialized(false), neuronDetails(false), synapseDetails(false),
             memoryDetails(false), neuromodulationDetails(false),
             refreshRate(30), maxHistorySize(100), currentLine(0),
             lastUpdateTime(0.0), framesRendered(0) {}
};

ConsoleVisualizer::ConsoleVisualizer() {
    pImpl = std::make_unique<Impl>();
}

bool ConsoleVisualizer::initialize(std::shared_ptr<Brain> brain,
                                   std::shared_ptr<AgentBrain> agent,
                                   std::shared_ptr<SimpleWorld> world) {
    try {
        pImpl->brain = brain;
        pImpl->agent = agent;
        pImpl->world = world;
        pImpl->initialized = true;
        pImpl->consoleHistory.clear();
        pImpl->currentLine = 0;
        pImpl->lastUpdateTime = std::chrono::steady_clock::now().time_since_epoch().count() / 1e9;
        pImpl->framesRendered = 0;
        
        // Set default options
        pImpl->neuronDetails = pImpl->getOption("neuron_details") == "true";
        pImpl->synapseDetails = pImpl->getOption("synapse_details") == "true";
        pImpl->memoryDetails = pImpl->getOption("memory_details") == "true";
        pImpl->neuromodulationDetails = pImpl->getOption("neuromodulation_details") == "true";
        pImpl->refreshRate = std::stoi(pImpl->getOption("refresh_rate"));
        
        NLM_LOG_INFO("ConsoleVisualizer initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConsoleVisualizer initialization failed: " + std::string(e.what()));
        return false;
    }
}

void ConsoleVisualizer::update(const VisualizationData& data) {
    if (!pImpl->initialized) return;
    
    // Add to history
    pImpl->consoleHistory.push_back(data);
    if (pImpl->consoleHistory.size() > pImpl->maxHistorySize) {
        pImpl->consoleHistory.erase(pImpl->consoleHistory.begin());
    }
    
    // Calculate refresh rate
    auto currentTime = std::chrono::steady_clock::now().time_since_epoch().count() / 1e9;
    double deltaTime = currentTime - pImpl->lastUpdateTime;
    
    // Render if it's time
    if (deltaTime >= 1.0 / pImpl->refreshRate) {
        render();
        pImpl->lastUpdateTime = currentTime;
        pImpl->framesRendered++;
    }
}

void ConsoleVisualizer::render() {
    if (!pImpl->initialized || pImpl->consoleHistory.empty()) return;
    
    const VisualizationData& data = pImpl->consoleHistory.back();
    
    // Clear screen (platform-dependent)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    // Header
    std::cout << "========================================" << std::endl;
    std::cout << "           NLM Phase 6 - Visualization    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Step: " << data.step << " | Time: " << std::fixed << std::setprecision(3) 
              << data.timestamp << "s | FPS: " << pImpl->refreshRate << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Neural Activity
    std::cout << "NEURAL ACTIVITY:" << std::endl;
    std::cout << "  Total Neurons: " << pImpl->brain ? pImpl->brain->getTotalNeuronCount() : 0 << std::endl;
    std::cout << "  Active Neurons: " << data.firingNeuronCount << std::endl;
    std::cout << "  Total Spikes: " << data.totalSpikeCount << std::endl;
    std::cout << "  Firing Rate: " << std::fixed << std::setprecision(2) 
              << (data.simulationTime > 0 ? data.firingNeuronCount / data.simulationTime : 0.0) << " Hz" << std::endl;
    
    if (pImpl->neuronDetails) {
        std::cout << "  E/I Ratio: " << std::fixed << std::setprecision(3) << data.excitationInhibitionRatio << std::endl;
    }
    
    // Memory Systems
    std::cout << "MEMORY SYSTEMS:" << std::endl;
    std::cout << "  Working Memory Load: " << std::fixed << std::setprecision(3) << data.workingMemoryLoad << std::endl;
    std::cout << "  Episodic Memory Episodes: " << data.episodicMemoryCount << std::endl;
    
    if (pImpl->memoryDetails) {
        std::cout << "  Concept Activations: " << data.conceptActivations.size() << std::endl;
    }
    
    // Neuromodulation
    std::cout << "NEUROMODULATION:" << std::endl;
    std::cout << "  Dopamine Level: " << std::fixed << std::setprecision(3) << data.dopamineLevel << std::endl;
    std::cout << "  Curiosity Level: " << std::fixed << std::setprecision(3) << data.curiosityLevel << std::endl;
    std::cout << "  Novelty Level: " << std::fixed << std::setprecision(3) << data.noveltyLevel << std::endl;
    std::cout << "  Prediction Error: " << std::fixed << std::setprecision(3) << data.predictionError << std::endl;
    
    if (pImpl->neuromodulationDetails) {
        std::cout << "  Novelty: " << std::fixed << std::setprecision(3) << data.noveltyLevel << std::endl;
    }
    
    // Attention and Planning
    if (!data.attentionWeights.empty()) {
        std::cout << "ATTENTION:" << std::endl;
        std::cout << "  Focus Areas: " << data.attentionWeights.size() << std::endl;
        if (pImpl->neuronDetails) {
            for (size_t i = 0; i < std::min(size_t(5), data.attentionWeights.size()); ++i) {
                std::cout << "    Area " << i << ": " << std::fixed << std::setprecision(3) 
                          << data.attentionWeights[i] << std::endl;
            }
        }
    }
    
    // Agent State
    if (pImpl->agent) {
        std::cout << "AGENT STATE:" << std::endl;
        std::cout << "  Energy: " << std::fixed << std::setprecision(3) << data.agentEnergy << std::endl;
        std::cout << "  Health: " << std::fixed << std::setprecision(3) << data.agentHealth << std::endl;
        std::cout << "  Position: (" << data.agentPosition[0] << ", " << data.agentPosition[1] << ")" << std::endl;
        std::cout << "  Orientation: " << std::fixed << std::setprecision(3) << data.agentOrientation << " rad" << std::endl;
    }
    
    // Performance
    std::cout << "PERFORMANCE:" << std::endl;
    std::cout << "  Frames Rendered: " << pImpl->framesRendered << std::endl;
    std::cout << "  Console History Size: " << pImpl->consoleHistory.size() << std::endl;
    
    // Controls
    std::cout << "========================================" << std::endl;
    std::cout << "Controls: Not implemented yet" << std::endl;
    std::cout << "========================================" << std::endl;
    
    pImpl->currentLine = (pImpl->currentLine + 1) % 25;
}

bool ConsoleVisualizer::exportData(const std::string& filepath,
                                   const std::string& format) {
    if (!pImpl->initialized) return false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for export: " + filepath);
            return false;
        }
        
        if (format == "csv" || format == "auto") {
            // Export to CSV
            file << "Step,Timestamp,TotalSpikes,ActiveNeurons,FiringRate,WorkingMemoryLoad," << std::endl;
            for (const auto& data : pImpl->consoleHistory) {
                file << data.step << "," << data.timestamp << "," << data.totalSpikeCount << "," 
                     << data.firingNeuronCount << "," << (data.simulationTime > 0 ? data.firingNeuronCount / data.simulationTime : 0.0) << "," 
                     << data.workingMemoryLoad << "," << std::endl;
            }
        } else {
            NLM_LOG_ERROR("Unsupported export format: " + format);
            return false;
        }
        
        file.close();
        NLM_LOG_INFO("Console visualization exported to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ConsoleVisualizer export failed: " + std::string(e.what()));
        return false;
    }
}

void ConsoleVisualizer::setOption(const std::string& key, const std::string& value) {
    pImpl->options[key] = value;
    
    // Update internal state based on options
    if (key == "neuron_details") {
        pImpl->neuronDetails = (value == "true");
    } else if (key == "synapse_details") {
        pImpl->synapseDetails = (value == "true");
    } else if (key == "memory_details") {
        pImpl->memoryDetails = (value == "true");
    } else if (key == "neuromodulation_details") {
        pImpl->neuromodulationDetails = (value == "true");
    } else if (key == "refresh_rate") {
        pImpl->refreshRate = std::stoi(value);
    }
}

std::string ConsoleVisualizer::getOption(const std::string& key) const {
    auto it = pImpl->options.find(key);
    return (it != pImpl->options.end()) ? it->second : "";
}

bool ConsoleVisualizer::isReady() const {
    return pImpl->initialized && !pImpl->consoleHistory.empty();
}

void ConsoleVisualizer::shutdown() {
    if (pImpl->initialized) {
        pImpl->initialized = false;
        pImpl->consoleHistory.clear();
        NLM_LOG_INFO("ConsoleVisualizer shutdown complete");
    }
}

void ConsoleVisualizer::setRefreshRate(int fps) {
    pImpl->refreshRate = fps;
    pImpl->options["refresh_rate"] = std::to_string(fps);
}

void ConsoleVisualizer::setMaxHistory(size_t maxPoints) {
    pImpl->maxHistorySize = maxPoints;
    while (pImpl->consoleHistory.size() > pImpl->maxHistorySize) {
        pImpl->consoleHistory.erase(pImpl->consoleHistory.begin());
    }
}

void ConsoleVisualizer::toggleNeuronDetails(bool enable) {
    pImpl->neuronDetails = enable;
    pImpl->options["neuron_details"] = enable ? "true" : "false";
}

void ConsoleVisualizer::toggleSynapseDetails(bool enable) {
    pImpl->synapseDetails = enable;
    pImpl->options["synapse_details"] = enable ? "true" : "false";
}

void ConsoleVisualizer::toggleMemoryDetails(bool enable) {
    pImpl->memoryDetails = enable;
    pImpl->options["memory_details"] = enable ? "true" : "false";
}

void ConsoleVisualizer::toggleNeuromodulationDetails(bool enable) {
    pImpl->neuromodulationDetails = enable;
    pImpl->options["neuromodulation_details"] = enable ? "true" : "false";
}

// HeadlessVisualizer implementation
struct HeadlessVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool initialized;
    std::string outputDirectory;
    bool compressionEnabled;
    size_t exportFrequency;
    
    std::vector<VisualizationData> headlessHistory;
    size_t lastExportStep;
    
    Impl() : initialized(false), compressionEnabled(false), exportFrequency(100), lastExportStep(0) {}
};

HeadlessVisualizer::HeadlessVisualizer() {
    pImpl = std::make_unique<Impl>();
}

bool HeadlessVisualizer::initialize(std::shared_ptr<Brain> brain,
                                    std::shared_ptr<AgentBrain> agent,
                                    std::shared_ptr<SimpleWorld> world) {
    try {
        pImpl->brain = brain;
        pImpl->agent = agent;
        pImpl->world = world;
        pImpl->initialized = true;
        pImpl->headlessHistory.clear();
        pImpl->lastExportStep = 0;
        
        // Create output directory if it doesn't exist
        if (!pImpl->outputDirectory.empty()) {
            #ifdef _WIN32
                system(("mkdir " + pImpl->outputDirectory).c_str());
            #else
                system(("mkdir -p " + pImpl->outputDirectory).c_str());
            #endif
        }
        
        NLM_LOG_INFO("HeadlessVisualizer initialized successfully (output: " + pImpl->outputDirectory + ")");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("HeadlessVisualizer initialization failed: " + std::string(e.what()));
        return false;
    }
}

void HeadlessVisualizer::update(const VisualizationData& data) {
    if (!pImpl->initialized) return;
    
    pImpl->headlessHistory.push_back(data);
    
    // Export data periodically
    if (data.step > 0 && data.step % pImpl->exportFrequency == 0) {
        exportData("headless_data.csv", "csv");
    }
}

void HeadlessVisualizer::render() {
    // No rendering in headless mode
}

bool HeadlessVisualizer::exportData(const std::string& filepath,
                                    const std::string& format) {
    if (!pImpl->initialized) return false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for export: " + filepath);
            return false;
        }
        
        if (format == "csv") {
            // Export to CSV
            file << "Step,Timestamp,TotalSpikes,ActiveNeurons,FiringRate,WorkingMemoryLoad," << std::endl;
            for (const auto& data : pImpl->headlessHistory) {
                file << data.step << "," << data.timestamp << "," << data.totalSpikeCount << "," 
                     << data.firingNeuronCount << "," << (data.simulationTime > 0 ? data.firingNeuronCount / data.simulationTime : 0.0) << "," 
                     << data.workingMemoryLoad << "," << std::endl;
            }
        } else if (format == "json") {
            // Export to JSON (simplified)
            file << "{\"visualization_data\": [" << std::endl;
            for (size_t i = 0; i < pImpl->headlessHistory.size(); ++i) {
                const auto& data = pImpl->headlessHistory[i];
                file << "  {\"step\": " << data.step << ", \"timestamp\": " << data.timestamp 
                     << ", \"total_spikes\": " << data.totalSpikeCount << ", \"active_neurons\": " 
                     << data.firingNeuronCount << "}" << (i < pImpl->headlessHistory.size() - 1 ? "," : "") 
                     << std::endl;
            }
            file << "]}" << std::endl;
        } else {
            NLM_LOG_ERROR("Unsupported export format: " + format);
            return false;
        }
        
        file.close();
        NLM_LOG_INFO("Headless visualization exported to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("HeadlessVisualizer export failed: " + std::string(e.what()));
        return false;
    }
}

void HeadlessVisualizer::setOption(const std::string& key, const std::string& value) {
    if (key == "output_directory") {
        pImpl->outputDirectory = value;
    } else if (key == "compression") {
        pImpl->compressionEnabled = (value == "true");
    } else if (key == "export_frequency") {
        pImpl->exportFrequency = std::stoi(value);
    }
}

std::string HeadlessVisualizer::getOption(const std::string& key) const {
    if (key == "output_directory") return pImpl->outputDirectory;
    if (key == "compression") return pImpl->compressionEnabled ? "true" : "false";
    if (key == "export_frequency") return std::to_string(pImpl->exportFrequency);
    return "";
}

bool HeadlessVisualizer::isReady() const {
    return pImpl->initialized && !pImpl->headlessHistory.empty();
}

void HeadlessVisualizer::shutdown() {
    if (pImpl->initialized) {
        pImpl->initialized = false;
        pImpl->headlessHistory.clear();
        NLM_LOG_INFO("HeadlessVisualizer shutdown complete");
    }
}

void HeadlessVisualizer::setOutputDirectory(const std::string& directory) {
    pImpl->outputDirectory = directory;
}

void HeadlessVisualizer::enableDataCompression(bool enable) {
    pImpl->compressionEnabled = enable;
}

void HeadlessVisualizer::setExportFrequency(size_t steps) {
    pImpl->exportFrequency = steps;
}

// WebVisualizer implementation (simplified)
struct WebVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool initialized;
    bool serverRunning;
    int port;
    std::string webRoot;
    
    Impl() : initialized(false), serverRunning(false), port(8080) {}
};

WebVisualizer::WebVisualizer() {
    pImpl = std::make_unique<Impl>();
}

bool WebVisualizer::initialize(std::shared_ptr<Brain> brain,
                               std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world) {
    try {
        pImpl->brain = brain;
        pImpl->agent = agent;
        pImpl->world = world;
        pImpl->initialized = true;
        pImpl->serverRunning = false;
        
        NLM_LOG_INFO("WebVisualizer initialized successfully (port: " + std::to_string(pImpl->port) + ")");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("WebVisualizer initialization failed: " + std::string(e.what()));
        return false;
    }
}

void WebVisualizer::update(const VisualizationData& data) {
    // Web visualization updates could send data to connected clients
}

void WebVisualizer::render() {
    // Web rendering is handled by the web server
}

bool WebVisualizer::exportData(const std::string& filepath,
                               const std::string& format) {
    NLM_LOG_WARNING("WebVisualizer export not fully implemented - use headless or console instead");
    return false;
}

void WebVisualizer::setOption(const std::string& key, const std::string& value) {
    if (key == "port") {
        pImpl->port = std::stoi(value);
    } else if (key == "web_root") {
        pImpl->webRoot = value;
    }
}

std::string WebVisualizer::getOption(const std::string& key) const {
    if (key == "port") return std::to_string(pImpl->port);
    if (key == "web_root") return pImpl->webRoot;
    return "";
}

bool WebVisualizer::isReady() const {
    return pImpl->initialized;
}

void WebVisualizer::shutdown() {
    if (pImpl->initialized) {
        stopServer();
        pImpl->initialized = false;
        NLM_LOG_INFO("WebVisualizer shutdown complete");
    }
}

void WebVisualizer::startServer(int port) {
    pImpl->port = port;
    pImpl->serverRunning = true;
    NLM_LOG_INFO("Web server started on port " + std::to_string(pImpl->port));
}

void WebVisualizer::stopServer() {
    if (pImpl->serverRunning) {
        pImpl->serverRunning = false;
        NLM_LOG_INFO("Web server stopped");
    }
}

bool WebVisualizer::isServerRunning() const {
    return pImpl->serverRunning;
}

void WebVisualizer::setWebRoot(const std::string& root) {
    pImpl->webRoot = root;
}

// VisualizationManager implementation
struct VisualizationManager::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool initialized;
    
    std::map<VisualizationMode, std::shared_ptr<VisualizationInterface>> visualizers;
    
    double totalRenderTime;
    double totalUpdateTime;
    size_t totalFramesRendered;
    
    Impl() : initialized(false), totalRenderTime(0.0), totalUpdateTime(0.0), totalFramesRendered(0) {
        // Create default visualizers
        visualizers[VisualizationMode::Terminal] = std::make_shared<ConsoleVisualizer>();
        visualizers[VisualizationMode::Headless] = std::make_shared<HeadlessVisualizer>();
        visualizers[VisualizationMode::Web] = std::make_shared<WebVisualizer>();
    }
};

VisualizationManager::VisualizationManager() {
    pImpl = std::make_unique<Impl>();
}

VisualizationManager::~VisualizationManager() {
    shutdownAll();
}

std::shared_ptr<VisualizationInterface> VisualizationManager::createVisualizer(VisualizationMode mode) {
    auto it = pImpl->visualizers.find(mode);
    if (it != pImpl->visualizers.end()) {
        return it->second;
    }
    
    // Create new visualizer
    std::shared_ptr<VisualizationInterface> visualizer;
    switch (mode) {
        case VisualizationMode::Terminal:
            visualizer = std::make_shared<ConsoleVisualizer>();
            break;
        case VisualizationMode::Headless:
            visualizer = std::make_shared<HeadlessVisualizer>();
            break;
        case VisualizationMode::Web:
            visualizer = std::make_shared<WebVisualizer>();
            break;
        case VisualizationMode::Binary:
        case VisualizationMode::CSV:
        case VisualizationMode::JSON:
        case VisualizationMode::Performance:
            visualizer = std::make_shared<HeadlessVisualizer>(); // Use headless as default for non-display modes
            break;
    }
    
    pImpl->visualizers[mode] = visualizer;
    return visualizer;
}

bool VisualizationManager::initializeAll(std::shared_ptr<Brain> brain,
                                         std::shared_ptr<AgentBrain> agent,
                                         std::shared_ptr<SimpleWorld> world) {
    try {
        pImpl->brain = brain;
        pImpl->agent = agent;
        pImpl->world = world;
        pImpl->initialized = true;
        
        // Initialize all visualizers
        for (auto& pair : pImpl->visualizers) {
            if (!pair.second->initialize(brain, agent, world)) {
                NLM_LOG_ERROR("Failed to initialize visualization mode: " + std::to_string(static_cast<int>(pair.first)));
                return false;
            }
        }
        
        NLM_LOG_INFO("All visualization systems initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("VisualizationManager initialization failed: " + std::string(e.what()));
        return false;
    }
}

void VisualizationManager::updateAll(const VisualizationData& data) {
    if (!pImpl->initialized) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Update all visualizers
    for (auto& pair : pImpl->visualizers) {
        try {
            pair.second->update(data);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to update visualization mode " + std::to_string(static_cast<int>(pair.first)) + ": " + std::string(e.what()));
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalUpdateTime += duration;
}

void VisualizationManager::renderAll() {
    if (!pImpl->initialized) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Render all visualizers
    for (auto& pair : pImpl->visualizers) {
        try {
            pair.second->render();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to render visualization mode " + std::to_string(static_cast<int>(pair.first)) + ": " + std::string(e.what()));
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalRenderTime += duration;
    pImpl->totalFramesRendered++;
}

bool VisualizationManager::exportAll(const std::string& directory,
                                     const std::string& format) {
    try {
        // Create directory if it doesn't exist
        #ifdef _WIN32
            system(("mkdir " + directory).c_str());
        #else
            system(("mkdir -p " + directory).c_str());
        #endif
        
        // Export from all visualizers
        for (auto& pair : pImpl->visualizers) {
            std::string filepath = directory + "/visualization_" + std::to_string(static_cast<int>(pair.first)) + "." + format;
            if (!pair.second->exportData(filepath, format)) {
                NLM_LOG_ERROR("Failed to export visualization mode " + std::to_string(static_cast<int>(pair.first)));
                return false;
            }
        }
        
        NLM_LOG_INFO("All visualization data exported to: " + directory);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("VisualizationManager export failed: " + std::string(e.what()));
        return false;
    }
}

void VisualizationManager::setGlobalOption(const std::string& key, const std::string& value) {
    for (auto& pair : pImpl->visualizers) {
        pair.second->setOption(key, value);
    }
}

bool VisualizationManager::isInitialized() const {
    return pImpl->initialized;
}

size_t VisualizationManager::getActiveVisualizerCount() const {
    size_t count = 0;
    for (auto& pair : pImpl->visualizers) {
        if (pair.second->isReady()) {
            count++;
        }
    }
    return count;
}

void VisualizationManager::shutdownAll() {
    if (!pImpl->initialized) return;
    
    // Shutdown all visualizers
    for (auto& pair : pImpl->visualizers) {
        try {
            pair.second->shutdown();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error shutting down visualization mode " + std::to_string(static_cast<int>(pair.first)) + ": " + std::string(e.what()));
        }
    }
    
    pImpl->initialized = false;
    NLM_LOG_INFO("All visualization systems shutdown complete");
}

double VisualizationManager::getAverageRenderTime() const {
    return pImpl->totalFramesRendered > 0 ? pImpl->totalRenderTime / pImpl->totalFramesRendered : 0.0;
}

double VisualizationManager::getAverageUpdateTime() const {
    return pImpl->totalFramesRendered > 0 ? pImpl->totalUpdateTime / pImpl->totalFramesRendered : 0.0;
}

size_t VisualizationManager::getTotalFramesRendered() const {
    return pImpl->totalFramesRendered;
}

// DataExporter implementation
struct DataExporter::Impl {
    // Implementation details would go here
    Impl() {}
};

DataExporter::DataExporter() {
    pImpl = std::make_unique<Impl>();
}

DataExporter::~DataExporter() = default;

bool DataExporter::exportToCSV(const VisualizationData& data, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "step,timestamp,total_spikes,active_neurons,firing_rate,working_memory_load," << std::endl;
        file << data.step << "," << data.timestamp << "," << data.totalSpikeCount << "," 
             << data.firingNeuronCount << "," << (data.simulationTime > 0 ? data.firingNeuronCount / data.simulationTime : 0.0) << "," 
             << data.workingMemoryLoad << "," << std::endl;
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool DataExporter::exportToJSON(const VisualizationData& data, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple JSON export
        file << "{\"step\": " << data.step << ", \"timestamp\": " << data.timestamp 
             << ", \"total_spikes\": " << data.totalSpikeCount << ", \"active_neurons\": " 
             << data.firingNeuronCount << "}" << std::endl;
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool DataExporter::exportToBinary(const VisualizationData& data, const std::string& filepath) {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple binary export
        file.write(reinterpret_cast<const char*>(&data.step), sizeof(data.step));
        file.write(reinterpret_cast<const char*>(&data.timestamp), sizeof(data.timestamp));
        file.write(reinterpret_cast<const char*>(&data.totalSpikeCount), sizeof(data.totalSpikeCount));
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool DataExporter::exportToXML(const VisualizationData& data, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<?xml version=\"1.0\"?>" << std::endl;
        file << "<visualization_data>" << std::endl;
        file << "  <step>" << data.step << "</step>" << std::endl;
        file << "  <timestamp>" << data.timestamp << "</timestamp>" << std::endl;
        file << "  <total_spikes>" << data.totalSpikeCount << "</total_spikes>" << std::endl;
        file << "  <active_neurons>" << data.firingNeuronCount << "</active_neurons>" << std::endl;
        file << "</visualization_data>" << std::endl;
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool DataExporter::exportToHDF5(const VisualizationData& data, const std::string& filepath) {
    // HDF5 export is complex and would require HDF5 library
    // For now, return false
    return false;
}

bool DataExporter::exportBatch(const std::vector<VisualizationData>& data,
                               const std::string& directory,
                               const std::string& format) {
    try {
        #ifdef _WIN32
            system(("mkdir " + directory).c_str());
        #else
            system(("mkdir -p " + directory).c_str());
        #endif
        
        for (size_t i = 0; i < data.size(); ++i) {
            std::string filepath = directory + "/data_" + std::to_string(i) + "." + format;
            exportToCSV(data[i], filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<VisualizationData> DataExporter::normalizeData(const std::vector<VisualizationData>& data) {
    std::vector<VisualizationData> normalized = data;
    
    if (data.empty()) return normalized;
    
    // Simple normalization - scale values to [0, 1]
    double maxSpikes = 0.0;
    double minSpikes = std::numeric_limits<double>::max();
    
    for (const auto& d : data) {
        maxSpikes = std::max(maxSpikes, static_cast<double>(d.totalSpikeCount));
        minSpikes = std::min(minSpikes, static_cast<double>(d.totalSpikeCount));
    }
    
    for (auto& d : normalized) {
        if (maxSpikes > minSpikes) {
            d.totalSpikeCount = static_cast<size_t>((static_cast<double>(d.totalSpikeCount) - minSpikes) / (maxSpikes - minSpikes) * 1000);
        }
    }
    
    return normalized;
}

std::vector<VisualizationData> DataExporter::filterData(const std::vector<VisualizationData>& data,
                                                        const std::string& filterExpression) {
    // Simple filter implementation - just return all data for now
    return data;
}

std::vector<VisualizationData> DataExporter::resampleData(const std::vector<VisualizationData>& data,
                                                           double targetInterval) {
    // Simple resample implementation - just return all data for now
    return data;
}

bool DataExporter::saveMetadata(const std::string& filepath, const std::map<std::string, std::string>& metadata) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& pair : metadata) {
            file << pair.first << "=" << pair.second << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::map<std::string, std::string> DataExporter::loadMetadata(const std::string& filepath) {
    std::map<std::string, std::string> metadata;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return metadata;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                metadata[key] = value;
            }
        }
        
        file.close();
        
    } catch (const std::exception& e) {
    }
    
    return metadata;
}

double DataExporter::calculateCorrelation(const std::vector<VisualizationData>& data,
                                          const std::string& field1, const std::string& field2) {
    if (data.size() < 2) return 0.0;
    
    std::vector<double> values1, values2;
    
    for (const auto& d : data) {
        if (field1 == "total_spikes") values1.push_back(static_cast<double>(d.totalSpikeCount));
        else if (field1 == "active_neurons") values1.push_back(static_cast<double>(d.firingNeuronCount));
        else if (field1 == "firing_rate") values1.push_back(d.firingNeuronCount / std::max(d.simulationTime, 0.001));
        else if (field1 == "working_memory_load") values1.push_back(d.workingMemoryLoad);
        
        if (field2 == "total_spikes") values2.push_back(static_cast<double>(d.totalSpikeCount));
        else if (field2 == "active_neurons") values2.push_back(static_cast<double>(d.firingNeuronCount));
        else if (field2 == "firing_rate") values2.push_back(d.firingNeuronCount / std::max(d.simulationTime, 0.001));
        else if (field2 == "working_memory_load") values2.push_back(d.workingMemoryLoad);
    }
    
    // Simple correlation calculation
    double sum1 = 0.0, sum2 = 0.0, sum1xsum2 = 0.0, sum1sq = 0.0, sum2sq = 0.0;
    
    for (size_t i = 0; i < values1.size(); ++i) {
        sum1 += values1[i];
        sum2 += values2[i];
        sum1xsum2 += values1[i] * values2[i];
        sum1sq += values1[i] * values1[i];
        sum2sq += values2[i] * values2[i];
    }
    
    double n = static_cast<double>(values1.size());
    double numerator = n * sum1xsum2 - sum1 * sum2;
    double denominator = std::sqrt((n * sum1sq - sum1 * sum1) * (n * sum2sq - sum2 * sum2));
    
    return (denominator > 0.0) ? (numerator / denominator) : 0.0;
}

double DataExporter::calculateMean(const std::vector<VisualizationData>& data,
                                   const std::string& field) {
    if (data.empty()) return 0.0;
    
    double sum = 0.0;
    
    for (const auto& d : data) {
        if (field == "total_spikes") sum += static_cast<double>(d.totalSpikeCount);
        else if (field == "active_neurons") sum += static_cast<double>(d.firingNeuronCount);
        else if (field == "firing_rate") sum += d.firingNeuronCount / std::max(d.simulationTime, 0.001);
        else if (field == "working_memory_load") sum += d.workingMemoryLoad;
    }
    
    return sum / data.size();
}

double DataExporter::calculateStdDev(const std::vector<VisualizationData>& data,
                                     const std::string& field) {
    if (data.size() < 2) return 0.0;
    
    double mean = calculateMean(data, field);
    double sum = 0.0;
    
    for (const auto& d : data) {
        double value = 0.0;
        if (field == "total_spikes") value = static_cast<double>(d.totalSpikeCount);
        else if (field == "active_neurons") value = static_cast<double>(d.firingNeuronCount);
        else if (field == "firing_rate") value = d.firingNeuronCount / std::max(d.simulationTime, 0.001);
        else if (field == "working_memory_load") value = d.workingMemoryLoad;
        
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / (data.size() - 1));
}

} // namespace nlm
