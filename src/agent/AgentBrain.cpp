#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))  // Use move semantics for efficiency
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
{
    NLM_LOG_INFO("Initializing AgentBrain...");
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        // Validate brain structure
        if (brain_->getRegions().empty()) {
            NLM_LOG_WARN("Brain has no regions - AgentBrain will have empty neuron groups");
        }
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) continue;  // Safety check
                        
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
                        if (!n) continue;  // Safety check
                        
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
        
        NLM_LOG_INFO("AgentBrain initialized with " +
                     std::to_string(motorForward_.size()) + " forward, " +
                     std::to_string(motorBackward_.size()) + " backward, " +
                     std::to_string(motorTurnLeft_.size()) + " left, " +
                     std::to_string(motorTurnRight_.size()) + " right, " +
                     std::to_string(motorInteract_.size()) + " interact, " +
                     std::to_string(motorWait_.size()) + " wait motor neurons, " +
                     std::to_string(sensoryVision_.size()) + " vision, " +
                     std::to_string(sensoryTouch_.size()) + " touch, " +
                     std::to_string(sensoryInternal_.size()) + " internal, " +
                     std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain pointer is null");
        return;
    }
    
    if (!world.getWorld() || world.getWorld()->getWidth() == 0 || world.getWorld()->getHeight() == 0) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: invalid world parameters");
        return;
    }
    
    // Validate expected input/output sizes match world capabilities
    size_t expectedSensorySize = getSensoryInputSize();
    if (expectedSensorySize > 0) {
        NLM_LOG_INFO("AgentBrain expects sensory input size: " + std::to_string(expectedSensorySize));
    }
    
    size_t expectedMotorSize = getMotorOutputSize();
    if (expectedMotorSize > 0) {
        NLM_LOG_INFO("AgentBrain expects motor output size: " + std::to_string(expectedMotorSize));
    }
    
    // Resize previousVision_ to match world vision dimensions
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryProprioception_.size()) + " proprioception sensory neurons");
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
    if (!brain_) {
        NLM_LOG_WARN("Cannot process sensory input: brain pointer is null");
        return;
    }
    
    // Store previous vision for novelty detection
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        previousVision_ = vision;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    if (!sensoryVision_.empty() && !vision.empty()) {
        const size_t minSize = std::min(sensoryVision_.size(), vision.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryVision_[i]) {
                // Enhanced current injection with saturation
                float current = vision[i] * 5.0f;
                current = std::clamp(current, -100.0f, 100.0f);  // Prevent overflow
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    if (!sensoryTouch_.empty() && !percept.getTouch().empty()) {
        const auto& touch = percept.getTouch();
        const size_t minSize = std::min(sensoryTouch_.size(), touch.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;
                current = std::clamp(current, -50.0f, 50.0f);
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    if (!sensoryInternal_.empty() && !percept.getInternal().empty()) {
        const auto& intern = percept.getInternal();
        const size_t minSize = std::min(sensoryInternal_.size(), intern.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
                current = std::clamp(current, -25.0f, 25.0f);
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    if (!sensoryProprioception_.empty() && !percept.getProprioception().empty()) {
        const auto& proprio = percept.getProprioception();
        const size_t minSize = std::min(sensoryProprioception_.size(), proprio.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
                current = std::clamp(current, -20.0f, 20.0f);
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision)
    float totalDiff = 0.0f;
    if (!vision.empty() && !previousVision_.empty()) {
        const size_t minVisionSize = std::min(vision.size(), previousVision_.size());
        for (size_t i = 0; i < minVisionSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Enhanced novelty calculation with logarithmic scaling
        float normalization = std::max<float>(static_cast<float>(minVisionSize), 1.0f);
        noveltyLevel_ = std::log(1.0f + totalDiff / normalization) * 10.0f;  // Scale to reasonable range
        
        // Decay and update
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Clamp to reasonable range
        noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
    }
    
    // Update curiosity with weighted combination of novelty and prediction error
    if (curiosityEnabled_) {
        float noveltyWeight = 0.7f;
        float predictionWeight = 0.3f;
        curiosityLevel_ = noveltyWeight * noveltyLevel_ + predictionWeight * std::abs(predictionError_);
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using enhanced metrics
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        size_t validNeurons = 0;
        
        for (Neuron* n : neurons) {
            if (!n) continue;  // Safety check
            
            const auto& state = n->getState();
            
            // Enhanced activity calculation using multiple metrics
            float membraneDev = std::abs(state.membranePotential - state.restingPotential);
            float firingRateFactor = state.firingRate / 100.0f;  // Normalize firing rate
            float adaptationFactor = std::min(1.0f, state.adaptationVariable / 10.0f);
            
            // Combine metrics with weighted importance
            float activity = membraneDev * 0.6f + firingRateFactor * 0.3f + adaptationFactor * 0.1f;
            totalActivity += activity;
            validNeurons++;
        }
        
        if (validNeurons == 0) return 0.0f;
        return totalActivity / validNeurons;
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Enhanced motor selection with confidence thresholding
    struct MotorCommandWithConfidence {
        MotorCommand cmd;
        float activity;
        float confidence;  // Activity relative to best
    };
    
    std::vector<MotorCommandWithConfidence> commands;
    
    // Calculate confidence (how much better this is than the next best)
    std::vector<float> activities = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    std::sort(activities.begin(), activities.end(), std::greater<float>());
    
    float maxActivity = activities[0];
    float secondMax = activities.size() > 1 ? activities[1] : 0.0f;
    
    // Build command list with confidence
    MotorCommand cmdList[] = {MotorCommand::MoveForward, MotorCommand::MoveBackward, 
                               MotorCommand::TurnLeft, MotorCommand::TurnRight, 
                               MotorCommand::Interact, MotorCommand::Wait};
    
    float commandActs[] = {forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct};
    
    for (size_t i = 0; i < 6; ++i) {
        float confidence = (maxActivity - commandActs[i]) / (maxActivity + 0.001f);  // Avoid division by zero
        confidence = std::clamp(confidence, 0.0f, 1.0f);
        
        commands.push_back({cmdList[i], commandActs[i], confidence});
    }
    
    // Select best command with confidence thresholding
    MotorCommand best = MotorCommand::Wait;
    float bestConfidence = 0.0f;
    
    for (const auto& c : commands) {
        if (c.activity > 0.1f && c.confidence > 0.3f) {  // Minimum activity and confidence
            if (c.confidence > bestConfidence) {
                bestConfidence = c.confidence;
                best = c.cmd;
            }
        }
    }
    
    // Override to Wait if no confident command
    if (bestConfidence < 0.3f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-based exploration with multiple strategies
    if (curiosityLevel_ <= 0.1f) {  // Low curiosity - minimal exploration
        return defaultCmd;
    }
    
    // Exploration strategy varies with curiosity level
    float explorationIntensity = std::min(curiosityLevel_ * 2.0f, 1.0f);  // Cap at 100%
    
    switch (static_cast<int>(explorationIntensity * 10)) {
        case 0:  // Very low curiosity - no exploration
            return defaultCmd;
            
        case 1:  // Very low curiosity - 5% chance of exploration
        case 2:  // Low curiosity - 10% chance
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < explorationIntensity * 0.5f) {
                return exploreRandomAction();
            }
            break;
            
        case 3:  // Low curiosity - 15% chance
        case 4:  // Moderate curiosity - 25% chance
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < explorationIntensity * 0.8f) {
                return exploreRandomAction();
            }
            break;
            
        case 5:  // Moderate curiosity - 35% chance
        case 6:  // High curiosity - 50% chance
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < explorationIntensity) {
                return exploreRandomAction();
            }
            break;
            
        case 7:  // High curiosity - 75% chance
        case 8:  // Very high curiosity - 90% chance
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < explorationIntensity * 1.1f) {
                return exploreRandomAction();
            }
            break;
            
        case 9:  // Very high curiosity - 100% exploration
        case 10:  // Extreme curiosity - 100% exploration
            return exploreRandomAction();
    }
    
    // Fall back to default command
    return defaultCmd;
}

MotorCommand AgentBrain::exploreRandomAction() {
    // Enhanced random exploration with weighted probabilities
    // Some actions are more likely than others (natural behavior)
    float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    
    // Weighted probabilities for more natural behavior
    if (r < 0.35f) return MotorCommand::MoveForward;     // 35% forward
    if (r < 0.55f) return MotorCommand::TurnLeft;       // 20% left
    if (r < 0.75f) return MotorCommand::TurnRight;      // 20% right
    if (r < 0.90f) return MotorCommand::MoveBackward;   // 15% backward
    if (r < 0.97f) return MotorCommand::Interact;       // 7% interact
    return MotorCommand::Wait;                          // 3% wait
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        return;
    }
    
    // Enhanced prediction error calculation with temporal difference
    predictionError_ = reward - predictedReward;
    
    // Update expected reward with exponential moving average
    float alpha = 0.1f;  // Learning rate for expectation
    expectedReward_ = (1.0f - alpha) * expectedReward_ + alpha * reward;
    
    // Enhanced dopamine-like signal with complex dynamics
    // Dopamine burst for unexpected large positive reward
    // Dopamine dip for unexpected negative reward
    // Dopamine phasic for prediction error magnitude
    
    float baseDopamine = 0.0f;
    float rewardGain = 1.0f;
    float predictionErrorGain = 0.5f;
    
    // Apply reward prediction error with biological plausibility
    float predictionErrorInfluence = predictionError_ * predictionErrorGain;
    
    // Add reward magnitude effect (phasic response)
    float rewardPhasic = reward * rewardGain;
    
    // Combine prediction error and reward signals
    float totalDopamineSignal = baseDopamine + predictionErrorInfluence + rewardPhasic;
    
    // Apply non-linear transformation to get final dopamine level
    // Dopamine saturates at high levels
    dopamineLevel_ = std::tanh(totalDopamineSignal);
    
    // Clamp to realistic range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated weight changes with eligibility traces
    if (std::abs(predictionError_) > 0.01f || std::abs(reward) > 0.01f) {
        // Enhanced synaptic plasticity
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (!syn) continue;
                
                float eligibility = syn->getEligibilityTrace();
                
                if (std::abs(eligibility) > 0.001f) {
                    // Advanced reward-modulated weight change
                    float temporalFactor = std::exp(-0.01f * developmentalAge_);  // Decay over time
                    float plasticityFactor = 1.0f + std::abs(dopamineLevel_) * 2.0f;  // Dopamine modulates plasticity
                    
                    // Apply weight change with eligibility trace and temporal factor
                    float delta = eligibility * dopamineLevel_ * plasticityFactor * temporalFactor;
                    
                    // Enhanced weight clamping with adaptive bounds
                    float newWeight = syn->getWeight() + delta;
                    float adaptiveMax = 2.0f * (1.0f + plasticityModifier_);  // Increases with development
                    float adaptiveMin = -adaptiveMax * 0.5f;
                    newWeight = std::clamp(newWeight, adaptiveMin, adaptiveMax);
                    syn->setWeight(newWeight);
                    
                    // Enhanced eligibility trace decay
                    float decayRate = 0.1f + 0.05f * std::abs(dopamineLevel_);  // Faster decay with high dopamine
                    syn->decayEligibilityTrace(decayRate);
                }
            }
        }
    }
    
    // Modulate plasticity rules based on dopamine
    // Positive dopamine enhances LTP, negative dopamine enhances LTD
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;  // Linear relationship
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);  // Prevent extreme values
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Scale STDP parameters based on dopamine level
        float ltpWeight = 0.01f * plasticityFactor;
        float ltdWeight = 0.012f * plasticityFactor;
        
        // Different scaling for positive vs negative dopamine
        if (dopamineLevel_ > 0.1f) {
            // High dopamine favors LTP
            ltpWeight *= (1.0f + dopamineLevel_);
        } else if (dopamineLevel_ < -0.1f) {
            // Low dopamine favors LTD
            ltdWeight *= (1.0f - dopamineLevel_);
        }
        
        stdp->setLTPWeight(ltpWeight);
        stdp->setLTDWeight(ltdWeight);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Enhanced developmental stages with more sophisticated dynamics
    // Plasticity naturally decays with age but with critical periods
    
    double ageInMinutes = developmentalAge_ / 60.0;  // Convert to minutes for readability
    
    if (ageInMinutes < 60.0) {  // ~1 minute - Early development
        plasticityModifier_ = 1.2f;  // Enhanced plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        NLM_LOG_INFO("Development: Initial stage - high plasticity (age: " + 
                     std::to_string(ageInMinutes) + " min)");
    } else if (ageInMinutes < 300.0) {  // ~5 minutes - Critical period
        plasticityModifier_ = 0.9f * std::exp(-(ageInMinutes - 60.0) / 100.0);  // Gradual decline
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        NLM_LOG_INFO("Development: Critical period - moderate plasticity (age: " + 
                     std::to_string(ageInMinutes) + " min)");
    } else if (ageInMinutes < 900.0) {  // ~15 minutes - Maturation
        plasticityModifier_ = 0.6f * std::exp(-(ageInMinutes - 300.0) / 300.0);  // Slow decay
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        NLM_LOG_INFO("Development: Maturation - reduced plasticity (age: " + 
                     std::to_string(ageInMinutes) + " min)");
    } else {  // > 15 minutes - Adult
        plasticityModifier_ = 0.3f;  // Stabilized plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        NLM_LOG_INFO("Development: Adult stage - stable plasticity (age: " + 
                     std::to_string(ageInMinutes) + " min)");
    }
    
    // Enhanced structural plasticity with age-dependent rates
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Synaptogenesis decreases exponentially with age
            float synaptogenesisRate = 0.0002f * std::exp(-ageInMinutes / 200.0);
            
            // Pruning increases with age (more selective)
            float pruningRate = 0.00005f * (1.0f + ageInMinutes / 1000.0);
            
            // Apply age-dependent rates
            sp->setSynaptogenesisRate(synaptogenesisRate);
            sp->setPruningRate(pruningRate);
            
            NLM_LOG_INFO("Structural plasticity: synaptogenesis=" + std::to_string(synaptogenesisRate) +
                         ", pruning=" + std::to_string(pruningRate));
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
    // Complete reset of all internal state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear all neuron activity and reset to resting state
    for (auto* neuron : motorForward_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorBackward_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorTurnLeft_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorTurnRight_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorInteract_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : motorWait_) {
        if (neuron) neuron->reset();
    }
    
    for (auto* neuron : sensoryVision_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : sensoryTouch_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : sensoryInternal_) {
        if (neuron) neuron->reset();
    }
    for (auto* neuron : sensoryProprioception_) {
        if (neuron) neuron->reset();
    }
    
    // Clear previous vision data
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset completed - all systems cleared");
}

} // namespace nlm
