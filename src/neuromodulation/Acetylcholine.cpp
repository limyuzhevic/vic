#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityBoost;
    float memoryEnhancement;
    float attentionBoost;
    
    // Memory consolidation state
    std::vector<float> memoryTrace;
    float consolidationFactor;
    
    // Synaptic plasticity state
    float globalPlasticity;
    std::vector<float> synapseModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , plasticityBoost(1.5f)
        , memoryEnhancement(1.3f)
        , attentionBoost(1.2f)
        , consolidationFactor(0.8f)
        , globalPlasticity(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (ACh) system initialized for memory and attention");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + (pImpl->level * 0.5f * (pImpl->plasticityBoost - 1.0f));
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    float target = pImpl->baseline;
    pImpl->level += (target - pImpl->level) * 0.05f * static_cast<float>(dt);
}

// Working memory consolidation enhancement
void Acetylcholine::enhanceWorkingMemory() {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory()) {
        return;
    }
    
    auto* workingMemory = pImpl->brain->getWorkingMemory();
    float memoryModulation = pImpl->memoryEnhancement * pImpl->level;
    
    // Strengthen memory traces based on ACh level
    workingMemory->strengthenMemory(memoryModulation);
    
    // Enhance memory trace stability
    const auto& memoryNeurons = workingMemory->getMemoryNeurons();
    for (const auto& neuronId : memoryNeurons) {
        float activation = workingMemory->getNeuronActivation(neuronId);
        float enhancedActivation = activation * memoryModulation;
        workingMemory->storeToNeuron(neuronId, enhancedActivation);
    }
    
    // Add memory trace for consolidation
    std::vector<float> wmPattern = workingMemory->retrieve();
    pImpl->memoryTrace.push_back(wmPattern.empty() ? 0.0f : *std::max_element(wmPattern.begin(), wmPattern.end()));
}

// Attention mechanism enhancement
void Acetylcholine::enhanceAttention() {
    if (!pImpl->brain || !pImpl->brain->getAttention()) {
        return;
    }
    
    auto* attention = pImpl->brain->getAttention();
    float attentionModulation = pImpl->attentionBoost * pImpl->level;
    
    // Apply attentional bias to enhance focus
    attention->setExcitationStrength(1.5f * attentionModulation);
    attention->setInhibitionStrength(0.3f / attentionModulation);
    
    // If there are active neurons in working memory, focus attention on them
    if (pImpl->brain->getWorkingMemory() && pImpl->brain->getWorkingMemory()->getActiveTraces() > 0) {
        std::vector<NeuronId> memoryNeurons = pImpl->brain->getWorkingMemory()->getMemoryNeurons();
        if (!memoryNeurons.empty()) {
            for (const auto& neuronId : memoryNeurons) {
                float activation = pImpl->brain->getWorkingMemory()->getNeuronActivation(neuronId);
                if (activation > 0.5f) {
                    attention->applyBottomUpSalience(neuronId, activation * attentionModulation);
                }
            }
        }
    }
}

// Synaptic plasticity enhancement
void Acetylcholine::enhancePlasticity() {
    if (!pImpl->brain) {
        return;
    }
    
    pImpl->globalPlasticity = 1.0f + pImpl->level * 0.5f * (pImpl->plasticityBoost - 1.0f);
    
    // Modulate synaptic plasticity rules based on ACh level
    if (auto* stdp = pImpl->brain->getSTDP()) {
        float ltpWeight = 0.01f * pImpl->globalPlasticity;
        float ltdWeight = 0.012f * pImpl->globalPlasticity;
        float tau = 20.0f * (1.0f + pImpl->level * 0.5f); // Longer time constant
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Apply plasticity modulation to all synapses in the brain
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto* synapse : region->getSynapses()) {
            float currentWeight = synapse->getWeight();
            float plasticityModulation = 1.0f + pImpl->level * 0.3f * (pImpl->plasticityBoost - 1.0f);
            
            // Modulate synapse eligibility trace for future plasticity
            float eligibilityTrace = synapse->getEligibilityTrace() * plasticityModulation;
            synapse->setEligibilityTrace(eligibilityTrace);
        }
    }
}

void Acetylcholine::signalAttentionEvent(float noveltyLevel) {
    // Phasic release of ACh in response to novel or salient events
    float burstLevel = std::min(1.0f, pImpl->level + noveltyLevel * 0.5f * pImpl->releaseRate);
    setLevel(burstLevel);
    
    // Trigger enhanced processing
    enhanceWorkingMemory();
    enhanceAttention();
    enhancePlasticity();
}

void Acetylcholine::signalLearningEvent() {
    // Sustained ACh release during learning and encoding
    float learningLevel = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
    setLevel(learningLevel);
    
    // Enhance memory consolidation during learning
    enhanceWorkingMemory();
    
    // Modulate plasticity for encoding new information
    enhancePlasticity();
}

const std::vector<float>& Acetylcholine::getMemoryTrace() const {
    return pImpl->memoryTrace;
}

void Acetylcholine::clearMemoryTrace() {
    pImpl->memoryTrace.clear();
}

float Acetylcholine::getConsolidationFactor() const {
    return pImpl->consolidationFactor * pImpl->level;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionBoost * pImpl->level;
}

} // namespace nlm
