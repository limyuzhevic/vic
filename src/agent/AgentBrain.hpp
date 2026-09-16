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
    
    // Modern C++ API enhancements
    
    // State management for serialization/pickling
    struct AgentState {
        float dopamineLevel_;
        float noveltyLevel_;
        float curiosityLevel_;
        float predictionError_;
        float expectedReward_;
        double developmentalAge_;
        float plasticityModifier_;
        bool rewardModulationEnabled_;
        bool structuralPlasticityEnabled_;
        bool developmentEnabled_;
        bool curiosityEnabled_;
        std::vector<float> previousVision_;
        
        AgentState() : dopamineLevel_(0.0f), noveltyLevel_(0.0f), curiosityLevel_(0.0f),
                       predictionError_(0.0f), expectedReward_(0.0f), developmentalAge_(0.0),
                       plasticityModifier_(1.0f), rewardModulationEnabled_(true),
                       structuralPlasticityEnabled_(true), developmentEnabled_(true),
                       curiosityEnabled_(true), sensoryNoveltyDecay_(0.99f) {}
        
        // Convert state to dictionary-like structure
        std::map<std::string, double> toMap() const {
            std::map<std::string, double> result;
            result["dopamine"] = dopamineLevel_;
            result["novelty"] = noveltyLevel_;
            result["curiosity"] = curiosityLevel_;
            result["prediction_error"] = predictionError_;
            result["expected_reward"] = expectedReward_;
            result["developmental_age"] = developmentalAge_;
            result["plasticity_modifier"] = plasticityModifier_;
            result["reward_mod_enabled"] = rewardModulationEnabled_ ? 1.0 : 0.0;
            result["structural_plastic_enabled"] = structuralPlasticityEnabled_ ? 1.0 : 0.0;
            result["development_enabled"] = developmentEnabled_ ? 1.0 : 0.0;
            result["curiosity_enabled"] = curiosityEnabled_ ? 1.0 : 0.0;
            return result;
        }
        
        // Load state from dictionary-like structure
        void fromMap(const std::map<std::string, double>& map) {
            auto getValue = [&map](const std::string& key, double defaultValue) {
                auto it = map.find(key);
                return (it != map.end()) ? it->second : defaultValue;
            };
            
            dopamineLevel_ = static_cast<float>(getValue("dopamine", 0.0));
            noveltyLevel_ = static_cast<float>(getValue("novelty", 0.0));
            curiosityLevel_ = static_cast<float>(getValue("curiosity", 0.0));
            predictionError_ = static_cast<float>(getValue("prediction_error", 0.0));
            expectedReward_ = static_cast<float>(getValue("expected_reward", 0.0));
            developmentalAge_ = getValue("developmental_age", 0.0);
            plasticityModifier_ = static_cast<float>(getValue("plasticity_modifier", 1.0));
            rewardModulationEnabled_ = (getValue("reward_mod_enabled", 1.0) > 0.5);
            structuralPlasticityEnabled_ = (getValue("structural_plastic_enabled", 1.0) > 0.5);
            developmentEnabled_ = (getValue("development_enabled", 1.0) > 0.5);
            curiosityEnabled_ = (getValue("curiosity_enabled", 1.0) > 0.5);
        }
    };
    
    // Get current agent state
    AgentState getState() const {
        AgentState state;
        state.dopamineLevel_ = dopamineLevel_;
        state.noveltyLevel_ = noveltyLevel_;
        state.curiosityLevel_ = curiosityLevel_;
        state.predictionError_ = predictionError_;
        state.expectedReward_ = expectedReward_;
        state.developmentalAge_ = developmentalAge_;
        state.plasticityModifier_ = plasticityModifier_;
        state.rewardModulationEnabled_ = rewardModulationEnabled_;
        state.structuralPlasticityEnabled_ = structuralPlasticityEnabled_;
        state.developmentEnabled_ = developmentEnabled_;
        state.curiosityEnabled_ = curiosityEnabled_;
        state.previousVision_ = previousVision_;
        return state;
    }
    
    // Set agent state from saved state
    void setState(const AgentState& state) {
        dopamineLevel_ = state.dopamineLevel_;
        noveltyLevel_ = state.noveltyLevel_;
        curiosityLevel_ = state.curiosityLevel_;
        predictionError_ = state.predictionError_;
        expectedReward_ = state.expectedReward_;
        developmentalAge_ = state.developmentalAge_;
        plasticityModifier_ = state.plasticityModifier_;
        rewardModulationEnabled_ = state.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = state.structuralPlasticityEnabled_;
        developmentEnabled_ = state.developmentEnabled_;
        curiosityEnabled_ = state.curiosityEnabled_;
        previousVision_ = state.previousVision_;
    }
    
    // Enhanced convenience methods for Pythonic API
    
    // Batch enable/disable features
    void enableFeatures(bool reward, bool plastic, bool develop, bool curious) {
        enableRewardModulation(reward);
        enableStructuralPlasticity(plastic);
        enableDevelopment(develop);
        enableCuriosity(curious);
    }
    
    // Get learning statistics
    struct LearningStats {
        float curiosity_level;
        float novelty_level;
        float prediction_error;
        float neuromodulation_level;
        DevelopmentalStage developmental_stage;
        float plasticity_modifier;
        float exploration_ratio;  // Based on curiosity > threshold
        
        LearningStats() : curiosity_level(0.0f), novelty_level(0.0f), prediction_error(0.0f),
                         neuromodulation_level(0.0f), developmental_stage(DevelopmentalStage::Initial),
                         plasticity_modifier(1.0f), exploration_ratio(0.0f) {}
    };
    
    LearningStats getLearningStats() const {
        LearningStats stats;
        stats.curiosity_level = getCuriosityLevel();
        stats.novelty_level = getNoveltyLevel();
        stats.prediction_error = getPredictionError();
        stats.neuromodulation_level = getNeuromodulationLevel();
        stats.developmental_stage = getDevelopmentalStage();
        stats.plasticity_modifier = plasticityModifier_;
        stats.exploration_ratio = (curiosityLevel_ > 0.3f) ? curiosityLevel_ * 0.3f : 0.0f;
        return stats;
    }
    
    // Training convenience method
    void trainStep(SimulationStep step, float reward, float predictedReward) {
        // Process current percept (will be stored in private state)
        if (curiosityEnabled_) {
            // Increment curiosity based on prediction error
            curiosityLevel_ += std::abs(predictionError_) * 0.05f;
            curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        }
        
        // Apply reward modulation
        applyRewardModulation(reward, predictedReward);
        
        // Update development
        updateDevelopment(0.1);  // Fixed timestep for convenience
    }
    
    // Method chaining support (returns reference to self)
    class ChainingHelper {
        AgentBrain& agent_;
    public:
        ChainingHelper(AgentBrain& agent) : agent_(agent) {}
        
        // Enable features for chaining
        ChainingHelper& withRewardModulation(bool enable = true) {
            agent_.enableRewardModulation(enable);
            return *this;
        }
        
        ChainingHelper& withStructuralPlasticity(bool enable = true) {
            agent_.enableStructuralPlasticity(enable);
            return *this;
        }
        
        ChainingHelper& withDevelopment(bool enable = true) {
            agent_.enableDevelopment(enable);
            return *this;
        }
        
        ChainingHelper& withCuriosity(bool enable = true) {
            agent_.enableCuriosity(enable);
            return *this;
        }
        
        // Complete the chain
        AgentBrain& configure() {
            return agent_;
        }
    };
    
    // Get chaining helper for fluent configuration
    ChainingHelper configure() {
        return ChainingHelper(*this);
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
};

} // namespace nlm
