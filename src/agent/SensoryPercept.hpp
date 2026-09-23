#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <sstream>

namespace nlm {

// SensoryPercept - Enhanced sensory data packet with advanced processing
// Provides flexible signal handling, preprocessing, and analysis capabilities
class SensoryPercept {
public:
    // Enhanced constructor with configurable dimensions
    SensoryPercept(size_t visionWidth = 16, size_t visionHeight = 16,
                  size_t touchChannels = 8, size_t internalChannels = 4,
                  size_t proprioceptionChannels = 6, size_t audioChannels = 0);
    ~SensoryPercept() = default;
    
    // Disable copying, enable moving
    SensoryPercept(const SensoryPercept&) = delete;
    SensoryPercept& operator=(const SensoryPercept&) = delete;
    SensoryPercept(SensoryPercept&&) noexcept;
    SensoryPercept& operator=(SensoryPercept&&) noexcept;
    
    // Vision: configurable grayscale intensity grid
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v);
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    void setVisionDimensions(size_t width, size_t height);
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t);
    size_t getTouchChannels() const { return touchChannels_; }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i);
    size_t getInternalChannels() const { return internalChannels_; }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p);
    size_t getProprioceptionChannels() const { return proprioceptionChannels_; }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a);
    size_t getAudioChannels() const { return audioChannels_; }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Statistical analysis methods
    struct SignalStats {
        float mean;
        float stddev;
        float min;
        float max;
        float median;
        float variance;
        size_t size;
        bool isEmpty;
    };
    
    SignalStats getVisionStats() const;
    SignalStats getTouchStats() const;
    SignalStats getInternalStats() const;
    SignalStats getProprioceptionStats() const;
    SignalStats getAudioStats() const;
    SignalStats getSignalStats(const std::vector<float>& signal) const;
    
    // Signal processing methods
    void applySmoothing(float factor = 0.5f);
    void applyNoiseReduction(float threshold = 0.05f);
    void applyNormalization();
    void applyContrastEnhancement(float factor = 1.0f);
    void applyGaussianFilter(float sigma = 1.0f);
    void applyEdgeDetection(float threshold = 0.1f);
    void applyTemporalFiltering(double alpha = 0.1);
    
    // Advanced signal processing
    void applyBandpassFilter(float lowFreq, float highFreq, size_t sampleRate = 1000);
    void applyWaveletTransform();
    void applyPrincipalComponentAnalysis();
    void applyIndependentComponentAnalysis();
    
    // Feature extraction
    std::vector<float> extractTextureFeatures() const;
    std::vector<float> extractMotionFeatures() const;
    std::vector<float> extractColorFeatures() const;
    std::vector<float> extractShapeFeatures() const;
    std::vector<float> extractStatisticalFeatures() const;
    
    // Pattern recognition
    bool detectPattern(const std::vector<float>& pattern, float threshold = 0.8f) const;
    float calculatePatternSimilarity(const std::vector<float>& pattern) const;
    std::vector<size_t> findPatternMatches(const std::vector<float>& pattern) const;
    
    // State management
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    std::string serialize() const;
    bool deserialize(const std::string& data);
    
    // Batch processing
    static std::vector<std::shared_ptr<SensoryPercept>> batchProcess(
        const std::vector<std::shared_ptr<SensoryPercept>>& percepts,
        const std::string& operation = "average");
    
    // Comparison and analysis
    double calculateSimilarity(const SensoryPercept& other) const;
    double calculateDistance(const SensoryPercept& other) const;
    bool isEquivalent(const SensoryPercept& other, double tolerance = 0.01) const;
    
    // Configuration
    struct SensoryConfig {
        size_t visionWidth = 16;
        size_t visionHeight = 16;
        size_t touchChannels = 8;
        size_t internalChannels = 4;
        size_t proprioceptionChannels = 6;
        size_t audioChannels = 0;
        bool enableSmoothing = false;
        float smoothingFactor = 0.5f;
        bool enableNoiseReduction = false;
        float noiseReductionThreshold = 0.05f;
        bool enableNormalization = false;
        bool enableContrastEnhancement = false;
        float contrastFactor = 1.0f;
        bool enableFiltering = false;
        float filterSigma = 1.0f;
        bool enableEdgeDetection = false;
        float edgeThreshold = 0.1f;
        bool enableTemporalFiltering = false;
        double temporalAlpha = 0.1f;
        bool enableFeatureExtraction = false;
        bool enablePatternRecognition = false;
        float patternThreshold = 0.8f;
        bool enableSerialization = false;
        bool enableBatchProcessing = false;
    };
    
    const SensoryConfig& getConfig() const { return config_; }
    void updateConfig(const SensoryConfig& newConfig);
    
    // Validation and error handling
    bool validate() const;
    std::vector<std::string> getErrors() const { return errors_; }
    std::vector<std::string> getWarnings() const { return warnings_; }
    void clearDiagnosticLogs();
    
    // Performance monitoring
    struct PerformanceMetrics {
        double processingTime;
        size_t signalsProcessed;
        size_t patternsDetected;
        size_t featuresExtracted;
        double memoryUsage;
        bool isHealthy;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Debug and diagnostic tools
    bool verifyIntegrity() const;
    std::string getSystemStatus() const;
    std::vector<std::string> getDiagnosticInfo() const;
    
    // Advanced utility methods
    void augmentNoise(float intensity = 0.1f);
    void compressSignal(float threshold = 0.05f);
    void enhanceSignal(float factor = 1.0f);
    std::vector<float> applyWindowFunction(const std::string& windowType = "hamming") const;
    std::vector<float> segmentSignal(size_t segmentSize) const;
    double calculateSpectralDensity() const;
    std::vector<float> applyMelFilterbank(size_t numFilters = 40) const;
    
private:
    // Helper methods
    void validateDimensions() const;
    void logError(const std::string& error);
    void logWarning(const std::string& warning);
    void updatePerformanceMetrics() const;
    void clearDiagnosticLogsInternal();
    std::vector<float> applyGaussianKernel(const std::vector<float>& signal, float sigma) const;
    float calculateEntropy(const std::vector<float>& signal) const;
    std::vector<float> applyPCA(const std::vector<std::vector<float>>& data) const;
    
    // Core data
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    size_t touchChannels_;
    std::vector<float> internal_;
    size_t internalChannels_;
    std::vector<float> proprioception_;
    size_t proprioceptionChannels_;
    std::vector<float> audio_;
    size_t audioChannels_;
    double timestamp_;
    
    // Configuration
    SensoryConfig config_;
    
    // Performance monitoring
    mutable PerformanceMetrics perfMetrics_;
    mutable double lastPerfUpdate_;
    
    // Diagnostic logs
    mutable std::vector<std::string> errors_;
    mutable std::vector<std::string> warnings_;
    
    // State tracking
    bool dataValidated_;
    bool systemHealthy_;
    
    // Statistical caches
    mutable std::unordered_map<std::string, SignalStats> statCache_;
    mutable bool statsDirty_;
};

} // namespace nlm
