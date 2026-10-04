#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <algorithm>

namespace nlm {

// ========== DOPAMINE IMPLEMENTATION ==========

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    std::vector<float> rewardHistory;
    size_t rewardCount;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), rewardCount(0) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced dopamine-modulated plasticity factor
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    recordReward(reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    recordReward(error > 0 ? error : 0.0f);
}

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Dopamine::applyToNeuralExcitability() {
    if (!pImpl->brain) return;
    
    // Dopamine modulates neural excitability by adjusting effective current injection
    float dopamineLevel = getLevel();
    if (dopamineLevel > 0.0f) {
        // Enhances sensory processing and motor preparation
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, dopamineLevel * 2.0f);
        pImpl->brain->injectCurrentToNeurons(NeuronType::Motor, dopamineLevel * 1.5f);
        
        // Also adds to internal neurons for integration
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, dopamineLevel * 0.5f);
    }
}

void Dopamine::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    if (!stdp) return;
    
    float factor = getPlasticityFactor();
    
    // Scale STDP learning rates based on dopamine level
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    stdp->setLTPWeight(ltpWeight * factor);
    stdp->setLTDWeight(ltdWeight * factor);
    
    // Hebbian learning is enhanced by dopamine for reward-associated patterns
    if (hebbian && factor > 0.7f) {
        // For strong dopamine signals, strengthen reward-related associations
        hebbian->setLearningRate(hebbian->getLearningRate() * factor);
    }
}

void Dopamine::applyToMemory(class NeuralEpisodicMemory* episodicMemory) {
    if (!pImpl->brain || !episodicMemory) return;
    
    // Dopamine enhances memory consolidation
    float dopamineLevel = getLevel();
    if (dopamineLevel > 0.3f) {
        // Stronger dopamine leads to more robust memory encoding
        float consolidationFactor = 0.5f + 0.5f * dopamineLevel;
        
        // Store reward in the most recent episode if available
        if (pImpl->brain->getTotalSpikeCount() > 0) {
            // Update the last stored episode with reward information
            // This would require access to the memory system to update the last episode
            // For now, we'll note that dopamine affects memory consolidation
        }
    }
}

void Dopamine::applyToAttention(class AttentionalSelection* attention) {
    if (!pImpl->brain || !attention) return;
    
    float dopamineLevel = getLevel();
    
    // Dopamine enhances attention to salient/reward-related stimuli
    if (dopamineLevel > 0.0f) {
        // Boost attention mechanisms
        attention->setExcitationStrength(attention->getExcitationStrength() * (1.0f + dopamineLevel * 0.5f));
        
        // Reduce attention to irrelevant stimuli
        attention->setInhibitionStrength(attention->getInhibitionStrength() * (1.0f - dopamineLevel * 0.2f));
    }
}

void Dopamine::applyToDevelopment(class DevelopmentSystem* development) {
    if (!development) return;
    
    float dopamineLevel = getLevel();
    
    // Dopamine affects developmental plasticity and circuit refinement
    if (dopamineLevel > 0.5f) {
        // Strong dopamine enhances pruning of unused connections
        development->setPruningRate(development->getPruningRate() * (1.0f + dopamineLevel * 0.5f));
    }
}

void Dopamine::recordReward(float reward) {
    pImpl->rewardHistory.push_back(reward);
    pImpl->rewardCount++;
    
    // Keep history manageable
    if (pImpl->rewardHistory.size() > 1000) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

float Dopamine::getAverageReward() const {
    if (pImpl->rewardHistory.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float reward : pImpl->rewardHistory) {
        sum += reward;
    }
    return sum / static_cast<float>(pImpl->rewardHistory.size());
}

    size_t Dopamine::getRewardCount() const {
        return pImpl->rewardCount;
    }

// ========== ACETYLCHOLINE IMPLEMENTATION ==========

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float noveltySensitivity;
    bool isNoveltySignal;
    float attentionModulation;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), 
              noveltySensitivity(1.0f), isNoveltySignal(false), attentionModulation(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for novel experiences
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset novelty signal
    pImpl->isNoveltySignal = false;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Acetylcholine::applyToNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.0f) {
        // ACh enhances cortical excitability and sensory processing
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, achLevel * 1.5f);
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, achLevel * 1.0f);
        
        // Modulate attention and arousal
        pImpl->attentionModulation = 1.0f + achLevel * 0.8f;
    }
}

void Acetylcholine::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    if (!stdp || !hebbian) return;
    
    float factor = getPlasticityFactor();
    
    // ACh specifically enhances STDP for novel patterns
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    // Boost LTP (learning to reinforce) for novel associations
    stdp->setLTPWeight(ltpWeight * factor * 1.2f);
    // Moderate LTD (learning to forget) to allow new learning
    stdp->setLTDWeight(ltdWeight * factor * 0.8f);
    
    // Strongly enhance Hebbian learning for attention-modulated patterns
    hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.5f);
}

void Acetylcholine::applyToMemory(class NeuralEpisodicMemory* episodicMemory) {
    if (!pImpl->brain || !episodicMemory) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.2f) {
        // ACh enhances memory encoding and consolidation of novel experiences
        float encodingFactor = 0.5f + 0.5f * achLevel;
        
        // Store attention-modulated encoding information
        if (pImpl->brain->getTotalSpikeCount() > 0) {
            // Enhanced memory encoding for salient stimuli
        }
    }
}

void Acetylcholine::applyToAttention(class AttentionalSelection* attention) {
    if (!pImpl->brain || !attention) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.0f) {
        // ACh strongly modulates attention
        attention->setExcitationStrength(attention->getExcitationStrength() * pImpl->attentionModulation);
        attention->setInhibitionStrength(attention->getInhibitionStrength() * 0.5f); // Reduce inhibition
        
        // If there's a novelty signal, boost attention further
        if (pImpl->isNoveltySignal) {
            attention->setExcitationStrength(attention->getExcitationStrength() * 1.3f);
        }
    }
}

void Acetylcholine::applyToDevelopment(class DevelopmentSystem* development) {
    if (!development) return;
    
    float achLevel = getLevel();
    if (achLevel > 0.0f) {
        // ACh promotes synaptic growth and circuit refinement during development
        development->setSynaptogenesisRate(development->getSynaptogenesisRate() * (1.0f + achLevel * 0.5f));
    }
}

void Acetylcholine::signalNovelty(float novelty) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->noveltySensitivity);
    pImpl->isNoveltySignal = true;
}

void Acetylcholine::enhanceAttentionDuringNovelty(bool isNovel) {
    pImpl->isNoveltySignal = isNovel;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

// ========== NORADRENALINE IMPLEMENTATION ==========

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceModulation;
    bool isThreatSignal;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.03f), peak(1.0f), decayRate(0.08f), 
              arousalLevel(1.0f), vigilanceModulation(1.0f), isThreatSignal(false) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and threat
    return 0.4f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset threat signal
    pImpl->isThreatSignal = false;
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Norepinephrine::applyToNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.0f) {
        // NE increases overall arousal and neural excitability
        pImpl->arousalLevel = 1.0f + neLevel * 1.2f;
        pImpl->vigilanceModulation = 1.0f + neLevel * 0.5f;
        
        // Strong NE enhances internal neuron activity (arousal)
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, neLevel * 2.0f);
        
        // Moderate NE enhances sensory processing
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, neLevel * 1.2f);
    }
}

void Norepinephrine::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    if (!stdp || !hebbian) return;
    
    float factor = getPlasticityFactor();
    
    // NE enhances plasticity during high arousal states
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    // Boost learning during arousal
    stdp->setLTPWeight(ltpWeight * factor * 1.1f);
    stdp->setLTDWeight(ltdWeight * factor * 0.9f);
    
    // Hebbian learning is enhanced by NE for salient/important events
    if (pImpl->isThreatSignal) {
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.3f);
    } else {
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.1f);
    }
}

void Norepinephrine::applyToMemory(class NeuralEpisodicMemory* episodicMemory) {
    if (!pImpl->brain || !episodicMemory) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.1f) {
        // NE enhances memory encoding during high arousal states
        float encodingFactor = 0.3f + 0.7f * neLevel;
        
        // Stronger encoding for emotionally salient events
        if (pImpl->isThreatSignal) {
            encodingFactor *= 1.5f;
        }
    }
}

void Norepinephrine::applyToAttention(class AttentionalSelection* attention) {
    if (!pImpl->brain || !attention) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.0f) {
        // NE enhances attention and alertness
        attention->setExcitationStrength(attention->getExcitationStrength() * pImpl->vigilanceModulation);
        attention->setInhibitionStrength(attention->getInhibitionStrength() * 0.7f); // Reduce inhibition
        
        // If threat signal, boost attention dramatically
        if (pImpl->isThreatSignal) {
            attention->setExcitationStrength(attention->getExcitationStrength() * 1.5f);
        }
    }
}

void Norepinephrine::applyToDevelopment(class DevelopmentSystem* development) {
    if (!development) return;
    
    float neLevel = getLevel();
    if (neLevel > 0.0f) {
        // NE affects stress-related development and circuit pruning
        development->setPruningRate(development->getPruningRate() * (1.0f + neLevel * 0.3f));
    }
}

void Norepinephrine::signalStress(float stress) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * 1.0f);
    pImpl->isThreatSignal = true;
}

void Norepinephrine::enhanceVigilance(bool isThreat) {
    pImpl->isThreatSignal = isThreat;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceModulation() const {
    return pImpl->vigilanceModulation;
}

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float socialModulation;
    float behavioralControl;
    bool isSocialFeedback;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.06f),
              socialModulation(1.0f), behavioralControl(1.0f), isSocialFeedback(false) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and behavioral state
    return 0.2f + 0.8f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reset social feedback signal
    pImpl->isSocialFeedback = false;
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void Serotonin::applyToNeuralExcitability() {
    if (!pImpl->brain) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.0f) {
        // Serotonin modulates mood-related neural excitability
        // Generally reduces impulsivity and stabilizes firing
        pImpl->behavioralControl = 1.0f + serotoninLevel * 0.5f;
        
        // Moderate effect on internal neurons for mood regulation
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, serotoninLevel * 0.5f);
        
        // Reduce excessive excitability in sensory neurons to prevent overstimulation
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, -serotoninLevel * 0.3f);
    }
}

void Serotonin::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    if (!stdp || !hebbian) return;
    
    float factor = getPlasticityFactor();
    
    // Serotonin modulates plasticity based on behavioral state
    float ltpWeight = stdp->getLTPWeight();
    float ltdWeight = stdp->getLTDWeight();
    
    // Higher serotonin stabilizes plasticity (prevents runaway learning)
    stdp->setLTPWeight(ltpWeight * (0.8f + 0.2f * factor));
    stdp->setLTDWeight(ltdWeight * (0.9f - 0.1f * factor));
    
    // Hebbian learning is modulated by serotonin
    if (pImpl->isSocialFeedback) {
        // Social feedback enhances appropriate learning
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 0.9f);
    } else {
        // Baseline learning rate
        hebbian->setLearningRate(hebbian->getLearningRate() * factor * 1.0f);
    }
}

void Serotonin::applyToMemory(class NeuralEpisodicMemory* episodicMemory) {
    if (!pImpl->brain || !episodicMemory) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.1f) {
        // Serotonin enhances memory consolidation and regulatory functions
        float consolidationFactor = 0.5f + 0.5f * serotoninLevel;
        
        // Better regulation of memory retrieval and inhibition
        if (pImpl->isSocialFeedback) {
            consolidationFactor *= 1.2f;  // Enhanced social memory processing
        }
    }
}

void Serotonin::applyToAttention(class AttentionalSelection* attention) {
    if (!pImpl->brain || !attention) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.0f) {
        // Serotonin modulates attention and impulse control
        attention->setExcitationStrength(attention->getExcitationStrength() * pImpl->behavioralControl);
        attention->setInhibitionStrength(attention->getInhibitionStrength() * (1.0f + serotoninLevel * 0.3f));
        
        // Enhanced behavioral control
        if (pImpl->isSocialFeedback) {
            attention->setExcitationStrength(attention->getExcitationStrength() * 0.9f); // More selective attention
        }
    }
}

void Serotonin::applyToDevelopment(class DevelopmentSystem* development) {
    if (!development) return;
    
    float serotoninLevel = getLevel();
    if (serotoninLevel > 0.0f) {
        // Serotonin affects behavioral development and social circuit refinement
        development->setSynaptogenesisRate(development->getSynaptogenesisRate() * (1.0f - serotoninLevel * 0.2f));
        development->setPruningRate(development->getPruningRate() * (1.0f + serotoninLevel * 0.3f));
    }
}

void Serotonin::signalSocialFeedback(float socialFeedback) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialFeedback * 0.5f);
    pImpl->isSocialFeedback = true;
}

void Serotonin::modulateImpulsivity(float impulsivity) {
    // Reduces impulsivity based on serotonin level
    if (impulsivity > 0.5f) {
        float reduction = pImpl->level * 0.3f;
        // This would affect neural firing patterns in the brain
    }
}

void Serotonin::regulateMood(float mood) {
    // Modulates mood-related neural circuits
    if (mood < 0.0f) {  // Negative mood
        pImpl->level = std::max(pImpl->peak * 0.3f, pImpl->level - 0.1f);
    } else {  // Positive mood
        pImpl->level = std::min(pImpl->peak, pImpl->level + mood * 0.1f);
    }
}

float Serotonin::getSocialModulation() const {
    return pImpl->socialModulation;
}

float Serotonin::getBehavioralControl() const {
    return pImpl->behavioralControl;
}

} // namespace nlm