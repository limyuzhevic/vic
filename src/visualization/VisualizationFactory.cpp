#include "VisualizationFactory.hpp"
#include "BrainVisualizer.hpp"
#include "NeuralNetworkVisualizer.hpp"
#include "MemoryVisualizer.hpp"
#include "NeuromodulationVisualizer.hpp"
#include "CognitiveVisualizer.hpp"
#include "DevelopmentVisualizer.hpp"
#include "PerformanceVisualizer.hpp"
#include <map>
#include <string>
#include <vector>

namespace nlm {

std::unique_ptr<BrainVisualizer> VisualizationFactory::createNeuralNetworkVisualizer(const std::string& type) {
    return std::make_unique<NeuralNetworkVisualizer>(type);
}

std::unique_ptr<BrainVisualizer> VisualizationFactory::createMemoryVisualizer(const std::string& type) {
    return std::make_unique<MemoryVisualizer>(type);
}

std::unique_ptr<BrainVisualizer> VisualizationFactory::createNeuromodulationVisualizer(const std::string& type) {
    return std::make_unique<NeuromodulationVisualizer>(type);
}

std::unique_ptr<BrainVisualizer> VisualizationFactory::createCognitiveVisualizer(const std::string& type) {
    return std::make_unique<CognitiveVisualizer>(type);
}

std::unique_ptr<BrainVisualizer> VisualizationFactory::createDevelopmentVisualizer(const std::string& type) {
    return std::make_unique<DevelopmentVisualizer>(type);
}

std::unique_ptr<BrainVisualizer> VisualizationFactory::createPerformanceVisualizer(const std::string& type) {
    return std::make_unique<PerformanceVisualizer>(type);
}

std::unique_ptr<BrainVisualizer> VisualizationFactory::createVisualizer(const std::string& name, const std::string& type) {
    static const std::map<std::string, std::function<std::unique_ptr<BrainVisualizer>(const std::string&)>> creatorMap = {
        {"neural_network", createNeuralNetworkVisualizer},
        {"memory", createMemoryVisualizer},
        {"neuromodulation", createNeuromodulationVisualizer},
        {"cognitive", createCognitiveVisualizer},
        {"development", createDevelopmentVisualizer},
        {"performance", createPerformanceVisualizer}
    };
    
    auto it = creatorMap.find(name);
    if (it != creatorMap.end()) {
        return it->second(type);
    }
    
    return nullptr;
}

std::vector<std::string> VisualizationFactory::getAvailableVisualizers() {
    return {"neural_network", "memory", "neuromodulation", "cognitive", "development", "performance"};
}

std::vector<std::string> VisualizationFactory::getVisualizerTypes(const std::string& name) {
    // In a more complete implementation, this would read from configuration
    // For now, return default types
    return {"default", "detailed", "compact", "scientific"};
}

} // namespace nlm