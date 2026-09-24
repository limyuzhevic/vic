#pragma once

namespace nlm {

/**
 * AgentBody - Represents the agent's physical body and state in the world
 * 
 * The body contains position, orientation, energy, health, and other physical properties
 * that determine how the agent interacts with the environment.
 */
class AgentBody {
public:
    AgentBody();
    ~AgentBody();
    
    /**
     * Configure body parameters
     * 
     * @param maxEnergy Maximum energy capacity
     * @param visionWidth Vision width in pixels
     * @param visionHeight Vision height in pixels
     */
    void configure(float maxEnergy, size_t visionWidth, size_t visionHeight);
    
    /**
     * Set random seed for reproducible initialization
     * 
     * @param seed Random seed value
     */
    void setRandomSeed(uint64_t seed);
    
    /**
     * Reset body to initial state
     * 
     * Places agent at random valid position with full energy
     */
    void reset();
    
    /**
     * Update body state over time
     * 
     * @param timestep Time elapsed since last update
     */
    void update(double timestep);
    
    /**
     * Apply motor command to body
     * 
     * @param motorCommand Command to execute
     * @param currentTime Current simulation time
     * @return ActionResult describing outcome
     */
    ActionResult applyMotorCommand(MotorCommand motorCommand, double currentTime);
    
    /**
     * Get current X coordinate
     */
    float getX() const { return x_; }
    
    /**
     * Get current Y coordinate
     */
    float getY() const { return y_; }
    
    /**
     * Get current orientation (radians)
     */
    float getOrientation() const { return orientation_; }
    
    /**
     * Get current energy level
     */
    float getEnergy() const { return energy_; }
    
    /**
     * Get current health level
     */
    float getHealth() const { return health_; }
    
    /**
     * Get simulation time
     */
    double getSimulationTime() const { return simTime_; }
    
    /**
     * Get maximum energy capacity
     */
    float getMaxEnergy() const { return maxEnergy_; }
    
    /**
     * Get world width
     */
    float getWidth() const { return width_; }
    
    /**
     * Get world height
     */
    float getHeight() const { return height_; }
    
    /**
     * Get vision width
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * Get vision height
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * Set agent starting position
     */
    void setAgentStart(float x, float y);
    
    /**
     * Get current position
     * 
     * @return X coordinate
     */
    float getX() const { return x_; }
    
    /**
     * Get current Y coordinate
     * 
     * @return Y coordinate
     */
    float getY() const { return y_; }
    
    /**
     * Get current orientation (radians)
     * 
     * @return Orientation in radians [-PI, PI]
     */
    float getOrientation() const { return orientation_; }
    
    /**
     * Get current energy level
     * 
     * @return Energy (0.0 to maxEnergy_)
     */
    float getEnergy() const { return energy_; }
    
    /**
     * Get current health level
     * 
     * @return Health (typically 1.0 = healthy, decreasing with damage)
     */
    float getHealth() const { return health_; }
    
    /**
     * Get simulation time
     * 
     * @return Total time elapsed since reset
     */
    double getSimulationTime() const { return simTime_; }
    
    /**
     * Get maximum energy capacity
     * 
     * @return Maximum energy
     */
    float getMaxEnergy() const { return maxEnergy_; }
    
    /**
     * Get world width
     * 
     * @return World width
     */
    float getWidth() const { return width_; }
    
    /**
     * Get world height
     * 
     * @return World height
     */
    float getHeight() const { return height_; }
    
    /**
     * Get vision width
     * 
     * @return Vision width in pixels
     */
    size_t getVisionWidth() const { return visionWidth_; }
    
    /**
     * Get vision height
     * 
     * @return Vision height in pixels
     */
    size_t getVisionHeight() const { return visionHeight_; }
    
    /**
     * Set agent starting position
     * 
     * @param x X coordinate
     * @param y Y coordinate
     */
    void setAgentStart(float x, float y);
    
    /**
     * Get sensory percept from body's current state
     * 
     * Generates vision, touch, and internal signals based on body state
     * 
     * @return Sensory percept representing current state
     */
    SensoryPercept getSensoryPercept() const;
    
    /**
     * Check if position is valid (within world bounds)
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return True if position is valid
     */
    bool isValidPosition(float x, float y) const;
    
    /**
     * Add object to world
     * 
     * @param obj Object to add
     */
    void addObject(const WorldObject& obj);
    
    /**
     * Remove object from world
     * 
     * @param x X coordinate
     * @param y Y coordinate
     */
    void removeObject(float x, float y);
    
    /**
     * Get object at position
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @return Pointer to object or nullptr if none
     */
    WorldObject* getObjectAt(float x, float y);
    
    /**
     * Check for passive collisions (damage)
     */
    void checkCollisions();
    
    /**
     * Generate vision for agent
     * 
     * Casts rays in front of agent to detect objects and walls
     */
    void generateVision();
    
    /**
     * Get next random number from agent's RNG
     * 
     * @return Random number
     */
    uint64_t nextRandom();
    
private:
    // World dimensions
    float width_;                // World width
    float height_;               // World height
    size_t visionWidth_;         // Vision width in pixels
    size_t visionHeight_;        // Vision height in pixels
    
    // Body properties
    float maxEnergy_;            // Maximum energy capacity
    float energyDecayRate_;      // Energy decay per timestep
    float resourceEnergyGain_;   // Energy gained from resources
    
    // State
    double simTime_;            // Total simulation time
    uint64_t rngSeed_;           // Random seed for reproducible behavior
    uint64_t rngState_;          // Current RNG state
    
    // Agent position and motion
    float x_;                    // X coordinate
    float y_;                    // Y coordinate
    float orientation_;          // Orientation in radians
    float velocityX_;            // X velocity
    float velocityY_;            // Y velocity
    float angularVelocity_;      // Angular velocity
    bool isMoving_;              // Moving flag
    bool isTurning_;             // Turning flag
    
    // Biological state
    float energy_;               // Current energy
    float health_;               // Current health
    double age_;                 // Age in simulation time
    double lastActionTime_;      // Time of last action
    
    // World objects
    std::vector<WorldObject> objects_;  // Objects in world
    
    // Vision system
    std::vector<float> vision_;  // Vision intensities (visionWidth_ * visionHeight_)
};

} // namespace nlm
