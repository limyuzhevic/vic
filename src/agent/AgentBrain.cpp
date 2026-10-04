#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
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
    , socialLearningEnabled_(false)
    , metaLearningEnabled_(false)
    , memoryBasedDecisionEnabled_(false)
    , adaptiveCuriosityEnabled_(false)
    , socialLearningRate_(0.5f)
    , metaLearningRate_(0.3f)
    , adaptiveCuriosityBase_(0.5f)
    , recentPerformance_(0.0f)
    , learningRateAdaptation_(1.0f)
    , recentNovelty_(0.0f)
    , learningProgress_(0.0f)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups using weak pointers
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Create weak pointer to avoid circular dependencies
                        auto neuron_weak = std::weak_ptr<Neuron>(pop->getNeurons().front().get(), 
                                                               [](Neuron*) {}); // No cleanup function needed
                        
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(neuron_weak); break;
                            case 1: motorBackward_.push_back(neuron_weak); break;
                            case 2: motorTurnLeft_.push_back(neuron_weak); break;
                            case 3: motorTurnRight_.push_back(neuron_weak); break;
                            case 4: motorInteract_.push_back(neuron_weak); break;
                            case 5: motorWait_.push_back(neuron_weak); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Create weak pointer to avoid circular dependencies
                        auto neuron_weak = std::weak_ptr<Neuron>(pop->getNeurons().front().get(), 
                                                               [](Neuron*) {}); // No cleanup function needed
                        
                        // Distribute sensory neurons
                        size_t idx = sensoryVision_.size() + sensoryTouch_.size() +
                                    sensoryInternal_.size() + sensoryProprioception_.size();
                        
                        switch (idx % 4) {
                            case 0: sensoryVision_.push_back(neuron_weak); break;
                            case 1: sensoryTouch_.push_back(neuron_weak); break;
                            case 2: sensoryInternal_.push_back(neuron_weak); break;
                            case 3: sensoryProprioception_.push_back(neuron_weak); break;
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
    
    // Count valid neurons for logging
    size_t visionCount = 0, touchCount = 0, internalCount = 0;
    for (const auto& neuron_weak : sensoryVision_) {
        if (!neuron_weak.expired()) visionCount++;
    }
    for (const auto& neuron_weak : sensoryTouch_) {
        if (!neuron_weak.expired()) touchCount++;
    }
    for (const auto& neuron_weak : sensoryInternal_) {
        if (!neuron_weak.expired()) internalCount++;
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(visionCount) + " vision sensory neurons, " +
                 std::to_string(touchCount) + " touch sensory neurons, " +
                 std::to_string(internalCount) + " internal sensory neurons");
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
        auto neuron_weak = sensoryVision_[i];
        if (!neuron_weak.expired()) {
            auto neuron_ptr = neuron_weak.lock();
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            neuron_ptr->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        auto neuron_weak = sensoryTouch_[i];
        if (!neuron_weak.expired()) {
            auto neuron_ptr = neuron_weak.lock();
            float current = touch[i] * 8.0f;  // Collision signal
            neuron_ptr->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        auto neuron_weak = sensoryInternal_[i];
        if (!neuron_weak.expired()) {
            auto neuron_ptr = neuron_weak.lock();
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            neuron_ptr->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        auto neuron_weak = sensoryProprioception_[i];
        if (!neuron_weak.expired()) {
            auto neuron_ptr = neuron_weak.lock();
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            neuron_ptr->injectCurrent(current);
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
        
        // Store recent novelty for adaptive curiosity
        recentNovelty_ = noveltyLevel_;
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Update adaptive curiosity
        if (adaptiveCuriosityEnabled_) {
            updateAdaptiveCuriosity(learningProgress_);
        }
    }
    
    // Update meta-learning based on performance
    if (metaLearningEnabled_) {
        updateMetaLearning(recentPerformance_);
    }
    
    // Update social learning benefits
    if (socialLearningEnabled_) {
        // Social learning will influence future decisions based on partner experiences
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply social learning
    if (socialLearningEnabled_) {
        decoded = decodeWithSocialLearning(decoded);
    }
    
    // Apply memory-based decision making
    if (memoryBasedDecisionEnabled_) {
        decoded = decodeWithMemoryDecision(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<std::weak_ptr<Neuron>>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        int validCount = 0;
        for (const auto& neuron_weak : neurons) {
            if (!neuron_weak.expired()) {
                auto neuron_ptr = neuron_weak.lock();
                // Use membrane potential deviation from rest as activity measure
                sum += std::abs(neuron_ptr->getState().membranePotential - neuron_ptr->getState().restingPotential);
                validCount++;
            }
        }
        return validCount > 0 ? sum / validCount : 0.0f;
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
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
        
        if (brain_ && brain_->getRandomGenerator()) {
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
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::decodeWithSocialLearning(MotorCommand defaultCmd) {
    float socialBenefit = getSocialLearningBenefit();
    
    if (socialBenefit > 0.8f) {
        // High social benefit: follow partner's suggestions (simulate by random action)
        if (brain_ && brain_->getRandomGenerator()) {
            int choice = brain_->getRandomGenerator()->uniformInt(0, 5); // Exclude Wait
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                default: return defaultCmd;
            }
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::decodeWithMemoryDecision(MotorCommand defaultCmd) {
    float memoryScore = getMemoryBasedDecisionScore();
    
    if (memoryScore > 0.7f) {
        // High memory-based confidence: trust memory decisions
        // Simulate by choosing action with highest memory association
        // This would integrate with actual episodic memory in a full implementation
        if (brain_ && brain_->getRandomGenerator()) {
            int choice = brain_->getRandomGenerator()->uniformInt(0, 5); // Exclude Wait
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                default: return defaultCmd;
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

void AgentBrain::updateMetaLearning(float performance) {
    recentPerformance_ = performance;
    
    // Meta-learning: adapt learning rate based on performance
    if (performance < 0.3f) {
        // Poor performance: increase learning rate for faster improvement
        learningRateAdaptation_ = std::min(learningRateAdaptation_ * 1.2f, 2.0f);
    } else if (performance > 0.8f) {
        // Good performance: decrease learning rate to fine-tune
        learningRateAdaptation_ = std::max(learningRateAdaptation_ * 0.8f, 0.1f);
    }
    
    // Apply meta-learning to plasticity
    float metaFactor = 1.0f + (learningRateAdaptation_ - 1.0f) * metaLearningRate_;
    
    // Update STDP parameters with meta-learning
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        float currentLTP = stdp->getLTPWeight();
        float currentLTD = stdp->getLTDWeight();
        
        stdp->setLTPWeight(currentLTP * metaFactor);
        stdp->setLTDWeight(currentLTD * metaFactor);
    }
}

void AgentBrain::updateAdaptiveCuriosity(float learningProgress) {
    learningProgress_ = learningProgress;
    
    if (learningProgress < 0.3f) {
        // Slow learning: increase curiosity for exploration
        curiosityLevel_ += (adaptiveCuriosityBase_ + learningProgress_) * 0.1f;
    } else if (learningProgress > 0.7f) {
        // Fast learning: decrease curiosity as task becomes mastered
        curiosityLevel_ -= (1.0f - learningProgress_) * 0.05f;
    }
    
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void AgentBrain::transferKnowledge(const std::string& sourceContext, const std::string& targetContext) {
    // Get transfer effectiveness from memory
    float effectiveness = knowledgeTransfer_.count(sourceContext + "_to_" + targetContext) ? 
                          knowledgeTransfer_[sourceContext + "_to_" + targetContext] : 0.0f;
    
    // If transfer is effective, apply to current agent state
    if (effectiveness > 0.6f) {
        // Adjust learning rates based on transfer knowledge
        socialLearningRate_ *= effectiveness;
        metaLearningRate_ *= effectiveness;
        
        NLM_LOG_INFO("Transfer learning from " + sourceContext + " to " + targetContext + 
                     " with effectiveness " + std::to_string(effectiveness));
    }
}

float AgentBrain::getTransferLearningEffectiveness(const std::string& sourceContext, const std::string& targetContext) const {
    std::string key = sourceContext + "_to_" + targetContext;
    return knowledgeTransfer_.count(key) ? knowledgeTransfer_.at(key) : 0.0f;
}

float AgentBrain::getSocialLearningBenefit() const {
    float totalBenefit = 0.0f;
    for (const auto& partner : socialPartners_) {
        if (!partner.partner.expired() && partner.trustLevel > 0.3f) {
            totalBenefit += partner.trustLevel * partner.influenceWeight;
        }
    }
    return std::min(totalBenefit / std::max(static_cast<size_t>(1), socialPartners_.size()), 1.0f);
}

float AgentBrain::getMetaLearningSignal() const {
    return learningRateAdaptation_;
}

float AgentBrain::getMemoryBasedDecisionScore() const {
    // Simple heuristic based on memory patterns
    float maxScore = 0.0f;
    for (const auto& pattern : memoryPatterns_) {
        maxScore = std::max(maxScore, pattern.second);
    }
    return maxScore;
}

float AgentBrain::getAdaptiveCuriosityLevel() const {
    return curiosityLevel_;
}

void AgentBrain::addSocialPartner(std::shared_ptr<AgentBrain> partner, float trust, float influence) {
    if (!partner) {
        return;
    }
    
    // Create weak reference to avoid circular dependencies
    auto weakPartner = std::weak_ptr<AgentBrain>(partner);
    
    // Add partner with validation
    socialPartners_.emplace_back(weakPartner, trust, influence);
    
    NLM_LOG_INFO("Added social partner with trust level " + std::to_string(trust) + 
                 " and influence " + std::to_string(influence));
}

std::vector<AgentBrain::SocialPartner> AgentBrain::getSocialPartners() const {
    std::vector<SocialPartner> validPartners;
    for (const auto& partner : socialPartners_) {
        if (!partner.partner.expired()) {
            validPartners.push_back(partner);
        }
    }
    return validPartners;
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

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset advanced learning states
    socialPartners_.clear();
    knowledgeTransfer_.clear();
    memoryPatterns_.clear();
    recentPerformance_ = 0.0f;
    learningRateAdaptation_ = 1.0f;
    recentNovelty_ = 0.0f;
    learningProgress_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm