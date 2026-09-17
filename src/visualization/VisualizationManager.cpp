// Visualization manager implementation
#include "VisualizationManager.hpp"
#include "console/ConsoleVisualization.hpp"
#include "gui/GUIVisualization.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace nlm {

struct VisualizationManager::Impl {
    // Configuration
    VisualizationBackend backend = VisualizationBackend::None;
    VisualizationMode mode = VisualizationMode::Disabled;
    std::string preferredGUI = "glfw";
    
    // Individual visualization control
    bool consoleEnabled = false;
    bool guiEnabled = false;
    
    // Visualization instances
    std::unique_ptr<ConsoleVisualization> consoleViz;
    std::unique_ptr<GUIVisualization> guiViz;
    
    // Active state
    bool active = false;
    bool initialized = false;
    
    // Performance tracking
    double updateRate = 60.0;
    uint64_t frameCount = 0;
    double lastFrameTime = 0.0;
    
    // Step recording for post-simulation analysis
    struct RecordedStep {
        SimulationStep step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
        double timestamp;
    };
    
    std::vector<RecordedStep> recordedSteps;
    
    // Learning data for plotting
    std::vector<double> recordedWeights;
    std::vector<double> recordedRewards;
    std::vector<double> recordedErrors;
    
    Impl() : active(false), initialized(false) {}
};

VisualizationManager::VisualizationManager() : pImpl(std::make_unique<Impl>()) {}

VisualizationManager::~VisualizationManager() {
    close();
}

bool VisualizationManager::initialize(const std::string& configFile) {
    NLM_LOG_INFO("Initializing Visualization Manager...");
    
    // Load configuration if provided
    if (!configFile.empty()) {
        NLM_LOG_INFO("Loading visualization configuration from: " + configFile);
        // parseConfigFile(configFile);
    }
    
    // Create visualization instances based on configuration
    if (pImpl->mode == VisualizationMode::ConsoleOnly || 
        pImpl->mode == VisualizationMode::BothEnabled) {
        pImpl->consoleEnabled = true;
        pImpl->consoleViz = std::make_unique<ConsoleVisualization>();
        if (!pImpl->consoleViz->initialize()) {
            NLM_LOG_ERROR("Failed to initialize Console Visualization");
            return false;
        }
        NLM_LOG_INFO("Console Visualization initialized");
    }
    
    if (pImpl->mode == VisualizationMode::GUIOnly || 
        pImpl->mode == VisualizationMode::BothEnabled) {
        pImpl->guiEnabled = true;
        pImpl->guiViz = std::make_unique<GUIVisualization>();
        
        // Configure GUI backend
        if (pImpl->preferredGUI == "sdl") {
            pImpl->guiViz->setBackend(GUIBackend::SDL);
        } else {
            pImpl->guiViz->setBackend(GUIBackend::GLFW);
        }
        
        if (!pImpl->guiViz->initialize()) {
            NLM_LOG_ERROR("Failed to initialize GUI Visualization");
            return false;
        }
        NLM_LOG_INFO("GUI Visualization initialized");
    }
    
    pImpl->active = true;
    pImpl->initialized = true;
    
    NLM_LOG_INFO("Visualization Manager initialized successfully");
    return true;
}

void VisualizationManager::update() {
    if (!pImpl->active) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastFrameTime).count();
    
    if (elapsed >= 1.0 / pImpl->updateRate) {
        // Update console visualization
        if (pImpl->consoleEnabled && pImpl->consoleViz) {
            pImpl->consoleViz->update();
        }
        
        // Update GUI visualization
        if (pImpl->guiEnabled && pImpl->guiViz) {
            pImpl->guiViz->update();
        }
        
        pImpl->frameCount++;
        pImpl->lastFrameTime = now;
    }
}

void VisualizationManager::render() {
    if (!pImpl->active) return;
    
    // Render console visualization (if in console mode or both)
    if (pImpl->consoleEnabled && pImpl->consoleViz) {
        pImpl->consoleViz->render();
    }
    
    // Render GUI visualization (if GUI is enabled)
    if (pImpl->guiEnabled && pImpl->guiViz) {
        pImpl->guiViz->render();
    }
}

void VisualizationManager::close() {
    NLM_LOG_INFO("Closing Visualization Manager...");
    
    if (pImpl->consoleViz) {
        pImpl->consoleViz->close();
        pImpl->consoleViz.reset();
    }
    
    if (pImpl->guiViz) {
        pImpl->guiViz->close();
        pImpl->guiViz.reset();
    }
    
    pImpl->active = false;
    pImpl->initialized = false;
    
    // Clear recorded data
    pImpl->recordedSteps.clear();
    pImpl->recordedWeights.clear();
    pImpl->recordedRewards.clear();
    pImpl->recordedErrors.clear();
    
    NLM_LOG_INFO("Visualization Manager closed");
}

bool VisualizationManager::isActive() const {
    return pImpl->active;
}

// Configuration methods
void VisualizationManager::setBackend(VisualizationBackend backend) {
    pImpl->backend = backend;
    
    // Map to mode
    switch (backend) {
        case VisualizationBackend::None:
            pImpl->mode = VisualizationMode::Disabled;
            break;
        case VisualizationBackend::Console:
            pImpl->mode = VisualizationMode::ConsoleOnly;
            break;
        case VisualizationBackend::GUI:
            pImpl->mode = VisualizationMode::GUIOnly;
            break;
        case VisualizationBackend::Both:
            pImpl->mode = VisualizationMode::BothEnabled;
            break;
    }
}

void VisualizationManager::setMode(VisualizationMode mode) {
    pImpl->mode = mode;
    
    // Map to backend
    switch (mode) {
        case VisualizationMode::Disabled:
            pImpl->backend = VisualizationBackend::None;
            break;
        case VisualizationMode::ConsoleOnly:
            pImpl->backend = VisualizationBackend::Console;
            break;
        case VisualizationMode::GUIOnly:
            pImpl->backend = VisualizationBackend::GUI;
            break;
        case VisualizationMode::BothEnabled:
            pImpl->backend = VisualizationBackend::Both;
            break;
    }
}

void VisualizationManager::setPreferredGUI(std::string backend) {
    pImpl->preferredGUI = backend;
}

void VisualizationManager::enableConsoleVisualization(bool enable) {
    pImpl->consoleEnabled = enable;
}

void VisualizationManager::enableGUIVisualization(bool enable) {
    pImpl->guiEnabled = enable;
}

// Console-specific configuration
void VisualizationManager::configureConsole(bool enableNetworkViz, bool enableActivityViz, 
                                          bool enableSpikesViz, bool enableWeightsViz,
                                          bool enableBrainStatesViz, bool enableAgentInteractionViz,
                                          bool enableResultsViz) {
    if (pImpl->consoleViz) {
        pImpl->consoleViz->setEnableNetworkViz(enableNetworkViz);
        pImpl->consoleViz->setEnableActivityViz(enableActivityViz);
        pImpl->consoleViz->setEnableSpikesViz(enableSpikesViz);
        pImpl->consoleViz->setEnableWeightsViz(enableWeightsViz);
        pImpl->consoleViz->setEnableBrainStatesViz(enableBrainStatesViz);
        pImpl->consoleViz->setEnableAgentInteractionViz(enableAgentInteractionViz);
        pImpl->consoleViz->setEnableResultsViz(enableResultsViz);
    }
}

void VisualizationManager::setConsoleMaxSpikes(size_t maxSpikes) {
    if (pImpl->consoleViz) {
        pImpl->consoleViz->setMaxSpikesDisplay(maxSpikes);
    }
}

void VisualizationManager::setConsoleMaxNetworkNodes(size_t maxNodes) {
    if (pImpl->consoleViz) {
        pImpl->consoleViz->setMaxNetworkNodes(maxNodes);
    }
}

void VisualizationManager::setConsoleColorMode(VisualizationColorMode mode) {
    if (pImpl->consoleViz) {
        pImpl->consoleViz->setColorMode(mode);
    }
}

// GUI-specific configuration
void VisualizationManager::configureGUI(std::string backend, int width, int height, int x, int y) {
    pImpl->preferredGUI = backend;
    if (pImpl->guiViz) {
        if (backend == "sdl") {
            pImpl->guiViz->setBackend(GUIBackend::SDL);
        } else {
            pImpl->guiViz->setBackend(GUIBackend::GLFW);
        }
        pImpl->guiViz->setWindowSize(width, height);
        pImpl->guiViz->setWindowPosition(x, y);
    }
}

void VisualizationManager::setGUITheme(const std::string& theme) {
    if (pImpl->guiViz) {
        pImpl->guiViz->setTheme(theme);
    }
}

void VisualizationManager::setGUILayout(GUILayout layout) {
    if (pImpl->guiViz) {
        pImpl->guiViz->setLayout(layout);
    }
}

// Add visualization panels to GUI
void VisualizationManager::addGUIPanel(const std::string& panelId, GUIElementType elementType, const std::string& label) {
    if (pImpl->guiViz) {
        pImpl->guiViz->addElement(elementType, panelId, label);
    }
}

void VisualizationManager::removeGUIPanel(const std::string& panelId) {
    if (pImpl->guiViz) {
        pImpl->guiViz->removeElement(panelId);
    }
}

void VisualizationManager::configureGUIPanel(const std::string& panelId, const std::string& type,
                                            const std::unordered_map<std::string, bool>& config) {
    if (pImpl->guiViz) {
        if (type == "network") {
            pImpl->guiViz->configureNetworkPanel(panelId, config.at("showNeurons"), 
                                               config.at("showConnections"), 
                                               config.at("showActivity"));
        } else if (type == "activity") {
            pImpl->guiViz->configureActivityPanel(panelId, config.at("showSpikes"),
                                                 config.at("showRates"), 
                                                 config.at("showHistograms"));
        } else if (type == "brain_states") {
            pImpl->guiViz->configureBrainStatesPanel(panelId, config.at("showMemory"),
                                                   config.at("showNeuromodulation"),
                                                   config.at("showPrediction"));
        } else if (type == "agent") {
            pImpl->guiViz->configureAgentPanel(panelId, config.at("showSensory"),
                                             config.at("showMotor"), 
                                             config.at("showExploration"));
        } else if (type == "results") {
            pImpl->guiViz->configureResultsPanel(panelId, config.at("showLearning"),
                                               config.at("showConsolidation"),
                                               config.at("showDevelopment"));
        }
    }
}

// Bind data to visualizations
void VisualizationManager::bindBrainData(const Brain& brain) {
    if (pImpl->consoleViz) {
        pImpl->consoleViz->visualizeNetwork(brain);
        pImpl->consoleViz->visualizeActivity(brain);
    }
    
    if (pImpl->guiViz) {
        pImpl->guiViz->bindBrainData(brain);
    }
}

void VisualizationManager::bindSpikeData(const std::vector<SpikeEvent>& spikes) {
    if (pImpl->consoleViz) {
        pImpl->consoleViz->visualizeSpikes(spikes);
    }
    
    if (pImpl->guiViz) {
        pImpl->guiViz->bindSpikeData(spikes);
    }
}

void VisualizationManager::bindLearningData(const std::vector<double>& weights,
                                             const std::vector<double>& rewards,
                                             const std::vector<double>& errors) {
    pImpl->recordedWeights = weights;
    pImpl->recordedRewards = rewards;
    pImpl->recordedErrors = errors;
    
    if (pImpl->consoleViz) {
        pImpl->consoleViz->plotLearningCurves(weights, rewards, errors);
    }
    
    if (pImpl->guiViz) {
        pImpl->guiViz->bindLearningData(weights, rewards, errors);
    }
}

void VisualizationManager::bindMetricsData(float totalReward, float avgFiringRate,
                                            float noveltyLevel, float curiosityLevel, 
                                            float dopamineLevel) {
    if (pImpl->consoleViz) {
        // Console visualization doesn't have direct metrics binding
        // but could update its stats display
    }
    
    if (pImpl->guiViz) {
        pImpl->guiViz->bindMetricsData(totalReward, avgFiringRate, noveltyLevel,
                                      curiosityLevel, dopamineLevel);
    }
}

// Interactive control
bool VisualizationManager::isElementHovered(const std::string& panelId) const {
    if (pImpl->guiViz) {
        return pImpl->guiViz->isElementHovered(panelId);
    }
    return false;
}

bool VisualizationManager::isElementClicked(const std::string& panelId) const {
    if (pImpl->guiViz) {
        return pImpl->guiViz->isElementClicked(panelId);
    }
    return false;
}

// Data recording and export
void VisualizationManager::recordStepData(SimulationStep step, size_t neuronCount,
                                          size_t synapseCount, size_t firingCount,
                                          float avgFiringRate) {
    Impl::RecordedStep recordedStep;
    recordedStep.step = step;
    recordedStep.neuronCount = neuronCount;
    recordedStep.synapseCount = synapseCount;
    recordedStep.firingCount = firingCount;
    recordedStep.avgFiringRate = avgFiringRate;
    recordedStep.timestamp = pImpl->frameCount / pImpl->updateRate;
    
    pImpl->recordedSteps.push_back(recordedStep);
    
    // Record to console visualization if active
    if (pImpl->consoleViz) {
        // Could update console with current step data
    }
}

bool VisualizationManager::saveVisualizationState(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving visualization state: " + filepath);
        return false;
    }
    
    file << "VisualizationState\n";
    file << "Version: 1.0\n";
    file << "Active: " << (pImpl->active ? "true" : "false") << "\n";
    file << "Mode: " << static_cast<int>(pImpl->mode) << "\n";
    file << "Backend: " << static_cast<int>(pImpl->backend) << "\n";
    file << "ConsoleEnabled: " << (pImpl->consoleEnabled ? "true" : "false") << "\n";
    file << "GUIEnabled: " << (pImpl->guiEnabled ? "true" : "false") << "\n";
    
    // Save step data
    file << "\nStepData:\n";
    for (const auto& step : pImpl->recordedSteps) {
        file << "  Step: " << step.step << "\n";
        file << "  Neurons: " << step.neuronCount << "\n";
        file << "  Synapses: " << step.synapseCount << "\n";
        file << "  FiringCount: " << step.firingCount << "\n";
        file << "  AvgFiringRate: " << step.avgFiringRate << "\n";
        file << "  Timestamp: " << step.timestamp << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Visualization state saved to: " + filepath);
    return true;
}

bool VisualizationManager::loadVisualizationState(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for loading visualization state: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Active: ") == 0) {
            pImpl->active = (line.substr(7) == "true");
        } else if (line.find("Mode: ") == 0) {
            pImpl->mode = static_cast<VisualizationMode>(std::stoi(line.substr(6)));
        } else if (line.find("Backend: ") == 0) {
            pImpl->backend = static_cast<VisualizationBackend>(std::stoi(line.substr(9)));
        } else if (line.find("ConsoleEnabled: ") == 0) {
            pImpl->consoleEnabled = (line.substr(16) == "true");
        } else if (line.find("GUIEnabled: ") == 0) {
            pImpl->guiEnabled = (line.substr(11) == "true");
        }
    }
    
    file.close();
    NLM_LOG_INFO("Visualization state loaded from: " + filepath);
    return true;
}

// Helper methods
void VisualizationManager::clearScreen() const {
    if (pImpl->consoleViz) {
        pImpl->consoleViz->clearScreen();
    }
}

void VisualizationManager::printStatus() const {
    std::cout << "=== VISUALIZATION STATUS ===" << std::endl;
    std::cout << "Active: " << (pImpl->active ? "YES" : "NO") << std::endl;
    std::cout << "Mode: " << static_cast<int>(pImpl->mode) << std::endl;
    std::cout << "Backend: " << static_cast<int>(pImpl->backend) << std::endl;
    std::cout << "Console Enabled: " << (pImpl->consoleEnabled ? "YES" : "NO") << std::endl;
    std::cout << "GUI Enabled: " << (pImpl->guiEnabled ? "YES" : "NO") << std::endl;
    std::cout << "Frame Count: " << pImpl->frameCount << std::endl;
    std::cout << "Recorded Steps: " << pImpl->recordedSteps.size() << std::endl;
    std::cout << "============================" << std::endl;
}

// Static factory methods
std::unique_ptr<VisualizationInterface> VisualizationManager::createVisualization(
    VisualizationBackend backend, VisualizationMode mode) {
    
    if (mode == VisualizationMode::Disabled || backend == VisualizationBackend::None) {
        return nullptr;
    }
    
    if (backend == VisualizationBackend::Console || mode == VisualizationMode::ConsoleOnly) {
        return std::make_unique<ConsoleVisualization>();
    } else if (backend == VisualizationBackend::GUI || mode == VisualizationMode::GUIOnly) {
        return std::make_unique<GUIVisualization>();
    } else if (backend == VisualizationBackend::Both || mode == VisualizationMode::BothEnabled) {
        // Return both? For now return GUI which has more features
        return std::make_unique<GUIVisualization>();
    }
    
    return nullptr;
}

bool VisualizationManager::isBackendAvailable(VisualizationBackend backend) {
    // Check if backends are available
    // This is a placeholder - actual implementation would check system resources
    return true;  // Assume all backends are available for now
}

std::vector<std::string> VisualizationManager::getAvailableBackends() {
    return {"None", "Console", "GUI", "Both"};
}

} // namespace nlm