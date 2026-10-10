#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

// ========= FORWARD DECLARATIONS =========
class MotorDecoder;
class SensoryProcessor;
class NeuromodulationManager;
class DevelopmentManager;
class ExplorationManager;

// ========= COMPONENT CLASSES ==========

/**
 * @brief Manages motor neuron group classification and activity calculation
 */
class MotorGroupManager {
public:
    MotorGroupManager();
    ~MotorGroupManager() = default;
    
    // Disable copying, enable moving
    MotorGroupManager(const MotorGroupManager&) = delete;
    MotorGroupManager& operator=(const MotorGroupManager&) = delete;
    MotorGroupManager(MotorGroupManager&&) noexcept = default;
    MotorGroupManager& operator=(MotorGroupManager&&) noexcept = default;
    
    // Add neuron to appropriate motor group based on its position/index
    void registerNeuron(Neuron* neuron);
    
    // Get activity of a motor group
    float getGroupActivity(const std::string& group) const;
    
    // Update group activities from current neuron states
    void updateGroupActivities(const std::unordered_map<std::string, std::vector<Neuron*>>& groups);
    
    // Get neurons in a specific group
    const std::vector<Neuron*>& getGroup(const std::string& group);
    
    // Get all motor group names
    std::vector<std::string> getGroupNames() const;
    
private:
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
    size_t getNextIndex();
    
    std::vector<std::string> groupNames_;
    std::unordered_map<std::string, std::vector<Neuron*>> groups_;
    std::unordered_map<std::string, float> groupActivities_;
    size_t nextIndex_;
};

/**
 * @brief Manages sensory input processing and transduction
 */
class SensoryProcessingManager {
public:
    SensoryProcessingManager();
    ~SensoryProcessingManager() = default;
    
    // Disable copying, enable moving
    SensoryProcessingManager(const SensoryProcessingManager&) = delete;
    SensoryProcessingManager& operator=(const SensoryProcessingManager&) = delete;
    SensoryProcessingManager(SensoryProcessingManager&&) noexcept = default;
    SensoryProcessingManager& operator=(SensoryProcessingManager&&) noexcept = default;
    
    // Register sensory neurons by type
    void registerVisionNeurons(const std::vector<Neuron*>& neurons);
    void registerTouchNeurons(const std::vector<Neuron*>& neurons);
    void registerInternalNeurons(const std::vector<Neuron*>& neurons);
    void registerProprioceptionNeurons(const std::vector<Neuron*>& neurons);
    
    // Process sensory percepts
    void processVision(const std::vector<float>& visionData);
    void processTouch(const std::vector<float>& touchData);
    void processInternal(const std::vector<float>& internalData);
    void processProprioception(const std::vector<float>& proprioData);
    
    // Compute novelty from vision data
    float computeNovelty(const std::vector<float>& currentVision, float decayFactor);
    
    // Get sensory neuron counts
    size_t getTotalSensoryNeuronCount() const;
    size_t getVisionNeuronCount() const;
    size_t getTouchNeuronCount() const;
    size_t getInternalNeuronCount() const;
    size_t getProprioceptionNeuronCount() const;
    
    void setWorkingMemory(NeuralWorkingMemory* workingMemory) {
        workingMemory_ = workingMemory;
    }
    
private:
    void injectScaledCurrent(const std::vector<Neuron*>& neurons, 
                           const std::vector<float>& data, 
                           float scaleFactor, 
                           const std::string& modality);
    
    std::vector<Neuron*> visionNeurons_;
    std::vector<Neuron*> touchNeurons_;
    std::vector<Neuron*> internalNeurons_;
    std::vector<Neuron*> proprioceptionNeurons_;
    std::vector<float> previousVision_;
    NeuralWorkingMemory* workingMemory_;
};

/**
 * @brief Handles neuromodulation processing and effects
 */
class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager() = default;
    
    // Disable copying, enable moving
    NeuromodulationManager(const NeuromodulationManager&) = delete;
    NeuromodulationManager& operator=(const NeuromodulationManager&) = delete;
    NeuromodulationManager(NeuromodulationManager&&) noexcept = default;
    NeuromodulationManager& operator=(NeuromodulationManager&&) noexcept = default;
    
    // Update neuromodulation state
    void update(float timestep);
    
    // Apply reward-based neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Get current neuromodulation levels
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getDopamineLevel() const;
    float getPredictionError() const;
    float getExpectedReward() const;
    
    // Configuration
    void enableCuriosity(bool enable);
    void enableRewardModulation(bool enable);
    void setSensoryNoveltyDecay(float decay);
    
    // Set brain reference
    void setBrain(Brain* brain) {
        brain_ = brain;
    }
    
private:
    void applySynapticModulation();
    void updatePlasticityFactors();
    void reset();
    
    // State
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float rewardPredictionError_;
    
    // Configuration
    bool enableCuriosity_;
    bool enableRewardModulation_;
    float sensoryNoveltyDecay_;
    
    // Dependencies
    Brain* brain_;
    
    friend class AgentBrain; // AgentBrain manages neuromodulation
};

/**
 * @brief Handles developmental progression and age-based effects
 */
class DevelopmentManager {
public:
    DevelopmentManager();
    ~DevelopmentManager() = default;
    
    // Disable copying, enable moving
    DevelopmentManager(const DevelopmentManager&) = delete;
    DevelopmentManager& operator=(const DevelopmentManager&) = delete;
    DevelopmentManager(DevelopmentManager&&) noexcept = default;
    DevelopmentManager& operator=(DevelopmentManager&&) noexcept = default;
    
    // Update development state
    void update(double timestep);
    
    // Get development state
    DevelopmentalStage getCurrentStage() const;
    float getPlasticityModifier() const;
    double getDevelopmentalAge() const;
    
    // Configuration
    void enableDevelopment(bool enable);
    void enableStructuralPlasticity(bool enable);
    
    // Set brain reference
    void setBrain(Brain* brain) {
        brain_ = brain;
    }
    
private:
    void setStage(DevelopmentalStage stage, float modifier);
    void updateStructuralPlasticityRates();
    void reset();
    
    // State
    double developmentalAge_;
    DevelopmentalStage currentStage_;
    float plasticityModifier_;
    
    // Configuration
    bool enableDevelopment_;
    bool enableStructuralPlasticity_;
    
    // Dependencies
    Brain* brain_;
    
    friend class AgentBrain; // AgentBrain manages development
};

/**
 * @brief Handles curiosity-driven exploration behavior
 */
class ExplorationManager {
public:
    ExplorationManager();
    ~ExplorationManager() = default;
    
    // Disable copying, enable moving
    ExplorationManager(const ExplorationManager&) = delete;
    ExplorationManager& operator=(const ExplorationManager&) = delete;
    ExplorationManager(ExplorationManager&&) noexcept = default;
    ExplorationManager& operator=(ExplorationManager&&) noexcept = default;
    
    // Update exploration state
    void update(float curiosityLevel, RandomGenerator* rng);
    
    // Select exploratory action based on curiosity
    MotorCommand selectExploratoryAction(MotorCommand defaultCmd);
    
    // Configuration
    void setExplorationEnabled(bool enable);
    
    // Getters
    float getCuriosityLevel() const { return curiosityLevel_; }
    bool getExplorationEnabled() const { return enableExploration_; }
    
private:
    void reset();
    MotorCommand getRandomMotorCommand(int choice);
    
    float curiosityLevel_;
    bool enableExploration_;
    RandomGenerator* rng_;
    
    friend class AgentBrain; // AgentBrain manages exploration
};

// =============================================================================
// AGENT BRAIN IMPLEMENTATION
// =============================================================================

/**
 * @brief AgentBrain: Connects NLM brain to the world
 * Handles sensory transduction and motor decoding
 * 
 * This class has been restructured with modular components for better:
 * - Separation of concerns
 * - Testability
 * - Maintainability
 * - Extensibility
 */
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept = default;
    AgentBrain& operator=(AgentBrain&&) noexcept = default;
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { neuromodulation_->enableRewardModulation(enable); }
    void enableStructuralPlasticity(bool enable) { development_->enableStructuralPlasticity(enable); }
    void enableDevelopment(bool enable) { development_->enableDevelopment(enable); }
    void enableCuriosity(bool enable) { neuromodulation_->enableCuriosity(enable); }
    
    bool isRewardModulationEnabled() const { return neuromodulation_->enableRewardModulation(); }
    bool isStructuralPlasticityEnabled() const { return development_->enableStructuralPlasticity(); }
    bool isDevelopmentEnabled() const { return development_->enableDevelopment(); }
    bool isCuriosityEnabled() const { return neuromodulation_->enableCuriosity(); }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Initialize neuron groups using component managers
    void initializeNeuronGroups();
    void registerSensoryNeurons(const std::vector<Neuron*>& neurons);
    
    // Component managers
    std::unique_ptr<MotorGroupManager> motorGroupManager_;
    std::unique_ptr<SensoryProcessingManager> sensoryProcessor_;
    std::unique_ptr<NeuromodulationManager> neuromodulation_;
    std::unique_ptr<DevelopmentManager> development_;
    std::unique_ptr<ExplorationManager> exploration_;
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (legacy, kept for compatibility)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (legacy, kept for compatibility)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state (legacy, for compatibility)
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state (legacy, for compatibility)
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags (legacy, for compatibility)
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Helper methods for legacy compatibility
    std::unordered_map<std::string, std::vector<Neuron*>> getMotorGroups();
    std::vector<std::pair<MotorCommand, float>> getCommandActivities(
        const std::unordered_map<std::string, std::vector<Neuron*>>& groups);
};

} // namespace nlm
