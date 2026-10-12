#include "NeuralVisualizer.hpp"
#include <iostream>

namespace nlm {

struct NeuralVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    float spikeThreshold;
    bool showMembranePotential;
    bool showFiringRate;
    std::string colorScheme;
    double updateInterval;
    std::vector<NeuronId> visibleNeurons;
    std::vector<float> neuronActivity;
    std::vector<float> membranePotentials;
    std::vector<float> firingRates;
    std::vector<float> spikeTimes;
    
    Impl() : visible(false), width(800), height(600), spikeThreshold(0.0f), 
             showMembranePotential(true), showFiringRate(true), 
             colorScheme("thermal"), updateInterval(100.0), 
             neuronActivity(), membranePotentials(), firingRates(), spikeTimes() {}
};

NeuralVisualizer::NeuralVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool NeuralVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    pImpl->neuronActivity.clear();
    pImpl->membranePotentials.clear();
    pImpl->firingRates.clear();
    pImpl->spikeTimes.clear();
    return true;
}

void NeuralVisualizer::update() {
    if (!pImpl->brain || !pImpl->visible) return;
    
    // Collect neural activity data from brain
    pImpl->neuronActivity.clear();
    pImpl->membranePotentials.clear();
    pImpl->firingRates.clear();
    pImpl->spikeTimes.clear();
    
    if (pImpl->brain) {
        auto spikeSystem = pImpl->brain->getSpikeSystem();
        if (spikeSystem) {
            // In a real implementation, we would get actual neuron data
            // For now, create sample data
            size_t numNeurons = 100;
            pImpl->neuronActivity.resize(numNeurons);
            pImpl->membranePotentials.resize(numNeurons);
            pImpl->firingRates.resize(numNeurons);
            pImpl->spikeTimes.resize(numNeurons);
            
            for (size_t i = 0; i < numNeurons; ++i) {
                pImpl->neuronActivity[i] = (static_cast<float>(rand()) / RAND_MAX) * 0.5f;
                pImpl->membranePotentials[i] = -65.0f + (static_cast<float>(rand()) / RAND_MAX) * 30.0f;
                pImpl->firingRates[i] = (static_cast<float>(rand()) / RAND_MAX) * 50.0f;
                pImpl->spikeTimes[i] = (static_cast<float>(rand()) / RAND_MAX) * 100.0f;
            }
        }
    }
}

void NeuralVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering Neural Activity Visualization:" << std::endl;
    std::cout << "  Neurons visible: " << pImpl->visibleNeurons.size() << std::endl;
    std::cout << "  Membrane potential range: " << 
                 (pImpl->membranePotentials.empty() ? 0.0f : *std::min_element(pImpl->membranePotentials.begin(), pImpl->membranePotentials.end())) << " - " <<
                 (pImpl->membranePotentials.empty() ? 0.0f : *std::max_element(pImpl->membranePotentials.begin(), pImpl->membranePotentials.end())) << " mV" << std::endl;
    std::cout << "  Firing rate range: " << 
                 (pImpl->firingRates.empty() ? 0.0f : *std::min_element(pImpl->firingRates.begin(), pImpl->firingRates.end())) << " - " <<
                 (pImpl->firingRates.empty() ? 0.0f : *std::max_element(pImpl->firingRates.begin(), pImpl->firingRates.end())) << " Hz" << std::endl;
    
    // In a real implementation, this would render to a graphics API (OpenGL, Vulkan, etc.)
    // For now, just print the data
}

void NeuralVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool NeuralVisualizer::isVisible() const {
    return pImpl->visible;
}

void NeuralVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void NeuralVisualizer::setSpikeThreshold(float threshold) {
    pImpl->spikeThreshold = threshold;
}

void NeuralVisualizer::setShowMembranePotential(bool show) {
    pImpl->showMembranePotential = show;
}

void NeuralVisualizer::setShowFiringRate(bool show) {
    pImpl->showFiringRate = show;
}

void NeuralVisualizer::setColorScheme(const std::string& scheme) {
    pImpl->colorScheme = scheme;
}

void NeuralVisualizer::setUpdateInterval(double interval) {
    pImpl->updateInterval = interval;
}

const std::vector<NeuronId>& NeuralVisualizer::getVisibleNeurons() const {
    return pImpl->visibleNeurons;
}

void NeuralVisualizer::setVisibleNeurons(const std::vector<NeuronId>& neurons) {
    pImpl->visibleNeurons = neurons;
}

std::vector<float> NeuralVisualizer::getNeuronActivity() const {
    return pImpl->neuronActivity;
}

std::vector<float> NeuralVisualizer::getMembranePotentials() const {
    return pImpl->membranePotentials;
}

std::vector<float> NeuralVisualizer::getFiringRates() const {
    return pImpl->firingRates;
}

std::vector<float> NeuralVisualizer::getSpikeTimes() const {
    return pImpl->spikeTimes;
}

} // namespace nlm