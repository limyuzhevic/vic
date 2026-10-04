#ifndef NLM_AGENT_BRAIN_MEMORY_HPP
#define NLM_AGENT_BRAIN_MEMORY_HPP

#include "AgentBrain.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include <memory>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace nlm {

// Forward declarations for forward references
// Neuron is already included above

class AgentBrainMemoryManager {
public:
    static AgentBrainMemoryManager& getInstance() {
        static AgentBrainMemoryManager instance;
        return instance;
    }
    
    // Register neuron with AgentBrain
    void registerNeuron(std::shared_ptr<Neuron> neuron, AgentBrain& agentBrain);
    
    // Unregister neuron from AgentBrain
    void unregisterNeuron(std::shared_ptr<Neuron> neuron, AgentBrain& agentBrain);
    
    // Get all neurons registered with an AgentBrain
    std::vector<std::weak_ptr<Neuron>> getRegisteredNeurons(AgentBrain& agentBrain);
    
    // Clear all registrations for an AgentBrain
    void clearAgentBrain(AgentBrain& agentBrain);
    
    // Get neuron count for debugging
    size_t getNeuronCount() const { return totalNeuronCount_; }
    
private:
    AgentBrainMemoryManager() : totalNeuronCount_(0) {}
    ~AgentBrainMemoryManager() = default;
    
    // Map from AgentBrain to its registered neurons
    std::unordered_map<AgentBrain*, std::vector<std::weak_ptr<Neuron>>> agentBrainNeurons_;
    
    // Counter for total neurons tracked
    std::atomic<size_t> totalNeuronCount_;
    
    // Mutex for thread safety
    mutable std::shared_mutex mutex_;
    
    // Prevent copying
    AgentBrainMemoryManager(const AgentBrainMemoryManager&) = delete;
    AgentBrainMemoryManager& operator=(const AgentBrainMemoryManager&) = delete;
};

// Helper functions for safe pointer operations
namespace AgentBrainMemory {
    
    // Safely lock weak pointers and return valid pointers
    template<typename T>
    std::vector<T*> lockWeakPointers(const std::vector<std::weak_ptr<T>>& weakPtrs) {
        std::vector<T*> result;
        result.reserve(weakPtrs.size());
        for (const auto& weakPtr : weakPtrs) {
            if (auto locked = weakPtr.lock()) {
                result.push_back(locked.get());
            }
        }
        return result;
    }
    
    // Check if any weak pointer is still valid
    template<typename T>
    bool hasValidWeakPointers(const std::vector<std::weak_ptr<T>>& weakPtrs) {
        for (const auto& weakPtr : weakPtrs) {
            if (weakPtr.lock()) {
                return true;
            }
        }
        return false;
    }
    
    // Safe neuron access with validation
    Neuron* safeGetNeuron(const std::weak_ptr<Neuron>& weakNeuron);
    
    // Count valid weak pointers
    template<typename T>
    size_t countValidWeakPointers(const std::vector<std::weak_ptr<T>>& weakPtrs) {
        size_t count = 0;
        for (const auto& weakPtr : weakPtrs) {
            if (weakPtr.lock()) {
                count++;
            }
        }
        return count;
    }
    
    // Update AgentBrain's neuron collections with current neuron state
    void updateAgentBrainNeuronCollections(AgentBrain& agentBrain);
}

// Enhanced AgentBrain with memory management
class AgentBrainEnhanced {
public:
    AgentBrainEnhanced(std::shared_ptr<Brain> brain);
    ~AgentBrainEnhanced();
    
    // Disable copying
    AgentBrainEnhanced(const AgentBrainEnhanced&) = delete;
    AgentBrainEnhanced& operator=(const AgentBrainEnhanced&) = delete;
    
    // Enable moving
    AgentBrainEnhanced(AgentBrainEnhanced&& other) noexcept;
    AgentBrainEnhanced& operator=(AgentBrainEnhanced&& other) noexcept;
    
    // Core functionality
    void initialize(const SimpleWorld& world);
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(double timestep);
    void reset();
    
    // Memory management
    void updateNeuronCollections();  // Update weak pointer collections
    bool isNeuronCollectionValid() const;  // Check if collections are valid
    size_t getValidNeuronCount() const;   // Get count of valid neurons
    
    // Access to enhanced functionality
    const std::vector<std::weak_ptr<Neuron>>& getMotorForward() const { return motorForward_; }
    const std::vector<std::weak_ptr<Neuron>>& getSensoryVision() const { return sensoryVision_; }
    
    // Original AgentBrain methods (for compatibility)
    size_t getSensoryInputSize() const { return originalBrain_.getSensoryInputSize(); }
    size_t getMotorOutputSize() const { return originalBrain_.getMotorOutputSize(); }
    MotorCommand decodeMotorCommandWithCuriosity();  // Enhanced version
    
    // Get the original brain
    Brain* getBrain() { return originalBrain_.getBrain(); }
    
private:
    // Original AgentBrain functionality
    AgentBrain originalBrain_;
    
    // Enhanced neuron collections with weak pointers
    std::vector<std::weak_ptr<Neuron>> motorForward_;
    std::vector<std::weak_ptr<Neuron>> motorBackward_;
    std::vector<std::weak_ptr<Neuron>> motorTurnLeft_;
    std::vector<std::weak_ptr<Neuron>> motorTurnRight_;
    std::vector<std::weak_ptr<Neuron>> motorInteract_;
    std::vector<std::weak_ptr<Neuron>> motorWait_;
    
    std::vector<std::weak_ptr<Neuron>> sensoryVision_;
    std::vector<std::weak_ptr<Neuron>> sensoryTouch_;
    std::vector<std::weak_ptr<Neuron>> sensoryInternal_;
    std::vector<std::weak_ptr<Neuron>> sensoryProprioception_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // State
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    double developmentalAge_;
    float plasticityModifier_;
    double sensoryNoveltyDecay_;
    
    // Previous vision for novelty detection
    std::vector<float> previousVision_;
    
    // Lock for thread safety
    mutable std::shared_mutex memoryLock_;
    
    // Update the enhanced neuron collections from the original AgentBrain
    void synchronizeNeuronCollections();
    
    // Internal cleanup for moved-from object
    void clearAgentBrainInternal();
    
    // Friend declaration for memory manager
    friend class AgentBrainMemoryManager;
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_MEMORY_HPP
