#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

/**
 * @file SimpleWorld.hpp
 * 
 * Simple world simulation system for NLM
 * 
 * This file provides the simple world simulation system for NLM, implementing:
 * - Simple 2D world simulation
 * - World object management
 * - Agent-environment interaction
 * - Sensory experience generation
 * - Simple physics simulation
 * 
 * The simple world system in NLM:
 * - **WorldObject**: Represents an object in the world
 * - **SimpleWorld**: Provides simple 2D world for NLM development
 * - **Object management**: Manages world objects
 * - **Agent interaction**: Manages agent-environment interaction
 * - **Sensory generation**: Provides sensory experience
 * - **Physics simulation**: Simulates simple physics
 * 
 * Key features:
 * - **Simple world**: Provides simple 2D world simulation
 * - **Object management**: Manages world objects
 * - **Agent interaction**: Manages agent-environment interaction
 * - **Sensory generation**: Provides sensory experience
 * - **Physics simulation**: Simulates simple physics
 * 
 * Simple world characteristics in NLM:
 * - **Simple world**: Provides simple 2D world for NLM development
 * - **Object management**: Manages world objects
 * - **Agent interaction**: Manages agent-environment interaction
 * - **Sensory generation**: Provides sensory experience
 * - **Physics simulation**: Simulates simple physics
 * 
 * @note Critical for agent-environment interaction
 * @note Used extensively in world simulation
 * @note Essential for learning and adaptation
 * @see AgentBrain.hpp - For brain integration
 * @see AgentBody.hpp - For agent state
 * @see SensoryPercept.hpp - For sensory perception
 * @see SimpleWorld.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @struct WorldObject
 * 
 * Neural implementation of world object.
 * 
 * This class implements the world object representation, providing
 * a simple object representation for the world. It is essential for:
 * - **Object representation**: Represents objects in the world
 * - **World management**: Manages world objects
 * - **Object positioning**: Manages object positioning
 * - **Object properties**: Manages object properties
 * 
 * World object characteristics in NLM:
 * - **Object representation**: Represents objects in the world
 * - **World management**: Manages world objects
 * - **Object positioning**: Manages object positioning
 * - **Object properties**: Manages object properties
 * 
 * Key components:
 * - **Position**: x and y coordinates
 * - **Radius**: Object radius
 * - **Type**: World object type
 * - **Value**: Reward value or damage
 * - **Active**: Object active state
 * 
 * @note Position and type only - no semantic labels
 * @see WorldObjectType - For object types
 * @see SimpleWorld - For world management
 * 
 * @author NLM Development Team
 */
struct WorldObject {
    /** Object x-coordinate */
    float x;
    
    /** Object y-coordinate */
    float y;
    
    /** Object radius */
    float radius;
    
    /** Object type */
    WorldObjectType type;
    
    /** Reward value (for resources) or damage (for hazards) */
    float value;
    
    /** Object active state */
    bool active;
    
    /**
     * @brief Default constructor
     * 
     * Creates a default world object with empty values.
     * The world object is initialized with default values for
     * all object fields.
     */
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a world object with specified parameters.
     * The world object is initialized with specified position,
     * type, value, and radius.
     */
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

/**
 * @class SimpleWorld
 * 
 * Neural implementation of simple 2D world for NLM development.
 * 
 * This class implements the simple 2D world for NLM development, providing
 * a simple simulation environment. It is essential for:
 * - **World simulation**: Provides simple 2D world simulation
 * - **Object management**: Manages world objects
 * - **Agent interaction**: Manages agent-environment interaction
 * - **Sensory generation**: Provides sensory experience
 * - **Physics simulation**: Simulates simple physics
 * 
 * Simple world characteristics in NLM:
 * - **Simple world**: Provides simple 2D world for NLM development
 * - **Object management**: Manages world objects
 * - **Agent interaction**: Manages agent-environment interaction
 * - **Sensory generation**: Provides sensory experience
 * - **Physics simulation**: Simulates simple physics
 * 
 * Key components:
 * - **Configuration**: Configures world parameters
 * - **Reset**: Resets world to initial state
 * - **Agent positioning**: Sets agent starting position
 * - **Update**: Updates world state
 * - **Motor command application**: Applies motor commands to agent
 * - **Sensory generation**: Generates sensory percept for agent
 * - **Object management**: Manages world objects
 * - **Collision detection**: Checks for collisions
 * - **Interaction management**: Manages object interactions
 * 
 * @note Provides sensory experience without semantic labels
 * @see WorldObject - For world object representation
 * @see AgentBrain.hpp - For brain integration
 * @see AgentBody.hpp - For agent state
 * @see SensoryPercept.hpp - For sensory perception
 * 
 * @author NLM Development Team
 */
class SimpleWorld {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default simple world instance with default parameters.
     * The simple world is initialized with default values for
     * all world fields.
     */
    SimpleWorld();
    
    /**
     * @brief Destructor
     * 
     * Cleans up simple world resources and performs any necessary
     * cleanup operations. The simple world is left in a valid but
     * non-operational state after destruction.
     */
    ~SimpleWorld();
    
    /**
     * @brief Configure world parameters
     * 
     * Configures the world with specified parameters. This is used for:
     * - World configuration
     * - World setup
     * - World initialization
     * - World parameter setting
     */
    void configure(float width, float height, size_t visionWidth, size_t visionHeight);
    
    /**
     * @brief Reset world to initial state
     * 
     * Resets the world to its initial state. This is used for:
     * - World reset
     * - Episode initialization
     * - State reset
     * - World management
     */
    void reset();
    
    /**
     * @brief Set agent starting position
     * 
     * Sets the agent's starting position. This is used for:
     * - Agent positioning
     * - World setup
     * - Agent initialization
     * - World configuration
     */
    void setAgentStart(float x, float y);
    
    /**
     * @brief Update world state
     * 
     * Updates the world state. This is used for:
     * - World update
     * - State management
     * - World processing
     * - World simulation
     */
    void update(double timestep);
    
    /**
     * @brief Apply motor command to agent
     * 
     * Applies a motor command to the agent. This is used for:
     * - Motor command application
     * - Agent movement
     * - World interaction
     * - Action execution
     */
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
    
    /**
     * @brief Get current sensory percept for the agent
     * 
     * Returns the current sensory percept for the agent. This is used for:
     * - Sensory perception
     * - Sensory access
     * - Sensory processing
     * - Sensory management
     */
    const SensoryPercept& getSensoryPercept() const { return sensory_; }
    
    /**
     * @brief Get agent body state
     * 
     * Returns the agent body state. This is used for:
     * - Agent state access
     * - Agent state management
     * - Agent state processing
     * - Agent state control
     */
    const AgentBody& getAgentBody() const { return agent_; }
    
    /**
     * @brief Add object to world
     * 
     * Adds an object to the world. This is used for:
     * - Object addition
     * - World object management
     * - World setup
     * - World initialization
     */
    void addObject(const WorldObject& obj);
    
    /**
     * @brief Remove object at position
     * 
     * Removes the object at the specified position. This is used for:
     * - Object removal
     * - World object management
     * - World cleanup
     * - World maintenance
     */
    void removeObject(float x, float y);
    
    /**
     * @brief Check if position is valid
     * 
     * Checks if the position is valid (not wall). This is used for:
     * - Position validation
     * - Collision detection
     * - Position checking
     * - Position management
     */
    bool isValidPosition(float x, float y) const;
    
    /**
     * @brief Get object at position
     * 
     * Returns the object at the specified position (if any). This is used for:
     * - Object access
     * - Object retrieval
     * - Object management
     * - Object processing
     */
    WorldObject* getObjectAt(float x, float y);
    
    /**
     * @brief Get world width
     * 
     * Returns the world width. This is used for:
     * - Width access
     * - Width management
     * - Width processing
     * - Width control
     */
    float getWidth() const { return width_; }
    
    /**
     * @brief Get world height
     * 
     * Returns the world height. This is used for:
     * - Height access
     * - Height management
     * - Height processing
     * - Height control
     */
    float getHeight() const { return height_; }
    
    /**
     * @brief Get maximum energy
     * 
     * Returns the maximum energy. This is used for:
     * - Energy access
     * - Energy management
     * - Energy processing
     * - Energy control
     */
    float getMaxEnergy() const { return maxEnergy_; }
    
    /**
     * @brief Set maximum energy
     * 
     * Sets the maximum energy. This is used for:
     * - Energy setting
     * - Energy management
     * - Energy processing
     * - Energy control
     */
    void setMaxEnergy(float e) { maxEnergy_ = e; }
    
    /**
     * @brief Get energy decay rate
     * 
     * Returns the energy decay rate. This is used for:
     * - Decay rate access
     * - Decay rate management
     * - Decay rate processing
     * - Decay rate control
     */
    float getEnergyDecayRate() const { return energyDecayRate_; }
    
    /**
     * @brief Set energy decay rate
     * 
     * Sets the energy decay rate. This is used for:
     * - Decay rate setting
     * - Decay rate management
     * - Decay rate processing
     * - Decay rate control
     */
    void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
    
    /**
     * @brief Get simulation time
     * 
     * Returns the simulation time. This is used for:
     * - Time access
     * - Time management
     * - Time processing
     * - Time control
     */
    double getSimulationTime() const { return simTime_; }
    
    /**
     * @brief Set random seed
     * 
     * Sets the random seed. This is used for:
     * - Seed setting
     * - Random seed management
     * - Random seed processing
     * - Random seed control
     */
    void setRandomSeed(uint64_t seed);
    
    /**
     * @brief Get random seed
     * 
     * Returns the random seed. This is used for:
     * - Seed access
     * - Seed management
     * - Seed processing
     * - Seed control
     */
    uint64_t getRandomSeed() const { return rngSeed_; }
    
private:
    /**
     * @brief Generate vision from current view
     * 
     * Generates vision from the current view. This is used for:
     * - Vision generation
     * - Sensory perception
     * - Sensory processing
     * - Sensory management
     */
    void generateVision();
    
    /**
     * @brief Check for collisions
     * 
     * Checks for collisions. This is used for:
     * - Collision detection
     * - Collision management
     * - Collision processing
     * - Collision control
     */
    void checkCollisions();
    
    /**
     * @brief Interact with nearby objects
     * 
     * Interacts with nearby objects. This is used for:
     * - Interaction management
     * - Object interaction
     * - Interaction processing
     * - Interaction control
     */
    ActionResult interact();
    
    /**
     * @brief Get next random number
     * 
     * Returns the next random number. This is used for:
     * - Random number generation
     * - Random number management
     * - Random number processing
     * - Random number control
     */
    uint64_t nextRandom();
    
    /** World width */
    float width_;
    
    /** World height */
    float height_;
    
    /** Agent */
    AgentBody agent_;
    
    /** Objects in world */
    std::vector<WorldObject> objects_;
    
    /** Sensory state */
    SensoryPercept sensory_;
    
    /** Vision configuration */
    size_t visionWidth_;
    size_t visionHeight_;
    
    /** Energy system */
    float maxEnergy_;
    float energyDecayRate_;
    float resourceEnergyGain_;
    
    /** Simulation time */
    double simTime_;
    
    /** Random seed */
    uint64_t rngSeed_;
    
    /** Simple RNG state */
    uint64_t rngState_;
};

} // namespace nlm
