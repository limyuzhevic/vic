#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Signal processing configuration
struct SignalProcessingConfig {
    bool enableNormalization = true;       // Normalize signals to [0, 1] or [-1, 1]
    bool enableCompression = false;        // Compress high-dimensional signals
    float compressionFactor = 0.5f;       // Signal compression ratio (0-1)
    bool enableFiltering = false;          // Apply simple filtering
    float filterAlpha = 0.3f;              // Filter smoothing factor
    bool enableFeatureExtraction = false;   // Extract features from signals
    bool enableTemporalSmoothing = false;  // Apply temporal smoothing
    float temporalSmoothingFactor = 0.7f;   // Temporal smoothing factor
    bool enableSignalValidation = true;    // Validate signal ranges
    float minValidSignal = -1.0f;          // Minimum valid signal value
    float maxValidSignal = 1.0f;           // Maximum valid signal value
    bool clampToRange = true;               // Clamp signals to valid range
    bool applyNoise = false;               // Add noise for robustness
    float noiseLevel = 0.01f;              // Noise level for signal processing
};

// Signal feature for feature extraction
struct SignalFeature {
    std::string name;                      // Feature name
    float value;                            // Feature value
    float weight;                           // Feature importance weight
    
    SignalFeature(const std::string& n, float v, float w = 1.0f) 
        : name(n), value(v), weight(w) {}
};

// High-level sensory signal processor
class SensoryProcessor {
public:
    explicit SensoryProcessor(const SignalProcessingConfig& config = SignalProcessingConfig()) 
        : config_(config) {}

    // Process vision signal with optional feature extraction
    std::vector<float> processVision(const std::vector<float>& vision) const {
        std::vector<float> processed = vision;
        
        if (config_.enableNormalization) {
            processed = normalizeSignal(processed, 0.0f, 1.0f);
        }
        
        if (config_.enableFiltering) {
            processed = applyMovingAverage(processed, config_.filterAlpha);
        }
        
        if (config_.enableCompression && config_.compressionFactor > 0.0f) {
            processed = compressSignal(processed, config_.compressionFactor);
        }
        
        if (config_.enableFeatureExtraction) {
            processed = extractFeatures(processed);
        }
        
        if (config_.enableTemporalSmoothing && !temporalBuffer_.empty()) {
            processed = applyTemporalSmoothing(processed, config_.temporalSmoothingFactor);
        }
        
        if (config_.applyNoise && config_.noiseLevel > 0.0f) {
            processed = addNoise(processed, config_.noiseLevel);
        }
        
        if (config_.clampToRange) {
            processed = clampSignal(processed, config_.minValidSignal, config_.maxValidSignal);
        }
        
        // Update temporal buffer
        if (config_.enableTemporalSmoothing) {
            temporalBuffer_.push_back(processed);
            if (temporalBuffer_.size() > 10) {
                temporalBuffer_.pop_front();
            }
        }
        
        return processed;
    }

    // Process touch signal
    std::vector<float> processTouch(const std::vector<float>& touch) const {
        std::vector<float> processed = touch;
        
        if (config_.enableNormalization) {
            processed = normalizeSignal(processed, -1.0f, 1.0f);
        }
        
        if (config_.clampToRange) {
            processed = clampSignal(processed, config_.minValidSignal, config_.maxValidSignal);
        }
        
        return processed;
    }

    // Process internal signal
    std::vector<float> processInternal(const std::vector<float>& internal) const {
        std::vector<float> processed = internal;
        
        if (config_.enableNormalization) {
            processed = normalizeSignal(processed, -1.0f, 1.0f);
        }
        
        if (config_.enableFiltering) {
            processed = applyMovingAverage(processed, config_.filterAlpha);
        }
        
        if (config_.clampToRange) {
            processed = clampSignal(processed, config_.minValidSignal, config_.maxValidSignal);
        }
        
        return processed;
    }

    // Process proprioception signal
    std::vector<float> processProprioception(const std::vector<float>& proprio) const {
        std::vector<float> processed = proprio;
        
        if (config_.enableNormalization) {
            processed = normalizeSignal(processed, -1.0f, 1.0f);
        }
        
        if (config_.enableFeatureExtraction) {
            processed = extractFeatures(processed);
        }
        
        if (config_.clampToRange) {
            processed = clampSignal(processed, config_.minValidSignal, config_.maxValidSignal);
        }
        
        return processed;
    }

    // Process audio signal
    std::vector<float> processAudio(const std::vector<float>& audio) const {
        std::vector<float> processed = audio;
        
        if (config_.enableNormalization) {
            processed = normalizeSignal(processed, -1.0f, 1.0f);
        }
        
        if (config_.enableFiltering) {
            processed = applyMovingAverage(processed, config_.filterAlpha);
        }
        
        if (config_.clampToRange) {
            processed = clampSignal(processed, config_.minValidSignal, config_.maxValidSignal);
        }
        
        return processed;
    }

    // Get feature vector from signal
    std::vector<SignalFeature> extractFeatures(const std::vector<float>& signal) const {
        std::vector<SignalFeature> features;
        
        if (signal.empty()) return features;
        
        // Basic statistics features
        float sum = std::accumulate(signal.begin(), signal.end(), 0.0f);
        float mean = sum / signal.size();
        
        float sumSq = 0.0f;
        for (float val : signal) {
            float diff = val - mean;
            sumSq += diff * diff;
        }
        float variance = sumSq / signal.size();
        float stddev = std::sqrt(variance);
        
        float min = *std::min_element(signal.begin(), signal.end());
        float max = *std::max_element(signal.begin(), signal.end());
        
        features.emplace_back("mean", mean, 1.0f);
        features.emplace_back("stddev", stddev, 0.5f);
        features.emplace_back("min", min, 0.3f);
        features.emplace_back("max", max, 0.3f);
        features.emplace_back("range", max - min, 0.4f);
        features.emplace_back("skewness", 0.0f, 0.2f); // Would compute actual skewness
        features.emplace_back("kurtosis", 0.0f, 0.2f);  // Would compute actual kurtosis
        
        return features;
    }

    const SignalProcessingConfig& getConfig() const { return config_; }
    void setConfig(const SignalProcessingConfig& config) { 
        config_ = config;
        // Clear temporal buffer on config change
        temporalBuffer_.clear();
    }

    // Signal validation
    bool validateSignal(const std::vector<float>& signal) const {
        if (!config_.enableSignalValidation) return true;
        
        for (float val : signal) {
            if (val < config_.minValidSignal || val > config_.maxValidSignal) {
                return false;
            }
        }
        
        return true;
    }

private:
    std::vector<float> normalizeSignal(const std::vector<float>& signal, 
                                       float minExpected, float maxExpected) const {
        if (signal.empty()) return signal;
        
        float minVal = *std::min_element(signal.begin(), signal.end());
        float maxVal = *std::max_element(signal.begin(), signal.end());
        
        if (maxVal - minVal < 1e-6f) {
            return std::vector<float>(signal.size(), 0.5f);
        }
        
        std::vector<float> normalized(signal.size());
        for (size_t i = 0; i < signal.size(); ++i) {
            float normalizedVal = (signal[i] - minVal) / (maxVal - minVal);
            // Scale to expected range
            normalizedVal = minExpected + normalizedVal * (maxExpected - minExpected);
            normalized[i] = normalizedVal;
        }
        
        return normalized;
    }

    std::vector<float> applyMovingAverage(const std::vector<float>& signal, float alpha) const {
        if (signal.empty() || alpha <= 0.0f) return signal;
        
        std::vector<float> filtered(signal.size());
        filtered[0] = signal[0];
        
        for (size_t i = 1; i < signal.size(); ++i) {
            filtered[i] = alpha * signal[i] + (1.0f - alpha) * filtered[i - 1];
        }
        
        return filtered;
    }

    std::vector<float> compressSignal(const std::vector<float>& signal, float factor) const {
        if (factor <= 0.0f || factor >= 1.0f || signal.empty()) return signal;
        
        std::vector<float> compressed;
        compressed.reserve(static_cast<size_t>(std::ceil(signal.size() * factor)));
        
        size_t step = static_cast<size_t>(1.0f / factor);
        for (size_t i = 0; i < signal.size(); i += step) {
            float sum = 0.0f;
            size_t count = 0;
            for (size_t j = i; j < std::min(i + step, signal.size()); ++j) {
                sum += signal[j];
                count++;
            }
            compressed.push_back(sum / count);
        }
        
        return compressed;
    }

    std::vector<float> applyTemporalSmoothing(const std::vector<float>& currentSignal, 
                                               float smoothingFactor) const {
        if (temporalBuffer_.empty()) {
            temporalBuffer_.push_back(currentSignal);
            return currentSignal;
        }
        
        const std::vector<float>& lastSignal = temporalBuffer_.back();
        
        std::vector<float> smoothed(currentSignal.size());
        for (size_t i = 0; i < currentSignal.size(); ++i) {
            smoothed[i] = smoothingFactor * currentSignal[i] + 
                         (1.0f - smoothingFactor) * 
                         (i < lastSignal.size() ? lastSignal[i] : currentSignal[i]);
        }
        
        return smoothed;
    }

    std::vector<float> addNoise(const std::vector<float>& signal, float noiseLevel) const {
        std::vector<float> noisy = signal;
        std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);
        
        for (float& val : noisy) {
            val += dist(rng);
        }
        
        return noisy;
    }

    std::vector<float> clampSignal(const std::vector<float>& signal, float minVal, float maxVal) const {
        std::vector<float> clamped(signal.size());
        for (size_t i = 0; i < signal.size(); ++i) {
            clamped[i] = std::clamp(signal[i], minVal, maxVal);
        }
        return clamped;
    }

    mutable std::deque<std::vector<float>> temporalBuffer_;
    SignalProcessingConfig config_;
};

// Enhanced Sensory data packet - with signal processing and feature extraction
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Configuration
    void configureVision(size_t width, size_t height);
    void configureTouch(size_t count);
    void configureInternal(size_t count);
    void configureProprioception(size_t count);
    void configureAudio(size_t count);
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    void setVisionSize(size_t width, size_t height) {
        visionWidth_ = width;
        visionHeight_ = height;
        vision_.resize(width * height, 0.0f);
    }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    size_t getTouchCount() const { return touch_.size(); }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    size_t getInternalCount() const { return internal_.size(); }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    size_t getProprioceptionCount() const { return proprioception_.size(); }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    size_t getAudioCount() const { return audio_.size(); }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Process signals with configured processor
    void processSignals();
    
    // Signal validation
    bool validateAllSignals() const;
    
    // Feature extraction
    std::vector<SignalFeature> extractFeatures() const;
    
    // Statistics
    float getVisionAverage() const;
    float getTouchAverage() const;
    float getInternalAverage() const;
    float getProprioceptionAverage() const;
    float getAudioAverage() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Signal processing configuration
    void setSignalProcessor(const SensoryProcessor& processor) { signalProcessor_ = processor; }
    const SensoryProcessor& getSignalProcessor() const { return signalProcessor_; }
    
    // Signal compression
    void enableSignalCompression(float factor = 0.5f);
    void disableSignalCompression();
    
    // Filtering
    void enableFiltering(float alpha = 0.3f);
    void disableFiltering();
    
    // Feature extraction
    void enableFeatureExtraction();
    void disableFeatureExtraction();
    
    // Temporal smoothing
    void enableTemporalSmoothing(float factor = 0.7f);
    void disableTemporalSmoothing();
    
    // Signal validation
    void enableSignalValidation(bool enable = true);
    void setSignalRange(float min, float max);
    
    // Noise injection (for robustness testing)
    void enableNoiseInjection(float level = 0.01f);
    void disableNoiseInjection();
    
    // Advanced signal manipulation
    void addNoiseToSignals(float level = 0.01f);
    void applyGain(float gain);
    void applyOffset(float offset);
    void scaleSignals(float factor);
    
    // Signal statistics
    struct SignalStats {
        float mean;
        float variance;
        float min;
        float max;
        float range;
        size_t count;
        
        SignalStats() : mean(0.0f), variance(0.0f), min(0.0f), max(0.0f), 
                       range(0.0f), count(0) {}
    };
    
    SignalStats computeSignalStats(const std::vector<float>& signal) const;
    std::unordered_map<std::string, SignalStats> computeAllSignalStats() const;

private:
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
    
    // Signal processing
    SensoryProcessor signalProcessor_;
    
    // Processed signal buffers
    std::vector<float> processedVision_;
    std::vector<float> processedTouch_;
    std::vector<float> processedInternal_;
    std::vector<float> processedProprioception_;
    std::vector<float> processedAudio_;
    
    bool signalsProcessed_;
};

} // namespace nlm