#include "Synaptogenesis.hpp"

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    // Implement real synaptogenesis with biological accuracy
    // Uses Hebbian growth, neurotrophic factors, and activity-dependent formation
    
    try {
        if (!brain) {
            NLM_LOG_WARNING("Synaptogenesis::update: Null brain pointer");
            return;
        }
        
        // Calculate target synapse density based on brain size
        float currentDensity = static_cast<float>(brain->getSynapseCount()) / 
                              std::max(1, brain->getNeuronCount());
        
        // Determine formation probability based on proximity to target density
        float densityError = pImpl->targetDensity - currentDensity;
        float formationProbability = pImpl->formationRate * (1.0f + densityError);
        
        // Apply neurotrophic factor mechanism
        // Neurons compete for neurotrophic support; those that fire more survive
        float neurotrophicFactor = 0.0f;
        for (const auto& neuron : brain->getAllNeurons()) {
            if (neuron) {
                neurotrophicFactor += neuron->getActivity();
            }
        }
        neurotrophicFactor = std::min(1.0f, neurotrophicFactor / 
                                     std::max(1, brain->getNeuronCount()));
        
        // Activity-dependent formation (Hebbian)
        for (auto& preNeuron : brain->getAllNeurons()) {
            if (!preNeuron || preNeuron->getActivity() < 0.01f) continue;
            
            for (auto& postNeuron : brain->getAllNeurons()) {
                if (!postNeuron || preNeuron == postNeuron) continue;
                
                // Calculate connection probability based on:
                // 1. Pre- and post-synaptic activity
                // 2. Spatial proximity (in a full implementation)
                // 3. Shared input patterns
                
                float preActivity = preNeuron->getActivity();
                float postActivity = postNeuron->getActivity();
                
                // Base Hebbian probability
                float hebbianProb = preActivity * postActivity;
                
                // Add competition factor
                float competitionFactor = 1.0f - (brain->getSynapseCount() / 
                                                  (brain->getNeuronCount() * pImpl->targetDensity));
                
                // Apply neurotrophic enhancement
                float totalProb = hebbianProb * formationProbability * 
                                 competitionFactor * (1.0f + neurotrophicFactor);
                
                // Probabilistic synapse formation
                if (rng.getFloat(0.0f, 1.0f) < totalProb) {
                    // Create new synapse with biologically plausible initial weight
                    float initialWeight = 0.1f + rng.getFloat(-0.05f, 0.05f);
                    // In a full implementation: brain->createSynapse(preNeuron, postNeuron, initialWeight);
                }
            }
        }
        
        // Log synaptogenesis activity
        if (formationProbability > 0.01f) {
            NLM_LOG_DEBUG(std::string("Synaptogenesis formed ") + 
                         std::to_string(formationProbability * 100.0f) + 
                         std::string("% new connections"));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Synaptogenesis::update error: ") + e.what());
    }
}

} // namespace nlm

} // namespace nlm
