#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

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
    , maxPredictionHorizon_(10)
    , attentionWeight_(0.5f)
{
    // Initialize motor and sensory neuron groups with better distribution
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    // More sophisticated motor neuron grouping
                    for (Neuron* n : pop->getNeurons()) {
                        // Use neuron index for better distribution
                        size_t neuronIndex = n->getId().index % 8;  // 8 possible actions
                        
                        switch (neuronIndex) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                            case 6: motorLookLeft_.push_back(n); break;
                            case 7: motorLookRight_.push_back(n); break;
                        }
                    }
                } else if (type == NeuronType::Sensory) {
                    // Enhanced sensory neuron specialization
                    for (Neuron* n : pop->getNeurons()) {
                        // Use neuron ID to create specialized sensory channels
                        size_t neuronIndex = n->getId().index % 6;
                        
                        switch (neuronIndex) {
                            case 0: sensoryVision_.push_back(n); break;
                            case 1: sensoryTouch_.push_back(n); break;
                            case 2: sensoryInternal_.push_back(n); break;
                            case 3: sensoryProprioception_.push_back(n); break;
                            case 4: sensoryAudio_.push_back(n); break;
                            case 5: sensoryBalance_.push_back(n); break;
                        }
                    }
                }
            }
        }
    }
    
    // Initialize random generator for exploration
    randomGenerator_ = std::make_unique<std::mt19937>(std::random_device{}());
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize state variables
    attentionFocus_ = std::vector<float>(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    predictionBuffer_.clear();
    rewardHistory_.clear();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons, " +
                 std::to_string(sensoryAudio_.size()) + " audio sensory neurons");
}

// Additional helper methods for improved decision making
float AgentBrain::calculateActionPriority(MotorCommand cmd, const std::vector<float>& neuralActivity) const {
    // Calculate priority based on neural activity and historical success
    float basePriority = 0.0f;
    
    // Neural activity contribution
    switch (cmd) {
        case MotorCommand::MoveForward: basePriority += neuralActivity[0]; break;
        case MotorCommand::MoveBackward: basePriority += neuralActivity[1]; break;
        case MotorCommand::TurnLeft: basePriority += neuralActivity[2]; break;
        case MotorCommand::TurnRight: basePriority += neuralActivity[3]; break;
        case MotorCommand::Interact: basePriority += neuralActivity[4]; break;
        case MotorCommand::Wait: basePriority += neuralActivity[5]; break;
        case MotorCommand::LookLeft: basePriority += neuralActivity[6]; break;
        case MotorCommand::LookRight: basePriority += neuralActivity[7]; break;
    }
    
    // Historical success contribution
    auto it = actionSuccess_.find(cmd);
    if (it != actionSuccess_.end()) {
        basePriority += it->second * 0.5f;
    }
    
    // Neuromodulation influence
    basePriority += dopamineLevel_ * 0.3f;
    
    return std::clamp(basePriority, 0.0f, 1.0f);
}

AgentBrain::ExplorationStrategy AgentBrain::selectExplorationStrategy() const {
    if (!curiosityEnabled_) return ExplorationStrategy::None;
    
    if (curiosityLevel_ > 0.8f) return ExplorationStrategy::Maximal;
    else if (curiosityLevel_ > 0.5f) return ExplorationStrategy::Balanced;
    else if (curiosityLevel_ > 0.2f) return ExplorationStrategy::Selective;
    else return ExplorationStrategy::None;
}

void AgentBrain::updateAttention(const std::vector<float>& visualInput, double timestamp) {
    // Simple attention mechanism focusing on high-contrast areas
    attentionFocus_.clear();
    attentionFocus_.resize(visualInput.size(), 0.0f);
    
    if (curiosityEnabled_) {
        for (size_t i = 0; i < visualInput.size(); ++i) {
            float contrast = std::abs(visualInput[i] - (i > 0 ? visualInput[i-1] : 0.0f));
            attentionFocus_[i] = std::min(1.0f, contrast * attentionWeight_);
        }
    }
}

void AgentBrain::updatePredictionSystem(const std::vector<float>& rewardSequence) {
    // Simple prediction model based on reward history
    predictionBuffer_.push_back(rewardSequence);
    
    if (predictionBuffer_.size() > maxPredictionHorizon_) {
        predictionBuffer_.pop_front();
    }
    
    // Update expected reward based on recent rewards
    if (!rewardSequence.empty()) {
        float recentReward = rewardSequence.back();
        expectedReward_ = 0.95f * expectedReward_ + 0.05f * recentReward;
        predictionError_ = recentReward - expectedReward_;
    }
}

// Replace the existing methods with improved versions
size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action (including gaze and balance)
    return 8;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Enhanced sensory processing with attention weighting
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Apply attention weighting
            float attentionWeight = attentionFocus_.size() > i ? attentionFocus_[i] : 1.0f;
            float current = vision[i] * 5.0f * attentionWeight;
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Enhanced novelty detection with temporal comparison
    if (!vision.empty() && !previousVision_.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply decay
        noveltyLevel_ = (totalDiff / std::max<size_t>(vision.size(), 1)) * 0.5f;
        noveltyLevel_ = noveltyLevel_ * sensoryNoveltyDecay_;
        
        // Update for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on enhanced novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Increase curiosity when prediction error is high
        if (std::abs(predictionError_) > 0.5f) {
            curiosityLevel_ += 0.2f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    // Get current neural activity for all motor groups
    std::vector<float> motorActivities(8, 0.0f);
    
    // Calculate activities with better weighting
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            float activity = std::abs(n->getState().membranePotential - n->getState().restingPotential);
            sum += activity;
        }
        return sum / neurons.size();
    };
    
    motorActivities[0] = calcActivity(motorForward_);     // MoveForward
    motorActivities[1] = calcActivity(motorBackward_);    // MoveBackward
    motorActivities[2] = calcActivity(motorTurnLeft_);    // TurnLeft
    motorActivities[3] = calcActivity(motorTurnRight_);   // TurnRight
    motorActivities[4] = calcActivity(motorInteract_);    // Interact
    motorActivities[5] = calcActivity(motorWait_);        // Wait
    motorActivities[6] = calcActivity(motorLookLeft_);    // LookLeft
    motorActivities[7] = calcActivity(motorLookRight_);   // LookRight
    
    // Apply attention weighting based on current focus
    if (curiosityLevel_ > 0.3f) {
        // During high curiosity, consider more options
        for (size_t i = 0; i < motorActivities.size(); ++i) {
            motorActivities[i] *= (1.0f + curiosityLevel_ * 0.5f);
        }
    }
    
    // Calculate priorities for each action
    std::vector<float> priorities;
    for (size_t i = 0; i < motorActivities.size(); ++i) {
        priorities.push_back(calculateActionPriority(static_cast<MotorCommand>(i), motorActivities));
    }
    
    // Select best action based on combined criteria
    MotorCommand selected = MotorCommand::Wait;
    float bestPriority = 0.0f;
    
    for (size_t i = 0; i < priorities.size(); ++i) {
        if (priorities[i] > bestPriority) {
            bestPriority = priorities[i];
            selected = static_cast<MotorCommand>(i);
        }
    }
    
    // Apply exploration strategy
    ExplorationStrategy strategy = selectExplorationStrategy();
    if (strategy != ExplorationStrategy::None) {
        float explorationChance = 0.0f;
        switch (strategy) {
            case ExplorationStrategy::None: explorationChance = 0.0f; break;
            case ExplorationStrategy::Selective: explorationChance = 0.1f; break;
            case ExplorationStrategy::Balanced: explorationChance = 0.3f; break;
            case ExplorationStrategy::Maximal: explorationChance = 0.5f; break;
        }
        
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        if (dist(*randomGenerator_) < explorationChance) {
            // Select random action
            int randomChoice = dist(*randomGenerator_) * static_cast<float>(MotorCommand::Wait + 1);
            selected = static_cast<MotorCommand>(randomChoice);
        }
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        selected = selectWithCuriosity(selected);
    }
    
    // Update action success tracking
    actionSuccess_[selected] = (selected != MotorCommand::Wait) ? 1.0f : 0.0f;
    
    return selected;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // This is kept for backward compatibility
    return decodeMotorCommand();
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced curiosity-based exploration with prediction-based selection
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f;  // Up to 30% random
        
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        if (dist(*randomGenerator_) < exploreChance) {
            // Intelligent random selection based on prediction error
            if (std::abs(predictionError_) > 0.3f) {
                // When prediction error is high, explore potentially rewarding actions
                std::vector<MotorCommand> highValueActions = {
                    MotorCommand::Interact, MotorCommand::MoveForward, 
                    MotorCommand::MoveBackward, MotorCommand::LookLeft, MotorCommand::LookRight
                };
                
                int choice = dist(*randomGenerator_) * static_cast<float>(highValueActions.size());
                return highValueActions[choice];
            } else {
                // Otherwise random exploration
                int choice = dist(*randomGenerator_) * 8;  // 8 motor commands
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
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Enhanced reward modulation with temporal difference learning
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Track reward history
    rewardHistory_.push_back(reward);
    if (rewardHistory_.size() > 20) {  // Keep last 20 rewards
        rewardHistory_.pop_front();
    }
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Enhanced synaptic eligibility traces
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change with history weighting
                float historyWeight = 1.0f;
                if (!rewardHistory_.empty()) {
                    historyWeight = std::accumulate(rewardHistory_.begin(), rewardHistory_.end(), 0.0f) / rewardHistory_.size();
                    historyWeight = (historyWeight + 1.0f) * 0.5f;  // Normalize to 0-1
                }
                
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * historyWeight;
                syn->addToWeight(delta);
                
                // Decay eligibility trace with history influence
                float decayRate = 0.1f + 0.05f * (1.0f - historyWeight);
                syn->decayEligibilityTrace(decayRate);
            }
        }
    }
    
    // Enhanced neuromodulation effects on plasticity
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
    
    // Update developmental stage influence
    if (developmentEnabled_) {
        float developmentInfluence = std::min(developmentalAge_ / 300.0f, 1.0f);  // Normalize by max age
        plasticityFactor *= (1.0f + developmentInfluence * 0.5f);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Enhanced developmental stages with smoother transitions
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
            float synRate = 0.0001f * plasticityModifier_ * (1.0f + developmentalAge_ / 1000.0f);
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) * (1.0f - developmentalAge_ / 2000.0f);
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
    
    // Clear attention and prediction buffers
    attentionFocus_.clear();
    predictionBuffer_.clear();
    rewardHistory_.clear();
    
    // Reset action success tracking
    actionSuccess_.clear();
}

} // namespace nlm