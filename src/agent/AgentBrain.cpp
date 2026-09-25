#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding

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
    , neuralPlanner_(new NeuralPlanner())
    , selfModel_(new SelfModel())
    , socialLearning_(new SocialLearning())
{
    // Initialize cognitive systems
    neuralPlanner_ = std::make_unique<NeuralPlanner>();
    selfModel_ = std::make_unique<SelfModel>();
    socialLearning_ = std::make_unique<SocialLearning>();
    predictionSystem_ = nullptr;  // Will be set via connectPredictionSystem
    
    // Initialize temporary state
    currentGoal_.clear();
    lastPlannedAction_ = ActionType::Wait;
    currentVisionAfterAction_.clear();
    
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
    
    // === COGNITION SYSTEMS INTEGRATION ===
    
    // Get current neural activity as state vector for cognition systems
    std::vector<float> currentState = vision;
    if (!touch.empty()) {
        currentState.insert(currentState.end(), touch.begin(), touch.end());
    }
    if (!intern.empty()) {
        currentState.insert(currentState.end(), intern.begin(), intern.end());
    }
    if (!proprio.empty()) {
        currentState.insert(currentState.end(), proprio.begin(), proprio.end());
    }
    
    // Update neural planner based on current state and goal
    updateNeuralPlanner(currentState);
    
    // Record sensory input for self-model learning
    // Store before state for self-model tracking
    std::vector<float> beforeState = currentState;
    
    // Update self-model with new sensory experience
    // (Self-model learning happens after actions are executed)
    
    // === END COGNITION SYSTEMS INTEGRATION ===
    
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
    
    // Reset cognitive systems
    if (neuralPlanner_) {
        neuralPlanner_->clearCache();
        neuralPlanner_->setCurrentGoal(std::vector<float>());
    }
    if (selfModel_) {
        selfModel_->clear();
    }
    if (socialLearning_) {
        socialLearning_->clear();
    }
}

void AgentBrain::updateNeuralPlanner(const std::vector<float>& currentState) {
    if (!neuralPlanner_ || !brain_) return;
    
    // Update neural planner with current state
    // The neural planner uses prediction system to evaluate action sequences
    
    // If we have a prediction system, use it for planning
    auto* predictionSystem = brain_->getPredictionSystem();
    if (predictionSystem) {
        // Create a simple sensory input from current state
        // This is a simplified approach - real implementation would use
        // the brain's sensory input system
        
        // Set prediction system's current state (simplified)
        // Update planning based on prediction accuracy
        float predictionError = predictionSystem->getPredictionError();
        
        // Adjust planning depth based on prediction error
        if (predictionError > 0.5f) {
            // High prediction error = explore more
            neuralPlanner_->setPlanningDepth(std::min(neuralPlanner_->getPlanningDepth() + 1, 10));
            neuralPlanner_->setPlanningConfidence(0.3f);
        } else {
            // Good prediction = plan more deeply
            neuralPlanner_->setPlanningDepth(std::max(1u, neuralPlanner_->getPlanningDepth() - 1));
            neuralPlanner_->setPlanningConfidence(0.8f);
        }
        
        // Update neural planner's current goal based on prediction system
        std::vector<float> goalState = currentState;
        if (!goalState.empty()) {
            // Set a simple goal state (target approach)
            neuralPlanner_->setCurrentGoal(goalState);
        }
    }
    
    // Plan next action based on current state
    ActionType plannedAction = neuralPlanner_->planAction(currentState);
    
    // If planning succeeded, update action quality
    if (plannedAction != ActionType::Wait) {
        neuralPlanner_->setActionQuality(plannedAction, 0.8f);
    }
}

void AgentBrain::updateSelfModel(const std::vector<float>& beforeState, 
                                const std::vector<float>& afterState,
                                ActionType action) {
    if (!selfModel_) return;
    
    // Update self-model with action consequences
    selfModel_->recordSelfAction(action, beforeState, afterState);
    
    // Predict next state using self-model
    std::vector<float> predictedState = selfModel_->predictActionConsequence(action, beforeState);
    
    // Update self-model with prediction error
    selfModel_->updateSelfModel(predictedState, afterState, action);
    
    // Update capability level based on prediction accuracy
    float confidence = selfModel_->getSelfModelConfidence(action);
    // Self-model capability is a running average of confidence
    static float capabilityAccumulator = 0.0f;
    static size_t capabilityCount = 0;
    capabilityAccumulator += confidence;
    capabilityCount++;
    if (capabilityCount > 0) {
        float newCapability = capabilityAccumulator / capabilityCount;
        // Update neural planner capability level
        if (neuralPlanner_) {
            // This affects planning confidence
            float planningConf = neuralPlanner_->getPlanningConfidence();
            neuralPlanner_->setPlanningConfidence(planningConf * 0.5f + newCapability * 0.5f);
        }
    }
}

void AgentBrain::recordObservation(ActionType observedAction,
                                   const std::vector<float>& observerState,
                                   const std::vector<float>& resultingState) {
    if (!socialLearning_) return;
    
    // Record observation of other agent's action and effect
    socialLearning_->observeAgentAction(observedAction, observerState, resultingState);
    
    // Update social knowledge based on observation
    bool canImitate = socialLearning_->canImitate(observedAction);
    
    }
    
    return defaultCmd;
}

// Connection methods for cognition systems

void AgentBrain::connectPredictionSystem(PredictionSystem* predictionSystem) {
    predictionSystem_ = predictionSystem;
    if (predictionSystem_) {
        NLM_LOG_INFO("AgentBrain connected to PredictionSystem");
    }
}

void AgentBrain::connectNeuralPlanner(NeuralPlanner* planner) {
    // Note: neuralPlanner_ is owned by AgentBrain, not set via this method
    // This method is kept for API compatibility
    if (planner) {
        NLM_LOG_INFO("AgentBrain connected to NeuralPlanner");
    }
}

void AgentBrain::connectSelfModel(SelfModel* selfModel) {
    // Note: selfModel_ is owned by AgentBrain, not set via this method
    // This method is kept for API compatibility
    if (selfModel) {
        NLM_LOG_INFO("AgentBrain connected to SelfModel");
    }
}

void AgentBrain::connectSocialLearning(SocialLearning* socialLearning) {
    // Note: socialLearning_ is owned by AgentBrain, not set via this method
    // This method is kept for API compatibility
    if (socialLearning) {
        NLM_LOG_INFO("AgentBrain connected to SocialLearning");
    }
}

void AgentBrain::updateCognitionSystems() {
    if (!brain_) return;
    
    // Update neural planner with current state
    if (neuralPlanner_ && curiosityEnabled_) {
        // Get current goal state (simplified - in real implementation would extract from brain)
        std::vector<float> currentState(256, 0.0f);
        for (size_t i = 0; i < std::min(currentState.size(), previousVision_.size()); ++i) {
            currentState[i] = previousVision_[i];
        }
        
        // Set current goal based on accumulated experience
        float maxValue = 0.0f;
        for (float v : currentState) {
            maxValue = std::max(maxValue, v);
        }
        std::vector<float> goal = currentState;
        for (float& g : goal) {
            g = maxValue * 0.8f; // Approach higher activity levels
        }
        
        neuralPlanner_->setCurrentGoal(goal);
        
        // Plan action based on current state and goal
        ActionType plannedAction = neuralPlanner_->planAction(currentState, 0.5f);
        
        // Store planned action for experience
        lastPlannedAction_ = plannedAction;
    }
    
    // Update self-model with action-consequence experience
    if (selfModel_ && lastAction_.type != ActionType::Wait && lastAction_.type != ActionType::Custom) {
        // Record action and its effect
        selfModel_->recordSelfAction(lastAction_.type, previousVision_, 
                                    currentVisionAfterAction_);
    }
    
    // Update social learning from agent's own behavior
    if (socialLearning_ && predictionError_ > 0.5f) {
        // Record own actions as social learning examples
        socialLearning_->observeAgentAction(lastAction_.type, previousVision_, 
                                           currentVisionAfterAction_);
    }
    
    // Update prediction system
    if (predictionSystem_) {
        // Create prediction based on current sensory state
        auto predictedState = predictionSystem_->predictNextState(
            createSensoryInputFromVision(previousVision_));
        
        // This would be updated when world updates and we see the result
        // For now, just record prediction error based on agent's own predictions
        float predError = predictionSystem_->getPredictionError();
        predictionError_ = predError; // Update existing prediction error
    }
    
    // Update planning quality based on actual outcomes
    if (neuralPlanner_ && lastPlannedAction_ != ActionType::Wait && lastAction_.type != ActionType::Wait) {
        // If plan was followed and succeeded, update planner quality
        if (lastPlannedAction_ == lastAction_.type) {
            neuralPlanner_->updatePlanQuality({lastPlannedAction_}, {lastAction_.type}, expectedReward_);
        }
    }
}

void AgentBrain::setGoal(const std::vector<float>& goal) {
    currentGoal_ = goal;
}

std::vector<float> AgentBrain::getCurrentGoal() const {
    return currentGoal_;
}

NeuralPlanner* AgentBrain::getNeuralPlanner() {
    return neuralPlanner_.get();
}

SelfModel* AgentBrain::getSelfModel() {
    return selfModel_.get();
}

SocialLearning* AgentBrain::getSocialLearning() {
    return socialLearning_.get();
}

PredictionSystem* AgentBrain::getPredictionSystem() {
    return predictionSystem_;
}

void AgentBrain::resetCognitionSystems() {
    if (neuralPlanner_) neuralPlanner_->clearCache();
    if (selfModel_) selfModel_->clear();
    if (socialLearning_) socialLearning_->clear();
    if (predictionSystem_) predictionSystem_->clearHistory();
}

// Helper method for creating sensory input from vision data
SensoryInput AgentBrain::createSensoryInputFromVision(const std::vector<float>& visionData) {
    SensoryInput input;
    input.setData(visionData);
    input.setTimestamp(0.0);
    return input;
}

} // namespace nlm