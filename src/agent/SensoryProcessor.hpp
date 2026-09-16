#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// Sensory processing configuration parameters
struct SensoryConfig {
    // Vision processing
    float visionScaleFactor_ = 5.0f;         // Scale factor for vision input
    float visionNoiseLevel_ = 0.01f;        // Vision sensor noise
    bool enableVisionFiltering_ = true;      // Enable vision filtering
    float visionSmoothing_ = 0.9f;           // Vision signal smoothing
    
    // Touch processing
    float touchScaleFactor_ = 8.0f;          // Scale factor for touch input
    float touchNoiseLevel_ = 0.02f;         // Touch sensor noise
    float touchThreshold_ = 0.1f;            // Touch detection threshold
    
    // Internal signals processing
    float internalScaleFactor_ = 5.0f;       // Scale factor for internal signals
    float internalNoiseLevel_ = 0.05f;      // Internal signal noise
    float internalSmoothing_ = 0.8f;         // Internal signal smoothing
    
    // Proprioception processing
    float proprioceptionScaleFactor_ = 3.0f; // Scale factor for proprioception
    float proprioceptionNoiseLevel_ = 0.03f; // Proprioception sensor noise
    
    // Cross-modal integration
    bool enableCrossModalIntegration_ = true; // Enable integration between modalities
    float integrationWeightVision_ = 0.4f;   // Weight for vision in integration
    float integrationWeightTouch_ = 0.3f;    // Weight for touch in integration
    float integrationWeightInternal_ = 0.2f; // Weight for internal in integration
    float integrationWeightProprioception_ = 0.1f; // Weight for proprioception
    
    // Configuration flags
    bool enableSensoryFiltering_ = true;      // Enable overall sensory filtering
    bool enableSensoryNormalization_ = true;   // Enable sensory normalization
    bool enableSensoryCompression_ = false;   // Enable sensory dynamic range compression
    bool enableTemporalFiltering_ = true;     // Enable temporal (motion) filtering
    
    // Processing strategies
    enum class ProcessingStrategy {
        None,           // No processing
        Linear,         // Simple linear scaling
        Logarithmic,    // Logarithmic scaling
        Sigmoid,        // Sigmoid function
        Adaptive       // Adaptive based on statistics
    };
    
    ProcessingStrategy visionProcessingStrategy_ = ProcessingStrategy::Linear;
    ProcessingStrategy touchProcessingStrategy_ = ProcessingStrategy::Linear;
    ProcessingStrategy internalProcessingStrategy_ = ProcessingStrategy::Linear;
    ProcessingStrategy proprioceptionProcessingStrategy_ = ProcessingStrategy::Linear;
    
    // Size configuration
    size_t visionNeurons_ = 256;             // Number of vision neurons
    size_t touchNeurons_ = 8;                // Number of touch neurons
    size_t internalNeurons_ = 4;             // Number of internal neurons
    size_t proprioceptionNeurons_ = 6;      // Number of proprioception neurons
};

// SensoryProcessor: Processes sensory input and prepares it for the brain
// Separates sensory processing logic from AgentBrain for better modularity
class SensoryProcessor {
public:
    SensoryProcessor(const SensoryConfig& config = SensoryConfig());
    ~SensoryProcessor();
    
    // Configure sensory processor
    void configure(const SensoryConfig& config) { config_ = config; }
    
    // Process sensory percept from world
    void processSensoryInput(const SensoryPercept& percept);
    
    // Get processed vision signal
    const std::vector<float>& getVisionSignal() const { return visionSignal_; }
    
    // Get processed touch signal
    const std::vector<float>& getTouchSignal() const { return touchSignal_; }
    
    // Get processed internal signal
    const std::vector<float>& getInternalSignal() const { return internalSignal_; }
    
    // Get processed proprioception signal
    const std::vector<float>& getProprioceptionSignal() const { return proprioceptionSignal_; }
    
    // Get all processed signals concatenated
    std::vector<float> getAllProcessedSignals() const;
    
    // Get all original (unprocessed) signals
    const std::vector<float>& getOriginalVision() const { return originalVision_; }
    const std::vector<float>& getOriginalTouch() const { return originalTouch_; }
    const std::vector<float>& getOriginalInternal() const { return originalInternal_; }
    const std::vector<float>& getOriginalProprioception() const { return originalProprioception_; }
    
    // Apply neuromodulatory effects to sensory processing
    void applyNeuromodulation(float dopamineLevel, float noveltyLevel);
    
    // Get novelty level (difference from previous input)
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Update sensory history for temporal processing
    void updateSensoryHistory();
    
    // Reset sensory processor
    void reset();
    
    // Validate state
    bool isValid() const;
    
    // Get configuration
    const SensoryConfig& getConfig() const { return config_; }
    
    // Configure processing strategies
    void setVisionStrategy(SensoryConfig::ProcessingStrategy strategy) { 
        config_.visionProcessingStrategy_ = strategy; 
    }
    void setTouchStrategy(SensoryConfig::ProcessingStrategy strategy) { 
        config_.touchProcessingStrategy_ = strategy; 
    }
    void setInternalStrategy(SensoryConfig::ProcessingStrategy strategy) { 
        config_.internalProcessingStrategy_ = strategy; 
    }
    void setProprioceptionStrategy(SensoryConfig::ProcessingStrategy strategy) { 
        config_.proprioceptionProcessingStrategy_ = strategy; 
    }
    
private:
    // Processing methods
    void processVision(const std::vector<float>& input);
    void processTouch(const std::vector<float>& input);
    void processInternal(const std::vector<float>& input);
    void processProprioception(const std::vector<float>& input);
    
    // Individual processing strategies
    float processLinear(float value, float scale, float offset) const;
    float processLogarithmic(float value, float scale) const;
    float processSigmoid(float value, float scale, float midpoint) const;
    float processAdaptive(float value, float scale, float& history) const;
    
    // Signal filtering
    void applyFiltering(std::vector<float>& signal, float smoothing) const;
    void applyNormalization(std::vector<float>& signal) const;
    void applyCompression(std::vector<float>& signal) const;
    void applyTemporalFiltering(std::vector<float>& signal, float decay) const;
    
    // Cross-modal integration
    void integrateModalities();
    void updateTemporalSmoothing();
    
    // Novelty detection
    void computeNovelty(const std::vector<float>& current, const std::vector<float>& previous);
    
    // State variables
    std::vector<float> originalVision_;       // Original vision input
    std::vector<float> originalTouch_;        // Original touch input
    std::vector<float> originalInternal_;     // Original internal input
    std::vector<float> originalProprioception_; // Original proprioception input
    
    std::vector<float> visionSignal_;         // Processed vision signal
    std::vector<float> touchSignal_;          // Processed touch signal
    std::vector<float> internalSignal_;       // Processed internal signal
    std::vector<float> proprioceptionSignal_; // Processed proprioception signal
    
    // History for temporal processing and novelty detection
    std::vector<float> visionHistory_;        // Previous vision signal
    std::vector<float> touchHistory_;         // Previous touch signal
    std::vector<float> internalHistory_;      // Previous internal signal
    std::vector<float> proprioceptionHistory_;// Previous proprioception signal
    
    // Neuromodulatory effects
    float noveltyLevel_;                       // Current novelty level
    float dopamineModulation_;                 // Dopamine modulation factor
    
    // Configuration
    SensoryConfig config_;
    
    // Statistics for adaptive processing
    struct ProcessingStats {
        float visionMean_ = 0.0f;
        float visionStd_ = 1.0f;
        float touchMean_ = 0.0f;
        float touchStd_ = 1.0f;
        size_t updateCount_ = 0;
    } stats_;
};

} // namespace nlm