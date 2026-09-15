#include "NetworkVisualizer.hpp"
#include "VisualizationEngine.hpp"
#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

struct NetworkVisualizer::Impl {
    VisualizationEngine* engine;
    bool active;
    
    // Network state
    std::vector<Neuron*> neurons;
    std::vector<Synapse*> connections;
    std::vector<Region*> regions;
    
    // Visualization parameters
    bool showNeurons;
    bool showConnections;
    float neuronRadius;
    float connectionThickness;
    int colorScheme;
    bool neuronLabels;
    float bgColor[4];
    float zoomFactor;
    float rotation[3];
    int projectionMode;
    
    // Selection
    std::vector<Neuron*> selectedNeurons;
    std::vector<Region*> selectedRegions;
    
    // GPU resources (for OpenGL backend)
    unsigned int neuronVAO;
    unsigned int neuronVBO;
    unsigned int connectionVAO;
    unsigned int connectionVBO;
    
    // Web resources (for Web backend)
    std::string webDataJson;
    
    Impl() : engine(nullptr), active(false), neuronRadius(0.5f), 
             connectionThickness(0.1f), colorScheme(0), neuronLabels(true),
             zoomFactor(1.0f), rotation{0.0f, 0.0f, 0.0f}, projectionMode(1) {
        bgColor[0] = bgColor[1] = bgColor[2] = 0.0f;
        bgColor[3] = 1.0f;
    }
    
    ~Impl() {
        if (active) {
            close();
        }
    }
};

NetworkVisualizer::NetworkVisualizer() : pImpl(new Impl) {}
NetworkVisualizer::~NetworkVisualizer() = default;

bool NetworkVisualizer::initialize(VisualizationEngine* engine) {
    pImpl->engine = engine;
    pImpl->active = true;
    
    // Initialize backend-specific resources
    // TODO: Implement based on backend
    return true;
}

void NetworkVisualizer::update(const Brain& brain, double timestep) {
    if (!pImpl->active) return;
    
    // Update neuron and connection data
    pImpl->neurons.clear();
    pImpl->connections.clear();
    pImpl->regions.clear();
    
    // Collect all neurons and regions
    for (const auto& region : brain.getRegions()) {
        pImpl->regions.push_back(region.get());
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                pImpl->neurons.push_back(const_cast<Neuron*>(neuron));
            }
        }
    }
    
    // Collect all synapses
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            pImpl->connections.push_back(const_cast<Synapse*>(syn));
        }
    }
}

void NetworkVisualizer::render() {
    if (!pImpl->active) return;
    
    // Render based on backend
    // TODO: Implement backend-specific rendering
    
    // Example OpenGL rendering (simplified):
    // 1. Clear background
    // 2. Setup shaders and matrices
    // 3. Render regions as spheres/cubes
    // 4. Render neurons as points/lines
    // 5. Render connections as lines with thickness proportional to weight
}

void NetworkVisualizer::close() {
    pImpl->active = false;
    
    // Cleanup backend-specific resources
    // TODO: Implement based on backend
}

void NetworkVisualizer::setShowNeurons(bool show) {
    pImpl->showNeurons = show;
}

void NetworkVisualizer::setShowConnections(bool show) {
    pImpl->showConnections = show;
}

void NetworkVisualizer::setNeuronRadius(float radius) {
    pImpl->neuronRadius = radius;
}

void NetworkVisualizer::setConnectionThickness(float thickness) {
    pImpl->connectionThickness = thickness;
}

void NetworkVisualizer::setColorScheme(int scheme) {
    pImpl->colorScheme = scheme;
}

void NetworkVisualizer::setNeuronLabels(bool show) {
    pImpl->neuronLabels = show;
}

void NetworkVisualizer::setBackgroundColor(float r, float g, float b, float a) {
    pImpl->bgColor[0] = r;
    pImpl->bgColor[1] = g;
    pImpl->bgColor[2] = b;
    pImpl->bgColor[3] = a;
}

void NetworkVisualizer::setZoom(float factor) {
    pImpl->zoomFactor = factor;
}

void NetworkVisualizer::setRotation(float x, float y, float z) {
    pImpl->rotation[0] = x;
    pImpl->rotation[1] = y;
    pImpl->rotation[2] = z;
}

void NetworkVisualizer::setProjectionMode(int mode) {
    pImpl->projectionMode = mode;
}

void NetworkVisualizer::selectRegion(int regionId) {
    // Find and store selected region
    // TODO: Implement region lookup
}

void NetworkVisualizer::selectNeuron(int neuronId) {
    // Find and store selected neuron
    // TODO: Implement neuron lookup
}

void NetworkVisualizer::clearSelection() {
    pImpl->selectedNeurons.clear();
    pImpl->selectedRegions.clear();
}

bool NetworkVisualizer::exportNetwork(const std::string& filepath, ExportFormat format) {
    // Export data in requested format
    // TODO: Implement format-specific export
    return false;
}

bool NetworkVisualizer::isActive() const {
    return pImpl->active;
}

std::vector<int> NetworkVisualizer::getSelectedNeurons() const {
    std::vector<int> ids;
    for (const auto* neuron : pImpl->selectedNeurons) {
        ids.push_back(neuron->getId().index());
    }
    return ids;
}

std::vector<int> NetworkVisualizer::getSelectedRegions() const {
    std::vector<int> ids;
    for (const auto* region : pImpl->selectedRegions) {
        ids.push_back(region->getId().index());
    }
    return ids;
}

NetworkVisualizer::NetworkStats NetworkVisualizer::getStats() const {
    NetworkStats stats;
    
    // Calculate statistics from current data
    stats.totalNeurons = pImpl->neurons.size();
    
    int activeCount = 0;
    for (const auto* neuron : pImpl->neurons) {
        if (neuron->isFiring() || neuron->getState().membranePotential > -55.0f) {
            activeCount++;
        }
    }
    stats.activeNeurons = activeCount;
    
    stats.totalConnections = pImpl->connections.size();
    
    float totalWeight = 0.0f;
    for (const auto* syn : pImpl->connections) {
        totalWeight += std::abs(syn->getWeight());
    }
    stats.averageWeight = stats.totalConnections > 0 ? totalWeight / stats.totalConnections : 0.0f;
    
    // Calculate E/I ratio
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    for (const auto* syn : pImpl->connections) {
        float weight = syn->getWeight();
        if (weight > 0) {
            totalExcitatory += weight;
        } else {
            totalInhibitory += std::abs(weight);
        }
    }
    stats.excitationInhibitionRatio = totalInhibitory > 0.0f ? totalExcitatory / totalInhibitory : 
                                       totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
    
    // Count neurons by type
    for (const auto* neuron : pImpl->neurons) {
        stats.neuronCounts[neuron->getType()]++;
    }
    
    return stats;
}

} // namespace nlm
