#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <stdexcept>

namespace nlm {

// Validation error class for parameter validation failures in AgentBrain
class AgentBrainValidationError : public std::runtime_error {
public:
    explicit AgentBrainValidationError(const std::string& message) 
        : std::runtime_error("AgentBrain Validation Error: " + message) {}
    
    static AgentBrainValidationError InvalidDistributionRatio(const std::string& ratioName, float ratio) {
        return AgentBrainValidationError(ratioName + " must be in [0, 1] (got " + 
                                        std::to_string(ratio) + ")");
    }
    
    static AgentBrainValidationError InvalidDistributionSum(float total, float expected) {
        return AgentBrainValidationError("Distribution ratios must sum to 1.0 (got " + 
                                        std::to_string(total) + ", expected " + 
                                        std::to_string(expected) + ")");
    }
    
    static AgentBrainValidationError InvalidThreshold(const std::string& thresholdName, float value) {
        return AgentBrainValidationError(thresholdName + " must be positive (got " + 
                                        std::to_string(value) + ")");
    }
    
    static AgentBrainValidationError NullBrainPointer(const std::string& operation) {
        return AgentBrainValidationError("Null brain pointer detected in " + operation);
    }
    
    static AgentBrainValidationError OutOfBounds(const std::string& operation, size_t index, 
                                                size_t size, const std::string& arrayType) {
        return AgentBrainValidationError("Array access out of bounds in " + operation + ": " +
                                        arrayType + "[index=" + std::to_string(index) +
                                        ", size=" + std::to_string(size) + "]");
    }
};

// AgentBrain validation helper class
class AgentBrainValidator {
public:
    static bool validateMotorDistributionRatios(float forwardRatio, float backwardRatio,
                                               float leftRatio, float rightRatio,
                                               float interactRatio, float waitRatio,
                                               std::string& errorMessage) {
        float totalRatio = forwardRatio + backwardRatio + leftRatio + rightRatio + 
                          interactRatio + waitRatio;
        
        if (std::abs(totalRatio - 1.0f) > 0.01f) {
            errorMessage = "Motor distribution ratios must sum to 1.0 (got " + 
                          std::to_string(totalRatio) + ")";
            return false;
        }
        
        if (forwardRatio < 0.0f || backwardRatio < 0.0f || leftRatio < 0.0f || 
            rightRatio < 0.0f || interactRatio < 0.0f || waitRatio < 0.0f) {
            errorMessage = "All motor distribution ratios must be non-negative";
            return false;
        }
        
        return true;
    }
    
    static bool validateSensoryDistributionRatios(float visionRatio, float touchRatio,
                                                 float internalRatio, float proprioceptionRatio,
                                                 std::string& errorMessage) {
        float totalRatio = visionRatio + touchRatio + internalRatio + proprioceptionRatio;
        
        if (std::abs(totalRatio - 1.0f) > 0.01f) {
            errorMessage = "Sensory distribution ratios must sum to 1.0 (got " + 
                          std::to_string(totalRatio) + ")";
            return false;
        }
        
        if (visionRatio < 0.0f || touchRatio < 0.0f || internalRatio < 0.0f || 
            proprioceptionRatio < 0.0f) {
            errorMessage = "All sensory distribution ratios must be non-negative";
            return false;
        }
        
        return true;
    }
    
    static bool validateThreshold(float threshold, const std::string& thresholdName,
                                 std::string& errorMessage) {
        if (threshold < 0.0f) {
            errorMessage = thresholdName + " must be non-negative (got " + 
                          std::to_string(threshold) + ")";
            return false;
        }
        return true;
    }
    
    static bool validateProbability(float prob, const std::string& fieldName,
                                   std::string& errorMessage) {
        if (prob < 0.0f || prob > 1.0f) {
            errorMessage = fieldName + " must be in [0, 1] (got " + 
                          std::to_string(prob) + ")";
            return false;
        }
        return true;
    }
    
    static bool validateArrayBounds(size_t index, size_t size, const std::string& operation,
                                   const std::string& arrayType, std::string& errorMessage) {
        if (index >= size) {
            errorMessage = "Array access out of bounds in " + operation + ": " +
                          arrayType + "[index=" + std::to_string(index) +
                          ", size=" + std::to_string(size) + "]";
            return false;
        }
        return true;
    }
    
    static bool validateBrainPointer(const Brain* brain, const std::string& operation,
                                    std::string& errorMessage) {
        if (!brain) {
            errorMessage = "Null brain pointer detected in " + operation;
            return false;
        }
        return true;
    }
    
    static bool validateNeuronPointer(Neuron* neuron, const std::string& operation,
                                     std::string& errorMessage) {
        if (!neuron) {
            errorMessage = "Null neuron pointer detected in " + operation;
            return false;
        }
        return true;
    }
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain) : brain_(brain) {
    // Validate brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize with null brain pointer");
        throw AgentBrainValidationError::NullBrainPointer("AgentBrain constructor");
    }
    
    // Initialize with default configuration
    distributionConfig_ = DEFAULT_CONFIG;
    
    // Distribute neurons using configuration
    distributeMotorNeurons();
    distributeSensoryNeurons();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::distributeMotorNeurons() {
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::distributeMotorNeurons: Brain pointer is null");
        throw AgentBrainValidationError::NullBrainPointer("distributeMotorNeurons");
    }
    
    size_t totalNeurons = 0;
    for (const auto& region : brain_->getRegions()) {
        totalNeurons += region->getPopulationCount();
    }
    
    if (totalNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain: No neurons to distribute");
        return;
    }
    
    // Calculate neuron counts based on distribution ratios
    size_t forwardCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.motorForwardRatio_));
    size_t backwardCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.motorBackwardRatio_));
    size_t leftCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.motorTurnLeftRatio_));
    size_t rightCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.motorTurnRightRatio_));
    size_t interactCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.motorInteractRatio_));
    size_t waitCount = totalNeurons - (forwardCount + backwardCount + leftCount + rightCount + interactCount);
    
    // Ensure we don't exceed total neurons
    forwardCount = std::min(forwardCount, totalNeurons);
    backwardCount = std::min(backwardCount, totalNeurons - forwardCount);
    leftCount = std::min(leftCount, totalNeurons - forwardCount - backwardCount);
    rightCount = std::min(rightCount, totalNeurons - forwardCount - backwardCount - leftCount);
    interactCount = std::min(interactCount, totalNeurons - forwardCount - backwardCount - leftCount - rightCount);
    waitCount = totalNeurons - (forwardCount + backwardCount + leftCount + rightCount + interactCount);
    
    // Distribute neurons to groups
    distributeMotorNeuronsByCounts(forwardCount, backwardCount, leftCount, rightCount, interactCount, waitCount);
}

void AgentBrain::distributeMotorNeuronsByCounts(size_t forwardCount, size_t backwardCount,
                                                size_t leftCount, size_t rightCount,
                                                size_t interactCount, size_t waitCount) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::distributeMotorNeuronsByCounts: Brain pointer is null");
        throw AgentBrainValidationError::NullBrainPointer("distributeMotorNeuronsByCounts");
    }
    
    size_t distributed = 0;
    for (const auto& region : brain_->getRegions()) {
        if (!region) {
            NLM_LOG_ERROR("AgentBrain::distributeMotorNeuronsByCounts: Null region pointer");
            continue;
        }
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_ERROR("AgentBrain::distributeMotorNeuronsByCounts: Null population pointer");
                continue;
            }
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) {
                        NLM_LOG_ERROR("AgentBrain::distributeMotorNeuronsByCounts: Null neuron pointer");
                        continue;
                    }
                    if (distributed < forwardCount) {
                        motorForward_.push_back(n);
                        distributed++;
                    } else if (distributed < forwardCount + backwardCount) {
                        motorBackward_.push_back(n);
                        distributed++;
                    } else if (distributed < forwardCount + backwardCount + leftCount) {
                        motorTurnLeft_.push_back(n);
                        distributed++;
                    } else if (distributed < forwardCount + backwardCount + leftCount + rightCount) {
                        motorTurnRight_.push_back(n);
                        distributed++;
                    } else if (distributed < forwardCount + backwardCount + leftCount + rightCount + interactCount) {
                        motorInteract_.push_back(n);
                        distributed++;
                    } else if (distributed < forwardCount + backwardCount + leftCount + rightCount + interactCount + waitCount) {
                        motorWait_.push_back(n);
                        distributed++;
                    }
                }
            }
        }
    }
}

void AgentBrain::distributeSensoryNeurons() {
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::distributeSensoryNeurons: Brain pointer is null");
        throw AgentBrainValidationError::NullBrainPointer("distributeSensoryNeurons");
    }
    
    size_t totalNeurons = 0;
    for (const auto& region : brain_->getRegions()) {
        totalNeurons += region->getPopulationCount();
    }
    
    if (totalNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain: No neurons to distribute");
        return;
    }
    
    // Calculate neuron counts based on distribution ratios
    size_t visionCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.sensoryVisionRatio_));
    size_t touchCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.sensoryTouchRatio_));
    size_t internalCount = static_cast<size_t>(std::round(totalNeurons * distributionConfig_.sensoryInternalRatio_));
    size_t proprioceptionCount = totalNeurons - (visionCount + touchCount + internalCount);
    
    // Ensure we don't exceed total neurons
    visionCount = std::min(visionCount, totalNeurons);
    touchCount = std::min(touchCount, totalNeurons - visionCount);
    internalCount = std::min(internalCount, totalNeurons - visionCount - touchCount);
    proprioceptionCount = totalNeurons - (visionCount + touchCount + internalCount);
    
    // Distribute neurons to groups
    distributeSensoryNeuronsByCounts(visionCount, touchCount, internalCount, proprioceptionCount);
}

void AgentBrain::distributeSensoryNeuronsByCounts(size_t visionCount, size_t touchCount,
                                                size_t internalCount, size_t proprioceptionCount) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::distributeSensoryNeuronsByCounts: Brain pointer is null");
        throw AgentBrainValidationError::NullBrainPointer("distributeSensoryNeuronsByCounts");
    }
    
    size_t distributed = 0;
    for (const auto& region : brain_->getRegions()) {
        if (!region) {
            NLM_LOG_ERROR("AgentBrain::distributeSensoryNeuronsByCounts: Null region pointer");
            continue;
        }
        for (auto& pop : region->getPopulations()) {
            if (!pop) {
                NLM_LOG_ERROR("AgentBrain::distributeSensoryNeuronsByCounts: Null population pointer");
                continue;
            }
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    if (!n) {
                        NLM_LOG_ERROR("AgentBrain::distributeSensoryNeuronsByCounts: Null neuron pointer");
                        continue;
                    }
                    if (distributed < visionCount) {
                        sensoryVision_.push_back(n);
                        distributed++;
                    } else if (distributed < visionCount + touchCount) {
                        sensoryTouch_.push_back(n);
                        distributed++;
                    } else if (distributed < visionCount + touchCount + internalCount) {
                        sensoryInternal_.push_back(n);
                        distributed++;
                    } else if (distributed < visionCount + touchCount + internalCount + proprioceptionCount) {
                        sensoryProprioception_.push_back(n);
                        distributed++;
                    }
                }
            }
        }
    }
}

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
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Brain pointer is null");
        throw AgentBrainValidationError::NullBrainPointer("processSensoryInput");
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (!validateNeuronPointer(sensoryVision_[i], "processSensoryInput")) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: sensoryVision_ neuron is null");
            continue;
        }
        // Inject current proportional to vision intensity
        float current = vision[i] * 5.0f;  // Scale factor
        sensoryVision_[i]->injectCurrent(current);
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (!validateNeuronPointer(sensoryTouch_[i], "processSensoryInput")) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: sensoryTouch_ neuron is null");
            continue;
        }
        float current = touch[i] * 8.0f;  // Collision signal
        sensoryTouch_[i]->injectCurrent(current);
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < intern.size(); ++i) {
        if (!validateNeuronPointer(sensoryInternal_[i], "processSensoryInput")) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: sensoryInternal_ neuron is null");
            continue;
        }
        float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
        sensoryInternal_[i]->injectCurrent(current);
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (!validateNeuronPointer(sensoryProprioception_[i], "processSensoryInput")) {
            NLM_LOG_WARNING("AgentBrain::processSensoryInput: sensoryProprioception_ neuron is null");
            continue;
        }
        float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
        sensoryProprioception_[i]->injectCurrent(current);
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

float AgentBrain::calculateNeuronActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) {
        NLM_LOG_WARNING("AgentBrain::calculateNeuronActivity: Neuron vector is empty");
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        if (!validateNeuronPointer(n, "calculateNeuronActivity")) {
            NLM_LOG_WARNING("AgentBrain::calculateNeuronActivity: Neuron pointer is null");
            continue;
        }
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    
    return sum / neurons.size();
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group
    float forwardAct = calculateNeuronActivity(motorForward_);
    float backwardAct = calculateNeuronActivity(motorBackward_);
    float leftAct = calculateNeuronActivity(motorTurnLeft_);
    float rightAct = calculateNeuronActivity(motorTurnRight_);
    float interactAct = calculateNeuronActivity(motorInteract_);
    float waitAct = calculateNeuronActivity(motorWait_);
    
    // Validate activity values
    forwardAct = std::clamp(forwardAct, 0.0f, 10.0f);
    backwardAct = std::clamp(backwardAct, 0.0f, 10.0f);
    leftAct = std::clamp(leftAct, 0.0f, 10.0f);
    rightAct = std::clamp(rightAct, 0.0f, 10.0f);
    interactAct = std::clamp(interactAct, 0.0f, 10.0f);
    waitAct = std::clamp(waitAct, 0.0f, 10.0f);
    
    // Create command array
    struct MotorCommandActivity {
        MotorCommand cmd;
        float activity;
    };
    
    MotorCommandActivity commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    // Find maximum activity
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity (using configurable threshold)
    if (bestActivity < distributionConfig_.motorActivityThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::selectWithCuriosity: Brain pointer is null");
        return defaultCmd;
    }
    
    if (!validateBrainPointer(brain_, "selectWithCuriosity")) {
        return defaultCmd;
    }
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityEnabled_ && curiosityLevel_ > distributionConfig_.curiosityExplorationThreshold_) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * distributionConfig_.curiosityExplorationChanceMultiplier_;
        
        // Clamp exploration chance to reasonable range
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
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

void AgentBrain::setMotorDistributionRatios(
    float forwardRatio, float backwardRatio, float leftRatio, float rightRatio,
    float interactRatio, float waitRatio) {
    
    // Validate ratios using AgentBrainValidator
    std::string errorMessage;
    if (!AgentBrainValidator::validateMotorDistributionRatios(
        forwardRatio, backwardRatio, leftRatio, rightRatio, 
        interactRatio, waitRatio, errorMessage)) {
        NLM_LOG_ERROR("AgentBrain::setMotorDistributionRatios: " + errorMessage);
        return;
    }
    
    distributionConfig_.motorForwardRatio_ = forwardRatio;
    distributionConfig_.motorBackwardRatio_ = backwardRatio;
    distributionConfig_.motorTurnLeftRatio_ = leftRatio;
    distributionConfig_.motorTurnRightRatio_ = rightRatio;
    distributionConfig_.motorInteractRatio_ = interactRatio;
    distributionConfig_.motorWaitRatio_ = waitRatio;
    
    // Redistribute neurons if brain exists
    if (brain_) {
        distributeMotorNeurons();
    }
}

void AgentBrain::setSensoryDistributionRatios(
    float visionRatio, float touchRatio, float internalRatio, float proprioceptionRatio) {
    
    // Validate ratios using AgentBrainValidator
    std::string errorMessage;
    if (!AgentBrainValidator::validateSensoryDistributionRatios(
        visionRatio, touchRatio, internalRatio, proprioceptionRatio, errorMessage)) {
        NLM_LOG_ERROR("AgentBrain::setSensoryDistributionRatios: " + errorMessage);
        return;
    }
    
    distributionConfig_.sensoryVisionRatio_ = visionRatio;
    distributionConfig_.sensoryTouchRatio_ = touchRatio;
    distributionConfig_.sensoryInternalRatio_ = internalRatio;
    distributionConfig_.sensoryProprioceptionRatio_ = proprioceptionRatio;
    
    // Redistribute neurons if brain exists
    if (brain_) {
        distributeSensoryNeurons();
    }
}

void AgentBrain::setMotorActivityThreshold(float threshold) {
    // Validate threshold using AgentBrainValidator
    std::string errorMessage;
    if (!AgentBrainValidator::validateThreshold(threshold, "Motor activity threshold", errorMessage)) {
        NLM_LOG_ERROR("AgentBrain::setMotorActivityThreshold: " + errorMessage);
        return;
    }
    
    distributionConfig_.motorActivityThreshold_ = threshold;
}

void AgentBrain::setCuriosityExplorationThreshold(float threshold) {
    // Validate threshold using AgentBrainValidator
    std::string errorMessage;
    if (!AgentBrainValidator::validateThreshold(threshold, "Curiosity exploration threshold", errorMessage)) {
        NLM_LOG_ERROR("AgentBrain::setCuriosityExplorationThreshold: " + errorMessage);
        return;
    }
    
    // Additional validation for probability range
    if (threshold < 0.0f || threshold > 1.0f) {
        NLM_LOG_ERROR("AgentBrain::setCuriosityExplorationThreshold: Threshold must be between 0.0 and 1.0");
        return;
    }
    
    distributionConfig_.curiosityExplorationThreshold_ = threshold;
}

void AgentBrain::setCuriosityExplorationChanceMultiplier(float multiplier) {
    // Additional validation for probability range
    if (multiplier < 0.0f || multiplier > 1.0f) {
        NLM_LOG_ERROR("AgentBrain::setCuriosityExplorationChanceMultiplier: Multiplier must be between 0.0 and 1.0");
        return;
    }
    
    distributionConfig_.curiosityExplorationChanceMultiplier_ = multiplier;
}

void AgentBrain::enableRandomExploration(bool enable) {
    // Enable/disable the random exploration feature
    // Note: This is more of a configuration flag since random exploration is already implemented
    // through the curiosity system
}

void AgentBrain::enableMotorDecoding(bool enable) {
    // Enable/disable motor decoding
    // Note: This is more of a configuration flag since motor decoding is always enabled
    // if a brain is present
}

void AgentBrain::validateParameters() const {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::validateParameters: Brain pointer is null");
    }
    
    // Validate neuron groups
    if (motorForward_.empty() && motorBackward_.empty() && motorTurnLeft_.empty() && 
        motorTurnRight_.empty() && motorInteract_.empty() && motorWait_.empty()) {
        NLM_LOG_WARNING("AgentBrain::validateParameters: No motor neurons distributed");
    }
    
    if (sensoryVision_.empty() && sensoryTouch_.empty() && sensoryInternal_.empty() && 
        sensoryProprioception_.empty()) {
        NLM_LOG_WARNING("AgentBrain::validateParameters: No sensory neurons distributed");
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("AgentBrain::applyRewardModulation: Brain pointer is null");
        }
        return;
    }
    
    // Validate reward values
    if (std::isnan(reward) || std::isinf(reward)) {
        NLM_LOG_ERROR("AgentBrain::applyRewardModulation: Invalid reward value");
        return;
    }
    
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
    if (!brain_ || !developmentEnabled_) {
        if (!brain_) {
            NLM_LOG_WARNING("AgentBrain::updateDevelopment: Brain pointer is null");
        }
        return;
    }
    
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
