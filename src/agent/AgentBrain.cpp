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

    return best;
}

// Enhanced motor decoding with memory integration
MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    // Apply planning-based motor selection if available
    if (brain_->getPlanner()) {
        std::vector<float> currentState = getCurrentSensoryState();
        ActionType plannedAction = brain_->getPlanner()->planAction(currentState, expectedReward_);
        
        // Convert action to motor command
        MotorCommand plannedMotor = convertActionToMotor(plannedAction);
        
        // Blend planned action with direct decoding (blend based on confidence)
        float planningConfidence = brain_->getPlanner()->getPlanningConfidence();
        if (planningConfidence > 0.5f && plannedMotor != MotorCommand::Wait) {
            // If we have a good plan, prefer it over raw motor activity
            decoded = plannedMotor;
        }
    }
    
    // Apply concept-based motor selection if available
    if (brain_->getConceptFormation() && conceptFormationEnabled_) {
        std::vector<float> currentState = getCurrentSensoryState();
        size_t conceptId = brain_->getConceptFormation()->getMatchingConcept(currentState);
        
        if (conceptId > 0) {
            const auto& concept = brain_->getConceptFormation()->getConcept(conceptId);
            
            // Check if concept suggests specific action
            ActionType conceptSuggestedAction = getConceptSuggestedAction(concept);
            if (conceptSuggestedAction != ActionType::Wait) {
                MotorCommand conceptMotor = convertActionToMotor(conceptSuggestedAction);
                
                // Blend with current selection
                float conceptStrength = concept.avgStability;
                if (conceptStrength > 0.7f) {
                    decoded = conceptMotor;
                }
            }
        }
    }
    
    return decoded;
}

// Get current sensory state for cognitive systems
std::vector<float> AgentBrain::getCurrentSensoryState() const {
    std::vector<float> state;
    
    // Combine vision, touch, and internal sensors
    state.insert(state.end(), previousVision_.begin(), previousVision_.end());
    
    // Add other sensory modalities
    // In full implementation, would add touch, proprioception, etc.
    
    return state;
}

// Convert ActionType to MotorCommand
MotorCommand AgentBrain::convertActionToMotor(ActionType action) const {
    switch (action) {
        case ActionType::MoveForward: return MotorCommand::MoveForward;
        case ActionType::MoveBackward: return MotorCommand::MoveBackward;
        case ActionType::TurnLeft: return MotorCommand::TurnLeft;
        case ActionType::TurnRight: return MotorCommand::TurnRight;
        case ActionType::LookLeft: return MotorCommand::LookLeft;
        case ActionType::LookRight: return MotorCommand::LookRight;
        case ActionType::Interact: return MotorCommand::Interact;
        case ActionType::Wait: return MotorCommand::Wait;
        default: return MotorCommand::Wait;
    }
}

// Get action suggested by concept
ActionType AgentBrain::getConceptSuggestedAction(const DiscoveredConcept& concept) const {
    // Simple heuristic: if concept category contains certain words, suggest action
    std::string category = concept.categoryHint;
    
    if (category.find("reward") != std::string::npos) {
        return ActionType::Interact;
    } else if (category.find("danger") != std::string::npos || 
               category.find("harm") != std::string::npos) {
        return ActionType::Wait; // Avoid potentially harmful actions
    } else if (category.find("explore") != std::string::npos) {
        return ActionType::MoveForward;
    }
    
    return ActionType::Wait;
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

// Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
}

// Enhanced memory integration methods
void AgentBrain::integrateWithWorkingMemory() {
    if (!brain_ || !brain_->getWorkingMemory()) return;
    
    // Get working memory content
    const auto& memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
    
    if (memoryNeurons.empty()) return;
    
    // Analyze memory content for influence on motor output
    std::vector<float> memoryPattern = brain_->getWorkingMemory()->retrieve();
    
    // Check if memory suggests specific actions
    if (!memoryPattern.empty()) {
        // Apply memory influence based on activation levels
        float memoryActivation = brain_->getWorkingMemory()->getMemoryActivity();
        
        if (memoryActivation > 0.5f) {
            // High memory activation means we should be more persistent
            // If the agent was planning to do something, continue that action
            // For now, just increase the tendency to not change actions
            if (lastDecodedAction_ != MotorCommand::Wait) {
                // Keep current motor command more strongly
                currentMotorTendency_ = lastDecodedAction_;
                motorTendencyConfidence_ += 0.2f * memoryActivation;
            }
        }
    }
}

void AgentBrain::integrateWithEpisodicMemory() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Get recent episodes that might be relevant
    size_t episodeCount = brain_->getEpisodicMemory()->getEpisodeCount();
    if (episodeCount == 0) return;
    
    // Get most recent episodes
    auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(std::min<size_t>(3, episodeCount));
    
    for (const auto* episode : recentEpisodes) {
        if (!episode) continue;
        
        // Analyze episode for influence on current behavior
        if (episode->action == lastDecodedAction_) {
            // Success: this action worked before
            if (episode->reward > 0.0f) {
                // Positive outcome - reinforce this action
                reinforceAction(lastDecodedAction_, 0.5f);
            } else {
                // Negative outcome - be cautious
                reduceActionConfidence(lastDecodedAction_, 0.3f);
            }
        }
    }
}

void AgentBrain::integrateWithAssociativeMemory() {
    if (!brain_ || !brain_->getAssociativeMemory()) return;
    
    std::vector<float> currentState = getCurrentSensoryState();
    
    // Retrieve associations for current state
    auto associations = brain_->getAssociativeMemory()->retrieve(currentState, 3);
    
    for (const auto& assoc : associations) {
        // Convert associated pattern to action suggestion
        if (!assoc.empty()) {
            // Pattern might suggest a course of action
            // For now, just log the association
            // In full implementation, would convert pattern to motor command
        }
    }
}

void AgentBrain::applyMemoryGuidedLearning(float reward, float predictionError) {
    if (!brain_) return;
    
    // Update memory systems based on experience
    NeuralWorkingMemory* workingMem = brain_->getWorkingMemory();
    NeuralEpisodicMemory* episodicMem = brain_->getEpisodicMemory();
    NeuralAssociativeMemory* assocMem = brain_->getAssociativeMemory();
    
    if (workingMem) {
        // Store experience in working memory
        std::vector<float> currentPattern = getCurrentSensoryState();
        workingMem->store(currentPattern, std::abs(reward) * 0.5f + std::abs(predictionError) * 0.5f);
    }
    
    if (episodicMem && reward != 0.0f) {
        // Store episode when there's significant outcome
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep_;
        episode.action = lastDecodedAction_;
        episode.reward = reward;
        episode.energy = currentEnergy_;
        episode.novelty = noveltyLevel_;
        episode.sensoryState = getCurrentSensoryState();
        
        // Generate predicted next state based on action
        std::vector<float> predictedState = predictNextState();
        episode.resultingSensoryState = predictedState;
        episode.resultingReward = reward;
        
        episodicMem->storeEpisode(episode);
    }
    
    if (assocMem && !currentStatePattern_.empty()) {
        // Learn associations between current and resulting states
        assocMem->associateFromExperience(episode);
    }
}

void AgentBrain::applyMemoryGuidedPlanning() {
    if (!brain_ || !brain_->getPlanner()) return;
    
    std::vector<float> currentState = getCurrentSensoryState();
    
    // Get planning depth based on memory availability
    size_t planningDepth = 2;  // Default
    
    // Adjust planning based on memory
    if (brain_->getWorkingMemory() && brain_->getWorkingMemory()->getActiveTraces() > 0) {
        planningDepth = 4;  // More memory = deeper planning
    }
    
    brain_->getPlanner()->setPlanningDepth(planningDepth);
    
    // Set planning goal based on memory
    std::vector<float> goalState = getPlanningGoal();
    brain_->getPlanner()->setCurrentGoal(goalState);
    
    // Plan action
    ActionType plannedAction = brain_->getPlanner()->planAction(currentState, expectedReward_);
    plannedAction_ = plannedAction;
}

void AgentBrain::applyMemoryGuidedAttention() {
    if (!brain_ || !brain_->getAttention()) return;
    
    std::vector<float> currentState = getCurrentSensoryState();
    
    // Get competing sensory inputs
    std::vector<NeuronId> competitors;
    if (brain_->getWorkingMemory()) {
        competitors = brain_->getWorkingMemory()->getMemoryNeurons();
    }
    
    if (!competitors.empty()) {
        brain_->getAttention()->processCompetition(competitors);
    }
}

} // namespace nlm
