#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class SensoryPercept
 * @brief Sensory data packet representing the agent's perception of the environment
 * 
 * The SensoryPercept class represents a complete snapshot of all sensory inputs
 * that the agent perceives at a given moment in time. It contains low-level sensory
 * signals without semantic interpretation, allowing the brain to extract meaningful
 * patterns and make predictions.
 * 
 * Key design principles:
 * - No semantic labels or object identities (pure signal processing)
 * - All sensory modalities are treated uniformly as numerical signals
 * - Designed for efficient processing by neural networks
 * - Includes timestamps for temporal processing
 * 
 * Sensory modalities included:
 * - Vision: Grayscale intensity grid (16x16 = 256 values)
 * - Touch: Binary collision signals (8 values)
 * - Internal: Homeostatic state signals (4 values)
 * - Proprioception: Body position/velocity signals (6 values)
 * - Audio: Audio-like signals (configurable size, typically empty)
 */
class SensoryPercept {
public:
    /**
     * @brief Constructor - creates empty sensory percept
     * 
     * Initializes all sensory vectors with default sizes and zeros.
     * Vision is initialized to 16x16 grid, other modalities to their default sizes.
     */
    SensoryPercept();
    
    /**
     * @brief Destructor (default)
     */
    ~SensoryPercept() = default;

    // ============================================================================
    // Vision Sensory System
    // ============================================================================
    
    /**
     * @brief Get vision input data
     * 
     * Returns the complete vision grid as a flattened vector in row-major order.
     * Values are in the range [0.0, 1.0] representing intensity.
     * 
     * @return Constant reference to vision data vector
     */
    [[nodiscard]] const std::vector<float>& getVision() const noexcept { return vision_; }
    
    /**
     * @brief Set vision input data
     * 
     * Updates the vision data with new values. The vector should maintain the
     * dimensions specified by visionWidth_ and visionHeight_.
     * 
     * @param v New vision data vector
     */
    void setVision(const std::vector<float>& v) { vision_ = v; }
    
    /**
     * @brief Get vision grid width
     * 
     * @return Vision grid width in pixels
     */
    [[nodiscard]] size_t getVisionWidth() const noexcept { return visionWidth_; }
    
    /**
     * @brief Get vision grid height
     * 
     * @return Vision grid height in pixels
     */
    [[nodiscard]] size_t getVisionHeight() const noexcept { return visionHeight_; }

    // ============================================================================
    // Touch Sensory System
    // ============================================================================
    
    /**
     * @brief Get touch/proximity sensor data
     * 
     * Returns binary collision signals from proximity sensors distributed
     * across the agent's body. Values are in the range [0.0, 1.0] where 0.0
     * indicates no collision and 1.0 indicates collision.
     * 
     * @return Constant reference to touch data vector
     */
    [[nodiscard]] const std::vector<float>& getTouch() const noexcept { return touch_; }
    
    /**
     * @brief Set touch sensor data
     * 
     * @param t New touch data vector
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }

    // ============================================================================
    // Internal Sensory System
    // ============================================================================
    
    /**
     * @brief Get internal state signals
     * 
     * Returns homeostatic state signals that monitor the agent's internal
     * condition (energy, hunger, fatigue, etc.). Values are normalized to [0.0, 1.0]
     * where 1.0 represents optimal internal state.
     * 
     * @return Constant reference to internal state vector
     */
    [[nodiscard]] const std::vector<float>& getInternal() const noexcept { return internal_; }
    
    /**
     * @brief Set internal state signals
     * 
     * @param i New internal state data vector
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }

    // ============================================================================
    // Proprioceptive Sensory System
    // ============================================================================
    
    /**
     * @brief Get proprioception data
     * 
     * Returns body position and velocity signals that inform the agent about
     * its own movement and orientation. Values represent normalized body state.
     * 
     * @return Constant reference to proprioception data vector
     */
    [[nodiscard]] const std::vector<float>& getProprioception() const noexcept { return proprioception_; }
    
    /**
     * @brief Set proprioception data
     * 
     * @param p New proprioception data vector
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }

    // ============================================================================
    // Audio Sensory System
    // ============================================================================
    
    /**
     * @brief Get audio-like signals
     * 
     * Returns audio-like signals for auditory processing. By default, this is
     * empty (size 0) as NLM focuses primarily on visual and tactile processing.
     * Can be enabled for environments with sound.
     * 
     * @return Constant reference to audio data vector
     */
    [[nodiscard]] const std::vector<float>& getAudio() const noexcept { return audio_; }
    
    /**
     * @brief Set audio-like signals
     * 
     * @param a New audio data vector
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }

    // ============================================================================
    // Integration Methods
    // ============================================================================
    
    /**
     * @brief Get all sensory signals concatenated for brain input
     * 
     * Returns a single vector containing all sensory signals in a consistent
     * order: vision, touch, internal, proprioception, audio. This is the
     * standard format for brain input.
     * 
     * @return Vector containing all sensory signals concatenated
     */
    std::vector<float> getAllSignals() const;

    // ============================================================================
    // Temporal Information
    // ============================================================================
    
    /**
     * @brief Get sensory percept timestamp
     * 
     * Returns the simulation time at which this sensory percept was generated.
     * This is important for temporal processing and novel event detection.
     * 
     * @return Simulation time when percept was generated
     */
    [[nodiscard]] double getTimestamp() const noexcept { return timestamp_; }
    
    /**
     * @brief Set sensory percept timestamp
     * 
     * @param t Simulation time when percept was generated
     */
    void setTimestamp(double t) noexcept { timestamp_ = t; }

    // ============================================================================
    // Comparison and Utility
    // ============================================================================
    
    /**
     * @brief Equality comparison operator
     * 
     * Compares sensory data for equality (useful for testing and debugging).
     * Note: This compares only the data values, not metadata like timestamps.
     * 
     * @param other The other SensoryPercept to compare against
     * @return True if all sensory data is identical
     */
    bool operator==(const SensoryPercept& other) const;
    
    /**
     * @brief Inequality comparison operator
     * 
     * @param other The other SensoryPercept to compare against
     * @return True if sensory data differs
     */
    bool operator!=(const SensoryPercept& other) const { return !(*this == other); }

private:
    // ============================================================================
    // Vision System Storage
    // ============================================================================
    std::vector<float> vision_;        //!< Grayscale vision grid (visionWidth_ × visionHeight_ values)
    size_t visionWidth_;                //!< Vision grid width in pixels (typically 16)
    size_t visionHeight_;               //!< Vision grid height in pixels (typically 16)
    
    // ============================================================================
    // Touch System Storage
    // ============================================================================
    std::vector<float> touch_;          //!< Touch/proximity sensor data (typically 8 values)
    
    // ============================================================================
    // Internal State System Storage
    // ============================================================================
    std::vector<float> internal_;       //!< Internal homeostatic signals (typically 4 values)
    
    // ============================================================================
    // Proprioception System Storage
    // ============================================================================
    std::vector<float> proprioception_; //!< Body position/velocity signals (typically 6 values)
    
    // ============================================================================
    // Audio System Storage
    // ============================================================================
    std::vector<float> audio_;           //!< Audio-like signals (typically empty)
    
    // ============================================================================
    // Temporal Information
    // ============================================================================
    double timestamp_;                   //!< Simulation time when this percept was generated
};

} // namespace nlm
