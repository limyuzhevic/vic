#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>

namespace nlm {

// Forward declarations
class AgentBody;

// Enhanced SensoryPercept with better memory management and encapsulation
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    // Configuration for sensory processing
    struct Config {
        // Signal dimensions (maintained for backward compatibility)
        static constexpr size_t VISION_WIDTH = 16;
        static constexpr size_t VISION_HEIGHT = 16;
        static constexpr size_t TOUCH_COUNT = 8;
        static constexpr size_t INTERNAL_COUNT = 4;
        static constexpr size_t PROPRIOCEPT_COUNT = 6;
        
        // Audio configuration
        static constexpr size_t AUDIO_COUNT = 0; // Disabled by default
        
        // Processing parameters
        float visionMin = 0.0f;
        float visionMax = 1.0f;
        float touchMin = 0.0f;
        float touchMax = 1.0f;
        float internalMin = -1.0f;
        float internalMax = 1.0f;
        float proprioceptionMin = -1.0f;
        float proprioceptionMax = 1.0f;
        
        // Signal quality parameters
        float visionQualityThreshold = 0.01f;
        float touchThreshold = 0.1f;
        float internalThreshold = 0.1f;
        float proprioceptionThreshold = 0.1f;
    };

    SensoryPercept(const Config& config = Config{});
    ~SensoryPercept();
    
    // Copy constructor and assignment operator for deep copy semantics
    SensoryPercept(const SensoryPercept& other);
    SensoryPercept& operator=(const SensoryPercept& other);
    
    // Move constructor and assignment operator for efficient transfers
    SensoryPercept(SensoryPercept&& other) noexcept;
    SensoryPercept& operator=(SensoryPercept&& other) noexcept;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v);
    void setVision(std::vector<float>&& v) noexcept;
    
    size_t getVisionWidth() const { return config_.VISION_WIDTH; }
    size_t getVisionHeight() const { return config_.VISION_HEIGHT; }
    size_t getVisionSize() const { return vision_.size(); }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t);
    void setTouch(std::vector<float>&& t) noexcept;
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i);
    void setInternal(std::vector<float>&& i) noexcept;
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p);
    void setProprioception(std::vector<float>&& p) noexcept;
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a);
    void setAudio(std::vector<float>&& a) noexcept;
    bool isAudioEnabled() const { return audio_.size() > 0; }
    
    // Get all signals concatenated (for brain input) with optimized performance
    std::vector<float> getAllSignals() const;
    
    // Get signals as fixed-size arrays for better memory access patterns
    std::array<float, Config::VISION_WIDTH * Config::VISION_HEIGHT> getVisionArray() const;
    std::array<float, Config::TOUCH_COUNT> getTouchArray() const;
    std::array<float, Config::INTERNAL_COUNT> getInternalArray() const;
    std::array<float, Config::PROPRIOCEPT_COUNT> getProprioceptionArray() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Signal quality and validation
    bool validateVision() const;
    bool validateTouch() const;
    bool validateInternal() const;
    bool validateProprioception() const;
    
    // Signal normalization and scaling
    void normalizeVision();
    void normalizeTouch();
    void normalizeInternal();
    void normalizeProprioception();
    
    // Signal processing operations
    void applySmoothing(float alpha = 0.1f);
    void applyGain(float factor);
    void applyBias(float bias);
    
    // Comparison and difference operations
    float computeVisionDifference(const SensoryPercept& other) const;
    float computeTotalDifference(const SensoryPercept& other) const;
    
    // Configuration management
    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }
    
    // Performance optimization helpers
    size_t getTotalSignalCount() const;
    size_t getActiveSignalCount() const;
    bool hasSignalChanged(const SensoryPercept& previous) const;
    
    // Memory efficiency methods
    void clear();
    void resizeToFit();
    size_t memoryUsage() const;
    
    // Reset for new episode
    void reset();

private:
    // Core sensory data with optimized storage
    std::vector<float> vision_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    
    // Configuration
    Config config_;
    
    // Timestamp for temporal ordering
    double timestamp_;
    
    // Performance optimization caches
    mutable std::vector<float> allSignalsCache_;
    mutable bool allSignalsCacheValid_ = false;
    
    // Signal history for temporal processing
    struct SignalHistory {
        std::vector<float> visionHistory;
        std::vector<float> touchHistory;
        std::vector<float> internalHistory;
        std::vector<float> proprioceptionHistory;
        size_t historySize = 10;
        
        void initialize(size_t size) {
            visionHistory.resize(size, 0.0f);
            touchHistory.resize(size, 0.0f);
            internalHistory.resize(size, 0.0f);
            proprioceptionHistory.resize(size, 0.0f);
        }
        
        void update(const std::vector<float>& vision, const std::vector<float>& touch,
                   const std::vector<float>& internal, const std::vector<float>& proprioception);
        
        float computeTemporalDifference(const std::vector<float>& current, const std::vector<float>& previous) const;
    } signalHistory_;
    
    // Signal validation and quality checks
    bool validateSignalRange(const std::vector<float>& signal, float min, float max) const;
    float computeSignalQuality(const std::vector<float>& signal, float threshold) const;
    
    // Memory management helpers
    void ensureCapacity(const std::vector<float>& signal);
    void updateCache() const;
    
    // Signal processing helpers
    void normalizeSignal(std::vector<float>& signal, float min, float max);
    void applySignalFilter(std::vector<float>& signal, float alpha);
    
    // Comparison helpers
    static float vectorDifference(const std::vector<float>& a, const std::vector<float>& b);
    
}; // namespace nlm