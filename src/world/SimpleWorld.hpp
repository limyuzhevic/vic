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
 * @struct WorldObject
 * @brief Represents a physical object in the simulated world.
 * 
 * WorldObject is a simple data structure representing objects in the simulated
 * world, including resources, hazards, and obstacles. The structure stores
 * geometric information (position, radius) and behavioral parameters (type,
 * value) without semantic interpretation.
 * 
 * This structure enables the world simulation to maintain a collection of
 * objects that can be interacted with by the agent, while keeping the semantic
 * interpretation of these objects separate from the world simulation itself.
 * 
 * @note World objects are position-based and their meaning is determined by
 *       their type and value attributes. Resource objects have positive values,
 *       hazard objects have negative values, and neutral objects have zero value.
 * 
 * @see SimpleWorld for world simulation that uses WorldObject
 * @see WorldObjectType for available object types
 */
struct WorldObject {
    float x;
    float y;
    float radius;
    WorldObjectType type;
    float value;        // Reward value (for resources) or damage (for hazards)
    bool active;
    
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
/**
 * @class SimpleWorld
 * @brief Simple 2D world simulation for neural learning and development.
 * 
 * SimpleWorld is an environment simulation that provides a 2D space for agents
 * to interact with physical objects. The world delivers sensory experience to
 * agents without semantic interpretation, allowing neural systems to develop
 * their own understanding of the environment through interaction.
 * 
 * The world simulation includes physics-based interactions, energy management,
 * vision generation, and collision detection. It serves as a test bed for
 * neural development, learning, and autonomous behavior development.
 * 
 * @note SimpleWorld provides a concrete implementation of the world interface
 *       that connects neural agents to environmental interactions. It maintains
 *       a collection of physical objects and provides sensory input based on
 *       agent perspective and proximity.
 * 
 * @see AgentBrain for the interface between neural computation and world interaction
 * @see SensoryPercept for sensory data format
 * @see WorldObject for world object representation
 */
class SimpleWorld {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty world simulation with default parameters. The world must
     * be configured using configure() before use.
     * 
     * @note The world starts in an unconfigured state and cannot be used until
     *       configure() is called to set world dimensions and vision parameters.
     */
    SimpleWorld();
    
    /**
     * @brief Default destructor
     * 
     * Cleans up world simulation resources and terminates any ongoing simulations.
     * The destructor ensures proper cleanup of all world resources.
     */
    ~SimpleWorld();
    
    // Configuration
    /**
     * @brief Configure world dimensions and vision parameters
     * 
     * Sets up the world simulation with physical dimensions and sensory capabilities.
     * This method must be called before any world simulation can occur.
     * 
     * @param width World width in simulation units
     * @param height World height in simulation units
     * @param visionWidth Width of agent's visual field in world units
     * @param visionHeight Height of agent's visual field in world units
     * 
     * @note Configuration establishes the physical and sensory boundaries of the
     *       world. The world dimensions determine the coordinate system for all
     *       object positions and agent movements.
     * 
     * @warning The world cannot be reconfigured after simulation has started.
     *         Configure before starting any world updates.
     * 
     * @see reset for resetting to configured state
     * @see update for world simulation steps
     */
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    // Reset world to initial state
    /**
     * @brief Reset world to initial state
     * 
     * Resets the world to its initial configuration state, clearing all accumulated
     * changes and preparing for a new simulation episode. This method restores
     * world geometry and object positions to their initial values.
     * 
     * @note Reset is useful for starting new simulation episodes, testing with
     *       clean environments, and ensuring reproducibility across runs.
     * 
     * @see configure for initial configuration
     * @see update for simulation steps
     * @see setAgentStart for setting new agent starting positions
     */
    void reset();
    
    // Set agent starting position
    /**
     * @brief Set agent starting position
     * 
     * Sets the agent's starting position in the world. The agent will be placed
     * at this position when the world is reset or at the start of a new episode.
     * 
     * @param x X-coordinate of agent starting position
     * @param y Y-coordinate of agent starting position
     * 
     * @note Starting position should be chosen to avoid immediate collisions with
     *       obstacles or hazards. Valid positions are within the configured world
     *       dimensions.
     * 
     * @see reset for using the starting position
     * @see isValidPosition for checking position validity
     * @see getAgentBody for retrieving current agent state
     */
    void setAgentStart(float x, float y);
    
    // Update world state (call each simulation step)
    /**
     * @brief Update world state for one simulation step
     * 
     * Advances the world simulation by one time step, updating all world dynamics
     * including agent movement, physics simulation, object interactions, and energy
     * management. This method should be called in a loop for continuous simulation.
     * 
     * @param timestep Duration of this simulation step in seconds
     * 
     * @note Update performs the core world simulation tasks:
     *       - Applies motor commands from agents
     *       - Updates agent physics and position
     *       - Manages energy decay and resource collection
     *       - Generates sensory percepts for agents
     *       - Handles collisions and interactions
     * 
     * @warning The timestep should be small enough to ensure numerical stability
     *         but large enough to be computationally efficient. Typical timesteps
     *         range from 0.001 to 0.1 seconds.
     * 
     * @see configure for initial setup
     * @see reset for resetting world state
     * @see applyMotorCommand for applying agent actions
     */
    void update(double timestep);
    
    // Apply motor command to agent
    /**
     * @brief Apply motor command to agent
     * 
     * Executes a motor command by applying it to the agent in the world.
     * This method handles the physics of agent movement and interaction
     * with world objects.
     * 
     * @param cmd Motor command containing action parameters (direction, speed,
     *            action type)
     * @param currentTime Current simulation time for timestamping interactions
     * 
     * @return ActionResult Result of the motor command application, including
     *         success status, energy changes, and object interactions
     * 
     * @retval ActionResult::Success Command applied successfully
     * @retval ActionResult::InvalidCommand Motor command is invalid or unsupported
     * @retval ActionResult::OutOfBounds Agent moved outside world boundaries
     * @retval ActionResult::Collision Agent collided with obstacle or hazard
     * 
     * @note This method processes the agent's action in the world physics model,
     *       updating agent position, handling collisions, and managing energy changes.
     * 
     * @see update for automatic motor command application
     * @see MotorCommand for motor command format
     * @see ActionResult for result codes
     */
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    
    // Get current sensory percept for the agent
    /**
     * @brief Get current sensory percept for the agent
     * 
     * Returns the current sensory percept that the agent should perceive from
     * the world. This method generates the sensory input based on agent position
     * and world state, providing the environmental information that the agent
     * uses for perception and decision-making.
     * 
     * @return const SensoryPercept& Reference to current sensory percept containing
     *         vision, touch, and other sensory modalities. This percept is updated
     *         automatically during world updates.
     * 
     * @note The sensory percept is generated each simulation step based on the
     *       agent's current position and viewing direction. It includes vision
     *               (visual field contents), touch (proximity to nearby objects), and
     *               other sensory modalities as configured.
     * 
     * @see AgentBrain::processSensoryInput for using sensory percept
     * @see generateVision for vision generation details
     */
    const SensoryPercept& getSensoryPercept() const { return sensory_; }
    
    // Get agent body state
    /**
     * @brief Get agent body state
     * 
     * Returns the current physical state of the agent in the world, including
     * position, velocity, energy level, and other physical attributes.
     * 
     * @return const AgentBody& Reference to current agent body state. This state
     *         is updated automatically during world simulation.
     * 
     * @note Agent body state includes position, velocity, energy level, and
     *               orientation. This information is essential for understanding the
     *               agent's current physical condition and capabilities.
     * 
     * @see setAgentStart for setting starting position
     * @see update for state updates during simulation
     * @see applyMotorCommand for state changes from actions
     */
    const AgentBody& getAgentBody() const { return agent_; }
    
    // Add object to world
    /**
     * @brief Add object to world
     * 
     * Adds a new object to the world simulation. The object will be included
     * in all world simulations and can be interacted with by agents.
     * 
     * @param obj World object to add to the simulation
     * 
     * @note Objects are stored in the world's object list and are available
     *               for all subsequent simulations. Objects with the same position
     *               will overwrite existing objects.
     * 
     * @see removeObject for removing objects
     * @see getObjectAt for retrieving objects
     * @see getWidth/getHeight for world boundary information
     */
    void addObject(const WorldObject& obj);
    
    // Remove object at position
    /**
     * @brief Remove object at position
     * 
     * Removes the object at the specified position from the world simulation.
     * This is useful for dynamically managing world objects during simulation.
     * 
     * @param x X-coordinate of position to remove object from
     * @param y Y-coordinate of position to remove object from
     * 
     * @note If no object exists at the specified position, this method has no effect.
     *       Objects are identified by their position in the world.
     * 
     * @see addObject for adding objects
     * @see getObjectAt for retrieving objects
     */
    void removeObject(float x, float y);
    
    // Check if position is valid (not wall)
    /**
     * @brief Check if position is valid (not wall)
     * 
     * Checks whether the specified position is valid for agent placement or
     * movement. Valid positions are those not occupied by walls or obstacles.
     * 
     * @param x X-coordinate to check
     * @param y Y-coordinate to check
     * 
     * @return bool true if position is valid (free space), false otherwise
     * 
     * @retval true Position is free of obstacles and valid for use
     * @retval false Position is occupied by wall, obstacle, or hazard
     * 
     * @note Wall positions are typically defined by the world configuration.
     *       This method helps agents avoid collisions and navigate the world.
     * 
     * @see setAgentStart for setting starting position
     * @see getObjectAt for checking specific objects at position
     */
    bool isValidPosition(float x, float y) const;
    
    // Get object at position (if any)
    /**
     * @brief Get object at position (if any)
     * 
     * Retrieves the world object at the specified position, if any. This method
     * is useful for querying the world state and understanding the environment.
     * 
     * @param x X-coordinate to query
     * @param y Y-coordinate to query
     * 
     * @return WorldObject* Pointer to world object at specified position, or nullptr
     *         if no object exists at that position.
     * 
     * @retval non-null Pointer to world object at specified position
     * @retval nullptr No object at specified position
     * 
     * @note This method provides read-only access to world objects. For modifying
     *               objects, use addObject() or removeObject().
     * 
     * @see addObject for adding objects
     * @see removeObject for removing objects
     * @see isValidPosition for checking position validity
     */
    WorldObject* getObjectAt(float x, float y);
    
    // World dimensions
    /**
     * @brief Get world width
     * 
     * Returns the configured width of the world simulation.
     * 
     * @return float World width in simulation units
     * 
     * @note World width is set during configuration and determines the maximum
     *       X-coordinate of valid positions in the world.
     * 
     * @see getHeight for world height
     * @see configure for setting world dimensions
     */
    float getWidth() const { return width_; }
    
    /**
     * @brief Get world height
     * 
     * Returns the configured height of the world simulation.
     * 
     * @return float World height in simulation units
     * 
     * @note World height is set during configuration and determines the maximum
     *       Y-coordinate of valid positions in the world.
     * 
     * @see getWidth for world width
     * @see configure for setting world dimensions
     */
    float getHeight() const { return height_; }
    
    // Energy settings
    /**
     * @brief Get maximum energy
     * 
     * Returns the maximum energy level the agent can have. This value is used
     * for energy normalization and maximum capacity.
     * 
     * @return float Maximum energy level
     * 
     * @note Maximum energy affects energy decay calculations and resource collection.
     *       It's a configuration parameter that can be adjusted to change energy dynamics.
     * 
     * @see setMaxEnergy for setting maximum energy
     * @see getEnergyDecayRate for energy decay rate
     */
    float getMaxEnergy() const { return maxEnergy_; }
    
    /**
     * @brief Set maximum energy
     * 
     * Sets the maximum energy level for the agent. This parameter affects energy
     * decay calculations and determines the maximum energy capacity.
     * 
     * @param e New maximum energy level
     * 
     * @note Changing maximum energy affects all energy calculations. It's typically
     *               set during world configuration but can be adjusted at any time.
     * 
     * @see getMaxEnergy for retrieving maximum energy
     * @see getEnergyDecayRate for energy decay rate
     */
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    
    /**
     * @brief Get energy decay rate
     * 
     * Returns the rate at which the agent's energy decays over time. Energy decay
     * represents the natural energy loss that occurs even without activity.
     * 
     * @return float Energy decay rate per timestep
     * 
     * @note Energy decay rate affects agent behavior and survival. Higher decay
     *               rates require more frequent resource collection for energy maintenance.
     * 
     * @see setEnergyDecayRate for setting decay rate
     * @see getMaxEnergy for maximum energy
     */
    float getEnergyDecayRate() const { return energyDecayRate_; }
    
    /**
     * @brief Set energy decay rate
     * 
     * Sets the rate at which the agent's energy decays over time. This parameter
     * controls the natural energy loss that occurs even without activity.
     * 
     * @param r New energy decay rate per timestep
     * 
     * @note Energy decay rate affects agent survival and foraging behavior.
     *               It's typically set during world configuration but can be adjusted
     *               to change energy dynamics.
     * 
     * @see getEnergyDecayRate for retrieving decay rate
     * @see setMaxEnergy for setting maximum energy
     */
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    
    // Time
    /**
     * @brief Get simulation time
     * 
     * Returns the current simulation time, which represents the total elapsed
     * time since the start of the simulation. This timestamp is useful for
     * time-sensitive operations and event sequencing.
     * 
     * @return double Current simulation time in seconds
     * 
     * @note Simulation time increases monotonically throughout the simulation
     *       and is reset when reset() is called. It's used for timestamping events
     *       and managing time-dependent processes.
     * 
     * @see update for time step increments
     * @see reset for time reset
     * @see applyMotorCommand for timestamped action execution
     */
    double getSimulationTime() const { return simTime_; }
    
    // Seed for reproducibility
    /**
     * @brief Set random seed for reproducibility
     * 
     * Sets the random seed used by the world simulation for any stochastic processes.
     * This is essential for reproducible simulations and debugging.
     * 
     * @param seed Random seed value
     * 
     * @note Setting the random seed ensures that stochastic processes produce
     *               the same sequence of random values across multiple simulation runs.
     *               This is crucial for debugging and scientific reproducibility.
     * 
     * @see getRandomSeed for retrieving current seed
     * @see configure for initial configuration
     */
    void setRandomSeed(uint64_t seed);
    
    /**
     * @brief Get random seed
     * 
     * Returns the current random seed used by the world simulation.
     * 
     * @return uint64_t Current random seed
     * 
     * @note The random seed affects the reproducibility of stochastic processes
     *       in the world simulation. It's typically set once during world setup
     *       and remains constant throughout the simulation.
     * 
     * @see setRandomSeed for setting random seed
     */
    uint64_t getRandomSeed() const { return rngSeed_; }
    
private:
    // Generate vision from current view
    /**
     * @brief Generate vision from current view
     * 
     * Generates the agent's visual percept based on its current position, orientation,
     * and the world object distribution. This method creates the vision component
     * of the sensory percept that the agent perceives.
     * 
     * @note Vision generation samples world objects within the agent's visual field
     *       and creates a visual representation based on object type, distance, and
     *               properties. The visual field is defined by visionWidth and visionHeight
     *               configuration parameters.
     * 
     * @see getSensoryPercept for accessing visual information
     * @see update for automatic vision generation
     */
    void generateVision();
    
    // Check for collisions
    /**
     * @brief Check for collisions
     * 
     * Checks for collisions between the agent and world objects, including
     * obstacles, hazards, and resources. This method detects when the agent's
     * position overlaps with object positions.
     * 
     * @note Collision detection is essential for preventing agents from
     *               moving into invalid positions or hazardous areas. It also enables
     *               interaction with objects when in proximity.
     * 
     * @see update for automatic collision checking
     * @see isValidPosition for position validity checking
     * @see interact for handling object interactions
     */
    void checkCollisions();
    
    // Interact with nearby objects
    /**
     * @brief Interact with nearby objects
     * 
     * Processes interactions between the agent and nearby world objects. This
     * method handles collection of resources, avoidance of hazards, and other
     * agent-object interactions based on object type and agent capabilities.
     * 
     * @return ActionResult Result of interaction processing, including energy changes
     *         and effect outcomes
     * 
     * @note Object interactions are triggered when the agent is in proximity
     *               to world objects. Different object types (resources, hazards, etc.)
     *               produce different interaction effects.
     * 
     * @see update for automatic interaction processing
     * @see generateVision for visual interaction information
     * @see checkCollisions for collision detection
     */
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
