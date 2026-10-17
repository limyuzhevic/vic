#pragma once

#include "EnhancedAgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryPercept.hpp"

namespace nlm
{

// Example 1: Custom listener implementation for tracking agent behavior
class BehaviorTrackingListener : public AgentBrainStateChangeListener {
public:
    struct BehaviorMetrics {
        double totalTimeInStates[4] = {0.0};  // Initial, CriticalPeriod, Maturation, Adult
        int actionCounts[8] = {0};  // MoveForward, MoveBackward, etc.
        float totalReward = 0.0f;
        float noveltySum = 0.0f;
        int curiosityChecks = 0;
    };
    
    BehaviorTrackingListener() : currentStage_(DevelopmentalStage::Initial) {}
    
    void onDevelopmentalStageChange(DevelopmentalStage oldStage, DevelopmentalStage newStage) override {
        // Track time spent in each developmental stage
        double timeInOldStage = getTimeInStage(oldStage);
        stagesStartTime_[oldStage] = getCurrentTimestamp();
        
        NLM_LOG_INFO(std::string("Developmental stage changed from ") + 
                    developmentalStageToString(oldStage) + " to " +
                    developmentalStageToString(newStage));
    }
    
    void onRewardModulationApplied(float reward, float predictionError) override {
        behaviorMetrics_.totalReward += reward;
        totalRewardSum_ += reward;
        
        NLM_LOG_DEBUG(std::string("Reward applied: ") + std::to_string(reward) +
                     " (Prediction error: " + std::to_string(predictionError) + ")");
    }
    
    void onCuriosityLevelChanged(float oldLevel, float newLevel) override {
        behaviorMetrics_.curiosityChecks++;
        curiosityHistory_.push_back(newLevel);
        
        if (newLevel > 0.8f) {
            NLM_LOG_WARN(std::string("High curiosity detected: ") + std::to_string(newLevel));
        }
    }
    
    void onNoveltyLevelChanged(float oldLevel, float newLevel) override {
        behaviorMetrics_.noveltySum += newLevel;
        noveltyHistory_.push_back(newLevel);
        
        if (newLevel > 0.5f) {
            NLM_LOG_DEBUG(std::string("High novelty detected: ") + std::to_string(newLevel));
        }
    }
    
    void onPredictionErrorChanged(float oldError, float newError) override {
        predictionErrorHistory_.push_back(newError);
        
        if (std::abs(newError) > 0.5f) {
            NLM_LOG_WARN(std::string("Large prediction error: ") + std::to_string(newError));
        }
    }
    
    void onConfigurationChanged(const std::string& parameterName, float oldValue, float newValue) override {
        NLM_LOG_INFO(std::string("Configuration changed: ") + parameterName + 
                    " from " + std::to_string(oldValue) + " to " + std::to_string(newValue));
        
        // Log significant parameter changes
        if (parameterName == "noveltyDecayRate" && std::abs(newValue - 0.99f) > 0.1f) {
            NLM_LOG_WARN(std::string("Significant novelty decay change detected"));
        }
    }
    
    void onErrorReported(const AgentBrainErrorHandler::ErrorInfo& error) override {
        NLM_LOG_ERROR(std::string("AgentBrain error reported: ") + error.message);
    }
    
    // Get current behavior metrics
    const BehaviorMetrics& getBehaviorMetrics() const { return behaviorMetrics_; }
    
    // Get historical data for analysis
    const std::vector<float>& getCuriosityHistory() const { return curiosityHistory_; }
    const std::vector<float>& getNoveltyHistory() const { return noveltyHistory_; }
    const std::vector<float>& getPredictionErrorHistory() const { return predictionErrorHistory_; }
    
    // Generate performance report
    std::string generateBehaviorReport() const {
        std::stringstream ss;
        ss << "Behavior Analysis Report\n";
        ss << "========================\n";
        ss << "Total Reward: " << behaviorMetrics_.totalReward << "\n";
        ss << "Average Novelty: " << (behaviorMetrics_.noveltySum / behaviorMetrics_.curiosityChecks) << "\n";
        ss << "Action Counts:\n";
        
        const char* actionNames[] = {"MoveForward", "MoveBackward", "TurnLeft", "TurnRight", 
                                     "LookLeft", "LookRight", "Interact", "Wait"};
        
        for (int i = 0; i < 8; ++i) {
            ss << "  " << actionNames[i] << ": " << behaviorMetrics_.actionCounts[i] << "\n";
        }
        
        return ss.str();
    }
    
    // Export metrics for external analysis
    std::string exportMetricsToJSON() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"total_reward\": " << behaviorMetrics_.totalReward << ",\n";
        ss << "  \"average_novelty\": " << (behaviorMetrics_.noveltySum / behaviorMetrics_.curiosityChecks) << ",\n";
        ss << "  \"action_counts\": [";
        
        for (int i = 0; i < 8; ++i) {
            ss << behaviorMetrics_.actionCounts[i];
            if (i < 7) ss << ", ";
        }
        ss << "],\n";
        ss << "  \"curiosity_history\": [";
        
        for (size_t i = 0; i < curiosityHistory_.size(); ++i) {
            ss << curiosityHistory_[i];
            if (i < curiosityHistory_.size() - 1) ss << ", ";
        }
        ss << "]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // Get time spent in developmental stages
    double getTimeInStage(DevelopmentalStage stage) const {
        auto it = stagesStartTime_.find(stage);
        if (it != stagesStartTime_.end()) {
            return getCurrentTimestamp() - it->second;
        }
        return 0.0;
    }
    
    // Get current developmental stage
    DevelopmentalStage getCurrentStage() const { return currentStage_; }
    
    // Set current stage (for manual tracking)
    void setCurrentStage(DevelopmentalStage stage) { currentStage_ = stage; }
    
private:
    BehaviorMetrics behaviorMetrics_;
    std::map<DevelopmentalStage, double> stagesStartTime_;
    std::vector<float> curiosityHistory_;
    std::vector<float> noveltyHistory_;
    std::vector<float> predictionErrorHistory_;
    float totalRewardSum_ = 0.0f;
    DevelopmentalStage currentStage_;
    
    double getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return static_cast<double>(millis) / 1000.0;
    }
    
    std::string developmentalStageToString(DevelopmentalStage stage) const {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            default: return "Unknown";
        }
    }
};

// Example 2: Advanced configuration manager for custom brain parameters
class CustomAgentBrainManager {
public:
    struct AdvancedConfiguration {
        // High-level configuration groups
        struct LearningParams {
            float predictionErrorSensitivity = 1.0f;
            float weightLearningRate = 0.01f;
            float eligibilityTraceDecay = 0.9f;
            bool useNeuronGating = false;
        } learning;
        
        struct DevelopmentalParams {
            double criticalPeriodStart = 60.0;
            double criticalPeriodEnd = 300.0;
            float plasticityDuringCriticalPeriod = 0.8f;
            bool enableStructuralPlasticity = true;
        } development;
        
        struct BehavioralParams {
            float curiosityThreshold = 0.3f;
            float explorationProbability = 0.2f;
            float noveltyImportance = 1.0f;
            bool use intrinsicMotivation = true;
        } behavior;
        
        struct SensoryParams {
            float visionSensitivity = 1.0f;
            float touchThreshold = 0.5f;
            float internalSignalScaling = 1.0f;
            bool useAudioSensing = false;
        } sensory;
    };
    
    CustomAgentBrainManager(std::shared_ptr<AgentBrain> brain) : brain_(brain) {}
    
    // Apply advanced configuration to the agent brain
    void applyAdvancedConfiguration(const AdvancedConfiguration& config) {
        NLM_LOG_INFO("Applying advanced configuration to agent brain");
        
        // Apply learning parameters
        brain_->getConfig().setParameter("predictionErrorLearningRate", 
                                        config.learning.weightLearningRate);
        brain_->getConfig().setParameter("eligibilityTraceDecay", 
                                        config.learning.eligibilityTraceDecay);
        
        // Apply developmental parameters
        brain_->getConfig().setParameter("developmentalStage2Age", 
                                        config.development.criticalPeriodEnd);
        brain_->getConfig().setParameter("plasticityModifier", 
                                        config.development.plasticityDuringCriticalPeriod);
        
        // Apply behavioral parameters
        brain_->getConfig().setParameter("curiosityThreshold", 
                                        config.behavior.curiosityThreshold);
        brain_->getConfig().setParameter("explorationBaseProbability", 
                                        config.behavior.explorationProbability);
        
        // Apply sensory parameters
        brain_->getConfig().setParameter("visionScaleFactor", 
                                        config.sensory.visionSensitivity);
        brain_->getConfig().setParameter("touchScaleFactor", 
                                        config.sensory.touchThreshold);
        
        // Enable/disable features
        brain_->enableRewardModulation(true);
        brain_->enableDevelopment(config.development.enableStructuralPlasticity);
        brain_->enableCuriosity(config.behavior.useIntrinsicMotivation);
        
        NLM_LOG_INFO("Advanced configuration applied successfully");
    }
    
    // Create a specialized brain configuration for specific environments
    static AdvancedConfiguration createEnvironmentSpecificConfig(const std::string& environmentType) {
        AdvancedConfiguration config;
        
        if (environmentType == "open_field") {
            // Open field environment: encourage exploration
            config.behavior.explorationProbability = 0.3f;
            config.behavior.curiosityThreshold = 0.2f;
            config.learning.predictionErrorSensitivity = 0.8f;
        } else if (environmentType == "maze") {
            // Maze environment: encourage systematic exploration
            config.behavior.explorationProbability = 0.1f;
            config.behavior.curiosityThreshold = 0.4f;
            config.learning.predictionErrorSensitivity = 1.2f;
        } else if (environmentType == "hazardous") {
            // Hazardous environment: conservative approach
            config.behavior.explorationProbability = 0.05f;
            config.behavior.curiosityThreshold = 0.6f;
            config.learning.predictionErrorSensitivity = 1.5f;
        } else if (environmentType == "resource_rich") {
            // Resource-rich environment: balance exploration and exploitation
            config.behavior.explorationProbability = 0.25f;
            config.behavior.curiosityThreshold = 0.35f;
            config.learning.predictionErrorSensitivity = 1.0f;
        }
        
        return config;
    }
    
    // Get current configuration status
    std::string getConfigurationStatus() const {
        std::stringstream ss;
        ss << "Current AgentBrain Configuration:\n";
        ss << "============================\n";
        
        const auto& config = brain_->getConfig();
        
        ss << "Vision Scale Factor: " << config.getParameter("visionScaleFactor", config.visionScaleFactor) << "\n";
        ss << "Touch Scale Factor: " << config.getParameter("touchScaleFactor", config.touchScaleFactor) << "\n";
        ss << "Curiosity Threshold: " << config.getParameter("curiosityThreshold", config.curiosityThreshold) << "\n";
        ss << "Exploration Base Probability: " << config.getParameter("explorationBaseProbability", config.explorationBaseProbability) << "\n";
        ss << "Prediction Error Learning Rate: " << config.getParameter("predictionErrorLearningRate", config.predictionErrorLearningRate) << "\n";
        
        return ss.str();
    }
    
    // Create a composite brain manager (for advanced users)
    void addBrainManager(std::shared_ptr<CustomAgentBrainManager> manager) {
        additionalManagers_.push_back(manager);
        NLM_LOG_INFO("Additional brain manager added");
    }
    
    // Execute all managers
    void executeAllManagers() {
        for (const auto& manager : additionalManagers_) {
            manager->executeCustomLogic(brain_);
        }
    }
    
protected:
    virtual void executeCustomLogic(std::shared_ptr<AgentBrain> brain) {
        // Default implementation - can be overridden by subclasses
    }
    
private:
    std::shared_ptr<AgentBrain> brain_;
    std::vector<std::shared_ptr<CustomAgentBrainManager>> additionalManagers_;
};

// Example 3: Advanced training manager for custom learning strategies
class TrainingManager {
public:
    struct TrainingStrategy {
        enum class Type {
            Standard,
            ExplorationFocused,
            Conservative,
            Adaptive,
            CuriosityDriven
        };
        
        Type type = Type::Standard;
        double learningRate = 0.01;
        double explorationRate = 0.1;
        double targetNoveltyLevel = 0.5;
        bool useRewardPrediction = true;
        bool adaptToEnvironment = true;
    };
    
    TrainingManager(std::shared_ptr<EnhancedAgentBrain> agent) : agent_(agent) {}
    
    // Execute training strategy
    void executeTrainingStep(double timestep) {
        switch (currentStrategy_.type) {
            case TrainingStrategy::Standard:
                executeStandardTraining(timestep);
                break;
            case TrainingStrategy::ExplorationFocused:
                executeExplorationFocusedTraining(timestep);
                break;
            case TrainingStrategy::Conservative:
                executeConservativeTraining(timestep);
                break;
            case TrainingStrategy::Adaptive:
                executeAdaptiveTraining(timestep);
                break;
            case TrainingStrategy::CuriosityDriven:
                executeCuriosityDrivenTraining(timestep);
                break;
        }
    }
    
    // Set training strategy
    void setStrategy(const TrainingStrategy& strategy) {
        currentStrategy_ = strategy;
        NLM_LOG_INFO(std::string("Training strategy set to: ") + strategyTypeToString(strategy.type));
    }
    
    // Get current strategy
    const TrainingStrategy& getCurrentStrategy() const { return currentStrategy_; }
    
    // Generate training report
    std::string generateTrainingReport() const {
        std::stringstream ss;
        ss << "Training Analysis Report\n";
        ss << "======================\n";
        ss << "Current Strategy: " << strategyTypeToString(currentStrategy_.type) << "\n";
        ss << "Learning Rate: " << currentStrategy_.learningRate << "\n";
        ss << "Exploration Rate: " << currentStrategy_.explorationRate << "\n";
        ss << "Target Novelty Level: " << currentStrategy_.targetNoveltyLevel << "\n";
        
        // Get agent's current performance metrics
        const auto& profiler = agent_->getProfiler();
        auto metrics = profiler.getCurrentMetrics();
        
        ss << "Agent Performance:\n";
        ss << "  Average Processing Time: " << metrics.totalUpdateTime << "ms\n";
        ss << "  Sensory Processing Rate: " << (metrics.sensoryProcessCount / std::max<double>(1.0, metrics.totalUpdateTime / 1000.0)) << " per second\n";
        ss << "  Motor Decoding Rate: " << (metrics.motorDecodeCount / std::max<double>(1.0, metrics.totalUpdateTime / 1000.0)) << " per second\n";
        
        return ss.str();
    }
    
    // Export training data for analysis
    std::string exportTrainingData() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"strategy\": \"" << strategyTypeToString(currentStrategy_.type) << "\",\n";
        ss << "  \"learning_rate\": " << currentStrategy_.learningRate << ",\n";
        ss << "  \"exploration_rate\": " << currentStrategy_.explorationRate << ",\n";
        ss << "  \"target_novelty\": " << currentStrategy_.targetNoveltyLevel << ",\n";
        ss << "  \"reward_prediction\": " << (currentStrategy_.useRewardPrediction ? "true" : "false") << ",\n";
        ss << "  \"adaptive\": " << (currentStrategy_.adaptToEnvironment ? "true" : "false") << "\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // Import training configuration
    bool importTrainingConfig(const std::string& configStr) {
        // Simple JSON parsing for training configuration
        // In production, use a proper JSON parser
        try {
            if (configStr.find("learning_rate") != std::string::npos) {
                size_t start = configStr.find("learning_rate": ") + 15;
                size_t end = configStr.find(",", start);
                currentStrategy_.learningRate = std::stof(configStr.substr(start, end - start));
            }
            if (configStr.find("exploration_rate") != std::string::npos) {
                size_t start = configStr.find("exploration_rate": ") + 19;
                size_t end = configStr.find(",", start);
                currentStrategy_.explorationRate = std::stof(configStr.substr(start, end - start));
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    
private:
    std::shared_ptr<EnhancedAgentBrain> agent_;
    TrainingStrategy currentStrategy_;
    
    void executeStandardTraining(double timestep) {
        // Standard reinforcement learning approach
        agent_->getCoreBrain().applyRewardModulation(0.1f, 0.05f);
        agent_->getCoreBrain().updateDevelopment(timestep);
    }
    
    void executeExplorationFocusedTraining(double timestep) {
        // Emphasize exploration and curiosity
        agent_->getCoreBrain().enableCuriosity(true);
        agent_->getProfiler().setEnabled(true);
        agent_->getCoreBrain().applyRewardModulation(0.05f, 0.02f);
        agent_->getCoreBrain().updateDevelopment(timestep);
    }
    
    void executeConservativeTraining(double timestep) {
        // Conservative approach, minimize risk
        agent_->getCoreBrain().enableCuriosity(false);
        agent_->getCoreBrain().applyRewardModulation(0.01f, 0.005f);
        agent_->getCoreBrain().updateDevelopment(timestep);
    }
    
    void executeAdaptiveTraining(double timestep) {
        // Adaptive approach that adjusts to environment
        agent_->getCoreBrain().applyRewardModulation(0.08f, 0.04f);
        agent_->getCoreBrain().updateDevelopment(timestep);
        
        // Adjust parameters based on performance
        auto& profiler = agent_->getProfiler();
        auto metrics = profiler.getCurrentMetrics();
        
        if (metrics.totalUpdateTime > 100.0) {  // If too slow, reduce complexity
            agent_->getCoreBrain().enableDevelopment(false);
        }
    }
    
    void executeCuriosityDrivenTraining(double timestep) {
        // Emphasize curiosity-driven exploration
        agent_->getCoreBrain().enableCuriosity(true);
        
        // Set high exploration probability
        auto& config = agent_->getCoreBrain().getConfig();
        config.setParameter("explorationBaseProbability", 0.4f);
        
        agent_->getCoreBrain().applyRewardModulation(0.03f, 0.01f);
        agent_->getCoreBrain().updateDevelopment(timestep);
    }
    
    std::string strategyTypeToString(TrainingStrategy::Type type) const {
        switch (type) {
            case TrainingStrategy::Type::Standard: return "Standard";
            case TrainingStrategy::Type::ExplorationFocused: return "ExplorationFocused";
            case TrainingStrategy::Type::Conservative: return "Conservative";
            case TrainingStrategy::Type::Adaptive: return "Adaptive";
            case TrainingStrategy::Type::CuriosityDriven: return "CuriosityDriven";
            default: return "Unknown";
        }
    }
};

} // namespace nlm