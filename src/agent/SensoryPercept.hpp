#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

// SensoryPercept: Container for sensory data from the world
// Holds vision, touch, proprioception, internal signals, and audio
class SensoryPercept {
public:
    // Default constructor
    SensoryPercept();
    
    // Get vision data
    const Vision& getVision() const;
    void setVision(const Vision& vision);
    
    // Get touch data
    const Audio& getTouch() const;
    void setTouch(const Audio& touch);
    
    // Get internal signals (neuromodulation, prediction error, etc.)
    const InternalSignals& getInternal() const;
    void setInternal(const InternalSignals& internal);
    
    // Get proprioception (body position, velocity)
    const std::vector<float>& getProprioception() const;
    void setProprioception(const std::vector<float>& proprioception);
    
    // Get audio data
    const Audio& getAudio() const;
    void setAudio(const Audio& audio);
    
    // Get all signal values as vector
    std::vector<float> getAllSignals() const;
    
    // Get timestamp
    Timestamp getTimestamp() const;
    void setTimestamp(Timestamp timestamp);
    
    // Reset all sensory data
    void reset();
    
    // Get vision width
    size_t getVisionWidth() const;
    
    // Get vision height
    size_t getVisionHeight() const;
    
private:
    Vision vision_;
    Audio touch_;
    InternalSignals internal_;
    std::vector<float> proprioception_;
    Audio audio_;
    Timestamp timestamp_;
};

} // namespace nlm