#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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
    , sensoryNoveltyDecay_(0.99f)
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
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
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
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
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
    // Advanced action selection with curiosity-driven exploration
    // Integrates curiosity, novelty, and prediction error for optimal behavior
    
    // Calculate exploration propensity based on multiple factors
    float explorationFactor = 0.0f;
    
    // 1. Curiosity factor: high curiosity drives exploration
    explorationFactor += curiosityLevel_ * 0.4f;
    
    // 2. Novelty factor: novel environments encourage exploration
    explorationFactor += noveltyLevel_ * 0.3f;
    
    // 3. Prediction error factor: unexpected situations encourage exploration
    explorationFactor += std::abs(predictionError_) * 0.2f;
    
    // 4. Dopamine factor: low dopamine increases exploration (in depression models)
    explorationFactor += std::max(0.0f, 0.5f - std::abs(dopamineLevel_)) * 0.1f;
    
    // 5. Developmental factor: critical periods have high exploration
    float developmentalExploration = 0.0f;
    if (developmentalAge_ < 60.0f) {  // Critical period
        developmentalExploration = 0.3f;
    } else if (developmentalAge_ < 300.0f) {  // Maturation
        developmentalExploration = 0.1f;
    }
    explorationFactor += developmentalExploration * 0.1f;
    
    // Clamp exploration factor
    explorationFactor = std::clamp(explorationFactor, 0.0f, 1.0f);
    
    // Determine if we should explore based on exploration factor
    float exploreThreshold = 0.3f + explorationFactor * 0.4f;  // Higher factor = more exploration
    exploreThreshold = std::clamp(exploreThreshold, 0.3f, 0.8f);
    
    // Random number for exploration decision
    float explorationRoll = brain_->getRandomGenerator() ? 
        brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
    
    if (explorationRoll < exploreThreshold) {
        // EXPLORATION: Choose action based on curiosity and novelty
        return chooseExplorationAction();
    } else {
        // EXPLOITATION: Use default action (most rewarding known action)
        return defaultCmd;
    }
}

MotorCommand AgentBrain::chooseExplorationAction() {
    // Choose exploration action based on sensory interest and curiosity
    
    // Calculate action suitability based on current sensory input
    std::vector<std::pair<MotorCommand, float>> actionScores;
    
    // Vision-based action selection
    float visionInterest = calculateVisionInterest();
    if (visionInterest > 0.5f) {
        // High visual interest: explore visually
        actionScores.push_back({MotorCommand::LookLeft, visionInterest * 0.3f});
        actionScores.push_back({MotorCommand::LookRight, visionInterest * 0.3f});
        actionScores.push_back({MotorCommand::Interact, visionInterest * 0.4f});
    }
    
    // Movement-based action selection
    float movementInterest = calculateMovementInterest();
    if (movementInterest > 0.3f) {
        // High movement interest: explore the environment
        actionScores.push_back({MotorCommand::MoveForward, movementInterest * 0.4f});
        actionScores.push_back({MotorCommand::MoveBackward, movementInterest * 0.2f});
        actionScores.push_back({MotorCommand::TurnLeft, movementInterest * 0.2f});
        actionScores.push_back({MotorCommand::TurnRight, movementInterest * 0.2f});
    }
    
    // Novelty-based action selection
    float noveltyBasedScore = noveltyLevel_ * 0.5f;
    if (noveltyBasedScore > 0.2f) {
        // Novelty suggests trying something different
        actionScores.push_back({MotorCommand::Wait, noveltyBasedScore * 0.3f});
    }
    
    // Random action selection with bias toward unexplored actions
    if (actionScores.empty()) {
        // No preferred actions, choose random
        int choice = brain_->getRandomGenerator() ? 
            brain_->getRandomGenerator()->uniformInt(0, 7) : 0;
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
    
    // Select best action based on scores
    MotorCommand bestAction = defaultCmd;
    float bestScore = 0.0f;
    
    for (const auto& actionScore : actionScores) {
        if (actionScore.second > bestScore) {
            bestScore = actionScore.second;
            bestAction = actionScore.first;
        }
    }
    
    // Add random exploration: with small probability, choose random action
    float randomChance = curiosityLevel_ * 0.2f;  // More random when curious
    if (brain_->getRandomGenerator() && 
        brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < randomChance) {
        // Force random action to ensure diverse exploration
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
    
    return bestAction;
}

float AgentBrain::calculateVisionInterest() const {
    // Calculate how visually interesting the current scene is
    // High edge density, novel patterns, etc. increase interest
    
    // This would access the actual visual features from the brain
    // For now, use a simplified version based on novelty
    float baseInterest = noveltyLevel_ * 0.5f;
    
    // Add curiosity-based amplification
    float curiosityAmplification = curiosityLevel_ * 0.3f;
    
    // Consider prediction error
    float predictionErrorAmplification = std::abs(predictionError_) * 0.2f;
    
    return std::min(1.0f, baseInterest + curiosityAmplification + predictionErrorAmplification);
}

float AgentBrain::calculateMovementInterest() const {
    // Calculate how much the agent should move based on internal state
    // This would analyze proprioception, energy levels, etc.
    
    // Simplified version based on developmental stage and energy
    float developmentalInterest = 0.0f;
    if (developmentalAge_ < 60.0f) {  // Critical period - high exploration
        developmentalInterest = 0.8f;
    } else if (developmentalAge_ < 300.0f) {  // Maturation
        developmentalInterest = 0.5f;
    } else {  // Adult
        developmentalInterest = 0.2f;
    }
    
    // Base interest on energy level (low energy = less movement)
    float energyLevel = 0.5f;  // Placeholder - would come from actual internal signals
    float energyInterest = energyLevel * 0.5f;
    
    // Combine factors
    return std::min(1.0f, (developmentalInterest + energyInterest) * 0.5f);
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
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm
