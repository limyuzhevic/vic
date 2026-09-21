#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../prediction/PredictionSystem.hpp"
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
{
    // Initialize cognitive systems
    if (brain_) {
        // Get references to higher cognition systems
        planner_ = brain_->getPlanner();
        conceptFormation_ = brain_->getConceptFormation();
        predictionSystem_ = brain_->getPredictionSystem();
        developmentSystem_ = brain_->getDevelopmentSystem();
        
        // Initialize cognitive features
        cognitiveEnabled_ = true;
        
        // Set up cognitive integration points
        cognitiveIntegration_.reserve(8);
        
        // Add cognitive tracking variables
        cognitiveActive_ = 0.0f;
        conceptActivation_ = 0.0f;
        planProgress_ = 0.0f;
        attentionFocus_ = 0.0f;
        
        // Initialize cognitive novelty detection
        cognitiveNovelty_ = 0.0f;
        cognitiveNoveltyDecay_ = 0.98f;
        
        // Set up cognitive state tracking
        previousCognitiveState_.reserve(64);
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
    
    // Initialize cognitive state tracking
    previousCognitiveState_.assign(64, 0.0f);
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    
    // Log integrated cognition systems
    if (planner_) {
        NLM_LOG_INFO("AgentBrain: Integrated with NeuralPlanner");
    }
    if (conceptFormation_) {
        NLM_LOG_INFO("AgentBrain: Integrated with ConceptFormation");
    }
    if (predictionSystem_) {
        NLM_LOG_INFO("AgentBrain: Integrated with PredictionSystem");
    }
    if (developmentSystem_) {
        NLM_LOG_INFO("AgentBrain: Integrated with DevelopmentSystem");
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
void AgentBrain::updateHigherCognition(double timestep) {
    if (!brain_ || !cognitiveEnabled_) return;
    
    // Update cognitive activity based on neuromodulation
    // Dopamine enhances cognitive processing when within optimal range
    float dopamineOptimal = 0.3f;
    cognitiveActive_ = std::exp(-std::pow(dopamineLevel_ - dopamineOptimal, 2) / 0.1f);
    
    // Update concept activation based on novelty and reward prediction
    conceptActivation_ = (cognitiveNovelty_ + std::abs(predictionError_) * 0.5f) * 0.5f;
    conceptActivation_ = std::clamp(conceptActivation_, 0.0f, 1.0f);
    
    // Update planning progress based on developmental stage
    float planProgressFactor = plasticityModifier_;
    if (brain_->getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod) {
        planProgressFactor *= 1.5f;
    } else if (brain_->getDevelopmentalStage() == DevelopmentalStage::Adult) {
        planProgressFactor *= 0.5f;
    }
    
    planProgress_ = std::clamp(planProgress_ + planProgressFactor * (float)timestep * 0.01f, 0.0f, 1.0f);
    
    // Update attention focus based on sensory input and prediction error
    float attentionInput = noveltyLevel_ * 0.4f + std::abs(predictionError_) * 0.3f + curiosityLevel_ * 0.3f;
    attentionFocus_ = attentionFocus_ * 0.95f + attentionInput * 0.05f;
    attentionFocus_ = std::clamp(attentionFocus_, 0.0f, 1.0f);
    
    // Update cognitive novelty with decay
    cognitiveNovelty_ *= cognitiveNoveltyDecay_;
    
    // Update prediction error with noisy decay
    predictionError_ *= 0.99f;
    
    // Update cognitive integration points with current cognitive state
    std::vector<float> cognitiveState(64, 0.0f);
    // Fill cognitive state with relevant features
    cognitiveState[0] = cognitiveActive_;
    cognitiveState[1] = conceptActivation_;
    cognitiveState[2] = planProgress_;
    cognitiveState[3] = attentionFocus_;
    cognitiveState[4] = cognitiveNovelty_;
    cognitiveState[5] = dopamineLevel_;
    cognitiveState[6] = curiosityLevel_;
    cognitiveState[7] = noveltyLevel_;
    
    // Decay old cognitive state
    if (previousCognitiveState_.size() >= cognitiveState.size()) {
        for (size_t i = 0; i < cognitiveState.size(); ++i) {
            previousCognitiveState_[i] = previousCognitiveState_[i] * 0.95f + cognitiveState[i] * 0.05f;
        }
    }
    
    // Log cognitive processing
    if (cognitiveActive_ > 0.1f || conceptActivation_ > 0.1f) {
        NLM_LOG_INFO("AgentBrain: Cognitive processing - Active: " + 
                     std::to_string(cognitiveActive_) + 
                     ", Concepts: " + std::to_string(conceptActivation_) + 
                     ", Plan: " + std::to_string(planProgress_) + 
                     ", Attention: " + std::to_string(attentionFocus_));
    }
}

// Cognitive tracking methods
float AgentBrain::getAttentionFocus() const {
    return attentionFocus_;
}

float AgentBrain::getCognitiveNovelty() const {
    return cognitiveNovelty_;
}

// Get current self state for cognitive systems
std::vector<float> AgentBrain::getCurrentSelfState() const {
    std::vector<float> state(32, 0.0f);
    if (!brain_) return state;
    
    // Fill self state with current cognitive and neural properties
    state[0] = cognitiveActive_;
    state[1] = conceptActivation_;
    state[2] = planProgress_;
    state[3] = attentionFocus_;
    state[4] = cognitiveNovelty_;
    state[5] = dopamineLevel_;
    state[6] = curiosityLevel_;
    state[7] = noveltyLevel_;
    state[8] = predictionError_;
    
    // Add motor system state
    state[9] = getMotorCommandActivity(MotorCommand::MoveForward);
    state[10] = getMotorCommandActivity(MotorCommand::MoveBackward);
    state[11] = getMotorCommandActivity(MotorCommand::TurnLeft);
    state[12] = getMotorCommandActivity(MotorCommand::TurnRight);
    state[13] = getMotorCommandActivity(MotorCommand::Interact);
    state[14] = getMotorCommandActivity(MotorCommand::Wait);
    
    // Add sensory system state
    state[15] = getSensoryActivity(SensoryType::Vision);
    state[16] = getSensoryActivity(SensoryType::Touch);
    state[17] = getSensoryActivity(SensoryType::Internal);
    state[18] = getSensoryActivity(SensoryType::Proprioception);
    
    // Add developmental state
    state[19] = (float)getDevelopmentalStage();
    state[20] = (float)plasticityModifier_;
    
    // Add previous cognitive state
    for (size_t i = 0; i < std::min(previousCognitiveState_.size(), size_t(13)); ++i) {
        state[21 + i] = previousCognitiveState_[i];
    }
    
    return state;
}

// Process cognitive input and update internal state
void AgentBrain::processCognitiveInput(const std::vector<float>& input) {
    if (!brain_ || input.empty()) return;
    
    // Extract cognitive features from input
    float inputAttention = input.size() > 0 ? input[0] : 0.0f;
    float inputCuriosity = input.size() > 1 ? input[1] : 0.0f;
    float inputPlan = input.size() > 2 ? input[2] : 0.0f;
    float inputNovelty = input.size() > 3 ? input[3] : 0.0f;
    
    // Update attention focus with cognitive input
    attentionFocus_ = attentionFocus_ * 0.9f + inputAttention * 0.1f;
    attentionFocus_ = std::clamp(attentionFocus_, 0.0f, 1.0f);
    
    // Update curiosity with cognitive input
    if (curiosityEnabled_) {
        curiosityLevel_ = curiosityLevel_ * 0.95f + inputCuriosity * 0.05f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    // Update planning progress with cognitive input
    planProgress_ = planProgress_ * 0.9f + inputPlan * 0.1f;
    planProgress_ = std::clamp(planProgress_, 0.0f, 1.0f);
    
    // Update cognitive novelty with input
    cognitiveNovelty_ = cognitiveNovelty_ * cognitiveNoveltyDecay_ + inputNovelty * 0.05f;
    cognitiveNovelty_ = std::clamp(cognitiveNovelty_, 0.0f, 1.0f);
    
    // Update previous cognitive state
    std::vector<float> currentCognitiveState(64, 0.0f);
    currentCognitiveState[0] = cognitiveActive_;
    currentCognitiveState[1] = conceptActivation_;
    currentCognitiveState[2] = planProgress_;
    currentCognitiveState[3] = attentionFocus_;
    currentCognitiveState[4] = cognitiveNovelty_;
    currentCognitiveState[5] = dopamineLevel_;
    currentCognitiveState[6] = curiosityLevel_;
    currentCognitiveState[7] = noveltyLevel_;
    
    // Integrate with cognitive systems if available
    if (planner_ && input.size() > 8) {
        // Pass planning-related input to neural planner
        std::vector<float> planningInput(input.begin(), input.begin() + 16);
        planner_->updatePlanning(planningInput);
    }
    
    if (conceptFormation_ && input.size() > 16) {
        // Pass concept-related input to concept formation
        std::vector<float> conceptInput(input.begin() + 16, input.begin() + 32);
        conceptFormation_->updateConcepts(conceptInput);
    }
    
    if (predictionSystem_ && input.size() > 32) {
        // Pass prediction-related input to prediction system
        std::vector<float> predictionInput(input.begin() + 32, input.begin() + 48);
        predictionSystem_->updatePredictions(predictionInput);
    }
    
    if (developmentSystem_ && input.size() > 48) {
        // Pass development-related input to development system
        std::vector<float> developmentInput(input.begin() + 48, std::min(input.begin() + 64, input.end()));
        developmentSystem_->updateDevelopment(developmentInput);
    }
    
    // Update cognitive integration points
    for (const auto& integrationFunc : cognitiveIntegration_) {
        integrationFunc(currentCognitiveState);
    }
    
    // Log cognitive processing
    NLM_LOG_DEBUG("AgentBrain: Cognitive input processed - Attention: " + 
                  std::to_string(attentionFocus_) + 
                  ", Curiosity: " + std::to_string(curiosityLevel_));
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

float AgentBrain::getMotorCommandActivity(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward:
            return motorForward_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(motorForward_);
        case MotorCommand::MoveBackward:
            return motorBackward_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(motorBackward_);
        case MotorCommand::TurnLeft:
            return motorTurnLeft_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(motorTurnLeft_);
        case MotorCommand::TurnRight:
            return motorTurnRight_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(motorTurnRight_);
        case MotorCommand::Interact:
            return motorInteract_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(motorInteract_);
        case MotorCommand::Wait:
            return motorWait_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(motorWait_);
        default:
            return 0.0f;
    }
}

float AgentBrain::getSensoryActivity(SensoryType type) const {
    switch (type) {
        case SensoryType::Vision:
            return sensoryVision_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(sensoryVision_);
        case SensoryType::Touch:
            return sensoryTouch_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(sensoryTouch_);
        case SensoryType::Internal:
            return sensoryInternal_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(sensoryInternal_);
        case SensoryType::Proprioception:
            return sensoryProprioception_.empty() ? 0.0f : decodeFromMotorNeuronsMotorGroup(sensoryProprioception_);
        default:
            return 0.0f;
    }
}

float AgentBrain::decodeFromMotorNeuronsMotorGroup(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

} // namespace nlm
