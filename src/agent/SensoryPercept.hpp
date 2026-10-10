#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @file SensoryPercept.hpp
 * 
 * Sensory data packet for NLM
 * 
 * This file provides the sensory data packet structure used by NLM to
 * transmit sensory information from the agent's body to the brain.
 * The sensory packet contains only low-level signals without semantic
 * interpretation or object labels.
 * 
 * Key features:
 * - **Low-level signals**: Only raw sensory data, no semantic labels
 * - **Modality separation**: Different sensory channels (vision, touch, etc.)
 * - **Timestamped**: Each sensory packet has a timestamp
 * - **Brain-input compatible**: Can be concatenated for brain input
 * - **Body-state independent**: No reference to body state or context
 * 
 * Sensory packet characteristics:
 * - **Vision**: Grayscale intensity grid (no RGB, no object labels)
 * - **Touch**: Binary collision signals
 * - **Internal**: Homeostatic signals
 * - **Proprioception**: Body position/velocity signals
 * - **Audio**: Audio-like signals (if enabled)
 * 
 * @note Critical for sensory transduction and brain communication
 * @note Used extensively in agent perception systems
 * @note Essential for forming internal world model
 * @see AgentBrain.hpp - For brain interaction
 * @see AgentBody.hpp - For agent state
 * @see SensoryInput.hpp - For sensory input system
 * @see SensoryPercept.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class SensoryPercept
 * 
 * Neural implementation of sensory data packets.
 * 
 * This class implements the sensory data packet structure used by NLM to
 * transmit sensory information from the agent's body to the brain. It
 * contains only low-level sensory signals without semantic interpretation
 * or object labels, providing a raw data stream for neural processing.
 * 
 * Sensory perception characteristics in NLM:
 * - **Low-level representation**: Only raw sensory data, no semantic labels
 * - **Modality separation**: Different sensory channels (vision, touch, etc.)
 * - **Timestamped**: Each sensory packet has a timestamp
 * - **Brain-input compatible**: Can be concatenated for brain input
 * - **Body-state independent**: No reference to body state or context
 * 
 * Key components:
 * - **Vision**: Grayscale intensity grid (no RGB, no object labels)
 * - **Touch**: Binary collision signals
 * - **Internal**: Homeostatic signals
 * - **Proprioception**: Body position/velocity signals
 * - **Audio**: Audio-like signals (if enabled)
 * 
 * @note Critical for sensory transduction and brain communication
 * @note Used extensively in agent perception systems
 * @note Essential for forming internal world model
 * @see AgentBrain.hpp - For brain interaction
 * @see AgentBody.hpp - For agent state
 * @see SensoryInput.hpp - For sensory input system
 * 
 * @author NLM Development Team
 */
class SensoryPercept {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default sensory percept instance with empty sensory data.
     * The sensory percept is initialized with default empty values for
     * all sensory channels.
     */
    SensoryPercept();
    
    /**
     * @brief Destructor
     * 
     * Cleans up sensory percept resources and performs any necessary
     * cleanup operations. The sensory percept is left in a valid but
     * non-operational state after destruction.
     */
    ~SensoryPercept() = default;
    
    /**
     * @brief Get vision signals
     * 
     * Returns the vision sensory data as a vector of grayscale intensities.
     * This represents a simple grayscale intensity grid without RGB values
     * or object labels, formatted in row-major order with values 0.0-1.0.
     */
    const std::vector<float>& getVision() const { return vision_; }
    
    /**
     * @brief Set vision signals
     * 
     * Sets the vision sensory data to the provided vector of grayscale intensities.
     * This updates the vision sensory channel for the current percept.
     */
    void setVision(const std::vector<float>& v) { vision_ = v; }
    
    /**
     * @brief Get vision width
     * 
     * Returns the width of the vision grid in pixels.
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * @brief Get vision height
     * 
     * Returns the height of the vision grid in pixels.
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * @brief Get touch signals
     * 
     * Returns the touch sensory data as a vector of binary collision signals.
     * This represents proximity sensor readings.
     */
    const std::vector<float>& getTouch() const { return touch_; }
    
    /**
     * @brief Set touch signals
     * 
     * Sets the touch sensory data to the provided vector of binary collision signals.
     * This updates the touch sensory channel for the current percept.
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    /**
     * @brief Get internal signals
     * 
     * Returns the internal sensory data as a vector of homeostatic signals.
     * This represents the agent's internal state (energy, health, etc.).
     */
    const std::vector<float>& getInternal() const { return internal_; }
    
    /**
     * @brief Set internal signals
     * 
     * Sets the internal sensory data to the provided vector of homeostatic signals.
     * This updates the internal sensory channel for the current percept.
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    /**
     * @brief Get proprioception signals
     * 
     * Returns the proprioception sensory data as a vector of body position/velocity signals.
     * This represents the agent's body state (position, velocity, orientation).
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    
    /**
     * @brief Set proprioception signals
     * 
     * Sets the proprioception sensory data to the provided vector of body position/velocity signals.
     * This updates the proprioception sensory channel for the current percept.
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    /**
     * @brief Get audio-like signals
     * 
     * Returns the audio sensory data as a vector of audio-like signals.
     * This represents audio input (if enabled).
     */
    const std::vector<float>& getAudio() const { return audio_; }
    
    /**
     * @brief Set audio-like signals
     * 
     * Sets the audio sensory data to the provided vector of audio-like signals.
     * This updates the audio sensory channel for the current percept.
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    /**
     * @brief Get all signals concatenated
     * 
     * Returns all sensory signals concatenated into a single vector for brain input.
     * This provides a unified sensory stream for neural processing.
     */
    std::vector<float> getAllSignals() const;
    
    /**
     * @brief Get timestamp
     * 
     * Returns the timestamp of this sensory percept.
     */
    double getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Set timestamp
     * 
     * Sets the timestamp of this sensory percept.
     */
    void setTimestamp(double t) { timestamp_ = t; }
    
private:
    /** Vision sensory data (grayscale intensity grid) */
    std::vector<float> vision_;
    
    /** Vision grid width in pixels */
    size_t visionWidth_;
    
    /** Vision grid height in pixels */
    size_t visionHeight_;
    
    /** Touch/proximity sensory data */
    std::vector<float> touch_;
    
    /** Internal state sensory data */
    std::vector<float> internal_;
    
    /** Proprioception sensory data (body position/velocity) */
    std::vector<float> proprioception_;
    
    /** Audio sensory data */
    std::vector<float> audio_;
    
    /** Sensory packet timestamp */
    double timestamp_;
};

} // namespace nlm
