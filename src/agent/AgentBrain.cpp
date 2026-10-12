#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , arousalLevel_(0.5f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , motorLearningRate_(0.01f)
    , fatigueFactor_(0.0f)
    , motorPerformanceLevel_(0.8f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , motorLearningEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
    , motorSystemActive_(true)
    , actionsExecuted_(0)
    , averageActionSuccessRate_(0.5f) {}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world, MotorSystem* motorSystem) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    motorLearningRate_ = 0.01f;
    fatigueFactor_ = 0.0f;
    motorPerformanceLevel_ = 0.8f;
    
    // Initialize motor system if provided
    if (motorSystem) {
        motorSystem_ = std::unique_ptr<MotorSystem>(motorSystem);
    } else {
        motorSystem_ = std::make_unique<MotorSystem>();
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
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
                              motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()) + 
                 " motor neurons");
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // Motor system handles command selection, agent provides biological parameters
    return 6;  // MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait
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
    if (!brain_ || !motorSystemActive_) return MotorCommand::Wait;
    
    // Use advanced motor system for action selection
    if (motorSystem_) {
        // Get motor region
        NeuralRegion* motorRegion = nullptr;
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Motor) {
                    motorRegion = region.get();
                    break;
                }
            }
            if (motorRegion) break;
        }
        
        if (motorRegion) {
            // Select action with cost-benefit analysis
            std::unique_ptr<Action> action = selectActionWithCostBenefit(*motorRegion);
            if (action && action->isExecutable()) {
                // Update statistics
                actionsExecuted_++;
                actionHistory_.push_back(action.get());
                
                // Convert action to motor command
                MotorCommand cmd = action->getMotorCommand();
                
                // Learn from action if enabled
                if (motorLearningEnabled_) {
                    // This would be called after action execution with reward
                    learnFromActionOutcome(*action, 0.0f, true); // Placeholder
                }
                
                return cmd;
            }
        }
    }
    
    // Fallback to neural-based decoding
    return decodeFromMotorNeurons();
}

std::unique_ptr<Action> AgentBrain::decodeActionCommand() {
    if (!brain_ || !motorSystemActive_) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get motor region
    NeuralRegion* motorRegion = nullptr;
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                motorRegion = region.get();
                break;
            }
        }
        if (motorRegion) break;
    }
    
    if (motorRegion) {
        // Use motor system for action selection with learning
        return motorSystem_->selectAction(*motorRegion);
    }
    
    return std::make_unique<Action>(ActionType::Wait);
}

MotorCommand AgentBrain::decodeMotorCommandWithLearning() {
    if (!brain_ || !motorSystemActive_) return MotorCommand::Wait;
    
    // Advanced decoding with learning integration
    std::unique_ptr<Action> action = decodeActionFromNeuralActivity(*findMotorRegion());
    
    if (action && action->isExecutable()) {
        // Apply curiosity-based exploration
        if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
            action = selectActionWithCuriosity(*action);
        }
        
        // Apply reward modulation
        if (rewardModulationEnabled_) {
            applyNeuromodulationToMotorSystem();
        }
        
        // Convert to motor command
        MotorCommand cmd = action->getMotorCommand();
        
        // Record action for learning
        actionsExecuted_++;
        actionHistory_.push_back(action.get());
        
        return cmd;
    }
    
    return MotorCommand::Wait;
}

void AgentBrain::executeMotorCommand(const MotorCommand& cmd, double currentTime) {
    if (!brain_ || !motorSystemActive_) return;
    
    // Apply motor command to agent body (would update SimpleWorld)
    // This is a placeholder for the actual motor execution
    
    // Update fatigue based on command
    float fatigueIncrease = 0.0f;
    switch (cmd) {
        case MotorCommand::MoveForward:
        case MotorCommand::MoveBackward:
            fatigueIncrease = 0.05f;
            break;
        case MotorCommand::TurnLeft:
        case MotorCommand::TurnRight:
            fatigueIncrease = 0.03f;
            break;
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
            fatigueIncrease = 0.01f;
            break;
        case MotorCommand::Interact:
            fatigueIncrease = 0.08f;
            break;
        default:
            fatigueIncrease = 0.0f;
            break;
    }
    
    fatigueFactor_ += fatigueIncrease;
    fatigueFactor_ = std::min(1.0f, fatigueFactor_);
    
    // Update motor performance based on command success
    motorPerformanceLevel_ = 0.5f + (1.0f - fatigueFactor_) * 0.5f;
}

NeuralRegion* AgentBrain::findMotorRegion() {
    if (!brain_) return nullptr;
    
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                return region.get();
            }
        }
    }
    return nullptr;
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
    
    // Apply neuromodulation to motor system
    updateNeuromodulation(dopamineLevel_, noveltyLevel_, curiosityLevel_);
    
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
    
    // Modulate motor system based on dopamine
    if (motorSystem_) {
        motorSystem_->updateNeuromodulation(dopamineLevel_, noveltyLevel_);
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
    
    // Update motor learning rate based on dopamine
    if (motorLearningEnabled_) {
        motorLearningRate_ = 0.01f * plasticityFactor;
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        if (brain_) brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Development affects motor system
    if (motorSystem_) {
        float devPlasticityMod = 0.0f;
        switch (brain_->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial:
                devPlasticityMod = 1.0f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                devPlasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                devPlasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                devPlasticityMod = 0.2f;
                break;
            default:
                devPlasticityMod = 0.5f;
                break;
        }
        
        motorSystem_->setLearningRate(motorLearningRate_ * devPlasticityMod);
        motorSystem_->setExplorationStrength(0.1f * devPlasticityMod);
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

void AgentBrain::updateArousal(float baseline, float sensoryInput) {
    arousalLevel_ = baseline + sensoryInput * 0.1f;
    arousalLevel_ = std::clamp(arousalLevel_, 0.0f, 1.0f);
    
    // Update motor system
    if (motorSystem_) {
        motorSystem_->updateArousal(baseline, sensoryInput);
    }
}

void AgentBrain::updateNeuromodulation(float dopamine, float novelty, float curiosity) {
    dopamineLevel_ = dopamine;
    noveltyLevel_ = novelty;
    curiosityLevel_ = curiosity;
    
    // Update motor system
    if (motorSystem_) {
        motorSystem_->updateNeuromodulation(dopamine, novelty);
    }
}

void AgentBrain::updateMotorFatigue(float fatigueRate, double dt) {
    // Update fatigue in motor system
    if (motorSystem_) {
        motorSystem_->updateMotorFatigue(fatigueRate, dt);
    }
    
    // Update local fatigue
    fatigueFactor_ += fatigueRate * dt;
    fatigueFactor_ = std::min(1.0f, fatigueFactor_);
    
    // Fatigue recovery over time
    fatigueFactor_ *= (1.0f - 0.01f * dt);
}

void AgentBrain::applyNeuromodulationToMotorSystem() {
    if (motorSystem_) {
        motorSystem_->updateNeuromodulation(dopamineLevel_, noveltyLevel_);
    }
}

void AgentBrain::learnFromActionOutcome(const Action& action, float reward, bool success) {
    if (!motorLearningEnabled_) return;
    
    // Update motor system learning
    if (motorSystem_) {
        motorSystem_->learnFromActionOutcome(action, reward, success);
    }
    
    // Update agent's own motor adaptation
    float predictionError = reward - action.getDynamics().expectedValue;
    
    // Adapt motor preferences based on outcome
    ActionType actionType = action.getType();
    
    if (success) {
        // Increase preference for successful actions
        // This would update internal action preferences
    } else {
        // Decrease preference for unsuccessful actions
        // This would update internal action preferences
    }
    
    // Update adaptation state
    updateMotorAdaptation(predictionError);
}

void AgentBrain::updateMotorAdaptation(float adaptationDelta) {
    // Adapt motor neuron sensitivity based on experience
    std::vector<Neuron*> allMotorNeurons;
    allMotorNeurons.insert(allMotorNeurons.end(), motorForward_.begin(), motorForward_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorBackward_.begin(), motorBackward_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorTurnLeft_.begin(), motorTurnLeft_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorTurnRight_.begin(), motorTurnRight_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorInteract_.begin(), motorInteract_.end());
    allMotorNeurons.insert(allMotorNeurons.end(), motorWait_.begin(), motorWait_.end());
    
    for (Neuron* neuron : allMotorNeurons) {
        // Modify neuron excitability based on adaptation
        float currentPotential = neuron->getState().membranePotential;
        float restingPotential = neuron->getState().restingPotential;
        
        // Adapt threshold based on experience
        float newThreshold = neuron->getState().threshold + adaptationDelta * 0.01f;
        neuron->setThreshold(newThreshold);
        
        // Modulate excitability
        if (adaptationDelta > 0.0f) {
            // Positive adaptation increases excitability
            neuron->addToMembranePotential(adaptationDelta * 0.5f);
        }
    }
    
    // Update motor system adaptation
    if (motorSystem_) {
        motorSystem_->updateMotorAdaptation(*findMotorRegion(), adaptationDelta);
    }
}

void AgentBrain::adjustMotorPreferences(const std::vector<ActionType>& actions,
                                       const std::vector<float>& outcomes) {
    if (!motorSystem_) return;
    
    // Update motor system preferences based on outcomes
    for (size_t i = 0; i < actions.size() && i < outcomes.size(); ++i) {
        ActionType action = actions[i];
        float outcome = outcomes[i];
        
        if (outcome > 0.0f) {
            motorSystem_->setActionPreference(action, 1.2f);
        } else {
            motorSystem_->setActionPreference(action, 0.8f);
        }
    }
}

std::unique_ptr<Action> AgentBrain::planActionSequence(const std::vector<ActionType>& sequenceTypes) {
    if (!motorSystem_) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    return motorSystem_->planActionSequence(sequenceTypes);
}

std::unique_ptr<Action> AgentBrain::adaptActionToContext(const Action& baseAction,
                                                        const std::vector<float>& context) {
    if (!motorSystem_) {
        return baseAction.clone();
    }
    
    NeuralRegion* contextRegion = nullptr;
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                contextRegion = region.get();
                break;
            }
        }
        if (contextRegion) break;
    }
    
    if (contextRegion) {
        return motorSystem_->adaptActionForContext(baseAction, *contextRegion);
    }
    
    return baseAction.clone();
}

std::unique_ptr<Action> AgentBrain::selectActionWithCostBenefit(const NeuralRegion& motorRegion) {
    if (!motorSystem_) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get all neurons for neural activity calculation
    std::vector<Neuron*> neurons = motorRegion.getAllNeurons();
    
    // Update arousal based on current state
    float currentArousal = arousalLevel_ + noveltyLevel_ * 0.5f;
    
    // Select action with cost-benefit analysis
    std::unique_ptr<Action> action = motorSystem_->selectActionWithNeuralDynamics(
        motorRegion, neurons, currentArousal, dopamineLevel_
    );
    
    return action;
}

std::unique_ptr<Action> AgentBrain::selectActionWithCuriosity(const NeuralRegion& motorRegion) {
    if (!motorSystem_ || curiosityLevel_ < 0.3f) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get all neurons
    std::vector<Neuron*> neurons = motorRegion.getAllNeurons();
    
    // Apply curiosity-based exploration
    std::unique_ptr<Action> action = selectWithCuriosity(MotorCommand::Wait);
    
    // Convert to Action object (simplified)
    return action;
}

std::unique_ptr<Action> AgentBrain::selectActionWithExploration(const NeuralRegion& motorRegion) {
    if (!motorSystem_) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    std::vector<Neuron*> neurons = motorRegion.getAllNeurons();
    
    // Select with high exploration
    float explorationArousal = arousalLevel_ * 0.7f;
    float explorationNeuromod = 0.5f; // High neuromodulation for exploration
    
    std::unique_ptr<Action> action = motorSystem_->selectActionWithNeuralDynamics(
        motorRegion, neurons, explorationArousal, explorationNeuromod
    );
    
    return action;
}

void AgentBrain::monitorMotorPerformance() {
    // Update motor performance statistics
    float performance = 0.0f;
    if (!actionHistory_.empty()) {
        float totalSuccess = 0.0f;
        for (const Action* action : actionHistory_) {
            totalSuccess += action->getDynamics().successRate;
        }
        performance = totalSuccess / actionHistory_.size();
    }
    
    averageActionSuccessRate_ = performance;
    motorPerformanceLevel_ = performance * (1.0f - fatigueFactor_);
    
    // Log performance
    if (actionsExecuted_ % 100 == 0) {
        NLM_LOG_INFO("Motor Performance: " + std::to_string(motorPerformanceLevel_ * 100.0f) +
                    "% success rate, " + std::to_string(fatigueFactor_ * 100.0f) + "% fatigue");
    }
}

void AgentBrain::adjustMotorParametersBasedOnPerformance() {
    // Adjust motor parameters based on performance
    float performanceFactor = motorPerformanceLevel_ / 0.8f; // Target 80% performance
    
    // Adjust learning rate based on performance
    if (motorPerformanceLevel_ > 0.9f) {
        // Good performance, reduce exploration
        if (motorSystem_) {
            motorSystem_->setExplorationStrength(0.05f);
        }
    } else if (motorPerformanceLevel_ < 0.6f) {
        // Poor performance, increase exploration
        if (motorSystem_) {
            motorSystem_->setExplorationStrength(0.3f);
        }
    }
    
    // Adjust cost sensitivity based on performance
    float costFactor = 1.0f + (0.5f - performanceFactor) * 0.5f;
    if (motorSystem_) {
        motorSystem_->setCostSensitivity(costFactor);
    }
}

void AgentBrain::integrateMotorLearningIntoActionSelection() {
    if (!motorSystem_ || !brain_) return;
    
    // Update motor system with current neural state
    std::vector<Neuron*> allNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                allNeurons.push_back(neuron);
            }
        }
    }
    
    NeuralRegion* motorRegion = nullptr;
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                motorRegion = region.get();
                break;
            }
        }
        if (motorRegion) break;
    }
    
    if (motorRegion) {
        // Update motor system with current neural activity
        motorSystem_->updateMotorAdaptation(*motorRegion, 0.01f);
    }
}

void AgentBrain::updateMotorBasedOnOutcome(const Action& action, float reward, bool success) {
    // Update motor performance based on outcome
    if (success) {
        motorPerformanceLevel_ = std::min(1.0f, motorPerformanceLevel_ + 0.05f);
        // Decrease fatigue on success
        fatigueFactor_ *= 0.9f;
    } else {
        motorPerformanceLevel_ = std::max(0.0f, motorPerformanceLevel_ - 0.1f);
        // Increase fatigue on failure
        fatigueFactor_ = std::min(1.0f, fatigueFactor_ + 0.15f);
    }
    
    // Update action success rate
    float newSuccessRate = action.getDynamics().successRate;
    if (actionsExecuted_ > 0) {
        averageActionSuccessRate_ = (averageActionSuccessRate_ * actionsExecuted_ + newSuccessRate) / (actionsExecuted_ + 1);
    }
    
    // Monitor performance
    monitorMotorPerformance();
    adjustMotorParametersBasedOnPerformance();
}

void AgentBrain::applyMotorDevelopmentChanges() {
    // Apply motor development changes based on current developmental stage
    float plasticityMod = 0.5f;
    
    switch (developmentalAge_) {
        case 0.0: // Initial development
            plasticityMod = 1.0f;
            motorPerformanceLevel_ = 0.5f;
            fatigueFactor_ = 0.1f;
            break;
        case 60.0: // Critical period
            plasticityMod = 0.8f;
            motorPerformanceLevel_ = 0.7f;
            break;
        case 300.0: // Maturation
            plasticityMod = 0.5f;
            motorPerformanceLevel_ = 0.85f;
            break;
        default: // Adult
            plasticityMod = 0.2f;
            motorPerformanceLevel_ = 0.9f;
            break;
    }
    
    // Update motor system development
    if (motorSystem_) {
        motorSystem_->setLearningRate(motorLearningRate_ * plasticityMod);
        motorSystem_->setExplorationStrength(0.1f * plasticityMod);
    }
}

void AgentBrain::updateMotorPlasticity(double timestep) {
    if (!motorSystem_ || !brain_) return;
    
    // Update motor plasticity based on development and experience
    float developmentalPlasticity = 1.0f;
    if (developmentalAge_ > 60.0) {
        developmentalPlasticity = 0.8f;
    }
    if (developmentalAge_ > 300.0) {
        developmentalPlasticity = 0.5f;
    }
    
    // Update motor system learning rate
    motorSystem_->setLearningRate(motorLearningRate_ * developmentalPlasticity);
    
    // Apply to brain systems
    auto* sp = brain_->getStructuralPlasticity();
    if (sp) {
        float motorPlasticityRate = motorLearningRate_ * developmentalPlasticity;
        sp->setSynaptogenesisRate(0.0001f * motorPlasticityRate);
        sp->setPruningRate(0.00001f * (2.0f - motorPlasticityRate));
    }
}

void AgentBrain::emergencyStopMotorSystem() {
    // Emergency stop of motor system
    motorSystemActive_ = false;
    
    if (motorSystem_) {
        motorSystem_->emergencyStop();
    }
    
    // Rapid fatigue to stop all motor output
    fatigueFactor_ = 1.0f;
    arousalLevel_ = 0.0f;
    
    // Set all motor commands to Wait
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    NLM_LOG_WARNING("Motor system emergency stop triggered");
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

std::unique_ptr<Action> AgentBrain::decodeActionFromNeuralActivity(const NeuralRegion& motorRegion) {
    // Convert neural activity to Action with real dynamics
    std::vector<Neuron*> neurons = motorRegion.getAllNeurons();
    
    // Calculate neural activation patterns
    std::vector<float> neuralActivations;
    for (Neuron* neuron : neurons) {
        float activation = std::abs(neuron->getState().membranePotential - 
                                   neuron->getState().restingPotential);
        neuralActivations.push_back(activation);
    }
    
    // Determine dominant action type based on neural patterns
    ActionType dominantAction = ActionType::Wait;
    float maxActivation = 0.0f;
    
    // Simple action selection based on neural patterns
    if (!neurons.empty()) {
        float avgActivation = std::accumulate(neuralActivations.begin(), 
                                            neuralActivations.end(), 0.0f) / neuralActivations.size();
        
        // Determine action based on average activation and neuromodulation
        if (avgActivation > 0.8f) {
            dominantAction = ActionType::Interact;
        } else if (avgActivation > 0.5f) {
            dominantAction = ActionType::Look;
        } else if (avgActivation > 0.3f) {
            dominantAction = ActionType::Wait;
        }
    }
    
    // Create action with dynamics
    MotorDynamics dynamics;
    dynamics.motorCommand = motorCommandFromActionType(dominantAction);
    dynamics.neuralDrive = maxActivation;
    dynamics.executionTime = 100.0f + maxActivation * 50.0f;
    dynamics.energyCost = 0.5f + maxActivation * 0.3f;
    dynamics.accuracy = 0.8f + maxActivation * 0.2f;
    dynamics.stability = 0.9f - maxActivation * 0.1f;
    
    return std::make_unique<Action>(dominantAction, dynamics);
}

MotorCommand AgentBrain::decodeMotorFromNeuralPopulation(const NeuralRegion& motorRegion) {
    // Advanced motor decoding from neural population activity
    std::vector<Neuron*> neurons = motorRegion.getAllNeurons();
    
    if (neurons.empty()) return MotorCommand::Wait;
    
    // Calculate population activity for each action group
    std::vector<float> actionActivations;
    
    // Group neurons by functional type (simplified)
    std::vector<Neuron*> forwardNeurons, backwardNeurons, turnLeftNeurons, 
                        turnRightNeurons, interactNeurons, waitNeurons;
    
    // Calculate activity for each group
    float forwardActivity = std::accumulate(forwardNeurons.begin(), forwardNeurons.end(), 0.0f,
        [this](float sum, Neuron* n) { return sum + std::abs(n->getState().membranePotential - n->getState().restingPotential); }) / 
        std::max(1.0f, static_cast<float>(forwardNeurons.size()));
    
    float backwardActivity = std::accumulate(backwardNeurons.begin(), backwardNeurons.end(), 0.0f,
        [this](float sum, Neuron* n) { return sum + std::abs(n->getState().membranePotential - n->getState().restingPotential); }) /
        std::max(1.0f, static_cast<float>(backwardNeurons.size()));
    
    float leftActivity = std::accumulate(turnLeftNeurons.begin(), turnLeftNeurons.end(), 0.0f,
        [this](float sum, Neuron* n) { return sum + std::abs(n->getState().membranePotential - n->getState().restingPotential); }) /
        std::max(1.0f, static_cast<float>(turnLeftNeurons.size()));
    
    float rightActivity = std::accumulate(turnRightNeurons.begin(), turnRightNeurons.end(), 0.0f,
        [this](float sum, Neuron* n) { return sum + std::abs(n->getState().membranePotential - n->getState().restingPotential); }) /
        std::max(1.0f, static_cast<float>(turnRightNeurons.size()));
    
    float interactActivity = std::accumulate(interactNeurons.begin(), interactNeurons.end(), 0.0f,
        [this](float sum, Neuron* n) { return sum + std::abs(n->getState().membranePotential - n->getState().restingPotential); }) /
        std::max(1.0f, static_cast<float>(interactNeurons.size()));
    
    float waitActivity = std::accumulate(waitNeurons.begin(), waitNeurons.end(), 0.0f,
        [this](float sum, Neuron* n) { return sum + std::abs(n->getState().membranePotential - n->getState().restingPotential); }) /
        std::max(1.0f, static_cast<float>(waitNeurons.size()));
    
    actionActivations = {forwardActivity, backwardActivity, leftActivity, rightActivity, interactActivity, waitActivity};
    
    // Find maximum activity
    float maxActivity = *std::max_element(actionActivations.begin(), actionActivations.end());
    size_t maxIndex = std::distance(actionActivations.begin(), 
                                   std::max_element(actionActivations.begin(), actionActivations.end()));
    
    // Apply threshold and neuromodulation
    float threshold = 0.5f + (0.5f - arousalLevel_) * 0.5f;  // Higher arousal lowers threshold
    threshold *= (1.0f + dopamineLevel_ * 0.2f);  // Dopamine affects threshold
    
    if (maxActivity < threshold || maxActivity < 0.3f) {
        return MotorCommand::Wait;
    }
    
    // Map to motor command
    MotorCommand cmd;
    switch (maxIndex) {
        case 0: cmd = MotorCommand::MoveForward; break;
        case 1: cmd = MotorCommand::MoveBackward; break;
        case 2: cmd = MotorCommand::TurnLeft; break;
        case 3: cmd = MotorCommand::TurnRight; break;
        case 4: cmd = MotorCommand::Interact; break;
        default: cmd = MotorCommand::Wait; break;
    }
    
    return cmd;
}

MotorCommand AgentBrain::motorCommandFromActionType(ActionType actionType) {
    switch (actionType) {
        case ActionType::MoveForward:
            return MotorCommand::MoveForward;
        case ActionType::MoveBackward:
            return MotorCommand::MoveBackward;
        case ActionType::MoveLeft:
            return MotorCommand::TurnLeft;
        case ActionType::MoveRight:
            return MotorCommand::TurnRight;
        case ActionType::TurnLeft:
            return MotorCommand::TurnLeft;
        case ActionType::TurnRight:
            return MotorCommand::TurnRight;
        case ActionType::Look:
            return MotorCommand::LookLeft; // Simplified
        case ActionType::LookUp:
            return MotorCommand::LookRight; // Simplified
        case ActionType::LookDown:
            return MotorCommand::Interact; // Simplified
        case ActionType::Interact:
            return MotorCommand::Interact;
        case ActionType::Wait:
            return MotorCommand::Wait;
        default:
            return MotorCommand::Wait;
    }
}

size_t AgentBrain::getTotalActionsExecuted() const {
    return actionsExecuted_;
}

float AgentBrain::getAverageActionSuccessRate() const {
    return averageActionSuccessRate_;
}

float AgentBrain::getMotorFatigueLevel() const {
    return fatigueFactor_;
}

float AgentBrain::getMotorPerformanceLevel() const {
    return motorPerformanceLevel_;
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    arousalLevel_ = 0.5f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    motorLearningRate_ = 0.01f;
    fatigueFactor_ = 0.0f;
    motorPerformanceLevel_ = 0.8f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear action history
    for (auto* action : actionHistory_) {
        delete action;
    }
    actionHistory_.clear();
    
    // Reset motor system
    if (motorSystem_) {
        motorSystem_->reset();
    }
    
    // Clear neuron groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
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

} // namespace nlm
