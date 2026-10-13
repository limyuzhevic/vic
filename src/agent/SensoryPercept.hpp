#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class SensoryPercept
 * @brief Low-level sensory data packet - what the agent perceives
 * 
 * The SensoryPercept class contains raw sensory signals without semantic labels.
 * It represents the direct input from the world's sensors to the agent's brain,
 * containing only the physical measurements before any interpretation.
 * 
 * This class implements the sensory transduction layer, converting world state
 * into neural-level signals that can be processed by the NLM brain.
 * 
 * Design Philosophy:
 * - NO semantic labels: Keeps signals low-level for brain processing
 * - Time-stamped: Includes timestamp for temporal analysis
 * - Modular: Each sensory modality is separate but combinable
 * - Extensible: Easy to add new sensory modalities
 */
class SensoryPercept {
public:
    /**
     * @brief Constructor - creates empty sensory percept
     * 
     * Initializes all sensory vectors with standard sizes for the default
     * configuration. This provides a complete but empty sensory state.
     */
    SensoryPercept();
    
    /**
     * @brief Destructor - cleans up any allocated resources
     */
    ~SensoryPercept() = default;
    
    /**
     * @brief Get vision data (grayscale intensity grid)
     * 
     * Vision represents what the agent sees as a 2D grayscale array.
     * This is the primary visual input for spatial navigation and object
     * detection tasks.
     * 
     * @return Reference to vision vector (row-major order)
     * @note Values range from 0.0 (black) to 1.0 (white)
     * @note Size = visionWidth_ * visionHeight_ (typically 16x16 = 256)
     */
    const std::vector<float>& getVision() const { return vision_; }
    
    /**
     * @brief Set vision data
     * 
     * @param v New vision vector (0.0-1.0 values, row-major)
     */
    void setVision(const std::vector<float>& v) { vision_ = v; }
    
    /**
     * @brief Get vision grid width
     * 
     * @return Vision grid width in cells
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * @brief Get vision grid height
     * 
     * @return Vision grid height in cells
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * @brief Get touch/proximity sensor data
     * 
     * Touch sensors detect contact or proximity to nearby objects.
     * This includes collisions, object proximity, and haptic feedback.
     * 
     * @return Reference to touch vector
     * @note Binary collision signals (typically 0.0-1.0, where 1.0 = collision)
     * @note Size = 8 (standard for most configurations)
     */
    const std::vector<float>& getTouch() const { return touch_; }
    
    /**
     * @brief Set touch/proximity sensor data
     * 
     * @param t New touch vector
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    /**
     * @brief Get internal state signals
     * 
     * Internal signals represent the agent's homeostatic state,
     * including energy, health, hunger, thirst, and other physiological
     * parameters that drive motivated behavior.
     * 
     * @return Reference to internal state vector
     * @note Size = 4 (energy, health, hunger, thirst typically)
     */
    const std::vector<float>& getInternal() const { return internal_; }
    
    /**
     * @brief Set internal state signals
     * 
     * @param i New internal state vector
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    /**
     * @brief Get proprioception signals
     * 
     * Proprioception provides information about the agent's body state,
     * including position, velocity, and orientation in the world.
     * This enables coordination of movement and spatial awareness.
     * 
     * @return Reference to proprioception vector
     * @note Size = 6 (position, velocity, orientation typically)
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    
    /**
     * @brief Set proprioception signals
     * 
     * @param p New proprioception vector
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    /**
     * @brief Get audio-like signals (if enabled)
     * 
     * Audio signals provide sound and acoustic information from the world.
     * This can include speech, music, environmental sounds, and alarms.
     * 
     * @return Reference to audio vector
     * @note Size = 0 by default (audio typically disabled)
     */
    const std::vector<float>& getAudio() const { return audio_; }
    
    /**
     * @brief Set audio-like signals
     * 
     * @param a New audio vector
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    /**
     * @brief Get all sensory signals concatenated
     * 
     * This method provides a flattened view of all sensory modalities,
     * which is useful for direct injection into the brain's sensory input
     * layer when the brain expects a single input vector.
     * 
     * @return Vector containing all signals in order: vision, touch, internal,
     *         proprioception, audio
     */
    std::vector<float> getAllSignals() const;
    
    /**
     * @brief Get sensory percept timestamp
     * 
     * @return Timestamp when this percept was created
     */
    double getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Set sensory percept timestamp
     * 
     * @param t New timestamp
     */
    void setTimestamp(double t) { timestamp_ = t; }
    
    /**
     * @brief Check if percept contains valid data
     * 
     * @return true if at least one sensory modality has data
     * @note Useful for early termination in processing loops
     */
    bool hasValidData() const {
        return !vision_.empty() || !touch_.empty() || 
               !internal_.empty() || !proprioception_.empty() ||
               !audio_.empty();
    }

private:
    /**
     * @brief Vision sensory data (2D grayscale array)
     * 
     * Stored as a flattened vector in row-major order. Represents what
     * the agent perceives visually. Size is determined by visionWidth_ and
     * visionHeight_ configuration parameters.
     */
    std::vector<float> vision_;
    
    /**
     * @brief Vision grid dimensions
     * 
     * These determine the resolution of visual processing. Higher values
     * provide more detailed visual information but require more computation.
     */
    size_t visionWidth_;
    size_t visionHeight_;
    
    /**
     * @brief Touch/proximity sensor data
     * 
     * Binary collision signals from contact and proximity sensors.
     * Used for object interaction and collision avoidance.
     */
    std::vector<float> touch_;
    
    /**
     * @brief Internal state signals
     * 
     * Homeostatic and motivational signals that drive goal-directed
     * behavior and influence decision-making.
     */
    std::vector<float> internal_;
    
    /**
     * @brief Proprioception signals
     * 
     * Body state information including position, velocity, and orientation.
     * Essential for coordinated movement and spatial navigation.
     */
    std::vector<float> proprioception_;
    
    /**
     * @brief Audio-like signals
     * 
     * Acoustic information for sound-based perception and communication.
     * Can be used for spatial audio, speech recognition, or environmental
     * awareness.
     */
    std::vector<float> audio_;
    
    /**
     * @brief Time when this percept was created
     * 
     * Used for temporal analysis, change detection, and timing of actions.
     * Allows the agent to reason about time and sequence of events.
     */
    double timestamp_;

}; // namespace nlm
