#include "PlasticitySystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

struct PlasticitySystem::Impl {
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    float currentPlasticityMod_;
    
    Impl() : brain_(nullptr), currentPlasticityMod_(1.0f) {}
};

PlasticitySystem::PlasticitySystem() : pImpl(std::make_unique<Impl>()) {}
PlasticitySystem::~PlasticitySystem() = default;

void PlasticitySystem::initialize(Brain* brain, std::shared_ptr<Config> config) {
    pImpl->brain_ = brain;
    pImpl->config_ = config;
}

void PlasticitySystem::update(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                             STDP* stdp, Hebbian* hebbian,
                             float plasticityMod, TimestepDuration timestep,
                             Timestamp currentTime) {
    pImpl->currentPlasticityMod_ = plasticityMod;
    
    // Apply plasticity rules to all synapses
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    stdp->update(syn.get(), preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    hebbian->update(syn.get(), preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void PlasticitySystem::applyDevelopmentEffects(StructuralPlasticity* structuralPlasticity,
                                               DevelopmentalStage developmentalStage) {
    if (structuralPlasticity) {
        float plasticityMod = 1.0f;
        
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }
        
        structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
        structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void PlasticitySystem::handleStructuralPlasticity(Brain* brain, RandomGenerator& rng,
                                                 SimulationStep currentStep) {
    if (currentStep % 100 == 0 && brain && brain->getStructuralPlasticity()) {
        brain->getStructuralPlasticity()->update(brain, rng);
    }
}

void PlasticitySystem::reset() {
    pImpl->currentPlasticityMod_ = 1.0f;
}

} // namespace nlm
