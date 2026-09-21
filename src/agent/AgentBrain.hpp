#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <string>

namespace nlm {

// Agent configuration system
struct AgentConfig {
    // Neuron distribution ratios (0.0-1.0)
    struct NeuronDistribution {
        float motorForward = 0.20f;      // Move forward
        float motorBackward = 0.20f;     // Move backward
        float motorTurnLeft = 0.15f;     // Turn left
        float motorTurnRight = 0.15f;    // Turn right
        float motorInteract = 0.15f;     // Interact
        float motorWait = 0.15f;         // Wait
        
        // Sensory distribution
        float sensoryVision = 0.60f;     // Vision processing
        float sensoryTouch = 0.20f;      // Touch sensing
        float sensoryInternal = 0.10f;   // Internal signals
        float sensoryProprioception = 0.10f; // Body position
    };
    
    NeuronDistribution distribution;
    
    // Sensory configuration
    struct SensoryConfig {
        size_t visionWidth = 16;
        size_t visionHeight = 16;
        size_t touchPoints = 8;
        size_t internalSignals = 4;
        size_t proprioceptionPoints = 6;
        bool enableAudio = false;
        size_t audioChannels = 0;
        
        // Signal processing
        float visionScale = 5.0f;
        float touchScale = 8.0f;
        float internalScale = 5.0f;
        float proprioceptionScale = 3.0f;
        
        // Normalization
        bool normalizeVision = true;
        bool normalizeTouch = true;
        bool normalizeInternal = true;
        bool normalizeProprioception = true;
    } sensory;
    
    // Neuromodulation parameters
    struct NeuromodulationConfig {
        float noveltyDecayRate = 0.99f;
        float curiosityBaseLevel = 0.1f;
        float dopamineClampMin = -1.0f;
        float dopamineClampMax = 1.0f;
        float curiosityThreshold = 0.3f;
        float explorationChance = 0.3f;
        float curiosityAmplification = 2.0f;
        float errorAmplification = 0.5f;
    } neuromodulation;
    
    // Development parameters
    struct DevelopmentConfig {
        double initialAge = 0.0;
        double growthRate = 1.0;
        double criticalPeriodEnd = 300.0; // 5 minutes
        double maturationEnd = 900.0;     // 15 minutes
        float plasticityInitial = 1.0f;
        float plasticityCritical = 0.8f;
        float plasticityMature = 0.5f;
        float plasticityAdult = 0.2f;
    } development;
    
    // Sampling and monitoring
    struct SamplingConfig {
        bool enableNeuralSampling = false;
        size_t sampleEveryNSteps = 100;
        size_t maxSampleHistory = 1000;
        bool recordSpikes = true;
        bool recordWeights = true;
        bool recordActivity = true;
        
        // Performance monitoring
        bool enablePerformanceMonitoring = true;
        double monitorInterval = 1.0;
        size_t maxPerformanceHistory = 100;
    } sampling;
    
    // File and serialization
    struct FileConfig {
        std::string defaultConfigFile = "configs/agent_config.json";
        std::string checkpointFile = "checkpoints/agent_checkpoint.bin";
        std::string sampleOutputFile = "samples/agent_samples.dat";
        bool saveSamples = false;
        bool saveCheckpoints = true;
    } file;
    
    // Flags
    bool useDistribution = true;        // Use config-driven distribution
    bool useSensoryConfig = true;       // Use sensory configuration
    bool useNeuromodulationConfig = true;
    bool useDevelopmentConfig = true;
    bool useSamplingConfig = true;
    
    // Helper methods
    static AgentConfig createDefaultConfig() {
        AgentConfig config;
        
        // Set default values based on original hardcoded values
        config.sensory = SensoryConfig{
            16, 16, 8, 4, 6, false, 0, 5.0f, 8.0f, 5.0f, 3.0f, true, true, true, true
        };
        
        config.neuromodulation = NeuromodulationConfig{
            0.99f, 0.1f, -1.0f, 1.0f, 0.3f, 0.3f, 2.0f, 0.5f
        };
        
        config.development = DevelopmentConfig{
            0.0, 1.0, 300.0, 900.0, 1.0f, 0.8f, 0.5f, 0.2f
        };
        
        return config;
    }
    
    float getTotalMotorRatio() const {
        return distribution.motorForward + distribution.motorBackward +
               distribution.motorTurnLeft + distribution.motorTurnRight +
               distribution.motorInteract + distribution.motorWait;
    }
    
    float getTotalSensoryRatio() const {
        return distribution.sensoryVision + distribution.sensoryTouch +
               distribution.sensoryInternal + distribution.sensoryProprioception;
    }
    
    void validate() const {
        // Validate ratios sum to 1.0 (within tolerance)
        float motorTotal = getTotalMotorRatio();
        float sensoryTotal = getTotalSensoryRatio();
        
        const float tolerance = 0.01f;
        if (std::abs(motorTotal - 1.0f) > tolerance) {
            // Normalize motor ratios
            float scale = 1.0f / motorTotal;
            // This would adjust ratios - simplified for now
        }
        
        if (std::abs(sensoryTotal - 1.0f) > tolerance) {
            // Normalize sensory ratios
            float scale = 1.0f / sensoryTotal;
            // This would adjust ratios - simplified for now
        }
    }
};

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config = AgentConfig::createDefaultConfig());
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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Get config file path for loading brain state
    std::string getConfigFilePath() const { return configFilePath_; }
    void setConfigFilePath(const std::string& path) { configFilePath_ = path; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Neuron group distribution configuration
    void setMotorDistributionConfig(const std::vector<float>& distribution);
    void setSensoryDistributionConfig(const std::vector<float>& distribution);
    std::vector<float> getMotorDistributionConfig() const { return motorDistribution_; }
    std::vector<float> getSensoryDistributionConfig() const { return sensoryDistribution_; }
    
    // New methods for enhanced functionality
    const AgentConfig& getConfig() const { return config_; }
    void updateConfig(const AgentConfig& newConfig) { 
        config_ = newConfig; 
        config_.validate();
    }
    
    // Sampling methods
    void enableSampling(bool enable) { samplingEnabled_ = enable; }
    bool isSamplingEnabled() const { return samplingEnabled_; }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double timeStep = 0.0;
        float cpuUsage = 0.0f;
        float memoryUsage = 0.0f;
        size_t spikeCount = 0;
        float averageActivity = 0.0f;
    };
    
    PerformanceMetrics getPerformanceMetrics() const { return currentMetrics_; }
    
    // New methods for enhanced functionality
    const AgentConfig& getConfig() const { return config_; }
    void updateConfig(const AgentConfig& newConfig) { 
        config_ = newConfig; 
        config_.validate();
    }
    
    // Sampling methods
    void enableSampling(bool enable) { samplingEnabled_ = enable; }
    bool isSamplingEnabled() const { return samplingEnabled_; }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double timeStep = 0.0;
        float cpuUsage = 0.0f;
        float memoryUsage = 0.0f;
        size_t spikeCount = 0;
        float averageActivity = 0.0f;
    };
    
    PerformanceMetrics getPerformanceMetrics() const { return currentMetrics_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced initialization with configurable distribution
    void initializeNeuronDistribution();
    
    // Enhanced sensory processing
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    void processAudioInput(const std::vector<float>& audio);
    
    // Novelty and curiosity computation
    void computeNoveltyAndCuriosity(const std::vector<float>& vision);
    
    // Sampling and monitoring
    void recordSample(const std::string& type, const std::vector<float>& data);
    void updatePerformanceMetrics();
    
    std::shared_ptr<Brain> brain_;
    AgentConfig config_;
    
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
    std::vector<Neuron*> sensoryAudio_;
    
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
    bool samplingEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Sampling history
    std::vector<std::pair<std::string, std::vector<float>>> sampleHistory_;
    size_t sampleStepCount_;
    
    // Performance metrics
    PerformanceMetrics currentMetrics_;
    double lastPerformanceUpdate_;
    
    // Configuration file path support
    std::string configFilePath_;
    bool useConfigFile_;
};

} // namespace nlm
