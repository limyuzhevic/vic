#include "PlasticityManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

PlasticityManager::PlasticityManager(Brain* brain)
    : brain_(brain)
    , stdp(std::make_unique<STDP>())
    , hebbian(std::make_unique<Hebbian>())
    , structuralPlasticity(std::make_unique<StructuralPlasticity>()) {
}

PlasticityManager::~PlasticityManager() = default;

bool PlasticityManager::initialize() {
    NLM_LOG_INFO("Initializing Plasticity Manager...");
    
    if (!brain_) return false;
    
    // Configure STDP parameters from brain config
    float ltpWeight = brain_->pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = brain_->pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = brain_->pImpl->config->getOr<float>("stdp_tau", 20.0f);
    stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity parameters
    float synaptogenesisRate = brain_->pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = brain_->pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
    
    NLM_LOG_INFO("Plasticity Manager initialized successfully");
    return true;
}

void PlasticityManager::reset() {
    // Plasticity systems don't need reset - they maintain state across steps
}

void PlasticityManager::step(SimulationStep currentStep, Timestamp currentTime, double timestep) {
    applyPlasticityRules(currentStep, currentTime, timestep);
    applyHomeostaticScaling(currentStep);
}

void PlasticityManager::applyPlasticityRules(SimulationStep currentStep, Timestamp currentTime, double timestep) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (brain_->pImpl->dopamine) {
        plasticityMod = brain_->pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : brain_->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
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

void PlasticityManager::applyHomeostaticScaling(SimulationStep currentStep) {
    // Phase 6, tried and reverted: global inhibitory feedback
    // This is the homeostatic synaptic scaling (Phase 4 logic)
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : brain_->pImpl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
}

} // namespace nlm
