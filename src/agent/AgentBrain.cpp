// ================================================
// IMPROVED AGENT BRAIN WITH CONSTANTS AND BETTER ORGANIZATION
// ================================================

#include "AgentBrain.hpp"
#include "../core/Constants.hpp"  // NEW: Named constants for all magic numbers
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// ================================================
// HELPER FUNCTIONS FOR AGENT BRAIN
// ================================================

// Helper function to categorize motor neurons more efficiently
static void categorizeMotorNeurons(
    std::vector<Neuron*>& motorForward,
    std::vector<Neuron*>& motorBackward,
    std::vector<Neuron*>& motorTurnLeft,
    std::vector<Neuron*>& motorTurnRight,
    std::vector<Neuron*>& motorInteract,
    std::vector<Neuron*>& motorWait,
    std::vector<Neuron*>& allMotorNeurons) {
    
    if (allMotorNeurons.empty()) return;
    
    // Pre-allocate vectors based on expected counts for better performance
    size_t totalMotorGroups = motorForward.size() + motorBackward.size() +
                             motorTurnLeft.size() + motorTurnRight.size() +
                             motorInteract.size() + motorWait.size();
    
    // Use deterministic but efficient distribution
    size_t groupSize = allMotorNeurons.size() / 6; // 6 motor action groups
    size_t remainder = allMotorNeurons.size() % 6;
    
    size_t idx = 0;
    for (size_t i = 0; i < 6; ++i) {
        size_t groupSizeWithRemainder = groupSize + (i < remainder ? 1 : 0);
        std::vector<Neuron*>& targetGroup = (i == 0) ? motorForward :
                                           (i == 1) ? motorBackward :
                                           (i == 2) ? motorTurnLeft :
                                           (i == 3) ? motorTurnRight :
                                           (i == 4) ? motorInteract : motorWait;
        
        targetGroup.reserve(targetGroup.size() + groupSizeWithRemainder);
        for (size_t j = 0; j < groupSizeWithRemainder && idx < allMotorNeurons.size(); ++j, ++idx) {
            targetGroup.push_back(allMotorNeurons[idx]);
        }
    }
}

// Helper function to categorize sensory neurons efficiently
static void categorizeSensoryNeurons(
    std::vector<Neuron*>& visionNeurons,
    std::vector<Neuron*>& touchNeurons,
    std::vector<Neuron*>& internalNeurons,
    std::vector<Neuron*>& proprioceptionNeurons,
    std::vector<Neuron*>& allSensoryNeurons) {
    
    if (allSensoryNeurons.empty()) return;
    
    // Use deterministic distribution for sensory neurons
    size_t totalSensoryGroups = visionNeurons.size() + touchNeurons.size() +
                               internalNeurons.size() + proprioceptionNeurons.size();
    
    size_t groupSize = allSensoryNeurons.size() / 4; // 4 sensory modalities
    size_t remainder = allSensoryNeurons.size() % 4;
    
    size_t idx = 0;
    for (size_t i = 0; i < 4; ++i) {
        size_t groupSizeWithRemainder = groupSize + (i < remainder ? 1 : 0);
        std::vector<Neuron*>& targetGroup = (i == 0) ? visionNeurons :
                                           (i == 1) ? touchNeurons :
                                           (i == 2) ? internalNeurons : proprioceptionNeurons;
        
        targetGroup.reserve(targetGroup.size() + groupSizeWithRemainder);
        for (size_t j = 0; j < groupSizeWithRemainder && idx < allSensoryNeurons.size(); ++j, ++idx) {
            targetGroup.push_back(allSensoryNeurons[idx]);
        }
    }
}

// Helper function for motor command activity calculation
static float calculateMotorActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

// ================================================
// AGENT BRAIN CONSTRUCTOR - IMPROVED
// ================================================

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
    , sensoryNoveltyDecay_(AgentBrainConstants::NoveltyDecayRate)
{
    // Initialize motor and sensory neuron groups efficiently
    if (brain_) {
        std::vector<Neuron*> allMotorNeurons;
        std::vector<Neuron*> allSensoryNeurons;
        
        // First pass: collect all neurons by type
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                for (Neuron* n : pop->getNeurons()) {
                    if (type == NeuronType::Motor) {
                        allMotorNeurons.push_back(n);
                    } else if (type == NeuronType::Sensory) {
                        allSensoryNeurons.push_back(n);
                    }
                }
            }
        }
        
        // Use helper functions for efficient categorization
        categorizeMotorNeurons(motorForward_, motorBackward_, motorTurnLeft_, 
                              motorTurnRight_, motorInteract_, motorWait_, allMotorNeurons);
        
        categorizeSensoryNeurons(sensoryVision_, sensoryTouch_, sensoryInternal_,
                                sensoryProprioception_, allSensoryNeurons);
        
        NLM_LOG_INFO("AgentBrain initialized with " +
                     std::to_string(allMotorNeurons.size()) + " motor neurons, " +
                     std::to_string(allSensoryNeurons.size()) + " sensory neurons");
    }
}

AgentBrain::~AgentBrain() = default;

// ================================================
// INITIALIZATION - IMPROVED
// ================================================

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(SensoryMotorConstants::VisionNeuronCount, 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// ================================================
// SENSORY PROCESSING - IMPROVED
// ================================================

size_t AgentBrain::getSensoryInputSize() const {
    // Use named constants for sensory input sizes
    return SensoryMotorConstants::VisionNeuronCount + 
           SensoryMotorConstants::TouchNeuronCount +
           SensoryMotorConstants::InternalNeuronCount +
           SensoryMotorConstants::ProprioceptionNeuronCount;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return SensoryMotorConstants::MotorActionCount;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input processing
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Use named constant for scale factor
            float current = vision[i] * SensoryMotorConstants::VisionCurrentScale;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * SensoryMotorConstants::TouchCurrentScale;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            // Center and scale internal signals
            float current = (intern[i] * 2.0f - 1.0f) * SensoryMotorConstants::InternalCurrentScale;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception processing
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            // Center and scale proprioception signals
            float current = (proprio[i] * 2.0f - 1.0f) * SensoryMotorConstants::ProprioceptionCurrentScale;
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
        
        // Normalize novelty
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay novelty
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

// ================================================
// MOTOR COMMAND DECODING - IMPROVED
// ================================================

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration using named constant
    if (curiosityEnabled_ && curiosityLevel_ > NeuromodulationConstants::DefaultCuriosityThreshold) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Use helper function for activity calculation
    float forwardAct = calculateMotorActivity(motorForward_);
    float backwardAct = calculateMotorActivity(motorBackward_);
    float leftAct = calculateMotorActivity(motorTurnLeft_);
    float rightAct = calculateMotorActivity(motorTurnRight_);
    float interactAct = calculateMotorActivity(motorInteract_);
    float waitAct = calculateMotorActivity(motorWait_);
    
    // Find maximum activity using structured approach
    struct MotorCommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    MotorCommandActivity commands[] = {
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
    
    // Only act if there's meaningful activity using named constant
    if (bestActivity < AgentBrainConstants::MotorActivityThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConstants::HighCuriosityLevel) {
        // Higher curiosity = more exploration using named constant
        float exploreChance = curiosityLevel_ * NeuromodulationConstants::DefaultExplorationChance;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command using named constant for max choices
            int maxChoices = AgentBrainConstants::RandomMotorCommandChoices;
            int choice = brain_->getRandomGenerator()->uniformInt(0, maxChoices);
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

// ================================================
// NEUROMODULATION - IMPROVED
// ================================================

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward using named constant
    expectedReward_ = NeuromodulationConstants::ExpectedRewardSmoothing * expectedReward_ +
                     (1.0f - NeuromodulationConstants::ExpectedRewardSmoothing) * reward;
    
    // Dopamine-like signal (based on prediction error) using named constant
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range using named constant
    dopamineLevel_ = std::clamp(dopamineLevel_, -NeuromodulationConstants::DefaultDopamineRange,
                                NeuromodulationConstants::DefaultDopamineRange);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > PlasticityConstants::DefaultWeightChangeThreshold) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace using named constant
                syn->decayEligibilityTrace(AgentBrainConstants::EligibilityTraceDecay);
            }
        }
    }
    
    // Modulate plasticity based on dopamine using named constants
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

// ================================================
// DEVELOPMENT - IMPROVED
// ================================================

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age using named constants
    if (developmentalAge_ < DevelopmentConstants::InitialStageDuration) {  // ~1 minute
        plasticityModifier_ = DevelopmentConstants::InitialPlasticityModifier;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < DevelopmentConstants::CriticalPeriodDuration) {  // ~5 minutes
        plasticityModifier_ = DevelopmentConstants::CriticalPeriodPlasticityModifier;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < DevelopmentConstants::MaturationStageDuration) {  // ~15 minutes
        plasticityModifier_ = DevelopmentConstants::MaturationPlasticityModifier;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = DevelopmentConstants::AdultPlasticityModifier;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = PlasticityConstants::DefaultSynaptogenesisRate * plasticityModifier_;
            float pruneRate = PlasticityConstants::DefaultPruningRate * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

// ================================================
// GETTERS - IMPROVED
// ================================================

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

// ================================================
// RESET - IMPROVED
// ================================================

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
