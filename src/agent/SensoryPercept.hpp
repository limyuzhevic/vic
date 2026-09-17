#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    // Configuration constants
    static constexpr size_t DEFAULT_VISION_WIDTH = 16;
    static constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    static constexpr size_t DEFAULT_TOUCH_SIZE = 8;
    static constexpr size_t DEFAULT_INTERNAL_SIZE = 4;
    static constexpr size_t DEFAULT_PROPRIOCEPTION_SIZE = 6;
    
    SensoryPercept()
        : visionWidth_(DEFAULT_VISION_WIDTH)
        , visionHeight_(DEFAULT_VISION_HEIGHT)
        , timestamp_(0.0)
    {
        initializeArrays();
    }
    
    explicit SensoryPercept(size_t visionWidth, size_t visionHeight)
        : visionWidth_(visionWidth)
        , visionHeight_(visionHeight)
        , timestamp_(0.0)
    {
        initializeArrays();
    }
    
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { 
        validateArraySize(v, visionWidth_ * visionHeight_, "vision");
        vision_ = v; 
    }
    void setVision(size_t width, size_t height, const std::vector<float>& v) {
        validateArraySize(v, width * height, "vision");
        visionWidth_ = width;
        visionHeight_ = height;
        vision_ = v;
    }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    size_t getVisionArea() const { return visionWidth_ * visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { 
        validateArraySize(t, DEFAULT_TOUCH_SIZE, "touch");
        touch_ = t; 
    }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { 
        validateArraySize(i, DEFAULT_INTERNAL_SIZE, "internal");
        internal_ = i; 
    }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { 
        validateArraySize(p, DEFAULT_PROPRIOCEPTION_SIZE, "proprioception");
        proprioception_ = p; 
    }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Get perceptual statistics
    struct PerceptualStats {
        float visionMean = 0.0f;
        float visionStdDev = 0.0f;
        float touchMax = 0.0f;
        float internalMax = 0.0f;
        float proprioceptionMax = 0.0f;
        double getTotalProcessingTime() const { return processingTime_; }
        
    private:
        friend class SensoryPercept;
        double processingTime_ = 0.0;
    };
    
    PerceptualStats getStats() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Validation and utility methods
    bool validate() const;
    bool isEmpty() const;
    size_t getTotalSignalCount() const;
    std::string toString() const;
    
    // Reset for new episode
    void reset() {
        std::fill(vision_.begin(), vision_.end(), 0.0f);
        std::fill(touch_.begin(), touch_.end(), 0.0f);
        std::fill(internal_.begin(), internal_.end(), 0.0f);
        std::fill(proprioception_.begin(), proprioception_.end(), 0.0f);
        std::fill(audio_.begin(), audio_.end(), 0.0f);
        timestamp_ = 0.0;
    }
    
    // Copy assignment
    SensoryPercept& operator=(const SensoryPercept&) = default;
    
private:
    // Helper methods
    void initializeArrays();
    void validateArraySize(const std::vector<float>& array, size_t expectedSize, 
                          const std::string& fieldName) const;
    
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    double timestamp_;
};

} // namespace nlm
