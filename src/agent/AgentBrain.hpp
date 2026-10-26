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
    
    // Get configuration parameters
    float getSensoryNoveltyDecay() const { return sensoryNoveltyDecay_; }
    float getCuriosityNoveltyWeight() const { return curiosityNoveltyWeight_; }
    float getCuriosityPredictionErrorWeight() const { return curiosityPredictionErrorWeight_; }
    float getExpectedRewardDecayFactor() const { return expectedRewardDecayFactor_; }
    float getExpectedRewardUpdateFactor() const { return expectedRewardUpdateFactor_; }
    float getPlasticityFactorBase() const { return plasticityFactorBase_; }
    float getPlasticityFactorMultiplier() const { return plasticityFactorMultiplier_; }
    float getActionThreshold() const { return actionThreshold_; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getDevelopmentMaturationModifier() const { return developmentMaturationModifier_; }
    
    // Set configuration parameters
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    void setCuriosityNoveltyWeight(float weight) { curiosityNoveltyWeight_ = weight; }
    void setCuriosityPredictionErrorWeight(float weight) { curiosityPredictionErrorWeight_ = weight; }
    void setExpectedRewardDecayFactor(float factor) { expectedRewardDecayFactor_ = factor; }
    void setExpectedRewardUpdateFactor(float factor) { expectedRewardUpdateFactor_ = factor; }
    void setPlasticityFactorBase(float base) { plasticityFactorBase_ = base; }
    void setPlasticityFactorMultiplier(float multiplier) { plasticityFactorMultiplier_ = multiplier; }
    void setActionThreshold(float threshold) { actionThreshold_ = threshold; }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setDevelopmentMaturationModifier(float modifier) { developmentMaturationModifier_ = modifier; }
    
    // Utility functions for advanced users
    void setDevelopmentStage(DevelopmentalStage stage) {
        if (brain_) brain_->setDevelopmentalStage(stage);
    }
    
    void setPlasticityModifier(float modifier) {
        plasticityModifier_ = modifier;
        if (structuralPlasticityEnabled_ && brain_) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                float synRate = 0.0001f * modifier;
                float pruneRate = 0.00001f * (2.0f - modifier);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    }
    
    void setNeuromodulationLevel(float level) {
        dopamineLevel_ = std::clamp(level, -1.0f, 1.0f);
    }
    
    // Get brain statistics
    float getAverageFiringRate() const {
        if (!brain_) return 0.0f;
        return brain_->getAverageFiringRate();
    }
    
    float getExcitationInhibitionRatio() const {
        if (!brain_) return 0.0f;
        return brain_->getExcitationInhibitionRatio();
    }
    
    float getDevelopmentProgress() const {
        // Simple progress based on developmental age (0-100%)
        const float maxAge = 900.0f; // Maturation stage
        return std::min(developmentalAge_ / maxAge * 100.0f, 100.0f);
    }
    
    // Apply common neuromodulation presets
    void applyCuriosityBoost() {
        setCuriosityLevel(1.0f);
    }
    
    void applyExplorationMode() {
        enableCuriosity(true);
        setCuriosityThreshold(0.2f);
        setActionThreshold(0.3f);
    }
    
    void applyExploitationMode() {
        enableCuriosity(false);
        setCuriosityThreshold(0.5f);
        setActionThreshold(0.7f);
    }
    
    void applyDevelopmentAcceleration() {
        enableDevelopment(true);
        setDevelopmentMaturationModifier(0.8f);
        setPlasticityFactorBase(0.6f);
    }
    
    void applyMaturationDeceleration() {
        enableDevelopment(true);
        setDevelopmentMaturationModifier(0.3f);
        setPlasticityFactorBase(0.4f);
    }
    
    // Debug and logging utilities
    void enableDebugLogging(bool enable) { debugLoggingEnabled_ = enable; }
    bool isDebugLoggingEnabled() const { return debugLoggingEnabled_; }
    
    void logDebugInfo(const std::string& message) const {
        if (debugLoggingEnabled_) {
            NLM_LOG_DEBUG("AgentBrain[" + std::to_string(getDevelopmentProgress()) + "%]: " + message);
        }
    }
    
    void logPerformanceStats() const {
        if (debugLoggingEnabled_ && brain_) {
            float avgFiringRate = getAverageFiringRate();
            float eir = getExcitationInhibitionRatio();
            int activeNeurons = brain_->getActiveNeuronCount();
            int firingNeurons = brain_->getFiringNeuronCount();
            int totalNeurons = brain_->getTotalNeuronCount();
            
            NLM_LOG_DEBUG("Performance Stats - Avg Firing Rate: " + std::to_string(avgFiringRate) + " Hz, " +
                         "E/I Ratio: " + std::to_string(eir) + ", " +
                         "Active Neurons: " + std::to_string(activeNeurons) + ", " +
                         "Firing Neurons: " + std::to_string(firingNeurons) + ", " +
                         "Total Neurons: " + std::to_string(totalNeurons));
        }
    }
    
    void logMemoryUsage() const {
        if (debugLoggingEnabled_) {
            size_t visionSize = previousVision_.size();
            size_t motorNeurons = motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() +
                                 motorTurnRight_.size() + motorLookLeft_.size() + motorLookRight_.size() +
                                 motorLook_.size() + motorLookUp_.size() + motorLookDown_.size() +
                                 motorInteract_.size() + motorEat_.size() + motorDrink_.size() +
                                 motorRest_.size() + motorWait_.size() + motorCustom_.size();
            size_t sensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                  sensoryInternal_.size() + sensoryProprioception_.size();
            
            NLM_LOG_DEBUG("Memory Usage - Vision buffer: " + std::to_string(visionSize) + " floats, " +
                         "Motor neurons: " + std::to_string(motorNeurons) + ", " +
                         "Sensory neurons: " + std::to_string(sensoryNeurons) + " neurons");
        }
    }
    
    void logNeuromodulationState() const {
        if (debugLoggingEnabled_) {
            NLM_LOG_DEBUG("Neuromodulation State - Dopamine Level: " + std::to_string(dopamineLevel_) + ", " +
                         "Novelty Level: " + std::to_string(noveltyLevel_) + ", " +
                         "Curiosity Level: " + std::to_string(curiosityLevel_) + ", " +
                         "Prediction Error: " + std::to_string(predictionError_));
        }
    }
    
    void logDevelopmentState() const {
        if (debugLoggingEnabled_) {
            DevelopmentalStage stage = getDevelopmentalStage();
            NLM_LOG_DEBUG("Development State - Stage: " + std::to_string(static_cast<int>(stage)) + ", " +
                         "Age: " + std::to_string(developmentalAge_) + " steps, " +
                         "Plasticity Modifier: " + std::to_string(plasticityModifier_));
        }
    }
    
    // Validation and error checking
    bool validateConfiguration() const {
        bool valid = true;
        
        if (curiosityEnabled_ && curiosityThreshold_ < 0.0f) {
            NLM_LOG_ERROR("Invalid configuration: curiosity threshold must be >= 0.0");
            valid = false;
        }
        
        if (actionThreshold_ < 0.0f || actionThreshold_ > 1.0f) {
            NLM_LOG_ERROR("Invalid configuration: action threshold must be between 0.0 and 1.0");
            valid = false;
        }
        
        if (curiosityThreshold_ > 1.0f) {
            NLM_LOG_ERROR("Invalid configuration: curiosity threshold must be <= 1.0");
            valid = false;
        }
        
        if (sensoryNoveltyDecay_ < 0.0f || sensoryNoveltyDecay_ > 1.0f) {
            NLM_LOG_ERROR("Invalid configuration: sensory novelty decay must be between 0.0 and 1.0");
            valid = false;
        }
        
        if (expectedRewardDecayFactor_ < 0.0f || expectedRewardDecayFactor_ > 1.0f) {
            NLM_LOG_ERROR("Invalid configuration: expected reward decay factor must be between 0.0 and 1.0");
            valid = false;
        }
        
        if (plasticityFactorBase_ < 0.0f || plasticityFactorBase_ > 2.0f) {
            NLM_LOG_ERROR("Invalid configuration: plasticity factor base must be between 0.0 and 2.0");
            valid = false;
        }
        
        if (plasticityFactorMultiplier_ < -1.0f || plasticityFactorMultiplier_ > 1.0f) {
            NLM_LOG_ERROR("Invalid configuration: plasticity factor multiplier must be between -1.0 and 1.0");
            valid = false;
        }
        
        if (valid) {
            NLM_LOG_INFO("Configuration validation passed");
        }
        
        return valid;
    }
    
    // Get system health information
    std::string getSystemHealth() const {
        std::ostringstream oss;
        oss << "=== System Health ===\n";
        
        if (!brain_) {
            oss << "ERROR: Brain not initialized\n";
            return oss.str();
        }
        
        oss << "Brain: " << brain_->getTotalNeuronCount() << " neurons, " +
                brain_->getTotalSynapseCount() << " synapses\n";
        oss << "Memory Usage: OK\n";
        oss << "Configuration: " << (validateConfiguration() ? "VALID" : "INVALID") << "\n";
        oss << "Debug Logging: " << (debugLoggingEnabled_ ? "ENABLED" : "DISABLED") << "\n";
        
        return oss.str();
    }
    
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
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
    std::vector<Neuron*> motorLook_;
    std::vector<Neuron*> motorLookUp_;
    std::vector<Neuron*> motorLookDown_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorEat_;
    std::vector<Neuron*> motorDrink_;
    std::vector<Neuron*> motorRest_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorCustom_;
    
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
    
    // Configuration parameters
    float sensoryNoveltyDecay_;
    float curiosityNoveltyWeight_;
    float curiosityPredictionErrorWeight_;
    float expectedRewardDecayFactor_;
    float expectedRewardUpdateFactor_;
    float plasticityFactorBase_;
    float plasticityFactorMultiplier_;
    float actionThreshold_;
    float curiosityThreshold_;
    float developmentMaturationModifier_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Debug and logging
    bool debugLoggingEnabled_;
    mutable std::chrono::steady_clock::time_point lastDebugLogTime_;
};

} // namespace nlm
