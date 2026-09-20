#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // New methods for enhanced functionality
    
    // Get expected number of motor groups based on configuration
    size_t getOptimalMotorGroupCount() const;
    
    // Get current neuromodulation gain
    float getNeuromodulationGain() const { return neuromodulationGain_; }
    
    // Set neuromodulation gain
    void setNeuromodulationGain(float gain) { neuromodulationGain_ = gain; }
    
    // Get exploration rate
    float getExplorationRate() const { return explorationRate_; }
    
    // Set exploration rate
    void setExplorationRate(float rate) { explorationRate_ = std::clamp(rate, 0.0f, 1.0f); }
    
    // Get maximum curiosity threshold
    float getMaxCuriosityThreshold() const { return maxCuriosityThreshold_; }
    
    // Set maximum curiosity threshold
    void setMaxCuriosityThreshold(float threshold) { maxCuriosityThreshold_ = std::clamp(threshold, 0.0f, 1.0f); }
    
    // Get vision and height for coordinate weighting
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Set vision dimensions
    void setVisionDimensions(size_t width, size_t height) {
        visionWidth_ = width;
        visionHeight_ = height;
    }
    
    // Enable/disable input validation
    void enableValidation(bool enable) { validationEnabled_ = enable; }
    bool isValidationEnabled() const { return validationEnabled_; }
    
    // Debug and monitoring methods
    void checkState() const;
    size_t getErrorCount() const;
    void resetErrorCount();
    
    // Performance monitoring
    std::string getPerformanceStats() const;
    double getLastUpdateTime() const;
    size_t getStepCount() const;
    
    // Debug information
    std::string getDebugInfo() const;
    void logStatus() const;
    
private:
    // Core brain reference
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups - enhanced with look commands
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
    
    // Sensory neuron groups - enhanced with audio
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // New configuration parameters
    float maxCuriosityThreshold_;
    float explorationRate_;
    float neuromodulationGain_;
    bool validationEnabled_;
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Error tracking
    size_t errorCount_;
    double lastUpdateTime_;
    size_t stepCount_;
    
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced processing methods
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& intern);
    void processProprioceptionInput(const std::vector<float>& proprio);
    void processAudioInput(const std::vector<float>& audio);
    
    // Enhanced novelty computation
    void computeEnhancedNovelty(const std::vector<float>& vision);
    
    // Enhanced curiosity update
    void updateCuriosityWithPredictionError();
    
    // Input validation
    void validateSensoryInput(const SensoryPercept& percept) const;
};

} // namespace nlm
