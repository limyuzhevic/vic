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
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize motor and sensory neuron groups with improved distribution
    if (brain_) {
        // Use random distribution for more natural neuron assignment
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> motorDist(0, 5);
        std::uniform_int_distribution<> sensoryDist(0, 3);
        
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Improved: Better balance of motor neuron groups
                        int group = motorDist(gen);
                        switch (static_cast<size_t>(group)) {
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
                        // Improved: Balanced sensory distribution
                        int group = sensoryDist(gen);
                        switch (static_cast<size_t>(group)) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                        }
                    }
                }
            }
        }
        
        // Log initialization statistics
        NLM_LOG_INFO("AgentBrain initialized with " +
                     std::to_string(motorForward_.size() + motorBackward_.size() + 
                                   motorTurnLeft_.size() + motorTurnRight_.size() +
                                   motorInteract_.size() + motorWait_.size()) + 
                     " motor neurons and " +
                     std::to_string(sensoryVision_.size() + sensoryTouch_.size() +
                                   sensoryInternal_.size() + sensoryProprioception_.size()) +
                     " sensory neurons");
    } else {
        NLM_LOG_WARNING("AgentBrain created with null brain pointer");
    }
}

AgentBrain::~AgentBrain() = default;

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

void AgentBrain::initialize(const SimpleWorld& world) {
    // Enhanced initialization with better error handling
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Validate that world has valid vision dimensions
    if (world.getVisionWidth() == 0 || world.getVisionHeight() == 0) {
        NLM_LOG_WARNING("Invalid world vision dimensions: " +
                       std::to_string(world.getVisionWidth()) + "x" +
                       std::to_string(world.getVisionHeight()));
        return;
    }
    
    // Validate neuron groups are properly assigned
    size_t totalVisionNeurons = sensoryVision_.size();
    size_t totalTouchNeurons = sensoryTouch_.size();
    size_t totalInternalNeurons = sensoryInternal_.size();
    size_t totalProprioceptionNeurons = sensoryProprioception_.size();
    
    NLM_LOG_INFO("AgentBrain initialized with " +
                 std::to_string(totalVisionNeurons) + " vision sensory neurons, " +
                 std::to_string(totalTouchNeurons) + " touch sensory neurons, " +
                 std::to_string(totalInternalNeurons) + " internal sensory neurons, " +
                 std::to_string(totalProprioceptionNeurons) + " proprioception neurons");
    
    // Log motor neuron distribution for debugging
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() + 
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorInteract_.size() + motorWait_.size();
    if (totalMotorNeurons > 0) {
        NLM_LOG_INFO("Motor neuron distribution - Forward: " + 
                     std::to_string(motorForward_.size()) +
                     ", Backward: " + std::to_string(motorBackward_.size()) +
                     ", TurnLeft: " + std::to_string(motorTurnLeft_.size()) +
                     ", TurnRight: " + std::to_string(motorTurnRight_.size()) +
                     ", Interact: " + std::to_string(motorInteract_.size()) +
                     ", Wait: " + std::to_string(motorWait_.size()));
    }
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
    // Enhanced reward modulation with better error handling and validation
    if (!brain_) {
        NLM_LOG_ERROR("Cannot apply reward modulation: brain pointer is null");
        return;
    }
    
    if (!rewardModulationEnabled_) {
        NLM_LOG_DEBUG("Reward modulation is disabled");
        return;
    }
    
    // Validate reward range (reasonable bounds)
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_WARNING("Invalid reward value: " + std::to_string(reward));
        reward = std::clamp(reward, -100.0f, 100.0f);
    }
    
    if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
        NLM_LOG_WARNING("Invalid predicted reward value: " + std::to_string(predictedReward));
        predictedReward = std::clamp(predictedReward, -100.0f, 100.0f);
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average with bounded learning rate)
    constexpr float LEARNING_RATE = 0.05f;
    expectedReward_ += LEARNING_RATE * (reward - expectedReward_);
    
    // Dopamine-like signal (based on prediction error with saturation)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range (dopamine levels in biological systems are typically -1 to 1)
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces - improved error handling
    bool synapseModified = false;
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with bounded update
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                
                // Bound weight change to prevent runaway excitation/inhibition
                constexpr float MAX_WEIGHT_CHANGE = 0.05f;
                if (std::abs(delta) > MAX_WEIGHT_CHANGE) {
                    delta = std::copysign(MAX_WEIGHT_CHANGE, delta);
                }
                
                syn->addToWeight(delta);
                synapseModified = true;
                
                // Decay eligibility trace with configurable rate
                constexpr float ELIGIBILITY_DECAY = 0.1f;
                syn->decayEligibilityTrace(ELIGIBILITY_DECAY);
            }
        }
    }
    
    if (synapseModified) {
        NLM_LOG_DEBUG("Applied reward modulation: reward=" + std::to_string(reward) +
                     " pred=" + std::to_string(predictedReward) +
                     " delta=" + std::to_string(predictionError_));
    }
    
    // Modulate plasticity based on dopamine - improved logic
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    
    // Ensure plasticity factor stays within reasonable bounds
    constexpr float MIN_PLASTICITY_FACTOR = 0.1f;
    constexpr float MAX_PLASTICITY_FACTOR = 2.0f;
    plasticityFactor = std::clamp(plasticityFactor, MIN_PLASTICITY_FACTOR, MAX_PLASTICITY_FACTOR);
    
    // Apply to STDP if available - better error checking
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        // Store original values for logging
        float originalLTP = stdp->getLTPWeight();
        float originalLTD = stdp->getLTDWeight();
        
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
        
        // Log if plasticity changed significantly
        if (std::abs(0.01f * plasticityFactor - originalLTP) > 0.001f ||
            std::abs(0.012f * plasticityFactor - originalLTD) > 0.001f) {
            NLM_LOG_DEBUG("Plasticity modulated: LTP " + std::to_string(originalLTP) + " -> " +
                         std::to_string(0.01f * plasticityFactor) + 
                         ", LTD " + std::to_string(originalLTD) + " -> " +
                         std::to_string(0.012f * plasticityFactor));
        }
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
