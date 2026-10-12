#include "NLMLiveVisualization.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>

namespace nlm {

struct NLMLiveVisualization::Impl {
    // Visualization state
    bool active = false;
    bool paused = false;
    int width = 1920;
    int height = 1080;
    double updateRate = 60.0;
    double lastFrameTime = 0.0;
    double timeScale = 1.0;
    
    // Camera state
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    float cameraZ = 100.0f;
    float zoomLevel = 1.0f;
    float rotationYaw = 0.0f;
    float rotationPitch = 0.0f;
    float rotationRoll = 0.0f;
    
    // Rendering configuration
    bool showConnections = true;
    bool showActivity = true;
    bool showHeatmap = true;
    bool showWorkingMemory = true;
    bool showEpisodicMemory = true;
    bool showAllNeuromodulators = true;
    bool showDevelopmentStages = true;
    bool showSystemFlow = true;
    
    // Data storage
    std::vector<NeuralNode> neuralNodes;
    std::vector<NeuralConnection> neuralConnections;
    ActivityHeatmap activityHeatmap;
    MemoryState memoryState;
    NeuromodulationState neuromodulationState;
    DevelopmentState developmentState;
    SystemFlowState systemFlowState;
    
    // Research data capture
    std::vector<std::chrono::system_clock::time_point> captureTimestamps;
    std::vector<std::vector<NeuralNode>> capturedFrames;
    size_t maxCapturedFrames = 1000;
    
    // Threading
    std::mutex updateMutex;
    std::atomic<bool> updateThreadRunning{false};
    std::thread updateThread;
    
    // Headless mode
    bool headless = false;
    
    Impl() {}
    ~Impl() {
        if (updateThreadRunning) {
            updateThreadRunning = false;
            if (updateThread.joinable()) {
                updateThread.join();
            }
        }
    }
};

NLMLiveVisualization::NLMLiveVisualization() : pImpl(std::make_unique<Impl>()) {
}

NLMLiveVisualization::~NLMLiveVisualization() = default;

bool NLMLiveVisualization::initialize(int width, int height, bool headless) {
    pImpl->width = width;
    pImpl->height = height;
    pImpl->headless = headless;
    
    NLM_LOG_INFO("NLMLiveVisualization: Initialized with " + std::to_string(width) + "x" + std::to_string(height) + " resolution");
    if (headless) {
        NLM_LOG_INFO("NLMLiveVisualization: Running in headless mode");
    }
    
    pImpl->active = true;
    
    // Start update thread
    pImpl->updateThreadRunning = true;
    pImpl->updateThread = std::thread([this]() {
        while (pImpl->updateThreadRunning) {
            if (!pImpl->paused) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0 / pImpl->updateRate)));
            }
        }
    });
    
    return true;
}

void NLMLiveVisualization::update(const class Brain& brain, const class AgentBrain* agent) {
    std::lock_guard<std::mutex> lock(pImpl->updateMutex);
    
    auto currentTime = std::chrono::system_clock::now();
    auto timeSinceLastFrame = std::chrono::duration<double>(currentTime - pImpl->lastFrameTime).count();
    pImpl->lastFrameTime = currentTime;
    
    // Clear previous frame data
    pImpl->neuralNodes.clear();
    pImpl->neuralConnections.clear();
    
    // Extract neural network data
    for (const auto& region : brain.getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto* neuron : population->getNeurons()) {
                NeuralNode node;
                node.id = neuron->getId();
                
                // Position neurons based on region and population
                node.x = region->getPosition().x + population->getId() * 10.0f;
                node.y = region->getPosition().y;
                node.z = region->getPosition().z + population->getId() * 5.0f;
                
                // Calculate neuron radius based on activity
                node.radius = 2.0f + neuron->getMembranePotential() * 0.5f;
                
                // Get activation level from membrane potential
                node.activation = (neuron->getMembranePotential() + 65.0f) / 130.0f;
                node.previousActivation = node.activation; // Simplified
                
                // Color based on neuron type and activity
                if (neuron->getNeuronType() == NeuronType::Excitatory) {
                    node.color[0] = 0.2f + node.activation * 0.8f; // Red channel
                    node.color[1] = 0.2f + node.activation * 0.2f; // Green channel
                    node.color[2] = 0.8f; // Blue channel
                } else {
                    node.color[0] = 0.8f; // Red channel
                    node.color[1] = 0.2f + node.activation * 0.3f; // Green channel
                    node.color[2] = 0.2f + node.activation * 0.8f; // Blue channel
                }
                
                // Spike detection
                if (neuron->hasSpiked()) {
                    node.isSpiking = true;
                    node.lastSpikeTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                }
                
                pImpl->neuralNodes.push_back(node);
            }
        }
        
        // Draw connections between neurons
        if (pImpl->showConnections) {
            for (const auto& synapse : region->getSynapses()) {
                NeuralConnection conn;
                conn.fromId = synapse->getPreSynapticNeuronId();
                conn.toId = synapse->getPostSynapticNeuronId();
                conn.weight = synapse->getWeight();
                conn.normalizedWeight = (conn.weight + 1.0f) / 2.0f;
                conn.isExcitatory = synapse->isExcitatory();
                conn.transparency = 0.5f + std::abs(conn.weight) * 0.5f;
                
                pImpl->neuralConnections.push_back(conn);
            }
        }
    }
    
    // Update activity heatmap
    if (pImpl->showHeatmap) {
        pImpl->activityHeatmap.gridWidth = 100;
        pImpl->activityHeatmap.gridHeight = 100;
        pImpl->activityHeatmap.grid.assign(pImpl->activityHeatmap.gridWidth * pImpl->activityHeatmap.gridHeight, 0.0f);
        pImpl->activityHeatmap.minValue = 0.0f;
        pImpl->activityHeatmap.maxValue = 1.0f;
        pImpl->activityHeatmap.lastUpdate = currentTime;
    }
    
    // Update memory state
    if (agent) {
        pImpl->memoryState.workingMemoryActiveTraces = agent->getBrain()->getWorkingMemory() ? 
            agent->getBrain()->getWorkingMemory()->getActiveTraces() : 0;
        
        pImpl->memoryState.episodicMemoryCount = agent->getBrain()->getEpisodicMemory() ? 
            static_cast<uint32_t>(agent->getBrain()->getEpisodicMemory()->getEpisodeCount()) : 0;
        
        pImpl->memoryState.lastUpdate = currentTime;
    }
    
    // Update neuromodulation state
    if (agent) {
        pImpl->neuromodulationState.dopamine = agent->getNeuromodulationLevel();
        pImpl->neuromodulationState.curiosity = agent->getCuriosityLevel();
        pImpl->neuromodulationState.novelty = agent->getNoveltyLevel();
        pImpl->neuromodulationState.predictionError = agent->getPredictionError();
        pImpl->neuromodulationState.attention = 0.5f; // Simplified
        pImpl->neuromodulationState.learningRate = 0.01f; // Simplified
        pImpl->neuromodulationState.lastUpdate = currentTime;
    }
    
    // Update development state
    if (agent) {
        pImpl->developmentState.developmentalStage = static_cast<float>(agent->getDevelopmentalStage());
        pImpl->developmentState.plasticityRate = 0.1f + pImpl->developmentState.developmentalStage * 0.05f; // Simplified
        pImpl->developmentState.synapticMaturation = pImpl->developmentState.developmentalStage;
        pImpl->developmentState.myelinationProgress = pImpl->developmentState.developmentalStage * 0.8f;
        pImpl->developmentState.lastUpdate = currentTime;
    }
    
    // Update system flow state
    pImpl->systemFlowState.brainStepStatus = "OK";
    pImpl->systemFlowState.memorySystemStatus = agent && agent->getBrain()->getWorkingMemory() ? "ACTIVE" : "INACTIVE";
    pImpl->systemFlowState.neuromodulationStatus = agent ? "ACTIVE" : "INACTIVE";
    pImpl->systemFlowState.predictionSystemStatus = agent && agent->getBrain()->getPredictionSystem() ? "ACTIVE" : "INACTIVE";
    pImpl->systemFlowState.planningSystemStatus = "ACTIVE"; // Simplified
    pImpl->systemFlowState.lastUpdate = currentTime;
}

void NLMLiveVisualization::render() {
    if (!pImpl->active || pImpl->paused && pImpl->headless) {
        return;
    }
    
    if (!pImpl->headless) {
        // In a real implementation, this would render using OpenGL/Vulkan
        // For this implementation, we'll log the visualization data
        NLM_LOG_INFO("NLMLiveVisualization: Rendering " + std::to_string(pImpl->neuralNodes.size()) + " neurons and " + 
                    std::to_string(pImpl->neuralConnections.size()) + " connections");
    } else {
        // Headless mode - just process data
        NLM_LOG_INFO("NLMLiveVisualization: Headless rendering - processing " + std::to_string(pImpl->neuralNodes.size()) + " neural nodes");
    }
}

void NLMLiveVisualization::close() {
    pImpl->active = false;
    pImpl->paused = false;
    
    if (pImpl->updateThreadRunning) {
        pImpl->updateThreadRunning = false;
        if (pImpl->updateThread.joinable()) {
            pImpl->updateThread.join();
        }
    }
    
    NLM_LOG_INFO("NLMLiveVisualization: Closed");
}

bool NLMLiveVisualization::isActive() const {
    return pImpl->active;
}

void NLMLiveVisualization::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double NLMLiveVisualization::getUpdateRate() const {
    return pImpl->updateRate;
}

bool NLMLiveVisualization::exportToPNG(const std::string& filepath) {
    NLM_LOG_INFO("NLMLiveVisualization: Exporting to PNG: " + filepath);
    // In a real implementation, this would use a graphics library
    return true;
}

bool NLMLiveVisualization::exportToSVG(const std::string& filepath) {
    NLM_LOG_INFO("NLMLiveVisualization: Exporting to SVG: " + filepath);
    // In a real implementation, this would use an SVG library
    return true;
}

bool NLMLiveVisualization::exportToJSON(const std::string& filepath) {
    try {
        nlohmann::json j;
        
        j["neuralNodes"] = std::vector<NeuralNode>(pImpl->neuralNodes);
        j["neuralConnections"] = std::vector<NeuralConnection>(pImpl->neuralConnections);
        j["activityHeatmap"] = {
            {"gridWidth", pImpl->activityHeatmap.gridWidth},
            {"gridHeight", pImpl->activityHeatmap.gridHeight},
            {"minValue", pImpl->activityHeatmap.minValue},
            {"maxValue", pImpl->activityHeatmap.maxValue}
        };
        
        j["memoryState"] = {
            {"workingMemoryActiveTraces", pImpl->memoryState.workingMemoryActiveTraces},
            {"episodicMemoryCount", pImpl->memoryState.episodicMemoryCount},
            {"memoryConsolidationProgress", pImpl->memoryState.memoryConsolidationProgress}
        };
        
        j["neuromodulationState"] = {
            {"dopamine", pImpl->neuromodulationState.dopamine},
            {"curiosity", pImpl->neuromodulationState.curiosity},
            {"novelty", pImpl->neuromodulationState.novelty},
            {"predictionError", pImpl->neuromodulationState.predictionError},
            {"attention", pImpl->neuromodulationState.attention},
            {"learningRate", pImpl->neuromodulationState.learningRate}
        };
        
        j["developmentState"] = {
            {"developmentalStage", pImpl->developmentState.developmentalStage},
            {"plasticityRate", pImpl->developmentState.plasticityRate},
            {"synapticMaturation", pImpl->developmentState.synapticMaturation},
            {"myelinationProgress", pImpl->developmentState.myelinationProgress}
        };
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            NLM_LOG_INFO("NLMLiveVisualization: JSON export successful: " + filepath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NLMLiveVisualization: JSON export failed: " + std::string(e.what()));
    }
    
    return false;
}

bool NLMLiveVisualization::exportToMP4(const std::string& filepath, double fps) {
    NLM_LOG_INFO("NLMLiveVisualization: Exporting to MP4: " + filepath + " at " + std::to_string(fps) + " FPS");
    // In a real implementation, this would use a video encoding library
    return true;
}

void NLMLiveVisualization::setNeuronRenderingMode(bool showConnections, bool showActivity, bool showHeatmap) {
    pImpl->showConnections = showConnections;
    pImpl->showActivity = showActivity;
    pImpl->showHeatmap = showHeatmap;
}

void NLMLiveVisualization::setMemoryVisualizationMode(bool showWorkingMemory, bool showEpisodicMemory) {
    pImpl->showWorkingMemory = showWorkingMemory;
    pImpl->showEpisodicMemory = showEpisodicMemory;
}

void NLMLiveVisualization::setNeuromodulationVisualizationMode(bool showAllNeuromodulators) {
    pImpl->showAllNeuromodulators = showAllNeuromodulators;
}

void NLMLiveVisualization::setDevelopmentVisualizationMode(bool showDevelopmentStages) {
    pImpl->showDevelopmentStages = showDevelopmentStages;
}

void NLMLiveVisualization::setIntegrationFlowVisualizationMode(bool showSystemFlow) {
    pImpl->showSystemFlow = showSystemFlow;
}

void NLMLiveVisualization::setCameraPosition(float x, float y, float z) {
    pImpl->cameraX = x;
    pImpl->cameraY = y;
    pImpl->cameraZ = z;
}

void NLMLiveVisualization::setZoomLevel(float zoom) {
    pImpl->zoomLevel = zoom;
}

void NLMLiveVisualization::setRotation(float yaw, float pitch, float roll) {
    pImpl->rotationYaw = yaw;
    pImpl->rotationPitch = pitch;
    pImpl->rotationRoll = roll;
}

void NLMLiveVisualization::setTimeScale(double timeScale) {
    pImpl->timeScale = timeScale;
}

void NLMLiveVisualization::togglePause(bool pause) {
    pImpl->paused = pause;
}

bool NLMLiveVisualization::isPaused() const {
    return pImpl->paused;
}

const std::vector<NLMLiveVisualization::NeuralNode>& NLMLiveVisualization::getNeuralNodes() const {
    return pImpl->neuralNodes;
}

const std::vector<NLMLiveVisualization::NeuralConnection>& NLMLiveVisualization::getNeuralConnections() const {
    return pImpl->neuralConnections;
}

const NLMLiveVisualization::ActivityHeatmap& NLMLiveVisualization::getActivityHeatmap() const {
    return pImpl->activityHeatmap;
}

const NLMLiveVisualization::MemoryState& NLMLiveVisualization::getMemoryState() const {
    return pImpl->memoryState;
}

const NLMLiveVisualization::NeuromodulationState& NLMLiveVisualization::getNeuromodulationState() const {
    return pImpl->neuromodulationState;
}

const NLMLiveVisualization::DevelopmentState& NLMLiveVisualization::getDevelopmentState() const {
    return pImpl->developmentState;
}

const NLMLiveVisualization::SystemFlowState& NLMLiveVisualization::getSystemFlowState() const {
    return pImpl->systemFlowState;
}

void NLMLiveVisualization::captureFrame() {
    std::lock_guard<std::mutex> lock(pImpl->updateMutex);
    
    pImpl->captureTimestamps.push_back(std::chrono::system_clock::now());
    pImpl->capturedFrames.push_back(pImpl->neuralNodes);
    
    if (pImpl->capturedFrames.size() > pImpl->maxCapturedFrames) {
        pImpl->capturedFrames.erase(pImpl->capturedFrames.begin());
        pImpl->captureTimestamps.erase(pImpl->captureTimestamps.begin());
    }
    
    NLM_LOG_INFO("NLMLiveVisualization: Frame captured, total frames: " + std::to_string(pImpl->capturedFrames.size()));
}

const std::vector<std::chrono::system_clock::time_point>& NLMLiveVisualization::getCaptureTimestamps() const {
    return pImpl->captureTimestamps;
}

const std::vector<std::vector<NLMLiveVisualization::NeuralNode>>& NLMLiveVisualization::getCapturedFrames() const {
    return pImpl->capturedFrames;
}

} // namespace nlm
