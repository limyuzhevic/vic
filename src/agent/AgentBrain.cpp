#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <unordered_map>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, const Configuration& config)
    : brain_(brain)
    , config_(config)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , novelAccumulation_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , performanceScore_(0.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , advancedFeaturesEnabled_(false)
    , randomStateInitialized_(false)
    , neuronAccessCount_(0)
    , lastActivityCalculationTime_(0.0f)
    , totalActivityScore_(0.0f)
{
    config_.validate();
    
    // Initialize random state for exploration
    if (brain_) {
        randomDevice_ = std::make_unique<std::random_device>();
        randomGenerator_ = std::make_unique<std::mt19937>(
            (*randomDevice_)()
        );
        randomStateInitialized_ = true;
    }
    
    // Initialize motor and sensory neuron groups using intelligent strategy
    if (config_.useAdvancedGrouping_) {
        groupMotorNeuronsIntelligent();
        groupSensoryNeuronsIntelligent();
    } else {
        // Legacy grouping for backward compatibility
        if (brain_) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    NeuronType type = pop->getNeuronType();
                    
                    if (type == NeuronType::Motor) {
                        for (Neuron* n : pop->getNeurons()) {
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
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    previousTouch_.resize(world.getTouchSize(), 0.0f);
    previousInternal_.resize(world.getInternalSize(), 0.0f);
    previousProprioception_.resize(world.getProprioceptionSize(), 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    performanceScore_ = 0.0f;
    neuronAccessCount_ = 0;
    lastActivityCalculationTime_ = 0.0f;
    totalActivityScore_ = 0.0f;
    novelAccumulation_ = 0.0f;
    
    if (advancedFeaturesEnabled_) {
        NLM_LOG_INFO("AgentBrain initialized with ADVANCED features: " +
                     std::to_string(sensoryVision_.size()) + " vision neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal neurons, " +
                     std::to_string(sensoryProprioception_.size()) + " proprioception neurons");
        NLM_LOG_INFO("Configuration: useAdvancedGrouping_=" + std::to_string(config_.useAdvancedGrouping_) +
                     ", multiModalNovelty_=" + std::to_string(config_.multiModalNovelty_) +
                     ", strategy=" + config_.neuronGroupingStrategy_);
    } else {
        NLM_LOG_INFO("AgentBrain initialized with legacy features: " +
                     std::to_string(sensoryVision_.size()) + " vision neurons, " +
                     std::to_string(sensoryTouch_.size()) + " touch neurons, " +
                     std::to_string(sensoryInternal_.size()) + " internal neurons");
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    // Can be dynamically calculated based on world configuration
    size_t visionSize = 256;  // 16x16
    size_t touchSize = 8;    // Fixed number of touch sensors
    size_t internalSize = 4; // Fixed number of internal sensors
    size_t proprioceptionSize = 6; // Fixed number of proprioception sensors
    
    if (advancedFeaturesEnabled_) {
        // More flexible sizing based on world
        return visionSize + touchSize + internalSize + proprioceptionSize;
    } else {
        return visionSize + touchSize + internalSize;
    }
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Store previous state for novelty calculation
    previousVision_ = percept.getVision();
    if (!percept.getTouch().empty() && percept.getTouch().size() == previousTouch_.size()) {
        previousTouch_ = percept.getTouch();
    }
    if (!percept.getInternal().empty() && percept.getInternal().size() == previousInternal_.size()) {
        previousInternal_ = percept.getInternal();
    }
    if (!percept.getProprioception().empty() && percept.getProprioception().size() == previousProprioception_.size()) {
        previousProprioception_ = percept.getProprioception();
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Apply scale factor from configuration
            float current = vision[i] * config_.visionScale_;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * config_.touchScale_;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * config_.internalScale_;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * config_.proprioceptionScale_;
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute multi-modal novelty
    noveltyLevel_ = config_.multiModalNovelty_ ? 
                   calculateMultiModalNovelty(percept) : 
                   calculateVisionNovelty(vision, previousVision_);
    
    // Apply novelty decay
    noveltyLevel_ *= config_.noveltyDecay_;
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
    
    if (advancedFeaturesEnabled_) {
        // Update performance metrics
        updatePerformanceMetrics();
        decayNoveltyLevels();
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > getAdaptiveCuriosityThreshold()) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Use optimized activity calculation
    float forwardAct = calculateNeuralActivityOptimized(motorForward_);
    float backwardAct = calculateNeuralActivityOptimized(motorBackward_);
    float leftAct = calculateNeuralActivityOptimized(motorTurnLeft_);
    float rightAct = calculateNeuralActivityOptimized(motorTurnRight_);
    float interactAct = calculateNeuralActivityOptimized(motorInteract_);
    float waitAct = calculateNeuralActivityOptimized(motorWait_);
    
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
    
    // Only act if there's meaningful activity (adaptive threshold)
    if (bestActivity < getAdaptiveActionThreshold()) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * config_.explorationRate_;  // Up to 30% random
        
        if (randomStateInitialized_) {
            float r = std::uniform_real_distribution<float>(0.0f, 1.0f)(*randomGenerator_);
            if (r < exploreChance) {
                // Random motor command with weighted probabilities based on curiosity
                std::map<MotorCommand, float> probabilities;
                
                if (curiosityLevel_ > 0.8f) {
                    // High curiosity: more random exploration
                    probabilities = {
                        {MotorCommand::MoveForward, 0.2f},
                        {MotorCommand::MoveBackward, 0.2f},
                        {MotorCommand::TurnLeft, 0.2f},
                        {MotorCommand::TurnRight, 0.2f},
                        {MotorCommand::LookLeft, 0.1f},
                        {MotorCommand::LookRight, 0.1f},
                        {MotorCommand::Interact, 0.1f},
                        {MotorCommand::Wait, 0.1f}
                    };
                } else {
                    // Medium curiosity: more goal-directed exploration
                    probabilities = {
                        {MotorCommand::MoveForward, 0.3f},
                        {MotorCommand::MoveBackward, 0.3f},
                        {MotorCommand::TurnLeft, 0.2f},
                        {MotorCommand::TurnRight, 0.2f},
                        {MotorCommand::LookLeft, 0.05f},
                        {MotorCommand::LookRight, 0.05f},
                        {MotorCommand::Interact, 0.05f},
                        {MotorCommand::Wait, 0.05f}
                    };
                }
                
                return weightedRandomAction(probabilities);
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
    expectedReward_ = config_.rewardPredictionWeight_ * expectedReward_ + 
                     (1.0f - config_.rewardPredictionWeight_) * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -config_.dopamineClamp_, config_.dopamineClamp_);
    
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
    float plasticityFactor = config_.plasticityFactorRange_ * 0.5f + 
                             config_.plasticityFactorRange_ * 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, config_.plasticityFactorRange_);
    
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
    if (developmentalAge_ < config_.developmentalStageCritical_) {  // Early development
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < config_.developmentalStageMaturation_) {  // Critical period
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < config_.developmentalStageMaturation_ * 2.5f) {  // Maturation
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
    novelAccumulation_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    performanceScore_ = 0.0f;
    
    // Clear previous sensory states
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    std::fill(previousTouch_.begin(), previousTouch_.end(), 0.0f);
    std::fill(previousInternal_.begin(), previousInternal_.end(), 0.0f);
    std::fill(previousProprioception_.begin(), previousProprioception_.end(), 0.0f);
}

void AgentBrain::configureNeuronGrouping(const std::string& strategy) {
    config_.neuronGroupingStrategy_ = strategy;
    
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Apply new grouping strategy
    if (strategy == "behavioral") {
        groupMotorNeuronsIntelligent();
        groupSensoryNeuronsIntelligent();
        NLM_LOG_INFO("Configured behavioral neuron grouping strategy");
    } else if (strategy == "sensory") {
        // Group by sensory modality
        if (brain_) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    NeuronType type = pop->getNeuronType();
                    if (type == NeuronType::Sensory) {
                        for (Neuron* n : pop->getNeurons()) {
                            sensoryVision_.push_back(n); // Simplified - in reality would distinguish
                        }
                    }
                }
            }
        }
        NLM_LOG_INFO("Configured sensory neuron grouping strategy");
    } else if (strategy == "developmental") {
        // Group by developmental function
        groupMotorNeuronsIntelligent();
        NLM_LOG_INFO("Configured developmental neuron grouping strategy");
    } else {
        NLM_LOG_WARN("Unknown neuron grouping strategy: " + strategy + ". Using default.");
        // Fall back to legacy grouping
        if (brain_) {
            for (const auto& region : brain_->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    NeuronType type = pop->getNeuronType();
                    
                    if (type == NeuronType::Motor) {
                        for (Neuron* n : pop->getNeurons()) {
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
}

std::map<std::string, float> AgentBrain::getComprehensiveStats() const {
    std::map<std::string, float> stats;
    
    if (brain_) {
        stats["neuron_count"] = static_cast<float>(brain_->getTotalNeuronCount());
        stats["synapse_count"] = static_cast<float>(brain_->getTotalSynapseCount());
        stats["active_neuron_count"] = static_cast<float>(brain_->getActiveNeuronCount());
        stats["firing_neuron_count"] = static_cast<float>(brain_->getFiringNeuronCount());
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        stats["total_spike_count"] = static_cast<float>(brain_->getTotalSpikeCount());
        stats["developmental_stage"] = static_cast<float>(getDevelopmentalStage());
        stats["neuromodulation_level"] = getNeuromodulationLevel();
        stats["curiosity_level"] = getCuriosityLevel();
        stats["novelty_level"] = getNoveltyLevel();
        stats["prediction_error"] = getPredictionError();
        stats["expected_reward"] = expectedReward_;
        stats["performance_score"] = performanceScore_;
        stats["plasticity_modifier"] = plasticityModifier_;
        
        // Add group statistics
        stats["motor_forward_neurons"] = static_cast<float>(motorForward_.size());
        stats["motor_backward_neurons"] = static_cast<float>(motorBackward_.size());
        stats["motor_turn_left_neurons"] = static_cast<float>(motorTurnLeft_.size());
        stats["motor_turn_right_neurons"] = static_cast<float>(motorTurnRight_.size());
        stats["motor_interact_neurons"] = static_cast<float>(motorInteract_.size());
        stats["motor_wait_neurons"] = static_cast<float>(motorWait_.size());
        
        stats["sensory_vision_neurons"] = static_cast<float>(sensoryVision_.size());
        stats["sensory_touch_neurons"] = static_cast<float>(sensoryTouch_.size());
        stats["sensory_internal_neurons"] = static_cast<float>(sensoryInternal_.size());
        stats["sensory_proprioception_neurons"] = static_cast<float>(sensoryProprioception_.size());
        
        stats["neuron_access_count"] = static_cast<float>(neuronAccessCount_);
        stats["activity_calculation_time_ms"] = lastActivityCalculationTime_ * 1000.0f;
        
        // Configuration info
        stats["advanced_features_enabled"] = advancedFeaturesEnabled_ ? 1.0f : 0.0f;
        stats["multi_modal_novelty_enabled"] = config_.multiModalNovelty_ ? 1.0f : 0.0f;
        stats["use_advanced_grouping"] = config_.useAdvancedGrouping_ ? 1.0f : 0.0f;
    }
    
    return stats;
}

float AgentBrain::analyzeSensoryNovelty(const SensoryPercept& percept) const {
    return calculateMultiModalNovelty(percept);
}

Action AgentBrain::planAction(const SensoryPercept& percept) {
    // Process sensory input
    processSensoryInput(percept);
    
    // Get the best motor command
    MotorCommand motorCommand = decodeMotorCommand();
    
    // Create action with parameters
    Action actionObj;
    actionObj.setType(static_cast<ActionType>(motorCommand));
    
    // Add sophisticated parameters based on action type and context
    std::vector<float> params;
    if (motorCommand == MotorCommand::MoveForward) {
        params = {1.0f, 0.5f, curiosityLevel_ * 0.3f};  // speed, duration, curiosity modifier
    } else if (motorCommand == MotorCommand::MoveBackward) {
        params = {0.8f, 0.4f, noveltyLevel_ * 0.2f};
    } else if (motorCommand == MotorCommand::TurnLeft) {
        params = {0.5f, 0.3f, std::abs(predictionError_) * 0.5f};  // angle, speed, error modifier
    } else if (motorCommand == MotorCommand::TurnRight) {
        params = {0.5f, 0.3f, std::abs(predictionError_) * 0.5f};
    } else if (motorCommand == MotorCommand::LookLeft || motorCommand == MotorCommand::LookRight) {
        params = {0.8f, 0.2f, curiosityLevel_};  // gaze speed, duration, curiosity
    } else if (motorCommand == MotorCommand::Interact) {
        params = {0.8f, 0.1f, performanceScore_};  // strength, duration, performance
    } else {
        params = {0.5f, 0.1f, 0.5f};  // Wait with moderate parameters
    }
    
    actionObj.setParameters(params);
    return actionObj;
}

// Intelligent neuron grouping strategies
void AgentBrain::groupMotorNeuronsIntelligent() {
    if (!brain_) return;
    
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Group motor neurons by functional regions and population characteristics
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            std::string regionName = region->getName();
            
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type != NeuronType::Motor) continue;
                
                for (Neuron* n : pop->getNeurons()) {
                    // Use region name and population properties to determine function
                    if (regionName.find("forward") != std::string::npos || 
                        regionName.find("approach") != std::string::npos) {
                        motorForward_.push_back(n);
                    } else if (regionName.find("backward") != std::string::npos ||
                               regionName.find("retreat") != std::string::npos) {
                        motorBackward_.push_back(n);
                    } else if (regionName.find("turn") != std::string::npos ||
                               regionName.find("rotate") != std::string::npos) {
                        // Randomly distribute left/right for flexibility
                        if (motorTurnLeft_.size() <= motorTurnRight_.size()) {
                            motorTurnLeft_.push_back(n);
                        } else {
                            motorTurnRight_.push_back(n);
                        }
                    } else if (regionName.find("interact") != std::string::npos ||
                               regionName.find("manipulate") != std::string::npos) {
                        motorInteract_.push_back(n);
                    } else {
                        // Default to wait behavior for unclassified regions
                        motorWait_.push_back(n);
                    }
                }
            }
        }
        
        // Ensure balanced distribution if strategy prefers it
        if (config_.neuronGroupingStrategy_ == "behavioral" && 
            (motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() + 
             motorTurnRight_.size() + motorInteract_.size() + motorWait_.size()) > 0) {
            
            // Log distribution for debugging
            NLM_LOG_INFO("Intelligent grouping: " +
                         std::to_string(motorForward_.size()) + " forward, " +
                         std::to_string(motorBackward_.size()) + " backward, " +
                         std::to_string(motorTurnLeft_.size()) + " left, " +
                         std::to_string(motorTurnRight_.size()) + " right, " +
                         std::to_string(motorInteract_.size()) + " interact, " +
                         std::to_string(motorWait_.size()) + " wait neurons");
        }
    }
}

void AgentBrain::groupSensoryNeuronsIntelligent() {
    if (!brain_) return;
    
    // Clear existing groups
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    // Group sensory neurons based on population type and region
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            std::string regionName = region->getName();
            
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                if (type != NeuronType::Sensory) continue;
                
                for (Neuron* n : pop->getNeurons()) {
                    // Classify based on region name and population characteristics
                    if (regionName.find("vision") != std::string::npos || 
                        regionName.find("visual") != std::string::npos) {
                        sensoryVision_.push_back(n);
                    } else if (regionName.find("touch") != std::string::npos ||
                               regionName.find("proprioception") != std::string::npos) {
                        sensoryProprioception_.push_back(n);
                    } else if (regionName.find("internal") != std::string::npos ||
                               regionName.find("homeostatic") != std::string::npos) {
                        sensoryInternal_.push_back(n);
                    } else if (regionName.find("audio") != std::string::npos ||
                               regionName.find("sound") != std::string::npos) {
                        sensoryTouch_.push_back(n); // Simplified - audio grouped with touch
                    } else {
                        // Default to vision for unclassified regions
                        sensoryVision_.push_back(n);
                    }
                }
            }
        }
        
        // Log distribution for debugging
        NLM_LOG_INFO("Sensory grouping: " +
                     std::to_string(sensoryVision_.size()) + " vision, " +
                     std::to_string(sensoryTouch_.size()) + " touch, " +
                     std::to_string(sensoryInternal_.size()) + " internal, " +
                     std::to_string(sensoryProprioception_.size()) + " proprioception neurons");
    }
}

// Advanced novelty calculation
float AgentBrain::calculateMultiModalNovelty(const SensoryPercept& percept) const {
    float totalNovelty = 0.0f;
    
    if (!percept.getVision().empty()) {
        float visionNovelty = calculateVisionNovelty(percept.getVision(), previousVision_);
        totalNovelty += visionNovelty * config_.visionNoveltyWeight_;
    }
    
    if (!percept.getTouch().empty() && config_.multiModalNovelty_) {
        float touchNovelty = calculateTouchNovelty(percept.getTouch());
        totalNovelty += touchNovelty * config_.touchNoveltyWeight_;
    }
    
    if (!percept.getInternal().empty() && config_.multiModalNovelty_) {
        float internalNovelty = calculateInternalNovelty(percept.getInternal());
        totalNovelty += internalNovelty * config_.internalNoveltyWeight_;
    }
    
    if (!percept.getProprioception().empty() && config_.multiModalNovelty_) {
        float proprioceptionNovelty = calculateProprioceptionNovelty(percept.getProprioception());
        totalNovelty += proprioceptionNovelty * config_.proprioceptionNoveltyWeight_;
    }
    
    return std::min(totalNovelty, 1.0f); // Clamp to [0, 1]
}

float AgentBrain::calculateVisionNovelty(const std::vector<float>& current, const std::vector<float>& previous) const {
    if (current.empty() || current.size() != previous.size()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < current.size(); ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    // Normalize
    return totalDiff / std::max<size_t>(current.size(), 1);
}

float AgentBrain::calculateTouchNovelty(const std::vector<float>& current) const {
    if (current.empty()) return 0.0f;
    
    // Novelty is based on magnitude of touch signals
    float total = 0.0f;
    for (float val : current) {
        total += std::abs(val);
    }
    
    return std::min(total / 10.0f, 1.0f); // Normalize to [0, 1]
}

float AgentBrain::calculateInternalNovelty(const std::vector<float>& current) const {
    if (current.empty()) return 0.0f;
    
    // Internal novelty based on changes in homeostatic signals
    float total = 0.0f;
    for (float val : current) {
        total += std::abs(val - 0.5f); // Compare to expected midpoint
    }
    
    return std::min(total / 5.0f, 1.0f); // Normalize to [0, 1]
}

float AgentBrain::calculateProprioceptionNovelty(const std::vector<float>& current) const {
    if (current.empty()) return 0.0f;
    
    // Proprioception novelty based on movement changes
    float total = 0.0f;
    for (float val : current) {
        total += std::abs(val);
    }
    
    return std::min(total / 8.0f, 1.0f); // Normalize to [0, 1]
}

// Adaptive threshold calculation
float AgentBrain::getAdaptiveActionThreshold() const {
    if (!config_.adaptiveThresholds_) return config_.actionThreshold_;
    
    // Adjust threshold based on novelty and performance
    float threshold = config_.actionThreshold_;
    
    // Lower threshold when novelty is high (encourage exploration)
    threshold -= noveltyLevel_ * 0.1f;
    
    // Adjust based on performance
    threshold -= performanceScore_ * 0.05f;
    
    // Clamp to reasonable range
    return std::clamp(threshold, 0.1f, 1.0f);
}

float AgentBrain::getAdaptiveCuriosityThreshold() const {
    if (!config_.adaptiveThresholds_) return config_.curiosityThreshold_;
    
    // Adjust threshold based on environmental uncertainty
    float threshold = config_.curiosityThreshold_;
    
    // Lower when expected reward is uncertain
    threshold -= std::abs(predictionError_) * 0.1f;
    
    // Adjust based on development stage
    if (developmentalAge_ > 450.0) {  // Adult stage
        threshold += 0.1f;  // More conservative
    }
    
    return std::clamp(threshold, 0.0f, 0.8f);
}

// Performance optimization
void AgentBrain::optimizeNeuronAccess() {
    neuronAccessCount_++;
    
    // Simple optimization: cache frequently accessed neurons
    // In a real implementation, this would be more sophisticated
    if (neuronAccessCount_ % 100 == 0) {
        // Periodic cleanup or optimization
        decayNoveltyLevels();
    }
}

float AgentBrain::calculateNeuralActivityOptimized(const std::vector<Neuron*>& neurons) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (neurons.empty()) {
        lastActivityCalculationTime_ = 0.0f;
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    
    float result = sum / neurons.size();
    
    // Measure and cache calculation time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    lastActivityCalculationTime_ = static_cast<float>(duration.count()) / 1000.0f; // Convert to ms
    
    // Update performance score
    totalActivityScore_ = (totalActivityScore_ * 0.9f) + (result * 0.1f); // Moving average
    
    return result;
}

// State management
void AgentBrain::updatePerformanceMetrics() const {
    if (advancedFeaturesEnabled_) {
        // Update performance based on action success and reward
        float currentPerformance = 0.0f;
        
        // This would be updated by external feedback (e.g., world rewards)
        // For now, use a simple heuristic
        currentPerformance = curiosityLevel_ * 0.3f + 
                           (1.0f - std::abs(predictionError_)) * 0.4f + 
                           (noveltyLevel_ * (1.0f - noveltyLevel_)) * 0.3f;  // Peak at 0.5 novelty
        
        // Update running average
        performanceScore_ = (performanceScore_ * 0.9f) + (currentPerformance * 0.1f);
    }
}

void AgentBrain::decayNoveltyLevels() {
    // Apply additional decay to novelty levels
    noveltyLevel_ *= 0.95f;
    novelAccumulation_ *= 0.95f;
    curiosityLevel_ *= 0.98f;
}

// Weighted random selection
MotorCommand AgentBrain::weightedRandomAction(const std::map<MotorCommand, float>& probabilities) const {
    if (probabilities.empty() || !randomStateInitialized_) {
        return MotorCommand::Wait;
    }
    
    float totalWeight = 0.0f;
    for (const auto& pair : probabilities) {
        totalWeight += pair.second;
    }
    
    if (totalWeight <= 0.0f) return MotorCommand::Wait;
    
    float randomValue = std::uniform_real_distribution<float>(0.0f, totalWeight)(*randomGenerator_);
    
    float cumulative = 0.0f;
    for (const auto& pair : probabilities) {
        cumulative += pair.second;
        if (randomValue <= cumulative) {
            return pair.first;
        }
    }
    
    // Fallback (shouldn't reach here)
    return MotorCommand::Wait;
}

} // namespace nlm
