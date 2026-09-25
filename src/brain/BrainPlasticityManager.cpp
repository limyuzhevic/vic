#include "BrainPlasticityManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

void BrainPlasticityManager::initialize(Brain* brain) {
    auto* pImpl = brain->pImpl.get();
    auto* config = brain->getConfig().get();
    
    if (!pImpl || !config) return;
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    
    if (pImpl->stdp) {
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Configure structural plasticity
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(pruningRate);
    }
}

void BrainPlasticityManager::applyPlasticity(Brain* brain, Timestamp currentTime, TimestepDuration timestep) {
    auto* pImpl = brain->pImpl.get();
    
    if (!pImpl) return;
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainPlasticityManager::updateSynapseStates(Brain* brain, Timestamp currentTime) {
    auto* pImpl = brain->pImpl.get();
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            syn->step(currentTime);
        }
    }
}

void BrainPlasticityManager::applyStructuralPlasticity(Brain* brain, SimulationStep currentStep) {
    auto* pImpl = brain->pImpl.get();
    
    if (currentStep % 100 == 0 && pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(brain, *pImpl->rng);
    }
}

void BrainPlasticityManager::collectPlasticityStats(Brain* brain) const {
    // Plasticity statistics will be collected in logStatus()
}

} // namespace nlm
