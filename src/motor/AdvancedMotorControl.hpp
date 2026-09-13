// Motor planning: Combines primitives into action sequences
// Plans smooth, goal-directed movements based on sensory feedback
class MotorPlanning;

// Advanced motor control: Smooth, goal-directed movements
// Integrates motor primitives, planning, and feedback control
class AdvancedMotorControl {
public:
    AdvancedMotorControl();
    ~AdvancedMotorControl();
    
    // Initialize motor control system
    void initialize(class Brain* brain);
    
    // Execute smooth movement
    void executeMovement(const std::vector<float>& target, 
                        const std::vector<float>& currentState,
                        TimestepDuration dt);
    
    // Get smooth motor command
    std::vector<float> getMotorCommand() const;
    
    // Update motor control
    void update(TimestepDuration dt);
    
    // Get motor command with velocity and acceleration limits
    std::vector<float> getConstrainedMotorCommand(const std::vector<float>& currentVelocities,
                                                 const std::vector<float>& targetVelocities,
                                                 TimestepDuration dt) const;
    
    // Get trajectory from start to goal using cubic interpolation
    std::vector<float> interpolateTrajectory(const std::vector<float>& start,
                                            const std::vector<float>& goal,
                                            float timeProgress) const;
    
    // Check if reached target
    bool isAtTarget(const std::vector<float>& current, const std::vector<float>& target,
                   float tolerance = 0.01f) const;
    
    // Get current movement state
    const std::vector<float>& getCurrentTarget() const { return pImpl->currentTarget_; }
    const std::vector<float>& getCurrentPosition() const { return pImpl->currentPosition_; }
    std::vector<float> getCurrentVelocity() const { return pImpl->currentVelocity_; }
    
    // Set movement parameters
    void setMovementSpeed(float speed) { pImpl->movementSpeed_ = speed; }
    void setAccelerationLimit(float limit) { pImpl->accelerationLimit_ = limit; }
    void setDecelerationDistance(float distance) { pImpl->decelerationDistance_ = distance; }
    
    // Reset motor control
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Motor planning subsystem
    MotorPlanning motorPlanner_;
};
