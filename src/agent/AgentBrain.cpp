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
            if (!region) {
                NLM_LOG_WARN("Found null region in brain, skipping");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_WARN("Found null population in region, skipping");
                    continue;
                }
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        if (!n) {
                            NLM_LOG_WARN("Found null neuron in motor population, skipping");
                            continue;
                        }
                        
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
                        if (!n) {
                            NLM_LOG_WARN("Found null neuron in sensory population, skipping");
                            continue;
                        }
                        
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
    
    // Validate that we have at least some motor neurons
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() +
                               motorTurnLeft_.size() + motorTurnRight_.size() +
                               motorInteract_.size() + motorWait_.size();
    
    if (totalMotorNeurons == 0) {
        NLM_LOG_WARN("No motor neurons found in brain - AgentBrain may not function correctly");
    }
    
    // Validate that we have sensory neurons
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (totalSensoryNeurons == 0) {
        NLM_LOG_WARN("No sensory neurons found in brain - AgentBrain may not function correctly");
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
    // Use named constants instead of magic numbers
    constexpr size_t VISION_PIXELS = 16 * 16;  // 256
    constexpr size_t TOUCH_SENSORS = 8;
    constexpr size_t INTERNAL_SIGNALS = 4;
    constexpr size_t PROPRIOCEPTORS = 6;
    
    return VISION_PIXELS + TOUCH_SENSORS + INTERNAL_SIGNALS + PROPRIOCEPTORS;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action group
    // Use named constant instead of magic number
    constexpr size_t MOTOR_ACTION_GROUPS = 6;
    
    return MOTOR_ACTION_GROUPS;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Scale factors for sensory transduction - use named constants
    constexpr float VISION_SCALE = 5.0f;
    constexpr float TOUCH_SCALE = 8.0f;
    constexpr float INTERNAL_CENTER = 2.0f;
    constexpr float INTERNAL_SCALE = 5.0f;
    constexpr float PROPRIOCEPTION_CENTER = 2.0f;
    constexpr float PROPRIOCEPTION_SCALE = 3.0f;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * VISION_SCALE;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * TOUCH_SCALE; // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * INTERNAL_CENTER - 1.0f) * INTERNAL_SCALE; // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * PROPRIOCEPTION_CENTER - 1.0f) * PROPRIOCEPTION_SCALE; // Center and scale
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Novelty detection
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
        // Curiosuty calculation constants
        constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;
        constexpr float CURIOSITY_ERROR_WEIGHT = 0.5f;
        
        curiosityLevel_ = noveltyLevel_ * CURIOSITY_NOVELTY_WEIGHT + std::abs(predictionError_) * CURIOSITY_ERROR_WEIGHT;
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
    // Threshold for considering an action "meaningful"
    constexpr float ACTION_THRESHOLD = 0.5f;
    
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
    if (bestActivity < ACTION_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Thresholds for curiosity-based exploration - use named constants
    constexpr float HIGH_CURiosity_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CHANCE_FACTOR = 0.3f;
    constexpr int MAX_MOTOR_ACTION_GROUPS = 5;
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > HIGH_CURiosity_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * EXPLORATION_CHANCE_FACTOR;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command from available motor groups
            // Only use motor groups that actually exist in the brain
            int choice = brain_->getRandomGenerator()->uniformInt(0, MAX_MOTOR_ACTION_GROUPS);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    // Apply curiosity-based exploration with smart strategies
    return applySmartExploration(defaultCmd);
}

MotorCommand AgentBrain::applySmartExploration(MotorCommand defaultCmd) {
    // Enhanced curiosity-based exploration with adaptive strategies
    // Combines random exploration with context-aware decision making
    
    if (!curiosityEnabled_ || curiosityLevel_ <= 0.3f) {
        return defaultCmd;
    }
    
    // Get current brain state for context-aware exploration
    if (!brain_) return defaultCmd;
    
    // Exploration strategy thresholds - use named constants
    constexpr float LOW_CURiosity_THRESHOLD = 0.3f;
    constexpr float HIGH_CURiosity_THRESHOLD = 0.7f;
    constexpr float EXPLORATION_BIAS_FACTOR = 1.2f;
    constexpr float GENTLE_EXPLORATION_FACTOR = 0.2f;
    
    // Exploration strategies based on curiosity level
    float normalizedCuriosity = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    
    // Strategy 1: High curiosity = exploratory behavior
    if (normalizedCuriosity > HIGH_CURiosity_THRESHOLD) {
        // Maximum exploration: choose from all motor commands
        // This is ideal for discovering new behaviors
        int choice = brain_->getRandomGenerator()->uniformInt(0, 6);
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            default: return MotorCommand::Interact;
        }
    }
    
    // Strategy 2: Medium curiosity = balanced exploration
    else if (normalizedCuriosity > LOW_CURiosity_THRESHOLD) {
        // Moderate exploration: bias toward less-tried actions
        // This promotes variety while maintaining some consistency
        
        // Use named constants for action bias
        float weights[6] = {1.0f, 1.0f, 1.0f, 1.0f, EXPLORATION_BIAS_FACTOR, EXPLORATION_BIAS_FACTOR}; // Favor exploration actions
        
        // Normalize weights
        float totalWeight = weights[0] + weights[1] + weights[2] +
                           weights[3] + weights[4] + weights[5];
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, totalWeight);
        float cumulative = 0.0f;
        
        for (int i = 0; i < 6; ++i) {
            cumulative += weights[i];
            if (r <= cumulative) {
                switch (i) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::LookLeft;
                    case 5: return MotorCommand::LookRight;
                }
            }
        }
    }
    
    // Strategy 3: Low curiosity = slight exploration bias
    else {
        // Gentle exploration: occasionally try new actions
        // This prevents getting stuck in local optima
        
        // Use named constant for exploration probability
        float exploreChance = normalizedCuriosity * GENTLE_EXPLORATION_FACTOR;  // Up to 20% chance
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        if (r < exploreChance) {
            // 50/50 chance to try new action or stay with default
            if (brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.5f) {
                // Try new action
                int choice = brain_->getRandomGenerator()->uniformInt(0, 5);
                switch (choice) {
                    case 0: return MotorCommand::MoveForward;
                    case 1: return MotorCommand::MoveBackward;
                    case 2: return MotorCommand::TurnLeft;
                    case 3: return MotorCommand::TurnRight;
                    case 4: return MotorCommand::Interact;
                    default: return MotorCommand::Wait;
                }
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
