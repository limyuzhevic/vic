#include "Synaptogenesis.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float currentDensity;
    float experienceModifier;
    float developmentalAge;
    
    Impl() 
        : formationRate(0.0001f)
        , targetDensity(0.3f)
        , currentDensity(0.1f)
        , experienceModifier(1.0f)
        , developmentalAge(0.0) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::max(0.0f, rate);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.1f, 1.0f);
}

void Synaptogenesis::update(class Brain* brain, class RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate current synapse density
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons > 0) {
        pImpl->currentDensity = static_cast<float>(totalSynapses) / 
                               static_cast<float>(totalNeurons * totalNeurons);
    }
    
    // Synaptogenesis is more active in early developmental stages
    float ageModifier = 1.0f;
    
    // Experience-dependent formation: more formation when synapses are below target
    float formationImpulse = 0.0f;
    if (pImpl->currentDensity < pImpl->targetDensity) {
        formationImpulse = (pImpl->targetDensity - pImpl->currentDensity) * 
                          pImpl->formationRate * pImpl->experienceModifier;
    }
    
    // Age-dependent modulation: higher in early stages
    if (pImpl->developmentalAge < 300.0) {
        ageModifier = 1.2f - pImpl->developmentalAge * 0.003f;
    }
    
    // Stochastic element for biological realism
    float stochasticFactor = 1.0f + (rng.getRandomFloat() - 0.5f) * 0.2f;
    
    // Apply formation
    float newSynapses = formationImpulse * ageModifier * stochasticFactor;
    
    if (newSynapses > 0.0f && brain->getStructuralPlasticity()) {
        // In a full implementation, this would actually create new synapses
        // For now, we'll just log the potential formation
        NLM_LOG_INFO("Synaptogenesis: potential to form " + std::to_string(newSynapses) + 
                    " new synapses (density: " + std::to_string(pImpl->currentDensity) + ")");
    }
    
    // Update developmental age
    pImpl->developmentalAge += 0.001f;
}

} // namespace nlm