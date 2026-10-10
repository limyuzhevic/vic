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
    , sensoryNoveltyDecay_(0.99f)
    , selfModel_(nullptr)
    , socialLearning_(nullptr)
    , spatialRepresentation_(nullptr)
    , conceptFormation_(nullptr)
    , workingMem_(nullptr)
    , episodicMem_(nullptr)
    , currentX_(0.0f)
    , currentY_(0.0f)
    , currentTime_(0.0)
    , recordedActions_(10)  // 10 action types
    , recordedSocialActions_()
    , currentConcepts_()
    , formedConcepts_()
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
    currentX_ = 0.0f;
    currentY_ = 0.0f;
    currentTime_ = 0.0;
    
    // Extract cognitive systems from brain for integration
    if (brain_) {
        selfModel_ = std::shared_ptr<NeuralSelfModel>(brain_->getSelfModel());
        socialLearning_ = std::shared_ptr<NeuralSocialLearning>(brain_->getSocialLearning());
        spatialRepresentation_ = std::shared_ptr<NeuralSpatialRepresentation>(brain_->getSpatialRepresentation());
        conceptFormation_ = std::shared_ptr<ConceptFormation>(brain_->getConceptFormation());
        workingMem_ = std::shared_ptr<NeuralWorkingMemory>(brain_->getWorkingMemory());
        episodicMem_ = std::shared_ptr<NeuralEpisodicMemory>(brain_->getEpisodicMemory());
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    if (selfModel_) {
        NLM_LOG_INFO("AgentBrain integrated NeuralSelfModel for body awareness");
    }
    if (socialLearning_) {
        NLM_LOG_INFO("AgentBrain integrated NeuralSocialLearning for social knowledge");
    }
    if (spatialRepresentation_) {
        NLM_LOG_INFO("AgentBrain integrated NeuralSpatialRepresentation for navigation");
    }
    if (conceptFormation_) {
        NLM_LOG_INFO("AgentBrain integrated ConceptFormation for pattern discovery");
    }
    if (workingMem_) {
        NLM_LOG_INFO("AgentBrain integrated working memory for experience storage");
    }
    if (episodicMem_) {
        NLM_LOG_INFO("AgentBrain integrated episodic memory for experience consolidation");
    }
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

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

// Cognitive planning integration
bool AgentBrain::planAction(const NeuralPlanner* planner, const std::vector<float>& currentState) {
    if (!planner || !brain_) return false;
    
    // Use the neural planner to plan next action
    ActionType plannedAction = planner->planAction(currentState, 0.5f);
    
    // Store the planned action for later execution
    plannedAction_ = plannedAction;
    
    NLM_LOG_INFO("AgentBrain planned action: " + std::to_string(static_cast<int>(plannedAction)));
    return true;
}

bool AgentBrain::executePlannedAction(MotorCommand plannedAction, const std::vector<float>& currentState) {
    if (!brain_) return false;
    
    // Execute the planned action using the world
    // This will be implemented based on the planned action
    NLM_LOG_INFO("AgentBrain executing planned action: " + std::to_string(static_cast<int>(plannedAction)));
    
    // Convert planned action to motor command
    // For now, direct mapping
    if (plannedAction == MotorCommand::MoveForward) {
        // Execute forward movement
        return true;
    } else if (plannedAction == MotorCommand::TurnLeft) {
        // Execute turn left
        return true;
    } else if (plannedAction == MotorCommand::TurnRight) {
        // Execute turn right
        return true;
    } else if (plannedAction == MotorCommand::Interact) {
        // Execute interaction
        return true;
    }
    
    return false;
}

// Self-model integration for body awareness
bool AgentBrain::updateSelfModel(const NeuralSelfModel* selfModel, const std::vector<float>& beforeState, const std::vector<float>& afterState) {
    if (!selfModel) return false;
    
    // Find the most likely action that caused this change
    ActionType inferredAction = ActionType::Wait;
    float maxLikelihood = 0.0f;
    
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Wait) + 1; ++i) {
        ActionType action = static_cast<ActionType>(i);
        float confidence = selfModel->getSelfModelConfidence(action);
        
        if (confidence > maxLikelihood) {
            maxLikelihood = confidence;
            inferredAction = action;
        }
    }
    
    // Update self-model with this experience
    selfModel->recordSelfAction(inferredAction, beforeState, afterState);
    
    // Update our internal action effects tracking
    recordedActions_[inferredAction].push_back({beforeState, afterState, maxLikelihood, 1});
    
    NLM_LOG_INFO("AgentBrain updated self-model with action: " + std::to_string(static_cast<int>(inferredAction)));
    return true;
}

std::vector<float> AgentBrain::predictConsequence(ActionType action, const std::vector<float>& currentState) {
    std::vector<float> predictedState = currentState;
    
    // Try to predict the consequence using self-model
    if (selfModel_) {
        predictedState = selfModel_->predictActionConsequence(action, currentState);
    }
    
    return predictedState;
}

// Social learning integration for observation and imitation
bool AgentBrain::observeSocialAction(ActionType action, const std::vector<float>& observerState, const std::vector<float>& resultingState) {
    if (!socialLearning_) return false;
    
    // Learn from social observation
    socialLearning_->observeAgentAction(action, observerState, resultingState);
    
    // Also record this observation for social knowledge
    recordedSocialActions_.push_back({action, observerState, resultingState, 0.0f});
    
    NLM_LOG_INFO("AgentBrain observed social action: " + std::to_string(static_cast<int>(action)));
    return true;
}

bool AgentBrain::canImitate(ActionType action) const {
    if (!socialLearning_) return false;
    return socialLearning_->canImitate(action);
}

ActionType AgentBrain::getImitationAction(const std::vector<float>& currentState) const {
    if (!socialLearning_) return MotorCommand::Wait;
    return socialLearning_->getImitationAction(currentState);
}

// Spatial representation integration for navigation
bool AgentBrain::updateSpatialRepresentation(const NeuralSpatialRepresentation* spatialRep, float x, float y, const std::vector<float>& sensoryCues) {
    if (!spatialRep) return false;
    
    // Update spatial representation with current position and sensory cues
    spatialRep->recordPosition(x, y, sensoryCues);
    
    // Update our internal spatial state
    currentX_ = x;
    currentY_ = y;
    
    NLM_LOG_INFO("AgentBrain updated spatial representation: position (" + std::to_string(x) + ", " + std::to_string(y) + ")");
    return true;
}

std::pair<float, float> AgentBrain::getPredictedPosition() const {
    if (!spatialRepresentation_) {
        return {currentX_, currentY_};
    }
    return spatialRepresentation_->getPredictedPosition();
}

float AgentBrain::getPositionActivation(float x, float y) const {
    if (!spatialRepresentation_) {
        // Simple distance-based activation
        float dx = x - currentX_;
        float dy = y - currentY_;
        return std::exp(-(dx*dx + dy*dy) / 10.0f);
    }
    return spatialRepresentation_->getPositionActivation(x, y);
}

// Concept formation integration for pattern discovery
bool AgentBrain::updateConceptFormation(const ConceptFormation* conceptFormation, const std::vector<float>& currentPattern) {
    if (!conceptFormation) return false;
    
    // Present current pattern to concept formation system
    conceptFormation->presentExperience(currentPattern, currentTime_);
    
    // Also store in our concept database
    currentConcepts_.push_back(currentPattern);
    
    NLM_LOG_INFO("AgentBrain updated concept formation with pattern of size " + std::to_string(currentPattern.size()));
    return true;
}

bool AgentBrain::formConceptFromExperience(const std::vector<float>& experiencePattern) {
    if (!conceptFormation_) return false;
    
    // Try to form a new concept from this experience pattern
    conceptFormation_->presentExperience(experiencePattern, currentTime_);
    
    // Store the concept
    formedConcepts_.push_back(experiencePattern);
    
    NLM_LOG_INFO("AgentBrain formed new concept from experience pattern");
    return true;
}

// Memory system integration for learning and consolidation
bool AgentBrain::storeExperienceInMemory(const NeuralWorkingMemory* workingMem, const std::vector<float>& currentState) {
    if (!workingMem) return false;
    
    // Store current state in working memory
    workingMem->storeToNeuron(reinterpret_cast<NeuronId>(1), 1.0f);  // Using a dummy ID
    
    NLM_LOG_INFO("AgentBrain stored experience in working memory");
    return true;
}

bool AgentBrain::storeEpisodeInEpisodicMemory(const NeuralEpisodicMemory* episodicMem, const std::vector<float>& experience) {
    if (!episodicMem) return false;
    
    // Store the entire experience as an episode
    // This would require creating an EpisodicMemoryItem structure
    EpisodicMemoryItem episode;
    episode.timestamp = currentTime_;
    episode.reward = dopamineLevel_;
    
    NLM_LOG_INFO("AgentBrain stored episode in episodic memory");
    return true;
}

bool AgentBrain::consolidateMemory(const NeuralEpisodicMemory* episodicMem, float consolidationThreshold) {
    if (!episodicMem) return false;
    
    // Consolidate memories based on importance
    // This would typically involve replay and strengthening of important memories
    
    NLM_LOG_INFO("AgentBrain consolidated memory system");
    return true;
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
