#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @brief Sensory data packet - what the agent perceives
 * 
 * The SensoryPercept class represents the complete set of low-level sensory
 * inputs that an agent receives from its environment. This data structure
 * captures the raw sensory signals that the agent's brain uses to understand
 * and interact with the world, without any semantic interpretation or object
 * labels.
 * 
 * The agent's perception is composed of multiple sensory modalities, each
 * providing different types of information about the environment:
 * 
 * - **Vision**: A 2D grid of grayscale intensity values representing visual
 *             scene appearance. No RGB channels, object identities, or semantic
 *             information are included - just raw pixel intensities.
 * 
 * - **Touch**: Binary collision/proximity sensor data indicating contact
 *            with objects in the environment. Each touch sensor provides a
 *            continuous value representing proximity or contact strength.
 * 
 * - **Internal**: Homeostatic and internal physiological signals that
 *             represent the agent's internal state, such as energy levels,
 *             hunger, or other regulatory signals.
 * 
 * - **Proprioception**: Body position, velocity, and orientation signals
 *               that provide feedback about the agent's own physical state.
 * 
 * - **Audio**: Sound sensor data (if implemented) representing acoustic
 *            information from the environment.
 * 
 * The sensory percept is time-stamped to allow for temporal processing
 * and change detection. All sensory signals are stored as floating-point
 * values in the range [0.0, 1.0] unless otherwise specified, facilitating
 * consistent processing across different modalities.
 * 
 * @note This class contains only low-level sensory signals - no semantic
 *       labels, object identities, or interpreted information is stored.
 *       Semantic processing and interpretation happens in higher-level
 *       brain modules.
 */
class SensoryPercept {
public:
    /**
     * @brief Construct a new SensoryPercept with default dimensions
     * 
     * Initializes all sensory buffers with default sizes and values,
     * preparing the agent for processing environmental inputs. The default
     * configuration provides a standard sensory envelope that matches the
     * agent's expected input requirements.
     */
    SensoryPercept();
    
    /**
     * @brief Destroy the SensoryPercept instance
     * 
     * The destructor performs no explicit cleanup as all member variables
     * use standard C++ containers that handle their own memory management.
     */
    ~SensoryPercept() = default;
    
    /**
     * @brief Get the visual input grid
     * 
     * Returns a reference to the 2D vision grid that represents the agent's
     * visual perception of the environment. The vision data is stored as a
     * flattened 1D vector in row-major order, with values typically in the
     * range [0.0, 1.0] representing intensity levels.
     * 
     * @return const reference to the vision vector
     */
    const std::vector<float>& getVision() const { return vision_; }
    
    /**
     * @brief Set the visual input grid
     * 
     * Updates the agent's visual perception with new vision data. The input
     * vector should have dimensions visionWidth_ × visionHeight_ and contain
     * intensity values typically in the range [0.0, 1.0].
     * 
     * @param v New vision vector to set
     */
    void setVision(const std::vector<float>& v) { vision_ = v; }
    
    /**
     * @brief Get the width of the vision grid
     * 
     * Returns the horizontal resolution of the visual field in pixels.
     * 
     * @return Vision grid width in pixels
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * @brief Get the height of the vision grid
     * 
     * Returns the vertical resolution of the visual field in pixels.
     * 
     * @return Vision grid height in pixels
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * @brief Get the touch sensor inputs
     * 
     * Returns a reference to the touch sensor vector containing proximity
     * and collision data. Each element represents the activation level of
     * a specific touch sensor, typically in the range [0.0, 1.0].
     * 
     * @return const reference to the touch sensor vector
     */
    const std::vector<float>& getTouch() const { return touch_; }
    
    /**
     * @brief Set the touch sensor inputs
     * 
     * Updates the touch sensor data with new proximity/collision values.
     * The input vector should contain values for all touch sensors in the
     * agent's sensory array.
     * 
     * @param t New touch sensor vector to set
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    /**
     * @brief Get the internal state signals
     * 
     * Returns a reference to the internal sensor vector containing homeostatic
     * and physiological state data. These signals represent the agent's internal
     * condition and regulatory needs, typically in the range [0.0, 1.0].
     * 
     * @return const reference to the internal sensor vector
     */
    const std::vector<float>& getInternal() const { return internal_; }
    
    /**
     * @brief Set the internal state signals
     * 
     * Updates the internal sensor data with new homeostatic values.
     * This includes signals related to energy, hunger, fatigue, and other
     * regulatory states that drive motivated behaviors.
     * 
     * @param i New internal sensor vector to set
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    /**
     * @brief Get the proprioceptive signals
     * 
     * Returns a reference to the proprioception vector containing body state
     * information including position, velocity, orientation, and other
     * proprioceptive data. These signals provide feedback about the agent's
     * own physical state and movement.
     * 
     * @return const reference to the proprioception vector
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    
    /**
     * @brief Set the proprioceptive signals
     * 
     * Updates the proprioceptive data with new body state information.
     * This includes position, velocity, angular position, and other kinematic
     * parameters that describe the agent's physical state.
     * 
     * @param p New proprioception vector to set
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    /**
     * @brief Get the audio-like signals
     * 
     * Returns a reference to the audio sensor vector containing acoustic
     * information from the environment. This sensory modality is optional
     * and may be empty if audio sensing is not implemented.
     * 
     * @return const reference to the audio sensor vector
     */
    const std::vector<float>& getAudio() const { return audio_; }
    
    /**
     * @brief Set the audio-like signals
     * 
     * Updates the audio sensor data with new acoustic information.
     * This is an optional sensory modality that may not be implemented
     * in all agent configurations.
     * 
     * @param a New audio sensor vector to set
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    /**
     * @brief Get all sensory signals concatenated into a single vector
     * 
     * Combines all available sensory modalities into one continuous vector
     * for easy access by the brain's input processing systems. The concatenated
     * order is: vision, touch, internal, proprioception, audio (if present).
     * 
     * @return Vector containing all sensory signals in order
     */
    std::vector<float> getAllSignals() const;
    
    /**
     * @brief Get the timestamp of this sensory percept
     * 
     * Returns the simulation time at which this sensory percept was created
     * or updated. This timestamp allows for temporal processing and change
     * detection across sensory inputs.
     * 
     * @return Simulation timestamp
     */
    double getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Set the timestamp of this sensory percept
     * 
     * Updates the time stamp associated with this sensory percept, useful
     * for temporal processing and synchronization with simulation steps.
     * 
     * @param t New timestamp value
     */
    void setTimestamp(double t) { timestamp_ = t; }
    
private:
    /**
     * @brief Visual input grid (16×16 = 256 values)
     * 
     * The agent's visual perception stored as a flattened 1D vector in
     * row-major order. Contains grayscale intensity values typically in the
     * range [0.0, 1.0]. This represents the agent's current view of the
     * environment without semantic interpretation.
     */
    std::vector<float> vision_;
    
    /**
     * @brief Vision grid width (always 16 for current implementation)
     */
    size_t visionWidth_;
    
    /**
     * @brief Vision grid height (always 16 for current implementation)
     */
    size_t visionHeight_;
    
    /**
     * @brief Touch/proximity sensor array (8 values)
     * 
     * Contains binary collision signals from the agent's touch sensors.
     * Each element represents the proximity or contact level with nearby
     * objects, typically in the range [0.0, 1.0].
     */
    std::vector<float> touch_;
    
    /**
     * @brief Internal state signals (4 values)
     * 
     * Contains homeostatic and regulatory signals representing the agent's
     * internal physiological state. These signals drive motivated behaviors
     * and survival needs.
     */
    std::vector<float> internal_;
    
    /**
     * @brief Proprioceptive signals (6 values)
     * 
     * Contains body position, velocity, and orientation signals providing
     * feedback about the agent's own physical state and movement dynamics.
     */
    std::vector<float> proprioception_;
    
    /**
     * @brief Audio sensor array (optional, may be empty)
     * 
     * Contains sound sensor data if audio sensing is implemented.
     * This is an optional sensory modality that may not be present
     * in all agent configurations.
     */
    std::vector<float> audio_;
    
    /**
     * @brief Time stamp for this sensory percept
     * 
     * The simulation time at which this percept was created or updated,
     * enabling temporal processing and change detection.
     */
    double timestamp_;
};

} // namespace nlm
