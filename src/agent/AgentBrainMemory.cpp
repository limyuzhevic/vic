#include "AgentBrainMemory.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <iostream>
#include <unordered_map>

namespace nlm {

// AgentBrainMemoryManager implementation
AgentBrainMemoryManager& AgentBrainMemoryManager::getInstance() {
    static AgentBrainMemoryManager instance;
    return instance;
}

void AgentBrainMemoryManager::registerNeuron(std::shared_ptr<Neuron> neuron, AgentBrain& agentBrain) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    // Check if this AgentBrain already has this neuron
    auto& neurons = agentBrainNeurons_[&agentBrain];
    bool alreadyRegistered = false;
    for (const auto& weakNeuron : neurons) {
        if (weakNeuron.lock() == neuron) {
            alreadyRegistered = true;
            break;
        }
    }
    
    if (!alreadyRegistered) {
        neurons.push_back(neuron);
        totalNeuronCount_++;
        NLM_LOG_DEBUG("Registered neuron " + std::to_string(neuron->getId().index()) + 
                     " with AgentBrain at " + std::to_string(reinterpret_cast<uintptr_t>(&agentBrain)));
    }
}

void AgentBrainMemoryManager::unregisterNeuron(std::shared_ptr<Neuron> neuron, AgentBrain& agentBrain) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    auto it = agentBrainNeurons_.find(&agentBrain);
    if (it != agentBrainNeurons_.end()) {
        auto& neurons = it->second;
        neurons.erase(std::remove_if(neurons.begin(), neurons.end(),
            [&neuron](const std::weak_ptr<Neuron>& weakNeuron) {
                return weakNeuron.lock() == neuron;
            }),
            neurons.end());
        
        if (neurons.empty()) {
            agentBrainNeurons_.erase(it);
        }
        
        totalNeuronCount_--;
        NLM_LOG_DEBUG("Unregistered neuron " + std::to_string(neuron->getId().index()) + 
                     " from AgentBrain at " + std::to_string(reinterpret_cast<uintptr_t>(&agentBrain)));
    }
}

std::vector<std::weak_ptr<Neuron>> AgentBrainMemoryManager::getRegisteredNeurons(AgentBrain& agentBrain) {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    auto it = agentBrainNeurons_.find(&agentBrain);
    if (it != agentBrainNeurons_.end()) {
        return it->second;
    }
    
    return std::vector<std::weak_ptr<Neuron>>();
}

void AgentBrainMemoryManager::clearAgentBrain(AgentBrain& agentBrain) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    auto it = agentBrainNeurons_.find(&agentBrain);
    if (it != agentBrainNeurons_.end()) {
        totalNeuronCount_ -= it->second.size();
        agentBrainNeurons_.erase(it);
        NLM_LOG_DEBUG("Cleared all neurons for AgentBrain at " + std::to_string(reinterpret_cast<uintptr_t>(&agentBrain)));
    }
}

// Helper functions implementation
namespace AgentBrainMemory {
    
    Neuron* safeGetNeuron(const std::weak_ptr<Neuron>& weakNeuron) {
        if (auto locked = weakNeuron.lock()) {
            return locked.get();
        }
        return nullptr;
    }
    
    bool hasValidWeakPointers(const std::vector<std::weak_ptr<Neuron>>& weakPtrs) {
        for (const auto& weakPtr : weakPtrs) {
            if (weakPtr.lock()) {
                return true;
            }
        }
        return false;
    }
    
    size_t countValidWeakPointers(const std::vector<std::weak_ptr<Neuron>>& weakPtrs) {
        size_t count = 0;
        for (const auto& weakPtr : weakPtrs) {
            if (weakPtr.lock()) {
                count++;
            }
        }
        return count;
    }
    
    void updateAgentBrainNeuronCollections(AgentBrain& agentBrain) {
        // This function would update the AgentBrain's weak pointer collections
        // by scanning through the brain's neural regions and populations
        // For now, it's a placeholder that would be implemented as needed
        NLM_LOG_INFO("Updating AgentBrain neuron collections");
    }
}

// AgentBrainEnhanced implementation
AgentBrainEnhanced::AgentBrainEnhanced(std::shared_ptr<Brain> brain)
    : originalBrain_(std::move(brain))
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , sensoryNoveltyDecay_(0.99f) {
    
    NLM_LOG_INFO("AgentBrainEnhanced initialized");
}

AgentBrainEnhanced::~AgentBrainEnhanced() {
    // Clear memory manager registrations
    AgentBrainMemoryManager::getInstance().clearAgentBrain(originalBrain_);
    NLM_LOG_INFO("AgentBrainEnhanced destroyed");
}

AgentBrainEnhanced::AgentBrainEnhanced(AgentBrainEnhanced&& other) noexcept
    : originalBrain_(std::move(other.originalBrain_))
    , motorForward_(std::move(other.motorForward_))
    , motorBackward_(std::move(other.motorBackward_))
    , motorTurnLeft_(std::move(other.motorTurnLeft_))
    , motorTurnRight_(std::move(other.motorTurnRight_))
    , motorInteract_(std::move(other.motorInteract_))
    , motorWait_(std::move(other.motorWait_))
    , sensoryVision_(std::move(other.sensoryVision_))
    , sensoryTouch_(std::move(other.sensoryTouch_))
    , sensoryInternal_(std::move(other.sensoryInternal_))
    , sensoryProprioception_(std::move(other.sensoryProprioception_))
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltyLevel_)
    , curiosityLevel_(other.curiosityLevel_)
    , predictionError_(other.predictionError_)
    , expectedReward_(other.expectedReward_)
    , developmentalAge_(other.developmentalAge_)
    , plasticityModifier_(other.plasticityModifier_)
    , sensoryNoveltyDecay_(other.sensoryNoveltyDecay_)
    , previousVision_(std::move(other.previousVision_)) {
    
    // Update memory manager registrations
    AgentBrainMemoryManager::getInstance().clearAgentBrain(originalBrain_);
    other.clearAgentBrainInternal();
}

AgentBrainEnhanced& AgentBrainEnhanced::operator=(AgentBrainEnhanced&& other) noexcept {
    if (this != &other) {
        // Clear current registrations
        AgentBrainMemoryManager::getInstance().clearAgentBrain(originalBrain_);
        
        // Move data
        originalBrain_ = std::move(other.originalBrain_);
        motorForward_ = std::move(other.motorForward_);
        motorBackward_ = std::move(other.motorBackward_);
        motorTurnLeft_ = std::move(other.motorTurnLeft_);
        motorTurnRight_ = std::move(other.motorTurnRight_);
        motorInteract_ = std::move(other.motorInteract_);
        motorWait_ = std::move(other.motorWait_);
        sensoryVision_ = std::move(other.sensoryVision_);
        sensoryTouch_ = std::move(other.sensoryTouch_);
        sensoryInternal_ = std::move(other.sensoryInternal_);
        sensoryProprioception_ = std::move(other.sensoryProprioception_);
        
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        sensoryNoveltyDecay_ = other.sensoryNoveltyDecay_;
        previousVision_ = std::move(other.previousVision_);
        
        // Update memory manager registrations
        AgentBrainMemoryManager::getInstance().clearAgentBrain(originalBrain_);
        other.clearAgentBrainInternal();
    }
    return *this;
}

void AgentBrainEnhanced::synchronizeNeuronCollections() {
    std::unique_lock<std::shared_mutex> lock(memoryLock_);
    
    // Clear existing collections
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
    
    // Update from original AgentBrain if it has neuron management
    if (originalBrain_.hasNeuronManagement()) {
        // This would be implemented based on how the original AgentBrain manages neurons
        // For now, it's a placeholder
        NLM_LOG_INFO("Synchronizing neuron collections from original AgentBrain");
    }
}

void AgentBrainEnhanced::initialize(const SimpleWorld& world) {
    originalBrain_.initialize(world);
    synchronizeNeuronCollections();
}

void AgentBrainEnhanced::processSensoryInput(const SensoryPercept& percept) {
    originalBrain_.processSensoryInput(percept);
    synchronizeNeuronCollections();
}

MotorCommand AgentBrainEnhanced::decodeMotorCommand() {
    return originalBrain_.decodeMotorCommand();
}

MotorCommand AgentBrainEnhanced::decodeMotorCommandWithCuriosity() {
    // Enhanced version with curiosity-based exploration
    MotorCommand decoded = decodeMotorCommand();
    
    // Apply curiosity-based exploration if enabled
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

void AgentBrainEnhanced::applyRewardModulation(float reward, float predictedReward) {
    originalBrain_.applyRewardModulation(reward, predictedReward);
    
    // Update local state
    predictionError_ = reward - predictedReward;
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
}

void AgentBrainEnhanced::updateDevelopment(double timestep) {
    if (developmentEnabled_) {
        developmentalAge_ += timestep;
        
        // Simple developmental stages
        if (developmentalAge_ < 60.0) {
            plasticityModifier_ = 1.0f;
        } else if (developmentalAge_ < 300.0) {
            plasticityModifier_ = 0.8f;
        } else if (developmentalAge_ < 900.0) {
            plasticityModifier_ = 0.5f;
        } else {
            plasticityModifier_ = 0.2f;
        }
    }
}

void AgentBrainEnhanced::reset() {
    originalBrain_.reset();
    
    // Reset local state
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    previousVision_.clear();
    
    // Update neuron collections
    synchronizeNeuronCollections();
}

bool AgentBrainEnhanced::isNeuronCollectionValid() const {
    std::shared_lock<std::shared_mutex> lock(memoryLock_);
    
    return AgentBrainMemory::hasValidWeakPointers(motorForward_) ||
           AgentBrainMemory::hasValidWeakPointers(sensoryVision_);
}

size_t AgentBrainEnhanced::getValidNeuronCount() const {
    std::shared_lock<std::shared_mutex> lock(memoryLock_);
    
    size_t validCount = 0;
    validCount += AgentBrainMemory::countValidWeakPointers(motorForward_);
    validCount += AgentBrainMemory::countValidWeakPointers(motorBackward_);
    validCount += AgentBrainMemory::countValidWeakPointers(motorTurnLeft_);
    validCount += AgentBrainMemory::countValidWeakPointers(motorTurnRight_);
    validCount += AgentBrainMemory::countValidWeakPointers(motorInteract_);
    validCount += AgentBrainMemory::countValidWeakPointers(motorWait_);
    validCount += AgentBrainMemory::countValidWeakPointers(sensoryVision_);
    validCount += AgentBrainMemory::countValidWeakPointers(sensoryTouch_);
    validCount += AgentBrainMemory::countValidWeakPointers(sensoryInternal_);
    validCount += AgentBrainMemory::countValidWeakPointers(sensoryProprioception_);
    
    return validCount;
}

MotorCommand AgentBrainEnhanced::selectWithCuriosity(MotorCommand defaultCmd) {
    // Implementation of curiosity-based selection
    // This is a simplified version - the actual implementation would be more complex
    if (curiosityLevel_ > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel_ * 0.3f; // Up to 30% random
        
        float r = originalBrain_.getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command
            int choice = originalBrain_.getRandomGenerator()->uniformInt(0, 7);
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

void AgentBrainEnhanced::clearAgentBrainInternal() {
    // Internal cleanup for the moved-from object
    // This is called during move operations
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
    previousVision_.clear();
}

} // namespace nlm
