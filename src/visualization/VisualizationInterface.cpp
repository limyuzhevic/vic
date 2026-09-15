#include "VisualizationInterface.hpp"
#include "VisualizationSystem.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class VisualizationInterface::Impl {
public:
    std::unique_ptr<VisualizationSystem> visualizer;
    bool active;
    
    Impl() : active(false) {
        visualizer = std::make_unique<VisualizationSystem>();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}
VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    return pImpl->visualizer->initialize();
}

void VisualizationInterface::update() {
    // TODO: Get current timestep from clock
    pImpl->visualizer->update(*pImpl->brain, 0.001);
}

void VisualizationInterface::render() {
    pImpl->visualizer->render();
}

void VisualizationInterface::close() {
    pImpl->visualizer->close();
    pImpl->active = false;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active && pImpl->visualizer->isActive();
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->visualizer->setUpdateRate(hz);
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->visualizer->getUpdateRate();
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    pImpl->visualizer->visualizeNetwork(brain);
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    pImpl->visualizer->visualizeActivity(brain);
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // TODO: Create spike raster visualization
    pImpl->visualizer->visualizeSpikes(spikes);
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    pImpl->visualizer->visualizeWeights(brain);
}

} // namespace nlm
