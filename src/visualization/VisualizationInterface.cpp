#include "VisualizationInterface.hpp"
#include "NeuralVisualizer.hpp"
#include "NetworkTopologyVisualizer.hpp"
#include "ActivityHeatMapVisualizer.hpp"
#include "TimeSeriesVisualizer.hpp"
#include "StatisticalChartVisualizer.hpp"
#include "DashboardVisualizer.hpp"
#include "BrainStateVisualizer.hpp"
#include <iostream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::shared_ptr<NeuralVisualizer> neuralVisualizer;
    std::shared_ptr<NetworkTopologyVisualizer> topologyVisualizer;
    std::shared_ptr<ActivityHeatMapVisualizer> heatMapVisualizer;
    std::shared_ptr<TimeSeriesVisualizer> timeSeriesVisualizer;
    std::shared_ptr<StatisticalChartVisualizer> statisticalVisualizer;
    std::shared_ptr<DashboardVisualizer> dashboardVisualizer;
    std::shared_ptr<BrainStateVisualizer> brainStateVisualizer;
    
    Impl() : active(false), updateRate(30.0) {
        neuralVisualizer = std::make_shared<NeuralVisualizer>();
        topologyVisualizer = std::make_shared<NetworkTopologyVisualizer>();
        heatMapVisualizer = std::make_shared<ActivityHeatMapVisualizer>();
        timeSeriesVisualizer = std::make_shared<TimeSeriesVisualizer>();
        statisticalVisualizer = std::make_shared<StatisticalChartVisualizer>();
        dashboardVisualizer = std::make_shared<DashboardVisualizer>();
        brainStateVisualizer = std::make_shared<BrainStateVisualizer>();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // PLACEHOLDER: Use new visualization classes
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    // Update all visualization components
    if (pImpl->neuralVisualizer) pImpl->neuralVisualizer->update();
    if (pImpl->topologyVisualizer) pImpl->topologyVisualizer->update();
    if (pImpl->heatMapVisualizer) pImpl->heatMapVisualizer->update();
    if (pImpl->timeSeriesVisualizer) pImpl->timeSeriesVisualizer->update();
    if (pImpl->statisticalVisualizer) pImpl->statisticalVisualizer->update();
    if (pImpl->dashboardVisualizer) pImpl->dashboardVisualizer->update();
    if (pImpl->brainStateVisualizer) pImpl->brainStateVisualizer->update();
}

void VisualizationInterface::render() {
    // Render all visualization components
    if (pImpl->neuralVisualizer) pImpl->neuralVisualizer->render();
    if (pImpl->topologyVisualizer) pImpl->topologyVisualizer->render();
    if (pImpl->heatMapVisualizer) pImpl->heatMapVisualizer->render();
    if (pImpl->timeSeriesVisualizer) pImpl->timeSeriesVisualizer->render();
    if (pImpl->statisticalVisualizer) pImpl->statisticalVisualizer->render();
    if (pImpl->dashboardVisualizer) pImpl->dashboardVisualizer->render();
    if (pImpl->brainStateVisualizer) pImpl->brainStateVisualizer->render();
}

void VisualizationInterface::close() {
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
    if (pImpl->topologyVisualizer) {
        pImpl->topologyVisualizer->setVisible(true);
        pImpl->topologyVisualizer->initialize(std::make_shared<Brain>(brain));
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (pImpl->neuralVisualizer) {
        pImpl->neuralVisualizer->setVisible(true);
        pImpl->neuralVisualizer->initialize(std::make_shared<Brain>(brain));
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (pImpl->timeSeriesVisualizer) {
        // In a real implementation, would add spike data to time series
        pImpl->timeSeriesVisualizer->setVisible(true);
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (pImpl->statisticalVisualizer) {
        pImpl->statisticalVisualizer->setVisible(true);
        pImpl->statisticalVisualizer->initialize(std::make_shared<Brain>(brain));
    }
}

} // namespace nlm
