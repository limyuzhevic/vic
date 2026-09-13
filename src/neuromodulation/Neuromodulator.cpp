namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 6 Complete implementation

struct Neuromodulator::Impl {
    virtual ~Impl() = default;
};

Neuromodulator::Neuromodulator() : pImpl(std::make_unique<Impl>()) {}

Neuromodulator::~Neuromodulator() = default;

const char* Neuromodulator::getName() const {
    return "NM";
}

float Neuromodulator::getLevel() const {
    return 0.0f;
}

void Neuromodulator::setLevel(float level) {
    // Base implementation
}

float Neuromodulator::getPlasticityFactor() const {
    return 1.0f;
}

void Neuromodulator::update(TimestepDuration dt) {
    // Base implementation
}

// Dopamine: Reward and reinforcement learning signal
// Phase 6 Complete implementation

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f) {}
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
    // Dopamine modulates reward prediction error and learning
    // Higher dopamine = stronger plasticity for rewarded actions
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline (tonic dopamine)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward prediction
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    
    // Also signal prediction error
    if (brain_) {
        // Boost learning for rewarded actions
        for (auto& region : brain_->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp) {
                    // Reward-dependent plasticity
                    float change = syn->getWeight() * 0.002f * pImpl->level;
                    syn->addToWeight(change);
                }
            }
        }
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine signals prediction error (reward prediction error theory)
    // Positive error: better than expected, negative error: worse than expected
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

// Acetylcholine: Attention and memory consolidation
// Phase 6 Complete implementation

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityBoost;
    float attentionModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , plasticityBoost(2.0f)
        , attentionModulation(1.5f) {}
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
    // ACh enhances synaptic plasticity for memory consolidation
    return 1.0f + pImpl->level * (pImpl->plasticityBoost - 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline (tonic ACh)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Phasic ACh signals for attentional focus and learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
    
    if (brain_) {
        // Enhance attention to selected neurons
        // Store attention bias for attention system
    }
}

void Acetylcholine::signalNovelty(float novelty) {
    // ACh signals novel events for memory encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.5f);
    
    // Boost memory encoding for novel events
    if (brain_) {
        // Enhance working memory encoding for novel patterns
        if (brain_->getWorkingMemory()) {
            brain_->getWorkingMemory()->strengthenMemory(1.0f + novelty * 0.5f);
        }
    }
}

void Acetylcholine::signalRewardPredictionError(float error) {
    // ACh signals prediction errors for learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.3f);
}

void Acetylcholine::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

void Acetylcholine::setAttentionModulation(float mod) {
    pImpl->attentionModulation = mod;
}

// Norepinephrine: Arousal and vigilance
// Phase 6 Complete implementation

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalBoost;
    float responseGain;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(1.0f)
        , arousalBoost(2.0f)
        , responseGain(1.8f) {}
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
    // NE modulates arousal and baseline excitability
    return 1.0f + pImpl->level * (pImpl->responseGain - 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline (tonic NE)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalSignal) {
    // Phasic NE signals for increased arousal and vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalSignal * pImpl->releaseRate);
    
    if (brain_) {
        // Increase overall neural excitability (arousal)
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE increases response gain and reduces threshold
                    float current = neuron->getInjectedCurrent();
                    neuron->injectCurrent(current * pImpl->arousalBoost);
                }
            }
        }
    }
}

void Norepinephrine::signalNovelty(float novelty) {
    // NE responds to novel stimuli and increases alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.7f);
    
    // Enhance arousal for novel events
    if (brain_) {
        // Increased vigilance for novel stimuli
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE increases baseline excitability
                    neuron->injectCurrent(0.1f);
                }
            }
        }
    }
}

void Norepinephrine::signalThreat(float threatLevel) {
    // NE responds to threatening or salient events
    pImpl->level = std::min(pImpl->peak, pImpl->level + threatLevel * pImpl->releaseRate * 0.5f);
    
    // Enhance defensive responses
    if (brain_) {
        // Increase vigilance and reaction speed
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getNeuronType() == NeuronType::Inhibitory) {
                        // Enhance inhibitory control for threat response
                        neuron->injectCurrent(0.2f);
                    }
                }
            }
        }
    }
}

void Norepinephrine::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Norepinephrine::getArousalBoost() const {
    return pImpl->arousalBoost;
}

void Norepinephrine::setArousalBoost(float boost) {
    pImpl->arousalBoost = boost;
}

// Serotonin: Mood, impulsivity, and social behavior
// Phase 6 Complete implementation

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodModulation;
    float inhibitionStrength;
    float rewardResponse;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.0f)
        , moodModulation(1.0f)
        , inhibitionStrength(1.5f)
        , rewardResponse(0.8f) {}
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
    // Serotonin modulates mood and behavioral inhibition
    // High serotonin = more stable learning, less impulsive
    return 1.0f - pImpl->level * 0.5f;  // Inhibits plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline (tonic 5-HT)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodSignal) {
    // Phasic 5-HT signals for mood changes
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal * pImpl->releaseRate);
    
    if (brain_) {
        // Modulate behavioral inhibition based on mood
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // 5-HT increases behavioral inhibition and impulse control
                    if (moodSignal > 0.0f) {
                        // Positive mood = stronger inhibition
                        neuron->injectCurrent(-0.1f);
                    }
                }
            }
        }
    }
}

void Serotonin::signalInhibition(float inhibitionSignal) {
    // 5-HT increases behavioral inhibition and impulse control
    pImpl->level = std::min(pImpl->peak, pImpl->level + inhibitionSignal * pImpl->releaseRate);
    
    // Enhance inhibitory control
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Increase inhibitory neuron activity
                    if (neuron->getNeuronType() == NeuronType::Inhibitory) {
                        neuron->injectCurrent(inhibitionSignal * 0.5f);
                    }
                }
            }
        }
    }
}

void Serotonin::signalSocialFeedback(float socialFeedback) {
    // 5-HT responds to social feedback and modulates social learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialFeedback * pImpl->releaseRate * 0.6f);
    
    // Enhance social learning mechanisms
    if (brain_) {
        // Boost social learning and observation
        if (brain_->getPlanner()) {
            // Enhance social learning system
            // In real implementation, would modulate social learning directly
        }
    }
}

void Serotonin::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Serotonin::getInhibitionStrength() const {
    return pImpl->inhibitionStrength;
}

void Serotonin::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength = strength;
}

// PredictionError neuromodulator
// Phase 6 Complete implementation

struct PredictionError::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , learningBoost(3.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PE";
}

float PredictionError::getLevel() const {
    return pImpl->level;
}

void PredictionError::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error strongly modulates learning rate
    return 1.0f + pImpl->level * (pImpl->learningBoost - 1.0f);
}

void PredictionError::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void PredictionError::signalPredictionError(float error) {
    // Prediction error signal for learning
    pImpl->level = std::max(0.0f, pImpl->level + std::abs(error) * pImpl->releaseRate);
    
    if (brain_) {
        // Apply prediction error to synapses
        for (auto& region : brain_->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp) {
                    // Stronger prediction error = more learning
                    float learningMod = 1.0f + pImpl->level * 2.0f;
                    
                    // Update synapse weight with prediction error modulation
                    float currentWeight = syn->getWeight();
                    float change = currentWeight * (learningMod - 1.0f) * 0.001f;
                    syn->addToWeight(change);
                }
            }
        }
    }
}

void PredictionError::signalBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float PredictionError::getLearningBoost() const {
    return pImpl->learningBoost;
}

void PredictionError::setLearningBoost(float boost) {
    pImpl->learningBoost = boost;
}

// Curiosity neuromodulator
// Phase 6 Complete implementation

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float explorationBoost;
    float rewardModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , explorationBoost(2.0f)
        , rewardModulation(1.2f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity modulates exploration and reward modulation
    return 1.0f + pImpl->level * (pImpl->explorationBoost - 1.0f);
}

void Curiosity::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Curiosity::signalNovelty(float novelty) {
    // Phasic curiosity response to novel stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate);
    
    if (brain_) {
        // Enhance exploration behavior
        for (auto* neuron : brain_->getMotorNeurons()) {
            if (pImpl->level > 0.3f) {
                // Boost exploratory actions
                neuron->injectCurrent(pImpl->level * 0.5f);
            }
        }
    }
}

void Curiosity::signalUncertainty(float uncertainty) {
    // Increased uncertainty drives curiosity
    pImpl->level = std::min(pImpl->peak, pImpl->level + uncertainty * pImpl->releaseRate * 0.8f);
    
    // Enhance exploration when uncertain
    if (brain_) {
        // Increase motor activity for exploration
        for (auto* neuron : brain_->getMotorNeurons()) {
            neuron->injectCurrent(uncertainty * 0.3f);
        }
    }
}

void Curiosity::signalReward(float reward) {
    // Reward modulates curiosity
    if (reward > 0.0f) {
        // Positive reward increases exploration
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.5f);
    }
}

void Curiosity::signalBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Curiosity::getExplorationBoost() const {
    return pImpl->explorationBoost;
}

void Curiosity::setExplorationBoost(float boost) {
    pImpl->explorationBoost = boost;
}

// Novelty neuromodulator
// Phase 6 Complete implementation

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningBoost;
    float attentionBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , learningBoost(2.5f)
        , attentionBoost(1.3f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty strongly modulates learning and attention
    return 1.0f + pImpl->level * (pImpl->learningBoost - 1.0f);
}

void Novelty::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::signalNovelty(float noveltySignal) {
    // Strong novelty signal
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltySignal * pImpl->releaseRate * 2.0f);
    
    if (brain_) {
        // Enhance learning for novel patterns
        if (brain_->getWorkingMemory()) {
            brain_->getWorkingMemory()->strengthenMemory(pImpl->learningBoost);
        }
        
        // Boost attention for novel stimuli
        if (brain_->getAttention()) {
            // Store attention bias
        }
    }
}

void Novelty::signalThreat(float threat) {
    // Novelty also responds to threatening events
    pImpl->level = std::min(pImpl->peak, pImpl->level + threat * pImpl->releaseRate * 0.7f);
}

void Novelty::signalSocialFeedback(float socialFeedback) {
    // Novelty responds to social feedback
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialFeedback * pImpl->releaseRate * 0.4f);
}

void Novelty::signalBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Novelty::getLearningBoost() const {
    return pImpl->learningBoost;
}

void Novelty::setLearningBoost(float boost) {
    pImpl->learningBoost = boost;
}

void Novelty::setAttentionBoost(float boost) {
    pImpl->attentionBoost = boost;
}

} // namespace nlm


