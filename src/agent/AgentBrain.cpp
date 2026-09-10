#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
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
    , planningConfidence_(0.0f)
    , neuralPlanner_(nullptr)
    , conceptFormation_(nullptr)
    , attentionalSelection_(nullptr)
    , predictionSystem_(nullptr)
{
    // Initialize cognitive systems
    integrateCognitiveSystems();
    
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

void AgentBrain::integrateCognitiveSystems() {
    if (!brain_) return;
    
    // Initialize cognitive systems
    neuralPlanner_ = brain_->getPlanner();
    conceptFormation_ = brain_->getConceptFormation();
    attentionalSelection_ = brain_->getAttention();
    predictionSystem_ = brain_->getPredictionSystem();
    
    if (neuralPlanner_) {
        neuralPlanner_->initialize(brain_.get());
        NLM_LOG_INFO("Neural planner integrated");
    }
    
    if (conceptFormation_) {
        conceptFormation_->initialize(brain_.get());
        NLM_LOG_INFO("Concept formation integrated");
    }
    
    if (attentionalSelection_) {
        attentionalSelection_->initialize(brain_.get());
        NLM_LOG_INFO("Attentional selection integrated");
    }
    
    if (predictionSystem_) {
        NLM_LOG_INFO("Prediction system integrated");
    }
}

void AgentBrain::updateConceptFormation(const SensoryPercept& percept) {
    if (!conceptFormation_) return;
    
    // Extract sensory features from percept
    std::vector<float> pattern;
    std::vector<float> features;
    
    // Vision features (flatten vision data)
    const auto& vision = percept.getVision();
    pattern.insert(pattern.end(), vision.begin(), vision.end());
    
    // Extract basic features
    features.resize(4, 0.0f);
    if (!vision.empty()) {
        float avgBrightness = 0.0f;
        float contrast = 0.0f;
        for (float v : vision) {
            avgBrightness += v;
        }
        avgBrightness /= vision.size();
        
        for (float v : vision) {
            contrast += (v - avgBrightness) * (v - avgBrightness);
        }
        contrast = std::sqrt(contrast / vision.size());
        
        features[0] = avgBrightness;      // Brightness
        features[1] = contrast;           // Contrast
        features[2] = noveltyLevel_;      // Novelty
        features[3] = curiosityLevel_;    // Curiosity
    }
    
    // Present experience to concept formation
    size_t conceptId = conceptFormation_->presentExperience(pattern, features, expectedReward_, 0);
    
    // Use concept for action selection if available
    if (conceptId > 0) {
        const DiscoveredConcept* concept = conceptFormation_->getConcept(conceptId);
        if (concept && concept->avgStability > 0.5f) {
            // Concept is stable, use its knowledge
            const auto& prototype = conceptFormation_->getConceptPrototype(conceptId);
            
            // Convert concept features to action preference
            if (!prototype.empty() && prototype[0] > 0.7f) {
                // Bright concept -> interact
                currentGoal_.clear();
                currentGoal_.push_back(0.0f); // Approach goal
            }
        }
    }
}

void AgentBrain::updatePredictionSystem(const SensoryPercept& percept) {
    if (!predictionSystem_) return;
    
    // Create sensory input from percept
    SensoryInput currentState;
    
    // Extract vision data
    const auto& vision = percept.getVision();
    currentState.setVision(vision);
    
    // Extract other sensory data
    currentState.setTouch(percept.getTouch());
    currentState.setInternal(percept.getInternal());
    currentState.setProprioception(percept.getProprioception());
    
    // Make prediction
    auto predictedState = predictionSystem_->predictNextState(currentState);
    
    // Update with actual observation
    predictionSystem_->updatePredictions(*predictedState, currentState);
    
    // Get prediction error
    predictionError_ = predictionSystem_->getPredictionError();
    
    // Use prediction error for learning
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

void AgentBrain::applyAttentionalSelection(const SensoryPercept& percept) {
    if (!attentionalSelection_) return;
    
    // Collect competing neuron IDs from different sensory modalities
    std::vector<NeuronId> competitors;
    
    // Add vision neurons
    for (Neuron* n : sensoryVision_) {
        if (n) competitors.push_back(n->getId());
    }
    
    // Add touch neurons
    for (Neuron* n : sensoryTouch_) {
        if (n) competitors.push_back(n->getId());
    }
    
    // Add internal neurons
    for (Neuron* n : sensoryInternal_) {
        if (n) competitors.push_back(n->getId());
    }
    
    // Process competition to select winners
    std::vector<NeuronId> winners = attentionalSelection_->processCompetition(
        competitors, 0.5f // Global inhibition
    );
    
    // Update attended regions based on winners
    attendedRegions_.clear();
    for (NeuronId winnerId : winners) {
        // Find which region this neuron belongs to
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (Neuron* n : pop->getNeurons()) {
                    if (n && n->getId() == winnerId) {
                        attendedRegions_.push_back(region->getId());
                    }
                }
            }
        }
    }
}

void AgentBrain::executeNeuralPlanning() {
    if (!neuralPlanner_) return;
    
    // Convert current sensory state to planning state
    std::vector<float> currentState;
    
    // Use vision as primary planning input
    const auto& vision = previousVision_;
    currentState.insert(currentState.end(), vision.begin(), vision.end());
    
    // Add neuromodulatory state
    currentState.push_back(dopamineLevel_);
    currentState.push_back(curiosityLevel_);
    currentState.push_back(predictionError_);
    
    // Set goal based on attended regions
    currentGoal_.clear();
    if (!attendedRegions_.empty()) {
        // Focus on first attended region
        currentGoal_.push_back(0.5f); // Moderate goal value
    }
    
    // Plan action
    ActionType plannedAction = neuralPlanner_->planAction(currentState, 0.5f);
    
    // Convert action to motor command
    MotorCommand plannedMotorCmd;
    switch (plannedAction) {
        case ActionType::MoveForward: plannedMotorCmd = MotorCommand::MoveForward; break;
        case ActionType::MoveBackward: plannedMotorCmd = MotorCommand::MoveBackward; break;
        case ActionType::TurnLeft: plannedMotorCmd = MotorCommand::TurnLeft; break;
        case ActionType::TurnRight: plannedMotorCmd = MotorCommand::TurnRight; break;
        case ActionType::LookLeft: plannedMotorCmd = MotorCommand::LookLeft; break;
        case ActionType::LookRight: plannedMotorCmd = MotorCommand::LookRight; break;
        case ActionType::Interact: plannedMotorCmd = MotorCommand::Interact; break;
        case ActionType::Wait: plannedMotorCmd = MotorCommand::Wait; break;
        default: plannedMotorCmd = MotorCommand::Wait; break;
    }
    
    // Store planned action for potential use
    // In a real implementation, this would be used instead of basic decoding
    
    // Update planning confidence
    planningConfidence_ = neuralPlanner_->getPlanningConfidence();
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
