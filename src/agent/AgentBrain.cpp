// AgentBrain.cpp - Refactored implementation using improved structure and constants

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

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
    , sensoryNoveltyDecay_(AgentBrainConstants::DEFAULT_NOVELTY_DECAY)
{
    // Initialize motor and sensory neuron groups using utility function
    if (brain_) {
        initializeMotorGroups(*this, brain_);
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
    return AgentBrainConstants::SIZEOF_VISION + AgentBrainConstants::SIZEOF_TOUCH +
           AgentBrainConstants::SIZEOF_INTERNAL + AgentBrainConstants::SIZEOF_PROPRIOCEPTION;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return AgentBrainConstants::MOTOR_GROUP_COUNT;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Process vision input
    const auto& vision = percept.getVision();
    processVisionInput(vision, sensoryVision_, AgentBrainConstants::VISION_SCALE_FACTOR);
    
    // Process touch input
    const auto& touch = percept.getTouch();
    processTouchInput(touch, sensoryTouch_, AgentBrainConstants::TOUCH_SCALE_FACTOR);
    
    // Process internal signals
    const auto& intern = percept.getInternal();
    processInternalSignals(intern, sensoryInternal_, 
                          AgentBrainConstants::INTERNAL_SIGNAL_CENTER,
                          AgentBrainConstants::INTERNAL_SIGNAL_SCALE);
    
    // Process proprioception
    const auto& proprio = percept.getProprioception();
    processProprioception(proprio, sensoryProprioception_,
                         AgentBrainConstants::PROPRIOCEPTION_CENTER,
                         AgentBrainConstants::PROPRIOCEPTION_SCALE);
    
    // Compute novelty using helper function
    VisionDifference diff{vision, previousVision_, noveltyLevel_, sensoryNoveltyDecay_};
    diff.compute();
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * AgentBrainConstants::CURIOSITY_NOVELTY_WEIGHT +
                         std::abs(predictionError_) * AgentBrainConstants::CURIOSITY_PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = AgentBrainHelpers::clamp(curiosityLevel_, 0.0f, AgentBrainConstants::MAX_CURIOUSITY_LEVEL);
    }
    
    // Store for next time
    previousVision_ = vision;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > AgentBrainConstants::EXPLORATION_THRESHOLD_LOW) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using helper function
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        return AgentBrainHelpers::calculateMotorGroupActivity(neurons);
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Create motor command with activities using structure
    MotorCommandWithActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    // Sort by activity and find best
    std::vector<MotorCommandWithActivity> commandVec(std::begin(commands), std::end(commands));
    std::sort(commandVec.begin(), commandVec.end());
    
    for (const auto& cmd : commandVec) {
        if (cmd.activity > bestActivity) {
            bestActivity = cmd.activity;
            best = cmd.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < AgentBrainConstants::MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > AgentBrainConstants::EXPLORATION_THRESHOLD_HIGH) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * AgentBrainConstants::MAX_EXPLORATION_CHANCE;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command using helper function
            return AgentBrainHelpers::getRandomMotorCommand(brain_);
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error using helper function
    predictionError_ = AgentBrainHelpers::calculatePredictionError(reward, predictedReward);
    
    // Update expected reward using helper function
    expectedReward_ = AgentBrainHelpers::updateExpectedReward(expectedReward_, reward);
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = AgentBrainHelpers::clamp(dopamineLevel_, 
                                             AgentBrainConstants::DOPAMINE_CLAMP_MIN,
                                             AgentBrainConstants::DOPAMINE_CLAMP_MAX);
    
    // Apply to all synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > AgentBrainConstants::ELIGIBILITY_THRESHOLD) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(AgentBrainConstants::ELIGIBILITY_TRACE_DECAY);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = AgentBrainConstants::PLASTICITY_FACTOR_MIN +
                            AgentBrainConstants::DOPAMINE_TO_PLASTICITY_FACTOR * dopamineLevel_;
    plasticityFactor = AgentBrainHelpers::clamp(plasticityFactor,
                                               AgentBrainConstants::PLASTICITY_FACTOR_MIN,
                                               AgentBrainConstants::PLASTICITY_FACTOR_MAX);
    
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
    if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_INITIAL) {  // ~1 minute
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_INITIAL;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_CRITICAL) {  // ~5 minutes
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_CRITICAL;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < AgentBrainConstants::DEVELOPMENT_STAGE_MATURATION) {  // ~15 minutes
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_MATURE;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = AgentBrainConstants::PLASTICITY_ADULT;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = AgentBrainConstants::SYNAPTOGENESIS_BASE_FACTOR * plasticityModifier_ * AgentBrainConstants::PLASTICITY_TO_SYNAPTOGENESIS_FACTOR;
            float pruneRate = AgentBrainConstants::PRUNING_BASE_FACTOR * (2.0f - plasticityModifier_) * AgentBrainConstants::PLASTICITY_TO_PRUNING_FACTOR;
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
