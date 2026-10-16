#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <future>
#include <unordered_map>

namespace nlm {

// Advanced AgentBrain constants
namespace AgentBrainConstants {
    constexpr float VISION_SENSITIVITY = 5.0f;
    constexpr float TOUCH_SENSITIVITY = 8.0f;
    constexpr float INTERNAL_SENSITIVITY = 5.0f;
    constexpr float PROPRIOCEPTION_SENSITIVITY = 3.0f;
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;
    constexpr float EXCURSION_FACTOR = 2.0f;
    constexpr float MIN_EXCURSION = 0.0f;
    constexpr float MAX_EXCURSION = 1.0f;
    constexpr float MIN_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CHANCE_FACTOR = 0.3f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    constexpr size_t MOTOR_GROUP_COUNT = 6;
    constexpr size_t SENSORY_GROUP_COUNT = 4;
    
    // Advanced constants
    constexpr float ADAPTIVE_SENSITIVITY = 0.8f;
    constexpr float CONSOLIDATION_THRESHOLD = 0.7f;
    constexpr float MEMORY_REPLAY_PROBABILITY = 0.3f;
    constexpr float STRUCTURAL_PLASTICITY_FACTOR = 1.5f;
    constexpr float NEUROMODULATION_SENSITIVITY = 1.2f;
    constexpr size_t ADVANCED_MOTOR_COMMANDS = 12;
    constexpr size_t MAX_ACTION_HISTORY = 1000;
    constexpr float LEARNING_RATE_ADAPTATION = 0.05f;
    constexpr float NOISE_LEVEL_BASE = 0.01f;
    constexpr float THRESHOLD_ADAPTATION = 0.1f;
}

// Advanced Motor Command enum with additional commands
enum class AdvancedMotorCommand {
    MoveForward = 0,      MoveBackward = 1,
    TurnLeft = 2,         TurnRight = 3,
    LookLeft = 4,         LookRight = 5,
    Interact = 6,        Wait = 7,
    
    // Advanced commands
    Accelerate = 8,      Decelerate = 9,
    RotateLeftFine = 10,  RotateRightFine = 11,
    ScanArea = 12        // Advanced sensory scanning command
};

// Advanced neural processing modes
enum class NeuralProcessingMode {
    Standard,             // Standard LIF dynamics
    Adaptive,            // Adaptive threshold and sensitivity
    Burst,               // Burst firing mode
    BurstAdaptive,       // Adaptive burst firing
    Stochastic,           // Stochastic neural dynamics
    Deterministic        // Deterministic neural dynamics
};

// Memory consolidation strategies
enum class MemoryConsolidationStrategy {
    Simple,              // Simple consolidation
    Priority,            // Priority-based consolidation
    ReplayBased,          // Memory replay-based consolidation
    TimeBased,            // Time-based consolidation
    StressBased          // Stress-based consolidation
};

// Advanced AgentBrain implementation
class AdvancedAgentBrain {
private:
    // Core components
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<SimpleWorld> world_;
    
    // Advanced state tracking
    std::unordered_map<AdvancedMotorCommand, float> actionHistory_;
    std::vector<MotorCommand> recentActions_;
    std::vector<float> rewardHistory_;
    std::vector<double> actionValues_;
    
    // Advanced neural parameters
    float adaptiveSensitivity_;
    float learningRate_;
    float noiseLevel_;
    float thresholdAdjustment_;
    float memoryConsolidationRate_;
    float structuralPlasticityRate_;
    float neuromodulationSensitivity_;
    
    // Advanced processing modes
    NeuralProcessingMode processingMode_;
    MemoryConsolidationStrategy consolidationStrategy_;
    bool enableAdvancedLearning_;
    bool enableAdaptiveThreshold_;
    bool enableMemoryReplay_;
    bool enableParallelProcessing_;
    
    // Performance monitoring
    double totalProcessingTime_;
    size_t totalStepsProcessed_;
    size_t totalActionsTaken_;
    std::chrono::system_clock::time_point lastPerformanceReport_;
    
    // Advanced motor neuron groups
    std::vector<Neuron*> advancedMotorGroups_[ADVANCED_MOTOR_COMMANDS];
    std::vector<Neuron*> adaptiveMotorGroups_[6];
    
    // Advanced sensory processing
    std::vector<float> enhancedVisionBuffer_;
    std::vector<float> temporalSensoryBuffer_;
    std::vector<float> contextualMemoryBuffer_;
    
    // Advanced neuromodulation
    std::vector<float> dopamineHistory_;
    std::vector<float> curiosityHistory_;
    std::vector<float> predictionErrorHistory_;
    std::vector<float> noveltyHistory_;
    
    // Configuration
    std::map<std::string, ConfigValue> advancedConfig_;
    
    // Threading for parallel processing
    std::mutex processingMutex_;
    std::future<void> asyncProcessingFuture_;
    
public:
    AdvancedAgentBrain(std::shared_ptr<Brain> brain, std::unique_ptr<SimpleWorld> world)
        : brain_(brain), world_(std::move(world))
        , adaptiveSensitivity_(AgentBrainConstants::ADAPTIVE_SENSITIVITY)
        , learningRate_(0.1f)
        , noiseLevel_(AgentBrainConstants::NOISE_LEVEL_BASE)
        , thresholdAdjustment_(0.0f)
        , memoryConsolidationRate_(0.5f)
        , structuralPlasticityRate_(AgentBrainConstants::STRUCTURAL_PLASTICITY_FACTOR)
        , neuromodulationSensitivity_(AgentBrainConstants::NEUROMODULATION_SENSITIVITY)
        , processingMode_(NeuralProcessingMode::Adaptive)
        , consolidationStrategy_(MemoryConsolidationStrategy::Priority)
        , enableAdvancedLearning_(true)
        , enableAdaptiveThreshold_(true)
        , enableMemoryReplay_(true)
        , enableParallelProcessing_(true)
        , totalProcessingTime_(0.0)
        , totalStepsProcessed_(0)
        , totalActionsTaken_(0)
    {
        initializeAdvancedComponents();
        setupAdvancedMotorGroups();
        setupEnhancedSensoryProcessing();
        
        NLM_LOG_INFO("AdvancedAgentBrain: Initialized with advanced neural processing capabilities");
    }
    
    ~AdvancedAgentBrain() {
        cancelAsyncProcessing();
    }
    
    // Advanced initialization
    void initializeAdvancedComponents() {
        // Initialize enhanced state tracking
        actionHistory_.reserve(ADVANCED_MOTOR_COMMANDS);
        for (size_t i = 0; i < ADVANCED_MOTOR_COMMANDS; ++i) {
            actionHistory_[static_cast<AdvancedMotorCommand>(i)] = 0.0f;
        }
        
        recentActions_.reserve(AgentBrainConstants::MAX_ACTION_HISTORY);
        rewardHistory_.reserve(AgentBrainConstants::MAX_ACTION_HISTORY);
        actionValues_.reserve(AgentBrainConstants::MAX_ACTION_HISTORY);
        
        // Initialize advanced buffers
        enhancedVisionBuffer_.resize(512, 0.0f);
        temporalSensoryBuffer_.resize(256, 0.0f);
        contextualMemoryBuffer_.resize(128, 0.0f);
        
        // Initialize neuromodulation history
        dopamineHistory_.reserve(1000);
        curiosityHistory_.reserve(1000);
        predictionErrorHistory_.reserve(1000);
        noveltyHistory_.reserve(1000);
        
        // Setup performance monitoring
        lastPerformanceReport_ = std::chrono::system_clock::now();
        totalProcessingTime_ = 0.0;
        totalStepsProcessed_ = 0;
        totalActionsTaken_ = 0;
        
        NLM_LOG_INFO("AdvancedAgentBrain: Advanced components initialized");
    }
    
    // Enhanced sensory processing with temporal and contextual integration
    void processAdvancedSensoryInput(const SensoryPercept& percept) {
        if (!brain_) return;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Enhanced vision processing with temporal smoothing
        processEnhancedVision(enhancedVisionBuffer_, percept.getVision());
        
        // Temporal sensory integration
        processTemporalSensory(temporalSensoryBuffer_, percept);
        
        // Contextual memory integration
        processContextualMemory(contextualMemoryBuffer_, percept);
        
        // Compute advanced novelty with temporal context
        computeAdvancedNovelty(enhancedVisionBuffer_, temporalSensoryBuffer_);
        
        // Update neuromodulation with enhanced signals
        updateAdvancedNeuromodulation();
        
        // Apply adaptive sensitivity
        applyAdaptiveSensitivity(enhancedVisionBuffer_);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        // Update performance metrics
        if (enableParallelProcessing_) {
            std::lock_guard<std::mutex> lock(processingMutex_);
            totalProcessingTime_ += duration.count() / 1000.0;  // Convert to milliseconds
        }
        
        totalStepsProcessed_++;
        
        // Log performance every 1000 steps
        if (totalStepsProcessed_ % 1000 == 0) {
            reportPerformance();
        }
    }
    
    // Advanced motor command processing with adaptive control
    AdvancedMotorCommand decodeAdvancedMotorCommand() {
        if (!brain_) return AdvancedMotorCommand::Wait;
        
        // Compute motor command with adaptive weighting
        auto motorCommand = computeAdaptiveMotorCommand();
        
        // Apply curiosity-based exploration
        if (enableAdvancedLearning_ && curiosityHistory_.size() > 10) {
            float avgCuriosity = computeAverage(dopamineHistory_);
            if (avgCuriosity > 0.3f) {
                motorCommand = applyAdvancedExploration(motorCommand);
            }
        }
        
        // Apply memory-based action selection
        if (enableMemoryReplay_) {
            motorCommand = applyMemoryBasedSelection(motorCommand);
        }
        
        // Update action history
        updateActionHistory(motorCommand);
        
        return motorCommand;
    }
    
    // Advanced learning algorithms
    void applyAdvancedRewardModulation(float reward, float predictedReward) {
        if (!brain_ || !enableAdvancedLearning_) return;
        
        // Compute prediction error with temporal smoothing
        float temporalPredictionError = computeTemporalPredictionError(reward, predictedReward);
        
        // Update learning rate with adaptation
        updateLearningRateAdaptation(temporalPredictionError);
        
        // Apply advanced reward modulation
        applyAdvancedDopamineSignals(temporalPredictionError);
        
        // Update action values with temporal difference learning
        updateActionValuesTemporalDifference(reward, predictedReward);
        
        // Apply memory consolidation
        consolidateMemoryWithStrategy(temporalPredictionError);
    }
    
    // Advanced development system with multiple stages
    void updateAdvancedDevelopment(double timestep) {
        if (!brain_) return;
        
        // Enhanced developmental stages
        updateAdaptiveDevelopmentalStages(timestep);
        
        // Apply structural plasticity adaptations
        if (enableMemoryReplay_) {
            applyAdaptiveStructuralPlasticity(timestep);
        }
        
        // Apply learning rate adaptations based on developmental stage
        applyDevelopmentalLearningRateAdaptation(timestep);
    }
    
    // Performance monitoring and optimization
    void reportPerformance() {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - lastPerformanceReport_);
        
        double avgProcessingTime = (totalStepsProcessed_ > 0) ? 
            (totalProcessingTime_ / totalStepsProcessed_) : 0.0;
        
        NLM_LOG_INFO("=== AdvancedAgentBrain Performance Report ===");
        NLM_LOG_INFO("Steps processed: " + std::to_string(totalStepsProcessed_));
        NLM_LOG_INFO("Average processing time per step: " + std::to_string(avgProcessingTime) + " ms");
        NLM_LOG_INFO("Total actions taken: " + std::to_string(totalActionsTaken_));
        NLM_LOG_INFO("Action diversity: " + std::to_string(computeActionDiversity()) + "/" + 
                     std::to_string(ADVANCED_MOTOR_COMMANDS));
        NLM_LOG_INFO("Memory consolidation rate: " + std::to_string(memoryConsolidationRate_));
        NLM_LOG_INFO("Learning rate: " + std::to_string(learningRate_));
        NLM_LOG_INFO("Advanced features enabled: Learning=" + std::string(enableAdvancedLearning_ ? "Yes" : "No") +
                     ", AdaptiveThreshold=" + std::string(enableAdaptiveThreshold_ ? "Yes" : "No") +
                     ", MemoryReplay=" + std::string(enableMemoryReplay_ ? "Yes" : "No") +
                     ", Parallel=" + std::string(enableParallelProcessing_ ? "Yes" : "No"));
        
        // Reset performance counters
        lastPerformanceReport_ = std::chrono::system_clock::now();
        totalProcessingTime_ = 0.0;
    }
    
    // Configuration management
    void setAdvancedConfig(const std::string& key, const ConfigValue& value) {
        advancedConfig_[key] = value;
        NLM_LOG_INFO("AdvancedAgentBrain: Set advanced config " + key + " = " + formatValue(value));
        
        // Apply configuration changes
        applyConfigurationChanges(key, value);
    }
    
    ConfigValue getAdvancedConfig(const std::string& key) const {
        auto it = advancedConfig_.find(key);
        return (it != advancedConfig_.end()) ? it->second : ConfigValue();
    }
    
    // Advanced debugging and analysis
    void performAdvancedAnalysis() {
        NLM_LOG_INFO("=== AdvancedAgentBrain Analysis ===");
        
        // Analyze action patterns
        analyzeActionPatterns();
        
        // Analyze reward prediction errors
        analyzePredictionErrors();
        
        // Analyze memory consolidation effectiveness
        analyzeMemoryConsolidation();
        
        // Analyze neuromodulation patterns
        analyzeNeuromodulationPatterns();
        
        // Generate performance recommendations
        generatePerformanceRecommendations();
    }
    
    // Parallel processing for enhanced performance
    void enableParallelProcessing() {
        if (!enableParallelProcessing_) return;
        
        std::lock_guard<std::mutex> lock(processingMutex_);
        
        // Create async processing task
        asyncProcessingFuture_ = std::async(std::launch::async, [this]() {
            performAsyncProcessing();
        });
        
        NLM_LOG_INFO("AdvancedAgentBrain: Enabled parallel processing");
    }
    
    // Cleanup
    void shutdown() {
        cancelAsyncProcessing();
        
        // Final performance report
        reportPerformance();
        
        NLM_LOG_INFO("AdvancedAgentBrain: Shutdown complete");
    }
    
private:
    // Helper methods for advanced functionality
    void setupAdvancedMotorGroups() {
        // Setup advanced motor neuron groups for fine-grained control
        for (size_t i = 0; i < ADVANCED_MOTOR_COMMANDS; ++i) {
            // Initialize adaptive motor groups
            if (i < 6) {
                // Standard motor commands
                advancedMotorGroups_[i].reserve(20);
            } else {
                // Advanced motor commands
                advancedMotorGroups_[i].reserve(10);
            }
        }
    }
    
    void setupEnhancedSensoryProcessing() {
        // Initialize enhanced sensory processing buffers and algorithms
        enhancedVisionBuffer_.assign(512, 0.0f);
        temporalSensoryBuffer_.assign(256, 0.0f);
        contextualMemoryBuffer_.assign(128, 0.0f);
        
        NLM_LOG_INFO("AdvancedAgentBrain: Enhanced sensory processing initialized");
    }
    
    void processEnhancedVision(std::vector<float>& buffer, const std::vector<float>& vision) {
        if (vision.empty()) return;
        
        // Apply adaptive sensitivity and smoothing
        float sensitivity = adaptiveSensitivity_;
        if (processingMode_ == NeuralProcessingMode::Adaptive) {
            sensitivity *= (1.0f + thresholdAdjustment_);
        }
        
        // Temporal smoothing
        for (size_t i = 0; i < std::min(buffer.size(), vision.size()); ++i) {
            buffer[i] = (buffer[i] * 0.8f) + (vision[i] * sensitivity * 0.2f);
        }
        
        // Apply noise for stochastic processing
        if (processingMode_ == NeuralProcessingMode::Stochastic) {
            for (size_t i = 0; i < buffer.size(); ++i) {
                buffer[i] += ((std::rand() % 200 - 100) / 10000.0f) * noiseLevel_;
            }
        }
    }
    
    void processTemporalSensory(std::vector<float>& buffer, const SensoryPercept& percept) {
        // Integrate sensory inputs over time
        buffer.insert(buffer.begin(), percept.getVision().begin(), percept.getVision().end());
        
        // Apply temporal decay
        for (float& value : buffer) {
            value *= 0.95f;
        }
        
        // Keep buffer size manageable
        if (buffer.size() > 256) {
            buffer.erase(buffer.begin() + 256, buffer.end());
        }
    }
    
    void processContextualMemory(std::vector<float>& buffer, const SensoryPercept& percept) {
        // Integrate with contextual memory for pattern recognition
        float contextualWeight = 0.3f;
        
        for (size_t i = 0; i < std::min(buffer.size(), percept.getVision().size()); ++i) {
            float contextualValue = percept.getVision()[i] * contextualWeight;
            buffer[i] = (buffer[i] * 0.9f) + contextualValue * 0.1f;
        }
    }
    
    void computeAdvancedNovelty(const std::vector<float>& visionBuffer, 
                               const std::vector<float>& temporalBuffer) {
        float totalNovelty = 0.0f;
        size_t validElements = 0;
        
        for (size_t i = 0; i < visionBuffer.size(); ++i) {
            if (i < temporalBuffer.size()) {
                float diff = std::abs(visionBuffer[i] - temporalBuffer[i]);
                totalNovelty += diff;
                validElements++;
            }
        }
        
        float novelty = (validElements > 0) ? (totalNovelty / validElements) : 0.0f;
        noveltyHistory_.push_back(novelty);
        if (noveltyHistory_.size() > 100) {
            noveltyHistory_.erase(noveltyHistory_.begin());
        }
        
        NLM_LOG_DEBUG("AdvancedAgentBrain: Novelty level = " + std::to_string(novelty));
    }
    
    void updateAdvancedNeuromodulation() {
        // Update advanced neuromodulation with enhanced sensitivity
        float novelty = noveltyHistory_.empty() ? 0.0f : noveltyHistory_.back();
        float predictionError = predictionErrorHistory_.empty() ? 0.0f : predictionErrorHistory_.back();
        
        // Enhanced dopamine signaling
        float dopamine = (novelty * neuromodulationSensitivity_) + (predictionError * 0.5f);
        dopamine = std::clamp(dopamine, -1.0f, 1.0f);
        dopamineHistory_.push_back(dopamine);
        if (dopamineHistory_.size() > 100) {
            dopamineHistory_.erase(dopamineHistory_.begin());
        }
        
        // Enhanced curiosity signaling
        float curiosity = (novelty * 1.5f) + (std::abs(predictionError) * 0.8f);
        curiosity = std::clamp(curiosity, 0.0f, 1.0f);
        curiosityHistory_.push_back(curiosity);
        if (curiosityHistory_.size() > 100) {
            curiosityHistory_.erase(curiosityHistory_.begin());
        }
    }
    
    void applyAdaptiveSensitivity(std::vector<float>& buffer) {
        if (!enableAdaptiveThreshold_) return;
        
        // Adapt sensitivity based on recent novelty
        float avgNovelty = computeAverage(noveltyHistory_);
        adaptiveSensitivity_ = AgentBrainConstants::ADAPTIVE_SENSITIVITY * (1.0f + avgNovelty * 0.5f);
        
        // Apply to buffer
        for (float& value : buffer) {
            value *= adaptiveSensitivity_;
        }
    }
    
    AdvancedMotorCommand computeAdaptiveMotorCommand() {
        // Compute motor command using advanced neural activity patterns
        std::vector<std::pair<AdvancedMotorCommand, float>> commandScores;
        
        // Get motor neuron activities
        for (size_t i = 0; i < ADVANCED_MOTOR_COMMANDS; ++i) {
            if (!advancedMotorGroups_[i].empty()) {
                float activity = computeAverageMotorActivity(advancedMotorGroups_[i]);
                commandScores.push_back({static_cast<AdvancedMotorCommand>(i), activity});
            }
        }
        
        // Sort by activity score
        std::sort(commandScores.begin(), commandScores.end(),
                  [](const std::pair<AdvancedMotorCommand, float>& a,
                     const std::pair<AdvancedMotorCommand, float>& b) {
                      return a.second > b.second;
                  });
        
        // Apply action value modulation
        if (!actionValues_.empty()) {
            float avgActionValue = computeAverage(actionValues_);
            for (auto& pair : commandScores) {
                pair.second += (pair.second - avgActionValue) * 0.2f;
            }
        }
        
        // Return highest scoring command
        if (!commandScores.empty() && commandScores[0].second > 0.1f) {
            return commandScores[0].first;
        }
        
        return AdvancedMotorCommand::Wait;
    }
    
    AdvancedMotorCommand applyAdvancedExploration(AdvancedMotorCommand defaultCmd) {
        // Advanced exploration with increasing randomness
        float exploreChance = curiosityHistory_.empty() ? 0.1f : curiosityHistory_.back() * 0.5f;
        exploreChance = std::clamp(exploreChance, 0.0f, 1.0f);
        
        float r = (std::rand() % 1000) / 1000.0f;
        if (r < exploreChance) {
            // Random advanced motor command
            int choice = std::rand() % ADVANCED_MOTOR_COMMANDS;
            return static_cast<AdvancedMotorCommand>(choice);
        }
        
        return defaultCmd;
    }
    
    AdvancedMotorCommand applyMemoryBasedSelection(AdvancedMotorCommand defaultCmd) {
        // Apply memory-based action selection
        if (actionHistory_.empty()) return defaultCmd;
        
        // Find most frequently used command
        AdvancedMotorCommand mostFrequentCmd = AdvancedMotorCommand::Wait;
        float maxFrequency = 0.0f;
        
        for (const auto& pair : actionHistory_) {
            if (pair.second > maxFrequency) {
                maxFrequency = pair.second;
                mostFrequentCmd = static_cast<AdvancedMotorCommand>((&pair - &actionHistory_.front()));
            }
        }
        
        // Occasionally select less frequent commands for exploration
        float explorationBonus = (std::rand() % 100) / 100.0f;
        if (explorationBonus > 0.7f) {
            // Select a less frequent command
            std::vector<AdvancedMotorCommand> alternatives;
            for (const auto& pair : actionHistory_) {
                if (static_cast<AdvancedMotorCommand>((&pair - &actionHistory_.front())) != mostFrequentCmd &&
                    pair.second < maxFrequency * 0.5f) {
                    alternatives.push_back(static_cast<AdvancedMotorCommand>((&pair - &actionHistory_.front())));
                }
            }
            
            if (!alternatives.empty()) {
                return alternatives[std::rand() % alternatives.size()];
            }
        }
        
        return defaultCmd;
    }
    
    void updateActionHistory(AdvancedMotorCommand command) {
        // Update action frequency history
        size_t cmdIndex = static_cast<size_t>(command);
        if (cmdIndex < ADVANCED_MOTOR_COMMANDS) {
            actionHistory_[command] += 1.0f;
            
            // Maintain recent actions
            recentActions_.push_back(static_cast<MotorCommand>(command));
            if (recentActions_.size() > AgentBrainConstants::MAX_ACTION_HISTORY) {
                recentActions_.erase(recentActions_.begin());
            }
            
            totalActionsTaken_++;
        }
    }
    
    float computeAverageMotorActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Neuron* neuron : neurons) {
            const auto& state = neuron->getState();
            totalActivity += std::abs(state.membranePotential - state.restingPotential);
        }
        
        return totalActivity / neurons.size();
    }
    
    float computeTemporalPredictionError(float reward, float predictedReward) {
        // Temporal difference error with smoothing
        static float lastPredictionError = 0.0f;
        float predictionError = reward - predictedReward;
        
        // Apply temporal smoothing
        predictionError = (predictionError + lastPredictionError * 0.5f) * 0.5f;
        lastPredictionError = predictionError;
        
        predictionErrorHistory_.push_back(predictionError);
        if (predictionErrorHistory_.size() > 100) {
            predictionErrorHistory_.erase(predictionErrorHistory_.begin());
        }
        
        return predictionError;
    }
    
    void updateLearningRateAdaptation(float predictionError) {
        // Adapt learning rate based on prediction error
        if (std::abs(predictionError) > 0.5f) {
            learningRate_ = std::min(learningRate_ * 1.1f, 1.0f);
        } else {
            learningRate_ = std::max(learningRate_ * 0.95f, 0.01f);
        }
        
        learningRate_ = std::clamp(learningRate_, 0.01f, 1.0f);
    }
    
    void applyAdvancedDopamineSignals(float predictionError) {
        // Apply enhanced dopamine signaling
        float dopamine = predictionError * neuromodulationSensitivity_;
        dopamine = std::clamp(dopamine, -1.0f, 1.0f);
        dopamineHistory_.push_back(dopamine);
        if (dopamineHistory_.size() > 100) {
            dopamineHistory_.erase(dopamineHistory_.begin());
        }
    }
    
    void updateActionValuesTemporalDifference(float reward, float predictedReward) {
        // Update action values using temporal difference learning
        float tdError = reward - predictedReward;
        
        // Apply TD update to action values
        if (!actionValues_.empty()) {
            for (float& value : actionValues_) {
                value += learningRate_ * tdError;
            }
        }
        
        actionValues_.push_back(0.0f);  // Initialize new action value
        if (actionValues_.size() > AgentBrainConstants::MAX_ACTION_HISTORY) {
            actionValues_.erase(actionValues_.begin());
        }
    }
    
    void consolidateMemoryWithStrategy(float predictionError) {
        // Memory consolidation based on strategy
        switch (consolidationStrategy_) {
            case MemoryConsolidationStrategy::Simple:
                simpleConsolidation(predictionError);
                break;
            case MemoryConsolidationStrategy::Priority:
                priorityConsolidation(predictionError);
                break;
            case MemoryConsolidationStrategy::ReplayBased:
                replayBasedConsolidation(predictionError);
                break;
            case MemoryConsolidationStrategy::TimeBased:
                timeBasedConsolidation(predictionError);
                break;
            case MemoryConsolidationStrategy::StressBased:
                stressBasedConsolidation(predictionError);
                break;
        }
    }
    
    void simpleConsolidation(float predictionError) {
        // Simple consolidation based on prediction error
        if (std::abs(predictionError) > AgentBrainConstants::CONSOLIDATION_THRESHOLD) {
            memoryConsolidationRate_ = std::min(memoryConsolidationRate_ * 1.2f, 1.0f);
        } else {
            memoryConsolidationRate_ = std::max(memoryConsolidationRate_ * 0.9f, 0.1f);
        }
    }
    
    void priorityConsolidation(float predictionError) {
        // Priority-based consolidation favoring high prediction errors
        if (std::abs(predictionError) > 0.3f) {
            memoryConsolidationRate_ = std::min(memoryConsolidationRate_ * 1.3f, 1.0f);
        }
    }
    
    void replayBasedConsolidation(float predictionError) {
        // Memory replay-based consolidation
        if (predictionErrorHistory_.size() >= 10) {
            float avgPredictionError = computeAverage(predictionErrorHistory_);
            if (std::abs(avgPredictionError) > 0.2f) {
                memoryConsolidationRate_ = std::min(memoryConsolidationRate_ * 1.1f, 1.0f);
            }
        }
    }
    
    void timeBasedConsolidation(float predictionError) {
        // Time-based consolidation with decay
        memoryConsolidationRate_ *= 0.99f;
    }
    
    void stressBasedConsolidation(float predictionError) {
        // Stress-based consolidation (high prediction error = high consolidation)
        float stressLevel = std::min(std::abs(predictionError) * 2.0f, 1.0f);
        memoryConsolidationRate_ = std::min(memoryConsolidationRate_ * (1.0f + stressLevel), 1.0f);
    }
    
    void updateAdaptiveDevelopmentalStages(double timestep) {
        // Enhanced developmental stages with adaptive plasticity
        if (developmentalAge_ < 60.0) {  // Initial stage
            plasticityModifier_ = 1.0f;
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        } else if (developmentalAge_ < 300.0) {  // Critical period
            plasticityModifier_ = 0.8f * (1.0f + thresholdAdjustment_);
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (developmentalAge_ < 900.0) {  // Maturation
            plasticityModifier_ = 0.5f * (1.0f + memoryConsolidationRate_);
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
        } else {  // Adult
            plasticityModifier_ = 0.2f * (1.0f + neuromodulationSensitivity_);
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    void applyAdaptiveStructuralPlasticity(double timestep) {
        // Apply adaptive structural plasticity
        if (structuralPlasticityEnabled_) {
            auto* sp = brain_->getStructuralPlasticity();
            if (sp) {
                // Higher synaptogenesis in early development
                float synRate = 0.0001f * plasticityModifier_ * (1.0f + memoryConsolidationRate_);
                float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
                sp->setSynaptogenesisRate(synRate);
                sp->setPruningRate(pruneRate);
            }
        }
    }
    
    void applyDevelopmentalLearningRateAdaptation(double timestep) {
        // Adjust learning rate based on developmental stage
        DevelopmentalStage stage = brain_->getDevelopmentalStage();
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                learningRate_ = 0.2f;  // High learning rate in initial stage
                break;
            case DevelopmentalStage::CriticalPeriod:
                learningRate_ = 0.15f;  // Moderate learning rate
                break;
            case DevelopmentalStage::Maturation:
                learningRate_ = 0.1f;   // Lower learning rate
                break;
            case DevelopmentalStage::Adult:
                learningRate_ = 0.05f;  // Minimal learning in adult stage
                break;
        }
    }
    
    float computeActionDiversity() const {
        // Compute action diversity (0.0 to 1.0)
        if (recentActions_.empty()) return 0.0f;
        
        std::unordered_map<MotorCommand, int> actionCounts;
        for (MotorCommand action : recentActions_) {
            actionCounts[action]++;
        }
        
        float maxCount = 0.0f;
        for (const auto& pair : actionCounts) {
            maxCount = std::max(maxCount, static_cast<float>(pair.second));
        }
        
        return 1.0f - (maxCount / recentActions_.size());  // Lower diversity if one action dominates
    }
    
    void analyzeActionPatterns() {
        // Analyze action patterns for debugging
        if (recentActions_.empty()) return;
        
        std::unordered_map<MotorCommand, int> actionCounts;
        for (MotorCommand action : recentActions_) {
            actionCounts[action]++;
        }
        
        NLM_LOG_INFO("AdvancedAgentBrain: Action pattern analysis:");
        for (const auto& pair : actionCounts) {
            float percentage = (static_cast<float>(pair.second) / recentActions_.size()) * 100.0f;
            NLM_LOG_INFO("  " + std::to_string(pair.first) + ": " + std::to_string(percentage) + "%")
        }
    }
    
    void analyzePredictionErrors() {
        // Analyze prediction errors for learning assessment
        if (predictionErrorHistory_.empty()) return;
        
        float avgPredictionError = computeAverage(predictionErrorHistory_);
        float maxPredictionError = computeMax(predictionErrorHistory_);
        
        NLM_LOG_INFO("AdvancedAgentBrain: Prediction error analysis:");
        NLM_LOG_INFO("  Average: " + std::to_string(avgPredictionError));
        NLM_LOG_INFO("  Maximum: " + std::to_string(maxPredictionError));
        NLM_LOG_INFO("  Standard deviation: " + std::to_string(computeStandardDeviation(predictionErrorHistory_)));
    }
    
    void analyzeMemoryConsolidation() {
        // Analyze memory consolidation effectiveness
        NLM_LOG_INFO("AdvancedAgentBrain: Memory consolidation analysis:");
        NLM_LOG_INFO("  Current consolidation rate: " + std::to_string(memoryConsolidationRate_));
        NLM_LOG_INFO("  Action history size: " + std::to_string(recentActions_.size()));
        NLM_LOG_INFO("  Reward history size: " + std::to_string(rewardHistory_.size()));
    }
    
    void analyzeNeuromodulationPatterns() {
        // Analyze neuromodulation patterns
        if (dopamineHistory_.empty() || curiosityHistory_.empty()) return;
        
        float avgDopamine = computeAverage(dopamineHistory_);
        float avgCuriosity = computeAverage(curiosityHistory_);
        
        NLM_LOG_INFO("AdvancedAgentBrain: Neuromodulation analysis:");
        NLM_LOG_INFO("  Average dopamine: " + std::to_string(avgDopamine));
        NLM_LOG_INFO("  Average curiosity: " + std::to_string(avgCuriosity));
        NLM_LOG_INFO("  Neuromodulation sensitivity: " + std::to_string(neuromodulationSensitivity_));
    }
    
    void generatePerformanceRecommendations() {
        // Generate performance recommendations based on analysis
        NLM_LOG_INFO("=== AdvancedAgentBrain Performance Recommendations ===");
        
        if (computeActionDiversity() < 0.3f) {
            NLM_LOG_INFO("Recommendation: Increase behavioral diversity for better exploration");
        }
        
        if (learningRate_ < 0.05f) {
            NLM_LOG_INFO("Recommendation: Adjust learning rate higher for faster adaptation");
        }
        
        if (memoryConsolidationRate_ > 0.9f) {
            NLM_LOG_INFO("Recommendation: Reduce memory consolidation to prevent overfitting");
        }
        
        if (std::abs(thresholdAdjustment_) > 0.5f) {
            NLM_LOG_INFO("Recommendation: Adjust threshold adaptation for more stable learning");
        }
    }
    
    void performAsyncProcessing() {
        // Perform asynchronous processing tasks
        NLM_LOG_INFO("AdvancedAgentBrain: Starting asynchronous processing");
        
        // Simulate async processing tasks
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Update some internal state
        for (float& value : enhancedVisionBuffer_) {
            value *= 0.99f;  // Slow decay
        }
        
        NLM_LOG_INFO("AdvancedAgentBrain: Asynchronous processing completed");
    }
    
    void cancelAsyncProcessing() {
        if (asyncProcessingFuture_.valid()) {
            asyncProcessingFuture_.wait();
        }
    }
    
    // Configuration change application
    void applyConfigurationChanges(const std::string& key, const ConfigValue& value) {
        if (key == "processing_mode") {
            if (std::holds_alternative<std::string>(value)) {
                std::string mode = std::get<std::string>(value);
                if (mode == "adaptive") {
                    processingMode_ = NeuralProcessingMode::Adaptive;
                } else if (mode == "burst") {
                    processingMode_ = NeuralProcessingMode::Burst;
                } else if (mode == "stochastic") {
                    processingMode_ = NeuralProcessingMode::Stochastic;
                }
            }
        } else if (key == "consolidation_strategy") {
            if (std::holds_alternative<std::string>(value)) {
                std::string strategy = std::get<std::string>(value);
                if (strategy == "priority") {
                    consolidationStrategy_ = MemoryConsolidationStrategy::Priority;
                } else if (strategy == "replay") {
                    consolidationStrategy_ = MemoryConsolidationStrategy::ReplayBased;
                }
            }
        } else if (key == "enable_advanced_learning") {
            if (std::holds_alternative<bool>(value)) {
                enableAdvancedLearning_ = std::get<bool>(value);
            }
        } else if (key == "enable_adaptive_threshold") {
            if (std::holds_alternative<bool>(value)) {
                enableAdaptiveThreshold_ = std::get<bool>(value);
            }
        } else if (key == "enable_memory_replay") {
            if (std::holds_alternative<bool>(value)) {
                enableMemoryReplay_ = std::get<bool>(value);
            }
        }
    }
    
    // Utility functions
    template<typename T>
    T computeAverage(const std::vector<T>& values) {
        if (values.empty()) return T(0);
        
        T sum = T(0);
        for (const T& value : values) {
            sum += value;
        }
        
        return sum / values.size();
    }
    
    template<typename T>
    T computeMax(const std::vector<T>& values) {
        if (values.empty()) return T(0);
        
        T max = values[0];
        for (const T& value : values) {
            if (value > max) max = value;
        }
        
        return max;
    }
    
    template<typename T>
    T computeStandardDeviation(const std::vector<T>& values) {
        if (values.size() < 2) return T(0);
        
        T mean = computeAverage(values);
        T variance = T(0);
        
        for (const T& value : values) {
            T diff = value - mean;
            variance += diff * diff;
        }
        
        return std::sqrt(variance / values.size());
    }
    
    std::string formatValue(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
}; // class AdvancedAgentBrain

// Helper function to extract MotorCommand from AdvancedMotorCommand
MotorCommand extractMotorCommand(AdvancedMotorCommand advancedCmd) {
    // Convert advanced motor command to standard motor command
    // For now, use a simple mapping - in a real implementation, this would be more sophisticated
    switch (advancedCmd) {
        case AdvancedMotorCommand::MoveForward: return MotorCommand::MoveForward;
        case AdvancedMotorCommand::MoveBackward: return MotorCommand::MoveBackward;
        case AdvancedMotorCommand::TurnLeft: return MotorCommand::TurnLeft;
        case AdvancedMotorCommand::TurnRight: return MotorCommand::TurnRight;
        case AdvancedMotorCommand::LookLeft: return MotorCommand::LookLeft;
        case AdvancedMotorCommand::LookRight: return MotorCommand::LookRight;
        case AdvancedMotorCommand::Interact: return MotorCommand::Interact;
        case AdvancedMotorCommand::Wait: return MotorCommand::Wait;
        case AdvancedMotorCommand::Accelerate: return MotorCommand::MoveForward;
        case AdvancedMotorCommand::Decelerate: return MotorCommand::Wait;
        case AdvancedMotorCommand::RotateLeftFine: return MotorCommand::TurnLeft;
        case AdvancedMotorCommand::RotateRightFine: return MotorCommand::TurnRight;
        case AdvancedMotorCommand::ScanArea: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

// Enhanced AgentBrain implementation using AdvancedAgentBrain
class EnhancedAgentBrain {
private:
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<SimpleWorld> world_;
    std::unique_ptr<AdvancedAgentBrain> advancedBrain_;
    bool useAdvancedFeatures_;
    
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain, std::unique_ptr<SimpleWorld> world)
        : brain_(brain), world_(std::move(world)), useAdvancedFeatures_(false) {
        // Initialize with advanced brain capabilities
        advancedBrain_ = std::make_unique<AdvancedAgentBrain>(brain, std::move(world_));
        
        NLM_LOG_INFO("EnhancedAgentBrain: Initialized with advanced neural processing capabilities");
    }
    
    void initialize(const SimpleWorld& world) {
        if (advancedBrain_) {
            advancedBrain_->initialize(world);
            enableAdvancedFeatures();
        }
    }
    
    void enableAdvancedFeatures() {
        useAdvancedFeatures_ = true;
        NLM_LOG_INFO("EnhancedAgentBrain: Advanced features enabled");
    }
    
    void disableAdvancedFeatures() {
        useAdvancedFeatures_ = false;
        NLM_LOG_INFO("EnhancedAgentBrain: Advanced features disabled");
    }
    
    bool areAdvancedFeaturesEnabled() const {
        return useAdvancedFeatures_;
    }
    
    // Enhanced sensory processing with advanced features
    void processSensoryInput(const SensoryPercept& percept) {
        if (advancedBrain_) {
            advancedBrain_->processAdvancedSensoryInput(percept);
        } else {
            // Fallback to standard processing
            // (Implementation would be in original AgentBrain.cpp)
        }
    }
    
    // Enhanced motor command processing
    MotorCommand decodeMotorCommand() {
        if (advancedBrain_ && useAdvancedFeatures_) {
            AdvancedMotorCommand advancedCmd = advancedBrain_->decodeAdvancedMotorCommand();
            return extractMotorCommand(advancedCmd);
        }
        // Fallback to standard processing
        // (Implementation would be in original AgentBrain.cpp)
        return MotorCommand::Wait;
    }
    
    // Enhanced reward modulation
    void applyRewardModulation(float reward, float predictedReward) {
        if (advancedBrain_) {
            advancedBrain_->applyAdvancedRewardModulation(reward, predictedReward);
        } else {
            // Fallback to standard processing
            // (Implementation would be in original AgentBrain.cpp)
        }
    }
    
    // Enhanced development updates
    void updateDevelopment(double timestep) {
        if (advancedBrain_) {
            advancedBrain_->updateAdvancedDevelopment(timestep);
        } else {
            // Fallback to standard processing
            // (Implementation would be in original AgentBrain.cpp)
        }
    }
    
    // Advanced configuration management
    void setConfig(const std::string& key, const ConfigValue& value) {
        if (advancedBrain_) {
            advancedBrain_->setAdvancedConfig(key, value);
        }
    }
    
    ConfigValue getConfig(const std::string& key) const {
        if (advancedBrain_) {
            return advancedBrain_->getAdvancedConfig(key);
        }
        return ConfigValue();
    }
    
    // Performance monitoring
    void reportPerformance() {
        if (advancedBrain_) {
            advancedBrain_->reportPerformance();
        }
    }
    
    // Advanced analysis
    void performAdvancedAnalysis() {
        if (advancedBrain_) {
            advancedBrain_->performAdvancedAnalysis();
        }
    }
    
    // Parallel processing
    void enableParallelProcessing() {
        if (advancedBrain_) {
            advancedBrain_->enableParallelProcessing();
        }
    }
    
    // Cleanup
    void shutdown() {
        if (advancedBrain_) {
            advancedBrain_->shutdown();
        }
    }
}; // class EnhancedAgentBrain

// Global function to create enhanced agent brain
EnhancedAgentBrain* createEnhancedAgentBrain(std::shared_ptr<Brain> brain, std::unique_ptr<SimpleWorld> world) {
    return new EnhancedAgentBrain(brain, std::move(world));
}

// Global function to delete enhanced agent brain
void deleteEnhancedAgentBrain(EnhancedAgentBrain* enhancedBrain) {
    delete enhancedBrain;
}

// Global function to enable advanced features
void enableAdvancedAgentBrainFeatures(EnhancedAgentBrain* enhancedBrain) {
    if (enhancedBrain) {
        enhancedBrain->enableAdvancedFeatures();
    }
}

// Global function to disable advanced features
void disableAdvancedAgentBrainFeatures(EnhancedAgentBrain* enhancedBrain) {
    if (enhancedBrain) {
        enhancedBrain->disableAdvancedFeatures();
    }
}

// Global function to set advanced configuration
void setAdvancedAgentBrainConfig(EnhancedAgentBrain* enhancedBrain, const std::string& key, const ConfigValue& value) {
    if (enhancedBrain) {
        enhancedBrain->setConfig(key, value);
    }
}

// Global function to get advanced configuration
ConfigValue getAdvancedAgentBrainConfig(EnhancedAgentBrain* enhancedBrain, const std::string& key) {
    if (enhancedBrain) {
        return enhancedBrain->getConfig(key);
    }
    return ConfigValue();
}

// Global function to perform advanced analysis
void performAdvancedAgentBrainAnalysis(EnhancedAgentBrain* enhancedBrain) {
    if (enhancedBrain) {
        enhancedBrain->performAdvancedAnalysis();
    }
}

// Global function to enable parallel processing
void enableAdvancedAgentBrainParallelProcessing(EnhancedAgentBrain* enhancedBrain) {
    if (enhancedBrain) {
        enhancedBrain->enableParallelProcessing();
    }
}

// Global function to shutdown advanced agent brain
void shutdownAdvancedAgentBrain(EnhancedAgentBrain* enhancedBrain) {
    if (enhancedBrain) {
        enhancedBrain->shutdown();
    }
}

} // namespace nlm
