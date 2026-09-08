#pragma once

// SensoryPercept.hpp - Sensory percept container
// This header defines the SensoryPercept class that aggregates all sensory inputs

#include "Vision.hpp"
#include "Audio.hpp"
#include "InternalSignals.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * @class SensoryPercept
 * @brief Container for all sensory inputs from environment
 * 
 * The SensoryPercept class aggregates all sensory modalities that an agent
 * receives from the environment. This includes vision (external visual input),
 * touch (tactile sensing), proprioception (body position awareness),
 * and internal signals (internal physiological states).
 * 
 * This class is used by the AgentBrain to communicate with the NLM brain,
 * allowing the brain to receive environmental information and generate
 * appropriate motor commands.
 */
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept();
    
    // Disable copying, enable moving
    SensoryPercept(const SensoryPercept&) = delete;
    SensoryPercept& operator=(const SensoryPercept&) = delete;
    SensoryPercept(SensoryPercept&&) noexcept;
    SensoryPercept& operator=(SensoryPercept&&) noexcept;
    
    // Vision sensory input
    Vision& getVision();
    const Vision& getVision() const;
    void setVision(const Vision& vision);
    
    // Vision properties
    size_t getVisionWidth() const;
    size_t getVisionHeight() const;
    size_t getVisionChannels() const;
    
    // Touch/Proprioception (combined for simplicity)
    InternalSignals& getTouch();
    const InternalSignals& getTouch() const;
    void setTouch(const InternalSignals& touch);
    
    // Internal signals (body state)
    InternalSignals& getInternal();
    const InternalSignals& getInternal() const;
    void setInternal(const InternalSignals& internal);
    
    // Proprioception (body position/rotation)
    std::vector<float>& getProprioception();
    const std::vector<float>& getProprioception() const;
    void setProprioception(const std::vector<float>& proprioception);
    
    // Audio sensory input
    Audio& getAudio();
    const Audio& getAudio() const;
    void setAudio(const Audio& audio);
    
    // Audio properties
    size_t getAudioSampleRate() const;
    size_t getAudioNumSamples() const;
    
    // All sensory signals
    std::vector<float> getAllSignals() const;
    
    // Timestamp management
    double getTimestamp() const;
    void setTimestamp(double timestamp);
    
    // Reset all sensory inputs
    void reset();
    
    // Check if any sensory data is present
    bool hasData() const;
    
    // Get total sensory dimensionality
    size_t getTotalDimensionality() const;
    
    // Get sensory data summary
    std::string getSummary() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm