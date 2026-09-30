#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>

namespace nlm {

// Enhanced AgentBrain: Connects NLM brain to the world with improved design
// Handles sensory transduction, motor decoding, and neuromodulation integration
// More maintainable, configurable, and extensible design

class AgentBrain {
public:
    // Enhanced constructor with dependency injection and configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, 
                      std::shared_ptr<Config> config = nullptr);
    ~AgentBrain();

    // Delete copy operations, enable move
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;

    // Initialize with world and configuration
    bool initialize(const SimpleWorld& world);

    // Enhanced sensory processing with better error handling
    bool processSensoryInput(const SensoryPercept& percept);

    // Enhanced motor decoding with better activity analysis
    MotorCommand decodeMotorCommand();

    // Advanced neuromodulation with eligibility traces
    void applyRewardModulation(float reward, float predictedReward = 0.0f);

    // Development system updates with age validation
    void updateDevelopment(double timestep);

    // Enhanced statistics and diagnostics
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;

    // Reset with more comprehensive state clearing
    void reset();

    // Configuration management
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

    // Enhanced configuration setters
    void setLearningRate(float rate) { learningRate_ = std::max(0.0f, std::min(1.0f, rate)); }
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = std::max(0.0f, std::min(1.0f, threshold)); }
    void setExplorationRate(float rate) { explorationRate_ = std::max(0.0f, std::min(1.0f, rate)); }

    // Performance monitoring and diagnostics
    struct PerformanceStats {
        double totalSensoryProcessingTime = 0.0;
        double totalMotorDecodingTime = 0.0;
        size_t sensoryProcessingCount = 0;
        size_t motorDecodingCount = 0;
        float averageActivityLevel = 0.0f;
        size_t novelEventsDetected = 0;
    };

    PerformanceStats getPerformanceStats() const { return stats_; }
    void resetPerformanceStats() { stats_ = PerformanceStats(); }

    // Get brain pointer for direct access (when needed)
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }

    // Export/import brain state for persistence
    bool saveAgentState(const std::string& filename) const;
    bool loadAgentState(const std::string& filename);

    // Health check and validation
    bool isHealthy() const;
    std::string getHealthReport() const;

private:
    // Helper structures for enhanced processing
    struct NeuronGroup {
        NeuronType type;
        std::vector<Neuron*> neurons;
        float sensitivity = 1.0f;
        float threshold = 0.0f;
        float gain = 1.0f;
        
        NeuronGroup(NeuronType t, float sens = 1.0f, float thresh = 0.0f, float g = 1.0f)
            : type(t), sensitivity(sens), threshold(thresh), gain(g) {}
    };

    struct SensoryMapping {
        size_t perceptIndex;
        size_t neuronGroupIndex;
        float scalingFactor;
        float offset;
        bool enabled;
    };

    struct MotorMapping {
        MotorCommand command;
        size_t neuronGroupIndex;
        float activationThreshold;
        float confidence;
    };

    // Enhanced neuron group management
    void initializeNeuronGroups();
    void distributeNeuronsByType();
    void optimizeNeuronGroupAssignments();

    // Enhanced motor decoding with weighted voting
    MotorCommand decodeFromMotorNeuronsWeighted();

    // Enhanced curiosity-based exploration
    MotorCommand selectWithCuriosityAdvanced(MotorCommand defaultCmd);

    // Novelty detection with temporal filtering
    void updateNoveltyDetection(const std::vector<float>& currentVision);
    float computeNovelty(const std::vector<float>& currentVision) const;

    // Enhanced neuromodulation with eligibility traces
    void applyAdvancedRewardModulation(float reward, float predictedReward);

    // Performance monitoring
    void updatePerformanceStats(double processingTime, size_t operationType);

    // Configuration
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;

    // Enhanced neuron group management
    std::vector<NeuronGroup> sensoryGroups_;
    std::vector<NeuronGroup> motorGroups_;
    std::vector<SensoryMapping> sensoryMappings_;
    std::vector<MotorMapping> motorMappings_;

    // Neuromodulation state with better management
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    float learningRate_;
    float curiosityThreshold_;
    float explorationRate_;

    // Development state with more detailed tracking
    double developmentalAge_;
    float plasticityModifier_;
    float developmentalPlasticityPeak_;
    size_t developmentStageDuration_;

    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool noveltyDetectionEnabled_;

    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    float temporalNoveltyWeight_;

    // Performance monitoring
    PerformanceStats stats_;

    // Statistics for diagnostics
    struct DiagnosticStats {
        size_t totalSensoryInputs = 0;
        size_t totalMotorOutputs = 0;
        size_t successfulDecodings = 0;
        float averageMotorConfidence = 0.0f;
        float peakActivityLevel = 0.0f;
        size_t adaptationEvents = 0;
    };
    DiagnosticStats diagnosticStats_;

    // Safety and validation
    bool validateSensoryInput(const SensoryPercept& percept) const;
    bool validateNeuronGroups() const;
    void cleanupInvalidNeurons();

    // Enhanced logging with performance metrics
    static void logWithContext(const std::string& message, LogLevel level = LogLevel::Info,
                              const std::string& context = "");
};

} // namespace nlm