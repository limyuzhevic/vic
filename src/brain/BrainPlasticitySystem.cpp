// Brain Plasticity System implementation
#include "BrainPlasticitySystem.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

BrainPlasticitySystem::BrainPlasticitySystem(Brain* brain) : brain(brain), learningRate(0.01f), stronglyConnectedSynapses(0) {
    stdp = std::make_unique<STDP>();
    hebbian = std::make_unique<Hebbian>();
    structuralPlasticity = std::make_unique<StructuralPlasticity>();
    developmentSystem = std::make_unique<DevelopmentSystem>();
}

void BrainPlasticitySystem::applyPlasticity(float plasticityMod, const std::vector<Neuron*>& neurons) {
    // Apply STDP with neuromodulation
    applySTDP(plasticityMod, neurons);
    
    // Apply Hebbian learning
    applyHebbian(neurons);
    
    // Update development system
    updateDevelopment(brain->getCurrentStep());
    
    // Adjust plasticity for development
    adjustPlasticityForDevelopment(brain->getDevelopmentalStage());
}

void BrainPlasticitySystem::applySTDP(float plasticityMod, const std::vector<Neuron*>& neurons) {
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    stdp->update(syn, preSpikes, postSpikes, brain->getTimestep());
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                    
                    if (std::abs(weight) > 0.5f) {
                        ++stronglyConnectedSynapses;
                    }
                }
            }
        }
    }
}

void BrainPlasticitySystem::applyHebbian(const std::vector<Neuron*>& neurons) {
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    hebbian->update(syn, preSpikes, postSpikes, brain->getTimestep());
                }
            }
        }
    }
}

void BrainPlasticitySystem::updateStructuralPlasticity(SimulationStep currentStep, std::unique_ptr<RandomGenerator>& rng) {
    if (currentStep % 100 == 0) {
        structuralPlasticity->update(brain, *rng);
    }
}

void BrainPlasticitySystem::updateDevelopment(SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {
        developmentSystem->update(brain, *brain->getRandomGenerator(), 1000.0);
    }
}

void BrainPlasticitySystem::adjustPlasticityForDevelopment(DevelopmentalStage stage) {
    float plasticityMod = 1.0f;
    
    switch (stage) {
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
    
    learningRate = 0.01f * plasticityMod;
    
    structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
    structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

} // namespace nlm