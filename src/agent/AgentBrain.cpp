#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Error/Error.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <string>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain,
                       const MotorDistributionAlgorithm& motorDistAlgorithm,
                       const SensoryGroupingConfig& sensoryConfig,
                       const NoveltyWeights& noveltyWeights,
                       const ExplorationCurve& explorationCurve)
    : brain_(brain)
    , motorDistAlgorithm_(motorDistAlgorithm)
    , sensoryConfig_(sensoryConfig)
    , noveltyWeights_(noveltyWeights)
    , explorationCurve_(explorationCurve)
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
    , motorCommandThreshold_(0.5f)
    , curiosityThreshold_(0.3f)
    , explorationBaseProb_(0.1f)
    , noveltyDecayRate_(0.99f)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize random number generator for reproducible distribution
    std::random_device rd;
    rng_.seed(rd());
    
    // Initialize motor and sensory neuron groups
    if (brain_) {
        updateDistribution();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::updateDistribution() {
    // Clear all neuron groups
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
    sensoryAudio_.clear();
    
    if (!brain_) return;
    
    // Distribute motor neurons based on configured algorithm
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                const auto& neurons = pop->getNeurons();
                if (neurons.empty()) continue;
                
                switch (motorDistAlgorithm_) {
                    case MotorDistributionAlgorithm::RoundRobin:
                        distributeMotorNeuronsRoundRobin(neurons);
                        break;
                    case MotorDistributionAlgorithm::Balanced:
                        distributeMotorNeuronsBalanced(neurons);
                        break;
                    case MotorDistributionAlgorithm::Random:
                        distributeMotorNeuronsRandom(neurons);
                        break;
                }
            } else if (type == NeuronType::Sensory) {
                const auto& neurons = pop->getNeurons();
                if (neurons.empty()) continue;
                
                // Distribute sensory neurons based on config
                for (Neuron* n : neurons) {
                    distributeSensoryNeuron(n);
                }
            }
        }
    }
}

void AgentBrain::distributeMotorNeuronsRoundRobin(const std::vector<Neuron*>& neurons) {
    size_t idx = motorForward_.size() + motorBackward_.size() + 
                 motorTurnLeft_.size() + motorTurnRight_.size() +
                 motorInteract_.size() + motorWait_.size();
    
    switch (idx % 6) {
        case 0: motorForward_.push_back(neurons[0]); break;
        case 1: motorBackward_.push_back(neurons[0]); break;
        case 2: motorTurnLeft_.push_back(neurons[0]); break;
        case 3: motorTurnRight_.push_back(neurons[0]); break;
        case 4: motorInteract_.push_back(neurons[0]); break;
        case 5: motorWait_.push_back(neurons[0]); break;
    }
}

void AgentBrain::distributeMotorNeuronsBalanced(const std::vector<Neuron*>& neurons) {
    // Calculate current group sizes
    std::vector<size_t> groupSizes = {
        motorForward_.size(), motorBackward_.size(), motorTurnLeft_.size(),
        motorTurnRight_.size(), motorInteract_.size(), motorWait_.size()
    };
    
    // Find group with smallest size
    size_t minSize = *std::min_element(groupSizes.begin(), groupSizes.end());
    std::vector<size_t> minGroups;
    
    for (size_t i = 0; i < groupSizes.size(); ++i) {
        if (groupSizes[i] == minSize) {
            minGroups.push_back(i);
        }
    }
    
    // Distribute to smallest group(s)
    size_t groupIdx = minGroups[rng_() % minGroups.size()];
    switch (groupIdx) {
        case 0: motorForward_.push_back(neurons[0]); break;
        case 1: motorBackward_.push_back(neurons[0]); break;
        case 2: motorTurnLeft_.push_back(neurons[0]); break;
        case 3: motorTurnRight_.push_back(neurons[0]); break;
        case 4: motorInteract_.push_back(neurons[0]); break;
        case 5: motorWait_.push_back(neurons[0]); break;
    }
}

void AgentBrain::distributeMotorNeuronsRandom(const std::vector<Neuron*>& neurons) {
    // Randomly select a distribution group
    std::uniform_int_distribution<size_t> dist(0, 5);
    size_t groupIdx = dist(rng_);
    
    switch (groupIdx) {
        case 0: motorForward_.push_back(neurons[0]); break;
        case 1: motorBackward_.push_back(neurons[0]); break;
        case 2: motorTurnLeft_.push_back(neurons[0]); break;
        case 3: motorTurnRight_.push_back(neurons[0]); break;
        case 4: motorInteract_.push_back(neurons[0]); break;
        case 5: motorWait_.push_back(neurons[0]); break;
    }
}

void AgentBrain::distributeSensoryNeuron(Neuron* neuron) {
    if (!sensoryConfig_.enableVision && !sensoryConfig_.enableTouch && 
        !sensoryConfig_.enableInternal && !sensoryConfig_.enableProprioception &&
        !sensoryConfig_.enableAudio) {
        return;
    }
    
    // Simplified distribution based on neuron index
    // In a real implementation, this would use more sophisticated logic
    if (sensoryConfig_.enableVision && sensoryVision_.size() <= sensoryTouch_.size() &&
        sensoryVision_.size() <= sensoryInternal_.size() &&
        sensoryVision_.size() <= sensoryProprioception_.size() &&
        sensoryVision_.size() <= sensoryAudio_.size()) {
        sensoryVision_.push_back(neuron);
    } else if (sensoryConfig_.enableTouch && sensoryTouch_.size() <= sensoryInternal_.size() &&
               sensoryTouch_.size() <= sensoryProprioception_.size() &&
               sensoryTouch_.size() <= sensoryAudio_.size()) {
        sensoryTouch_.push_back(neuron);
    } else if (sensoryConfig_.enableInternal && sensoryInternal_.size() <= sensoryProprioception_.size() &&
               sensoryInternal_.size() <= sensoryAudio_.size()) {
        sensoryInternal_.push_back(neuron);
    } else if (sensoryConfig_.enableProprioception &&
               sensoryProprioception_.size() <= sensoryAudio_.size()) {
        sensoryProprioception_.push_back(neuron);
    } else if (sensoryConfig_.enableAudio) {
        sensoryAudio_.push_back(neuron);
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    try {
        // Initialize previous sensory states based on config
        if (sensoryConfig_.enableVision) {
            previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
        } else {
            NLM_LOG_WARNING("Vision disabled in sensory config");
        }
        
        if (sensoryConfig_.enableTouch) {
            previousTouch_.resize(8, 0.0f);
        } else {
            NLM_LOG_WARNING("Touch disabled in sensory config");
        }
        
        if (sensoryConfig_.enableInternal) {
            previousInternal_.resize(4, 0.0f);
        } else {
            NLM_LOG_WARNING("Internal signals disabled in sensory config");
        }
        
        if (sensoryConfig_.enableProprioception) {
            previousProprioception_.resize(6, 0.0f);
        } else {
            NLM_LOG_WARNING("Proprioception disabled in sensory config");
        }
        
        if (sensoryConfig_.enableAudio) {
            previousAudio_.resize(0, 0.0f); // Audio not currently used
        } else {
            NLM_LOG_WARNING("Audio disabled in sensory config");
        }
        
        // Validate configuration
        bool hasEnabledSensory = sensoryConfig_.enableVision || 
                              sensoryConfig_.enableTouch || 
                              sensoryConfig_.enableInternal || 
                              sensoryConfig_.enableProprioception || 
                              sensoryConfig_.enableAudio;
        
        if (!hasEnabledSensory) {
            NLM_LOG_WARNING("No sensory modalities enabled - agent will have limited perception");
        }
        
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        
        // Calculate statistics with bounds checking
        size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() +
                                  motorTurnLeft_.size() + motorTurnRight_.size() +
                                  motorInteract_.size() + motorWait_.size();
        size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                   sensoryInternal_.size() + sensoryProprioception_.size() +
                                   sensoryAudio_.size();
        
        NLM_LOG_INFO("AgentBrain initialized with " + std::to_string(totalMotorNeurons) + 
                     " motor neurons, " + std::to_string(totalSensoryNeurons) + 
                     " sensory neurons");
                     
    } catch (const nlm::MemoryError& e) {
        NLM_LOG_ERROR(std::string("Memory allocation error during AgentBrain initialization: ") + e.what());
        throw;
    } catch (const nlm::ConfigError& e) {
        NLM_LOG_ERROR(std::string("Configuration error during AgentBrain initialization: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during AgentBrain initialization: ") + e.what());
        throw nlm::AgentBrainError(std::string("Failed to initialize AgentBrain: ") + e.what());
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    size_t size = 0;
    if (sensoryConfig_.enableVision) size += 256; // Vision (16x16)
    if (sensoryConfig_.enableTouch) size += 8;    // Touch
    if (sensoryConfig_.enableInternal) size += 4; // Internal
    if (sensoryConfig_.enableProprioception) size += 6; // Proprioception
    if (sensoryConfig_.enableAudio) size += 0;    // Audio
    return size;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::computeNoveltyFromAllModalities(const SensoryPercept& percept) {
    if (!curiosityEnabled_) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalNovelty = 0.0f;
    float totalWeight = 0.0f;
    
    if (sensoryConfig_.enableVision && !percept.getVision().empty() && !previousVision_.empty()) {
        computeVisionNovelty(percept.getVision(), previousVision_);
        totalNovelty += noveltyLevel_ * noveltyWeights_.vision;
        totalWeight += noveltyWeights_.vision;
    }
    
    if (sensoryConfig_.enableTouch && !percept.getTouch().empty() && !previousTouch_.empty()) {
        computeTouchNovelty(percept.getTouch(), previousTouch_);
        totalNovelty += noveltyLevel_ * noveltyWeights_.touch;
        totalWeight += noveltyWeights_.touch;
    }
    
    if (sensoryConfig_.enableInternal && !percept.getInternal().empty() && !previousInternal_.empty()) {
        computeInternalNovelty(percept.getInternal(), previousInternal_);
        totalNovelty += noveltyLevel_ * noveltyWeights_.internal;
        totalWeight += noveltyWeights_.internal;
    }
    
    if (sensoryConfig_.enableProprioception && !percept.getProprioception().empty() && !previousProprioception_.empty()) {
        computeProprioceptionNovelty(percept.getProprioception(), previousProprioception_);
        totalNovelty += noveltyLevel_ * noveltyWeights_.proprioception;
        totalWeight += noveltyWeights_.proprioception;
    }
    
    if (sensoryConfig_.enableAudio && !percept.getAudio().empty() && !previousAudio_.empty()) {
        computeAudioNovelty(percept.getAudio(), previousAudio_);
        totalNovelty += noveltyLevel_ * noveltyWeights_.audio;
        totalWeight += noveltyWeights_.audio;
    }
    
    // Decay overall novelty
    noveltyLevel_ *= noveltyDecayRate_;
    
    // Update based on weighted average
    if (totalWeight > 0.0f) {
        noveltyLevel_ = totalNovelty / totalWeight;
    }
}

void AgentBrain::computeVisionNovelty(const std::vector<float>& current, const std::vector<float>& previous) {
    if (current.empty() || previous.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareLen = std::min(current.size(), previous.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / compareLen;
    noveltyLevel_ = std::min(1.0f, avgDiff / 0.1f); // Normalize threshold
    previousVision_ = current;
}

void AgentBrain::computeTouchNovelty(const std::vector<float>& current, const std::vector<float>& previous) {
    if (current.empty() || previous.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareLen = std::min(current.size(), previous.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / compareLen;
    noveltyLevel_ = std::min(1.0f, avgDiff / 0.1f);
    previousTouch_ = current;
}

void AgentBrain::computeInternalNovelty(const std::vector<float>& current, const std::vector<float>& previous) {
    if (current.empty() || previous.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareLen = std::min(current.size(), previous.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / compareLen;
    noveltyLevel_ = std::min(1.0f, avgDiff / 0.1f);
    previousInternal_ = current;
}

void AgentBrain::computeProprioceptionNovelty(const std::vector<float>& current, const std::vector<float>& previous) {
    if (current.empty() || previous.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareLen = std::min(current.size(), previous.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / compareLen;
    noveltyLevel_ = std::min(1.0f, avgDiff / 0.1f);
    previousProprioception_ = current;
}

void AgentBrain::computeAudioNovelty(const std::vector<float>& current, const std::vector<float>& previous) {
    if (current.empty() || previous.empty()) {
        noveltyLevel_ = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareLen = std::min(current.size(), previous.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / compareLen;
    noveltyLevel_ = std::min(1.0f, avgDiff / 0.1f);
    previousAudio_ = current;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input
    if (sensoryConfig_.enableVision) {
        const auto& vision = percept.getVision();
        for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
            if (sensoryVision_[i]) {
                float current = vision[i] * 5.0f;
                sensoryVision_[i]->injectCurrent(current);
            }
        }
    }
    
    // Touch input
    if (sensoryConfig_.enableTouch) {
        const auto& touch = percept.getTouch();
        for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
            if (sensoryTouch_[i]) {
                float current = touch[i] * 8.0f;
                sensoryTouch_[i]->injectCurrent(current);
            }
        }
    }
    
    // Internal signals
    if (sensoryConfig_.enableInternal) {
        const auto& intern = percept.getInternal();
        for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
            if (sensoryInternal_[i]) {
                float current = (intern[i] * 2.0f - 1.0f) * 5.0f;
                sensoryInternal_[i]->injectCurrent(current);
            }
        }
    }
    
    // Proprioception
    if (sensoryConfig_.enableProprioception) {
        const auto& proprio = percept.getProprioception();
        for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
            if (sensoryProprioception_[i]) {
                float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;
                sensoryProprioception_[i]->injectCurrent(current);
            }
        }
    }
    
    // Audio (if enabled)
    if (sensoryConfig_.enableAudio) {
        const auto& audio = percept.getAudio();
        for (size_t i = 0; i < sensoryAudio_.size() && i < audio.size(); ++i) {
            if (sensoryAudio_[i]) {
                // Audio processing placeholder
                float current = audio[i] * 2.0f;
                sensoryAudio_[i]->injectCurrent(current);
            }
        }
    }
    
    // Compute novelty from all modalities
    computeNoveltyFromAllModalities(percept);
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = calculateCuriosityFromNovelty();
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > curiosityThreshold_) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
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
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity (configurable threshold)
    if (bestActivity < motorCommandThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Calculate exploration probability based on curiosity level
    float exploreProb = getExplorationProbability(curiosityLevel_);
    
    // Exploration: occasionally choose random action when curiosity is high
    float r = rng_() / (static_cast<float>(rng_.max()) + 1.0f);
    if (r < exploreProb) {
        return getRandomMotorCommand();
    }
    
    return defaultCmd;
}

float AgentBrain::getExplorationProbability(float curiosity) const {
    // Apply the configured exploration curve
    switch (explorationCurve_.type) {
        case ExplorationCurve::Linear:
            return explorationBaseProb_ + (1.0f - explorationBaseProb_) * curiosity;
        case ExplorationCurve::Exponential:
            return explorationBaseProb_ + (1.0f - explorationBaseProb_) * (1.0f - std::exp(-3.0f * curiosity));
        case ExplorationCurve::Sigmoid:
            return explorationBaseProb_ + (1.0f - explorationBaseProb_) / (1.0f + std::exp(-10.0f * (curiosity - explorationCurve_.inflectionPoint)));
        case ExplorationCurve::Logistic:
            return explorationBaseProb_ + (1.0f - explorationBaseProb_) / (1.0f + std::exp(-explorationCurve_.steepness * (curiosity - explorationCurve_.inflectionPoint)));
        default:
            return explorationBaseProb_ + (1.0f - explorationBaseProb_) * curiosity;
    }
}

MotorCommand AgentBrain::getRandomMotorCommand() const {
    std::uniform_int_distribution<size_t> dist(0, 5);
    size_t choice = dist(rng_);
    
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

bool AgentBrain::shouldExplore(float curiosity) const {
    return curiosity > curiosityThreshold_ && 
           getExplorationProbability(curiosity) > explorationBaseProb_;
}

float AgentBrain::calculateCuriosityFromNovelty() const {
    // Combine novelty with prediction error for curiosity
    float noveltyComponent = noveltyLevel_ * 2.0f;
    float predictionComponent = std::abs(predictionError_) * 0.5f;
    return noveltyComponent + predictionComponent;
}

size_t AgentBrain::getMotorNeuronCount(MotorCommand command) const {
    switch (command) {
        case MotorCommand::MoveForward: return motorForward_.size();
        case MotorCommand::MoveBackward: return motorBackward_.size();
        case MotorCommand::TurnLeft: return motorTurnLeft_.size();
        case MotorCommand::TurnRight: return motorTurnRight_.size();
        case MotorCommand::Interact: return motorInteract_.size();
        case MotorCommand::Wait: return motorWait_.size();
        default: return 0;
    }
}

size_t AgentBrain::getSensoryNeuronCount(const std::string& modality) const {
    static const std::unordered_map<std::string, size_t(const AgentBrain::*)() const> modalityMap = {
        {"vision", &AgentBrain::getVisionNeuronCount},
        {"touch", &AgentBrain::getTouchNeuronCount},
        {"internal", &AgentBrain::getInternalNeuronCount},
        {"proprioception", &AgentBrain::getProprioceptionNeuronCount},
        {"audio", &AgentBrain::getAudioNeuronCount}
    };
    
    auto it = modalityMap.find(modality);
    if (it != modalityMap.end()) {
        return (this->*it->second)();
    }
    return 0;
}

size_t AgentBrain::getVisionNeuronCount() const { return sensoryVision_.size(); }
size_t AgentBrain::getTouchNeuronCount() const { return sensoryTouch_.size(); }
size_t AgentBrain::getInternalNeuronCount() const { return sensoryInternal_.size(); }
size_t AgentBrain::getProprioceptionNeuronCount() const { return sensoryProprioception_.size(); }
size_t AgentBrain::getAudioNeuronCount() const { return sensoryAudio_.size(); }

float AgentBrain::getNeuronGroupActivity(const std::vector<Neuron*>& group) const {
    if (group.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : group) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / group.size();
}

float AgentBrain::getAverageMotorActivity() const {
    float totalActivity = 0.0f;
    float totalNeurons = 0.0f;
    
    totalActivity += getNeuronGroupActivity(motorForward_);
    totalActivity += getNeuronGroupActivity(motorBackward_);
    totalActivity += getNeuronGroupActivity(motorTurnLeft_);
    totalActivity += getNeuronGroupActivity(motorTurnRight_);
    totalActivity += getNeuronGroupActivity(motorInteract_);
    totalActivity += getNeuronGroupActivity(motorWait_);
    
    totalNeurons = motorForward_.size() + motorBackward_.size() +
                   motorTurnLeft_.size() + motorTurnRight_.size() +
                   motorInteract_.size() + motorWait_.size();
    
    return totalNeurons > 0.0f ? totalActivity / totalNeurons : 0.0f;
}

float AgentBrain::getAverageSensoryActivity() const {
    float totalActivity = 0.0f;
    float totalNeurons = 0.0f;
    
    totalActivity += getNeuronGroupActivity(sensoryVision_);
    totalActivity += getNeuronGroupActivity(sensoryTouch_);
    totalActivity += getNeuronGroupActivity(sensoryInternal_);
    totalActivity += getNeuronGroupActivity(sensoryProprioception_);
    totalActivity += getNeuronGroupActivity(sensoryAudio_);
    
    totalNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                   sensoryInternal_.size() + sensoryProprioception_.size() +
                   sensoryAudio_.size();
    
    return totalNeurons > 0.0f ? totalActivity / totalNeurons : 0.0f;
}

std::string AgentBrain::getDistributionStats() const {
    std::string stats = "Motor neuron distribution: ";
    stats += std::to_string(motorForward_.size()) + " forward, ";
    stats += std::to_string(motorBackward_.size()) + " backward, ";
    stats += std::to_string(motorTurnLeft_.size()) + " left, ";
    stats += std::to_string(motorTurnRight_.size()) + " right, ";
    stats += std::to_string(motorInteract_.size()) + " interact, ";
    stats += std::to_string(motorWait_.size()) + " wait.\n";
    
    stats += "Sensory neuron distribution: ";
    stats += std::to_string(sensoryVision_.size()) + " vision, ";
    stats += std::to_string(sensoryTouch_.size()) + " touch, ";
    stats += std::to_string(sensoryInternal_.size()) + " internal, ";
    stats += std::to_string(sensoryProprioception_.size()) + " proprioception, ";
    stats += std::to_string(sensoryAudio_.size()) + " audio.\n";
    
    return stats;
}

float AgentBrain::getTotalSensoryActivity() const {
    return getAverageSensoryActivity();
}

float AgentBrain::getTotalMotorActivity() const {
    return getAverageMotorActivity();
}

bool AgentBrain::isNeuronGroupEmpty(const std::vector<Neuron*>& group) const {
    return group.empty();
}

void AgentBrain::resetNoveltyHistory() {
    previousVision_.clear();
    previousTouch_.clear();
    previousInternal_.clear();
    previousProprioception_.clear();
    previousAudio_.clear();
    noveltyLevel_ = 0.0f;
}

float AgentBrain::getExplorationProbabilityForLevel(float curiosityLevel) const {
    return getExplorationProbability(curiosityLevel);
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    predictionError_ = reward - predictedReward;
    
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    if (developmentalAge_ < 60.0) {
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
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
    
    // Clear previous sensory states
    previousVision_.clear();
    previousTouch_.clear();
    previousInternal_.clear();
    previousProprioception_.clear();
    previousAudio_.clear();
    
    // Reinitialize distribution
    if (brain_) {
        updateDistribution();
    }
}
