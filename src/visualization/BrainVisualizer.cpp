#pragma once

#include "BrainVisualizer.hpp"
#include "../core/Logger/Logger.hpp"
#include <map>
#include <algorithm>

namespace nlm {

struct BrainVisualizer::Impl {
    std::shared_ptr<VisualizationCore> core;
    std::map<VisualizationType, std::unique_ptr<VisualizationRenderer>> renderers;
    bool showNetworkConnections;
    bool showNeuronActivity;
    bool showSpikes;
    bool showWeightMatrices;
    bool showStateSpace;
    double opacity;
    
    Impl() : showNetworkConnections(true), showNeuronActivity(true), 
             showSpikes(true), showWeightMatrices(false), showStateSpace(false),
             opacity(1.0) {}
};

BrainVisualizer::BrainVisualizer(std::shared_ptr<VisualizationCore> core) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->core = core;
    initializeRenderers();
}

BrainVisualizer::~BrainVisualizer() = default;

void BrainVisualizer::initializeRenderers() {
    // Create renderers for different visualization types
    pImpl->renderers[VisualizationType::Network] = std::make_unique<NetworkRenderer>("Network");
    pImpl->renderers[VisualizationType::Activity] = std::make_unique<ActivityRenderer>("Activity");
    pImpl->renderers[VisualizationType::Spikes] = std::make_unique<SpikeRenderer>("Spikes");
    pImpl->renderers[VisualizationType::Weights] = std::make_unique<WeightRenderer>("Weights");
    pImpl->renderers[VisualizationType::StateSpace] = std::make_unique<StateSpaceRenderer>("StateSpace");
}

void BrainVisualizer::update(const Brain& brain, double deltaTime) {
    // Collect data from brain for visualization
    for (auto& pair : pImpl->renderers) {
        if (pImpl->core->isVisualizationEnabled(pair.first)) {
            pair.second->update(brain, deltaTime);
        }
    }
}

void BrainVisualizer::render() {
    // Render all enabled visualizations
    for (auto& pair : pImpl->renderers) {
        if (pImpl->core->isVisualizationEnabled(pair.first)) {
            pair.second->render();
        }
    }
}

void BrainVisualizer::setVisualizationOption(VisualizationOption option, bool enabled) {
    switch (option) {
        case VisualizationOption::NetworkConnections:
            pImpl->showNetworkConnections = enabled;
            break;
        case VisualizationOption::NeuronActivity:
            pImpl->showNeuronActivity = enabled;
            break;
        case VisualizationOption::Spikes:
            pImpl->showSpikes = enabled;
            break;
        case VisualizationOption::WeightMatrices:
            pImpl->showWeightMatrices = enabled;
            break;
        case VisualizationOption::StateSpace:
            pImpl->showStateSpace = enabled;
            break;
    }
}

bool BrainVisualizer::getVisualizationOption(VisualizationOption option) const {
    switch (option) {
        case VisualizationOption::NetworkConnections:
            return pImpl->showNetworkConnections;
        case VisualizationOption::NeuronActivity:
            return pImpl->showNeuronActivity;
        case VisualizationOption::Spikes:
            return pImpl->showSpikes;
        case VisualizationOption::WeightMatrices:
            return pImpl->showWeightMatrices;
        case VisualizationOption::StateSpace:
            return pImpl->showStateSpace;
        default:
            return false;
    }
}

void BrainVisualizer::exportVisualization(const std::string& filename, VisualizationFormat format) {
    NLM_LOG_INFO("BrainVisualizer: Exporting visualization to " + filename + "." + formatToString(format));
    // Export implementation would go here
}

void BrainVisualizer::addDataSource(const std::string& name, std::shared_ptr<VisualizationData> data) {
    // Add data source for visualization
    NLM_LOG_INFO("BrainVisualizer: Added data source '" + name + "'");
}

void BrainVisualizer::removeDataSource(const std::string& name) {
    // Remove data source
    NLM_LOG_INFO("BrainVisualizer: Removed data source '" + name + "'");
}

} // namespace nlm