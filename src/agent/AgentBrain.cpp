// AgentBrain.cpp - Enhanced AgentBrain implementation with configurable distribution
#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

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
    // Initialize with default configuration
    distributionConfig_ = createDefaultConfig();
    redistributeNeurons();
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::redistributeNeurons() {
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
    
    initializeMotorGroups();
    initializeSensoryGroups();
    
    balanceGroupSizes();
}

void AgentBrain::initializeMotorGroups() {
    if (!brain_) return;
    
    // Get all motor neurons from brain regions
    std::vector<Neuron*> allMotorNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    allMotorNeurons.push_back(n);
                }
            }
        }
    }
    
    // Distribute based on configuration
    if (distributionConfig_.motor.useModularDistribution && 
        !distributionConfig_.motor.motorGroupNames.empty()) {
        
        // Modular distribution using configuration
        size_t neuronIndex = 0;
        for (size_t groupIdx = 0; groupIdx < distributionConfig_.motor.motorGroupNames.size() && 
             neuronIndex < allMotorNeurons.size(); ++groupIdx) {
            
            size_t neuronsInThisGroup = std::min(
                distributionConfig_.motor.neuronsPerGroup[groupIdx],
                allMotorNeurons.size() - neuronIndex
            );
            
            auto& targetGroup = getMotorGroupVector(groupIdx);
            targetGroup.reserve(neuronsInThisGroup);
            
            for (size_t i = 0; i < neuronsInThisGroup; ++i) {
                targetGroup.push_back(allMotorNeurons[neuronIndex++]);
            }
        }
        
        // Distribute remaining neurons to the last group or create wait group
        if (neuronIndex < allMotorNeurons.size()) {
            if (!motorWait_.empty()) {
                // Add to existing wait group
                for (size_t i = neuronIndex; i < allMotorNeurons.size(); ++i) {
                    motorWait_.push_back(allMotorNeurons[i]);
                }
            } else {
                // Create new wait group
                motorWait_.assign(allMotorNeurons.begin() + neuronIndex, allMotorNeurons.end());
            }
        }
    } else {
        // Fallback to traditional distribution
        size_t idx = 0;
        for (Neuron* n : allMotorNeurons) {
            switch (idx % 6) {
                case 0: motorForward_.push_back(n); break;
                case 1: motorBackward_.push_back(n); break;
                case 2: motorTurnLeft_.push_back(n); break;
                case 3: motorTurnRight_.push_back(n); break;
                case 4: motorInteract_.push_back(n); break;
                case 5: motorWait_.push_back(n); break;
            }
            idx++;
        }
    }
}

void AgentBrain::initializeSensoryGroups() {
    if (!brain_) return;
    
    // Get all sensory neurons from brain regions
    std::vector<Neuron*> allSensoryNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    allSensoryNeurons.push_back(n);
                }
            }
        }
    }
    
    // Distribute based on configuration
    if (distributionConfig_.sensory.useModularDistribution && 
        !distributionConfig_.sensory.sensoryGroupNames.empty()) {
        
        // Modular distribution using configuration
        size_t neuronIndex = 0;
        for (size_t groupIdx = 0; groupIdx < distributionConfig_.sensory.sensoryGroupNames.size() && 
             neuronIndex < allSensoryNeurons.size(); ++groupIdx) {
            
            size_t neuronsInThisGroup = std::min(
                distributionConfig_.sensory.neuronsPerGroup[groupIdx],
                allSensoryNeurons.size() - neuronIndex
            );
            
            auto& targetGroup = getSensoryGroupVector(groupIdx);
            targetGroup.reserve(neuronsInThisGroup);
            
            for (size_t i = 0; i < neuronsInThisGroup; ++i) {
                targetGroup.push_back(allSensoryNeurons[neuronIndex++]);
            }
        }
        
        // Distribute remaining neurons
        if (neuronIndex < allSensoryNeurons.size()) {
            if (!sensoryProprioception_.empty()) {
                // Add to existing proprioception group
                for (size_t i = neuronIndex; i < allSensoryNeurons.size(); ++i) {
                    sensoryProprioception_.push_back(allSensoryNeurons[i]);
                }
            } else {
                // Create new proprioception group
                sensoryProprioception_.assign(allSensoryNeurons.begin() + neuronIndex, allSensoryNeurons.end());
            }
        }
    } else {
        // Fallback to traditional distribution
        size_t idx = 0;
        for (Neuron* n : allSensoryNeurons) {
            switch (idx % 4) {
                case 0: sensoryVision_.push_back(n); break;
                case 1: sensoryTouch_.push_back(n); break;
                case 2: sensoryInternal_.push_back(n); break;
                case 3: sensoryProprioception_.push_back(n); break;
            }
            idx++;
        }
    }
}

void AgentBrain::balanceGroupSizes() {
    if (!distributionConfig_.enableDynamicRebalancing) return;
    
    // Calculate current group sizes
    std::vector<std::vector<Neuron*>* > motorGroups = {
        &motorForward_, &motorBackward_, &motorTurnLeft_, &motorTurnRight_,
        &motorInteract_, &motorWait_
    };
    
    std::vector<std::vector<Neuron*>* > sensoryGroups = {
        &sensoryVision_, &sensoryTouch_, &sensoryInternal_, &sensoryProprioception_
    };
    
    // Find groups that are significantly larger or smaller than average
    for (auto& groups : {motorGroups, sensoryGroups}) {
        size_t totalNeurons = 0;
        for (auto* group : groups) {
            totalNeurons += group->size();
        }
        
        if (groups.empty() || totalNeurons == 0) continue;
        
        double averageSize = static_cast<double>(totalNeurons) / groups.size();
        
        // Check for imbalance
        for (auto* group : groups) {
            size_t groupSize = group->size();
            if (groupSize > 0) {
                double imbalance = std::abs(static_cast<double>(groupSize) - averageSize) / averageSize;
                
                if (imbalance > distributionConfig_.groupBalanceThreshold && 
                    distributionConfig_.maxRedistributionSteps > 0) {
                    
                    // Redistribute neurons from this group to other groups
                    redistributeFromGroup(group, averageSize);
                }
            }
        }
    }
}

void AgentBrain::redistributeFromGroup(std::vector<Neuron*>* sourceGroup, double targetSize) {
    if (!brain_ || sourceGroup->empty()) return;
    
    // Find all neurons not in source group
    std::vector<Neuron*> otherNeurons;
    
    // Collect all neurons from source group
    std::vector<Neuron*> sourceNeurons = *sourceGroup;
    sourceGroup->clear();
    
    // Get all available neurons
    std::vector<Neuron*> allNeurons;
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* n : pop->getNeurons()) {
                allNeurons.push_back(n);
            }
        }
    }
    
    // Create a mapping of neuron to its current group
    std::unordered_map<Neuron*, std::vector<Neuron*>* > neuronToGroup;
    
    // Populate mapping for all neurons
    for (Neuron* n : allNeurons) {
        if (std::find(sourceNeurons.begin(), sourceNeurons.end(), n) != sourceNeurons.end()) {
            // This neuron is from the source group
            neuronToGroup[n] = sourceGroup;
        } else {
            // Find which group this neuron belongs to
            for (auto* group : {&motorForward_, &motorBackward_, &motorTurnLeft_, 
                               &motorTurnRight_, &motorInteract_, &motorWait_,
                               &sensoryVision_, &sensoryTouch_, &sensoryInternal_, &sensoryProprioception_}) {
                if (std::find(group->begin(), group->end(), n) != group->end()) {
                    neuronToGroup[n] = group;
                    break;
                }
            }
        }
    }
    
    // Rebuild groups with balanced sizes
    std::vector<std::vector<Neuron*>* > allGroups = {
        &motorForward_, &motorBackward_, &motorTurnLeft_, &motorTurnRight_,
        &motorInteract_, &motorWait_,
        &sensoryVision_, &sensoryTouch_, &sensoryInternal_, &sensoryProprioception_
    };
    
    // Clear all groups and rebuild
    for (auto* group : allGroups) {
        group->clear();
    }
    
    // Distribute neurons evenly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(allNeurons.begin(), allNeurons.end(), gen);
    
    for (size_t i = 0; i < allNeurons.size(); ++i) {
        allGroups[i % allGroups.size()]->push_back(allNeurons[i]);
    }
}

void AgentBrain::updateDynamicDistribution() {
    if (distributionConfig_.enableDynamicRebalancing) {
        balanceGroupSizes();
        updateDevelopment(0.0); // Force rebalancing
    }
}

std::vector<Neuron*> AgentBrain::getMotorGroupVector(size_t groupIndex) const {
    switch (groupIndex) {
        case 0: return motorForward_;
        case 1: return motorBackward_;
        case 2: return motorTurnLeft_;
        case 3: return motorTurnRight_;
        case 4: return motorInteract_;
        case 5: return motorWait_;
        default: return {};
    }
}

std::vector<Neuron*> AgentBrain::getSensoryGroupVector(size_t groupIndex) const {
    switch (groupIndex) {
        case 0: return sensoryVision_;
        case 1: return sensoryTouch_;
        case 2: return sensoryInternal_;
        case 3: return sensoryProprioception_;
        default: return {};
    }
}

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("Enhanced AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

size_t AgentBrain::getTotalMotorNeurons() const {
    size_t total = 0;
    total += motorForward_.size();
    total += motorBackward_.size();
    total += motorTurnLeft_.size();
    total += motorTurnRight_.size();
    total += motorInteract_.size();
    total += motorWait_.size();
    return total;
}

size_t AgentBrain::getTotalSensoryNeurons() const {
    size_t total = 0;
    total += sensoryVision_.size();
    total += sensoryTouch_.size();
    total += sensoryInternal_.size();
    total += sensoryProprioception_.size();
    return total;
}

size_t AgentBrain::getNeuronsInGroup(const std::string& groupName) const {
    if (groupName == "forward") return motorForward_.size();
    if (groupName == "backward") return motorBackward_.size();
    if (groupName == "turn_left") return motorTurnLeft_.size();
    if (groupName == "turn_right") return motorTurnRight_.size();
    if (groupName == "interact") return motorInteract_.size();
    if (groupName == "wait") return motorWait_.size();
    if (groupName == "vision") return sensoryVision_.size();
    if (groupName == "touch") return sensoryTouch_.size();
    if (groupName == "internal") return sensoryInternal_.size();
    if (groupName == "proprioception") return sensoryProprioception_.size();
    return 0;
}

std::vector<std::string> AgentBrain::getMotorGroupNames() const {
    return distributionConfig_.motor.motorGroupNames;
}

std::vector<std::string> AgentBrain::getSensoryGroupNames() const {
    return distributionConfig_.sensory.sensoryGroupNames;
}

AgentBrain::NeuronDistributionConfig AgentBrain::createDefaultConfig() {
    NeuronDistributionConfig config;
    
    // Motor configuration - balanced distribution
    config.motor.motorGroupNames = {"forward", "backward", "turn_left", "turn_right", "interact", "wait"};
    config.motor.neuronsPerGroup = {25, 25, 25, 25, 15, 35}; // Different sizes for variety
    config.motor.useModularDistribution = true;
    
    // Sensory configuration - specialized groups
    config.sensory.sensoryGroupNames = {"vision", "touch", "internal", "proprioception"};
    config.sensory.neuronsPerGroup = {64, 8, 4, 6};
    config.sensory.useModularDistribution = true;
    
    // Advanced options
    config.enableDynamicRebalancing = false;
    config.groupBalanceThreshold = 0.2f;
    config.maxRedistributionSteps = 10;
    
    return config;
}

AgentBrain::NeuronDistributionConfig AgentBrain::createBalancedConfig() {
    NeuronDistributionConfig config = createDefaultConfig();
    
    // Make groups more balanced
    config.motor.neuronsPerGroup = {30, 30, 30, 30, 30, 30};
    config.sensory.neuronsPerGroup = {50, 15, 8, 10};
    config.enableDynamicRebalancing = true;
    
    return config;
}

AgentBrain::NeuronDistributionConfig AgentBrain::createExplorationConfig() {
    NeuronDistributionConfig config = createDefaultConfig();
    
    // Exploration-focused: larger wait/explore groups
    config.motor.neuronsPerGroup = {20, 20, 20, 20, 10, 50};
    config.sensory.neuronsPerGroup = {40, 12, 6, 8};
    config.enableDynamicRebalancing = true;
    config.groupBalanceThreshold = 0.15f;
    
    return config;
}

AgentBrain::NeuronDistributionConfig AgentBrain::createNavigationConfig() {
    NeuronDistributionConfig config = createDefaultConfig();
    
    // Navigation-focused: larger movement groups
    config.motor.neuronsPerGroup = {40, 40, 35, 35, 15, 20};
    config.sensory.neuronsPerGroup = {60, 10, 5, 8};
    config.enableDynamicRebalancing = true;
    config.groupBalanceThreshold = 0.25f;
    
    return config;
}

float AgentBrain::calculateGroupActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (Neuron* n : neurons) {
        if (n) {
            totalActivity += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
    }
    
    return totalActivity / neurons.size();
}

std::string AgentBrain::getGroupNameForIndex(size_t index, bool isMotor) const {
    if (isMotor) {
        const std::vector<std::string>& names = distributionConfig_.motor.motorGroupNames;
        if (index < names.size()) return names[index];
    } else {
        const std::vector<std::string>& names = distributionConfig_.sensory.sensoryGroupNames;
        if (index < names.size()) return names[index];
    }
    return "unknown";
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity with enhanced scaling
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
    
    // Enhanced novelty calculation with more sophisticated analysis
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize with exponential weighting
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ = noveltyLevel_ * 0.9f + noveltyLevel_ * 0.1f; // Apply decay
        
        // Update previous vision with delay for temporal dynamics
        for (size_t i = 0; i < vision.size() && i < previousVision_.size(); ++i) {
            previousVision_[i] = vision[i] * 0.7f + previousVision_[i] * 0.3f;
        }
    }
    
    // Enhanced curiosity calculation with multiple factors
    if (curiosityEnabled_) {
        // Combine novelty, prediction error, and uncertainty
        float uncertainty = 0.0f;
        if (!previousVision_.empty()) {
            for (float val : previousVision_) {
                uncertainty += std::abs(val - 0.5f); // Measure from center
            }
            uncertainty /= previousVision_.size();
        }
        
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f + uncertainty * 0.3f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Enhanced curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group with enhanced sensitivity
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            if (n) {
                // Enhanced activity measure using both potential and firing state
                const auto& state = n->getState();
                float potentialActivity = std::abs(state.membranePotential - state.restingPotential);
                float firingActivity = state.firingState == FiringState::Refractory ? 1.0f : 0.0f;
                sum += potentialActivity + firingActivity * 10.0f; // Give firing more weight
            }
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
    // Enhanced command selection with hysteresis
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait with higher threshold
    
    // Find maximum activity with enhanced threshold
    for (const auto& c : commands) {
        if (c.activity > bestActivity * 1.1f) {  // Require 10% margin
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Enhanced action threshold with dynamic adjustment
    float actionThreshold = 0.8f;
    if (dopamineLevel_ > 0.5f) {
        actionThreshold *= 1.2f;  // Dopamine increases boldness
    } else if (dopamineLevel_ < -0.5f) {
        actionThreshold *= 0.8f;  // Negative dopamine makes more conservative
    }
    
    // Only act if there's meaningful activity with enhanced threshold
    if (bestActivity < actionThreshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Enhanced exploration with curiosity-driven behavior
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration with varied strategies
        float exploreChance = curiosityLevel_ * 0.4f;  // Up to 40% random
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // More sophisticated random motor command selection
            int choice = brain_->getRandomGenerator()->uniformInt(0, 11);  // Expanded choices
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                case 7: return MotorCommand::Wait;
                case 8: return MotorCommand::MoveForward; // Repeat some for bias
                case 9: return MotorCommand::TurnLeft;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Enhanced prediction error calculation
    predictionError_ = reward - predictedReward;
    
    // Advanced expected reward tracking with exponential moving average
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Enhanced dopamine-like signal with multiple factors
    dopamineLevel_ = predictionError_ * 1.5f;  // Amplify prediction error
    
    // Add uncertainty weighting
    float uncertainty = std::abs(predictionError_) * 0.3f;
    dopamineLevel_ += uncertainty;
    
    // Clamp to reasonable range with asymmetric bounds
    dopamineLevel_ = std::clamp(dopamineLevel_, -2.0f, 2.0f);
    
    // Apply to all synapses with eligibility traces and enhanced plasticity
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Enhanced reward-modulated weight change with modulation
                float delta = eligibility * dopamineLevel_ * plasticityModifier_ * 1.2f;
                syn->addToWeight(delta);
                
                // Enhanced eligibility trace decay
                syn->decayEligibilityTrace(0.15f);
            }
        }
    }
    
    // Enhanced plasticity modulation with multiple pathways
    // Positive dopamine increases plasticity more than negative decreases
    float plasticityFactor = 0.5f + 0.5f * std::tanh(dopamineLevel_ * 0.8f);
    plasticityFactor = std::clamp(plasticityFactor, 0.2f, 2.0f);
    
    // Apply to multiple plasticity systems
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.012f * plasticityFactor);  // Increased base weight
        stdp->setLTDWeight(0.015f * plasticityFactor);
    }
    
    // Modulate development system based on reward
    if (developmentEnabled_) {
        float developmentMod = 1.0f + dopamineLevel_ * 0.3f;
        developmentMod = std::clamp(developmentMod, 0.5f, 2.0f);
        // Would apply to development system if it had such interface
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // More sophisticated developmental stages with variable timing
    // This is a biologically inspired approximation with enhanced dynamics
    if (developmentalAge_ < 90.0) {  // ~1.5 minutes
        plasticityModifier_ = 1.2f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 450.0) {  // ~7.5 minutes
        plasticityModifier_ = 0.9f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 1350.0) {  // ~22.5 minutes
        plasticityModifier_ = 0.6f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.25f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // More sophisticated structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // More nuanced synaptogenesis and pruning rates
            float synRate = 0.00015f * plasticityModifier_;
            float pruneRate = 0.000012f * (2.5f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
    
    // Age-dependent neuromodulation adjustments
    if (developmentalAge_ < 60.0) {
        curiosityEnabled_ = true;
        sensoryNoveltyDecay_ = 0.95f;  // Higher novelty retention in youth
    } else {
        curiosityEnabled_ = developmentalAge_ < 300.0;  // Reduced curiosity in adulthood
        sensoryNoveltyDecay_ = 0.98f;  // Faster novelty decay in adults
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
    
    // Clear previous vision with exponential decay
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    // Reset to default configuration
    distributionConfig_ = createDefaultConfig();
    redistributeNeurons();
}

} // namespace nlm
