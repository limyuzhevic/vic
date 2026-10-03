#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
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
    , predictionSystem_(std::make_unique<PredictionSystem>())
    , prediction_(nullptr)
    , episodicMemory_(new NeuralEpisodicMemory())
    , neuralPlanner_(new NeuralPlanner())
{
    // Initialize neural planner
    if (brain_) {
        neuralPlanner_->initialize(brain_.get());
        episodicMemory_->initialize(brain_.get());
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
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize episodic memory with brain reference
    if (episodicMemory_) {
        episodicMemory_->initialize(brain_.get());
    }
    
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
    
    // STEP 1: Generate prediction based on current sensory input (before processing)
    // Create sensory input from percept for prediction
    std::vector<float> allInputData;
    allInputData.reserve(256 + 8 + 4 + 6);
    
    // Vision
    const auto& vision = percept.getVision();
    allInputData.insert(allInputData.end(), vision.begin(), vision.end());
    
    // Touch
    const auto& touch = percept.getTouch();
    allInputData.insert(allInputData.end(), touch.begin(), touch.end());
    
    // Internal
    const auto& intern = percept.getInternal();
    allInputData.insert(allInputData.end(), intern.begin(), intern.end());
    
    // Proprioception
    const auto& proprio = percept.getProprioception();
    allInputData.insert(allInputData.end(), proprio.begin(), proprio.end());
    
    // Generate prediction using prediction system
    SensoryInput predictedSensory(allInputData);
    SensoryInput actualSensory(allInputData);
    
    // Store prediction for learning
    prediction_ = std::make_unique<SensoryInput>(predictedSensory);
    
    // STEP 2: Calculate prediction error
    // For now, use a simple difference - real prediction would use prediction system
    float predictionError = 0.0f;
    if (!allInputData.empty()) {
        // Simple prediction error calculation (placeholder for real prediction)
        // In real implementation, this would use predictionSystem_->predictNextState()
        predictionError = 0.1f; // Initial prediction error
        setPredictionError(predictionError);
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
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
    
    // Store significant sensory information in working memory
    auto* workingMemory = brain_ ? brain_->getWorkingMemory() : nullptr;
    if (workingMemory) {
        // Store vision when intensity is significant (> 0.5)
        std::vector<float> significantVision;
        for (size_t i = 0; i < vision.size(); ++i) {
            if (vision[i] > 0.5f) {
                significantVision.push_back(vision[i]);
            }
        }
        
        if (!significantVision.empty()) {
            // Store with strength proportional to total significant intensity
            float strength = 0.0f;
            for (float v : significantVision) {
                strength += v;
            }
            strength = std::min(strength, 1.0f);
            workingMemory->store(significantVision, strength);
        }
        
        // Also store touch, internal, and proprioception if significant
        std::vector<float> significantTouch;
        for (size_t i = 0; i < touch.size(); ++i) {
            if (touch[i] > 0.5f) {
                significantTouch.push_back(touch[i]);
            }
        }
        if (!significantTouch.empty()) {
            float strength = std::min(significantTouch[0], 1.0f);
            workingMemory->store(significantTouch, strength);
        }
    }
    
    // STEP 3: Update neuromodulation based on prediction
    // Prediction affects neuromodulation levels
    if (predictionSystem_) {
        // Update prediction system with actual input for learning
        predictionSystem_->updatePredictions(predictedSensory, actualSensory);
        
        // Prediction error affects neuromodulation
        float systemPredictionError = predictionSystem_->getPredictionError();
        setPredictionError(systemPredictionError);
        
        // Adjust neuromodulation based on prediction
        // Positive prediction (good expectations) increases dopamine baseline
        // Negative prediction (surprise) increases exploration drive
        float predictionFactor = 1.0f - std::abs(predictionError_) * 0.5f;
        
        // Adjust curiosity based on prediction error
        if (curiosityEnabled_) {
            curiosityLevel_ = curiosityLevel_ * predictionFactor + std::abs(predictionError_) * 0.3f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
        // Update working memory based on prediction error
        if (workingMemory && predictionError_ > 0.2f) {
            // High prediction error strengthens learning in working memory
            workingMemory->increaseLearningRate(0.1f);
        }
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_ || !neuralPlanner_) return MotorCommand::Wait;
    
    // Get current plan from neural planner
    std::vector<ActionType> plan = getCurrentPlan();
    MotorCommand plannedCommand = MotorCommand::Wait;
    
    if (!plan.empty()) {
        // Convert planned action to motor command
        plannedCommand = convertActionToMotorCommand(plan[0]);
        
        // Update neural planner with actual action for learning
        neuralPlanner_->updatePlanQuality(plan, std::vector<ActionType>{plan[0]}, expectedReward_);
    } else {
        // Fallback to neural activity decoding
        plannedCommand = decodeFromMotorNeurons();
    }
    
    // Apply curiosity-based exploration on top of planned action
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        plannedCommand = selectWithCuriosity(plannedCommand);
    }
    
    return plannedCommand;
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

MotorCommand AgentBrain::convertActionToMotorCommand(ActionType action) const {
    // Convert ActionType enum to MotorCommand enum
    // This is a mapping from semantic actions to motor commands
    switch (action) {
        case ActionType::MoveForward:
            return MotorCommand::MoveForward;
        case ActionType::MoveBackward:
            return MotorCommand::MoveBackward;
        case ActionType::TurnLeft:
            return MotorCommand::TurnLeft;
        case ActionType::TurnRight:
            return MotorCommand::TurnRight;
        case ActionType::Look:
        case ActionType::LookLeft:
            return MotorCommand::LookLeft;
        case ActionType::LookUp:
            return MotorCommand::Wait; // Up movement not in MotorCommand
        case ActionType::LookDown:
            return MotorCommand::Wait; // Down movement not in MotorCommand
        case ActionType::Interact:
            return MotorCommand::Interact;
        case ActionType::Eat:
            return MotorCommand::Interact; // Eat uses same motor command as interact
        case ActionType::Drink:
            return MotorCommand::Interact; // Drink uses same motor command as interact
        case ActionType::Rest:
        case ActionType::Wait:
            return MotorCommand::Wait;
        case ActionType::Custom:
            return MotorCommand::Wait; // Custom actions default to wait
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

// Get prediction from prediction system
const SensoryInput* AgentBrain::getPrediction() const {
    if (predictionSystem_) {
        // Create a dummy sensory input for now - in real implementation this would come from predictionSystem_
        // This is a placeholder to satisfy the interface
        static SensoryInput dummyInput;
        return &dummyInput;
    }
    return nullptr;
}

// Set prediction error directly (for reward prediction errors)
void AgentBrain::setPredictionError(float error) {
    predictionError_ = error;
    
    // Update dopamine level (prediction error signal)
    if (rewardModulationEnabled_) {
        dopamineLevel_ = predictionError_;
        dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    }
}

std::vector<float> AgentBrain::getWorkingMemoryContents() const {
    if (!brain_) return {};
    auto* workingMemory = brain_->getWorkingMemory();
    if (!workingMemory) return {};
    return workingMemory->retrieve();
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

void AgentBrain::initializePlanning() {
    // Set up planning context using memory systems
    if (!brain_ || !neuralPlanner_) return;
    
    // Get current sensory state from working memory
    auto* workingMemory = brain_->getWorkingMemory();
    std::vector<float> currentState;
    if (workingMemory) {
        currentState = workingMemory->retrieve();
    }
    
    // Retrieve relevant episodic memories
    if (episodicMemory_) {
        // Get recent episodes for context
        auto recentEpisodes = episodicMemory_->getRecentEpisodes(5);
        std::vector<float> episodeContext;
        
        for (const auto* episode : recentEpisodes) {
            if (episode) {
                // Combine sensory states from recent episodes for context
                episodeContext.insert(episodeContext.end(), 
                                     episode->sensoryState.begin(), 
                                     episode->sensoryState.end());
            }
        }
        
        if (!episodeContext.empty()) {
            // Combine current state with episodic context
            std::vector<float> planningContext = currentState;
            planningContext.insert(planningContext.end(),
                                  episodeContext.begin(),
n                                  episodeContext.end());
            
            // Set goal based on current state (e.g., move away from hazards)
            neuralPlanner_->setCurrentGoal(planningContext);
        }
    }
    
    // Set prediction system integration
    if (predictionSystem_) {
        // Prepare state for prediction system
        SensoryInput planningState(currentState);
        predictionSystem_->train(planningState);
    }
    
    NLM_LOG_INFO("Planning initialized with working memory size: " + 
                 std::to_string(currentState.size()) + ", episodes: " +
                 std::to_string(episodicMemory_ ? episodicMemory_->getEpisodeCount() : 0));
}

std::vector<ActionType> AgentBrain::getCurrentPlan() const {
    std::vector<ActionType> currentPlan;
    
    if (!neuralPlanner_ || !brain_) return currentPlan;
    
    // Get current state for planning context
    auto* workingMemory = brain_->getWorkingMemory();
    std::vector<float> currentState;
    if (workingMemory) {
        currentState = workingMemory->retrieve();
    }
    
    if (currentState.empty()) return currentPlan;
    
    // Get planned action from neural planner
    ActionType plannedAction = neuralPlanner_->planAction(currentState);
    currentPlan.push_back(plannedAction);
    
    // Generate a simple sequence of planned actions
    // In a real implementation, this would be a full multi-step plan
    for (size_t i = 1; i < neuralPlanner_->getPlanningDepth(); ++i) {
        currentPlan.push_back(plannedAction); // Repeat for now
    }
    
    return currentPlan;
}
