#pragma once

/**
 * SensoryPercept.hpp - Sensory perception data structure
 * 
 * This file contains the SensoryPercept class which represents the processed
 * sensory input received by the agent from the environment. It encapsulates
 * different sensory modalities:
 * - Vision: 2D visual information (16x16 grid in current implementation)
 * - Touch: Contact/tactile information from physical interactions
 * - Internal: Internal body state and physiological signals
 * - Proprioception: Body position and movement information
 * - Audio: Audio-like signals (if enabled)
 * 
 * The percept serves as the interface between the external world and the
 * neural processing, converting raw sensory data into neural current inputs.
 */

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * SensoryPercept - Processed sensory information from the environment
 * 
 * This structure contains the agent's current perception of its environment,
 * including visual, tactile, internal, proprioceptive, and audio signals.
 * These perceptions are processed by the AgentBrain to generate appropriate
 * motor responses. Each sensory modality is stored separately but can be
 * combined for neural input.
 */
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    /**
     * Vision: simple grayscale intensity grid (no RGB, no object labels)
     * Format: row-major, values 0.0-1.0
     */
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * Touch/proximity sensors (binary collision signals)
     */
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    /**
     * Internal state signals (homeostatic signals)
     */
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    /**
     * Proprioception: body position/velocity signals
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    /**
     * Audio-like signals (if enabled)
     */
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    /**
     * Get all signals concatenated (for brain input)
     */
    std::vector<float> getAllSignals() const;
    
    /**
     * Timestamp for when this perception was recorded
     */
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
    /**
     * Clear all sensory inputs for reset purposes
     */
    void clear() {
        vision_.clear();
        touch_.clear();
        internal_.clear();
        proprioception_.clear();
        audio_.clear();
        timestamp_ = 0.0;
    }
    
    /**
     * Check if any sensory input is active
     */
    bool hasInput() const {
        return !vision_.empty() || !touch_.empty() || 
               !internal_.empty() || !proprioception_.empty() || !audio_.empty();
    }
    
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
