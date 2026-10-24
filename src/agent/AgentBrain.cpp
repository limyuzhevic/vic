#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include "../cognition/NeuralPlanner.hpp"
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
    , planningEnabled_(false)  // Phase 6: Enable neural planning
    , curiosityExploration_(0.0f)
    , explorationThreshold_(0.4f)
    , explorationCooldown_(0.0f)
    , explorationCooldownTime_(5.0)
{
    // Initialize neural planner (Phase 6: enable planning)
    if (brain_) {
        neuralPlanner_ = std::make_shared<NeuralPlanner>();
        neuralPlanner_->initialize(brain_.get());
        neuralPlanner_->setPlanningDepth(3);  // Look ahead 3 steps
        neuralPlanner_->setCurrentGoal(std::vector<float>(getSensoryInputSize(), 0.5f));  // Neutral goal
    }
    
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
                                    motorInteract_.size() + motorWait_.size() +
                                    motorLookLeft_.size() + motorLookRight_.size();
                        
                        switch (idx % 8) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            case 6: motorLookLeft_.push_back(n); break;
                            case 7: motorLookRight_.push_back(n); break;
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
    return 8;
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
            float current = touch[i] * 3.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& internal = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = internal[i] * 2.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprioception = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioception.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = proprioception[i] * 1.5f;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Update novelty detection
    noveltyLevel_ = 0.0f;
    if (!previousVision_.empty()) {
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            noveltyLevel_ += diff;
        }
        noveltyLevel_ /= std::min(vision.size(), previousVision_.size());
        // Decay novelty over time
        noveltyLevel_ *= sensoryNoveltyDecay_;
    }
    
    // Store for next time
    previousVision_ = vision;
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand defaultDecoded = decodeFromMotorNeurons();
    
    // Try neural planning if enabled
    MotorCommand plannedCommand = selectWithPlanning(previousVision_, defaultDecoded);
    
    // Apply curiosity-based exploration
    MotorCommand finalCommand = plannedCommand;
    if (curiosityEnabled_ && curiosityLevel_ > explorationThreshold_ && 
        explorationCooldown_ <= 0.0f) {
        finalCommand = selectWithCuriosity(finalCommand);
    }
    
    return finalCommand;
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
    float lookLeftAct = calcActivity(motorLookLeft_);
    float lookRightAct = calcActivity(motorLookRight_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct},
        {MotorCommand::LookLeft, lookLeftAct},
        {MotorCommand::LookRight, lookRightAct}
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

MotorCommand AgentBrain::selectWithPlanning(const std::vector<float>& currentState, 
                                         MotorCommand defaultCmd) {
    if (!neuralPlanner_ || !planningEnabled_ || currentState.empty()) {
        return defaultCmd;
    }
    
    try {
        // Get current goal from planning system
        auto goal = neuralPlanner_->getCurrentGoal();
        
        // Update planning with current state
        neuralPlanner_->setCurrentGoal(currentState);
        
        // Plan action based on current state and goal
        ActionType plannedAction = neuralPlanner_->planAction(currentState, 0.5f);  // Target reward = 0.5
        
        // Convert ActionType to MotorCommand
        MotorCommand plannedCommand = convertToMotorCommand(plannedAction);
        
        // Check if planned action is reasonable (not too different from default)
        if (plannedCommand != MotorCommand::Wait && 
            plannedCommand != defaultCmd) {
            
            // Update exploration state based on planning success
            float planConfidence = neuralPlanner_->getPlanningConfidence();
            if (planConfidence > 0.7f) {
                // High confidence = increase exploration
                curiosityExploration_ = std::min(1.0f, curiosityExploration_ + 0.1f);
            }
            
            NLM_LOG_INFO("Neural planning suggested: " + motorCommandToString(plannedAction) + 
                        " (confidence: " + std::to_string(planConfidence) + ")");
            
            return plannedCommand;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Neural planning error: " + std::string(e.what()));
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Update exploration cooldown
    explorationCooldown_ -= 0.1f;
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = brain_->getRandomGenerator()->uniformInt(0, 8);
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::Interact;
                case 5: return MotorCommand::Wait;
                case 6: return MotorCommand::LookLeft;
                case 7: return MotorCommand::LookRight;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::convertToMotorCommand(ActionType action) {
    switch (action) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Wait: return MotorCommand::Wait;
        case ActionType::LookLeft: return MotorCommand::LookLeft;
        case ActionType::LookRight: return MotorCommand::LookRight;
        default: return MotorCommand::Wait;
    }
}

std::string AgentBrain::motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        default: return "Unknown";
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Calculate prediction error
    predictionError_ = predictedReward - reward;
    
    // Dopamine response to prediction error
    dopamineLevel_ = predictionError_ * 0.1f;  // Scale factor
    
    // Limit dopamine range
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
    
    // Update neural planner based on development
    if (neuralPlanner_) {
        // Adjust planning depth based on development stage
        DevelopmentalStage stage = brain_->getDevelopmentalStage();
        size_t planningDepth = 3;
        if (stage == DevelopmentalStage::Initial) {
            planningDepth = 5;  // More exploration in early stages
        } else if (stage == DevelopmentalStage::Adult) {
            planningDepth = 2;  // More exploitation in adult stages
        }
        neuralPlanner_->setPlanningDepth(planningDepth);
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
    curiosityExploration_ = 0.0f;
    explorationCooldown_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

} // namespace nlm