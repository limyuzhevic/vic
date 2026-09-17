#include "ConsoleVisualization.hpp"
#include "../core/Logger/Logger.hpp"
#include "Brain.hpp"
#include "SimpleWorld.hpp"
#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

namespace nlm {

struct ConsoleVisualization::Impl {
    // Configuration flags
    bool enableNetworkViz = true;
    bool enableActivityViz = true;
    bool enableSpikesViz = true;
    bool enableWeightsViz = false;
    bool enableBrainStatesViz = true;
    bool enableAgentInteractionViz = true;
    bool enableResultsViz = false;
    
    // Display settings
    VisualizationColorMode colorMode = VisualizationColorMode::Heatmap;
    size_t maxSpikesDisplay = 100;
    size_t maxNetworkNodes = 50;
    
    // Performance tracking
    double updateRate = 30.0;
    bool active = false;
    
    // State tracking
    uint64_t lastStep = 0;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    // Spike history for raster plots
    std::vector<SpikeEvent> spikeHistory;
    
    // Network state cache
    std::vector<std::pair<NeuronId, double>> neuronActivity;
    std::vector<std::pair<std::pair<NeuronId, NeuronId>, double>> synapseWeights;
    
    // Progress tracking
    std::vector<double> learningCurveWeights;
    std::vector<double> learningCurveRewards;
    std::vector<double> learningCurveErrors;
    
    Impl() : lastStep(0), active(false) {
        lastUpdateTime = std::chrono::steady_clock::now();
    }
};

ConsoleVisualization::ConsoleVisualization() : pImpl(std::make_unique<Impl>()) {}

ConsoleVisualization::~ConsoleVisualization() = default;

bool ConsoleVisualization::initialize() {
    NLM_LOG_INFO("Initializing Console Visualization...");
    
    // Clear screen and setup
    clearScreen();
    printHeader("NLM Phase 6: Neural Network Visualization (Console Mode)");
    
    pImpl->active = true;
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Console Visualization initialized successfully");
    return true;
}

void ConsoleVisualization::update() {
    if (!pImpl->active) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastUpdateTime).count();
    
    if (elapsed >= 1.0 / pImpl->updateRate) {
        // Update state would be called from main loop
        pImpl->lastUpdateTime = now;
    }
}

void ConsoleVisualization::render() {
    clearScreen();
    printHeader("NLM Phase 6: Neural Network Visualization (Console Mode)");
    
    std::cout << std::fixed << std::setprecision(2);
    
    // Display neural network visualization
    if (pImpl->enableNetworkViz) {
        std::cout << "\n=== NEURAL NETWORK VISUALIZATION ===" << std::endl;
        std::cout << "Neurons:" << std::setw(10) << pImpl->neuronActivity.size() << "/" << pImpl->maxNetworkNodes << std::endl;
        std::cout << "Active Neurons:" << std::setw(10) << std::count_if(pImpl->neuronActivity.begin(), pImpl->neuronActivity.end(),
            [](const auto& n) { return n.second > 0.5; }) << std::endl;
        
        // Simple text-based network graph
        std::cout << "\nNetwork Topology (simplified):" << std::endl;
        for (size_t i = 0; i < std::min(pImpl->neuronActivity.size(), size_t(10)); ++i) {
            const auto& neuron = pImpl->neuronActivity[i];
            std::string active = neuron.second > 0.5 ? "[ACTIVE]" : "      ";
            std::cout << "  Neuron " << std::setw(4) << neuron.first << ": " << active << " rate: " << neuron.second << "Hz" << std::endl;
        }
        
        // Synapse visualization
        if (pImpl->enableWeightsViz) {
            std::cout << "\n  Synapses:" << std::endl;
            for (size_t i = 0; i < std::min(pImpl->synapseWeights.size(), size_t(10)); ++i) {
                const auto& synapse = pImpl->synapseWeights[i];
                std::string strength = "";
                switch (pImpl->colorMode) {
                    case VisualizationColorMode::Heatmap:
                        strength = (synapse.second > 0.5 ? "[STRONG]" : synapse.second > 0.2 ? "[MEDIUM]" : "[WEAK]");
                        break;
                    case VisualizationColorMode::Category:
                        strength = (synapse.second > 0.5 ? "[*]*" : synapse.second > 0.2 ? "[.]" : "[ ]") * 10;
                        break;
                    default:
                        strength = "";
                }
                std::cout << "    " << strength << " Synapse " << synapse.first.first << " -> " << synapse.first.second << ": " << synapse.second << std::endl;
            }
        }
    }
    
    // Display activity visualization
    if (pImpl->enableActivityViz) {
        std::cout << "\n=== ACTIVITY VISUALIZATION ===" << std::endl;
        std::cout << "Total Firing Rate: " << pImpl->calculateAverageFiringRate() << " Hz" << std::endl;
        std::cout << "Spike History: " << pImpl->spikeHistory.size() << " events (showing " << pImpl->maxSpikesDisplay << ")" << std::endl;
        
        // Spike raster plot (simplified)
        if (!pImpl->spikeHistory.empty()) {
            std::cout << "\nSpike Raster (last " << pImpl->maxSpikesDisplay << " events):" << std::endl;
            for (size_t i = 0; i < std::min(pImpl->spikeHistory.size(), pImpl->maxSpikesDisplay); ++i) {
                const auto& spike = pImpl->spikeHistory[i];
                std::cout << "  Time: " << std::setw(6) << spike.time << "  Neuron: " << spike.neuronId << std::endl;
            }
        }
    }
    
    // Display brain states visualization
    if (pImpl->enableBrainStatesViz) {
        std::cout << "\n=== BRAIN STATE VISUALIZATION ===" << std::endl;
        std::cout << "Working Memory: [█] Active" << std::endl;
        std::cout << "Episodic Memory: [█] " << pImpl->calculateMemoryStats() << " episodes" << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  Dopamine: [█] Active" << std::endl;
        std::cout << "  Curiosity: [█] Active" << std::endl;
        std::cout << "  Novelty: [█] Active" << std::endl;
        std::cout << "Prediction Error: [█] Active" << std::endl;
        std::cout << "Attention: [█] Focused" << std::endl;
    }
    
    // Display agent interaction visualization
    if (pImpl->enableAgentInteractionViz) {
        std::cout << "\n=== AGENT INTERACTION VISUALIZATION ===" << std::endl;
        std::cout << "Sensory Input:" << std::endl;
        std::cout << "  Vision: [█] Processing" << std::endl;
        std::cout << "  Touch: [█] Active" << std::endl;
        std::cout << "  Internal: [█] Monitoring" << std::endl;
        
        std::cout << "\nMotor Commands:" << std::endl;
        std::cout << "  Position: [█] Moving" << std::endl;
        std::cout << "  Velocity: [█] Active" << std::endl;
        std::cout << "  Acceleration: [█] Changing" << std::endl;
        
        std::cout << "\nExploration Behavior:" << std::endl;
        std::cout << "  Curiosity Level: [█] " << pImpl->calculateCuriosityLevel() << std::endl;
        std::cout << "  Novelty Detection: [█] Active" << std::endl;
    }
    
    // Display experimental results visualization
    if (pImpl->enableResultsViz) {
        std::cout << "\n=== EXPERIMENTAL RESULTS VISUALIZATION ===" << std::endl;
        
        if (!pImpl->learningCurveWeights.empty()) {
            std::cout << "Learning Curves:" << std::endl;
            std::cout << "  Weight Changes: [█] Showing over time" << std::endl;
            std::cout << "  Reward History: [█] " << pImpl->learningCurveRewards.size() << " samples" << std::endl;
            std::cout << "  Prediction Errors: [█] " << pImpl->learningCurveErrors.size() << " samples" << std::endl;
            
            // Simple ASCII plot for last few data points
            std::cout << "\nLast 10 Weight Changes:" << std::endl;
            for (size_t i = 0; i < std::min(pImpl->learningCurveWeights.size(), size_t(10)); ++i) {
                size_t idx = pImpl->learningCurveWeights.size() - 10 + i;
                if (idx < pImpl->learningCurveWeights.size()) {
                    double weight = pImpl->learningCurveWeights[idx];
                    std::string bar = "";
                    for (int j = 0; j < 20; ++j) {
                        if (weight > static_cast<double>(j) / 20.0 * 2.0) bar += "█";
                        else bar += " ";
                    }
                    std::cout << "  " << std::setw(4) << idx << ": " << bar << " " << weight << std::endl;
                }
            }
        }
        
        std::cout << "\nMemory Consolidation:" << std::endl;
        std::cout << "  Replay Events: [█] Active" << std::endl;
        std::cout << "  Development: [█] Ongoing" << std::endl;
    }
    
    // Display performance stats
    printStats(*(static_cast<Brain*>(nullptr)), 0.0f);  // Pass nullptr for now
    
    std::cout << std::endl << "=== END OF FRAME ===" << std::endl;
    std::cout.flush();
}

void ConsoleVisualization::close() {
    NLM_LOG_INFO("Closing Console Visualization...");
    clearScreen();
    printHeader("NLM Visualization Terminated");
    pImpl->active = false;
}

bool ConsoleVisualization::isActive() const {
    return pImpl->active;
}

void ConsoleVisualization::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double ConsoleVisualization::getUpdateRate() const {
    return pImpl->updateRate;
}

// Neural network visualization implementations
void ConsoleVisualization::visualizeNetwork(const Brain& brain) {
    // Cache neuron activity
    pImpl->neuronActivity.clear();
    for (const auto& region : brain.getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuron : population->getNeurons()) {
                NeuronId id = neuron->getId();
                double rate = neuron->getFiringRate();
                pImpl->neuronActivity.emplace_back(id, rate);
                
                // Sort and limit
                std::sort(pImpl->neuronActivity.begin(), pImpl->neuronActivity.end(),
                         [](const auto& a, const auto& b) { return a.second > b.second; });
                if (pImpl->neuronActivity.size() > pImpl->maxNetworkNodes) {
                    pImpl->neuronActivity.resize(pImpl->maxNetworkNodes);
                }
            }
        }
    }
}

void ConsoleVisualization::visualizeActivity(const Brain& brain) {
    // Update firing rate statistics
    double avgFiringRate = brain.getAverageFiringRate();
    size_t firingCount = brain.getFiringNeuronCount();
    
    // This would update any activity-related visualization state
    // For console, we just use the cached data from visualizeNetwork
}

void ConsoleVisualization::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Update spike history
    pImpl->spikeHistory.insert(pImpl->spikeHistory.end(), spikes.begin(), spikes.end());
    
    // Limit spike history size
    if (pImpl->spikeHistory.size() > pImpl->maxSpikesDisplay) {
        auto start = pImpl->spikeHistory.end() - pImpl->maxSpikesDisplay;
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), start);
    }
}

void ConsoleVisualization::visualizeWeights(const Brain& brain) {
    // Cache synapse weights
    pImpl->synapseWeights.clear();
    for (const auto& region : brain.getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuron : population->getNeurons()) {
                for (const auto& synapseHandle : neuron->getOutgoingSynapses()) {
                    if (synapseHandle) {
                        SynapseId synId = synapseHandle->getId();
                        NeuronId source = synapseHandle->getSourceNeuron();
                        NeuronId dest = synapseHandle->getDestinationNeuron();
                        double weight = synapseHandle->getWeight();
                        pImpl->synapseWeights.emplace_back(std::make_pair(source, dest), weight);
                    }
                }
            }
        }
    }
    
    // Sort and limit
    std::sort(pImpl->synapseWeights.begin(), pImpl->synapseWeights.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
    if (pImpl->synapseWeights.size() > 100) {
        pImpl->synapseWeights.resize(100);
    }
}

// Brain state visualization implementations
void ConsoleVisualization::visualizeBrainStates(const Brain& brain) {
    // Console implementation uses cached data from other methods
    // This is a placeholder for future expansion
}

void ConsoleVisualization::visualizeNeuromodulation(const Brain& brain) {
    // Console implementation would show neuromodulation levels
    // This uses cached data from other methods
}

void ConsoleVisualization::visualizePredictionErrors(const Brain& brain) {
    // Console implementation would show prediction error visualization
    // This uses cached data from other methods
}

void ConsoleVisualization::visualizeAttention(const Brain& brain) {
    // Console implementation would show attention areas
    // This uses cached data from other methods
}

// Agent interaction visualization implementations
void ConsoleVisualization::visualizeSensoryInput(const Brain& brain) {
    // Console implementation would show sensory input visualization
    // This would display vision, touch, internal signals
}

void ConsoleVisualization::visualizeMotorCommands(const Brain& brain) {
    // Console implementation would show motor command visualization
    // This would display agent movement and actions
}

void ConsoleVisualization::visualizeAgentPosition(const Brain& brain) {
    // Console implementation would show agent position in world
    // This would display x,y coordinates and orientation
}

void ConsoleVisualization::visualizeExploration(const Brain& brain) {
    // Console implementation would show curiosity and exploration
    // This would display exploration metrics
}

// Experimental results visualization implementations
void ConsoleVisualization::plotLearningCurves(const std::vector<double>& weights,
                                           const std::vector<double>& rewards,
                                           const std::vector<double>& errors) {
    pImpl->learningCurveWeights = weights;
    pImpl->learningCurveRewards = rewards;
    pImpl->learningCurveErrors = errors;
}

void ConsoleVisualization::visualizeMemoryConsolidation(const Brain& brain) {
    // Console implementation would show memory consolidation events
    // This would display replay events and consolidation progress
}

void ConsoleVisualization::visualizeDevelopment(const Brain& brain) {
    // Console implementation would show developmental progression
    // This would display developmental stage and changes
}

// Configuration setters
void ConsoleVisualization::setEnableNetworkViz(bool enable) {
    pImpl->enableNetworkViz = enable;
}

void ConsoleVisualization::setEnableActivityViz(bool enable) {
    pImpl->enableActivityViz = enable;
}

void ConsoleVisualization::setEnableSpikesViz(bool enable) {
    pImpl->enableSpikesViz = enable;
}

void ConsoleVisualization::setEnableWeightsViz(bool enable) {
    pImpl->enableWeightsViz = enable;
}

void ConsoleVisualization::setEnableBrainStatesViz(bool enable) {
    pImpl->enableBrainStatesViz = enable;
}

void ConsoleVisualization::setEnableAgentInteractionViz(bool enable) {
    pImpl->enableAgentInteractionViz = enable;
}

void ConsoleVisualization::setEnableResultsViz(bool enable) {
    pImpl->enableResultsViz = enable;
}

void ConsoleVisualization::setMaxSpikesDisplay(size_t maxSpikes) {
    pImpl->maxSpikesDisplay = maxSpikes;
    if (pImpl->spikeHistory.size() > maxSpikes) {
        auto start = pImpl->spikeHistory.end() - maxSpikes;
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), start);
    }
}

void ConsoleVisualization::setMaxNetworkNodes(size_t maxNodes) {
    pImpl->maxNetworkNodes = maxNodes;
    if (pImpl->neuronActivity.size() > maxNodes) {
        auto start = pImpl->neuronActivity.end() - maxNodes;
        pImpl->neuronActivity.erase(pImpl->neuronActivity.begin(), start);
    }
}

void ConsoleVisualization::setColorMode(VisualizationColorMode mode) {
    pImpl->colorMode = mode;
}

// Console control methods
void ConsoleVisualization::clearScreen() const {
    std::cout << "\033[2J\033[1;1H";  // ANSI escape codes for clear screen
}

void ConsoleVisualization::setCursorPosition(int x, int y) const {
    std::cout << "\033[" << y << ";" << x << "H";
}

void ConsoleVisualization::printHeader(const std::string& title) const {
    clearScreen();
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "  " << std::string(80, '=') << std::endl;
}

void ConsoleVisualization::printStats(const Brain& brain, float simTime) const {
    std::cout << std::endl << "=== PERFORMANCE STATS ===" << std::endl;
    std::cout << "Simulation Time: " << simTime << "s" << std::endl;
    std::cout << "Active Visualizations:" << std::endl;
    std::cout << "  Network: " << (pImpl->enableNetworkViz ? "ON" : "OFF") << std::endl;
    std::cout << "  Activity: " << (pImpl->enableActivityViz ? "ON" : "OFF") << std::endl;
    std::cout << "  Spikes: " << (pImpl->enableSpikesViz ? "ON" : "OFF") << std::endl;
    std::cout << "  Weights: " << (pImpl->enableWeightsViz ? "ON" : "OFF") << std::endl;
    std::cout << "  Brain States: " << (pImpl->enableBrainStatesViz ? "ON" : "OFF") << std::endl;
    std::cout << "  Agent Interaction: " << (pImpl->enableAgentInteractionViz ? "ON" : "OFF") << std::endl;
    std::cout << "  Results: " << (pImpl->enableResultsViz ? "ON" : "OFF") << std::endl;
    std::cout << "Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    std::cout << "Data Points:" << std::endl;
    std::cout << "  Neurons Cached: " << pImpl->neuronActivity.size() << std::endl;
    std::cout << "  Spikes Cached: " << pImpl->spikeHistory.size() << std::endl;
    std::cout << "  Synapses Cached: " << pImpl->synapseWeights.size() << std::endl;
}

// Helper methods
double ConsoleVisualization::Impl::calculateAverageFiringRate() const {
    if (neuronActivity.empty()) return 0.0;
    double total = 0.0;
    for (const auto& neuron : neuronActivity) {
        total += neuron.second;
    }
    return total / neuronActivity.size();
}

std::string ConsoleVisualization::Impl::calculateMemoryStats() const {
    return "0";  // Placeholder - would get actual memory count from brain
}

float ConsoleVisualization::Impl::calculateCuriosityLevel() const {
    return 0.5f;  // Placeholder - would get actual curiosity from brain
}

} // namespace nlm