#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include "VisualizationInterface.hpp"

namespace nlm {

class TextVisualizationMode;
class GraphicalVisualizationMode;
class WebVisualizationMode;

class VisualizationInterface::Impl {
public:
    bool active;
    double updateRate;
    std::unique_ptr<TextVisualizationMode> textMode;
    std::unique_ptr<GraphicalVisualizationMode> graphicalMode;
    std::unique_ptr<WebVisualizationMode> webMode;
    
    // Current brain state for visualization
    struct BrainState {
        Timestamp currentTime;
        SimulationStep currentStep;
        float firingRate;
        size_t totalSpikes;
        size_t neuronCount;
        size_t activeNeuronCount;
        float excitationInhibitionRatio;
        float averageSynapticWeight;
        
        // System integration status
        bool memoryWorkingMemoryIntegrated;
        bool memoryEpisodicMemoryIntegrated;
        bool neuromodulationIntegrated;
        bool predictionIntegrated;
        bool developmentIntegrated;
        bool checkpointingWorks;
        bool replayWorks;
        
        // Memory system metrics
        size_t workingMemoryTraces;
        size_t episodicEpisodes;
        size_t associativeAssociations;
        
        // Neuromodulation metrics
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionErrorLevel;
        
        // Development metrics
        DevelopmentalStage developmentalStage;
        double developmentalAge;
        float plasticityModifier;
        
        // Performance metrics
        float fps;
        float simulationTime;
        
        // Checkpoint/replay status
        std::string currentCheckpointPath;
        bool isSaving;
        bool isLoading;
        
        // Experiment metrics
        float experimentReward;
        float experimentProgress;
        size_t experimentSteps;
        
        // Spike history
        std::vector<SpikeEvent> recentSpikes;
        
        // Neural population activity
        std::vector<std::pair<PopulationId, float>> populationActivity;
        
        BrainState() : 
            currentTime(0.0), currentStep(0), firingRate(0.0), totalSpikes(0),
            neuronCount(0), activeNeuronCount(0), excitationInhibitionRatio(0.0),
            averageSynapticWeight(0.0), memoryWorkingMemoryIntegrated(false),
            memoryEpisodicMemoryIntegrated(false), neuromodulationIntegrated(false),
            predictionIntegrated(false), developmentIntegrated(false),
            checkpointingWorks(false), replayWorks(false), workingMemoryTraces(0),
            episodicEpisodes(0), associativeAssociations(0), dopamineLevel(0.0),
            curiosityLevel(0.0), noveltyLevel(0.0), predictionErrorLevel(0.0),
            developmentalStage(DevelopmentalStage::Initial), developmentalAge(0.0),
            plasticityModifier(1.0), fps(0.0), simulationTime(0.0),
            isSaving(false), isLoading(false), experimentReward(0.0),
            experimentProgress(0.0), experimentSteps(0) {}
    };
    
    BrainState currentState;
    
    // Update statistics
    struct UpdateStats {
        std::chrono::high_resolution_clock::time_point lastUpdate;
        size_t frameCount;
        double totalUpdateTime;
        double minUpdateTime;
        double maxUpdateTime;
        
        UpdateStats() : lastUpdate(), frameCount(0), totalUpdateTime(0.0),
                       minUpdateTime(1000.0), maxUpdateTime(0.0) {}
    };
    
    UpdateStats updateStats;
    
    Impl() : active(false), updateRate(30.0), textMode(nullptr),
             graphicalMode(nullptr), webMode(nullptr) {
        updateStats.lastUpdate = std::chrono::high_resolution_clock::now();
    }
};

// Text-based visualization mode
class TextVisualizationMode {
public:
    TextVisualizationMode() = default;
    ~TextVisualizationMode() = default;
    
    void setState(const VisualizationInterface::Impl::BrainState& state) {
        state_ = state;
    }
    
    std::string generateBrainStateText() const {
        std::stringstream ss;
        
        ss << "=== NLM BRAIN VISUALIZATION ===\n";
        ss << "Time: " << std::fixed << std::setprecision(3) << state_.currentTime << "s\n";
        ss << "Step: " << state_.currentStep << "\n";
        ss << "FPS: " << std::fixed << std::setprecision(1) << state_.fps << "\n";
        ss << "\n";
        
        // Neural System Status
        ss << "--- NEURAL SYSTEM ---\n";
        ss << "Neurons: " << state_.neuronCount << " (active: " << state_.activeNeuronCount << ")\n";
        ss << "Firing Rate: " << std::fixed << std::setprecision(3) << state_.firingRate << " Hz\n";
        ss << "Total Spikes: " << state_.totalSpikes << "\n";
        ss << "E/I Ratio: " << std::fixed << std::setprecision(3) << state_.excitationInhibitionRatio << "\n";
        ss << "Average Synaptic Weight: " << std::fixed << std::setprecision(5) << state_.averageSynapticWeight << "\n";
        ss << "\n";
        
        // Memory System Visualization
        ss << "--- MEMORY SYSTEMS ---\n";
        ss << "Working Memory: " << state_.workingMemoryTraces << " traces\n";
        ss << "Episodic Memory: " << state_.episodicEpisodes << " episodes\n";
        ss << "Associative Memory: " << state_.associativeAssociations << " associations\n";
        ss << "Episodic Memory Integrated: " << (state_.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << "\n";
        ss << "Working Memory Integrated: " << (state_.memoryWorkingMemoryIntegrated ? "✓" : "✗") << "\n";
        ss << "\n";
        
        // Neuromodulation System
        ss << "--- NEUROMODULATION ---\n";
        ss << "Dopamine: " << std::fixed << std::setprecision(3) << state_.dopamineLevel << "\n";
        ss << "Curiosity: " << std::fixed << std::setprecision(3) << state_.curiosityLevel << "\n";
        ss << "Novelty: " << std::fixed << std::setprecision(3) << state_.noveltyLevel << "\n";
        ss << "Prediction Error: " << std::fixed << std::setprecision(3) << state_.predictionErrorLevel << "\n";
        ss << "Neuromodulation Integrated: " << (state_.neuromodulationIntegrated ? "✓" : "✗") << "\n";
        ss << "\n";
        
        // Development System
        ss << "--- DEVELOPMENT ---\n";
        ss << "Stage: " << getDevelopmentStageName(state_.developmentalStage) << "\n";
        ss << "Age: " << std::fixed << std::setprecision(1) << state_.developmentalAge << "\n";
        ss << "Plasticity Modifier: " << std::fixed << std::setprecision(3) << state_.plasticityModifier << "\n";
        ss << "Development Integrated: " << (state_.developmentIntegrated ? "✓" : "✗") << "\n";
        ss << "\n";
        
        // System Integration Status
        ss << "--- SYSTEM INTEGRATION ---\n";
        ss << "Prediction System: " << (state_.predictionIntegrated ? "✓" : "✗") << "\n";
        ss << "Development: " << (state_.developmentIntegrated ? "✓" : "✗") << "\n";
        ss << "Checkpointing: " << (state_.checkpointingWorks ? "✓" : "✗") << "\n";
        ss << "Replay: " << (state_.replayWorks ? "✓" : "✗") << "\n";
        ss << "\n";
        
        // Checkpoint Status
        ss << "--- CHECKPOINT STATUS ---\n";
        ss << "Current Path: " << state_.currentCheckpointPath << "\n";
        ss << "Saving: " << (state_.isSaving ? "YES" : "NO") << "\n";
        ss << "Loading: " << (state_.isLoading ? "YES" : "NO") << "\n";
        ss << "\n";
        
        // Recent Spike Activity
        if (!state_.recentSpikes.empty()) {
            ss << "--- RECENT SPIKES ---\n";
            int displayCount = std::min(10, static_cast<int>(state_.recentSpikes.size()));
            for (int i = 0; i < displayCount; ++i) {
                const auto& spike = state_.recentSpikes[state_.recentSpikes.size() - 1 - i];
                ss << "Step " << spike.step << ", Neuron " << spike.source_neuron.value 
                   << " at " << std::fixed << std::setprecision(3) << spike.timestamp << "s\n";
            }
            ss << "... " << state_.recentSpikes.size() << " total spikes\n";
            ss << "\n";
        }
        
        // Population Activity
        if (!state_.populationActivity.empty()) {
            ss << "--- POPULATION ACTIVITY ---\n";
            for (const auto& pop : state_.populationActivity) {
                ss << "Population " << pop.first.value << ": " 
                   << std::fixed << std::setprecision(3) << pop.second << " activity\n";
            }
            ss << "\n";
        }
        
        // Performance Metrics
        ss << "--- PERFORMANCE METRICS ---\n";
        ss << "Simulation Time: " << std::fixed << std::setprecision(3) << state_.simulationTime << "s\n";
        ss << "Update Time - Min: " << std::fixed << std::setprecision(6) << updateStats.minUpdateTime * 1000 << "ms\n";
        ss << "Update Time - Max: " << std::fixed << std::setprecision(6) << updateStats.maxUpdateTime * 1000 << "ms\n";
        ss << "Update Time - Avg: " << std::fixed << std::setprecision(6) << (updateStats.totalUpdateTime / updateStats.frameCount) * 1000 << "ms\n";
        ss << "\n";
        
        return ss.str();
    }
    
private:
    std::string getDevelopmentStageName(DevelopmentalStage stage) const {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    VisualizationInterface::Impl::BrainState state_;
    mutable UpdateStats updateStats;
};

// Graphical visualization mode
class GraphicalVisualizationMode {
public:
    GraphicalVisualizationMode() = default;
    ~GraphicalVisualizationMode() = default;
    
    bool initialize() {
        // TODO: Initialize graphical library (SDL, OpenGL, etc.)
        initialized_ = false;
        return true;
    }
    
    void update(const VisualizationInterface::Impl::BrainState& state) {
        // TODO: Update graphical display with new state
        state_ = state;
    }
    
    void render() {
        // TODO: Render current visualization
    }
    
    void close() {
        // TODO: Clean up graphical resources
    }
    
    bool isInitialized() const { return initialized_; }
    
    void setState(const VisualizationInterface::Impl::BrainState& state) {
        state_ = state;
    }
    
private:
    bool initialized_ = false;
    VisualizationInterface::Impl::BrainState state_;
};

// Web-based visualization mode
class WebVisualizationMode {
public:
    WebVisualizationMode() = default;
    ~WebVisualizationMode() = default;
    
    bool initialize() {
        // TODO: Initialize web server and WebSocket connections
        initialized_ = false;
        return true;
    }
    
    void update(const VisualizationInterface::Impl::BrainState& state) {
        // TODO: Send state update to web clients
        state_ = state;
    }
    
    void close() {
        // TODO: Stop web server
    }
    
    bool isInitialized() const { return initialized_; }
    
    std::string getWebSocketStatus() const {
        return "WebSocket: " + std::string(initialized_ ? "Running" : "Stopped");
    }
    
    void setState(const VisualizationInterface::Impl::BrainState& state) {
        state_ = state;
    }
    
private:
    bool initialized_ = false;
    VisualizationInterface::Impl::BrainState state_;
};

// Helper functions for visualization
namespace VisualizationUtils {
    std::string formatTime(double time) {
        std::stringstream ss;
        if (time > 3600) {
            ss << std::fixed << std::setprecision(1) << time / 3600 << "h";
        } else if (time > 60) {
            ss << std::fixed << std::setprecision(1) << time / 60 << "m";
        } else {
            ss << std::fixed << std::setprecision(3) << time << "s";
        }
        return ss.str();
    }
    
    std::string formatNumber(size_t num) {
        std::stringstream ss;
        if (num >= 1000000) {
            ss << std::fixed << std::setprecision(1) << num / 1000000.0 << "M";
        } else if (num >= 1000) {
            ss << num / 1000 << "K";
        } else {
            ss << num;
        }
        return ss.str();
    }
    
    std::string getStatusSymbol(bool value) {
        return value ? "✓" : "✗";
    }
    
    std::string getPercentage(float value, float max) {
        std::stringstream ss;
        if (max > 0) {
            ss << std::fixed << std::setprecision(1) << (value / max * 100) << "%";
        } else {
            ss << "N/A";
        }
        return ss.str();
    }
}

// VisualizationInterface implementation
VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize text visualization (always available)
    pImpl->textMode = std::make_unique<TextVisualizationMode>();
    
    // Initialize graphical visualization (if available)
    pImpl->graphicalMode = std::make_unique<GraphicalVisualizationMode>();
    pImpl->graphicalMode->initialize();
    
    // Initialize web visualization (if available)
    pImpl->webMode = std::make_unique<WebVisualizationMode>();
    pImpl->webMode->initialize();
    
    pImpl->active = true;
    pImpl->updateStats.lastUpdate = std::chrono::high_resolution_clock::now();
    
    return true;
}

void VisualizationInterface::update() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // TODO: Update with actual brain state from simulation
    // For now, update with placeholder state
    pImpl->currentState.currentStep++;
    pImpl->currentState.currentTime = pImpl->currentState.currentStep * 0.001;
    
    // Update text visualization mode
    if (pImpl->textMode) {
        pImpl->textMode->setState(pImpl->currentState);
    }
    
    // Update graphical visualization mode
    if (pImpl->graphicalMode && pImpl->graphicalMode->isInitialized()) {
        pImpl->graphicalMode->setState(pImpl->currentState);
        pImpl->graphicalMode->update(pImpl->currentState);
    }
    
    // Update web visualization mode
    if (pImpl->webMode && pImpl->webMode->isInitialized()) {
        pImpl->webMode->setState(pImpl->currentState);
        pImpl->webMode->update(pImpl->currentState);
    }
    
    // Update performance stats
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - pImpl->updateStats.lastUpdate).count();
    
    pImpl->updateStats.frameCount++;
    pImpl->updateStats.totalUpdateTime += duration;
    pImpl->updateStats.minUpdateTime = std::min(pImpl->updateStats.minUpdateTime, duration);
    pImpl->updateStats.maxUpdateTime = std::max(pImpl->updateStats.maxUpdateTime, duration);
    pImpl->updateStats.lastUpdate = std::chrono::high_resolution_clock::now();
}

void VisualizationInterface::render() {
    // Update FPS
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - lastFrameTime).count();
    
    if (elapsed > 0.0) {
        pImpl->currentState.fps = 1.0 / elapsed;
    }
    lastFrameTime = currentTime;
    
    // Render using text mode (always available)
    if (pImpl->textMode) {
        std::cout << pImpl->textMode->generateBrainStateText() << std::flush;
    }
    
    // Render using graphical mode (if available)
    if (pImpl->graphicalMode && pImpl->graphicalMode->isInitialized()) {
        pImpl->graphicalMode->render();
    }
}

void VisualizationInterface::close() {
    if (pImpl->graphicalMode) {
        pImpl->graphicalMode->close();
    }
    
    if (pImpl->webMode) {
        pImpl->webMode->close();
    }
    
    pImpl->active = false;
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // TODO: Implement network visualization (graphical mode)
    // For now, update the current state with network information
    // This would update connectivity, neuron populations, etc.
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // TODO: Implement activity visualization (graphical mode)
    // For now, update the current state with activity information
    // This would update neural activity, firing patterns, etc.
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // TODO: Implement spike visualization (graphical mode)
    // For now, update the current state with spike information
    pImpl->currentState.recentSpikes = spikes;
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // TODO: Implement weight matrix visualization (graphical mode)
    // For now, update the current state with weight information
    // This would update connectivity strength, plasticity, etc.
}

// System-specific visualization methods
void VisualizationInterface::visualizeRealTimeBrainState(const Brain& brain, const SpikeSystem& spikeSystem) {
    // Real-time brain monitoring (neurons, spikes, connectivity)
    pImpl->currentState.currentStep = spikeSystem.getSpikeCount();
    pImpl->currentState.currentTime = pImpl->currentState.currentStep * 0.001;
    pImpl->currentState.firingRate = spikeSystem.getAverageSpikeRate();
    pImpl->currentState.totalSpikes = spikeSystem.getSpikeCount();
    pImpl->currentState.neuronCount = brain.getTotalNeuronCount();
    pImpl->currentState.activeNeuronCount = brain.getActiveNeuronCount();
    pImpl->currentState.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    
    // Get spike history for visualization
    const auto& spikeHistory = spikeSystem.getSpikeHistory();
    pImpl->currentState.recentSpikes.clear();
    for (const auto& event : spikeHistory) {
        if (pImpl->currentState.recentSpikes.size() < 1000) { // Limit history size
            pImpl->currentState.recentSpikes.push_back(
                SpikeEvent(event.source, event.timestamp, event.step)
            );
        }
    }
}

void VisualizationInterface::visualizeSystemIntegration(const Brain& brain) {
    // System integration visualization
    pImpl->currentState.memoryWorkingMemoryIntegrated = (brain.getWorkingMemory() != nullptr);
    pImpl->currentState.memoryEpisodicMemoryIntegrated = (brain.getEpisodicMemory() != nullptr);
    pImpl->currentState.neuromodulationIntegrated = (brain.getDopamine() != nullptr);
    pImpl->currentState.predictionIntegrated = (brain.getPredictionSystem() != nullptr);
    pImpl->currentState.developmentIntegrated = (brain.getDevelopmentSystem() != nullptr);
    
    // Get memory metrics
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        pImpl->currentState.workingMemoryTraces = workingMemory->getActiveTraces();
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        pImpl->currentState.episodicEpisodes = episodicMemory->getEpisodeCount();
    }
    
    auto* associativeMemory = brain.getAssociativeMemory();
    if (associativeMemory) {
        pImpl->currentState.associativeAssociations = associativeMemory->getAssociationCount();
    }
}

void VisualizationInterface::visualizeMemorySystem(const Brain& brain) {
    // Memory system visualization (memory traces, episodes, consolidation)
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    if (workingMemory) {
        pImpl->currentState.workingMemoryTraces = workingMemory->getActiveTraces();
    }
    
    if (episodicMemory) {
        pImpl->currentState.episodicEpisodes = episodicMemory->getEpisodeCount();
        pImpl->currentState.experimentReward = episodicMemory->getAverageReward();
    }
    
    if (associativeMemory) {
        pImpl->currentState.associativeAssociations = associativeMemory->getAssociationCount();
    }
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain) {
    // Neuromodulation visualization (neuromodulator levels and effects)
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    if (dopamine) {
        pImpl->currentState.dopamineLevel = dopamine->getLevel();
    }
    
    if (curiosity) {
        pImpl->currentState.curiosityLevel = curiosity->getLevel();
    }
    
    if (novelty) {
        pImpl->currentState.noveltyLevel = novelty->getLevel();
    }
    
    if (predictionError) {
        pImpl->currentState.predictionErrorLevel = predictionError->getLevel();
    }
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    // Development visualization (developmental stage progression)
    auto* developmentSystem = brain.getDevelopmentSystem();
    if (developmentSystem) {
        pImpl->currentState.developmentalStage = developmentSystem->getStage();
        pImpl->currentState.developmentalAge = developmentSystem->getDevelopmentalAge();
        pImpl->currentState.plasticityModifier = developmentSystem->getPlasticityModifier();
    }
}

void VisualizationInterface::visualizePerformanceMetrics(const Brain& brain, double simulationTime) {
    // Performance metrics (firing rates, spike counts, system health)
    pImpl->currentState.firingRate = brain.getAverageFiringRate();
    pImpl->currentState.totalSpikes = brain.getTotalSpikeCount();
    pImpl->currentState.simulationTime = simulationTime;
    pImpl->currentState.experimentSteps = simulationTime * 1000; // Approximate conversion
}

void VisualizationInterface::visualizeCheckpoint(const Brain& brain, const std::string& checkpointPath, bool isSaving, bool isLoading) {
    // Checkpoint visualization (save/load operations)
    pImpl->currentState.currentCheckpointPath = checkpointPath;
    pImpl->currentState.isSaving = isSaving;
    pImpl->currentState.isLoading = isLoading;
    
    if (isSaving && brain.save(checkpointPath)) {
        pImpl->currentState.checkpointingWorks = true;
    }
    
    if (isLoading && !brain.load(checkpointPath)) {
        pImpl->currentState.checkpointingWorks = false;
    }
}

void VisualizationInterface::visualizeExperiment(float reward, float progress, size_t steps) {
    // Experiment visualization (experiment progress and results)
    pImpl->currentState.experimentReward = reward;
    pImpl->currentState.experimentProgress = progress;
    pImpl->currentState.experimentSteps = steps;
}

std::string VisualizationInterface::getStatus() const {
    std::stringstream ss;
    ss << "Visualization Status:" << std::endl;
    ss << "  Active: " << (pImpl->active ? "Yes" : "No") << std::endl;
    ss << "  Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    ss << "  Brain State: " << pImpl->currentState.currentStep << " steps, " 
       << pImpl->currentState.currentTime << "s" << std::endl;
    ss << "  Neurons: " << pImpl->currentState.neuronCount << " active: " 
       << pImpl->currentState.activeNeuronCount << std::endl;
    ss << "  System Integration: " << (pImpl->currentState.memoryWorkingMemoryIntegrated ? "OK" : "Pending") << std::endl;
    return ss.str();
}

std::string VisualizationInterface::getWebSocketStatus() const {
    return pImpl->webMode ? pImpl->webMode->getWebSocketStatus() : "Web mode not available";
}

} // namespace nlm
