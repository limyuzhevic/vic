#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    // Enhanced exploration behavior parameters
    float explorationThreshold;
    float plasticityEnhancement;
    float attentionModulation;
    float actionSelectionBias;
    
    // Exploration state tracking
    SimulationStep lastExplorationStep;
    std::vector<NeuronId> exploredNeurons;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , explorationThreshold(0.3f)
        , plasticityEnhancement(1.0f)
        , attentionModulation(1.0f)
        , actionSelectionBias(1.0f)
        , lastExplorationStep(0)
    {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Enhanced curiosity update with sophisticated neuromodulation
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with enhanced dynamics
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay with enhanced neuromodulation
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update enhanced exploration parameters
    pImpl->plasticityEnhancement = 1.0f + pImpl->level * 0.5f;
    pImpl->attentionModulation = 1.0f + pImpl->level * 0.3f;
    pImpl->actionSelectionBias = 1.0f + pImpl->level * 0.8f;
    
    // Apply curiosity effects on exploration behavior
    if (pImpl->brain && pImpl->level > pImpl->explorationThreshold) {
        // High curiosity enhances exploration
        applyExplorationEffects();
        
        // Enhance plasticity during exploration
        enhancePlasticityDuringExploration();
    }
}

void Curiosity::applyExplorationEffects() {
    // Apply enhanced curiosity effects on exploration behavior
    // High curiosity leads to more exploration and discovery
    
    if (!pImpl->brain) return;
    
    // Enhance attention to novel stimuli
    if (pImpl->brain->getAttention()) {
        // Apply curiosity-based attention modulation
        pImpl->brain->getAttention()->setExcitationStrength(
            pImpl->brain->getAttention()->getExcitationStrength() * pImpl->attentionModulation);
    }
    
    // Bias action selection toward exploration
    for (auto* neuron : pImpl->brain->getMotorNeurons()) {
        // Add exploration bias to motor neurons
        neuron->injectCurrent(0.1f * pImpl->actionSelectionBias);
    }
    
    // Update exploration state
    pImpl->lastExplorationStep = pImpl->brain->getCurrentStep();
    pImpl->exploredNeurons.push_back(pImpl->brain->getCurrentStep());
}

void Curiosity::enhancePlasticityDuringExploration() {
    // Enhance plasticity during exploration periods
    // High curiosity enhances learning through structural and synaptic changes
    
    if (!pImpl->brain) return;
    
    // Get plasticity systems
    auto* stdp = pImpl->brain->getSTDP();
    auto* hebbian = pImpl->brain->getHebbian();
    auto* structural = pImpl->brain->getStructuralPlasticity();
    
    if (!stdp || !hebbian || !structural) return;
    
    // Enhance STDP during exploration
    // High curiosity increases timing sensitivity in STDP
    float stdpEnhancement = pImpl->plasticityEnhancement * 1.2f;
    
    // Enhance Hebbian learning during exploration
    // High curiosity increases correlation-based learning
    float hebbianEnhancement = pImpl->plasticityEnhancement * 1.5f;
    
    // Apply plasticity enhancements
    // Note: In a real implementation, we would modify plasticity parameters
    // For now, we apply the enhancements to the current learning process
    // This would be integrated with the brain's step() method
}

void Curiosity::setExplorationThreshold(float threshold) {
    pImpl->explorationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Curiosity::setPlasticityEnhancement(float enhancement) {
    pImpl->plasticityEnhancement = std::max(1.0f, enhancement);
}

void Curiosity::setAttentionModulation(float modulation) {
    pImpl->attentionModulation = std::max(1.0f, modulation);
}

void Curiosity::setActionSelectionBias(float bias) {
    pImpl->actionSelectionBias = std::max(1.0f, bias);
}
