#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
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
    
    // ========== ENHANCED AGENT BRAIN API ==========
    
    // Configuration method chaining
    class ConfigurationBuilder {
    public:
        ConfigurationBuilder(AgentBrain* agentBrain);
        
        // Chain configuration methods
        ConfigurationBuilder& setRewardModulation(bool enable);
        ConfigurationBuilder& setStructuralPlasticity(bool enable);
        ConfigurationBuilder& setDevelopment(bool enable);
        ConfigurationBuilder& setCuriosity(bool enable);
        ConfigurationBuilder& setNeuromodulationLevel(float level);
        ConfigurationBuilder& setCuriosityLevel(float level);
        ConfigurationBuilder& setNoveltyLevel(float level);
        ConfigurationBuilder& setPredictionError(float error);
        
        // Apply all accumulated configuration
        void apply();
        
        // Reset builder state
        void reset();
        
        // Get current configuration state
        bool isRewardModulationEnabled() const;
        bool isStructuralPlasticityEnabled() const;
        bool isDevelopmentEnabled() const;
        bool isCuriosityEnabled() const;
        
    private:
        AgentBrain* agentBrain_;
        bool rewardModulationEnabled_;
        bool structuralPlasticityEnabled_;
        bool developmentEnabled_;
        bool curiosityEnabled_;
        float neuromodulationLevel_;
        float curiosityLevel_;
        float noveltyLevel_;
        float predictionError_;
    };
    
    // Create configuration builder for method chaining
    ConfigurationBuilder configure();
    
    // Subscription-based neuromodulation
    class NeuromodulationSubscription {
    public:
        NeuromodulationSubscription(AgentBrain* agentBrain, std::function<void(float)> callback);
        ~NeuromodulationSubscription();
        
        // Check if subscription is active
        bool isActive() const;
        
        // Get neuromodulation level that triggered callback
        float getLastTriggerLevel() const;
        
    private:
        AgentBrain* agentBrain_;
        std::function<void(float)> callback_;
        bool active_;
        float lastTriggerLevel_;
    };
    
    // Subscribe to neuromodulation events
    NeuromodulationSubscription subscribeToNeuromodulation(std::function<void(float)> callback);
    
    // Convenience methods for common agent operations
    void setupForControlTask(float targetReward = 1.0f, float explorationBonus = 0.1f);
    void setupForExplorationTask(float noveltyThreshold = 0.5f, float curiosityFactor = 1.0f);
    void setupForMemoryTask(float memoryCapacity = 1000.0f, float consolidationRate = 0.01f);
    
    // Experimental and research mode helpers
    bool enableExperimentalMode();
    bool enableResearchMode();
    bool disableAllModulators();
    
    // Debugging and profiling methods
    void startProfiler();
    void stopProfiler();
    bool isProfilerActive() const;
    std::string getProfilerReport() const;
    
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
    void enableLoggingToFile(const std::string& filepath);
    void disableLoggingToFile();
    bool isLoggingToFile() const;
    
    // Performance monitoring
    double getSimulationSpeed() const;
    double getAverageFiringRatePerStep() const;
    size_t getMemoryFootprint() const;
    
    // Statistical analysis
    struct StatisticalSummary {
        double mean;
        double variance;
        double stddev;
        double min;
        double max;
        size_t count;
        
        StatisticalSummary() : mean(0.0), variance(0.0), stddev(0.0), min(0.0), max(0.0), count(0) {}
    };
    
    StatisticalSummary computeStatisticalSummary(const std::vector<float>& data) const;
    StatisticalSummary computeMotorOutputStatistics() const;
    StatisticalSummary computeSensoryInputStatistics() const;
    
    // Batch operations
    void applyBatchNeuromodulation(const std::vector<std::pair<std::string, float>>& neuromodulators);
    void updateBatchDevelopment(double timestep);
    
    // State management
    void saveState(const std::string& filepath);
    bool loadState(const std::string& filepath);
    
    // Configuration validation
    bool validateConfiguration(std::vector<std::string>& errors) const;
    bool isConfigurationValid() const;
    
    // Export and visualization helpers
    std::string generateActivityReport() const;
    std::string generateConnectivityReport() const;
    
    // Simulation state management
    void pauseSimulation();
    void resumeSimulation();
    bool isSimulationPaused() const;
    
    // Checkpoint and recovery
    bool createCheckpoint(const std::string& prefix = "checkpoint");
    bool restoreFromCheckpoint(const std::string& filepath);
    std::vector<std::string> getAvailableCheckpoints() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
