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
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::processSensoryInput called with null brain pointer");
        return;
    }
    
    // Get sensory data from percept
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Validate that sensory arrays are not empty
    if (vision.empty() && touch.empty() && intern.empty() && proprio.empty()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput called with empty sensory percept");
        return;
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Check for NaN values
            if (std::isnan(vision[i])) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput vision value is NaN at index " + std::to_string(i));
                continue;  // Skip this value
            }
            
            // Clamp to reasonable range
            float currentValue = std::clamp(vision[i], -1.0f, 1.0f);
            
            // Inject current proportional to vision intensity
            float current = currentValue * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            // Check for NaN values
            if (std::isnan(touch[i])) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput touch value is NaN at index " + std::to_string(i));
                continue;
            }
            
            // Clamp to reasonable range
            float currentValue = std::clamp(touch[i], -1.0f, 1.0f);
            
            float current = currentValue * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Check for NaN values
            if (std::isnan(intern[i])) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput internal value is NaN at index " + std::to_string(i));
                continue;
            }
            
            // Clamp to reasonable range
            float currentValue = std::clamp(intern[i], -1.0f, 1.0f);
            
            float current = (currentValue * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Check for NaN values
            if (std::isnan(proprio[i])) {
                NLM_LOG_WARNING("AgentBrain::processSensoryInput proprioception value is NaN at index " + std::to_string(i));
                continue;
            }
            
            // Clamp to reasonable range
            float currentValue = std::clamp(proprio[i], -1.0f, 1.0f);
            
            float current = (currentValue * 2.0f - 1.0f) * 3.0f;  // Center and scale
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
    // Decode neural activity into motor commands
    //
    // This method:
    // 1. Calculates activity levels in each motor neuron group
    // 2. Selects the motor command with highest activity
    // 3. Applies curiosity-based exploration if enabled
    
    // Constants for motor decoding
    const float MIN_ACTIVITY_THRESHOLD = 0.5f;  // Minimum activity to trigger action
    const float CURIOSITY_THRESHOLD = 0.3f;     // Minimum curiosity level for exploration
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > CURIOSITY_THRESHOLD) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Decode motor neuron activity into motor commands
    //
    // This method analyzes activity across motor neuron populations and selects
    // the most appropriate motor command based on competitive activation.
    //
    // Process:
    // 1. Calculate activity in each motor group (forward, backward, left, right, interact, wait)
    // 2. Compare activities and select the most active command
    // 3. Apply minimum activity threshold to prevent weak commands from being executed
    
    // Constants for motor decoding
    const float MIN_ACTIVITY_THRESHOLD = 0.5f;  // Minimum normalized activity to trigger action
    
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
    if (bestActivity < MIN_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration behavior: occasionally choose random action when curiosity is high
    // 
    // Parameters:
    // - curiosityLevel_: A value between 0.0 and 1.0 representing the agent's curiosity
    //   (driven by novelty detection and prediction error)
    // - defaultCmd: The default motor command to return when not exploring
    
    // Constants for exploration behavior
    const float CURIOSITY_THRESHOLD = 0.5f;      // Minimum curiosity level to trigger exploration
    const float MAX_EXPLORATION_CHANCE = 0.3f;   // Maximum probability of choosing random action
    const int MOTOR_COMMAND_COUNT = 8;          // Number of MotorCommand enum values
    
    if (curiosityLevel_ > CURIOSITY_THRESHOLD) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * MAX_EXPLORATION_CHANCE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - map each possible random value to a motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, MOTOR_COMMAND_COUNT - 1);
            // MotorCommand enum has MOTOR_COMMAND_COUNT values, so we can directly map
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;  // Explicit mapping instead of default
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation called with null brain pointer or reward modulation disabled");
        return;
    }
    
    // Validate parameters
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation called with NaN values: reward=" + 
                       std::to_string(reward) + ", predictedReward=" + std::to_string(predictedReward));
        reward = std::max(0.0f, reward);
        predictedReward = std::max(0.0f, predictedReward);
    }
    
    // Clamp reward values to reasonable range
    const float MAX_REWARD = 100.0f;
    const float MIN_REWARD = -100.0f;
    
    if (reward > MAX_REWARD) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation reward value too large: " + std::to_string(reward));
        reward = MAX_REWARD;
    } else if (reward < MIN_REWARD) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation reward value too small: " + std::to_string(reward));
        reward = MIN_REWARD;
    }
    
    if (predictedReward > MAX_REWARD) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation predictedReward value too large: " + std::to_string(predictedReward));
        predictedReward = MAX_REWARD;
    } else if (predictedReward < MIN_REWARD) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation predictedReward value too small: " + std::to_string(predictedReward));
        predictedReward = MIN_REWARD;
    }
    
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
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_ERROR("AgentBrain::updateDevelopment called with null brain pointer or development disabled");
        return;
    }
    
    // Validate timestep parameter
    if (std::isnan(timestep) || std::isinf(timestep)) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment called with invalid timestep: " + std::to_string(timestep));
        timestep = 0.001;  // Default small timestep
    }
    
    // Check timestep is within reasonable bounds
    const double MAX_TIMESTEP = 1.0;  // 1 second maximum
    const double MIN_TIMESTEP = 0.000001;  // 1 microsecond minimum
    
    if (timestep > MAX_TIMESTEP) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment timestep too large: " + std::to_string(timestep));
        timestep = MAX_TIMESTEP;
    } else if (timestep < MIN_TIMESTEP) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment timestep too small: " + std::to_string(timestep));
        timestep = MIN_TIMESTEP;
    }
    
    // Ensure timestep is positive
    if (timestep <= 0.0) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment timestep must be positive: " + std::to_string(timestep));
        timestep = 0.001;  // Default timestep
    }
    
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
