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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Public constants and helper methods
    // Vision and sensory input sizes
    static constexpr size_t VISION_PIXELS = 256;          // 16x16 vision grid
    static constexpr size_t TOUCH_SENSORS = 8;           // Touch/proximity sensors
    static constexpr size_t INTERNAL_SENSORS = 4;        // Internal homeostatic signals
    static constexpr size_t PROPRIOCEPTION_SENSORS = 6;   // Body position/velocity signals
    static constexpr size_t MOTOR_GROUP_COUNT = 6;       // Motor neuron groups
    static constexpr size_t MOTOR_COMMAND_OPTIONS = 8;  // LookLeft/LookRight options
    
    // Developmental stage age thresholds (in simulation time)
    static constexpr double CRITICAL_STAGE_AGE = 60.0;      // ~1 minute
    static constexpr double CRITICAL_PERIOD_AGE = 300.0;    // ~5 minutes
    static constexpr double MATURATION_AGE = 900.0;         // ~15 minutes
    
    // Vision dimension (width * height)
    static constexpr size_t getVisionDimensionStatic() { return VISION_PIXELS; }
    
    // Helper method to calculate vision dimension
    static size_t getVisionDimension() {
        return 16 * 16; // Hard-coded for now, could be configurable
    }
    
    // Public configuration and control methods
    void setSensorNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    float getSensorNoveltyDecay() const { return sensoryNoveltyDecay_; }
    
    // Debug and monitoring utilities
    size_t getTotalMotorNeurons() const {
        return motorForward_.size() + motorBackward_.size() + 
               motorTurnLeft_.size() + motorTurnRight_.size() +
               motorInteract_.size() + motorWait_.size();
    }
    
    size_t getTotalSensoryNeurons() const {
        return sensoryVision_.size() + sensoryTouch_.size() +
               sensoryInternal_.size() + sensoryProprioception_.size();
    }
    
    // Get specific motor neuron group for debugging
    const std::vector<std::shared_ptr<Neuron>>& getMotorForwardNeurons() const { return motorForward_; }
    const std::vector<std::shared_ptr<Neuron>>& getMotorBackwardNeurons() const { return motorBackward_; }
    const std::vector<std::shared_ptr<Neuron>>& getMotorTurnLeftNeurons() const { return motorTurnLeft_; }
    const std::vector<std::shared_ptr<Neuron>>& getMotorTurnRightNeurons() const { return motorTurnRight_; }
    const std::vector<std::shared_ptr<Neuron>>& getMotorInteractNeurons() const { return motorInteract_; }
    const std::vector<std::shared_ptr<Neuron>>& getMotorWaitNeurons() const { return motorWait_; }
    
    // Get specific sensory neuron group for debugging
    const std::vector<std::shared_ptr<Neuron>>& getSensoryVisionNeurons() const { return sensoryVision_; }
    const std::vector<std::shared_ptr<Neuron>>& getSensoryTouchNeurons() const { return sensoryTouch_; }
    const std::vector<std::shared_ptr<Neuron>>& getSensoryInternalNeurons() const { return sensoryInternal_; }
    const std::vector<std::shared_ptr<Neuron>>& getSensoryProprioceptionNeurons() const { return sensoryProprioception_; }
    
    // Validation and error checking
    bool isValid() const {
        if (!brain_) return false;
        // Check that brain has regions
        if (brain_->getRegionCount() == 0) return false;
        return true;
    }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (using shared_ptr for memory safety)
    std::vector<std::shared_ptr<Neuron>> motorForward_;
    std::vector<std::shared_ptr<Neuron>> motorBackward_;
    std::vector<std::shared_ptr<Neuron>> motorTurnLeft_;
    std::vector<std::shared_ptr<Neuron>> motorTurnRight_;
    std::vector<std::shared_ptr<Neuron>> motorInteract_;
    std::vector<std::shared_ptr<Neuron>> motorWait_;
    
    // Sensory neuron groups (using shared_ptr for memory safety)
    std::vector<std::shared_ptr<Neuron>> sensoryVision_;
    std::vector<std::shared_ptr<Neuron>> sensoryTouch_;
    std::vector<std::shared_ptr<Neuron>> sensoryInternal_;
    std::vector<std::shared_ptr<Neuron>> sensoryProprioception_;
    
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
