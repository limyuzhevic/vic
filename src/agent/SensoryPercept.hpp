#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
//
// The SensoryPercept class represents the complete set of sensory inputs
// that an agent receives from its environment. This is the fundamental interface
// between the agent and the world, providing raw sensory data before any
cognitive processing or semantic interpretation.
//
// Key principles:
// - No semantic labels: Only physical quantities (intensity, position, velocity)
// - Low-level signals: Direct sensory measurements without object recognition
// - Multimodal input: Vision, touch, proprioception, internal state, audio
// - Time-stamped data: Each percept includes simulation timestamp
// - Concatenated format: All signals combined for brain input
//
// Sensory modalities:
// - Vision: Grayscale intensity grid (no RGB, no object labels)
// - Touch/Proximity: Binary collision signals (0.0-1.0)
// - Internal: Homeostatic signals (energy, health, metabolic state)
// - Proprioception: Body position, velocity, orientation
// - Audio: Sound intensity (if enabled)
//
// Usage example:
// @code
// // Create sensory percept from world observation
// auto percept = world->getSensoryPercept();
// 
// // Access individual sensory modalities
// std::vector<float> vision = percept.getVision();
// std::vector<float> touch = percept.getTouch();
// 
// // Get concatenated brain input
// std::vector<float> brainInput = percept.getAllSignals();
// 
// // Check timestamp for temporal processing
// double timestamp = percept.getTimestamp();
// @endcode
class SensoryPercept {
public:
    /**
     * Constructor - initializes empty sensory percept
     * 
     * Creates an empty sensory percept with all signal vectors initialized
     * to empty state. The percept timestamp is set to current simulation time.
     */
    SensoryPercept();

    /**
     * Destructor - cleans up sensory percept resources
     */
    ~SensoryPercept() = default;

    // Vision: simple grayscale intensity grid (no RGB, no object labels)

    /**
     * Get visual input grid
     * 
     * Returns the agent's visual percept as a 2D intensity grid.
     * Each value represents brightness (0.0 = black, 1.0 = white).
     * Grid is stored in row-major order (row by row, left to right).
     * 
     * @return Const reference to vision intensity grid
     * 
     * @note No object labels or semantic information - pure pixel intensity
     */
    const std::vector<float>& getVision() const { return vision_; }

    /**
     * Set visual input grid
     * 
     * Updates the agent's visual percept with new intensity values.
     * Validates that the grid matches configured vision dimensions.
     * 
     * @param v New vision intensity grid
     * 
     * @throws std::invalid_argument if grid dimensions don't match
     */
    void setVision(const std::vector<float>& v) { vision_ = v; }

    /**
     * Get vision grid width
     * 
     * @return Number of columns in vision grid
     */
    size_t getVisionWidth() const { return visionWidth_; }

    /**
     * Get vision grid height
     * 
     * @return Number of rows in vision grid
     */
    size_t getVisionHeight() const { return visionHeight_; }

    // Touch/proximity sensors (binary collision signals)

    /**
     * Get touch/proximity sensor signals
     * 
     * Returns signals from tactile sensors detecting nearby objects
     * and potential collisions. Values represent proximity level
     * (0.0 = no contact, 1.0 = touching/contact).
     * 
     * @return Const reference to touch sensor signals
     */
    const std::vector<float>& getTouch() const { return touch_; }

    /**
     * Set touch/proximity sensor signals
     * 
     * @param t Touch sensor signals vector
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }

    // Internal state signals (homeostatic signals)

    /**
     * Get internal homeostatic signals
     * 
     * Returns signals representing the agent's internal physiological state:
     * - Energy level (fuel for activity)
     * - Health status (damage, repair, recovery)
     * - Metabolic rate (activity level)
     * - Temperature (if applicable)
     * 
     * @return Const reference to internal state signals
     */
    const std::vector<float>& getInternal() const { return internal_; }

    /**
     * Set internal state signals
     * 
     * @param i Internal state signals vector
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }

    // Proprioception: body position/velocity signals

    /**
     * Get proprioceptive signals
     * 
     * Returns signals representing the agent's body state in the world:
     * - Position (x, y coordinates)
     * - Velocity (movement speed)
     * - Orientation (facing direction)
     * - Angular velocity (rotation speed)
     * 
     * @return Const reference to proprioceptive signals
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }

    /**
     * Set proprioceptive signals
     * 
     * @param p Proprioception signals vector
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }

    // Audio-like signals (if enabled)

    /**
     * Get audio-like signals
     * 
     * Returns sound intensity signals if audio sensing is enabled.
     * Values represent sound level (0.0 = silent, 1.0 = loud).
     * 
     * @return Const reference to audio signals (empty if audio disabled)
     */
    const std::vector<float>& getAudio() const { return audio_; }

    /**
     * Set audio-like signals
     * 
     * @param a Audio signals vector
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }

    // Sensor integration

    /**
     * Get all sensory signals concatenated (for brain input)
     * 
     * Concatenates all sensory modalities into a single vector for efficient
     * transmission to the brain. The order is fixed and must be consistent
     * with the brain's input expectations:
     * - Vision signals first
     * - Touch signals second
     * - Internal signals third
     * - Proprioception signals fourth
     * - Audio signals fifth (if enabled)
     * 
     * @return Concatenated sensory signals vector
     */
    std::vector<float> getAllSignals() const;

    // Time stamping

    /**
     * Get sensory percept timestamp
     * 
     * @return Simulation time when this percept was generated
     */
    double getTimestamp() const { return timestamp_; }

    /**
     * Set sensory percept timestamp
     * 
     * @param t Simulation time for this percept
     */
    void setTimestamp(double t) { timestamp_ = t; }

private:
    std::vector<float> vision_;          ///< Vision intensity grid
    size_t visionWidth_;                ///< Vision grid width (pixels)
    size_t visionHeight_;               ///< Vision grid height (pixels)
    
    std::vector<float> touch_;           ///< Touch/proximity sensor signals
    std::vector<float> internal_;       ///< Internal homeostatic signals
    std::vector<float> proprioception_; ///< Proprioceptive signals
    std::vector<float> audio_;          ///< Audio-like signals (if enabled)
    double timestamp_;                  ///< When this percept was generated
};

} // namespace nlm
