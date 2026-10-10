#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// World object - represents an object in the world
//
// A WorldObject represents any physical object in the world environment.
// Objects have geometric properties (position, size) and physical properties
// (type, value). Types include resources (food, coins), hazards (walls, obstacles),
// and special objects (goals, agents). Objects can be active or inactive.
//
// Value interpretation:
// - Positive values: reward resources (food, coins, gems)
// - Zero values: neutral objects (empty spaces, markers)
// - Negative values: hazards (damage, traps, hazards)
// - Special values: goals (typically positive, achievement-based)
//
// Usage example:
// @code
// // Create a resource object (food)
// WorldObject food(5.0f, 3.0f, WorldObjectType::Resource, 10.0f);
// 
// // Create a hazard object (wall)
// WorldObject wall(2.0f, 0.0f, WorldObjectType::Hazard, -5.0f);
// 
// // Create a goal object
// WorldObject goal(10.0f, 10.0f, WorldObjectType::Goal, 100.0f);
// @endcode
struct WorldObject {
    float x;                         ///< X coordinate in world space (meters)
    float y;                         ///< Y coordinate in world space (meters)
    float radius;                    ///< Physical radius (meters) for collision detection
    WorldObjectType type;            ///< Object type (Resource, Hazard, Goal, Empty)
    float value;                      ///< Value: reward (positive), damage (negative), or neutral (0)
    bool active;                      ///< Whether object is present and interactable
    
    /// Default constructor - creates empty object at origin
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    /**
     * Constructor with all parameters
     * 
     * @param px X coordinate
     * @param py Y coordinate  
     * @param t Object type
     * @param v Object value (reward/damage)
     * @param r Object radius
     */
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

/**
 * Simple 2D world for NLM development
 * 
 * The SimpleWorld provides a minimal physics simulation environment for testing
 * and developing neural learning machines. It offers:
 * 
 * - 2D continuous space with geometric objects
 * - Sensorimotor loops (vision, touch, proprioception)
 * - Energy-based motivation (survival, resource seeking)
 * - Collision detection and physical interactions
 * - Random event generation (object creation/destruction)
 * - Reproducible simulation with configurable seeds
 * 
 * Key design principles:
 * - No semantic labels - only low-level sensory signals
 * - Physics-based interactions (collision, energy transfer)
 * - Simple state space for tractable learning
 * - Real-time execution for interactive development
 * 
 * Usage example:
 * @code
 * // Create and configure world
 * auto world = std::make_unique<SimpleWorld>();
 * world->configure(20.0f, 20.0f, 10, 10);  // 20x20m world, 10x10 vision grid
 * world->setMaxEnergy(100.0f);
 * world->setEnergyDecayRate(0.01f);
 * world->setRandomSeed(12345);
 * 
 * // Add objects to world
 * world->addObject(WorldObject(5.0f, 5.0f, WorldObjectType::Resource, 20.0f));
 * world->addObject(WorldObject(15.0f, 15.0f, WorldObjectType::Hazard, -10.0f));
 * 
 * // Run simulation loop
 * world->setAgentStart(10.0f, 10.0f);
 * for (int step = 0; step < 1000; ++step) {
 *     double timestep = 0.1;  // 100ms simulation steps
 *     world->update(timestep);
 *     
 *     // Get sensory input for agent
 *     auto percept = world->getSensoryPercept();
 *     
 *     // Get current action for agent
 *     auto action = agentBrain->decodeMotorCommand();
 *     
 *     // Apply action in world
 *     auto result = world->applyMotorCommand(action, step * timestep);
 *     
 *     // Apply neuromodulation based on result
 *     agentBrain->applyRewardModulation(result.reward, result.predictedReward);
 * }
 * @endcode
 */
class SimpleWorld {
public:
    /**
     * Constructor - creates world with default parameters
     * 
     * Creates an empty world with default settings:
     * - 100x100 meter dimensions
     * - No initial objects
     * - Default energy settings
     * - Zero simulation time
     */
    SimpleWorld();

    /**
     * Destructor - cleans up world resources
     */
    ~SimpleWorld();

    // Configuration and setup

    /**
     * Configure world dimensions and sensory capabilities
     * 
     * Sets up the basic parameters for world simulation:
     * - Physical world dimensions
     * - Agent's sensory resolution (vision grid size)
     * - Default energy settings
     * 
     * @param width World width in meters
     * @param height World height in meters
     * @param visionWidth Vision grid width (pixels)
     * @param visionHeight Vision grid height (pixels)
     */
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);

    /**
     * Reset world to initial state
     * 
     * Restores world to its configured initial state:
     * - Clears all objects
     * - Resets agent to starting position
     * - Resets simulation time
     * - Regenerates random objects (if random seed set)
     * 
     * @see setRandomSeed()
     */
    void reset();

    /**
     * Set agent starting position
     * 
     * Defines where the agent begins in the world. Called automatically during
     * reset() and can be called manually for specific starting conditions.
     * 
     * @param x Starting X coordinate
     * @param y Starting Y coordinate
     */
    void setAgentStart(float x, float y);

    // Simulation loop

    /**
     * Update world state (main simulation step)
     * 
     * Performs one timestep of world physics and dynamics:
     * 1. Generate agent's sensory percept (vision, touch, etc.)
     * 2. Update agent state (energy decay, position)
     * 3. Check for object interactions (collisions)
     * 4. Execute scheduled events (object creation/destruction)
     * 5. Increment simulation time
     * 
     * @param timestep Simulation time step (seconds)
     */
    void update(double timestep);

    /**
     * Apply motor command to agent
     * 
     * Executes an action command in the world environment:
     * 1. Validates command (bounds checking, feasibility)
     * 2. Updates agent position and energy
     * 3. Checks for object interactions (collision, consumption, etc.)
     * 4. Applies action effects (movement, resource gain/loss)
     * 5. Returns outcome information for learning
     * 
     * @param cmd Motor command to execute
     * @param currentTime Current simulation time (for timing-dependent effects)
     * @return ActionResult with outcome details (reward, damage, state changes)
     */
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);

    // Sensor access

    /**
     * Get current sensory percept for the agent
     * 
     * Returns the agent's current sensory state, including:
     * - Vision: grayscale intensity grid (no object labels)
     * - Touch: proximity sensor signals
     * - Internal: homeostatic state signals
     * - Proprioception: body position/velocity
     * 
     * @return Current sensory percept
     */
    const SensoryPercept& getSensoryPercept() const { return sensory_; }

    /**
     * Get agent body state
     * 
     * @return Current agent body state (position, velocity, energy)
     */
    const AgentBody& getAgentBody() const { return agent_; }

    // Object management

    /**
     * Add object to world
     * 
     * Creates a new world object with the specified properties.
     * Objects are stored in a list for efficient access during simulation.
     * 
     * @param obj Object to add
     * 
     * @see removeObject()
     */
    void addObject(const WorldObject& obj);

    /**
     * Remove object at position
     * 
     * Removes any world object at the specified coordinates.
     * If multiple objects occupy the same position, only one is removed.
     * 
     * @param x X coordinate
     * @param y Y coordinate
     */
    void removeObject(float x, float y);

    /**
     * Check if position is valid (not wall)
     * 
     * Tests whether a position is free of obstacles:
     * - Checks for collision with existing objects
     * - Considers object radii for collision detection
     * - Returns true if position is walkable
     * 
     * @param x X coordinate to test
     * @param y Y coordinate to test
     * @return true if position is valid, false if obstacle present
     */
    bool isValidPosition(float x, float y) const;

    /**
     * Get object at position (if any)
     * 
     * Finds and returns the world object at the specified coordinates.
     * Uses exact position matching (within object radius).
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Pointer to object at position, or nullptr if none
     */
    WorldObject* getObjectAt(float x, float y);

    // Configuration access

    /**
     * Get world width
     * 
     * @return World width in meters
     */
    float getWidth() const { return width_; }

    /**
     * Get world height
     * 
     * @return World height in meters
     */
    float getHeight() const { return height_; }

    // Energy system configuration

    /**
     * Get maximum energy
     * 
     * @return Maximum energy capacity
     */
    float getMaxEnergy() const { return maxEnergy_; }

    /**
     * Set maximum energy
     * 
     * @param e New maximum energy
     */
    void setMaxEnergy(float e) { maxEnergy_ = e; }

    /**
     * Get energy decay rate
     * 
     * @return Energy loss per timestep (0.0 to 1.0)
     */
    float getEnergyDecayRate() const { return energyDecayRate_; }

    /**
     * Set energy decay rate
     * 
     * @param r New decay rate
     */
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }

    // Time management

    /**
     * Get current simulation time
     * 
     * @return Current simulation time in seconds
     */
    double getSimulationTime() const { return simTime_; }

    /**
     * Set random seed
     * 
     * Sets the random seed for reproducible object generation and events.
     * Call reset() after setting to regenerate world with new seed.
     * 
     * @param seed Random seed value
     * 
     * @see reset()
     */
    void setRandomSeed(uint64_t seed);

    /**
     * Get random seed
     * 
     * @return Current random seed
     */
    uint64_t getRandomSeed() const { return rngSeed_; }

private:
    // Core world physics and dynamics

    /**
     * Generate vision from current view
     * 
     * Computes the agent's visual percept based on:
     * - Agent's position and orientation
     * - World object positions and properties
     * - Vision field of view and resolution
     * - Distance attenuation and occlusion
     * 
     * @note Vision is grayscale intensity, no object labels
     * @see generateVision()
     */
    void generateVision();

    /**
     * Check for collisions
     * 
     * Detects and processes collisions between agent and world objects:
     * - Geometric intersection testing
     * - Collision response (bounce, damage, etc.)
     * - Energy transfer effects
     * - State changes (object destruction, pickup)
     * 
     * @see checkCollisions()
     */
    void checkCollisions();

    /**
     * Interact with nearby objects
     * 
     * Processes interactions between agent and nearby objects:
     * - Resource pickup (positive value objects)
     * - Hazard damage (negative value objects)
     * - Goal achievement (special objects)
     * - Environmental interactions (switches, triggers)
     * 
     * @return ActionResult with interaction outcomes
     */
    ActionResult interact();

    // World state

    float width_;                    ///< World width (meters)
    float height_;                   ///< World height (meters)

    // Agent state
    AgentBody agent_;                ///< Agent body state (kinematics, energy)

    // Environment objects
    std::vector<WorldObject> objects_;///< All objects in world

    // Agent sensory state
    SensoryPercept sensory_;          ///< Current sensory percept

    // Vision configuration
    size_t visionWidth_;            ///< Vision grid width (pixels)
    size_t visionHeight_;           ///< Vision grid height (pixels)

    // Energy system
    float maxEnergy_;                ///< Maximum energy capacity
    float energyDecayRate_;          ///< Energy loss per timestep
    float resourceEnergyGain_;       ///< Energy gained from resources

    // Simulation time
    double simTime_;                 ///< Current simulation time (seconds)

    // Randomness
    uint64_t rngSeed_;               ///< Seed for reproducible randomness
    uint64_t rngState_;               ///< Current RNG state (for efficient generation)

    // Internal helper methods
    uint64_t nextRandom();            ///< Generate next random number for world events
};

} // namespace nlm
