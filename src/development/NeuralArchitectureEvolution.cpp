#include "NeuralArchitectureEvolution.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct NeuralArchitectureEvolution::Impl {
    std::vector<NeuralRegionSpecialization> specializations;
    double lastTopologyUpdate;
    float regionalDifferentiation;
    
    Impl() : lastTopologyUpdate(0.0), regionalDifferentiation(0.0f) {}
};

NeuralArchitectureEvolution::NeuralArchitectureEvolution() : pImpl(new Impl) {}

NeuralArchitectureEvolution::~NeuralArchitectureEvolution() = default;

void NeuralArchitectureEvolution::update(Brain* brain, RandomGenerator& rng, double age, double dt) {
    // Create regions if none exist
    if (brain->getRegionCount() == 0) {
        createRegions(brain, rng, 3);
    }
    
    // Specialize regions based on development
    specializeRegions(brain, age);
    
    // Update topology evolution
    evolveTopology(brain, rng, age);
    
    // Update myelination
    myelinateConnections(brain, rng, age);
}

void NeuralArchitectureEvolution::createRegions(Brain* brain, RandomGenerator& rng, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        std::string regionName = "Region_" + std::to_string(i + 1);
        RegionId id = brain->addRegion(regionName);
        
        NeuralRegionSpecialization spec(id);
        pImpl->specializations.push_back(spec);
    }
}

void NeuralArchitectureEvolution::specializeRegions(Brain* brain, double age) {
    pImpl->specializations.clear();
    
    for (size_t i = 0; i < brain->getRegionCount(); ++i) {
        RegionId id(i + 1);
        NeuralRegionSpecialization spec(id);
        
        // Age-dependent specialization
        double ageFraction = std::min(1.0, age / 960.0);  // 960 simulation seconds = full development
        
        // Different regions specialize differently
        switch (i % 3) {
            case 0: // Sensory region
                spec.sensoryProcessing = std::min(1.0f, 0.3f + ageFraction * 0.7f);
                spec.motorControl = ageFraction * 0.2f;
                spec.internalRegulation = ageFraction * 0.3f;
                break;
            case 1: // Motor region
                spec.motorControl = std::min(1.0f, 0.3f + ageFraction * 0.7f);
                spec.sensoryProcessing = ageFraction * 0.2f;
                spec.internalRegulation = ageFraction * 0.3f;
                break;
            case 2: // Internal region
                spec.internalRegulation = std::min(1.0f, 0.3f + ageFraction * 0.7f);
                spec.sensoryProcessing = ageFraction * 0.3f;
                spec.motorControl = ageFraction * 0.2f;
                break;
        }
        
        // Interconnectivity increases with age
        spec.interconnectivity = std::min(1.0f, ageFraction * 0.8f + 0.2f);
        
        // Myelinization increases with age
        spec.myelinationLevel = std::min(1.0f, ageFraction * 0.9f);
        
        pImpl->specializations.push_back(spec);
    }
}

void NeuralArchitectureEvolution::myelinateConnections(Brain* brain, RandomGenerator& rng, double age) {
    double ageFraction = std::min(1.0, age / 960.0);
    float myelinationRate = 0.01f * ageFraction;  // Increases with development
    
    // In a full implementation, this would actually add myelin sheaths to synapses
    // For now, we update the specialization profile
    for (auto& spec : pImpl->specializations) {
        spec.myelinationLevel = std::min(1.0f, spec.myelinationLevel + myelinationRate * 0.1f);
    }
}

void NeuralArchitectureEvolution::evolveTopology(Brain* brain, RandomGenerator& rng, double age) {
    // Simple topology evolution - in a full implementation, this would
    // be more complex, including pruning, rewiring, and new synapse formation
    double ageFraction = std::min(1.0, age / 960.0);
    
    // Regional differentiation increases with age
    pImpl->regionalDifferentiation = std::min(1.0f, ageFraction * 0.8f + 0.2f);
}

std::vector<NeuralRegionSpecialization> NeuralArchitectureEvolution::getSpecializationProfile() const {
    return pImpl->specializations;
}

float NeuralArchitectureEvolution::getRegionalSpecialization(RegionId regionId) const {
    for (const auto& spec : pImpl->specializations) {
        if (spec.regionId == regionId) {
            return spec.sensoryProcessing + spec.motorControl + spec.internalRegulation;
        }
    }
    return 0.0f;
}

} // namespace nlm