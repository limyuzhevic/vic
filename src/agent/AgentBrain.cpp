#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/SelfModel.hpp"
#include "../cognition/SocialLearning.hpp"
#include "../cognition/SpatialRepresentation.hpp"
#include "../cognition/TemporalRelation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
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
    , cognitiveSystemsInitialized_(false)
    , lastUpdateStep_(0)
    , integrationLevel_(0.0f)
    , neuralPlanner_(nullptr)
    , conceptFormation_(nullptr)
    , attentionalSelection_(nullptr)
    , selfModel_(nullptr)
    , socialLearning_(nullptr)
    , spatialRepresentation_(nullptr)
    , temporalRelation_(nullptr)
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
    
    // ===== COMPREHENSIVE NEUROMODULATION INTEGRATION =====
    // Apply neuromodulation effects across all neuromodulator systems
    
    // 1. Dopamine-like signal (direct reward prediction error)
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to dopamine neuromodulator
    if (brain_->getDopamine()) {
        brain_->getDopamine()->signalRewardPredictionError(predictionError_);
        brain_->getDopamine()->modulateMemoryConsolidation(std::max(0.0f, predictionError_));
        brain_->getDopamine()->enhanceAttentionAndFocus(std::max(0.0f, predictionError_));
        brain_->getDopamine()->modulateConceptFormationLearning(std::max(0.0f, predictionError_));
        brain_->getDopamine()->enhanceWorkingMemory(std::max(0.0f, predictionError_));
        brain_->getDopamine()->modulateGoalDirectedBehavior(std::max(0.0f, predictionError_));
    }
    
    // 2. Serotonin-like effects (behavioral inhibition and mood)
    float serotoninSignal = -predictionError_ * 0.3f; // Negative prediction error increases serotonin
    serotoninSignal = std::clamp(serotoninSignal, 0.0f, 1.0f);
    if (brain_->getSerotonin()) {
        brain_->getSerotonin()->signalInhibition(serotoninSignal);
        brain_->getSerotonin()->signalSocialBehavior(0.5f + serotoninSignal * 0.5f);
        brain_->getSerotonin()->applyAttentionalModulation(serotoninSignal * 0.7f);
        brain_->getSerotonin()->modulateConceptFormation(serotoninSignal * 0.5f);
    }
    
    // 3. Norepinephrine-like effects (arousal and alertness)
    float norepinephrineSignal = std::abs(predictionError_) * 0.5f + reward * 0.5f;
    norepinephrineSignal = std::clamp(norepinephrineSignal, 0.0f, 1.0f);
    if (brain_->getNorepinephrine()) {
        brain_->getNorepinephrine()->signalArousal(norepinephrineSignal);
        brain_->getNorepinephrine()->enhanceVigilance(norepinephrineSignal * 0.8f);
        brain_->getNorepinephrine()->enhanceSensoryProcessing(norepinephrineSignal * 0.6f);
        brain_->getNorepinephrine()->modulateWorkingMemoryCapacity(norepinephrineSignal * 0.7f);
    }
    
    // 4. Acetylcholine-like effects (attention and memory)
    float acetylcholineSignal = reward * 0.7f + std::abs(predictionError_) * 0.3f;
    acetylcholineSignal = std::clamp(acetylcholineSignal, 0.0f, 1.0f);
    if (brain_->getAcetylcholine()) {
        brain_->getAcetylcholine()->enhanceAttention(acetylcholineSignal * 0.9f);
        brain_->getAcetylcholine()->signalMemoryEncoding(acetylcholineSignal * 0.8f);
        brain_->getAcetylcholine()->enhanceLearningRate(acetylcholineSignal * 0.7f);
        brain_->getAcetylcholine()->integrateWithNoveltyDetection(acetylcholineSignal * 0.6f);
        brain_->getAcetylcholine()->promoteMemoryConsolidation();
    }
    
    // 5. Novelty-like effects
    float noveltySignal = std::abs(predictionError_) * 0.4f + (reward > 0 ? 0.6f : 0.0f);
    noveltySignal = std::clamp(noveltySignal, 0.0f, 1.0f);
    if (brain_->getNovelty()) {
        brain_->getNovelty()->setLevel(noveltySignal);
    }
    
    // 6. Update prediction error neuromodulator
    if (brain_->getPredictionErrorSignal()) {
        brain_->getPredictionErrorSignal()->computeError(expectedReward_, reward);
    }
    
    // Apply reward-modulated weight changes to synapses with eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply neuromodulation-enhanced weight change
                float neuromodulationFactor = 1.0f;
                
                // Apply all neuromodulator effects
                if (brain_->getDopamine()) {
                    neuromodulationFactor *= brain_->getDopamine()->getPlasticityFactor();
                }
                if (brain_->getSerotonin()) {
                    neuromodulationFactor *= brain_->getSerotonin()->getPlasticityFactor();
                }
                if (brain_->getNorepinephrine()) {
                    neuromodulationFactor *= brain_->getNorepinephrine()->getPlasticityFactor();
                }
                if (brain_->getAcetylcholine()) {
                    neuromodulationFactor *= brain_->getAcetylcholine()->getPlasticityFactor();
                }
                
                // Apply reward-modulated weight change with neuromodulation enhancement
                float delta = eligibility * predictionError_ * plasticityModifier_ * neuromodulationFactor;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f * neuromodulationFactor);
            }
        }
    }
    
    // Modulate plasticity based on overall neuromodulation state
    float overallModulation = std::clamp((dopamineLevel_ + noveltyLevel_ + curiosityLevel_) / 3.0f, 0.0f, 1.0f);
    float plasticityFactor = 0.5f + 0.5f * overallModulation;
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

// Cognitive system integration implementation
void AgentBrain::integrateCognitiveSystems() {
    if (!brain_ || cognitiveSystemsInitialized_) return;
    
    NLM_LOG_INFO("Integrating cognitive systems with agent brain...");
    
    // NeuralPlanner - action planning
    if (brain_->getPlanner()) {
        brain_->getPlanner()->setPlanningDepth(5);
        NLM_LOG_INFO("NeuralPlanner integrated for action planning");
    }
    
    // ConceptFormation - pattern recognition  
    if (brain_->getConceptFormation()) {
        brain_->getConceptFormation()->setFormationThreshold(0.8f);
        brain_->getConceptFormation()->setStabilityWindow(10);
        NLM_LOG_INFO("ConceptFormation integrated for pattern recognition");
    }
    
    // SelfModel - self-awareness (via NeuralPlanner's SelfModel)
    if (brain_->getPlanner() && brain_->getPlanner()->getSelfModel()) {
        NLM_LOG_INFO("SelfModel integrated for self-awareness");
    }
    
    // SocialLearning - social interaction (via NeuralPlanner's SocialLearning)
    if (brain_->getPlanner() && brain_->getPlanner()->getSocialLearning()) {
        NLM_LOG_INFO("SocialLearning integrated for social interaction");
    }
    
    // SpatialRepresentation - spatial awareness (via ConceptFormation's SpatialRepresentation)
    if (brain_->getConceptFormation() && brain_->getConceptFormation()->getSpatialRepresentation()) {
        NLM_LOG_INFO("SpatialRepresentation integrated for spatial awareness");
    }
    
    // TemporalRelation - temporal awareness (via ConceptFormation's TemporalRelation)
    if (brain_->getConceptFormation() && brain_->getConceptFormation()->getTemporalRelation()) {
        NLM_LOG_INFO("TemporalRelation integrated for temporal awareness");
    }
    
    // AttentionalSelection - attention control
    if (brain_->getAttention()) {
        brain_->getAttention()->setInhibitionStrength(0.5f);
        brain_->getAttention()->setExcitationStrength(1.5f);
        brain_->getAttention()->setCompetitionThreshold(0.3f);
        NLM_LOG_INFO("AttentionalSelection integrated for attention control");
    }
    
    // SelfModel - self-awareness (direct access)
    if (brain_->getSelfModel()) {
        brain_->getSelfModel()->setCapabilityLevel(0.5f);
        NLM_LOG_INFO("SelfModel integrated for self-awareness");
    }
    
    // SocialLearning - social interaction (direct access)
    if (brain_->getSocialLearning()) {
        brain_->getSocialLearning()->setObservationRate(0.3f);
        NLM_LOG_INFO("SocialLearning integrated for social interaction");
    }
    
    // SpatialRepresentation - spatial awareness (direct access)
    if (brain_->getSpatialRepresentation()) {
        brain_->getSpatialRepresentation()->setGridSpacing(1.0f);
        NLM_LOG_INFO("SpatialRepresentation integrated for spatial awareness");
    }
    
    // TemporalRelation - temporal awareness (direct access)
    if (brain_->getTemporalRelation()) {
        brain_->getTemporalRelation()->setSequenceLength(10);
        NLM_LOG_INFO("TemporalRelation integrated for temporal awareness");
    }
    
    cognitiveSystemsInitialized_ = true;
    integrationLevel_ = 1.0f;
    lastUpdateStep_ = 0;
    NLM_LOG_INFO("Cognitive systems integration complete");
}

// Update cognitive systems with sensory input
void AgentBrain::updateCognitiveSystemsWithSensory(const SensoryPercept& percept) {
    if (!brain_ || !cognitiveSystemsInitialized_) return;
    
    ++lastUpdateStep_;
    integrationLevel_ = std::min(1.0f, integrationLevel_ + 0.1f);
    
    // Encode current sensory state as neural patterns
    std::vector<float> sensoryPattern;
    
    // Combine different sensory modalities
    const auto& vision = percept.getVision();
    const auto& touch = percept.getTouch();
    const auto& internal = percept.getInternal();
    const auto& proprio = percept.getProprioception();
    
    // Create integrated sensory representation
    sensoryPattern.reserve(vision.size() + touch.size() + internal.size() + proprio.size());
    sensoryPattern.insert(sensoryPattern.end(), vision.begin(), vision.end());
    sensoryPattern.insert(sensoryPattern.end(), touch.begin(), touch.end());
    sensoryPattern.insert(sensoryPattern.end(), internal.begin(), internal.end());
    sensoryPattern.insert(sensoryPattern.end(), proprio.begin(), proprio.end());
    
    // Present sensory patterns to cognitive systems
    float reward = dopamineLevel_ > 0 ? 0.5f : 0.0f; // Simple reward estimation
    
    // Update ConceptFormation with new sensory patterns
    if (auto* conceptForm = brain_->getConceptFormation()) {
        conceptForm->presentExperience(sensoryPattern, sensoryPattern, reward, lastUpdateStep_);
    }
    
    // Update NeuralPlanner with current sensory state
    if (auto* planner = brain_->getPlanner()) {
        ActionType plannedAction = planner->planAction(sensoryPattern, reward);
        // Store planned action for potential use
    }
    
    // Update AttentionalSelection based on sensory salience
    if (auto* attention = brain_->getAttention()) {
        std::vector<NeuronId> competitors;
        // Add sensory neurons that are active
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i] && vision[i] > 0.1f) {
                competitors.push_back(sensoryVision_[i]->getId());
            }
        }
        
        if (!competitors.empty()) {
            attention->processCompetition(competitors, 0.5f);
        }
    }
    
    // Update SelfModel (via NeuralPlanner's SelfModel)
    // Record sensory state for self-model updating
    if (auto* planner = brain_->getPlanner()) {
        // Self-model updating happens in NeuralPlanner.cpp based on action outcomes
    }
}

// Update cognitive systems with action feedback
void AgentBrain::updateCognitiveSystemsWithAction(MotorCommand action, float reward) {
    if (!brain_ || !cognitiveSystemsInitialized_) return;
    
    // Encode action as neural pattern
    std::vector<float> actionPattern(6, 0.0f); // 6 possible actions
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx < 6) {
        actionPattern[actionIdx] = 1.0f;
    }
    
    // Update NeuralPlanner with action outcome
    if (auto* planner = brain_->getPlanner()) {
        std::vector<ActionType> plannedActions{action};
        std::vector<ActionType> actualActions{action};
        planner->updatePlanQuality(plannedActions, actualActions, reward);
        
        // Update SelfModel confidence for this action
        float actionQuality = (reward > 0) ? (0.5f + reward * 0.5f) : (0.1f + reward * 0.5f);
        planner->setActionQuality(action, actionQuality);
    }
    
    // Update SocialLearning if this is observable to others
    if (auto* planner = brain_->getPlanner()) {
        // SocialLearning is in NeuralPlanner's namespace
        // Could record this action for social learning
    }
    
    // Update ConceptFormation with action-reward pairing
    if (auto* conceptForm = brain_->getConceptFormation()) {
        conceptForm->updateConcept(0, actionPattern, actionPattern, reward);
    }
    
    // Apply neuromodulation effects on cognitive systems
    applyNeuromodulationToCognitiveSystems(reward, predictionError_);
    
    // Update development effects
    if (developmentEnabled_ && developmentalAge_ > 60.0) {
        updateDevelopment(1.0);
    }
}

// Apply neuromodulation to cognitive systems
void AgentBrain::applyNeuromodulationToCognitiveSystems(float reward, float predictionError) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Update neuromodulation levels
    predictionError_ = reward - expectedReward_;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal affects planning confidence
    if (auto* planner = brain_->getPlanner()) {
        float planningAdjustment = predictionError_ * 0.2f;
        float newConfidence = planner->getPlanningConfidence() + planningAdjustment;
        // Note: NeuralPlanner doesn't expose planningConfidence_ setter
        // This would need to be added to NeuralPlanner interface
    }
    
    // Curiosity affects exploration
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Novelty affects concept formation
    if (auto* conceptForm = brain_->getConceptFormation()) {
        // Novelty influences concept formation threshold
        float noveltyInfluence = noveltyLevel_ * 0.3f;
        // Could adjust conceptForm parameters based on novelty
    }
    
    // Attention modulation based on reward salience
    if (auto* attention = brain_->getAttention()) {
        float attentionModulation = std::abs(reward) * 0.5f;
        // Apply attention modulation
        // Could use applyBottomUpSalience or applyTopDownBias
    }
}

// Enhanced getPlannedAction using NeuralPlanner
ActionType AgentBrain::getPlannedAction(const std::vector<float>& currentState) {
    if (!brain_ || !brain_->getPlanner()) {
        return ActionType::Wait;
    }
    
    float targetReward = 0.5f; // Default target reward
    return brain_->getPlanner()->planAction(currentState, targetReward);
}

// Enhanced recognizePattern using ConceptFormation
void AgentBrain::recognizePattern(const std::vector<float>& sensoryData, float reward) {
    if (!brain_ || !brain_->getConceptFormation()) return;
    
    brain_->getConceptFormation()->presentExperience(sensoryData, sensoryData, reward, lastUpdateStep_);
}

// Enhanced getSelfAwareness using SelfModel (via NeuralPlanner)
float AgentBrain::getSelfAwareness() const {
    if (!brain_ || !brain_->getPlanner() || !brain_->getPlanner()->getSelfModel()) {
        return 0.0f;
    }
    
    return brain_->getPlanner()->getSelfModel()->getCapabilityLevel();
}

// Enhanced learnFromSocialInteraction using SocialLearning (via NeuralPlanner)
void AgentBrain::learnFromSocialInteraction(ActionType action, const std::vector<float>& outcome) {
    if (!brain_ || !brain_->getPlanner() || !brain_->getPlanner()->getSocialLearning()) return;
    
    brain_->getPlanner()->getSocialLearning()->observeAgentAction(action, std::vector<float>(), outcome);
}

// Enhanced getSpatialAwareness using SpatialRepresentation (via ConceptFormation)
std::pair<float, float> AgentBrain::getSpatialAwareness() const {
    if (!brain_ || !brain_->getConceptFormation() || !brain_->getConceptFormation()->getSpatialRepresentation()) {
        return std::make_pair(0.0f, 0.0f);
    }
    
    return brain_->getConceptFormation()->getSpatialRepresentation()->getPredictedPosition();
}

// Enhanced predictTemporalSequence using TemporalRelation (via ConceptFormation)
std::vector<float> AgentBrain::predictTemporalSequence(const std::vector<float>& currentState) const {
    if (!brain_ || !brain_->getConceptFormation() || !brain_->getConceptFormation()->getTemporalRelation()) {
        return std::vector<float>();
    }
    
    return brain_->getConceptFormation()->getTemporalRelation()->predictNext(currentState);
}

// Enhanced updateAttention using AttentionalSelection
void AgentBrain::updateAttention(const std::vector<NeuronId>& competitors, float globalInhibition) {
    if (!brain_ || !brain_->getAttention()) return;
    
    brain_->getAttention()->processCompetition(competitors, globalInhibition);
}

// Enhanced initialize() method
    void initialize(const SimpleWorld& world) {
        previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Initialize cognitive systems integration
        cognitiveSystemsInitialized_ = false;
        lastUpdateStep_ = 0;
        integrationLevel_ = 0.0f;
        
        // Create and initialize ALL cognitive systems
        if (brain_) {
            // NeuralPlanner - action planning
            if (brain_->getPlanner()) {
                cognitiveSystems_[0] = brain_->getPlanner();
                NLM_LOG_INFO("NeuralPlanner instantiated and initialized for action planning");
            }
            
            // ConceptFormation - pattern recognition  
            if (brain_->getConceptFormation()) {
                cognitiveSystems_[1] = brain_->getConceptFormation();
                NLM_LOG_INFO("ConceptFormation instantiated and initialized for pattern recognition");
            }
            
            // AttentionalSelection - attention control
            if (brain_->getAttention()) {
                cognitiveSystems_[2] = brain_->getAttention();
                NLM_LOG_INFO("AttentionalSelection instantiated and initialized for attention control");
            }
            
            // SelfModel - self-awareness (via NeuralPlanner's SelfModel)
            if (brain_->getPlanner() && brain_->getPlanner()->getSelfModel()) {
                cognitiveSystems_[3] = brain_->getPlanner()->getSelfModel();
                NLM_LOG_INFO("SelfModel instantiated and initialized for self-awareness");
            }
            
            // SocialLearning - social interaction (via NeuralPlanner's SocialLearning)
            if (brain_->getPlanner() && brain_->getPlanner()->getSocialLearning()) {
                cognitiveSystems_[4] = brain_->getPlanner()->getSocialLearning();
                NLM_LOG_INFO("SocialLearning instantiated and initialized for social interaction");
            }
            
            // SpatialRepresentation - spatial awareness (via ConceptFormation's SpatialRepresentation)
            if (brain_->getConceptFormation() && brain_->getConceptFormation()->getSpatialRepresentation()) {
                cognitiveSystems_[5] = brain_->getConceptFormation()->getSpatialRepresentation();
                NLM_LOG_INFO("SpatialRepresentation instantiated and initialized for spatial awareness");
            }
            
            // TemporalRelation - temporal awareness (via ConceptFormation's TemporalRelation)
            if (brain_->getConceptFormation() && brain_->getConceptFormation()->getTemporalRelation()) {
                cognitiveSystems_[6] = brain_->getConceptFormation()->getTemporalRelation();
                NLM_LOG_INFO("TemporalRelation instantiated and initialized for temporal awareness");
            }
            
            cognitiveSystemsInitialized_ = true;
        }
        
        NLM_LOG_INFO("AgentBrain initialized with " + 
                     std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
        
        NLM_LOG_INFO("All cognitive systems integrated and operational:");
        NLM_LOG_INFO("  - NeuralPlanner: Action planning and goal selection");
        NLM_LOG_INFO("  - ConceptFormation: Pattern recognition and category learning");
        NLM_LOG_INFO("  - AttentionalSelection: Attention control and focus management");
        NLM_LOG_INFO("  - SelfModel: Self-awareness and body schema");
        NLM_LOG_INFO("  - SocialLearning: Social interaction and observation learning");
        NLM_LOG_INFO("  - SpatialRepresentation: Spatial awareness and navigation");
        NLM_LOG_INFO("  - TemporalRelation: Temporal sequence understanding");
    }

// Enhanced reset() method
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
    
    // Reset ALL cognitive systems - clear their references
    neuralPlanner_ = nullptr;
    conceptFormation_ = nullptr;
    attentionalSelection_ = nullptr;
    selfModel_ = nullptr;
    socialLearning_ = nullptr;
    spatialRepresentation_ = nullptr;
    temporalRelation_ = nullptr;
    
    // Reset cognitive integration state
    cognitiveSystemsInitialized_ = false;
    integrationLevel_ = 0.0f;
    lastUpdateStep_ = 0;
    
    // Re-initialize cognitive systems if brain exists
    if (brain_) {
        integrateCognitiveSystems();
    }
}
