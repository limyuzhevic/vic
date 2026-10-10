#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
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
    , episodicRecordingEnabled_(true)  // NEW: Enable episodic recording by default
    , sensoryNoveltyDecay_(0.99f)
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
    
    // ENHANCEMENT: Store sensory input in working memory
    // Convert sensory percept to neural pattern for working memory
    std::vector<float> sensoryPattern;
    
    // Vision pattern (256 values)
    sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
    
    // Touch pattern (8 values)
    std::vector<float> touchPattern(touch.begin(), touch.end());
    sensoryPattern.insert(sensoryPattern.end(), touchPattern.begin(), touchPattern.end());
    
    // Internal pattern (4 values)
    std::vector<float> internalPattern(intern.begin(), intern.end());
    sensoryPattern.insert(sensoryPattern.end(), internalPattern.begin(), internalPattern.end());
    
    // Proprioception pattern (6 values)
    std::vector<float> proprioPattern(proprio.begin(), proprio.end());
    sensoryPattern.insert(sensoryPattern.end(), proprioPattern.begin(), proprioPattern.end());
    
    // Store in working memory with strength based on overall sensory activity
    if (brain_ && brain_->getWorkingMemory()) {
        // Calculate overall sensory activity level
        float totalActivity = 0.0f;
        for (float val : vision) totalActivity += std::abs(val);
        for (float val : touch) totalActivity += std::abs(val);
        for (float val : intern) totalActivity += std::abs(val);
        for (float val : proprio) totalActivity += std::abs(val);
        
        float strength = std::min(1.0f, totalActivity / 10.0f);  // Normalize to 0-1
        
        // Store pattern in working memory
        brain_->getWorkingMemory()->store(sensoryPattern, strength);
        
        NLM_LOG_DEBUG("Stored sensory pattern in working memory: " + std::to_string(sensoryPattern.size()) + " elements, strength=" + std::to_string(strength));
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
    
    // Get current sensory state from working memory for planning
    std::vector<float> currentState;
    if (brain_->getWorkingMemory()) {
        currentState = brain_->getWorkingMemory()->retrieve();
    }
    
    // Get neural planner for planned action selection
    auto* planner = brain_->getPlanner();
    bool plannerAvailable = (planner != nullptr);
    
    MotorCommand plannedAction = MotorCommand::Wait;
    
    if (plannerAvailable && !currentState.empty()) {
        // Use neural planner to plan action based on current state
        // Target reward encourages exploration and learning
        float targetReward = 0.5f;  // Balanced exploration/exploitation
        plannedAction = planner->planAction(currentState, targetReward);
        
        // Get planner confidence for decision making
        float planningConfidence = planner->getPlanningConfidence();
        
        NLM_LOG_DEBUG("AgentBrain: Planner selected action " + std::to_string(static_cast<int>(plannedAction)) + 
                     " with confidence " + std::to_string(planningConfidence));
    }
    
    // Get motor command from both planned action and motor neurons
    MotorCommand motorBasedCommand = decodeFromMotorNeurons();
    
    // Decide which command to use based on availability and confidence
    MotorCommand finalCommand;
    
    if (plannerAvailable && !currentState.empty()) {
        // Use neural planning as primary method when available
        float planningConfidence = planner->getPlanningConfidence();
        
        if (planningConfidence > 0.6f) {
            // High confidence: trust the planned action
            finalCommand = plannedAction;
            NLM_LOG_DEBUG("AgentBrain: Using planned action (high confidence)");
        } else if (planningConfidence > 0.3f) {
            // Medium confidence: blend planned and motor-based actions
            // For now, prioritize planned action but with curiosity
            finalCommand = plannedAction;
            // Add curiosity-driven variation
            if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
                finalCommand = selectWithCuriosity(finalCommand);
            }
            NLM_LOG_DEBUG("AgentBrain: Using planned action with curiosity (medium confidence)");
        } else {
            // Low confidence: rely on motor neuron activity (reactive)
            finalCommand = motorBasedCommand;
            NLM_LOG_DEBUG("AgentBrain: Using reactive motor neurons (low confidence)");
        }
    } else {
        // No planner available: use motor neuron activity only
        finalCommand = motorBasedCommand;
        NLM_LOG_DEBUG("AgentBrain: No planner available, using reactive motor neurons");
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        finalCommand = selectWithCuriosity(finalCommand);
    }
    
    // Log decision
    NLM_LOG_DEBUG("AgentBrain: Final command = " + std::to_string(static_cast<int>(finalCommand)));
    
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

// ENHANCEMENT: Record complete experience in episodic memory
void AgentBrain::recordExperience(const SensoryPercept& percept, 
                                   MotorCommand action, 
                                   float reward, 
                                   SimulationStep step) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Convert sensory percept to neural pattern
    std::vector<float> sensoryPattern;
    
    // Vision pattern (256 values)
    const auto& vision = percept.getVision();
    sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
    
    // Touch pattern (8 values)
    const auto& touch = percept.getTouch();
    std::vector<float> touchPattern(touch.begin(), touch.end());
    sensoryPattern.insert(sensoryPattern.end(), touchPattern.begin(), touchPattern.end());
    
    // Internal pattern (4 values)
    const auto& intern = percept.getInternal();
    std::vector<float> internalPattern(intern.begin(), intern.end());
    sensoryPattern.insert(sensoryPattern.end(), internalPattern.begin(), internalPattern.end());
    
    // Proprioception pattern (6 values)
    const auto& proprio = percept.getProprioception();
    std::vector<float> proprioPattern(proprio.begin(), proprio.end());
    sensoryPattern.insert(sensoryPattern.end(), proprioPattern.begin(), proprioPattern.end());
    
    // Create episodic memory item
    EpisodicMemoryItem episode;
    episode.timestamp = step;
    episode.age = 0;
    episode.sensoryState = sensoryPattern;
    
    // Convert MotorCommand to ActionType
    ActionType actionType;
    switch (action) {
        case MotorCommand::MoveForward: actionType = ActionType::MoveForward; break;
        case MotorCommand::MoveBackward: actionType = ActionType::MoveBackward; break;
        case MotorCommand::TurnLeft: actionType = ActionType::TurnLeft; break;
        case MotorCommand::TurnRight: actionType = ActionType::TurnRight; break;
        case MotorCommand::LookLeft: actionType = ActionType::Look; break;
        case MotorCommand::LookRight: actionType = ActionType::Look; break;
        case MotorCommand::Interact: actionType = ActionType::Interact; break;
        default: actionType = ActionType::Wait; break;
    }
    episode.action = actionType;
    
    // Internal state (energy, reward, etc.)
    episode.reward = reward;
    episode.energy = noveltyLevel_;  // Use novelty as proxy for energy
    episode.novelty = noveltyLevel_;
    
    // Location information (would come from environment - placeholder)
    episode.positionX = 0.0f;
    episode.positionY = 0.0f;
    episode.orientation = 0.0f;
    
    // Consequence of action (same as initial state for now)
    episode.resultingSensoryState = sensoryPattern;
    episode.resultingReward = reward;
    
    // Also get some neural activity from brain
    episode.activeNeurons.clear();
    episode.neuronActivations.clear();
    
    if (brain_->getTotalNeuronCount() > 0) {
        // Sample some neurons from the brain
        size_t sampleCount = std::min<size_t>(50, brain_->getTotalNeuronCount());
        for (size_t i = 0; i < sampleCount; ++i) {
            // Use deterministic sampling based on step
            size_t neuronIndex = (i * 137 + step) % brain_->getTotalNeuronCount();
            
            // Get a neuron - this is simplified
            // In a real implementation, we would have access to specific neurons
            // For now, just add some placeholder data
            NeuronId dummyNeuron(static_cast<uint64_t>(neuronIndex));
            episode.activeNeurons.push_back(dummyNeuron);
            episode.neuronActivations.push_back(0.1f);  // Placeholder
        }
    }
    
    // Store in episodic memory
    brain_->getEpisodicMemory()->storeEpisode(episode);
    
    NLM_LOG_DEBUG("AgentBrain::recordExperience - stored episode at step " + std::to_string(step) + 
                 " with action " + std::to_string(static_cast<int>(actionType)) + 
                 " and reward " + std::to_string(reward));
}

void AgentBrain::enableEpisodicRecording(bool enable) {
    episodicRecordingEnabled_ = enable;
}

bool AgentBrain::isEpisodicRecordingEnabled() const {
    return episodicRecordingEnabled_;
}
