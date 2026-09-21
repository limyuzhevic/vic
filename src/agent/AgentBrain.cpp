#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_set>

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
    , predictionHorizon_(1.0)
    , planningDepth_(3)
    , performanceScore_(0.0f)
    , learningRate_(0.01f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , metaCognitionEnabled_(true)
    , activeInferenceEnabled_(true)
    , sleepConsolidationEnabled_(true)
    , metaLearningEnabled_(false)
    , hierarchicalControlEnabled_(false)
    , socialLearningEnabled_(false)
    , transferLearningEnabled_(false)
    , intrinsicMotivationWeight_(0.5f)
    , sensoryGain_(1.0f)
    , motorScaling_(1.0f)
    , sensoryNoveltyDecay_(0.99f)
    , predictionErrorThreshold_(0.1f)
    , metacognitionLevel_(0.0f)
    , selfMonitoring_(0.0f)
    , confidenceCalibration_(0.0f)
    , isAsleep_(false)
    , sleepStartTime_(0.0)
    , replayCount_(0)
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
    experienceBuffer_.clear();
    beliefs_.clear();
    expectations_.clear();
    neuronSalience_.clear();
    neuronImportance_.clear();
    winners_.clear();
    attendedRegions_.clear();
    skillHierarchy_.clear();
    bottomUpSalience_.clear();
    topDownBias_.clear();
    inhibitedNeurons_.clear();
    excitedNeurons_.clear();
    taskMemories_.clear();
    transferStrength_.clear();
    learningStrategies_.clear();
    strategyEffectiveness_.clear();
    socialRewards_.clear();
    socialValue_.clear();
    
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
    return 8;  // Added LookLeft, LookRight
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity with gain
            float current = vision[i] * 5.0f * sensoryGain_;  // Scale factor
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
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Active inference - update beliefs based on sensory input
    if (activeInferenceEnabled_) {
        updateActiveInference(0.001);  // Small timestep
    }
    
    // Meta-cognition - monitor cognitive processes
    if (metaCognitionEnabled_) {
        updateMetaCognition(0.001);
    }
    
    // Store experience for learning
    std::vector<float> currentState = percept.getAllSignals();
    addToExperienceBuffer(currentState);
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply meta-cognition-based refinement
    if (metaCognitionEnabled_ && metacognitionLevel_ > 0.5f) {
        decoded = selectWithMetaCognition(decoded);
    }
    
    // Apply active inference planning
    if (activeInferenceEnabled_ && isAsleep_ == false) {
        decoded = selectWithActiveInference(decoded);
    }
    
    // Apply hierarchical control if enabled
    if (hierarchicalControlEnabled_) {
        MotorCommand hierarchicalCmd = selectWithHierarchicalControl();
        if (hierarchicalCmd != MotorCommand::Wait) {
            decoded = hierarchicalCmd;
        }
    }
    
    // Apply social learning if enabled
    if (socialLearningEnabled_) {
        MotorCommand socialCmd = selectWithSocialLearning();
        if (socialCmd != MotorCommand::Wait) {
            decoded = socialCmd;
        }
    }
    
    // Apply transfer learning if enabled
    if (transferLearningEnabled_) {
        MotorCommand transferCmd = selectWithTransferLearning();
        if (transferCmd != MotorCommand::Wait) {
            decoded = transferCmd;
        }
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

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
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
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithMetaCognition(MotorCommand defaultCmd) {
    // Meta-cognition: evaluate the quality of the current action
    // and potentially select better alternatives
    
    if (brain_ == nullptr) return defaultCmd;
    
    // Get current neural activity patterns
    float currentActivity = 0.0f;
    for (auto* neuron : motorForward_) {
        currentActivity += std::abs(neuron->getState().membranePotential - 
                                  neuron->getState().restingPotential);
    }
    
    // Compute confidence based on activity level and prediction error
    float confidence = std::min(1.0f, currentActivity * 2.0f - predictionErrorThreshold_);
    
    // If confidence is low, try alternative actions
    if (confidence < 0.3f) {
        // Get all motor neuron activities
        std::vector<std::pair<MotorCommand, float>> cmdActivities;
        
        // Sample a few potential alternatives
        std::vector<MotorCommand> alternatives = {
            MotorCommand::MoveForward, MotorCommand::MoveBackward, 
            MotorCommand::TurnLeft, MotorCommand::TurnRight,
            MotorCommand::Wait
        };
        
        for (auto cmd : alternatives) {
            float activity = 0.0f;
            // Approximate activity for this command based on neural groups
            if (cmd == MotorCommand::MoveForward) {
                activity = calcMotorGroupActivity(motorForward_);
            } else if (cmd == MotorCommand::MoveBackward) {
                activity = calcMotorGroupActivity(motorBackward_);
            } else if (cmd == MotorCommand::TurnLeft) {
                activity = calcMotorGroupActivity(motorTurnLeft_);
            } else if (cmd == MotorCommand::TurnRight) {
                activity = calcMotorGroupActivity(motorTurnRight_);
            } else {
                activity = calcMotorGroupActivity(motorWait_);
            }
            cmdActivities.push_back({cmd, activity});
        }
        
        // Sort by activity level
        std::sort(cmdActivities.begin(), cmdActivities.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Try the best alternative
        if (cmdActivities.size() > 1 && cmdActivities[1].second > 0.1f) {
            return cmdActivities[1].first;
        }
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithActiveInference(MotorCommand defaultCmd) {
    // Active inference: select action that minimizes prediction error
    
    if (!brain_ || !activeInferenceEnabled_) return defaultCmd;
    
    // Get current state estimate and expectations
    if (beliefs_.empty() || expectations_.empty()) {
        return defaultCmd;
    }
    
    // Compute free energy for each potential action
    std::vector<std::pair<MotorCommand, float>> actionFreeEnergies;
    
    // Sample a set of potential actions
    std::vector<MotorCommand> potentialActions = {
        MotorCommand::MoveForward, MotorCommand::MoveBackward,
        MotorCommand::TurnLeft, MotorCommand::TurnRight,
        MotorCommand::LookLeft, MotorCommand::LookRight,
        MotorCommand::Interact, MotorCommand::Wait
    };
    
    for (auto action : potentialActions) {
        // Simulate the outcome of this action
        float predictedError = computeActionPredictionError(action);
        float actionCost = computeActionCost(action);
        
        // Free energy = prediction error + action cost
        float freeEnergy = predictedError + actionCost;
        
        actionFreeEnergies.push_back({action, freeEnergy});
    }
    
    // Select action with minimum free energy
    std::sort(actionFreeEnergies.begin(), actionFreeEnergies.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    // Only select if improvement is significant
    if (actionFreeEnergies.size() > 1 && 
        actionFreeEnergies[0].second < predictionErrorThreshold_ * 0.5f) {
        return actionFreeEnergies[0].first;
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::selectWithHierarchicalControl() {
    // Hierarchical control: select action based on skill proficiency
    
    if (!hierarchicalControlEnabled_) return MotorCommand::Wait;
    
    // Get current motor neuron activities
    std::vector<std::pair<MotorCommand, float>> cmdActivities;
    
    // Map skills to actions
    std::vector<std::pair<std::string, MotorCommand>> skillActionMap = {
        {"movement", MotorCommand::MoveForward},
        {"retreat", MotorCommand::MoveBackward},
        {"rotation", MotorCommand::TurnLeft},
        {"steering", MotorCommand::TurnRight},
        {"exploration", MotorCommand::LookLeft},
        {"investigation", MotorCommand::LookRight},
        {"interaction", MotorCommand::Interact},
        {"waiting", MotorCommand::Wait}
    };
    
    for (const auto& skillAction : skillActionMap) {
        std::string skill = skillAction.first;
        MotorCommand action = skillAction.second;
        
        // Get skill proficiency
        float proficiency = getSkillProficiency(skill);
        
        // Get corresponding neuron group activity
        float activity = 0.0f;
        if (action == MotorCommand::MoveForward) {
            activity = calcMotorGroupActivity(motorForward_);
        } else if (action == MotorCommand::MoveBackward) {
            activity = calcMotorGroupActivity(motorBackward_);
        } else if (action == MotorCommand::TurnLeft) {
            activity = calcMotorGroupActivity(motorTurnLeft_);
        } else if (action == MotorCommand::TurnRight) {
            activity = calcMotorGroupActivity(motorTurnRight_);
        } else if (action == MotorCommand::LookLeft) {
            activity = calcMotorGroupActivity(motorLookLeft_);
        } else if (action == MotorCommand::LookRight) {
            activity = calcMotorGroupActivity(motorLookRight_);
        } else if (action == MotorCommand::Interact) {
            activity = calcMotorGroupActivity(motorInteract_);
        } else {
            activity = calcMotorGroupActivity(motorWait_);
        }
        
        // Weighted score: proficiency * activity
        float score = proficiency * activity;
        cmdActivities.push_back({action, score});
    }
    
    // Select action with highest score
    if (!cmdActivities.empty()) {
        std::sort(cmdActivities.begin(), cmdActivities.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        if (cmdActivities[0].second > 0.1f) {
            return cmdActivities[0].first;
        }
    }
    
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::selectWithSocialLearning() {
    // Social learning: learn from observed behavior of other agents
    
    if (!socialLearningEnabled_) return MotorCommand::Wait;
    
    // Get social rewards and compute action values
    std::vector<std::pair<MotorCommand, float>> actionValues;
    
    // Sample observed actions and their rewards
    std::vector<std::pair<MotorCommand, float>> socialExperiences = {
        {MotorCommand::MoveForward, getSocialValueForAction(MotorCommand::MoveForward)},
        {MotorCommand::MoveBackward, getSocialValueForAction(MotorCommand::MoveBackward)},
        {MotorCommand::TurnLeft, getSocialValueForAction(MotorCommand::TurnLeft)},
        {MotorCommand::TurnRight, getSocialValueForAction(MotorCommand::TurnRight)},
        {MotorCommand::LookLeft, getSocialValueForAction(MotorCommand::LookLeft)},
        {MotorCommand::LookRight, getSocialValueForAction(MotorCommand::LookRight)},
        {MotorCommand::Interact, getSocialValueForAction(MotorCommand::Interact)},
        {MotorCommand::Wait, getSocialValueForAction(MotorCommand::Wait)}
    };
    
    // Filter out low-value actions
    for (const auto& exp : socialExperiences) {
        if (exp.second > 0.1f) {
            actionValues.push_back(exp);
        }
    }
    
    // Select action with highest social value
    if (!actionValues.empty()) {
        std::sort(actionValues.begin(), actionValues.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        return actionValues[0].first;
    }
    
    return MotorCommand::Wait;
}

MotorCommand AgentBrain::selectWithTransferLearning() {
    // Transfer learning: apply knowledge from similar tasks
    
    if (!transferLearningEnabled_) return MotorCommand::Wait;
    
    // Get current context for transfer
    std::string currentContext = getCurrentTaskContext();
    
    // Find similar tasks for transfer
    std::vector<std::pair<std::string, float>> similarTasks;
    for (const auto& taskMemory : taskMemories_) {
        std::string taskName = taskMemory.first;
        float strength = transferStrength_[taskName];
        
        // Compute similarity between current context and task
        float similarity = computeTaskSimilarity(currentContext, taskName);
        
        if (similarity > 0.3f) {
            similarTasks.push_back({taskMemory.first, strength * similarity});
        }
    }
    
    // Select best transferred action
    if (!similarTasks.empty()) {
        std::sort(similarTasks.begin(), similarTasks.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        std::string bestTask = similarTasks[0].first;
        // Apply the successful strategy from the best task
        return applyTransferredStrategy(bestTask);
    }
    
    return MotorCommand::Wait;
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
    
    // Update meta-cognition based on reward prediction error
    if (metaCognitionEnabled_) {
        confidenceCalibration_ += predictionError_ * 0.1f;
        confidenceCalibration_ = std::clamp(confidenceCalibration_, 0.0f, 1.0f);
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
    predictionHorizon_ = 1.0;
    planningDepth_ = 3;
    performanceScore_ = 0.0f;
    learningRate_ = 0.01f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset advanced agent state
    experienceBuffer_.clear();
    beliefs_.clear();
    expectations_.clear();
    neuronSalience_.clear();
    neuronImportance_.clear();
    winners_.clear();
    attendedRegions_.clear();
    skillHierarchy_.clear();
    bottomUpSalience_.clear();
    topDownBias_.clear();
    inhibitedNeurons_.clear();
    excitedNeurons_.clear();
    taskMemories_.clear();
    transferStrength_.clear();
    learningStrategies_.clear();
    strategyEffectiveness_.clear();
    socialRewards_.clear();
    socialValue_.clear();
    
    // Reset sleep state
    isAsleep_ = false;
    sleepStartTime_ = 0.0;
    replayCount_ = 0;
}

// Helper methods
float AgentBrain::calcMotorGroupActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

void AgentBrain::updateMetaCognition(double timestep) {
    // Meta-cognition: monitor cognitive processes and adapt strategies
    
    // Increase metacognition level with experience
    metacognitionLevel_ += 0.001f * timestep;
    metacognitionLevel_ = std::clamp(metacognitionLevel_, 0.0f, 1.0f);
    
    // Self-monitoring: track the quality of decisions
    selfMonitoring_ = confidenceCalibration_ * metacognitionLevel_;
    
    // Adjust strategy effectiveness based on performance
    if (!learningStrategies_.empty()) {
        for (const auto& strategy : learningStrategies_) {
            // Update effectiveness based on recent performance
            float effectiveness = strategyEffectiveness_[strategy];
            effectiveness += performanceScore_ * learningRate_ * timestep;
            effectiveness = std::clamp(effectiveness, 0.0f, 1.0f);
            strategyEffectiveness_[strategy] = effectiveness;
        }
    }
}

void AgentBrain::updateActiveInference(double timestep) {
    // Active inference: maintain beliefs about the world
    // and expect outcomes of actions
    
    if (experienceBuffer_.size() < 2) return;
    
    // Update beliefs based on sensory input
    // This is a simplified version - real active inference would use Bayes' theorem
    size_t stateSize = getSensoryInputSize();
    if (stateSize > 0 && beliefs_.size() == stateSize) {
        // Simple belief update: average recent experiences
        float totalWeight = 0.0f;
        for (size_t i = 0; i < std::min(experienceBuffer_.size(), (size_t)100); ++i) {
            size_t idx = experienceBuffer_.size() - 1 - i;
            for (size_t j = 0; j < stateSize && j < experienceBuffer_[idx].size(); ++j) {
                beliefs_[j] = 0.9f * beliefs_[j] + 0.1f * experienceBuffer_[idx][j];
            }
            totalWeight += 0.9f * i + 0.1f;
        }
        
        // Normalize beliefs
        for (size_t j = 0; j < stateSize && j < beliefs_.size(); ++j) {
            beliefs_[j] /= totalWeight;
        }
    }
    
    // Update expectations for action outcomes
    if (!beliefs_.empty()) {
        expectations_.resize(beliefs_.size());
        for (size_t i = 0; i < expectations_.size(); ++i) {
            // Simple expectation: assume actions will produce similar sensory patterns
            expectations_[i] = beliefs_[i] * 0.8f + 0.2f;  // Small random component
        }
    }
    
    // Sleep and consolidation if needed
    if (sleepConsolidationEnabled_ && isAsleep_) {
        performSleepConsolidation();
    }
}

void AgentBrain::updateHierarchicalControl(double timestep) {
    // Hierarchical control: maintain and update skill hierarchy
    
    if (!hierarchicalControlEnabled_) return;
    
    // Update skill proficiency based on performance
    for (const auto& skill : skillHierarchy_) {
        float proficiency = getSkillProficiency(skill);
        // Improve proficiency with good performance
        proficiency += performanceScore_ * learningRate_ * timestep * 0.5f;
        proficiency = std::clamp(proficiency, 0.0f, 1.0f);
        skillProficiency_[skill] = proficiency;
    }
    
    // Attention selection based on goals and skills
    if (primaryGoal_.empty() == false) {
        // Focus attention on skills relevant to primary goal
        for (const auto& skillAction : skillProficiency_) {
            if (isSkillRelevantToGoal(skillAction.first, primaryGoal_)) {
                applyTopDownBias(skillAction.first, skillAction.second * 0.5f);
            }
        }
    }
}

void AgentBrain::updateMetaLearning(double timestep) {
    // Meta-learning: learn how to learn
    
    if (!metaLearningEnabled_) return;
    
    // Adapt learning rate based on learning progress
    float learningProgress = std::min(1.0f, performanceScore_ * learningRate_);
    learningRate_ = 0.01f * (1.0f - learningProgress);  // Decrease learning rate as we learn
    
    // Select best learning strategy based on effectiveness
    if (!learningStrategies_.empty() && !strategyEffectiveness_.empty()) {
        std::string bestStrategy;
        float bestEffectiveness = 0.0f;
        
        for (const auto& strategy : learningStrategies_) {
            float effectiveness = strategyEffectiveness_[strategy];
            if (effectiveness > bestEffectiveness) {
                bestEffectiveness = effectiveness;
                bestStrategy = strategy;
            }
        }
        
        // Apply the best strategy
        if (bestEffectiveness > 0.5f) {
            // Use this strategy for future learning
            currentMetaLearningStrategy_ = bestStrategy;
        }
    }
}

void AgentBrain::updateTransferLearning(double timestep) {
    // Transfer learning: maintain and update transferred knowledge
    
    if (!transferLearningEnabled_) return;
    
    // Decay transfer strength over time
    for (auto& transfer : transferStrength_) {
        transfer.second *= 0.95f;  // Natural decay of transferred knowledge
    }
    
    // Update task memories with new experiences
    if (!experienceBuffer_.empty()) {
        std::string currentContext = getCurrentTaskContext();
        
        // Store current experience in appropriate task memory
        if (taskMemories_.find(currentContext) == taskMemories_.end()) {
            taskMemories_[currentContext] = std::vector<float>();
        }
        
        // Add to task memory
        taskMemories_[currentContext].insert(taskMemories_[currentContext].end(),
                                             experienceBuffer_.end() - getSensoryInputSize(),
                                             experienceBuffer_.end());
        
        // Limit memory size
        if (taskMemories_[currentContext].size() > 10000) {
            taskMemories_[currentContext].resize(10000);
        }
    }
}

void AgentBrain::updateSocialLearning(double timestep) {
    // Social learning: maintain and update social knowledge
    
    if (!socialLearningEnabled_) return;
    
    // Decay social value over time
    for (auto& social : socialValue_) {
        social.second *= 0.9f;
    }
    
    // Update social rewards based on recent performance
    socialRewards_.push_back(performanceScore_);
    if (socialRewards_.size() > 100) {
        socialRewards_.erase(socialRewards_.begin());
    }
    
    // Compute social value of actions based on observed rewards
    for (auto& social : socialValue_) {
        // Average recent social rewards
        float avgReward = 0.0f;
        if (!socialRewards_.empty()) {
            for (float reward : socialRewards_) {
                avgReward += reward;
            }
            avgReward /= socialRewards_.size();
        }
        
        // Update social value
        social.second = avgReward * 0.5f + social.second * 0.5f;
    }
}

void AgentBrain::performSleepConsolidation() {
    // Sleep consolidation: replay and consolidate memories
    
    if (!sleepConsolidationEnabled_ || !brain_) return;
    
    // Simple consolidation: strengthen important memories
    sleepStartTime_ += 0.001;
    
    // Replay important experiences
    if (replayCount_ < 10 && sleepStartTime_ > 10.0) {
        // Replay some experiences to strengthen them
        if (!experienceBuffer_.empty() && experienceBuffer_.size() >= getSensoryInputSize()) {
            size_t replayIndex = experienceBuffer_.size() - getSensoryInputSize() * 2;
            
            // Extract replay experience
            std::vector<float> replayExperience(
                experienceBuffer_.begin() + replayIndex,
                experienceBuffer_.begin() + replayIndex + getSensoryInputSize()
            );
            
            // Use this experience to update beliefs
            if (replayExperience.size() == beliefs_.size()) {
                for (size_t i = 0; i < replayExperience.size(); ++i) {
                    beliefs_[i] = 0.7f * beliefs_[i] + 0.3f * replayExperience[i];
                }
            }
            
            replayCount_++;
        }
    }
    
    // Wake up after enough consolidation
    if (sleepStartTime_ > 60.0) {
        isAsleep_ = false;
        replayCount_ = 0;
    }
}

void AgentBrain::consolidateMetaMemory() {
    // Meta-memory consolidation: organize and index meta-knowledge
    
    // Simple consolidation: categorize experiences
    if (!experienceBuffer_.empty()) {
        // Extract key features from recent experiences
        std::vector<float> recentFeatures;
        size_t featuresToExtract = std::min((size_t)100, experienceBuffer_.size() / getSensoryInputSize());
        
        for (size_t i = 0; i < featuresToExtract; ++i) {
            size_t idx = experienceBuffer_.size() - 1 - i * getSensoryInputSize();
            if (idx >= getSensoryInputSize()) {
                for (size_t j = 0; j < getSensoryInputSize() && j < experienceBuffer_[idx].size(); ++j) {
                    recentFeatures.push_back(experienceBuffer_[idx][j]);
                }
            }
        }
        
        // Store as meta-knowledge
        if (!recentFeatures.empty()) {
            metaMemory_.push_back(recentFeatures);
            if (metaMemory_.size() > 1000) {
                metaMemory_.erase(metaMemory_.begin());
            }
        }
    }
}

void AgentBrain::consolidateHierarchicalMemory() {
    // Hierarchical memory consolidation: organize skills and strategies
    
    // Update skill hierarchy based on performance
    if (primaryGoal_.empty() == false) {
        // Identify skills that are useful for the primary goal
        for (const auto& skill : skillHierarchy_) {
            if (isSkillRelevantToGoal(skill, primaryGoal_)) {
                // Increase proficiency for useful skills
                float currentProficiency = getSkillProficiency(skill);
                currentProficiency = std::min(1.0f, currentProficiency + performanceScore_ * 0.01f);
                skillProficiency_[skill] = currentProficiency;
            }
        }
    }
}

// Active inference helper methods
float AgentBrain::computeActionPredictionError(MotorCommand action) const {
    // Simple model: prediction error is difference between expected and actual
    // For now, return a dummy value based on action
    float actionValue = 0.0f;
    switch (action) {
        case MotorCommand::MoveForward: actionValue = 0.8f; break;
        case MotorCommand::MoveBackward: actionValue = 0.3f; break;
        case MotorCommand::TurnLeft: actionValue = 0.6f; break;
        case MotorCommand::TurnRight: actionValue = 0.7f; break;
        case MotorCommand::LookLeft: actionValue = 0.4f; break;
        case MotorCommand::LookRight: actionValue = 0.5f; break;
        case MotorCommand::Interact: actionValue = 0.9f; break;
        case MotorCommand::Wait: actionValue = 0.2f; break;
    }
    
    // Return error based on difference from expected value
    return std::abs(predictionError_ - (actionValue - 0.5f));
}

float AgentBrain::computeActionCost(MotorCommand action) const {
    // Simple action cost: higher for complex actions
    switch (action) {
        case MotorCommand::MoveForward:
        case MotorCommand::MoveBackward:
        case MotorCommand::Wait:
            return 0.1f;
        case MotorCommand::TurnLeft:
        case MotorCommand::TurnRight:
            return 0.2f;
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
            return 0.15f;
        case MotorCommand::Interact:
            return 0.3f;
        default:
            return 0.2f;
    }
}

// Hierarchical control helper methods
float AgentBrain::getSkillProficiency(const std::string& skill) const {
    auto it = skillProficiency_.find(skill);
    if (it != skillProficiency_.end()) {
        return it->second;
    }
    return 0.0f;
}

std::string AgentBrain::getCurrentTaskContext() const {
    // Simple task context based on primary goal
    if (!primaryGoal_.empty()) {
        return primaryGoal_;
    }
    return "default";
}

float AgentBrain::computeTaskSimilarity(const std::string& task1, const std::string& task2) const {
    // Simple string similarity
    if (task1 == task2) return 1.0f;
    if (task1.empty() || task2.empty()) return 0.0f;
    
    // Simple check for common substrings
    size_t minLen = std::min(task1.size(), task2.size());
    if (minLen < 3) return 0.0f;
    
    size_t common = 0;
    for (size_t i = 0; i < task1.size() - 2; ++i) {
        if (task1.substr(i, 3) == task2.substr(i, 3)) {
            common += 3;
        }
    }
    
    return static_cast<float>(common) / minLen;
}

MotorCommand AgentBrain::applyTransferredStrategy(const std::string& task) {
    // Apply strategy from a similar task
    auto it = taskMemories_.find(task);
    if (it != taskMemories_.end() && !it->second.empty()) {
        // Extract the most successful action pattern
        // For now, just return a default based on task name
        if (task.find("move") != std::string::npos) {
            return MotorCommand::MoveForward;
        } else if (task.find("turn") != std::string::npos) {
            return MotorCommand::TurnLeft;
        } else if (task.find("interact") != std::string::npos) {
            return MotorCommand::Interact;
        }
    }
    return MotorCommand::Wait;
}

float AgentBrain::getSocialValueForAction(MotorCommand action) const {
    // Get social value of an action based on observed behavior
    auto it = socialValue_.find(std::to_string(static_cast<int>(action)));
    if (it != socialValue_.end()) {
        return it->second;
    }
    
    // Default values based on action type
    switch (action) {
        case MotorCommand::MoveForward: return 0.7f;
        case MotorCommand::MoveBackward: return 0.3f;
        case MotorCommand::TurnLeft: return 0.5f;
        case MotorCommand::TurnRight: return 0.6f;
        case MotorCommand::LookLeft: return 0.4f;
        case MotorCommand::LookRight: return 0.5f;
        case MotorCommand::Interact: return 0.8f;
        case MotorCommand::Wait: return 0.2f;
        default: return 0.0f;
    }
}

bool AgentBrain::isSkillRelevantToGoal(const std::string& skill, const std::string& goal) const {
    // Simple check if skill is relevant to goal
    if (goal.empty()) return true;
    
    // Check for common keywords
    if (skill.find("move") != std::string::npos && goal.find("move") != std::string::npos) {
        return true;
    }
    if (skill.find("turn") != std::string::npos && goal.find("turn") != std::string::npos) {
        return true;
    }
    if (skill.find("interact") != std::string::npos && goal.find("interact") != std::string::npos) {
        return true;
    }
    
    // Default to true if goal is generic
    if (goal == "default" || goal == "explore") {
        return true;
    }
    
    return false;
}

} // namespace nlm