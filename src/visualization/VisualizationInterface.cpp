#include "VisualizationInterface.hpp"
#include "BrainVisualizer.hpp"
#include "VisualizationFactory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../memory/Memory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::vector<std::unique_ptr<BrainVisualizer>> visualizers;
    std::map<std::string, std::unique_ptr<BrainVisualizer>> visualizerMap;
    
    Impl() : active(false), updateRate(30.0) {
        // Initialize default visualizers
        visualizers.push_back(VisualizationFactory::createNeuralNetworkVisualizer("default"));
        visualizers.push_back(VisualizationFactory::createMemoryVisualizer("default"));
        visualizers.push_back(VisualizationFactory::createNeuromodulationVisualizer("default"));
        visualizers.push_back(VisualizationFactory::createCognitiveVisualizer("default"));
        visualizers.push_back(VisualizationFactory::createDevelopmentVisualizer("default"));
        visualizers.push_back(VisualizationFactory::createPerformanceVisualizer("default"));
        
        // Build visualizer map for easy access
        for (const auto& viz : visualizers) {
            visualizerMap[viz->getName()] = viz->clone();
        }
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // PLACEHOLDER: No visualization in Phase 1
    pImpl->active = false;
    
    // Initialize all visualizers
    for (auto& viz : pImpl->visualizers) {
        viz->initialize();
    }
    
    return true;
}

void VisualizationInterface::update() {
    // PLACEHOLDER: No visualization in Phase 1
}

void VisualizationInterface::render() {
    // PLACEHOLDER: No visualization in Phase 1
}

void VisualizationInterface::close() {
    pImpl->active = false;
    for (auto& viz : pImpl->visualizers) {
        viz->close();
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    for (auto& viz : pImpl->visualizers) {
        viz->setUpdateRate(hz);
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // TODO PHASE 2: Implement network visualization
    // For now, just log that it's being called
    NLM_LOG_INFO("VisualizationInterface: visualizing network topology");
    
    // In a complete implementation, we would:
    // 1. Collect neuron and synapse data from brain
    // 2. Update visualizers with the data
    // 3. Render the visualizations
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // TODO PHASE 2: Implement activity visualization
    NLM_LOG_INFO("VisualizationInterface: visualizing neural activity");
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // TODO PHASE 2: Implement spike raster plot
    NLM_LOG_INFO("VisualizationInterface: visualizing spike activity");
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // TODO PHASE 2: Implement weight matrix visualization
    NLM_LOG_INFO("VisualizationInterface: visualizing synaptic weights");
}

} // namespace nlm