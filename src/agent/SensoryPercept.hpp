#pragma once

#include <cstdint>
#include <string>

namespace nlm {

// Agent body state - contains physical properties and internal state
// This is part of the environment, not the brain
struct AgentBody {
    // Position in world
    float x;
    float y;
    
    // Orientation (radians)
    float orientation;
    
    // Velocity
    float velocityX;
    float velocityY;
    float angularVelocity;
    
    // Internal state
    float energy;          // Energy level (0-1)
    float health;          // Health (0-1)
    float age;             // Developmental age in simulation steps
    
    // Movement state
    bool isMoving;
    bool isTurning;
    float lastActionTime;
    
    AgentBody()
        : x(0.0f), y(0.0f), orientation(0.0f)
        , velocityX(0.0f), velocityY(0.0f), angularVelocity(0.0f)
        , energy(1.0f), health(1.0f), age(0.0f)
        , isMoving(false), isTurning(false), lastActionTime(0.0f) {}
    
    void reset() {
        x = 0.0f;
        y = 0.0f;
        orientation = 0.0f;
        velocityX = 0.0f;
        velocityY = 0.0f;
        angularVelocity = 0.0f;
        energy = 1.0f;
        health = 1.0f;
        age = 0.0f;
        isMoving = false;
        isTurning = false;
        lastActionTime = 0.0f;
    }
};

// Low-level motor commands that the brain can produce
// These are innate muscle-like signals, not semantic actions
enum class MotorCommand {
    MoveForward = 0,   // Propel forward
    MoveBackward = 1,  // Propel backward  
    TurnLeft = 2,      // Rotate left
    TurnRight = 3,     // Rotate right
    LookLeft = 4,      // Pan sensor left
    LookRight = 5,     // Pan sensor right
    Interact = 6,      // Interact with nearby object
    Wait = 7           // No movement
};

// Action result from the world's perspective
struct ActionResult {
    float reward;
    bool success;
    std::string message;
    
    ActionResult() : reward(0.0f), success(false), message("") {}
    ActionResult(float r, bool s, const std::string& m = "") 
        : reward(r), success(s), message(m) {}
};

// Object types in the world (for rendering/debug only, NOT given to brain)
enum class WorldObjectType {
    Empty = 0,
    Resource = 1,      // Positive reward
    Hazard = 2,        // Negative reward
    Wall = 3,         // Boundary
    Marker = 4        // Navigation aid
};

} // namespace nlm

// Begin SensoryPercept class definitions
#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <vector>
#include <stdexcept>

namespace nlm {

// Sensory data packet - what the agent perceives
// Contains only low-level sensory signals, NO semantic labels
class SensoryPercept {
public:
    SensoryPercept();
    ~SensoryPercept() = default;
    
    // Initialize sensory data to default empty state
    void initialize();
    
    // Reset to initial state with timestamp = 0
    void reset();
    
    // Validate that all vectors have correct sizes
    void validateDimensions() const;
    
    // Vision: simple grayscale intensity grid (no RGB, no object labels)
    // Format: row-major, values 0.0-1.0
    const std::vector<float>& getVision() const { return vision_; }
    void setVision(const std::vector<float>& v) { vision_ = v; }
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Touch/proximity sensors (binary collision signals)
    const std::vector<float>& getTouch() const { return touch_; }
    void setTouch(const std::vector<float>& t) { touch_ = t; }
    
    // Internal state signals (homeostatic signals)
    const std::vector<float>& getInternal() const { return internal_; }
    void setInternal(const std::vector<float>& i) { internal_ = i; }
    
    // Proprioception: body position/velocity signals
    const std::vector<float>& getProprioception() const { return proprioception_; }
    void setProprioception(const std::vector<float>& p) { proprioception_ = p; }
    
    // Audio-like signals (if enabled)
    const std::vector<float>& getAudio() const { return audio_; }
    void setAudio(const std::vector<float>& a) { audio_ = a; }
    
    // Get all signals concatenated (for brain input)
    std::vector<float> getAllSignals() const;
    
    // Timestamp
    double getTimestamp() const { return timestamp_; }
    void setTimestamp(double t) { timestamp_ = t; }
    
        // Convenience method to get all signal dimensions
    size_t getTotalSignalCount() const {
        return vision_.size() + touch_.size() + internal_.size() + 
               proprioception_.size() + audio_.size();
    }
    
    // Copy constructor and assignment operator for safety
    SensoryPercept(const SensoryPercept&) = default;
    SensoryPercept& operator=(const SensoryPercept&) = default;
    
    // Move constructor and assignment operator
    SensoryPercept(SensoryPercept&&) = default;
    SensoryPercept& operator=(SensoryPercept&&) = default;
    
    // Validate dimensions match expected sizes
    bool isValid() const {
        return vision_.size() == 256 && touch_.size() == 8 && 
               internal_.size() == 4 && proprioception_.size() == 6;
    }
    
    // Get signal size for brain input planning
    static size_t getBrainInputSize() {
        return 256 + 8 + 4 + 6;  // Vision + Touch + Internal + Proprioception
    }
    
    // Check if percept contains valid data (not all zeros)
    bool hasValidData() const {
        return !vision_.empty() || !touch_.empty() || !internal_.empty() || 
               !proprioception_.empty() || !audio_.empty();
    }
    
    // Get percept type for debugging
    std::string getTypeDescription() const;
    
    // Check if percept is empty (all vectors empty)
    bool isEmpty() const;
    
    // Assignment operators with proper handling
    SensoryPercept& operator=(const SensoryPercept& other);
    SensoryPercept& operator=(SensoryPercept&& other);
};

} // namespace nlm
