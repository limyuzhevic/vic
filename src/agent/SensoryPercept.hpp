#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Sensory data packet - what the agent perceives
/**
 * @class SensoryPercept
 * @brief Container for low-level sensory signals without semantic labels
 * 
 * SensoryPercept encapsulates all sensory modalities that an agent receives from
 * its environment. It contains only raw sensor data (intensity values, not object
 * labels or semantic information), making it suitable for processing by neural
 * networks and brain systems. The data is organized by sensory modality and
 * includes vision, touch, internal state, proprioception, and optional audio signals.
 * 
 * Key characteristics:
 * - No semantic interpretation (no object detection, recognition, etc.)
 * - Raw intensity values (typically 0.0-1.0 for vision, 0.0-1.0 for others)
 * - Timestamped for temporal alignment with actions
 * - Concatenated for brain input via getAllSignals()
 * - Suitable for end-to-end learning systems
 * 
 * Typical use case: Bridge between environment sensors and neural brain processing
 */
class SensoryPercept {
public:
    /**
     * @brief Construct a new SensoryPercept object
     * 
     * Initializes all sensory vectors to empty states with default
     * dimensions and sets timestamp to current time.
     */
    SensoryPercept();
    
    /**
     * @brief Default destructor (no explicit cleanup needed)
     * 
     * All data members are standard containers that handle their own cleanup.
     */
    ~SensoryPercept() = default;
    
    /**
     * @brief Get visual sensor data
     * 
     * @return const std::vector<float>& Reference to vision signal vector
     * @note Values are in range [0.0, 1.0] where 0.0 = black, 1.0 = white
     * @note Format: row-major 2D grid, visionWidth_ × visionHeight_ elements
     */
    const std::vector<float>& getVision() const { return vision_; }
    
    /**
     * @brief Set visual sensor data
     * 
     * @param v New vision signal vector (must match visionWidth_ × visionHeight_)
     * @note Values should be in range [0.0, 1.0]
     */
    void setVision(const std::vector<float>& v) { vision_ = v; }
    
    /**
     * @brief Get vision signal width
     * 
     * @return size_t Number of columns in the vision grid
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * @brief Get vision signal height
     * 
     * @return size_t Number of rows in the vision grid
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * @brief Get touch/proximity sensor data
     * 
     * @return const std::vector<float>& Reference to touch signal vector
     * @note Binary collision signals: 0.0 = no contact, 1.0 = contact
     * @note Position corresponds to body part or proximity sensor index
     */
    const std::vector<float>& getTouch() const { return touch_; }
    
    /**
     * @brief Set touch/proximity sensor data
     * 
     * @param t New touch signal vector
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    /**
     * @brief Get internal state sensor data
     * 
     * @return const std::vector<float>& Reference to internal signal vector
     * @note Homeostatic signals: hunger, thirst, fatigue, etc. (0.0-1.0)
     * @note Represents agent's physiological and metabolic states
     */
    const std::vector<float>& getInternal() const { return internal_; }
    
    /**
     * @brief Set internal state sensor data
     * 
     * @param i New internal signal vector
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    /**
     * @brief Get proprioception sensor data
     * 
     * @return const std::vector<float>& Reference to proprioception signal vector
     * @note Body position and velocity signals (0.0-1.0 normalized)
     * @note Includes joint angles, limb positions, movement velocities
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    
    /**
     * @brief Set proprioception sensor data
     * 
     * @param p New proprioception signal vector
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    /**
     * @brief Get audio-like sensor data (if enabled)
     * 
     * @return const std::vector<float>& Reference to audio signal vector
     * @note Optional auditory signals (0.0-1.0 intensity)
     * @note May be empty if audio sensing is disabled
     */
    const std::vector<float>& getAudio() const { return audio_; }
    
    /**
     * @brief Set audio-like sensor data
     * 
     * @param a New audio signal vector
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    /**
     * @brief Get all sensory signals concatenated
     * 
     * @return std::vector<float> Concatenated vector of all sensory signals
     * @note Order: vision, touch, internal, proprioception, audio
     * @note Total size = visionWidth_ × visionHeight_ + touch_.size() + 
     *       internal_.size() + proprioception_.size() + audio_.size()
     * @note Used as input to brain systems
     */
    std::vector<float> getAllSignals() const;
    
    /**
     * @brief Get sensor timestamp
     * 
     * @return double Timestamp when this percept was captured
     * @note Unix timestamp or simulation time unit
     */
    double getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Set sensor timestamp
     * 
     * @param t New timestamp value
     */
    void setTimestamp(double t) { timestamp_ = t; }
    
private:
    /**
     * @brief Visual field data (grayscale image)
     * 
     * Represents what the agent sees through its vision sensor.
     * Stored as a 1D vector in row-major order with dimensions visionWidth_ × visionHeight_.
     */
    std::vector<float> vision_;
    
    /**
     * @brief Width of the vision grid
     */
    size_t visionWidth_;
    
    /**
     * @brief Height of the vision grid
     */
    size_t visionHeight_;
    
    /**
     * @brief Touch/proximity sensor signals
     * 
     * Binary collision signals indicating contact with objects or boundaries.
     * Each element corresponds to a different touch sensor or body part.
     */
    std::vector<float> touch_;
    
    /**
     * @brief Internal state signals (homeostatic)
     * 
     * Represents the agent's physiological and metabolic conditions.
     * Includes hunger, thirst, fatigue, energy levels, etc.
     */
    std::vector<float> internal_;
    
    /**
     * @brief Proprioception signals (body awareness)
     * 
     * Contains information about body position, orientation, and movement.
     * Includes joint angles, limb positions, and velocity signals.
     */
    std::vector<float> proprioception_;
    
    /**
     * @brief Audio-like signals (optional)
     * 
     * Auditory or sound-related signals if audio sensing is enabled.
     * May be empty if audio capabilities are not implemented.
     */
    std::vector<float> audio_;
    
    /**
     * @brief Time when this percept was captured
     * 
     * Used for temporal alignment between perception and action.
     */
    double timestamp_;
};

} // namespace nlm
