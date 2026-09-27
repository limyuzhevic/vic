// AgentBrainEnhanced.cpp - Enhanced AgentBrain with cognition integration implementation
#include "AgentBrainEnhanced.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrainEnhanced::AgentBrainEnhanced(std::shared_ptr<Brain> brain)
    : AgentBrain(brain)
    , planningConfidence_(0.0f)
    , conceptClarity_(0.0f)
    , attentionFocus_(0.0f)
    , planningEnabled_(false)
    , conceptFormationEnabled_(false)
    , attentionEnabled_(false)
    , predictionEnabled_(false)
{
    // Initialize cognitive systems
    planner_ = std::make_shared<NeuralPlanner>();
    conceptFormation_ = std::make_shared<ConceptFormation>();
    attention_ = std::make_shared<AttentionalSelection>();
    predictionSystem_ = std::make_shared<PredictionSystem>();
    curiosity_ = std::make_shared<Curiosity>();
    dopamine_ = std::make_shared<Dopamine>();
    workingMemory_ = std::make_shared<NeuralWorkingMemory>();
    
    // Initialize cognitive feature buffers
    recentActions_.reserve(100);
    recentRewards_.reserve(100);
    recentObservations_.reserve(256);  // Vision input size
    
    NLM_LOG_INFO("AgentBrainEnhanced initialized with cognitive systems integration");
}

AgentBrainEnhanced::~AgentBrainEnhanced() = default;

void AgentBrainEnhanced::initialize(const SimpleWorld& world) {
    // Call parent initialization
    AgentBrain::initialize(world);
    
    // Initialize cognitive systems with brain reference
    if (brain_) {
        if (planner_) {
            planner_->initialize(brain_.get());
        }
        if (conceptFormation_) {
            conceptFormation_->initialize(brain_.get());
        }
        if (attention_) {
            attention_->initialize(brain_.get());
        }
        if (predictionSystem_) {
            // Prediction system doesn't have initialize method currently
        }
        if (curiosity_) {
            curiosity_->initialize(brain_.get());
        }
        if (dopamine_) {
            dopamine_->initialize(brain_.get());
        }
        if (workingMemory_) {
            workingMemory_->initialize(brain_.get());
            workingMemory_->setCapacity(1000);  // Larger capacity for advanced agents
        }
    }
    
    NLM_LOG_INFO("AgentBrainEnhanced cognitive systems initialized");
}

void AgentBrainEnhanced::processSensoryInput(const SensoryPercept& percept) {
    // Process sensory input with cognitive enhancement
    AgentBrain::processSensoryInput(percept);
    
    if (!brain_) return;
    
    // Extract cognitive features from sensory input
    extractCognitiveFeatures();
    
    // Update prediction system
    if (predictionEnabled_ && predictionSystem_) {
        // Predict expected sensory input based on current cognitive state
        std::vector<float> expectedInput = predictExpectedInput();
        std::vector<float> actualInput = percept.getVision();
        
        if (!actualInput.empty() && !expectedInput.empty()) {
            float predictionError = computePredictionError(expectedInput, actualInput);
            
            // Update prediction system with error
            if (predictionSystem_) {
                predictionSystem_->updatePredictionError(predictionError);
            }
        }
    }
    
    // Update working memory with enriched sensory information
    if (workingMemory_) {
        storeSensoryObservations(percept);
    }
    
    // Update cognitive systems
    updateCognitiveState();
}

MotorCommand AgentBrainEnhanced::decodeMotorCommand() {
    // Get base motor command from parent implementation
    MotorCommand baseCommand = AgentBrain::decodeMotorCommand();
    
    // Apply cognitive influences
    MotorCommand finalCommand = applyCognitiveInfluence(baseCommand);
    
    // Log cognitive decision process
    if (brain_) {
        NLM_LOG_DEBUG("AgentBrainEnhanced decision process: confidence=" + 
                   std::to_string(planningConfidence_) + 
                   " clarity=" + std::to_string(conceptClarity_) + 
                   " focus=" + std::to_string(attentionFocus_));
    }
    
    return finalCommand;
}

void AgentBrainEnhanced::applyRewardModulation(float reward, float predictedReward) {
    // Apply parent reward modulation
    AgentBrain::applyRewardModulation(reward, predictedReward);
    
    if (!brain_) return;
    
    // Enhanced reward modulation with cognitive learning
    if (dopamine_) {
        float predictionError = reward - predictedReward;
        
        // Update dopamine signal with cognitive factors
        float cognitiveModulation = computeCognitiveModulation();
        float enhancedError = predictionError * (1.0f + cognitiveModulation);
        
        dopamine_->updateRewardModulation(enhancedError);
    }
    
    // Store reward experience for cognitive learning
    storeCognitiveObservation(reward, std::vector<float>());
    
    // Update concept formation with reward information
    if (conceptFormationEnabled_ && conceptFormation_) {
        conceptFormation_->processReward(reward, predictedReward);
    }
    
    // Update curiosity based on reward prediction error
    if (curiosity_) {
        curiosity_->updateCuriosity(std::abs(predictionError));
    }
}

void AgentBrainEnhanced::updateCognitiveSystems(double timestep) {
    if (!brain_) return;
    
    // Update planner with current cognitive state
    if (planningEnabled_ && planner_) {
        planner_->updatePlanningState(attentionFocus_, planningConfidence_);
        std::optional<MotorCommand> plannedAction = planner_->generateAction();
        if (plannedAction) {
            // Store planned action for consideration
            storePlannedAction(*plannedAction);
        }
    }
    
    // Update concept formation with current experiences
    if (conceptFormationEnabled_ && conceptFormation_) {
        conceptFormation_->updateConcepts(recentObservations_, recentRewards_);
        conceptClarity_ = conceptFormation_->getConceptClarity();
    }
    
    // Update attention system
    if (attentionEnabled_ && attention_) {
        attention_->updateAttentionScope(attentionFocus_);
        attentionFocus_ = attention_->getFocusLevel();
    }
    
    // Update prediction system
    if (predictionEnabled_ && predictionSystem_) {
        predictionSystem_->updatePredictionState(recentObservations_);
    }
    
    // Update working memory with cognitive state
    if (workingMemory_) {
        workingMemory_->updateCognitiveState(attentionFocus_, planningConfidence_);
    }
}

float AgentBrainEnhanced::getPlanningConfidence() const {
    return planningConfidence_;
}

float AgentBrainEnhanced::getConceptClarity() const {
    return conceptClarity_;
}

float AgentBrainEnhanced::getAttentionFocus() const {
    return attentionFocus_;
}

size_t AgentBrainEnhanced::getWorkingMemoryUsage() const {
    return workingMemory_ ? workingMemory_->getActiveTraces() : 0;
}

size_t AgentBrainEnhanced::getEpisodicMemoryUsage() const {
    if (brain_) {
        auto* episodicMemory = brain_->getEpisodicMemory();
        return episodicMemory ? episodicMemory->getEpisodeCount() : 0;
    }
    return 0;
}

void AgentBrainEnhanced::enablePlanning(bool enable) {
    planningEnabled_ = enable;
    NLM_LOG_INFO("AgentBrainEnhanced planning " + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrainEnhanced::enableConceptFormation(bool enable) {
    conceptFormationEnabled_ = enable;
    NLM_LOG_INFO("AgentBrainEnhanced concept formation " + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrainEnhanced::enableAttention(bool enable) {
    attentionEnabled_ = enable;
    NLM_LOG_INFO("AgentBrainEnhanced attention " + std::string(enable ? "enabled" : "disabled"));
}

void AgentBrainEnhanced::enablePrediction(bool enable) {
    predictionEnabled_ = enable;
    NLM_LOG_INFO("AgentBrainEnhanced prediction " + std::string(enable ? "enabled" : "disabled"));
}

bool AgentBrainEnhanced::isPlanningEnabled() const {
    return planningEnabled_;
}

bool AgentBrainEnhanced::isConceptFormationEnabled() const {
    return conceptFormationEnabled_;
}

bool AgentBrainEnhanced::isAttentionEnabled() const {
    return attentionEnabled_;
}

bool AgentBrainEnhanced::isPredictionEnabled() const {
    return predictionEnabled_;
}

// Private helper methods
void AgentBrainEnhanced::updateCognitiveState() {
    // Update cognitive state based on recent experiences
    planningConfidence_ = std::clamp(planningConfidence_ * 0.95f + 0.05f, 0.0f, 1.0f);
    attentionFocus_ = std::clamp(attentionFocus_ * 0.9f + 0.1f, 0.0f, 1.0f);
}

MotorCommand AgentBrainEnhanced::applyCognitiveInfluence(MotorCommand baseCommand) {
    MotorCommand finalCommand = baseCommand;
    
    // Apply curiosity-based exploration
    if (curiosity_) {
        float curiosityLevel = curiosity_->getCuriosityLevel();
        if (curiosityLevel > 0.3f) {
            // Higher curiosity leads to more exploration
            float exploreChance = curiosityLevel * 0.3f;
            float r = brain_ ? brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
            
            if (r < exploreChance) {
                // Choose random action
                int choice = brain_ ? brain_->getRandomGenerator()->uniformInt(0, 7) : 0;
                switch (choice) {
                    case 0: finalCommand = MotorCommand::MoveForward; break;
                    case 1: finalCommand = MotorCommand::MoveBackward; break;
                    case 2: finalCommand = MotorCommand::TurnLeft; break;
                    case 3: finalCommand = MotorCommand::TurnRight; break;
                    case 4: finalCommand = MotorCommand::LookLeft; break;
                    case 5: finalCommand = MotorCommand::LookRight; break;
                    case 6: finalCommand = MotorCommand::Interact; break;
                    default: finalCommand = MotorCommand::Wait; break;
                }
            }
        }
    }
    
    // Apply planned actions if available
    if (planningEnabled_ && planner_) {
        std::optional<MotorCommand> plannedAction = planner_->getRecentAction();
        if (plannedAction && planningConfidence_ > 0.7f) {
            finalCommand = *plannedAction;
        }
    }
    
    return finalCommand;
}

void AgentBrainEnhanced::extractCognitiveFeatures() {
    // Extract features from current brain state for cognitive processing
    if (!brain_) return;
    
    // Extract motor neuron activity patterns
    std::vector<float> actionFeatures;
    if (brain_->getMotorNeurons()) {
        for (const auto* neuron : *brain_->getMotorNeurons()) {
            if (neuron) {
                actionFeatures.push_back(std::abs(neuron->getState().membranePotential - 
                                             neuron->getState().restingPotential));
            }
        }
    }
    
    // Update recent actions buffer
    recentActions_.push_back(actionFeatures.size() > 0 ? actionFeatures[0] : 0.0f);
    if (recentActions_.size() > 100) {
        recentActions_.erase(recentActions_.begin());
    }
    
    // Update cognitive confidence based on consistency
    float actionConsistency = computeActionConsistency();
    planningConfidence_ = std::max(planningConfidence_, actionConsistency);
}

void AgentBrainEnhanced::storeSensoryObservations(const SensoryPercept& percept) {
    if (workingMemory_) {
        const auto& vision = percept.getVision();
        if (!vision.empty()) {
            workingMemory_->storeObservation(vision);
            recentObservations_.assign(vision.begin(), vision.end());
            if (recentObservations_.size() > 256) {
                recentObservations_.erase(recentObservations_.begin() + 256, recentObservations_.end());
            }
        }
    }
}

void AgentBrainEnhanced::storePlannedAction(MotorCommand action) {
    // Store planned action for potential use
    // This could be used for action selection or learning
}

void AgentBrainEnhanced::storeCognitiveObservation(float reward, const std::vector<float>& state) {
    recentRewards_.push_back(reward);
    if (recentRewards_.size() > 100) {
        recentRewards_.erase(recentRewards_.begin());
    }
}

std::vector<float> AgentBrainEnhanced::predictExpectedInput() {
    // Predict expected sensory input based on current cognitive state
    std::vector<float> predictedInput(256, 0.0f);  // Default vision input size
    
    if (predictionSystem_) {
        // Use prediction system to generate expected input
        // For now, return empty prediction as prediction system needs implementation
    }
    
    // Simple prediction based on recent observations
    if (recentObservations_.size() > 10) {
        // Average recent observations as simple prediction
        float avgValue = 0.0f;
        for (float val : recentObservations_) {
            avgValue += val;
        }
        avgValue /= recentObservations_.size();
        
        for (size_t i = 0; i < predictedInput.size(); ++i) {
            predictedInput[i] = avgValue;
        }
    }
    
    return predictedInput;
}

float AgentBrainEnhanced::computePredictionError(const std::vector<float>& expected, 
                                                 const std::vector<float>& actual) {
    if (expected.empty() || actual.empty()) return 0.0f;
    
    float totalError = 0.0f;
    size_t minSize = std::min(expected.size(), actual.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalError += std::abs(expected[i] - actual[i]);
    }
    
    return totalError / minSize;
}

float AgentBrainEnhanced::computeCognitiveModulation() {
    // Compute cognitive modulation factor for reward processing
    float modulation = 1.0f;
    
    // Increase modulation based on concept clarity
    modulation += conceptClarity_ * 0.5f;
    
    // Increase modulation based on attention focus
    modulation += attentionFocus_ * 0.3f;
    
    // Increase modulation based on planning confidence
    modulation += planningConfidence_ * 0.2f;
    
    return modulation - 1.0f;  // Return as delta
}

float AgentBrainEnhanced::computeActionConsistency() {
    if (recentActions_.size() < 2) return 0.0f;
    
    // Compute consistency of recent actions
    float mean = 0.0f;
    for (float action : recentActions_) {
        mean += action;
    }
    mean /= recentActions_.size();
    
    float variance = 0.0f;
    for (float action : recentActions_) {
        variance += (action - mean) * (action - mean);
    }
    variance /= recentActions_.size();
    
    // Lower variance = higher consistency
    return std::max(0.0f, 1.0f - variance);
}

} // namespace nlm