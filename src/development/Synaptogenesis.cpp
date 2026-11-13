#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <random>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;           // Base synapse formation probability
    float criticalPeriodMultiplier; // Multiplier during critical period
    float targetDensity;            // Target synapse density
    float activityThreshold;       // Minimum activity to form synapses
    
    // Activity tracking
    std::unordered_map<NeuronId, float> neuronActivity;
    std::unordered_map<NeuronId, SimulationStep> lastConnectionStep;
    
    Impl() : formationRate(0.002f), criticalPeriodMultiplier(2.0f), 
             targetDensity(0.15f), activityThreshold(0.5f) {}
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
    pImpl->targetDensity = std::clamp(density, 0.05f, 0.5f);
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    const auto& developmentSystem = brain->getDevelopmentSystem();
    
    // Calculate formation rate based on developmental stage
    float formationRate = calculateFormationRate(developmentSystem);
    
    // Get current synapse density
    float currentDensity = calculateCurrentDensity(brain);
    
    // Adjust formation rate based on target density
    if (currentDensity < pImpl->targetDensity) {
        formationRate *= 1.5f;  // Form more synapses if below target
    } else if (currentDensity > pImpl->targetDensity * 1.2f) {
        formationRate *= 0.5f;  // Form fewer synapses if above target
    }
    
    // Perform synaptogenesis based on activity and development
    performSynaptogenesis(brain, rng, formationRate);
}

float Synaptogenesis::calculateFormationRate(const DevelopmentSystem* developmentSystem) {
    float rate = pImpl->formationRate;
    
    if (developmentSystem) {
        if (developmentSystem->isCriticalPeriod()) {
            rate *= pImpl->criticalPeriodMultiplier;
        } else if (developmentSystem->getStage() == DevelopmentalStage::Maturation) {
            rate *= 0.6f;
        } else if (developmentSystem->getStage() == DevelopmentalStage::Adult) {
            rate *= 0.2f;
        }
    }
    
    return rate;
}

float Synaptogenesis::calculateCurrentDensity(Brain* brain) {
    if (!brain) return 0.0f;
    
    auto neurons = brain->getAllNeurons();
    if (neurons.empty()) return 0.0f;
    
    // Count existing synapses
    size_t synapseCount = 0;
    for (const auto& neuron : neurons) {
        synapseCount += neuron->getOutgoingSynapses().size();
    }
    
    // Maximum possible synapses (complete connectivity)
    size_t maxPossible = neurons.size() * neurons.size();
    
    return (maxPossible > 0) ? static_cast<float>(synapseCount) / maxPossible : 0.0f;
}

void Synaptogenesis::performSynaptogenesis(Brain* brain, RandomGenerator& rng, float formationRate) {
    if (formationRate <= 0.0f) return;
    
    auto neurons = brain->getAllNeurons();
    if (neurons.size() < 2) return;
    
    // Determine number of formation attempts
    size_t attempts = static_cast<size_t>(formationRate * 2000.0f);
    
    for (size_t i = 0; i < attempts; ++i) {
        // Select pre- and postsynaptic neurons
        Neuron* preNeuron = selectNeuron(neurons, rng);
        Neuron* postNeuron = selectNeuron(neurons, rng);
        
        // Ensure different neurons and no self-connections
        if (preNeuron == postNeuron) continue;
        
        // Check if synapse already exists
        if (synapseExists(preNeuron, postNeuron)) continue;
        
        // Check if neurons have had recent connections
        if (hasRecentConnection(preNeuron, postNeuron)) continue;
        
        // Create synapse if all conditions met
        createSynapticConnection(preNeuron, postNeuron, rng);
    }
}

Neuron* Synaptogenesis::selectNeuron(const std117 = neurons, RandomGenerator& rng) {
    if (neurons.empty()) return nullptr;
    
    size_t index = rng.uniformInt(0, static_cast<int>(neurons.size() - 1);
    return neurons[index].get();
}

bool Synaptogenesis::synapseExists(Neuron* preNeuron, Neuron* postNeuron) {
    if (!preNeuron || !postNeuron) return false;
    
    for (const auto& handle : preNeuron->getOutgoingSynapses()) {
        auto synapse = postNeuron->findIncomingSynapse(handle);
        if (synapse) return true;
    }
    return false;
}

bool Synaptogenesis::hasRecentConnection(Neuron* preNeuron, Neuron* postNeuron) {
    if (!preNeuron || !postNeuron) return false;
    
    // Check if neurons have been connected recently
    SimulationStep currentStep = 1000; // Placeholder for current simulation step
    
    auto& preStep = pImpl->lastConnectionStep[preNeuron->getId()];
    auto& postStep = pImpl->lastConnectionStep[postNeuron->getId()];
    
    // Allow formation if no recent connections or if connections are old
    bool preHasRecent = (preStep > 0 && (currentStep - preStep) < 100);
    bool postHasRecent = (postStep > 0 && (currentStep - postStep) < 100);
    
    return (preHasRecent || postHasRecent);
}

void Synaptogenesis::createSynapticConnection(Neuron* preNeuron, Neuron* postNeuron, RandomGenerator& rng) {
    if (!preNeuron || !postNeuron) return;
    
    // Create synapse with biologically plausible parameters
    SynapseHandle handle = SynapseHandle(
        static_cast<uint64_t>(rng.uniformInt(1000, 9999)));
    
    // Determine synapse type based on developmental stage
    SynapseType synapseType = getDevelopmentallyAppropriateType(rng);
    
    // Create synapse (placeholder for actual implementation)
    // This would normally create a synapse object and add it to both neurons
}

SynapseType Synaptogenesis::getDevelopmentallyAppropriateType(RandomGenerator& rng) {
    // Critical period: more diverse connections
    // Mature brain: typically excitatory
    float typeChoice = rng.uniformReal(0.0f, 1.0f);
    
    if (typeChoice < 0.7f) {
        return SynapseType::Excitatory;
    } else if (typeChoice < 0.9f) {
        return SynapseType::Inhibitory;
    } else {
        return SynapseType::Modulatory;
    }
}

} // namespace nlm
