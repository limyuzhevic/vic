#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>

namespace nlm {

// Implementation of AgentBrain

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain,
                       std::shared_ptr<NeuromodulationSystem> neuromodulation,
                       std::shared_ptr<DevelopmentalController> development,
                       std::shared_ptr<PerformanceMonitor> monitor)
    : brain_(brain)
    , neuromodulation_(neuromodulation)
    , developmentController_(development)
    , performanceMonitor_(monitor)
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
    , debugEnabled_(false)
    , sensoryNoveltyDecay_(0.99f)
    , operationCount_(0)
    , lastMotorCommand_(MotorCommand::Wait)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
{
    if (debugEnabled_) {
        NLM_LOG_INFO("AgentBrain created with enhanced architecture");
    }
}

AgentBrain::~AgentBrain() {
    if (debugEnabled_) {
        NLM_LOG_INFO("AgentBrain destroyed");
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    operationCount_ = 0;
    lastMotorCommand_ = MotorCommand::Wait;
    
    // Initialize motor and sensory neuron groups
    initializeMotorGroups();
    initializeSensoryGroups();
    
    if (debugEnabled_) {
        NLM_LOG_INFO("AgentBrain initialized with " + 
                    std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                    std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                    std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
    }
}

void AgentBrain::initializeMotorGroups() {
    // Clear existing groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    if (!brain_) return;
    
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

void AgentBrain::initializeSensoryGroups() {
    // Clear existing groups
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    // Distribute sensory neurons based on subtype
                    NeuronSubtype subtype = n->getSubtype();
                    switch (subtype) {
                        case NeuronSubtype::Vision:
                            sensoryVision_.push_back(n);
                            break;
                        case NeuronSubtype::Touch:
                            sensoryTouch_.push_back(n);
                            break;
                        case NeuronSubtype::Internal:
                            sensoryInternal_.push_back(n);
                            break;
                        case NeuronSubtype::Proprioception:
                            sensoryProprioception_.push_back(n);
                            break;
                        default:
                            // Default to vision for unknown subtypes
                            sensoryVision_.push_back(n);
                            break;
                    }
                }
            }
        }
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

void AgentBrain::enableSubsystem(const std::string& subsystem, bool enable) {
    if (subsystem == "reward") {
        rewardModulationEnabled_ = enable;
    } else if (subsystem == "plasticity") {
        structuralPlasticityEnabled_ = enable;
    } else if (subsystem == "development") {
        developmentEnabled_ = enable;
    } else if (subsystem == "curiosity") {
        curiosityEnabled_ = enable;
    }
    
    if (debugEnabled_) {
        NLM_LOG_INFO("Subsystem " + subsystem + " " + (enable ? "enabled" : "disabled"));
    }
}

bool AgentBrain::isSubsystemEnabled(const std::string& subsystem) const {
    if (subsystem == "reward") return rewardModulationEnabled_;
    if (subsystem == "plasticity") return structuralPlasticityEnabled_;
    if (subsystem == "development") return developmentEnabled_;
    if (subsystem == "curiosity") return curiosityEnabled_;
    return false;
}

void AgentBrain::setCuriosityThreshold(float threshold) {
    curiosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
    if (debugEnabled_) {
        NLM_LOG_INFO("Curiosity threshold set to " + std::to_string(curiosityThreshold_));
    }
}

void AgentBrain::setExplorationRate(float rate) {
    explorationRate_ = std::clamp(rate, 0.0f, 1.0f);
    if (debugEnabled_) {
        NLM_LOG_INFO("Exploration rate set to " + std::to_string(explorationRate_));
    }
}

void AgentBrain::setNoveltyDecay(float decay) {
    sensoryNoveltyDecay_ = std::clamp(decay, 0.0f, 1.0f);
    if (debugEnabled_) {
        NLM_LOG_INFO("Novelty decay set to " + std::to_string(sensoryNoveltyDecay_));
    }
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    processSensoryInput(percept, std::vector<float>()); // Default weights
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept, 
                                   const std::vector<float>& customWeights) {
    if (!brain_) return;
    operationCount_++;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Apply custom weights if provided
            float weight = customWeights.empty() ? 5.0f : customWeights[i % customWeights.size()];
            float current = vision[i] * weight;
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
        
        // Apply decay
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        // Store for next time
        previousVision_ = vision;
    }
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        
        // Trigger exploration if curiosity exceeds threshold
        if (curiosityLevel_ > curiosityThreshold_) {
            triggerEvents("high_curiosity");
        }
    }
    
    // Update performance metrics
    if (performanceMonitor_) {
        performanceMonitor_->recordInputProcessingTime(0.001); // Placeholder timing
    }
}

MotorCommand AgentBrain::decodeMotorCommand(const MotorCommandStrategy& strategy) {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply strategy-specific logic
    switch (strategy.type_) {
        case MotorCommandStrategy::Standard:
            // Default behavior
            break;
        case MotorCommandStrategy::CuriouslyGuided:
            decoded = decodeMotorCommandWithCuriosity(decoded);
            break;
        case MotorCommandStrategy::Conservative:
            // Bias towards wait action
            if (decodeFromMotorNeurons() == MotorCommand::Wait) {
                decoded = MotorCommand::Wait;
            }
            break;
        case MotorCommandStrategy::Exploratory:
            // Always explore if curiosity is high
            if (curiosityLevel_ > 0.7f) {
                decoded = selectWithCuriosity(decoded);
            }
            break;
        case MotorCommandStrategy::Random:
            // Random action selection
            if (curiosityLevel_ > 0.5f) {
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
            break;
    }
    
    lastMotorCommand_ = decoded;
    
    // Update performance metrics
    if (performanceMonitor_) {
        performanceMonitor_->recordMotorDecodingTime(0.001); // Placeholder timing
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeMotorCommandWithCuriosity(const MotorCommandStrategy& strategy) {
    // Enhanced curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        return selectWithCuriosity(decodeFromMotorNeurons());
    }
    return decodeFromMotorNeurons();
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
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * explorationRate_;  // Up to explorationRate% random
        
        if (brain_->getRandomGenerator()) {
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
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward, const RewardContext& context) {
    applyRewardModulation(RewardSignal(reward, predictedReward, context));
}

void AgentBrain::applyRewardModulation(const RewardSignal& rewardSignal) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Compute prediction error
    predictionError_ = rewardSignal.reward - rewardSignal.predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * rewardSignal.reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_ * rewardSignal.context.dopamineScale;
    
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
    
    // Trigger events
    triggerEvents("reward_received");
}

void AgentBrain::updateDevelopment(double timestep, const DevelopmentContext& context) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Enhanced developmental stages based on age and context
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f + context.developmentBoost * 0.2f; // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f * (1.0f - context.developmentRetardation);
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
            float synRate = 0.0001f * plasticityModifier_ * context.plasticityBoost;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) * context.pruningEfficiency;
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    // Update performance metrics
    if (performanceMonitor_) {
        performanceMonitor_->recordDevelopmentUpdateTime(0.001); // Placeholder timing
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

void AgentBrain::reset(bool clearHistory) {
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    operationCount_ = 0;
    
    // Clear previous vision
    if (clearHistory) {
        std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
        lastActionDescription_.clear();
    }
    
    // Re-initialize neuron groups to ensure clean state
    initializeMotorGroups();
    initializeSensoryGroups();
    
    // Trigger reset event
    triggerEvents("brain_reset");
}

void AgentBrain::processMultipleSteps(const std::vector<SensoryPercept>& percepts) {
    for (const auto& percept : percepts) {
        processSensoryInput(percept);
    }
}

std::vector<MotorCommand> AgentBrain::decodeMultipleSteps(size_t count) {
    std::vector<MotorCommand> commands;
    commands.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        commands.push_back(decodeMotorCommand());
    }
    
    return commands;
}

std::unordered_map<std::string, float> AgentBrain::getSubsystemStates() const {
    std::unordered_map<std::string, float> states;
    states["reward_modulation"] = rewardModulationEnabled_ ? 1.0f : 0.0f;
    states["structural_plasticity"] = structuralPlasticityEnabled_ ? 1.0f : 0.0f;
    states["development"] = developmentEnabled_ ? 1.0f : 0.0f;
    states["curiosity"] = curiosityEnabled_ ? 1.0f : 0.0f;
    
    return states;
}

std::string AgentBrain::getPerformanceStats() const {
    std::ostringstream oss;
    oss << "AgentBrain Performance Statistics:";
    oss << "\n  Total operations: " << operationCount_;
    oss << "\n  Current curiosity: " << curiosityLevel_;
    oss << "\n  Current novelty: " << noveltyLevel_;
    oss << "\n  Current prediction error: " << predictionError_;
    oss << "\n  Developmental age: " << developmentalAge_;
    oss << "\n  Plasticity modifier: " << plasticityModifier_;
    
    if (performanceMonitor_) {
        oss << "\n  Performance monitoring: Active";
    } else {
        oss << "\n  Performance monitoring: Inactive";
    }
    
    return oss.str();
}

bool AgentBrain::saveState(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Create a simple binary format
    file.write(reinterpret_cast<const char*>(&dopamineLevel_), sizeof(dopamineLevel_));
    file.write(reinterpret_cast<const char*>(&noveltyLevel_), sizeof(noveltyLevel_));
    file.write(reinterpret_cast<const char*>(&curiosityLevel_), sizeof(curiosityLevel_));
    file.write(reinterpret_cast<const char*>(&predictionError_), sizeof(predictionError_));
    file.write(reinterpret_cast<const char*>(&expectedReward_), sizeof(expectedReward_));
    file.write(reinterpret_cast<const char*>(&developmentalAge_), sizeof(developmentalAge_));
    file.write(reinterpret_cast<const char*>(&plasticityModifier_), sizeof(plasticityModifier_));
    
    // Save motor group sizes
    uint32_t motorForwardSize = motorForward_.size();
    file.write(reinterpret_cast<const char*>(&motorForwardSize), sizeof(motorForwardSize));
    
    return true;
}

bool AgentBrain::loadState(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read back the saved data
    file.read(reinterpret_cast<char*>(&dopamineLevel_), sizeof(dopamineLevel_));
    file.read(reinterpret_cast<char*>(&noveltyLevel_), sizeof(noveltyLevel_));
    file.read(reinterpret_cast<char*>(&curiosityLevel_), sizeof(curiosityLevel_));
    file.read(reinterpret_cast<char*>(&predictionError_), sizeof(predictionError_));
    file.read(reinterpret_cast<char*>(&expectedReward_), sizeof(expectedReward_));
    file.read(reinterpret_cast<char*>(&developmentalAge_), sizeof(developmentalAge_));
    file.read(reinterpret_cast<char*>(&plasticityModifier_), sizeof(plasticityModifier_));
    
    // Clear and reinitialize groups
    initializeMotorGroups();
    initializeSensoryGroups();
    
    return true;
}

std::string AgentBrain::serializeState() const {
    std::ostringstream oss;
    oss << "dopamine:" << dopamineLevel_ << ";";
    oss << "novelty:" << noveltyLevel_ << ";";
    oss << "curiosity:" << curiosityLevel_ << ";";
    oss << "prediction_error:" << predictionError_ << ";";
    oss << "expected_reward:" << expectedReward_ << ";";
    oss << "developmental_age:" << developmentalAge_ << ";";
    oss << "plasticity_modifier:" << plasticityModifier_;
    
    return oss.str();
}

bool AgentBrain::deserializeState(const std::string& data) {
    try {
        std::istringstream iss(data);
        std::string token;
        
        while (std::getline(iss, token, ';')) {
            size_t colon = token.find(':');
            if (colon != std::string::npos) {
                std::string key = token.substr(0, colon);
                std::string value = token.substr(colon + 1);
                
                if (key == "dopamine") dopamineLevel_ = std::stof(value);
                else if (key == "novelty") noveltyLevel_ = std::stof(value);
                else if (key == "curiosity") curiosityLevel_ = std::stof(value);
                else if (key == "prediction_error") predictionError_ = std::stof(value);
                else if (key == "expected_reward") expectedReward_ = std::stof(value);
                else if (key == "developmental_age") developmentalAge_ = std::stod(value);
                else if (key == "plasticity_modifier") plasticityModifier_ = std::stof(value);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void AgentBrain::addEventCallback(const std::string& event, std::function<void()> callback) {
    eventCallbacks_[event].push_back(callback);
}

void AgentBrain::removeEventCallback(const std::string& event) {
    eventCallbacks_.erase(event);
}

void AgentBrain::triggerEvents(const std::string& event) const {
    auto it = eventCallbacks_.find(event);
    if (it != eventCallbacks_.end()) {
        for (const auto& callback : it->second) {
            callback();
        }
    }
}

std::string AgentBrain::getDebugInfo() const {
    std::ostringstream oss;
    oss << "=== AgentBrain Debug Info ===\n";
    oss << "Motor groups:";
    oss << "\n  Forward: " << motorForward_.size();
    oss << "\n  Backward: " << motorBackward_.size();
    oss << "\n  Turn Left: " << motorTurnLeft_.size();
    oss << "\n  Turn Right: " << motorTurnRight_.size();
    oss << "\n  Interact: " << motorInteract_.size();
    oss << "\n  Wait: " << motorWait_.size();
    
    oss << "\nSensory groups:";
    oss << "\n  Vision: " << sensoryVision_.size();
    oss << "\n  Touch: " << sensoryTouch_.size();
    oss << "\n  Internal: " << sensoryInternal_.size();
    oss << "\n  Proprioception: " << sensoryProprioception_.size();
    
    oss << "\nState:";
    oss << "\n  Dopamine: " << dopamineLevel_;
    oss << "\n  Novelty: " << noveltyLevel_;
    oss << "\n  Curiosity: " << curiosityLevel_;
    oss << "\n  Prediction Error: " << predictionError_;
    oss << "\n  Expected Reward: " << expectedReward_;
    oss << "\n  Developmental Age: " << developmentalAge_;
    oss << "\n  Plasticity Modifier: " << plasticityModifier_;
    
    oss << "\nSystem status:";
    oss << "\n  Reward modulation: " << (rewardModulationEnabled_ ? "Enabled" : "Disabled");
    oss << "\n  Structural plasticity: " << (structuralPlasticityEnabled_ ? "Enabled" : "Disabled");
    oss << "\n  Development: " << (developmentEnabled_ ? "Enabled" : "Disabled");
    oss << "\n  Curiosity: " << (curiosityEnabled_ ? "Enabled" : "Disabled");
    
    oss << "\nPerformance:";
    oss << "\n  Operations: " << operationCount_;
    oss << "\n  Debug enabled: " << (debugEnabled_ ? "Yes" : "No");
    
    oss << "\n=== End Debug Info ===\n";
    
    return oss.str();
}

void AgentBrain::logDebugInfo() const {
    if (debugEnabled_) {
        std::string debug = getDebugInfo();
        NLM_LOG_INFO(debug);
    }
}

// Implementation of supporting classes

MotorCommandStrategy MotorCommandStrategy::create(Type type, float param) {
    MotorCommandStrategy strategy;
    strategy.type_ = type;
    strategy.param_ = param;
    return strategy;
}

// Placeholder implementations for supporting classes
// These would be expanded with actual functionality

} // namespace nlm