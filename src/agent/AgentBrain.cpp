#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/SleepConsolidationSystem.hpp"
#include "../memory/PersistentMemorySystem.hpp"
#include "../memory/ReplaySystem.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , attentionStrength_(0.5f)
    , acetylcholineLevel_(0.0f)
    , norepinephrineLevel_(0.0f)
    , memoryAttentionalBias_(0.0f)
    , sleepState_(SleepConsolidationSystem::SleepConsolidationSystem::SleepState::State::Awake)
    , sleepPressure_(0.0f)
    , lastSleepTime_(0.0)
    , spontaneousReplayEnabled_(true)
    , sleepConsolidationActive_(true)
    , homeostaticBalance_(1.0f)
{
    // Initialize motor and sensory neuron groups
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize sleep state
    sleepState_ = SleepConsolidationSystem::SleepConsolidationSystem::SleepState::State::Awake;
    sleepPressure_ = 0.0f;
    lastSleepTime_ = 0.0;
    spontaneousReplayEnabled_ = true;
    sleepConsolidationActive_ = true;
    homeostaticBalance_ = 1.0f;
    
    // Initialize attentional modulation
    attentionStrength_ = 0.5f;
    acetylcholineLevel_ = 0.0f;
    norepinephrineLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    attentionFocus_ = 0.0f;
    memoryAttentionalBias_ = 0.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            // Apply attentional modulation
            float attentionalGain = 1.0f + attentionStrength_ * acetylcholineLevel_ * 0.5f;
            current *= attentionalGain;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            // Apply attentional modulation
            float attentionalGain = 1.0f + attentionStrength_ * norepinephrineLevel_ * 0.3f;
            current *= attentionalGain;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            // Apply attentional modulation for internal processing
            float attentionalGain = 1.0f + attentionStrength_ * acetylcholineLevel_ * 0.3f;
            current *= attentionalGain;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            // Apply attentional modulation
            float attentionalGain = 1.0f + attentionStrength_ * norepinephrineLevel_ * 0.2f;
            current *= attentionalGain;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update sleep state and pressure
    updateSleepState();
    
    // Update sleep state and pressure based on neuromodulatory signals and time
void AgentBrain::updateSleepState() {
    if (!brain_ || !brain_->getSleepConsolidationSystem()) {
        return;
    }
    
    // Get current sleep state from the sleep consolidation system
    const auto& sleepSystem = brain_->getSleepConsolidationSystem();
    
    // Update sleep pressure based on time awake and novelty
    if (sleepState_ == SleepConsolidationSystem::SleepState::State::Awake) {
        sleepPressure_ += 1.0f * (1.0f - serotoninLevel_) * 0.1f;  // Higher pressure without serotonin
        sleepPressure_ += noveltyLevel_ * 0.05f;  // Novelty increases sleep pressure
        sleepPressure_ = std::clamp(sleepPressure_, 0.0f, 1.0f);
    } else {
        // Reduce sleep pressure during sleep
        sleepPressure_ *= 0.9f;
        sleepPressure_ = std::max(0.0f, sleepPressure_ - 0.2f * homeostaticBalance_);
    }
    
    // Update sleep state based on sleep pressure and neuromodulators
    float sleepThreshold = 0.6f + (1.0f - acetylcholineLevel_) * 0.3f - 
                          norepinephrineLevel_ * 0.2f + 
                          sleepPressure_ * 0.4f;
    
    if (sleepState_ == SleepConsolidationSystem::SleepState::State::Awake && sleepPressure_ > sleepThreshold) {
        // Enter sleep state
        sleepState_ = SleepConsolidationSystem::SleepState::State::LightSleep;
        lastSleepTime_ = developmentalAge_;
        
        // Enable sleep consolidation
        if (sleepConsolidationActive_) {
            brain_->getSleepConsolidationSystem()->enterSleep(SleepConsolidationSystem::SleepConsolidationSystem::SleepState::State::LightSleep, 
                                                            acetylcholineLevel_ * 0.5f);
        }
        
        NLM_LOG_INFO("Entering light sleep state due to high sleep pressure");
    } else if (sleepState_ != SleepConsolidationSystem::SleepState::State::Awake) {
        // Check if we should wake up
        float wakeThreshold = 0.2f + serotoninLevel_ * 0.4f - 
                             norepinephrineLevel_ * 0.1f;
        
        if (sleepPressure_ < wakeThreshold) {
            // Wake up
            brain_->getSleepConsolidationSystem()->exitSleep();
            sleepState_ = SleepConsolidationSystem::SleepState::State::Awake;
            
            NLM_LOG_INFO("Woke up from sleep");
        }
    }
    
    // Update homeostatic balance based on sleep quality
    if (sleepState_ != SleepConsolidationSystem::SleepState::State::Awake) {
        homeostaticBalance_ = (homeostaticBalance_ * 0.8f) + 0.2f;
    } else {
        homeostaticBalance_ = (homeostaticBalance_ * 0.9f) + (1.0f - sleepPressure_) * 0.1f;
    }
    homeostaticBalance_ = std::clamp(homeostaticBalance_, 0.5f, 1.5f);
    
    // Apply sleep-related neuromodulatory effects
    // Enhanced acetylcholine during sleep for memory consolidation
    if (sleepState_ != SleepConsolidationSystem::SleepState::State::Awake) {
        acetylcholineLevel_ = std::clamp(acetylcholineLevel_ + 0.01f, 0.2f, 0.8f);
    } else {
        // Lower acetylcholine during wake
        acetylcholineLevel_ = std::clamp(acetylcholineLevel_ - 0.01f, 0.1f, 0.5f);
    }
    
    // Update spontaneous replay based on sleep state
    if (spontaneousReplayEnabled_ && sleepState_ != SleepConsolidationSystem::SleepState::State::Awake) {
        brain_->getReplaySystem()->setReplayStatistics(
            {{ "spontaneous_replay_enabled", 1.0f },
             { "sleep_state", static_cast<float>(sleepState_) },
             { "homeostatic_balance", homeostaticBalance_ }});
    }
}
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

float AgentBrain::getAttentionStrength() const {
    return attentionStrength_;
}

float AgentBrain::getAcetylcholineLevel() const {
    return acetylcholineLevel_;
}

float AgentBrain::getNorepinephrineLevel() const {
    return norepinephrineLevel_;
}

float AgentBrain::getAttentionFocus() const {
    return attentionFocus_;
}

void AgentBrain::updateAttentionalFocus(const std::vector<float>& vision,
                                      const std::vector<float>& touch,
                                      const std::vector<float>& internal,
                                      const std::vector<float>& proprioception) {
    // Compute salience based on sensory input variance
    float visionSalience = 0.0f;
    if (!vision.empty()) {
        float visionMean = 0.0f;
        for (float v : vision) visionMean += v;
        visionMean /= vision.size();
        
        float visionVariance = 0.0f;
        for (float v : vision) {
            float diff = v - visionMean;
            visionVariance += diff * diff;
        }
        visionSalience = std::sqrt(visionVariance / vision.size()) * 2.0f;
    }
    
    float touchSalience = 0.0f;
    if (!touch.empty()) {
        for (float t : touch) {
            if (t > 0.5f) touchSalience += 1.0f;
        }
        touchSalience = std::min(touchSalience, 2.0f);
    }
    
    float internalSalience = 0.0f;
    if (!internal.empty()) {
        for (float i : internal) {
            internalSalience += std::abs(i);
        }
        internalSalience = std::min(internalSalience, 2.0f);
    }
    
    // Combine saliences and apply neuromodulatory gains
    float combinedSalience = visionSalience * 0.4f + touchSalience * 0.3f + 
                             internalSalience * 0.3f;
    
    // Apply cholinergic enhancement
    combinedSalience *= (1.0f + acetylcholineLevel_ * 1.5f);
    
    // Apply noradrenergic arousal modulation
    combinedSalience *= (1.0f + norepinephrineLevel_ * 0.5f);
    
    // Smooth update to attention focus
    attentionFocus_ = 0.9f * attentionFocus_ + 0.1f * std::clamp(combinedSalience, 0.0f, 1.0f);
    
    // Update attention strength based on neuromodulatory state
    attentionStrength_ = 0.3f + 0.7f * acetylcholineLevel_ + 0.2f * norepinephrineLevel_;
    attentionStrength_ = std::clamp(attentionStrength_, 0.1f, 1.0f);
    
    // Update neuromodulator levels based on attentional state
    acetylcholineLevel_ = std::clamp(acetylcholineLevel_ + attentionFocus_ * 0.05f, 0.0f, 1.0f);
    norepinephrineLevel_ = std::clamp(norepinephrineLevel_ + attentionFocus_ * 0.03f, 0.0f, 1.0f);
}

void AgentBrain::feedAttentionToMemorySystems() {
    if (!brain_) return;
    
    // Apply attentional bias to working memory
    memoryAttentionalBias_ = attentionFocus_ * 0.8f;
    
    // Get working memory system
    auto* workingMemory = brain_->getWorkingMemory();
    if (workingMemory) {
        // Strengthen memory traces that are currently attended
        workingMemory->strengthenMemory(1.0f + memoryAttentionalBias_);
        
        // Apply attentional competition modulation
        workingMemory->runCompetition();
    }
    
    // Apply attentional focus to episodic memory encoding
    auto* episodicMemory = brain_->getEpisodicMemory();
    if (episodicMemory) {
        // Enhanced encoding based on attention
        if (attentionFocus_ > 0.7f) {
            // Boost encoding efficiency
            episodicMemory->boostEncodingEfficiency(1.2f + attentionFocus_ * 0.5f);
        }
    }
}

void AgentBrain::provideAttentionToConceptFormation() {
    if (!brain_) return;
    
    auto* conceptFormation = brain_->getConceptFormation();
    if (conceptFormation) {
        // Provide attentional guidance to concept formation
        // Higher attention focus increases sensitivity to patterns
        if (attentionFocus_ > 0.5f) {
            // Boost concept formation with attentional modulation
            conceptFormation->setFormationThreshold(0.5f + attentionFocus_ * 0.3f);
            
            // Enhance pattern matching based on attention
            conceptFormation->setStabilityThreshold(0.5f + attentionFocus_ * 0.4f);
        }
        
        // Feedback attentional predictions to concept updates
        if (predictionError_ > 0.0f && attentionFocus_ > 0.6f) {
            conceptFormation->updateWithAttention(predictionError_, attentionFocus_);
        }
    }
}

void AgentBrain::applyNeuromodulation(const Neuromodulator& signal) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = signal.getLevel(); // Simplified - normally would compare predicted vs actual reward
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * signal.getLevel();
    
    // Neuromodulation based on signal type
    if (signal.getName() == std::string("Acetylcholine")) {
        acetylcholineLevel_ = signal.getLevel();
    } else if (signal.getName() == std::string("Norepinephrine")) {
        norepinephrineLevel_ = signal.getLevel();
    } else if (signal.getName() == std::string("Dopamine")) {
        dopamineLevel_ = signal.getLevel();
    }
    
    // Clamp neuromodulator levels
    acetylcholineLevel_ = std::clamp(acetylcholineLevel_, 0.0f, 1.0f);
    norepinephrineLevel_ = std::clamp(norepinephrineLevel_, 0.0f, 1.0f);
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply neuromodulatory effects on neural excitability and plasticity
    applyNeuromodulatoryEffects();
    
    // Update attentional strength based on neuromodulatory state
    updateAttentionalStrength();
}

void AgentBrain::applyNeuromodulatoryEffects() {
    if (!brain_) return;
    
    // Apply acetylcholine effects on memory encoding and attention
    if (acetylcholineLevel_ > 0.3f) {
        float achGain = acetylcholineLevel_ * 2.0f;
        
        // Enhance sensory processing
        for (auto* neuron : brain_->getWorkingMemory()->getMemoryNeurons()) {
            neuron->injectCurrent(0.5f * achGain);
        }
        
        // Boost working memory capacity
        brain_->getWorkingMemory()->setCapacity(
            brain_->getWorkingMemory()->getCapacity() * (1.0f + acetylcholineLevel_ * 0.5f)
        );
    }
    
    // Apply norepinephrine effects on arousal and vigilance
    if (norepinephrineLevel_ > 0.3f) {
        float neGain = norepinephrineLevel_ * 1.5f;
        
        // Enhance signal-to-noise ratio
        attentionStrength_ *= (1.0f + norepinephrineLevel_ * 0.8f);
        
        // Increase firing thresholds
        attentionStrength_ = std::clamp(attentionStrength_, 0.1f, 1.0f);
    }
    
    // Apply dopamine effects on reward prediction and motivation
    if (dopamineLevel_ > 0.0f) {
        float dopamineGain = std::max(0.0f, dopamineLevel_) * 2.0f;
        
        // Enhance concept formation for rewarding stimuli
        auto* conceptFormation = brain_->getConceptFormation();
        if (conceptFormation) {
            // Modify concept formation based on reward prediction error
            if (predictionError_ > 0.0f) {
                conceptFormation->setFormationThreshold(
                    conceptFormation->getFormationThreshold() * (1.0f - dopamineGain * 0.2f)
                );
            }
        }
    }
}

void AgentBrain::updateAttentionalStrength() {
    // Integrate multiple neuromodulatory influences on attention
    float neuromodulatoryInfluence = 0.4f * acetylcholineLevel_ + 
                                   0.3f * norepinephrineLevel_ + 
                                   0.3f * std::max(0.0f, dopamineLevel_);
    
    attentionStrength_ = 0.2f + neuromodulatoryInfluence * 0.8f;
    attentionStrength_ = std::clamp(attentionStrength_, 0.1f, 1.0f);
    
    // Apply attention to memory systems
    if (brain_->getWorkingMemory()) {
        // Modulate working memory competition based on attentional strength
        if (attentionStrength_ > 0.7f) {
            // Enhanced competition - fewer winners, more selective
            brain_->getWorkingMemory()->setCapacity(
                brain_->getWorkingMemory()->getCapacity() * 0.8f
            );
        } else {
            // Reduced competition - more inclusive retention
            brain_->getWorkingMemory()->setCapacity(
                brain_->getWorkingMemory()->getCapacity() * 1.2f
            );
        }
    }
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset sleep state
    sleepState_ = SleepConsolidationSystem::SleepState::State::Awake;
    sleepPressure_ = 0.0f;
    lastSleepTime_ = 0.0;
    spontaneousReplayEnabled_ = true;
    sleepConsolidationActive_ = true;
    homeostaticBalance_ = 1.0f;
    
    // Reset neuromodulator levels
    acetylcholineLevel_ = 0.0f;
    norepinephrineLevel_ = 0.0f;
    serotoninLevel_ = 0.0f;
    attentionFocus_ = 0.0f;
    memoryAttentionalBias_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
