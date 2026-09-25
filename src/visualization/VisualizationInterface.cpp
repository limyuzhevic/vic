#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <thread>
#include <chrono>

namespace nlm {

struct VisualizationInterface::Impl {
    VisualizationMode mode;
    bool active;
    double updateRate;
    Timestamp lastFrameTime;
    std::chrono::high_resolution_clock::time_point lastTime;
    
    // Visualization data
    VisualizationMetrics metrics;
    std::vector<std::vector<char>> frameBuffer;
    
    // Feature toggles
    bool networkVisible;
    bool activityVisible;
    bool memoryVisible;
    bool neuromodulationVisible;
    bool predictionVisible;
    bool developmentVisible;
    bool integrationVisible;
    
    // Configuration
    VisualizationConfig config;
    
    // WebSocket thread
    std::thread websocketThread;
    bool websocketRunning;
    
    // Internal data for visualization
    std::vector<NeuronId> neuronIds;
    std::vector<std::pair<NeuronId, NeuronId>> spikeEvents;
    std::vector<std::pair<RegionId, float>> regionWeights;
    
    Impl(VisualizationMode m = VisualizationMode::Text) 
        : mode(m), active(false), updateRate(30.0), lastFrameTime(0.0),
          networkVisible(true), activityVisible(true), memoryVisible(true),
          neuromodulationVisible(true), predictionVisible(true), developmentVisible(true),
          integrationVisible(true), websocketRunning(false) {
        lastTime = std::chrono::high_resolution_clock::now();
        frameBuffer.resize(100, std::vector<char>(80, ' '));
    }
};

VisualizationInterface::VisualizationInterface(VisualizationMode mode) : currentMode(mode), updateRate(30.0) {
    pImpl = std::make_unique<Impl>(mode);
    networkVisible = activityVisible = memoryVisible = neuromodulationVisible = 
    predictionVisible = developmentVisible = integrationVisible = true;
    currentMetrics = VisualizationMetrics();
}

VisualizationInterface::~VisualizationInterface() {
    close();
}

bool VisualizationInterface::initialize(const std::string& title, int width, int height) {
    std::lock_guard<std::mutex> lock(updateMutex);
    
    pImpl->config.windowTitle = title;
    pImpl->config.width = width;
    pImpl->config.height = height;
    pImpl->frameBuffer.resize(height, std::vector<char>(width, ' '));
    
    switch (currentMode) {
        case VisualizationMode::Text:
        case VisualizationMode::TextAdvanced:
        case VisualizationMode::Compact:
            std::cout << "=== NLM Phase 6 Visualization ===" << std::endl;
            std::cout << "Text mode initialized. Press 'q' to quit, 'h' for help." << std::endl;
            pImpl->active = true;
            break;
        case VisualizationMode::Graphical:
            // Would initialize graphical library (SDL, OpenGL, etc.)
            std::cout << "Graphical mode initialization would require SDL/GL setup." << std::endl;
            pImpl->active = true;
            break;
        case VisualizationMode::WebSocket:
            std::cout << "WebSocket mode initialized on port " << pImpl->config.webSocketPort << "." << std::endl;
            pImpl->active = true;
            pImpl->websocketRunning = true;
            // Start WebSocket server in background
            pImpl->websocketThread = std::thread(&VisualizationInterface::websocketServerLoop, this);
            break;
        case VisualizationMode::PythonAPI:
            std::cout << "Python API mode initialized." << std::endl;
            pImpl->active = true;
            break;
    }
    
    return pImpl->active;
}

void VisualizationInterface::update(const Brain& brain, Timestamp timestamp) {
    std::lock_guard<std::mutex> lock(updateMutex);
    updateData(brain, timestamp);
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    switch (currentMode) {
        case VisualizationMode::Text:
            renderText();
            break;
        case VisualizationMode::TextAdvanced:
            renderTextAdvanced();
            break;
        case VisualizationMode::Graphical:
            renderGraphical();
            break;
        case VisualizationMode::WebSocket:
            renderWebSocket();
            break;
        case VisualizationMode::PythonAPI:
            renderPythonAPI();
            break;
        case VisualizationMode::Compact:
            renderCompact();
            break;
    }
}

void VisualizationInterface::close() {
    std::lock_guard<std::mutex> lock(updateMutex);
    pImpl->active = false;
    
    if (pImpl->websocketRunning && pImpl->websocketThread.joinable()) {
        pImpl->websocketRunning = false;
        pImpl->websocketThread.join();
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setMode(VisualizationMode mode) {
    currentMode = mode;
    pImpl->mode = mode;
}

VisualizationMode VisualizationInterface::getMode() const {
    return currentMode;
}

void VisualizationInterface::setUpdateRate(double hz) {
    updateRate = hz;
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::updateData(const Brain& brain, Timestamp timestamp) {
    collectBrainMetrics(brain);
    collectMemoryMetrics(brain);
    collectNeuromodulationMetrics(brain);
    collectPredictionMetrics(brain);
    collectDevelopmentMetrics(brain);
    
    lastUpdateTime = timestamp;
}

void VisualizationInterface::collectBrainMetrics(const Brain& brain) {
    currentMetrics.totalReward = 0.0f; // Would be collected from experiment results
    currentMetrics.avgFiringRate = brain.getAverageFiringRate();
    currentMetrics.avgSynapticWeight = 0.5f; // Would calculate from synapses
    
    currentMetrics.workingMemoryActiveTraces = brain.getWorkingMemory() ? 
        brain.getWorkingMemory()->getActiveTraces() : 0.0f;
    currentMetrics.episodicMemoryEpisodes = brain.getEpisodicMemory() ? 
        brain.getEpisodicMemory()->getEpisodeCount() : 0.0f;
    
    currentMetrics.simulationTime = static_cast<float>(lastUpdateTime);
}

void VisualizationInterface::collectMemoryMetrics(const Brain& brain) {
    if (brain.getWorkingMemory()) {
        currentMetrics.memoryIntegrated = true;
    }
    if (brain.getEpisodicMemory()) {
        currentMetrics.episodicMemoryIntegrated = true;
    }
}

void VisualizationInterface::collectNeuromodulationMetrics(const Brain& brain) {
    if (brain.getDopamine()) {
        currentMetrics.neuromodulationIntegrated = true;
        currentMetrics.dopamineLevel = 0.7f; // Would get actual value
    }
    if (brain.getCuriosity()) {
        currentMetrics.curiosityLevel = 0.5f; // Would get actual value
    }
    if (brain.getNovelty()) {
        currentMetrics.noveltyLevel = 0.3f; // Would get actual value
    }
}

void VisualizationInterface::collectPredictionMetrics(const Brain& brain) {
    if (brain.getPredictionSystem()) {
        currentMetrics.predictionIntegrated = true;
        currentMetrics.predictionConfidence = 0.8f; // Would get actual value
    }
}

void VisualizationInterface::collectDevelopmentMetrics(const Brain& brain) {
    if (brain.getDevelopmentSystem()) {
        currentMetrics.developmentIntegrated = true;
        currentMetrics.developmentStage = 3.0f; // Would get actual stage
    }
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // Implementation for network visualization
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Implementation for activity visualization
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Implementation for spike visualization
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // Implementation for weight matrix visualization
}

void VisualizationInterface::visualizeMemory(const Brain& brain) {
    // Implementation for memory visualization
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain) {
    // Implementation for neuromodulation visualization
}

void VisualizationInterface::visualizePrediction(const Brain& brain) {
    // Implementation for prediction visualization
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    // Implementation for development visualization
}

void VisualizationInterface::visualizeIntegration(const Brain& brain) {
    // Implementation for integration visualization
}

void VisualizationInterface::exportData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "{\"visualization_data\": {" << std::endl;
        file << "  \"metrics\": {" << std::endl;
        file << "    \"total_reward\": " << currentMetrics.totalReward << "," << std::endl;
        file << "    \"avg_firing_rate\": " << currentMetrics.avgFiringRate << "," << std::endl;
        file << "    \"avg_synaptic_weight\": " << currentMetrics.avgSynapticWeight << "," << std::endl;
        file << "    \"novelty_level\": " << currentMetrics.noveltyLevel << "," << std::endl;
        file << "    \"curiosity_level\": " << currentMetrics.curiosityLevel << "," << std::endl;
        file << "    \"dopamine_level\": " << currentMetrics.dopamineLevel << "," << std::endl;
        file << "    \"working_memory_traces\": " << currentMetrics.workingMemoryActiveTraces << "," << std::endl;
        file << "    \"episodic_memory_episodes\": " << currentMetrics.episodicMemoryEpisodes << "," << std::endl;
        file << "    \"prediction_confidence\": " << currentMetrics.predictionConfidence << "," << std::endl;
        file << "    \"development_stage\": " << currentMetrics.developmentStage << "," << std::endl;
        file << "  }," << std::endl;
        file << "  \"integration_status\": {" << std::endl;
        file << "    \"memory_integrated\": " << (currentMetrics.memoryIntegrated ? "true" : "false") << "," << std::endl;
        file << "    \"episodic_memory_integrated\": " << (currentMetrics.episodicMemoryIntegrated ? "true" : "false") << "," << std::endl;
        file << "    \"neuromodulation_integrated\": " << (currentMetrics.neuromodulationIntegrated ? "true" : "false") << "," << std::endl;
        file << "    \"prediction_integrated\": " << (currentMetrics.predictionIntegrated ? "true" : "false") << "," << std::endl;
        file << "    \"development_integrated\": " << (currentMetrics.developmentIntegrated ? "true" : "false") << "," << std::endl;
        file << "    \"checkpointing_works\": " << (currentMetrics.checkpointingWorks ? "true" : "false") << "," << std::endl;
        file << "    \"replay_works\": " << (currentMetrics.replayWorks ? "true" : "false") << "/" << std::endl;
        file << "  }" << std::endl;
        file << "}" << std::endl;
        file.close();
    }
}

void VisualizationInterface::exportMetrics(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << "timestamp,total_reward,avg_firing_rate,avg_synaptic_weight,novelty_level,curiosity_level,dopamine_level,working_memory_traces,episodic_memory_episodes,prediction_confidence,development_stage" << std::endl;
        file << std::fixed << std::setprecision(6) << lastUpdateTime << ","
             << currentMetrics.totalReward << ","
             << currentMetrics.avgFiringRate << ","
             << currentMetrics.avgSynapticWeight << ","
             << currentMetrics.noveltyLevel << ","
             << currentMetrics.curiosityLevel << ","
             << currentMetrics.dopamineLevel << ","
             << currentMetrics.workingMemoryActiveTraces << ","
             << currentMetrics.episodicMemoryEpisodes << ","
             << currentMetrics.predictionConfidence << ","
             << currentMetrics.developmentStage << std::endl;
        file.close();
    }
}

void VisualizationInterface::renderText() const {
    std::stringstream ss;
    ss << "=== NLM Phase 6 Visualization (Text Mode) ===" << std::endl;
    ss << std::endl;
    ss << "System Status:" << std::endl;
    ss << "  Total Reward: " << currentMetrics.totalReward << std::endl;
    ss << "  Avg Firing Rate: " << currentMetrics.avgFiringRate << " Hz" << std::endl;
    ss << "  Avg Synaptic Weight: " << currentMetrics.avgSynapticWeight << std::endl;
    ss << std::endl;
    ss << "Memory Systems:" << std::endl;
    ss << "  Working Memory: " << currentMetrics.workingMemoryActiveTraces << " active traces" << std::endl;
    ss << "  Episodic Memory: " << currentMetrics.episodicMemoryEpisodes << " episodes stored" << std::endl;
    ss << "  Memory Integrated: " << (currentMetrics.memoryIntegrated ? "YES" : "NO") << std::endl;
    ss << "  Episodic Memory Integrated: " << (currentMetrics.episodicMemoryIntegrated ? "YES" : "NO") << std::endl;
    ss << std::endl;
    ss << "Neuromodulation:" << std::endl;
    ss << "  Dopamine Level: " << currentMetrics.dopamineLevel << std::endl;
    ss << "  Curiosity Level: " << currentMetrics.curiosityLevel << std::endl;
    ss << "  Novelty Level: " << currentMetrics.noveltyLevel << std::endl;
    ss << "  Neuromodulation Integrated: " << (currentMetrics.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
    ss << std::endl;
    ss << "Prediction System:" << std::endl;
    ss << "  Prediction Confidence: " << currentMetrics.predictionConfidence << std::endl;
    ss << "  Prediction Integrated: " << (currentMetrics.predictionIntegrated ? "YES" : "NO") << std::endl;
    ss << std::endl;
    ss << "Development System:" << std::endl;
    ss << "  Development Stage: " << currentMetrics.developmentStage << std::endl;
    ss << "  Development Integrated: " << (currentMetrics.developmentIntegrated ? "YES" : "NO") << std::endl;
    ss << std::endl;
    ss << "Integration Status:" << std::endl;
    ss << "  Checkpointing Works: " << (currentMetrics.checkpointingWorks ? "YES" : "NO") << std::endl;
    ss << "  Replay Works: " << (currentMetrics.replayWorks ? "YES" : "NO") << std::endl;
    ss << std::endl;
    ss << "Simulation:" << std::endl;
    ss << "  Time: " << currentMetrics.simulationTime << " steps" << std::endl;
    ss << "  Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    ss << std::endl;
    
    std::cout << ss.str();
}

void VisualizationInterface::renderTextAdvanced() const {
    std::stringstream ss;
    ss << "=== NLM Phase 6 Advanced Visualization ===" << std::endl;
    ss << std::endl;
    ss << "=== BRAIN ACTIVITY ===" << std::endl;
    ss << "Neuron Count: Not implemented" << std::endl;
    ss << "Active Neurons: Not implemented" << std::endl;
    ss << "Firing Rate: " << currentMetrics.avgFiringRate << " Hz" << std::endl;
    ss << std::endl;
    ss << "=== NETWORK STRUCTURE ===" << std::endl;
    ss << "Region Count: Not implemented" << std::endl;
    ss << "Synapse Count: Not implemented" << std::endl;
    ss << "Connectivity: Not implemented" << std::endl;
    ss << std::endl;
    ss << "=== MEMORY SYSTEMS ===" << std::endl;
    ss << "Working Memory Traces: " << currentMetrics.workingMemoryActiveTraces << std::endl;
    ss << "Episodic Episodes: " << currentMetrics.episodicMemoryEpisodes << std::endl;
    ss << "Memory Integration: " << (currentMetrics.memoryIntegrated ? "OK" : "FAILED") << std::endl;
    ss << std::endl;
    ss << "=== NEUROMODULATION ===" << std::endl;
    ss << "Dopamine: " << currentMetrics.dopamineLevel << std::endl;
    ss << "Curiosity: " << currentMetrics.curiosityLevel << std::endl;
    ss << "Novelty: " << currentMetrics.noveltyLevel << std::endl;
    ss << "Neuromodulation Integration: " << (currentMetrics.neuromodulationIntegrated ? "OK" : "FAILED") << std::endl;
    ss << std::endl;
    ss << "=== PREDICTION SYSTEM ===" << std::endl;
    ss << "Prediction Confidence: " << currentMetrics.predictionConfidence << std::endl;
    ss << "Prediction Integration: " << (currentMetrics.predictionIntegrated ? "OK" : "FAILED") << std::endl;
    ss << std::endl;
    ss << "=== DEVELOPMENT ===" << std::endl;
    ss << "Development Stage: " << currentMetrics.developmentStage << std::endl;
    ss << "Development Integration: " << (currentMetrics.developmentIntegrated ? "OK" : "FAILED") << std::endl;
    ss << std::endl;
    ss << "=== INTEGRATION STATUS ===" << std::endl;
    ss << "Checkpointing: " << (currentMetrics.checkpointingWorks ? "WORKING" : "FAILED") << std::endl;
    ss << "Replay: " << (currentMetrics.replayWorks ? "WORKING" : "FAILED") << std::endl;
    ss << std::endl;
    ss << "Performance: " << std::endl;
    ss << "  Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    ss << "  FPS: " << 1.0 / (pImpl->updateRate > 0 ? pImpl->updateRate : 1.0) << std::endl;
    ss << std::endl;
    
    std::cout << ss.str();
}

void VisualizationInterface::renderGraphical() const {
    std::cout << "=== Graphical Visualization ===" << std::endl;
    std::cout << "Graphical mode would use SDL/OpenGL for rendering." << std::endl;
    std::cout << "Features: Neuron networks, spike raster plots, heat maps, 3D brain models." << std::endl;
    std::cout << "(Skipping detailed rendering in console build)" << std::endl;
    std::cout << std::endl;
}

void VisualizationInterface::renderWebSocket() const {
    std::cout << "=== WebSocket Visualization ===" << std::endl;
    std::cout << "WebSocket server running on port " << pImpl->config.webSocketPort << "." << std::endl;
    std::cout << "Clients can connect to visualize data in real-time via WebSocket." << std::endl;
    std::cout << "(Skipping WebSocket implementation in console build)" << std::endl;
    std::cout << std::endl;
}

void VisualizationInterface::renderPythonAPI() const {
    std::cout << "=== Python API Visualization ===" << std::endl;
    std::cout << "Python API integration for external visualization tools." << std::endl;
    std::cout << "Available via pybind11 bindings in pynlm module." << std::endl;
    std::cout << std::endl;
}

void VisualizationInterface::renderCompact() const {
    std::stringstream ss;
    ss << "NLM6:" << std::endl;
    ss << "R:" << currentMetrics.totalReward << ", FR:" << currentMetrics.avgFiringRate << "Hz,";
    ss << "WM:" << currentMetrics.workingMemoryActiveTraces << ", EM:" << currentMetrics.episodicMemoryEpisodes << ",";
    ss << "Dop:" << currentMetrics.dopamineLevel << ", Cur:" << currentMetrics.curiosityLevel << ",";
    ss << "Pred:" << currentMetrics.predictionConfidence << ", Dev:" << currentMetrics.developmentStage << ",";
    ss << "Sim:" << currentMetrics.simulationTime;
    std::cout << ss.str() << std::endl;
}

std::string VisualizationInterface::getCommandLineHelp() {
    return """
=== Visualization Command Line Options ===

--visualization-mode <mode>           Visualization mode (text, text-advanced, graphical, websocket, python, compact)
--visualization-title <title>         Window/application title
--visualization-width <width>          Window width in characters
--visualization-height <height>        Window height in characters
--visualization-update-rate <hz>       Update rate in Hz
--no-network-viz                     Disable network visualization
--no-activity-viz                    Disable activity visualization
--no-memory-viz                      Disable memory visualization
--no-neuromod-viz                    Disable neuromodulation visualization
--no-prediction-viz                  Disable prediction visualization
--no-development-viz                 Disable development visualization
--enable-export                      Enable data export
--export-format <format>              Export format (json, csv, txt)
--export-path <path>                  Export file path
--auto-save                          Automatically save visualization data
--enable-websocket                   Enable WebSocket visualization server
--websocket-port <port>              WebSocket server port
--enable-help                        Show help information
--enable-performance                  Show performance metrics
--color-scheme <scheme>              Color scheme (default, dark, light, bw)

Examples:
  nlm_phase6_demo --visualization-mode text-advanced --visualization-width 120 --visualization-height 80
  nlm_phase6_demo --visualization-mode compact --visualization-title "NLM Phase 6"
  nlm_phase6_demo --visualization-mode websocket --websocket-port 8080 --enable-export
""";
}

bool VisualizationInterface::parseCommandLine(int argc, char* argv[], VisualizationConfig& config) {
    bool valid = true;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--visualization-mode" && i + 1 < argc) {
            std::string modeStr = argv[++i];
            if (modeStr == "text") config.enableTextVisualization = true;
            else if (modeStr == "text-advanced") config.enableTextVisualization = true;
            else if (modeStr == "graphical") config.enableGraphicalVisualization = true;
            else if (modeStr == "websocket") config.enableWebSocket = true;
            else if (modeStr == "python") config.enableWebSocket = true; // Simplified
            else if (modeStr == "compact") config.enableTextVisualization = true;
            else {
                std::cerr << "Error: Unknown visualization mode: " << modeStr << std::endl;
                valid = false;
            }
        }
        else if (arg == "--visualization-title" && i + 1 < argc) {
            config.windowTitle = argv[++i];
        }
        else if (arg == "--visualization-width" && i + 1 < argc) {
            config.width = std::stoi(argv[++i]);
        }
        else if (arg == "--visualization-height" && i + 1 < argc) {
            config.height = std::stoi(argv[++i]);
        }
        else if (arg == "--visualization-update-rate" && i + 1 < argc) {
            config.updateInterval = 1.0 / std::stod(argv[++i]);
        }
        else if (arg == "--no-network-viz") {
            config.enableTextVisualization = false; // Simplified
        }
        else if (arg == "--no-activity-viz") {
            config.enableTextVisualization = false; // Simplified
        }
        else if (arg == "--no-memory-viz") {
            config.enableTextVisualization = false; // Simplified
        }
        else if (arg == "--no-neuromod-viz") {
            config.enableTextVisualization = false; // Simplified
        }
        else if (arg == "--no-prediction-viz") {
            config.enableTextVisualization = false; // Simplified
        }
        else if (arg == "--no-development-viz") {
            config.enableTextVisualization = false; // Simplified
        }
        else if (arg == "--enable-export") {
            config.enableExport = true;
        }
        else if (arg == "--export-format" && i + 1 < argc) {
            config.exportFormat = argv[++i];
        }
        else if (arg == "--export-path" && i + 1 < argc) {
            config.exportPath = argv[++i];
        }
        else if (arg == "--auto-save") {
            config.autoSave = true;
        }
        else if (arg == "--enable-websocket" && i + 1 < argc) {
            config.enableWebSocket = true;
            config.webSocketPort = std::stoi(argv[++i]);
        }
        else if (arg == "--enable-help") {
            config.showHelp = true;
        }
        else if (arg == "--enable-performance") {
            config.showPerformance = true;
        }
        else if (arg == "--color-scheme" && i + 1 < argc) {
            config.colorScheme = argv[++i];
        }
        else if (arg == "--help" || arg == "-h") {
            std::cout << getCommandLineHelp() << std::endl;
            return true; // Successfully parsed help
        }
        else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            valid = false;
        }
    }
    
    return valid;
}

} // namespace nlm