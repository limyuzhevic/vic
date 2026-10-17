#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace nlm {

// Forward declarations for extensibility
class AgentBehavior;
class NeuralGroupManager;
class SensorProcessor;
class MotorDecoder;

// AgentBrain: Connects NLM brain to the world with improved extensibility
// Handles sensory transduction and motor decoding with pluggable components
class AgentBrain {
public:
    // Constructor with optional custom components for advanced users
    AgentBrain(std::shared_ptr<Brain> brain,
               std::unique_ptr<NeuralGroupManager> groupManager = nullptr,
               std::unique_ptr<SensorProcessor> sensorProcessor = nullptr,
               std::unique_ptr<MotorDecoder> motorDecoder = nullptr);
    
    virtual ~AgentBrain();
    
    // Initialize with world and custom behaviors
    void initialize(const SimpleWorld& world, 
                   const std::vector<std::unique_ptr<AgentBehavior>>& behaviors = {});
    
    // Get sensory input size expected by brain (improved interface)
    [[nodiscard]] size_t getSensoryInputSize() const noexcept;
    
    // Get motor output size expected (improved interface)
    [[nodiscard]] size_t getMotorOutputSize() const noexcept;
    
    // Process sensory percept with advanced processing options
    void processSensoryInput(const SensoryPercept& percept,
                           const std::unordered_map<std::string, float>& processingOptions = {});
    
    // Decode brain motor activity into motor command with custom strategies
    MotorCommand decodeMotorCommand(const std::string& strategy = "default");
    
    // Apply neuromodulation with advanced control options
    void applyRewardModulation(float reward, float predictedReward,
                              const std::unordered_map<std::string, float>& modulationParams = {});
    
    // Update development system with extended parameters
    void updateDevelopment(double timestep, 
                          const std::unordered_map<std::string, float>& devParams = {});
    
    // Get current developmental stage
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level with customizable metrics
    [[nodiscard]] float getNeuromodulationLevel(const std::string& metric = "dopamine") const noexcept;
    
    // Get curiosity level with enhanced tracking
    [[nodiscard]] float getCuriosityLevel() const noexcept;
    
    // Get novelty level with adaptive sensitivity
    [[nodiscard]] float getNoveltyLevel() const noexcept;
    
    // Get prediction error with confidence bounds
    [[nodiscard]] float getPredictionError() const noexcept;
    
    // Reset agent for new episode with full state management
    void reset(const std::string& resetMode = "normal");
    
    // Get brain pointer (improved safety)
    [[nodiscard]] Brain* getBrain() const noexcept { return brain_.get(); }
    
    // Configuration with enhanced options
    void enableRewardModulation(bool enable, float gain = 1.0f) { 
        rewardModulationEnabled_ = enable; 
        rewardModulationGain_ = gain;
    }
    void enableStructuralPlasticity(bool enable, float rate = 1.0f) { 
        structuralPlasticityEnabled_ = enable; 
        structuralPlasticityRate_ = rate;
    }
    void enableDevelopment(bool enable, float speed = 1.0f) { 
        developmentEnabled_ = enable; 
        developmentSpeed_ = speed;
    }
    void enableCuriosity(bool enable, float threshold = 0.3f) { 
        curiosityEnabled_ = enable; 
        curiosityThreshold_ = threshold;
    }
    
    // Advanced configuration methods
    void setNeuromodulationSensitivity(float sensitivity) { neuromodulationSensitivity_ = sensitivity; }
    void setPlasticityThreshold(float threshold) { plasticityThreshold_ = threshold; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    
    // Component access for advanced users (improved encapsulation)
    [[nodiscard]] const NeuralGroupManager& getNeuralGroupManager() const { 
        return *groupManager_; 
    }
    [[nodiscard]] const SensorProcessor& getSensorProcessor() const { 
        return *sensorProcessor_; 
    }
    [[nodiscard]] const MotorDecoder& getMotorDecoder() const { 
        return *motorDecoder_; 
    }
    
    // Behavior management
    void addBehavior(std::unique_ptr<AgentBehavior> behavior);
    void removeBehavior(const std::string& behaviorName);
    void updateBehaviors(double timestep);
    
    // Advanced statistics and monitoring
    [[nodiscard]] std::unordered_map<std::string, float> getAdvancedStatistics() const;
    [[nodiscard]] bool isHealthy() const;
    
private:
    // Core brain connection
    std::shared_ptr<Brain> brain_;
    
    // Pluggable component system (improved architecture)
    std::unique_ptr<NeuralGroupManager> groupManager_;
    std::unique_ptr<SensorProcessor> sensorProcessor_;
    std::unique_ptr<MotorDecoder> motorDecoder_;
    
    // Advanced neural group management with better categorization
    std::vector<Neuron*> motorGroups_[6];  // Forward, Backward, Left, Right, Interact, Wait
    std::vector<Neuron*> sensoryGroups_[4];  // Vision, Touch, Internal, Proprioception
    
    // Enhanced neuromodulation state with multiple metrics
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float neuromodulationSensitivity_;
    
    // Development state with extended parameters
    double developmentalAge_;
    float plasticityModifier_;
    float developmentSpeed_;
    
    // Configuration flags with advanced parameters
    bool rewardModulationEnabled_;
    float rewardModulationGain_;
    bool structuralPlasticityEnabled_;
    float structuralPlasticityRate_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float curiosityThreshold_;
    float plasticityThreshold_;
    float learningRate_;
    
    // Previous sensory state for novelty detection (improved)
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    float adaptiveNoveltySensitivity_;
    
    // Custom behaviors for extensibility
    std::vector<std::unique_ptr<AgentBehavior>> behaviors_;
    
    // Monitoring and health tracking
    double lastUpdateTime_;
    uint64_t processingCycles_;
    mutable float healthScore_;
    
    // Private helper methods
    void initializeNeuralGroups();
    void updateSensorGroups();
    void updateMotorGroups();
    void calculateHealthScore() const;
    
    // Enhanced motor decoding strategies
    MotorCommand decodeFromMotorNeuronsAdvanced(const std::string& strategy);
    MotorCommand selectWithCuriosityAdvanced(MotorCommand defaultCmd, const std::string& strategy);
};

// Abstract base class for agent behaviors (plugin system foundation)
class AgentBehavior {
public:
    virtual ~AgentBehavior() = default;
    
    // Behavior name for identification
    [[nodiscard]] virtual std::string getName() const = 0;
    
    // Initialize behavior with brain access
    virtual void initialize(std::shared_ptr<Brain> brain) = 0;
    
    // Update behavior with timestep
    virtual void update(double timestep) = 0;
    
    // Process sensory input
    virtual void processSensoryInput(const SensoryPercept& percept) = 0;
    
    // Generate motor output
    virtual MotorCommand generateMotorOutput() = 0;
    
    // Handle reward signals
    virtual void handleReward(float reward, float predictedReward) = 0;
    
    // Reset behavior state
    virtual void reset() = 0;
    
    // Get behavior-specific statistics
    virtual std::unordered_map<std::string, float> getStatistics() const = 0;
};

// Neural group manager for advanced neural population control
class NeuralGroupManager {
public:
    virtual ~NeuralGroupManager() = default;
    
    // Organize neurons into functional groups
    virtual void organizeNeuralGroups(std::shared_ptr<Brain> brain) = 0;
    
    // Get neuron groups by type
    virtual const std::vector<Neuron*>& getMotorNeurons(NeuronType type) const = 0;
    virtual const std::vector<Neuron*>& getSensoryNeurons(NeuronType type) const = 0;
    
    // Get statistics about neural organization
    virtual std::unordered_map<std::string, float> getGroupStatistics() const = 0;
};

// Default implementation of neural group manager
class DefaultNeuralGroupManager : public NeuralGroupManager {
public:
    explicit DefaultNeuralGroupManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void organizeNeuralGroups(std::shared_ptr<Brain> brain) override;
    const std::vector<Neuron*>& getMotorNeurons(NeuronType type) const override;
    const std::vector<Neuron*>& getSensoryNeurons(NeuronType type) const override;
    std::unordered_map<std::string, float> getGroupStatistics() const override;
    
private:
    std::shared_ptr<Brain> brain_;
    std::unordered_map<NeuronType, std::vector<Neuron*>> motorGroups_;
    std::unordered_map<NeuronType, std::vector<Neuron*>> sensoryGroups_;
};

// Sensor processor for advanced sensory processing
class SensorProcessor {
public:
    virtual ~SensorProcessor() = default;
    
    // Process raw sensory data
    virtual std::vector<float> processVision(const std::vector<float>& rawVision) = 0;
    virtual std::vector<float> processTouch(const std::vector<float>& rawTouch) = 0;
    virtual std::vector<float> processInternal(const std::vector<float>& rawInternal) = 0;
    virtual std::vector<float> processProprioception(const std::vector<float>& rawProprioception) = 0;
    
    // Compute novelty detection
    virtual float computeNovelty(const std::vector<float>& current, const std::vector<float>& previous) = 0;
    
    // Get processing statistics
    virtual std::unordered_map<std::string, float> getProcessingStatistics() const = 0;
};

// Default sensor processor implementation
class DefaultSensorProcessor : public SensorProcessor {
public:
    std::vector<float> processVision(const std::vector<float>& rawVision) override;
    std::vector<float> processTouch(const std::vector<float>& rawTouch) override;
    std::vector<float> processInternal(const std::vector<float>& rawInternal) override;
    std::vector<float> processProprioception(const std::vector<float>& rawProprioception) override;
    
    float computeNovelty(const std::vector<float>& current, const std::vector<float>& previous) override;
    std::unordered_map<std::string, float> getProcessingStatistics() const override;
    
private:
    mutable std::unordered_map<std::string, float> statistics_;
};

// Motor decoder for advanced motor command generation
class MotorDecoder {
public:
    virtual ~MotorDecoder() = default;
    
    // Decode neural activity to motor commands
    virtual MotorCommand decode(const std::vector<Neuron*>& motorNeurons, 
                              const std::string& strategy = "default") = 0;
    
    // Get decoding statistics
    virtual std::unordered_map<std::string, float> getDecodingStatistics() const = 0;
};

// Default motor decoder implementation
class DefaultMotorDecoder : public MotorDecoder {
public:
    MotorCommand decode(const std::vector<Neuron*>& motorNeurons, 
                      const std::string& strategy = "default") override;
    std::unordered_map<std::string, float> getDecodingStatistics() const override;
    
private:
    mutable std::unordered_map<std::string, float> statistics_;
};

} // namespace nlm
