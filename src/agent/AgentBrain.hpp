pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>

namespace nlm {

// Advanced AgentBrain configuration
class AgentBrainConfig {
public:
    // Core settings
    size_t sensoryVisionWidth = 16;
    size_t sensoryVisionHeight = 16;
    size_t sensoryTouchCount = 8;
    size_t sensoryInternalCount = 4;
    size_t sensoryProprioceptionCount = 6;
    
    // Motor settings
    bool enableMotorForward = true;
    bool enableMotorBackward = true;
    bool enableMotorTurnLeft = true;
    bool enableMotorTurnRight = true;
    bool enableMotorInteract = true;
    bool enableMotorWait = true;
    
    // Neuromodulation settings
    bool enableRewardModulation = true;
    bool enableCuriosity = true;
    bool enableNoveltyDetection = true;
    float curiosityThreshold = 0.3f;
    float explorationRate = 0.15f;
    
    // Development settings
    bool enableDevelopment = true;
    bool enableStructuralPlasticity = true;
    bool enableSynaptogenesis = true;
    bool enablePruning = true;
    bool enableMaturation = true;
    double developmentDuration = 900.0; // ~15 minutes
    
    // Learning and plasticity
    float plasticityBase = 1.0f;
    float plasticityBoost = 1.5f;
    float plasticityDecay = 0.95f;
    float rewardDecay = 0.95f;
    float eligibilityDecay = 0.1f;
    
    // Sensory processing
    float visionScaleFactor = 5.0f;
    float touchScaleFactor = 8.0f;
    float internalScaleFactor = 5.0f;
    float proprioceptionScaleFactor = 3.0f;
    float noveltyDecayFactor = 0.99f;
    
    // Performance and optimization
    bool enablePerformanceOptimizations = true;
    bool enableVectorization = true;
    bool enableParallelProcessing = false;
    size_t neuralSimulationSteps = 10;
    
    // Advanced features
    bool enablePredictionIntegration = true;
    bool enableMemoryIntegration = true;
    bool enableCognitionIntegration = true;
    float predictionWeight = 0.5f;
    float memoryWeight = 0.5f;
    float cognitionWeight = 0.5f;
    
    // Logging and debugging
    bool enableDebugLogging = false;
    bool enablePerformanceMetrics = false;
    bool enableStepByStepLogging = false;
    std::string logFile = "agentbrain.log";
    
    // Validation and safety
    float minActivityThreshold = 0.1f;
    float maxActivityThreshold = 10.0f;
    float maxEnergyConsumption = 1.0f;
    float maxHealthDecay = 0.01f;
    
    // Custom neuron group distributions (advanced users)
    std::unordered_map<std::string, float> neuronGroupWeights;
    std::unordered_map<std::string, float> sensoryGroupWeights;
    
    AgentBrainConfig() {
        // Initialize default neuron group weights
        neuronGroupWeights["forward"] = 0.2f;
        neuronGroupWeights["backward"] = 0.2f;
        neuronGroupWeights["left"] = 0.2f;
        neuronGroupWeights["right"] = 0.2f;
        neuronGroupWeights["interact"] = 0.1f;
        neuronGroupWeights["wait"] = 0.3f;
        
        neuronGroupWeights["vision"] = 0.4f;
        neuronGroupWeights["touch"] = 0.3f;
        neuronGroupWeights["internal"] = 0.2f;
        neuronGroupWeights["proprioception"] = 0.1f;
    }
    
    // Load configuration from Config object
    void loadFromConfig(const Config& config) {
        if (config.hasKey("sensory_vision_width")) {
            sensoryVisionWidth = config.get<size_t>("sensory_vision_width");
        }
        if (config.hasKey("sensory_vision_height")) {
            sensoryVisionHeight = config.get<size_t>("sensory_vision_height");
        }
        if (config.hasKey("sensory_touch_count")) {
            sensoryTouchCount = config.get<size_t>("sensory_touch_count");
        }
        if (config.hasKey("sensory_internal_count")) {
            sensoryInternalCount = config.get<size_t>("sensory_internal_count");
        }
        if (config.hasKey("sensory_proprioception_count")) {
            sensoryProprioceptionCount = config.get<size_t>("sensory_proprioception_count");
        }
        
        // Motor settings
        if (config.hasKey("motor_forward_enabled")) {
            enableMotorForward = config.get<bool>("motor_forward_enabled");
        }
        if (config.hasKey("motor_backward_enabled")) {
            enableMotorBackward = config.get<bool>("motor_backward_enabled");
        }
        if (config.hasKey("motor_turn_left_enabled")) {
            enableMotorTurnLeft = config.get<bool>("motor_turn_left_enabled");
        }
        if (config.hasKey("motor_turn_right_enabled")) {
            enableMotorTurnRight = config.get<bool>("motor_turn_right_enabled");
        }
        if (config.hasKey("motor_interact_enabled")) {
            enableMotorInteract = config.get<bool>("motor_interact_enabled");
        }
        if (config.hasKey("motor_wait_enabled")) {
            enableMotorWait = config.get<bool>("motor_wait_enabled");
        }
        
        // Neuromodulation
        if (config.hasKey("reward_modulation_enabled")) {
            enableRewardModulation = config.get<bool>("reward_modulation_enabled");
        }
        if (config.hasKey("curiosity_enabled")) {
            enableCuriosity = config.get<bool>("curiosity_enabled");
        }
        if (config.hasKey("curiosity_threshold")) {
            curiosityThreshold = config.get<float>("curiosity_threshold");
        }
        if (config.hasKey("exploration_rate")) {
            explorationRate = config.get<float>("exploration_rate");
        }
        
        // Development
        if (config.hasKey("development_enabled")) {
            enableDevelopment = config.get<bool>("development_enabled");
        }
        if (config.hasKey("structural_plasticity_enabled")) {
            enableStructuralPlasticity = config.get<bool>("structural_plasticity_enabled");
        }
        if (config.hasKey("development_duration")) {
            developmentDuration = config.get<double>("development_duration");
        }
        
        // Performance
        if (config.hasKey("neural_simulation_steps")) {
            neuralSimulationSteps = config.get<size_t>("neural_simulation_steps");
        }
        
        // Advanced features
        if (config.hasKey("prediction_integration_enabled")) {
            enablePredictionIntegration = config.get<bool>("prediction_integration_enabled");
        }
        if (config.hasKey("memory_integration_enabled")) {
            enableMemoryIntegration = config.get<bool>("memory_integration_enabled");
        }
        if (config.hasKey("cognition_integration_enabled")) {
            enableCognitionIntegration = config.get<bool>("cognition_integration_enabled");
        }
    }
    
    // Validate configuration
    bool validate() const {
        if (sensoryVisionWidth == 0 || sensoryVisionHeight == 0) {
            std::cerr << "Error: Vision dimensions must be positive" << std::endl;
            return false;
        }
        
        if (sensoryTouchCount == 0 || sensoryInternalCount == 0 || sensoryProprioceptionCount == 0) {
            std::cerr << "Error: Sensory counts must be positive" << std::endl;
            return false;
        }
        
        if (curiosityThreshold < 0.0f || curiosityThreshold > 1.0f) {
            std::cerr << "Error: Curiosity threshold must be between 0.0 and 1.0" << std::endl;
            return false;
        }
        
        if (explorationRate < 0.0f || explorationRate > 1.0f) {
            std::cerr << "Error: Exploration rate must be between 0.0 and 1.0" << std::endl;
            return false;
        }
        
        if (developmentDuration <= 0.0) {
            std::cerr << "Error: Development duration must be positive" << std::endl;
            return false;
        }
        
        if (neuralSimulationSteps == 0) {
            std::cerr << "Error: Neural simulation steps must be positive" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Get configuration as string (for debugging)
    std::string toString() const {
        std::stringstream ss;
        ss << "=== AgentBrain Configuration ===\n";
        ss << "Vision: " << sensoryVisionWidth << "x" << sensoryVisionHeight << "\n";
        ss << "Touch: " << sensoryTouchCount << " sensors\n";
        ss << "Internal: " << sensoryInternalCount << " signals\n";
        ss << "Proprioception: " << sensoryProprioceptionCount << " signals\n";
        ss << "\nMotor Groups: ";
        ss << (enableMotorForward ? "F " : ""); ss << (enableMotorBackward ? "B " : "");
        ss << (enableMotorTurnLeft ? "L " : ""); ss << (enableMotorTurnRight ? "R " : "");
        ss << (enableMotorInteract ? "I " : ""); ss << (enableMotorWait ? "W " : "");
        ss << "\n\nNeuromodulation:\n";
        ss << "  Reward modulation: " << (enableRewardModulation ? "ON" : "OFF") << "\n";
        ss << "  Curiosity: " << (enableCuriosity ? "ON" : "OFF") << "\n";
        ss << "  Threshold: " << curiosityThreshold << "\n";
        ss << "  Exploration: " << explorationRate << "\n";
        ss << "\nDevelopment:\n";
        ss << "  Enabled: " << (enableDevelopment ? "YES" : "NO") << "\n";
        ss << "  Structural plasticity: " << (enableStructuralPlasticity ? "YES" : "NO") << "\n";
        ss << "  Duration: " << developmentDuration << " units\n";
        ss << "\nPerformance:\n";
        ss << "  Neural steps: " << neuralSimulationSteps << "\n";
        ss << "  Vectorization: " << (enableVectorization ? "YES" : "NO") << "\n";
        ss << "\nAdvanced features:\n";
        ss << "  Prediction integration: " << (enablePredictionIntegration ? "YES" : "NO") << "\n";
        ss << "  Memory integration: " << (enableMemoryIntegration ? "YES" : "NO") << "\n";
        ss << "  Cognition integration: " << (enableCognitionIntegration ? "YES" : "NO") << "\n";
        
        return ss.str();
    }
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with advanced configuration
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config);
    ~AgentBrain();
    
    // Initialize with world and configuration
    void initialize(const SimpleWorld& world);
    void initialize(const SimpleWorld& world, const AgentBrainConfig& config);
    
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
    
    // Configuration methods
    const AgentBrainConfig& getConfig() const { return config_; }
    void updateConfig(const AgentBrainConfig& newConfig);
    void setConfigValue(const std::string& key, const std::string& value);
    
    // Enable/disable features
    void enableRewardModulation(bool enable) { config_.enableRewardModulation = enable; }
    void enableCuriosity(bool enable) { config_.enableCuriosity = enable; }
    void enableDevelopment(bool enable) { config_.enableDevelopment = enable; }
    void enableStructuralPlasticity(bool enable) { config_.enableStructuralPlasticity = enable; }
    void enablePredictionIntegration(bool enable) { config_.enablePredictionIntegration = enable; }
    void enableMemoryIntegration(bool enable) { config_.enableMemoryIntegration = enable; }
    void enableCognitionIntegration(bool enable) { config_.enableCognitionIntegration = enable; }
    
    bool isRewardModulationEnabled() const { return config_.enableRewardModulation; }
    bool isCuriosityEnabled() const { return config_.enableCuriosity; }
    bool isDevelopmentEnabled() const { return config_.enableDevelopment; }
    bool isStructuralPlasticityEnabled() const { return config_.enableStructuralPlasticity; }
    bool isPredictionIntegrationEnabled() const { return config_.enablePredictionIntegration; }
    bool isMemoryIntegrationEnabled() const { return config_.enableMemoryIntegration; }
    bool isCognitionIntegrationEnabled() const { return config_.enableCognitionIntegration; }
    
    // Performance metrics
    void enableDebugLogging(bool enable) { config_.enableDebugLogging = enable; }
    void enablePerformanceMetrics(bool enable) { config_.enablePerformanceMetrics = enable; }
    void enableStepByStepLogging(bool enable) { config_.enableStepByStepLogging = enable; }
    
    // Get performance metrics
    float getAverageFiringRate() const;
    size_t getTotalNeuronsProcessed() const { return totalNeuronsProcessed_; }
    size_t getTotalStepsProcessed() const { return totalStepsProcessed_; }
    double getTotalProcessingTime() const { return totalProcessingTime_; }
    
    // Advanced motor command selection
    MotorCommand decodeMotorCommandWithAdvancedOptions() const;
    MotorCommand decodeMotorCommandWithPrediction() const;
    MotorCommand decodeMotorCommandWithMemory() const;
    MotorCommand decodeMotorCommandWithCognition() const;
    
    // Environment interaction
    float calculateEnergyCost(const MotorCommand& cmd) const;
    float calculateHealthDecay(float energy) const;
    bool validateAction(const MotorCommand& cmd) const;
    
    // Development stage calculations
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    std::string getDevelopmentalStageString() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons() const;
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd) const;
    
    // Advanced decoding with system integrations
    MotorCommand decodeWithPrediction(const std::vector<float>& neuralActivity) const;
    MotorCommand decodeWithMemory(const std::vector<float>& neuralActivity) const;
    MotorCommand decodeWithCognition(const std::vector<float>& neuralActivity) const;
    
    // Configuration
    AgentBrainConfig config_;
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance metrics
    mutable size_t totalNeuronsProcessed_;
    mutable size_t totalStepsProcessed_;
    mutable double totalProcessingTime_;
    mutable double lastStepTime_;
    
    // Initialize neuron groups based on brain regions and config
    void initializeNeuronGroups();
    
    // Distribute neurons to groups based on weights
    void distributeNeurons(const std::string& type, const std::vector<Neuron*>& neurons);
    
    // Calculate group weights with configurable biases
    float getGroupWeight(const std::string& group) const;
    
    // Advanced activity calculation with integration
    float calculateActivityWithIntegration(const std::vector<Neuron*>& neurons) const;
    
    // Validation and safety checks
    bool validateNeuralActivity(float activity) const;
    bool validateMotorCommand(const MotorCommand& cmd) const;
    
    // Debug and logging
    void logDebugInfo(const std::string& message) const;
    void logPerformanceMetrics() const;
};
