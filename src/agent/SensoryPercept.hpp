#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Sensory data packet - what the agent perceives
// Enhanced version with better memory management, validation, and extensibility
class SensoryPercept {
public:
    // Configuration options for sensory processing
    struct Config {
        // Vision configuration
        size_t visionWidth = 16;
        size_t visionHeight = 16;
        bool enableVision = true;
        
        // Touch configuration
        size_t touchCount = 8;
        bool enableTouch = true;
        
        // Internal signals configuration
        size_t internalCount = 4;
        bool enableInternal = true;
        
        // Proprioception configuration
        size_t proprioceptionCount = 6;
        bool enableProprioception = true;
        
        // Audio configuration
        size_t audioCount = 0;
        bool enableAudio = false;
        
        // Validation flags
        bool validateInputs = true;
        bool clampValues = true;
        bool enableTimestamps = true;
        
        // Novelty detection
        float noveltyThreshold = 0.1f;
        float noveltySmoothingFactor = 0.99f;
        
        // Audio processing
        bool enableAudioProcessing = false;
        float audioGain = 1.0f;
        
        // Performance optimization
        bool preallocateBuffers = true;
        bool enableCompression = false;
    };

    // Specialized sensory data types for advanced processing
    enum class SensoryModality {
        Vision,
        Touch,
        Internal,
        Proprioception,
        Audio,
        Combined
    };

    // Error types for sensory processing
    enum class ErrorType {
        None,
        DimensionMismatch,
        OutOfRange,
        NullPointer,
        MemoryAllocationFailed,
        ValidationFailed
    };

    // Constructor with enhanced configuration support
    explicit SensoryPercept(const Config& config = Config{});
    SensoryPercept(const SensoryPercept& other);
    SensoryPercept& operator=(const SensoryPercept& other);
    ~SensoryPercept() = default;
    
    // Move semantics for performance
    SensoryPercept(SensoryPercept&& other) noexcept;
    SensoryPercept& operator=(SensoryPercept&& other) noexcept;
    
    // Configure sensory processing
    void configure(const Config& config);
    const Config& getConfig() const { return config_; }
    
    // Vision methods with validation and error handling
    // Set vision data with bounds checking
    // Returns: ErrorType indicating success/failure
    ErrorType setVision(const std::vector<float>& v);
    
    // Set vision data with explicit dimensions
    ErrorType setVision(const std::vector<float>& v, size_t width, size_t height);
    
    // Get vision data
    const std::vector<float>& getVision() const { return vision_; }
    
    // Get vision dimensions
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    size_t getVisionElements() const { return visionWidth_ * visionHeight_; }
    
    // Check if vision data is valid
    bool hasVision() const { return !vision_.empty(); }
    bool isVisionComplete() const { return hasVision() && visionWidth_ > 0 && visionHeight_ > 0; }
    
    // Vision quality metrics
    float getVisionQuality() const;
    bool isVisionNormalized() const;
    
    // Touch methods with enhanced validation
    ErrorType setTouch(const std::vector<float>& t);
    const std::vector<float>& getTouch() const { return touch_; }
    bool hasTouch() const { return !touch_.empty(); }
    
    // Internal signals methods
    ErrorType setInternal(const std::vector<float>& i);
    const std::vector<float>& getInternal() const { return internal_; }
    bool hasInternal() const { return !internal_.empty(); }
    
    // Proprioception methods
    ErrorType setProprioception(const std::vector<float>& p);
    const std::vector<float>& getProprioception() const { return proprioception_; }
    bool hasProprioception() const { return !proprioception_.empty(); }
    
    // Audio methods (enhanced)
    ErrorType setAudio(const std::vector<float>& a);
    const std::vector<float>& getAudio() const { return audio_; }
    bool hasAudio() const { return !audio_.empty(); }
    
    // Combined access methods
    std::vector<float> getAllSignals() const;
    std::vector<float> getSignalsForModality(SensoryModality modality) const;
    
    // Validation and quality methods
    bool isValid() const;
    std::vector<ErrorType> validate() const;
    std::string getValidationErrors() const;
    
    // Timestamp management
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    void setTimestampNow(); // Current system time
    
    // Clone and copy operations
    std::unique_ptr<SensoryPercept> clone() const;
    
    // Comparison operators
    bool equals(const SensoryPercept& other) const;
    bool operator==(const SensoryPercept& other) const { return equals(other); }
    bool operator!=(const SensoryPercept& other) const { return !equals(other); }
    
    // Specialized processing methods
    float getNovelty(const SensoryPercept& previous) const;
    std::vector<float> getDifference(const SensoryPercept& other) const;
    SensoryModality dominantModality() const;
    
    // Statistical methods
    float getMeanValue(SensoryModality modality) const;
    float getStandardDeviation(SensoryModality modality) const;
    float getMaxValue(SensoryModality modality) const;
    float getMinValue(SensoryModality modality) const;
    
    // Utility methods
    size_t getTotalElements() const;
    size_t getElementCount(SensoryModality modality) const;
    std::string getSummary() const;
    
    // Memory management
    void clear();
    void reserve(size_t capacity);
    size_t capacity() const;
    size_t memoryUsage() const;
    
    // Debug and diagnostic
    void debugPrint() const;
    std::string toDebugString() const;
    
    // Advanced processing callbacks
    using DataProcessor = std::function<void(const std::vector<float>&, SensoryModality)>;
    void addDataProcessor(DataProcessor processor);
    void removeDataProcessor(DataProcessor processor);
    void processWithAllProcessors();
    
    // Compression methods
    std::vector<uint8_t> compress() const;
    static std::unique_ptr<SensoryPercept> decompress(const std::vector<uint8_t>& data);
    
    // Static factory methods
    static std::unique_ptr<SensoryPercept> createEmpty();
    static std::unique_ptr<SensoryPercept> createFromWorld(const SimpleWorld& world);
    static std::unique_ptr<SensoryPercept> createRandom(const Config& config);
    
private:
    // Helper methods
    ErrorType validateVisionDimensions(size_t width, size_t height) const;
    ErrorType validateVectorSize(const std::vector<float>& data, size_t expectedSize, 
                                 const std::string& name) const;
    void applyValueClamping(std::vector<float>& data) const;
    void notifyProcessors(SensoryModality modality, const std::vector<float>& data);
    
    // Data members with enhanced memory management
    std::vector<float> vision_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    
    size_t visionWidth_;
    size_t visionHeight_;
    double timestamp_;
    
    Config config_;
    
    // Advanced features
    std::vector<DataProcessor> dataProcessors_;
    mutable std::vector<ErrorType> validationCache_;
    mutable bool validationDirty_;
};

// Utility functions for sensory processing
namespace SensoryPerceptUtils {
    // Mathematical operations
    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b);
    static float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b);
    static float correlationCoefficient(const std::vector<float>& a, const std::vector<float>& b);
    
    // Signal processing
    static std::vector<float> applySmoothing(const std::vector<float>& data, float factor);
    static std::vector<float> normalize(const std::vector<float>& data);
    static std::vector<float> scale(const std::vector<float>& data, float min, float max);
    
    // Statistics
    static float mean(const std::vector<float>& data);
    static float variance(const std::vector<float>& data);
    static float standardDeviation(const std::vector<float>& data);
    static float max(const std::vector<float>& data);
    static float min(const std::vector<float>& data);
    
    // Special processing
    static std::vector<float> edgeDetection(const std::vector<float>& vision, size_t width, size_t height);
    static std::vector<float> motionDetection(const std::vector<float>& prevVision, 
                                               const std::vector<float>& currVision);
}

} // namespace nlm
