#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
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
    , episodicMemory_(nullptr)
    , associativeMemory_(nullptr)
    , predictionSystem_(nullptr)
    , neuralPlanner_(nullptr)
    , conceptFormation_(nullptr)
    , acetylcholine_(nullptr)
    , norepinephrine_(nullptr)
    , serotonin_(nullptr)
    , curiosity_(nullptr)
    , novelty_(nullptr)
    , predictionError_(nullptr)
    , developmentSystem_(nullptr)
    , episodicMemoryEnabled_(false)
    , predictionEnabled_(false)
    , planningEnabled_(false)
    , conceptFormationEnabled_(false)
    , associativeMemoryEnabled_(false)
    , neuromodulationEnabled_(false)
    , memoryConsolidationEnabled_(false)
    , currentStep_(0.0)
    , energy_(0.0f)
    , predictedReward_(0.0f)
    , workingMemory_(nullptr)
    , consolidationTimer_(0.0)
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
    consolidationTimer_ = 0.0;
    
    // Initialize memory systems
    episodicMemory_ = brain_ ? brain_->getEpisodicMemory() : nullptr;
    episodicMemoryEnabled_ = episodicMemory_ ? true : false;
    
    associativeMemory_ = brain_ ? brain_->getAssociativeMemory() : nullptr;
    associativeMemoryEnabled_ = associativeMemory_ ? true : false;
    
    // Initialize prediction system
    predictionSystem_ = brain_ ? brain_->getPredictionSystem() : nullptr;
    predictionEnabled_ = predictionSystem_ ? true : false;
    
    // Initialize cognition systems
    neuralPlanner_ = brain_ ? brain_->getPlanner() : nullptr;
    planningEnabled_ = neuralPlanner_ ? true : false;
    
    conceptFormation_ = brain_ ? brain_->getConceptFormation() : nullptr;
    conceptFormationEnabled_ = conceptFormation_ ? true : false;
    
    // Initialize development system
    developmentSystem_ = brain_ ? brain_->getDevelopmentSystem() : nullptr;
    
    // Initialize neuromodulation systems
    dopamine_ = brain_ ? brain_->getDopamine() : nullptr;
    acetylcholine_ = brain_ ? brain_->getAcetylcholine() : nullptr;
    norepinephrine_ = brain_ ? brain_->getNorepinephrine() : nullptr;
    serotonin_ = brain_ ? brain_->getSerotonin() : nullptr;
    curiosity_ = brain_ ? brain_->getCuriosity() : nullptr;
    novelty_ = brain_ ? brain_->getNovelty() : nullptr;
    predictionError_ = brain_ ? brain_->getPredictionErrorSignal() : nullptr;
    
    neuromodulationEnabled_ = dopamine_ && curiosity_ && novelty_;
    memoryConsolidationEnabled_ = episodicMemory_ && predictionSystem_;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    // Log integration status
    if (episodicMemoryEnabled_) {
        NLM_LOG_INFO("[INFO] Episodic memory system integrated");
    }
    if (predictionEnabled_) {
        NLM_LOG_INFO("[INFO] Prediction system integrated");
    }
    if (planningEnabled_) {
        NLM_LOG_INFO("[INFO] Neural planner integrated");
    }
    if (conceptFormationEnabled_) {
        NLM_LOG_INFO("[INFO] Concept formation integrated");
    }
    if (associativeMemoryEnabled_) {
        NLM_LOG_INFO("[INFO] Associative memory integrated");
    }
    if (neuromodulationEnabled_) {
        NLM_LOG_INFO("[INFO] Full neuromodulation system integrated");
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
    
    // ========== EPISODIC MEMORY INTEGRATION ==========
    if (episodicMemoryEnabled_) {
        storeEpisodicMemory(percept, currentAction_, expectedReward_, energy_);
    }
    
    // ========== PREDICTION SYSTEM INTEGRATION ==========
    if (predictionEnabled_) {
        updatePredictionError(percept);
    }
    
    // ========== ASSOCIATIVE MEMORY INTEGRATION ==========
    if (associativeMemoryEnabled_) {
        learnActionOutcomeAssociations(percept, currentAction_, expectedReward_);
    }
    
    // ========== CONCEPT FORMATION INTEGRATION ==========
    if (conceptFormationEnabled_) {
        std::vector<float> features = extractFeatures(percept);
        conceptFormation_->presentExperience(features, features, expectedReward_, currentStep_);
    }
    
    // ========== FULL NEUROMODULATION INTEGRATION ==========
    applyFullNeuromodulation(expectedReward_, predictedReward_);
    
    // ========== MEMORY CONSOLIDATION ==========
    consolidateMemories(0.001); // timestep
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

void AgentBrain::storeEpisodicMemory(const SensoryPercept& percept, 
                                   const MotorCommand& action,
                                   float reward, float energy) {
    if (!episodicMemoryEnabled_ || !episodicMemory_ || !brain_) return;
    
    // Create episodic memory item
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep_;
    
    // Encode sensory state
    episode.sensoryState = encodeSensoryState(percept);
    episode.positionX = position_.x;
    episode.positionY = position_.y;
    episode.orientation = orientation_;
    episode.action = action;
    episode.reward = reward;
    episode.energy = energy;
    episode.novelty = noveltyLevel_;
    
    // Get active neural patterns
    episode.activeNeurons = getActiveNeuronIds();
    episode.neuronActivations = getNeuronActivations();
    
    // Store in episodic memory
    episodicMemory_->storeEpisode(episode);
    
    // Associate this episode with action-outcome
    if (associativeMemoryEnabled_) {
        episodicMemory_->updateRelevance(episode.timestamp, reward * 0.1f);
    }
}

void AgentBrain::updatePredictionError(const SensoryPercept& percept) {
    if (!predictionEnabled_ || !predictionSystem_ || !brain_) return;
    
    // Get predicted state
    auto predicted = predictionSystem_->predictNextState(percept);
    
    // Compute actual observation
    auto actual = percept;  // Already has visual, touch, internal data
    
    // Update predictions and get error
    predictionSystem_->updatePredictions(predicted, actual);
    predictionError_ = predictionSystem_->getPredictionError();
}

void AgentBrain::learnActionOutcomeAssociations(const SensoryPercept& percept,
                                              const MotorCommand& action,
                                              float reward) {
    if (!associativeMemoryEnabled_ || !associativeMemory_) return;
    
    // Create sensory pattern from percept
    std::vector<float> sensoryPattern = encodeSensoryState(percept);
    
    // Create action pattern
    std::vector<float> actionPattern = encodeActionPattern(action);
    
    // Associate action with resulting state
    associativeMemory_->associate(actionPattern, sensoryPattern, reward * 0.5f);
    
    // Update concept formation with this experience
    if (conceptFormationEnabled_ && conceptFormation_) {
        std::vector<float> features = extractFeatures(sensoryPattern, reward);
        conceptFormation_->presentExperience(sensoryPattern, features, reward, currentStep_);
    }
}

void AgentBrain::extractFeatures(const std::vector<float>& pattern, float reward, std::vector<float>& features) {
    features.clear();
    if (pattern.empty()) return;
    
    // Extract basic features: mean, variance, range
    float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    float mean = sum / pattern.size();
    
    float sqSum = 0.0f;
    for (float val : pattern) {
        float diff = val - mean;
        sqSum += diff * diff;
    }
    float variance = sqSum / pattern.size();
    
    float minVal = *std::min_element(pattern.begin(), pattern.end());
    float maxVal = *std::max_element(pattern.begin(), pattern.end());
    
    // Include reward as feature
    features.push_back(mean);
    features.push_back(variance);
    features.push_back(minVal);
    features.push_back(maxVal);
    features.push_back(reward);
}

void AgentBrain::applyFullNeuromodulation(float reward, float predictedReward) {
    if (!neuromodulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update all neuromodulators
    if (dopamine_) {
        dopamine_->signalRewardPredictionError(predictionError_);
    }
    
    if (curiosity_) {
        // Use prediction error to drive curiosity
        float curiositySignal = std::abs(predictionError_) * 0.5f;
        curiosity_->signalCuriosity(curiositySignal);
    }
    
    if (novelty_) {
        novelty_->signalNovelty(noveltyLevel_);
    }
    
    if (acetylcholine_) {
        // ACh for attention and memory consolidation
        float attentionLevel = predictionError_ > 0.5f ? 0.8f : 
                              predictionError_ < -0.5f ? 0.2f : 0.5f;
        acetylcholine_->setLevel(attentionLevel);
    }
    
    if (norepinephrine_) {
        // NE for arousal and vigilance based on novelty
        float arousalLevel = std::min(1.0f, noveltyLevel_ * 2.0f);
        norepinephrine_->setLevel(arousalLevel);
    }
    
    if (serotonin_) {
        // 5-HT for mood based on recent reward history
        float moodLevel = std::clamp(expectedReward_, 0.0f, 1.0f);
        serotonin_->setLevel(moodLevel);
    }
    
    // Apply neuromodulatory effects to brain
    applyNeuromodulatoryEffects();
}

void AgentBrain::consolidateMemories(double timestep) {
    if (!memoryConsolidationEnabled_) return;
    
    // Check if it's time for consolidation (every N steps)
    consolidationTimer_ += timestep;
    if (consolidationTimer_ < CONSOLIDATION_INTERVAL) return;
    consolidationTimer_ = 0.0;
    
    // Select episodes for replay based on importance
    if (episodicMemory_) {
        size_t replayCount = std::min(static_cast<size_t>(plasticityModifier_ * 5), 20U);
        auto episodesForReplay = episodicMemory_->getEpisodesForReplay(replayCount);
        
        // Replay episodes for consolidation
        if (!episodesForReplay.empty()) {
            episodicMemory_->replaySequence(getEpisodeIds(episodesForReplay));
        }
    }
    
    // Strengthen important working memory traces
    if (workingMemory_) {
        workingMemory_->strengthenMemory(plasticityModifier_ * 0.1f);
    }
}

ActionType AgentBrain::actionToMotorCommand(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return ActionType::MoveForward;
        case MotorCommand::MoveBackward: return ActionType::MoveBackward;
        case MotorCommand::TurnLeft: return ActionType::TurnLeft;
        case MotorCommand::TurnRight: return ActionType::TurnRight;
        case MotorCommand::Interact: return ActionType::Interact;
        case MotorCommand::LookLeft: return ActionType::LookLeft;
        case MotorCommand::LookRight: return ActionType::LookRight;
        default: return ActionType::Wait;
    }
}

MotorCommand AgentBrain::selectActionWithPrediction(const std::vector<float>& state) {
    if (!predictionEnabled_ || !neuralPlanner_) {
        return decodeMotorCommand(); // Fallback to existing method
    }
    
    // Use neural planner for action sequence evaluation
    ActionType plannedAction = neuralPlanner_->planAction(state, expectedReward_);
    
    // Convert to motor command
    return actionToMotorCommand(plannedAction);
}

size_t AgentBrain::abstractReasoning(const std::vector<float>& pattern) {
    if (!conceptFormationEnabled_ || !conceptFormation_) {
        return 0; // No concept matching
    }
    
    // Find matching concept
    size_t conceptId = conceptFormation_->getMatchingConcept(pattern);
    
    if (conceptId > 0) {
        // Use concept prototype for generalization
        std::vector<float> prototype = conceptFormation_->getConceptPrototype(conceptId);
        
        // Store in working memory for use
        workingMemory_->store(prototype);
    }
    
    return conceptId;
}

std::vector<float> AgentBrain::generalizeFromConcept(size_t conceptId, 
                                                    const std::vector<float>& novelPattern) {
    if (!conceptFormationEnabled_ || !conceptFormation_) {
        return novelPattern;
    }
    
    // Get concept prototype
    std::vector<float> prototype = conceptFormation_->getConceptPrototype(conceptId);
    
    // Blend novel pattern with concept prototype
    std::vector<float> generalized = interpolate(novelPattern, prototype, 0.7f);
    
    return generalized;
}

void AgentBrain::patternCompletionRecall(const std::vector<float>& cuePattern) {
    if (!associativeMemoryEnabled_ || !associativeMemory_) return;
    
    // Retrieve associated patterns
    auto retrievedPatterns = associativeMemory_->retrieve(cuePattern, 3);
    
    // Use retrieved patterns for pattern completion
    for (const auto& pattern : retrievedPatterns) {
        workingMemory_->store(pattern);
    }
}

std::vector<float> AgentBrain::encodeSensoryState(const SensoryPercept& percept) {
    std::vector<float> encoded;
    // Encode percept into vector format for memory systems
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& intern = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Flatten all sensory data
    encoded.reserve(vision.size() + touch.size() + intern.size() + proprio.size());
    encoded.insert(encoded.end(), vision.begin(), vision.end());
    encoded.insert(encoded.end(), touch.begin(), touch.end());
    encoded.insert(encoded.end(), intern.begin(), intern.end());
    encoded.insert(encoded.end(), proprio.begin(), proprio.end());
    
    return encoded;
}

std::vector<float> AgentBrain::encodeActionPattern(const MotorCommand& action) {
    std::vector<float> pattern(6, 0.0f); // One-hot encoding for 6 actions
    switch (action) {
        case MotorCommand::MoveForward: pattern[0] = 1.0f; break;
        case MotorCommand::MoveBackward: pattern[1] = 1.0f; break;
        case MotorCommand::TurnLeft: pattern[2] = 1.0f; break;
        case MotorCommand::TurnRight: pattern[3] = 1.0f; break;
        case MotorCommand::Interact: pattern[4] = 1.0f; break;
        case MotorCommand::Wait: pattern[5] = 1.0f; break;
        case MotorCommand::LookLeft: pattern[0] = 1.0f; break;
        case MotorCommand::LookRight: pattern[3] = 1.0f; break;
    }
    return pattern;
}

std::vector<float> AgentBrain::interpolate(const std::vector<float>& a, const std::vector<float>& b, float factor) {
    std::vector<float> result;
    result.reserve(a.size());
    
    size_t minSize = std::min(a.size(), b.size());
    for (size_t i = 0; i < minSize; ++i) {
        result.push_back(a[i] * (1.0f - factor) + b[i] * factor);
    }
    
    return result;
}

void AgentBrain::updateNeuromodulationForDevelopment(DevelopmentalStage stage, float criticalPeriod) {
    if (!dopamine_) return;
    
    // Modulate neuromodulator levels based on developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High dopamine, low acetylcholine (exploration phase)
            dopamine_->setLevel(0.8f);
            if (acetylcholine_) acetylcholine_->setLevel(0.3f);
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Medium levels, high plasticity
            dopamine_->setLevel(0.5f * (1.0f + criticalPeriod));
            if (acetylcholine_) acetylcholine_->setLevel(0.6f);
            break;
            
        case DevelopmentalStage::Maturation:
            // Lower exploration, higher attention
            dopamine_->setLevel(0.3f);
            if (acetylcholine_) acetylcholine_->setLevel(0.8f);
            break;
            
        case DevelopmentalStage::Adult:
            // Stable levels, efficient processing
            dopamine_->setLevel(0.2f);
            if (acetylcholine_) acetylcholine_->setLevel(0.5f);
            break;
    }
}

std::vector<size_t> AgentBrain::getEpisodeIds(const std::vector<const EpisodicMemoryItem*>& episodes) {
    std::vector<size_t> ids;
    ids.reserve(episodes.size());
    
    for (const auto& episode : episodes) {
        // Find episode index in episodic memory
        if (episodicMemory_) {
            size_t index = episodicMemory_->getEpisodeIndex(episode);
            if (index != static_cast<size_t>(-1)) {
                ids.push_back(index);
            }
        }
    }
    
    return ids;
}

std::vector<float> AgentBrain::extractFeatures(const std::vector<float>& pattern, float reward) {
    return extractFeatures(pattern, reward, patternFeatures_);
}

std::vector<float> AgentBrain::getSensoryStateVector() {
    // Get current sensory state from percept history
    std::vector<float> state;
    if (!perceptHistory_.empty()) {
        state = encodeSensoryState(perceptHistory_.back());
    }
    return state;
}

std::vector<NeuronId> AgentBrain::getActiveNeuronIds() {
    std::vector<NeuronId> neuronIds;
    if (!brain_) return neuronIds;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* n : pop->getNeurons()) {
                if (n->getState().spikeCount > 0) {
                    neuronIds.push_back(n->getId());
                }
            }
        }
    }
    return neuronIds;
}

std::vector<float> AgentBrain::getNeuronActivations() {
    std::vector<float> activations;
    if (!brain_) return activations;
    
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* n : pop->getNeurons()) {
                activations.push_back(n->getState().membranePotential);
            }
        }
    }
    return activations;
}

void AgentBrain::applyNeuromodulatoryEffects() {
    if (!dopamine_ || !brain_) return;
    
    // Apply neuromodulatory effects to neural dynamics
    float dopamineLevel = dopamine_->getLevel();
    
    // Dopamine modulates neural excitability
    if (dopamineLevel > 0.0f) {
        float excitabilityMod = dopamineLevel * 0.5f;
        brain_->injectCurrentToNeurons(NeuronType::Internal, excitabilityMod);
    }
    
    // ACh modulates attention
    if (acetylcholine_) {
        float attentionLevel = acetylcholine_->getLevel();
        if (attentionLevel > 0.5f) {
            // Increase competition in working memory
            if (workingMemory_) {
                workingMemory_->runCompetition();
            }
        }
    }
    
    // NE modulates arousal
    if (norepinephrine_) {
        float arousalLevel = norepinephrine_->getLevel();
        if (arousalLevel > 0.7f) {
            // Increase neural gain
            brain_->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
        }
    }
}

void AgentBrain::reset() {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    currentStep_ = 0.0;
    energy_ = 0.0f;
    predictedReward_ = 0.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Clear episodic memory history
    perceptHistory_.clear();
    
    // Reset integration timers
    consolidationTimer_ = 0.0;
}
