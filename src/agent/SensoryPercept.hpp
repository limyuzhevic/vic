#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

/**
 * @brief Builder class for constructing SensoryPercept instances
 * 
 * Provides a fluent interface for building complex sensory percepts
 * with validation and proper initialization.
 */
class SensoryPerceptBuilder {
public:
    /**
     * @brief Add vision data to the builder
     * 
     * @param data Vision data vector (must match dimensions)
     * @return Reference to this builder for chaining
     */
    SensoryPerceptBuilder& addVision(const std::vector<float>& data);
    
    /**
     * @brief Add touch sensor data
     * 
     * @param data Touch data vector (typically binary collision signals)
     * @return Reference to this builder for chaining
     */
    SensoryPerceptBuilder& addTouch(const std::vector<float>& data);
    
    /**
     * @brief Add internal state signals
     * 
     * @param data Internal signals (homeostatic signals)
     * @return Reference to this builder for chaining
     */
    SensoryPerceptBuilder& addInternal(const std::vector<float>& data);
    
    /**
     * @brief Add proprioception signals
     * 
     * @param data Proprioception data (body position/velocity)
     * @return Reference to this builder for chaining
     */
    SensoryPerceptBuilder& addProprioception(const std::vector<float>& data);
    
    /**
     * @brief Add audio-like signals
     * 
     * @param data Audio data (if audio enabled)
     * @return Reference to this builder for chaining
     */
    SensoryPerceptBuilder& addAudio(const std::vector<float>& data);
    
    /**
     * @brief Set vision dimensions
     * 
     * @param width Vision width in pixels
     * @param height Vision height in pixels
     * @return Reference to this builder for chaining
     */
    SensoryPerceptBuilder& setDimensions(size_t width, size_t height);
    
    /**
     * @brief Build the SensoryPercept instance
     * 
     * @return Constructed SensoryPercept
     */
    SensoryPercept build();
    
    /**
     * @brief Validate builder state before building
     * 
     * @return True if builder has all required data
     */
    bool validate() const;
    
private:
    size_t visionWidth_ = 16;
    size_t visionHeight_ = 16;
    std::vector<float> visionData_;
    std::vector<float> touchData_;
    std::vector<float> internalData_;
    std::vector<float> proprioceptionData_;
    std::vector<float> audioData_;
};

/**
 * @brief Sensory data packet - what the agent perceives
 * 
 * Contains only low-level sensory signals, NO semantic labels.
 * Represents the raw input from the world's sensors.
 * 
 * The SensoryPercept serves as the interface between the world
 * and the brain's sensory processing systems. All data is kept
 * in low-level, biologically plausible formats.
 */
class SensoryPercept {
public:
    /**
     * @brief Default constructor with 16x16 vision
     * 
     * Creates a sensory percept with default dimensions.
     */
    SensoryPercept();
    
    /**
     * @brief Constructor with configurable vision dimensions
     * 
     * @param visionWidth Width of vision grid (default: 16)
     * @param visionHeight Height of vision grid (default: 16)
     */
    explicit SensoryPercept(size_t visionWidth, size_t visionHeight);
    
    /**
     * @brief Destructor
     */
    ~SensoryPercept() = default;
    
    /**
     * @brief Get vision data
     * 
     * @return Constant reference to vision vector
     * Vision is a grayscale intensity grid (no RGB, no object labels).
     * Values range from 0.0 to 1.0, format is row-major.
     */
    const std::vector<float>& getVision() const { return vision_; }
    
    /**
     * @brief Set vision data with validation
     * 
     * @param v Vision data vector (must match dimensions)
     */
    void setVision(const std::vector<float>& v);
    
    /**
     * @brief Get vision width
     * 
     * @return Width of vision grid in pixels
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * @brief Get vision height
     * 
     * @return Height of vision grid in pixels
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * @brief Get touch/proximity sensor data
     * 
     * @return Constant reference to touch vector
     * Touch signals are binary collision signals from the environment.
     */
    const std::vector<float>& getTouch() const { return touch_; }
    
    /**
     * @brief Set touch sensor data
     * 
     * @param t Touch data vector
     */
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    /**
     * @brief Get internal state signals
     * 
     * @return Constant reference to internal vector
     * Internal signals represent homeostatic signals (energy, health, etc.).
     */
    const std::vector<float>& getInternal() const { return internal_; }
    
    /**
     * @brief Set internal state signals
     * 
     * @param i Internal data vector
     */
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    /**
     * @brief Get proprioception signals
     * 
     * @return Constant reference to proprioception vector
     * Proprioception represents body position and velocity signals.
     */
    const std::vector<float>& getProprioception() const { return proprioception_; }
    
    /**
     * @brief Set proprioception signals
     * 
     * @param p Proprioception data vector
     */
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    /**
     * @brief Get audio-like signals
     * 
     * @return Constant reference to audio vector
     * Audio signals (if enabled) represent sound information.
     */
    const std::vector<float>& getAudio() const { return audio_; }
    
    /**
     * @brief Set audio-like signals
     * 
     * @param a Audio data vector
     */
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    /**
     * @brief Get all sensory signals concatenated
     * 
     * @return Vector containing all signals (vision + touch + internal + proprioception + audio)
     * Useful for providing input to the brain processing system.
     */
    std::vector<float> getAllSignals() const;
    
    /**
     * @brief Get sensory timestamp
     * 
     * @return Timestamp when this sensory percept was created
     */
    double getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Set sensory timestamp
     * 
     * @param t Timestamp value
     */
    void setTimestamp(double t) { timestamp_ = t; }
    
    /**
     * @brief Validate sensory percept integrity
     * 
     * @return True if all data is consistent and valid
     * Checks for dimension mismatches, NaN values, and data ranges.
     */
    bool validate() const;
    
    /**
     * @brief Get string representation for debugging
     * 
     * @return Formatted string describing the percept
     */
    std::string to_string() const;
    
    /**
     * @brief Create a builder for constructing SensoryPercept instances
     * 
     * @return Builder instance for fluent construction
     */
    static SensoryPerceptBuilder create();
    
private:
    // Helper methods for validation
    bool validateVision() const;
    bool validateTouch() const;
    bool validateInternal() const;
    bool validateProprioception() const;
    bool validateAudio() const;
    
    // Vision data (16x16 by default)
    std::vector<float> vision_;
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Other sensory modalities
    std::vector<float> touch_;
    std::vector<float> internal_;
    std::vector<float> proprioception_;
    std::vector<float> audio_;
    
    // Timestamp for temporal sequencing
    double timestamp_;
};

} // namespace nlm
