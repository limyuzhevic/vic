#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <any>
#include <cmath>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class SimpleWorld;
class SensoryPercept;
class Neuron;
class Action;
class MotorCommand;

// Learning profile configuration structure
struct LearningProfile {
    bool reward_modulation = true;
    bool structural_plasticity = true;
    bool development = true;
    bool curiosity = true;
};

// Implementation of AgentBrain methods
namespace {
    
    // Helper function to convert string to developmental stage
    DevelopmentalStage parseDevelopmentalStage(const std::string& stage_str) {
        if (stage_str == "CriticalPeriod") return DevelopmentalStage::CriticalPeriod;
        if (stage_str == "Maturation") return DevelopmentalStage::Maturation;
        if (stage_str == "Adult") return DevelopmentalStage::Adult;
        if (stage_str == "Aging") return DevelopmentalStage::Aging;
        return DevelopmentalStage::Initial;
    }
    
    // Helper function to convert developmental stage to string
    std::string developmentalStageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Initial";
        }
    }
    
} // anonymous namespace

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
    
    // ========== NEW METHODS FOR IMPROVED API ==========
    
    // Get focus level
    float getFocusLevel() const {
        if (brain_ && brain_->getAttention()) {
            return brain_->getAttention()->getFocusStrength();
        }
        return 0.0f;
    }
    
    // Get attention level
    float getAttentionLevel() const {
        if (brain_ && brain_->getAttention()) {
            return brain_->getAttention()->getAttentionStrength();
        }
        return 0.0f;
    }
    
    // Get comprehensive learning status
    std::unordered_map<std::string, bool> getLearningStatus() const {
        std::unordered_map<std::string, bool> status;
        status["reward_modulation"] = rewardModulationEnabled_;
        status["structural_plasticity"] = structuralPlasticityEnabled_;
        status["development"] = developmentEnabled_;
        status["curiosity"] = curiosityEnabled_;
        status["neuromodulation"] = (getNeuromodulationLevel() > 0.0f);
        return status;
    }
    
    // Get all neural levels
    std::unordered_map<std::string, float> getAllLevels() const {
        std::unordered_map<std::string, float> levels;
        levels["curiosity"] = getCuriosityLevel();
        levels["novelty"] = getNoveltyLevel();
        levels["neuromodulation"] = getNeuromodulationLevel();
        levels["prediction_error"] = getPredictionError();
        levels["focus"] = getFocusLevel();
        levels["attention"] = getAttentionLevel();
        return levels;
    }
    
    // Get comprehensive metrics summary
    std::unordered_map<std::string, std::any> getMetricsSummary() const {
        std::unordered_map<std::string, std::any> metrics;
        
        // Add all neural levels
        auto levels = getAllLevels();
        for (const auto& pair : levels) {
            metrics[pair.first] = pair.second;
        }
        
        // Add system state
        metrics["state"] = developmentalStageToString(getDevelopmentalStage());
        metrics["total_steps"] = totalSteps_;
        metrics["sensory_processed"] = totalSensoryProcessed_;
        metrics["actions_generated"] = totalActionsGenerated_;
        metrics["start_time"] = startTime_;
        metrics["session_duration"] = std::chrono::system_clock::now().time_since_epoch().count() - startTime_;
        
        return metrics;
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
    
    // Tracking state
    size_t totalSteps_;
    size_t totalSensoryProcessed_;
    size_t totalActionsGenerated_;
    double startTime_;
};

} // namespace nlm
