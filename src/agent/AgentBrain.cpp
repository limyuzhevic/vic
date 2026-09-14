// Include agent brain header
#include "AgentBrain.hpp"

// Include core logger for status logging
#include "../core/Logger/Logger.hpp"

// Standard C++ libraries for algorithms and math
#include <algorithm>
#include <cmath>

// Define namespace for NLM (Neural Learning Machine)
namespace nlm {

// Constants for sensor and motor configuration
namespace AgentConstants {
    // Motor neuron groups - 6 primary actions
    constexpr size_t MotorForwardCount = 6;
    constexpr size_t MotorBackwardCount = 6;
    constexpr size_t MotorTurnLeftCount = 6;
    constexpr size_t MotorTurnRightCount = 6;
    constexpr size_t MotorInteractCount = 6;
    constexpr size_t MotorWaitCount = 6;
    
    // Sensory neuron groups - 4 main types
    constexpr size_t SensoryVisionCount = 16;
    constexpr size_t SensoryTouchCount = 8;
    constexpr size_t SensoryInternalCount = 4;
    constexpr size_t SensoryProprioceptionCount = 6;
    
    // Scaling factors for sensor input processing
    constexpr float VisionCurrentScale = 5.0f;
    constexpr float TouchCurrentScale = 8.0f;
    constexpr float InternalCurrentScale = 5.0f;
    constexpr float ProprioceptionCurrentScale = 3.0f;
    
    // Novelty detection parameters
    constexpr float NoveltyDecayFactor = 0.99f;
    constexpr float CuriosityScaleFactor = 2.0f;
    constexpr float CuriosityErrorFactor = 0.5f;
    constexpr float ExplorationThreshold = 0.3f;
    constexpr float HighCuriosityThreshold = 0.5f;
    constexpr float ExplorationChance = 0.3f;
    
    // Reward modulation parameters
    constexpr float DopamineClampMin = -1.0f;
    constexpr float DopamineClampMax = 1.0f;
    constexpr float ExpectedRewardSmoothing = 0.95f;
    constexpr float LearningRateSmoothing = 0.05f;
    constexpr float PlasticityBase = 0.5f;
    constexpr float PlasticityRange = 0.5f;
    
    // Development system parameters
    constexpr double DevelopmentStage1End = 60.0;  // Initial
    constexpr double DevelopmentStage2End = 300.0; // CriticalPeriod
    constexpr double DevelopmentStage3End = 900.0; // Maturation
    
    // Structural plasticity rates
    constexpr float SynaptogenesisRateBase = 0.0001f;
    constexpr float PruningRateBase = 0.00001f;
}

// Constructor - initialize agent brain with given brain
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
    , sensoryNoveltyDecay_(AgentConstants::NoveltyDecayFactor)
{
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_WARN("AgentBrain initialized with null brain pointer");
        return;
    }
    
    // Initialize motor and sensory neuron groups
    InitializeNeuronGroups();
}

// Helper function to initialize neuron groups based on brain regions
void AgentBrain::InitializeNeuronGroups() {
    if (!brain_) {
        NLM_LOG_WARN("Cannot initialize neuron groups: brain is null");
        return;
    }
    
    // Track how many neurons we've assigned to each type
    struct NeuronGroup {
        std::vector<Neuron*>& group;
        size_t maxSize;
    };
    
    // Create tracking arrays for different neuron groups
    NeuronGroup motorGroups[] = {
        {motorForward_, AgentConstants::MotorForwardCount},
        {motorBackward_, AgentConstants::MotorBackwardCount},
        {motorTurnLeft_, AgentConstants::MotorTurnLeftCount},
        {motorTurnRight_, AgentConstants::MotorTurnRightCount},
        {motorInteract_, AgentConstants::MotorInteractCount},
        {motorWait_, AgentConstants::MotorWaitCount}
    };
    
    NeuronGroup sensoryGroups[] = {
        {sensoryVision_, AgentConstants::SensoryVisionCount},
        {sensoryTouch_, AgentConstants::SensoryTouchCount},
        {sensoryInternal_, AgentConstants::SensoryInternalCount},
        {sensoryProprioception_, AgentConstants::SensoryProprioceptionCount}
    };
    
    // Assign neurons to groups
    AssignMotorNeurons(motorGroups, sizeof(motorGroups) / sizeof(motorGroups[0]));
    AssignSensoryNeurons(sensoryGroups, sizeof(sensoryGroups) / sizeof(sensoryGroups[0]));
}

// Function to assign motor neurons to action groups
void AgentBrain::AssignMotorNeurons(NeuronGroup groups[], size_t numGroups) {
    if (numGroups == 0) return;
    
    size_t groupIndex = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            if (population->getNeuronType() != NeuronType::Motor) {
                continue;
            }
            
            for (Neuron* neuron : population->getNeurons()) {
                // Assign neuron to first available group
                if (!groups[groupIndex].group.empty() &&
                    groups[groupIndex].group.size() < groups[groupIndex].maxSize) {
                    groups[groupIndex].group.push_back(neuron);
                }
                
                groupIndex++;
                if (groupIndex >= numGroups) {
                    groupIndex = 0;  // Loop back to start
                }
            }
        }
    }
}

// Function to assign sensory neurons to groups  
void AgentBrain::AssignSensoryNeurons(NeuronGroup groups[], size_t numGroups) {
    if (numGroups == 0) return;
    
    size_t groupIndex = 0;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            if (population->getNeuronType() != NeuronType::Sensory) {
                continue;
            }
            
            for (Neuron* neuron : population->getNeurons()) {
                // Assign neuron to first available group
                if (!groups[groupIndex].group.empty() &&
                    groups[groupIndex].group.size() < groups[groupIndex].maxSize) {
                    groups[groupIndex].group.push_back(neuron);
                }
                
                groupIndex++;
                if (groupIndex >= numGroups) {
                    groupIndex = 0;  // Loop back to start
                }
            }
        }
    }
}

// Alternative helper for simpler neuron distribution
void AgentBrain::DistributeNeuronsToGroups(
    std::vector<Neuron*>& targetGroup,
    const std::vector<Neuron*>& sourceNeurons,
    size_t maxGroupSize) {
    
    if (sourceNeurons.empty() || maxGroupSize == 0) {
        return;
    }
    
    size_t currentSize = targetGroup.size();
    size_t availableSlots = maxGroupSize - currentSize;
    
    if (availableSlots == 0) {
        return;  // Group already full
    }
    
    // Calculate how many neurons to add
    size_t neuronsToAdd = std::min(sourceNeurons.size(), availableSlots);
    
    // Add neurons to target group
    for (size_t i = 0; i < neuronsToAdd; ++i) {
        targetGroup.push_back(sourceNeurons[i]);
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

// Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Use constant for current scaling
            float current = vision[i] * AgentConstants::VisionCurrentScale;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * AgentConstants::TouchCurrentScale;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * AgentConstants::InternalCurrentScale;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * AgentConstants::ProprioceptionCurrentScale;
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
        
        // Use constant for decay and update
        noveltyLevel_ *= AgentConstants::NoveltyDecayFactor;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * AgentConstants::CuriosityScaleFactor + 
                         std::abs(predictionError_) * AgentConstants::CuriosityErrorFactor;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentConstants::ExplorationThreshold) {
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
    struct CommandInfo {
        MotorCommand cmd;
        float activity;
    };
    
    CommandInfo commands[] = {
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
    
    // Use constant for minimum activity threshold
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentConstants::HighCuriosityThreshold) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentConstants::ExplorationChance;
        
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
    float plasticityFactor = AgentConstants::PlasticityBase + AgentConstants::PlasticityRange * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, AgentConstants::DopamineClampMin, AgentConstants::DopamineClampMax);
    
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
            void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = AgentConstants::ExpectedRewardSmoothing * expectedReward_ + 
                      AgentConstants::LearningRateSmoothing * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, AgentConstants::DopamineClampMin, AgentConstants::DopamineClampMax);
    
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
    float plasticityFactor = AgentConstants::PlasticityBase + AgentConstants::PlasticityRange * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, AgentConstants::DopamineClampMin, AgentConstants::DopamineClampMax);
    
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
    if (developmentalAge_ < AgentConstants::DevelopmentStage1End) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentConstants::DevelopmentStage2End) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentConstants::DevelopmentStage3End) {  // ~15 minutes
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
            float synRate = AgentConstants::SynaptogenesisRateBase * plasticityModifier_;
            float pruneRate = AgentConstants::PruningRateBase * (2.0f - plasticityModifier_);
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
