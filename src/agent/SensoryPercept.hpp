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
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    // Enhanced validation and utility methods
    bool validate() const;
    bool isComplete() const;
    void normalize() const;  // Normalize all signals to [0,1]
    void clamp(float minVal, float maxVal); // Clamp all values
    
    // Timestamp operations
    void updateTimestamp(); // Update to current time
    double getAge() const;   // Get age since timestamp
    
    // Signal statistics
    float getMaxValue() const;
    float getMinValue() const;
    float getMeanValue() const;
    float getVariance() const;
    
    // Signal manipulation
    void addNoise(float amount); // Add Gaussian noise
    void smooth(int kernelSize); // Apply simple moving average
    void copyTo(SensoryPercept& other) const; // Deep copy
    
    // Comparison and difference
    float getDifference(const SensoryPercept& other) const;
    bool equals(const SensoryPercept& other) const;
    
    // Serialization (for checkpointing)
    void save(std::ostream& os) const;
    void load(std::istream& is);
    
    // Debug and info
    std::string toString() const;
    void printDebug() const;
    
private:
    // Enhanced validation and internal helpers
    void validateVision() const;
    void validateTouch() const;
    void validateInternal() const;
    void validateProprioception() const;
    void validateAudio() const;
    
    // Signal processing helpers
    static float computeMax(const std::vector<float>& vec);
    static float computeMin(const std::vector<float>& vec);
    static float computeMean(const std::vector<float>& vec);
    static float computeVariance(const std::vector<float>& vec, float mean);
    static void addGaussianNoise(std::vector<float>& vec, float amount);
    static void applyMovingAverage(std::vector<float>& vec, int kernelSize);
    
private:
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
