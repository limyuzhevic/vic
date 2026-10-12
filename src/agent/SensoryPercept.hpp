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
    // Audio format specification (sample rate and channel count)
    struct AudioFormat {
        int sampleRate;      // Samples per second (e.g., 44100 Hz)
        int channels;        // Number of audio channels (1 = mono, 2 = stereo)
        int bitsPerSample;   // Bit depth (e.g., 16, 24, 32)
        
        AudioFormat(int sr = 44100, int ch = 1, int bps = 16) 
            : sampleRate(sr), channels(ch), bitsPerSample(bps) {}
    };
    
    // Default constructor - initializes with standard sensory parameters
    SensoryPercept(int visionW = 16, int visionH = 16, int touchCount = 8, 
                   int internalCount = 4, int proprioceptionCount = 6,
                   const AudioFormat& audioFormat = AudioFormat());
    ~SensoryPercept() = default;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const noexcept { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const noexcept { return visionWidth_; }
    size_t getVisionHeight() const noexcept { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const noexcept { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const noexcept { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const noexcept { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const noexcept { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    const AudioFormat& getAudioFormat() const noexcept { return audioFormat_; }
    void setAudioFormat(const AudioFormat& fmt) { audioFormat_ = fmt; }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Get total sensory input size (for brain interface)
    size_t getTotalSignalCount() const noexcept;
    
    // Validate sensory data integrity
    bool validate() const noexcept;
    
    // Timestamp
    double getTimestamp() const noexcept { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    // Move assignment operator (efficient transfer of large vectors)
    SensoryPercept& operator=(SensoryPercept&& other) noexcept;
    
    // Copy assignment operator (deep copy for thread safety)
    SensoryPercept& operator=(const SensoryPercept& other);
    
private:
    // Memory allocation optimization - reserve space upfront
    void reserveMemory() noexcept;
    
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    AudioFormat audioFormat_;
    double timestamp_;
};

} // namespace nlm
