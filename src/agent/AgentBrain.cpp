#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    // Configuration parameters with defaults
    , motorDistributionGroups_(6)
    , sensoryDistributionGroups_(4)
    , motorDistributionCounter_(0)
    , sensoryDistributionCounter_(0)
    // Sensory and motor sizing configuration
    , sensoryInputSize_(256 + 8 + 4 + 6)  // Vision + touch + internal + proprioception
    , motorOutputSize_(6)                 // One motor per action
    , visionWidth_(16)
    , visionHeight_(16)
    , touchInputSize_(8)
    , internalInputSize_(4)
    , proprioceptionInputSize_(6)
    // Curiosity configuration parameters
    , curiosityThreshold_(0.3f)
    , explorationThreshold_(0.5f)
    , explorationChanceMultiplier_(0.3f)
    , activityThreshold_(0.5f)
    , curiosityNoveltyCoefficient_(2.0f)
    , curiosityErrorCoefficient_(0.5f)
{
    // Initialize motor and sensory neuron groups with improved distribution logic
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    // Improved motor neuron distribution
                    size_t distributionIndex = 0;
                    for (Neuron* n : pop->getNeurons()) {
                        // Use modulo for consistent group assignment
                        size_t group = distributionIndex % motorDistributionGroups_;
                        distributionIndex++;
                        
                        switch (group) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            default: motorWait_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    // Improved sensory neuron distribution
                    size_t distributionIndex = 0;
                    for (Neuron* n : pop->getNeurons()) {
                        // Use modulo for consistent group assignment
                        size_t group = distributionIndex % sensoryDistributionGroups_;
                        distributionIndex++;
                        
                        switch (group) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                            default: sensoryVision_.push_back(n); break;
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
    // Use configurable sensory input size (vision + touch + internal + proprioception)
    return sensoryInputSize_;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Use configurable motor output size (one motor per action)
    return motorOutputSize_;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: No brain available");
        return;
    }
    
    if (!percept.isValid()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Invalid sensory percept");
        return;
    }
    
    // Validate that we have enough neurons for the expected input sizes
    if (sensoryVision_.size() > getSensoryInputSize()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Too many vision neurons");
    }
    if (sensoryTouch_.size() > getTouchInputSize()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Too many touch neurons");
    }
    if (sensoryInternal_.size() > getInternalInputSize()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Too many internal neurons");
    }
    if (sensoryProprioception_.size() > getProprioceptionInputSize()) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Too many proprioception neurons");
    }
    
    // Vision input (configurable size -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    if (!vision.empty()) {
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                // Inject current proportional to vision intensity
                float current = vision[i] * 5.0f;  // Scale factor
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Touch input (configurable size -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    if (!touch.empty()) {
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;  // Collision signal
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals (configurable size -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    if (!intern.empty()) {
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception (configurable size -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    if (!proprio.empty()) {
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        if (previousVision_.size() != vision.size()) {
            previousVision_.resize(vision.size(), 0.0f);
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: Resized previousVision buffer");
        }
        
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
        curiosityLevel_ = noveltyLevel_ * curiosityNoveltyCoefficient_ + std::abs(predictionError_) * curiosityErrorCoefficient_;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    float forwardAct = calculateActivity(motorForward_);
    float backwardAct = calculateActivity(motorBackward_);
    float leftAct = calculateActivity(motorTurnLeft_);
    float rightAct = calculateActivity(motorTurnRight_);
    float interactAct = calculateActivity(motorInteract_);
    float waitAct = calculateActivity(motorWait_);
    
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
    if (bestActivity < activityThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > explorationThreshold_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationChanceMultiplier_;  // Up to configurable chance
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command - use getMotorOutputSize() for configurable range
            int maxChoice = getMotorOutputSize();
            int choice = brain_->getRandomGenerator()->uniformInt(0, maxChoice);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

float AgentBrain::calculateActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Reward modulation disabled or no brain");
        return;
    }
    
    // Validate reward values
    if (std::isnan(reward) || std::isnan(predictedReward)) {
        NLM_LOG_WARNING("AgentBrain::applyRewardModulation: NaN reward values");
        reward = 0.0f;
        predictedReward = 0.0f;
    }
    
    // Clamp reward values to reasonable range
    reward = std::clamp(reward, -10.0f, 10.0f);
    predictedReward = std::clamp(predictedReward, -10.0f, 10.0f);
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Clamp expected reward
    expectedReward_ = std::clamp(expectedReward_, -5.0f, 5.0f);
    
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
    
    NLM_LOG_DEBUG("AgentBrain::applyRewardModulation: reward=" + std::to_string(reward) +
                 " pred=" + std::to_string(predictedReward) +
                 " error=" + std::to_string(predictionError_) +
                 " dopamine=" + std::to_string(dopamineLevel_));
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment: Development disabled or no brain");
        return;
    }
    
    // Validate timestep
    if (std::isnan(timestep) || timestep < 0.0) {
        NLM_LOG_WARNING("AgentBrain::updateDevelopment: Invalid timestep (" + std::to_string(timestep) + ")");
        timestep = 0.0;
    }
    
    // Limit timestep to reasonable range
    timestep = std::clamp(timestep, 0.0, 1.0);
    
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
    
    // Validate plasticity modifier
    plasticityModifier_ = std::clamp(plasticityModifier_, 0.1f, 2.0f);
    
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
    
    NLM_LOG_DEBUG("AgentBrain::updateDevelopment: age=" + std::to_string(developmentalAge_) +
                 " stage=" + std::to_string(static_cast<int>(brain_->getDevelopmentalStage())) +
                 " plasticity=" + std::to_string(plasticityModifier_));
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
    NLM_LOG_INFO("AgentBrain::reset: Starting agent reset");
    
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset configuration counters
    motorDistributionCounter_ = 0;
    sensoryDistributionCounter_ = 0;
    
    NLM_LOG_INFO("AgentBrain::reset: Agent reset completed");
}
