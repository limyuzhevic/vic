/**
 * @file SimpleWorld.hpp
 * @brief Simple 2D world simulation for NLM development
 * 
 * This file defines the SimpleWorld class which provides a simulated 2D environment
 * for NLM development. The world offers basic sensory experiences without semantic
 * labels, making it ideal for studying neural learning and development.
 * 
 * The SimpleWorld simulates:
 * - Physical geometry with configurable dimensions
 * - Dynamic objects (resources, hazards, agents)
 * - Basic agent sensing (vision) and interaction capabilities
 * - Energy management and decay dynamics
 * - Time-based state evolution
 * - Basic physics (collisions, movement)
 * 
 * @note This world implementation is deliberately simple and lacks semantic labels
 *       to focus on neural learning rather than environmental understanding.
 *       It's designed as a developmental environment for early-stage neural systems.
 * 
 * @warning The SimpleWorld maintains internal state that evolves over time. The
 *         update() method must be called regularly to maintain consistent physics
 *         and state. Failure to call update() may result in stale sensory data or
 *         inconsistent physics simulation.
 * 
 * @see AgentBrain.hpp - Interface for agent interaction with world
 * @see WorldObject - Individual objects in the world
 * @see AgentBody.hpp - Physical representation of agent in world
 * @see SensoryPercept.hpp - Sensory input from world
 */

#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// World object - represents an object in the world
// Position and type only - no semantic labels
/**
 * @brief Represents an object in the world with basic physical properties
 * 
 * WorldObject defines a simple 2D object in the simulated world. It contains
 * only positional, geometric, and type information without semantic labels like
 * "food" or "wall". This minimalism forces neural systems to learn patterns from
 * direct interaction rather than pre-defined semantics.
 * 
 * @note Objects can be resources (providing energy), hazards (causing damage),
 *       or empty space. The neural system must learn these relationships through
 *       interaction.
 * 
 * @warning Object positions are continuous (floating-point) to provide smooth
 *         movement and interaction. Rendering systems may need to quantize
 *         positions for display purposes.
 * 
 * @see SimpleWorld - Manages world objects and their interactions
 */
struct WorldObject {
    /// X-coordinate position in world space
    float x;
    
    /// Y-coordinate position in world space
    float y;
    
    /// Radius of the object (useful for circular objects)
    float radius;
    
    /// Type of object from WorldObjectType enum
    WorldObjectType type;
    
    /// Reward value (positive for resources, negative for hazards)
    float value;
    
    /// Whether the object is currently active
    bool active;
    
    /// Default constructor creates an empty object at origin
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    /// Construct a world object with specified properties
    ///
    /// @param px X-coordinate position
    /// @param py Y-coordinate position
    /// @param t Object type from WorldObjectType enum
    /// @param v Reward value (default: 0.0f)
    /// @param r Object radius (default: 0.5f)
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
/**
 * @brief Simple 2D world simulation environment for agent development
 * 
 * SimpleWorld provides a simulated environment where neural agents can develop
 * through interaction. It offers:
 * - Basic physics simulation (collisions, movement)
 * - Sensory input (vision) without semantic interpretation
 * - Object interaction (resources, hazards)
 * - Energy management with decay dynamics
 * - Reproducible execution with random seeds
 * 
 * The world is deliberately minimal, providing raw sensory data rather than
 * interpreted semantic information. This forces neural systems to learn patterns
 * from direct experience.
 * 
 * @note This world implementation is ideal for:
 * - Studying neural development and learning
 * - Testing prediction systems
 * - Developing curiosity-driven exploration
 * - Implementing neuromodulation based on rewards
 * 
 * @warning The SimpleWorld maintains internal state that evolves. The update()
 *         method must be called at regular intervals to maintain consistent
 *         physics and sensory data. Skipping update() calls may result in stale
 *         perception or physics inconsistencies.
 * 
 * @see AgentBrain.hpp - Interface for neural agents to interact with world
 * @see WorldObject - Individual objects within the world
 * @see AgentBrain::processSensoryInput() - How agents receive world input
 * @see ActionResult - Results of agent actions in world
 */
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    // Reset world to initial state
    void reset();
    
    // Set agent starting position
    void setAgentStart(float x, float y);
    
    // Update world state (call each simulation step)
    void update(double timestep);
    
    // Apply motor command to agent
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    
    // Get current sensory percept for the agent
    const SensoryPercept& getSensoryPercept() const { return sensory_; }
    
    // Get agent body state
    const AgentBody& getAgentBody() const { return agent_; }
    
    // Add object to world
    void addObject(const WorldObject& obj);
    
    // Remove object at position
    void removeObject(float x, float y);
    
    // Check if position is valid (not wall)
    bool isValidPosition(float x, float y) const;
    
    // Get object at position (if any)
    WorldObject* getObjectAt(float x, float y);
    
    // World dimensions
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    
    // Energy settings
    float getMaxEnergy() const { return maxEnergy_; }
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    float getEnergyDecayRate() const { return energyDecayRate_; }
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    
    // Time
    double getSimulationTime() const { return simTime_; }
    
    // Seed for reproducibility
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const { return rngSeed_; }
    
private:
    // Generate vision from current view
    void generateVision();
    
    // Check for collisions
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
    
    // World geometry
    float width_;
    float height_;
    
    // Agent
    AgentBody agent_;
    
    // Objects in world
    std::vector<WorldObject> objects_;
    
    // Sensory state
    SensoryPercept sensory_;
    
    // Vision configuration
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Energy system
    float maxEnergy_;
    float energyDecayRate_;
    float resourceEnergyGain_;
    
    // Simulation time
    double simTime_;
    
    // Random seed
    uint64_t rngSeed_;
    
    // Simple RNG state
    uint64_t rngState_;
    uint64_t nextRandom();
};

} // namespace nlm
