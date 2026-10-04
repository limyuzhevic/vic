#include "AgentBody.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <mutex>

namespace nlm {

AgentBody::AgentBody(const Config& config)
    : config_(config)
    , brain_(nullptr)
    , world_(nullptr)
{
    state_.reset();
    perfMetrics_.reset();
}

AgentBody::~AgentBody() = default;

// Copy constructor for deep copy semantics
AgentBody::AgentBody(const AgentBody& other)
    : config_(other.config_)
    , state_(other.state_)
    , brain_(other.brain_)
    , world_(other.world_)
    , perfMetrics_(other.perfMetrics_)
{
}

// Assignment operator for deep copy semantics
AgentBody& AgentBody::operator=(const AgentBody& other) {
    if (this != &other) {
        config_ = other.config_;
        state_ = other.state_;
        brain_ = other.brain_;
        world_ = other.world_;
        perfMetrics_ = other.perfMetrics_;
    }
    return *this;
}

// Move constructor for efficient transfer
AgentBody::AgentBody(AgentBody&& other) noexcept
    : config_(std::move(other.config_))
    , state_(std::move(other.state_))
    , brain_(std::move(other.brain_))
    , world_(std::move(other.world_))
    , perfMetrics_(std::move(other.perfMetrics_))
{
}

// Move assignment operator for efficient transfer
AgentBody& AgentBody::operator=(AgentBody&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
        state_ = std::move(other.state_);
        brain_ = std::move(other.brain_);
        world_ = std::move(other.world_);
        perfMetrics_ = std::move(other.perfMetrics_);
    }
    return *this;
}

void AgentBody::initialize(float startX, float startY, float startOrientation) {
    std::lock_guard<std::mutex> lock(mutex_); // If we had threading support
    
    setPosition(startX, startY);
    setOrientation(startOrientation);
    setEnergy(config_.defaultEnergy);
    setHealth(config_.defaultHealth);
    setAge(0.0);
    
    perfMetrics_.lastUpdateTime = 0.0;
    logState();
}

void AgentBody::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    state_.reset();
    perfMetrics_.reset();
    
    // Reset brain and world connections
    if (brain_) {
        brain_->reset();
    }
    
    logState();
}

void AgentBody::update(double timestep) {
    if (timestep <= 0.0) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    perfMetrics_.lastUpdateTime = timestep;
    perfMetrics_.computeTime += timestep;
    
    // Store previous state for interpolation
    state_.previousX = state_.x;
    state_.previousY = state_.y;
    state_.previousOrientation = state_.orientation;
    
    // Update energy and health
    updateEnergyHealth(timestep);
    updateAge(timestep);
    
    // Apply movement physics
    applyMovementPhysics(timestep);
    
    // Update movement state
    updateMovementState();
    
    // Check boundaries
    handleBoundaryCollision();
    
    // Update performance metrics
    perfMetrics_.memoryUsage = memoryUsage();
}

void AgentBody::interpolate(const PhysicalState& targetState, float alpha) {
    if (alpha <= 0.0f || alpha >= 1.0f) {
        state_ = targetState;
        return;
    }
    
    state_ = PhysicalState::interpolate(state_, targetState, alpha);
}

void AgentBody::setPosition(float x, float y) {
    if (!validatePosition(x, y)) {
        NLM_LOG_WARNING("Attempted to set invalid position: (" + 
                       std::to_string(x) + ", " + std::to_string(y) + ")");
        return;
    }
    
    state_.x = x;
    state_.y = y;
}

void AgentBody::setOrientation(float orientation) {
    state_.orientation = std::fmod(orientation, 2.0f * M_PI);
    if (state_.orientation < 0.0f) {
        state_.orientation += 2.0f * M_PI;
    }
}

void AgentBody::setVelocity(float vx, float vy, float angularVel) {
    state_.velocityX = vx;
    state_.velocityY = vy;
    state_.angularVelocity = angularVel;
}

void AgentBody::move(MotorCommand command, double timestep) {
    if (timestep <= 0.0) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    float speedFactor = 1.0f;
    
    // Check if we have enough energy
    float energyCost = config_.energyCostPerAction;
    if (state_.energy < energyCost) {
        speedFactor *= state_.energy / energyCost;
    }
    
    executeAction(command, timestep);
    
    // Update last action time
    state_.lastActionTime += timestep;
}

void AgentBody::applyForce(float fx, float fy, float torque) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Convert force to impulse (assuming timestep of 1.0)
    state_.velocityX += fx * config_.acceleration;
    state_.velocityY += fy * config_.acceleration;
    state_.angularVelocity += torque;
}

void AgentBody::applyImpulse(float fx, float fy, float torque) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    state_.velocityX += fx;
    state_.velocityY += fy;
    state_.angularVelocity += torque;
}

PhysicalState AgentBody::getSmoothedState(float alpha) const {
    if (alpha <= 0.0f || alpha >= 1.0f) {
        return state_;
    }
    
    PhysicalState smoothed;
    smoothed.x = state_.x * alpha + state_.previousX * (1.0f - alpha);
    smoothed.y = state_.y * alpha + state_.previousY * (1.0f - alpha);
    smoothed.orientation = state_.orientation * alpha + state_.previousOrientation * (1.0f - alpha);
    smoothed.velocityX = state_.velocityX;
    smoothed.velocityY = state_.velocityY;
    smoothed.angularVelocity = state_.angularVelocity;
    smoothed.energy = state_.energy;
    smoothed.health = state_.health;
    smoothed.age = state_.age;
    smoothed.isMoving = state_.isMoving;
    smoothed.isTurning = state_.isTurning;
    smoothed.lastActionTime = state_.lastActionTime;
    
    return smoothed;
}

void AgentBody::setEnergy(float energy) {
    energy = std::max(0.0f, std::min(energy, 1.0f));
    if (state_.energy != energy) {
        state_.energy = energy;
        NLM_LOG_INFO("Energy changed to " + std::to_string(energy));
    }
}

void AgentBody::setHealth(float health) {
    health = std::max(0.0f, std::min(health, 1.0f));
    if (state_.health != health) {
        state_.health = health;
        NLM_LOG_INFO("Health changed to " + std::to_string(health));
        
        if (health <= 0.0f) {
            NLM_LOG_ERROR("Agent health depleted - agent died!");
        }
    }
}

void AgentBody::setAge(double age) {
    if (age >= 0.0) {
        state_.age = age;
    }
}

void AgentBody::damage(float amount) {
    if (amount > 0.0f) {
        setHealth(state_.health - amount * config_.healthCostPerCollision);
    }
}

void AgentBody::heal(float amount) {
    if (amount > 0.0f) {
        setHealth(state_.health + amount * config_.healthRegenerationRate);
    }
}

void AgentBody::consumeEnergy(float amount) {
    if (amount > 0.0f) {
        setEnergy(state_.energy - amount);
    }
}

void AgentBody::regenerateEnergy(double timestep) {
    if (timestep > 0.0 && state_.energy < 1.0f) {
        float regeneration = static_cast<float>(timestep) * config_.energyRegenerationRate;
        setEnergy(state_.energy + regeneration);
    }
}

void AgentBody::regenerateHealth(double timestep) {
    if (timestep > 0.0 && state_.health < 1.0f) {
        float regeneration = static_cast<float>(timestep) * config_.healthRegenerationRate;
        setHealth(state_.health + regeneration);
    }
}

ActionResult AgentBody::interactWithObject(const WorldObjectInfo& object) {
    ActionResult result;
    
    if (!object.isValid()) {
        result.success = false;
        result.message = "Invalid object for interaction";
        return result;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    result.targetId = object.name;
    result.actionType = "interact";
    result.timestamp = state_.lastActionTime;
    
    switch (object.type) {
        case WorldObjectType::Resource:
            heal(object.value);
            result.reward = object.value * 10.0f;
            result.success = true;
            result.message = "Collected resource: " + object.name;
            break;
            
        case WorldObjectType::Food:
            setEnergy(std::min(1.0f, state_.energy + object.value));
            heal(object.value * 0.5f);
            result.reward = object.value * 15.0f;
            result.success = true;
            result.message = "Consumed food: " + object.name;
            break;
            
        case WorldObjectType::Tool:
            result.reward = object.value * 5.0f;
            result.success = true;
            result.message = "Found tool: " + object.name;
            break;
            
        case WorldObjectType::Hazard:
            damage(object.hazardLevel);
            result.reward = -object.hazardLevel * 10.0f;
            result.success = false;
            result.message = "Encountered hazard: " + object.name;
            break;
            
        case WorldObjectType::Door:
            result.reward = 2.0f;
            result.success = true;
            result.message = "Opened door: " + object.name;
            break;
            
        default:
            result.success = false;
            result.message = "Cannot interact with object type: " + std::to_string(static_cast<int>(object.type));
            break;
    }
    
    // Apply energy cost for interaction
    consumeEnergy(config_.energyCostPerAction);
    
    // Update performance metrics
    perfMetrics_.signalCount++;
    
    return result;
}

void AgentBody::checkBoundaries(const Config& config) {
    if (isOutOfBounds(state_.x, state_.y)) {
        handleBoundaryCollision();
    }
}

void AgentBody::handleBoundaryCollision() {
    float damping = config_.collisionDamping;
    float elasticity = config_.boundaryElasticity;
    
    // X boundaries
    if (state_.x < config_.minPosition) {
        state_.x = config_.minPosition;
        state_.velocityX = -state_.velocityX * damping * elasticity;
    } else if (state_.x > config_.maxPosition) {
        state_.x = config_.maxPosition;
        state_.velocityX = -state_.velocityX * damping * elasticity;
    }
    
    // Y boundaries
    if (state_.y < config_.minPosition) {
        state_.y = config_.minPosition;
        state_.velocityY = -state_.velocityY * damping * elasticity;
    } else if (state_.y > config_.maxPosition) {
        state_.y = config_.maxPosition;
        state_.velocityY = -state_.velocityY * damping * elasticity;
    }
    
    // Apply wrap-around if enabled
    if (config_.wrapAroundWorld) {
        if (state_.x < config_.minPosition) {
            state_.x += (config_.maxPosition - config_.minPosition);
        } else if (state_.x > config_.maxPosition) {
            state_.x -= (config_.maxPosition - config_.minPosition);
        }
        
        if (state_.y < config_.minPosition) {
            state_.y += (config_.maxPosition - config_.minPosition);
        } else if (state_.y > config_.maxPosition) {
            state_.y -= (config_.maxPosition - config_.minPosition);
        }
    }
}

void AgentBody::applyGravity(float strength) {
    if (strength > 0.0f) {
        applyForce(0.0f, -strength, 0.0f);
    }
}

bool AgentBody::isValid() const {
    if (!state_.isValid(config_)) {
        return false;
    }
    
    if (state_.energy < 0.0f || state_.health < 0.0f) {
        return false;
    }
    
    if (std::isnan(state_.x) || std::isnan(state_.y) || 
        std::isnan(state_.orientation) || std::isnan(state_.velocityX) ||
        std::isnan(state_.velocityY) || std::isnan(state_.angularVelocity)) {
        return false;
    }
    
    return true;
}

bool AgentBody::isSafe() const {
    // Agent is safe if it has reasonable energy and health
    return state_.energy > 0.1f && state_.health > 0.1f &&
           state_.age < 10000.0 && // Reasonable age limit
           isValid();
}

std::string AgentBody::validateState() const {
    std::stringstream ss;
    bool valid = true;
    
    if (!state_.isValid(config_)) {
        ss << "Physical state invalid: ";
        ss << "position (" << state_.x << ", " << state_.y << ") out of bounds; ";
        ss << "velocity (" << state_.velocityX << ", " << state_.velocityY << ") too high; ";
        ss << "angular velocity " << state_.angularVelocity << " too high; ";
        valid = false;
    }
    
    if (state_.energy < 0.0f || state_.energy > 1.0f) {
        ss << "Energy " << state_.energy << " out of range [0, 1]; ";
        valid = false;
    }
    
    if (state_.health < 0.0f || state_.health > 1.0f) {
        ss << "Health " << state_.health << " out of range [0, 1]; ";
        valid = false;
    }
    
    if (std::isnan(state_.x) || std::isnan(state_.y)) {
        ss << "Position contains NaN values; ";
        valid = false;
    }
    
    if (!valid) {
        ss << "State is invalid";
    } else {
        ss << "State is valid";
    }
    
    return ss.str();
}

void AgentBody::logState() const {
    std::stringstream ss;
    ss << "=== Agent Body State ===" << std::endl;
    ss << "Position: (" << std::fixed << std::setprecision(2) << state_.x << ", " << state_.y << ")" << std::endl;
    ss << "Orientation: " << std::fixed << std::setprecision(2) << state_.orientation << " rad" << std::endl;
    ss << "Velocity: (" << std::fixed << std::setprecision(2) << state_.velocityX << ", " << state_.velocityY << ")" << std::endl;
    ss << "Angular Velocity: " << std::fixed << std::setprecision(2) << state_.angularVelocity << " rad/s" << std::endl;
    ss << "Speed: " << std::fixed << std::setprecision(2) << getSpeed() << " units/s" << std::endl;
    ss << "Energy: " << std::fixed << std::setprecision(2) << state_.energy << "/1.0" << std::endl;
    ss << "Health: " << std::fixed << std::setprecision(2) << state_.health << "/1.0" << std::endl;
    ss << "Age: " << std::fixed << std::setprecision(2) << state_.age << std::endl;
    ss << "Moving: " << (state_.isMoving ? "Yes" : "No") << std::endl;
    ss << "Turning: " << (state_.isTurning ? "Yes" : "No") << std::endl;
    ss << "Last Action: " << std::fixed << std::setprecision(2) << state_.lastActionTime << std::endl;
    
    NLM_LOG_INFO(ss.str());
}

std::string AgentBody::debugString() const {
    std::stringstream ss;
    ss << "AgentBody Debug Info:" << std::endl;
    ss << "  State Valid: " << (isValid() ? "Yes" : "No") << std::endl;
    ss << "  Safe: " << (isSafe() ? "Yes" : "No") << std::endl;
    ss << "  Memory Usage: " << memoryUsage() << " bytes" << std::endl;
    ss << "  Performance: " << getPerformanceMetrics().getEfficiency() * 100.0f << "% efficient" << std::endl;
    ss << "  Total Time: " << getPerformanceMetrics().getTotalTime() << "s" << std::endl;
    ss << "  Signals Processed: " << getPerformanceMetrics().signalCount << std::endl;
    
    return ss.str();
}

void AgentBody::updateMovementState() {
    state_.isMoving = getSpeed() > config_.moveThreshold;
    state_.isTurning = getAngularSpeed() > config_.turnThreshold;
}

void AgentBody::updateEnergyHealth(double timestep) {
    if (timestep <= 0.0) return;
    
    // Apply natural decay
    float energyLoss = static_cast<float>(timestep) * config_.energyDrainRate;
    consumeEnergy(energyLoss);
    
    float healthLoss = static_cast<float>(timestep) * config_.healthDecayRate;
    damage(healthLoss);
    
    // Natural regeneration
    regenerateEnergy(timestep);
    regenerateHealth(timestep);
    
    // Age increases
    setAge(state_.age + timestep);
}

void AgentBody::updateAge(double timestep) {
    setAge(state_.age + timestep);
}

void AgentBody::handleBoundaryCollision() {
    checkBoundaries(config_);
}

void AgentBody::applyMovementPhysics(double timestep) {
    if (timestep <= 0.0) return;
    
    // Apply velocity changes
    state_.x += state_.velocityX * static_cast<float>(timestep);
    state_.y += state_.velocityY * static_cast<float>(timestep);
    
    // Apply rotation
    state_.orientation += state_.angularVelocity * static_cast<float>(timestep);
    
    // Normalize orientation
    state_.orientation = std::fmod(state_.orientation, 2.0f * M_PI);
    if (state_.orientation < 0.0f) {
        state_.orientation += 2.0f * M_PI;
    }
    
    // Apply friction/deceleration
    state_.velocityX *= (1.0f - config_.deceleration * static_cast<float>(timestep));
    state_.velocityY *= (1.0f - config_.deceleration * static_cast<float>(timestep));
    state_.angularVelocity *= (1.0f - config_.deceleration * static_cast<float>(timestep));
    
    // Clamp velocities
    float maxVel = config_.maxSpeed;
    float maxAngVel = config_.maxAngularSpeed;
    
    float vel = std::sqrt(state_.velocityX * state_.velocityX + state_.velocityY * state_.velocityY);
    if (vel > maxVel) {
        float scale = maxVel / vel;
        state_.velocityX *= scale;
        state_.velocityY *= scale;
    }
    
    if (std::abs(state_.angularVelocity) > maxAngVel) {
        state_.angularVelocity = std::copysign(maxAngVel, state_.angularVelocity);
    }
}

void AgentBody::moveForward(double timestep, float speedFactor) {
    float speed = config_.maxSpeed * speedFactor;
    float rad = state_.orientation;
    state_.velocityX += std::cos(rad) * speed * static_cast<float>(timestep);
    state_.velocityY += std::sin(rad) * speed * static_cast<float>(timestep);
}

void AgentBody::moveBackward(double timestep, float speedFactor) {
    float speed = config_.maxSpeed * speedFactor;
    float rad = state_.orientation + M_PI;
    state_.velocityX += std::cos(rad) * speed * static_cast<float>(timestep);
    state_.velocityY += std::sin(rad) * speed * static_cast<float>(timestep);
}

void AgentBody::turnLeft(double timestep, float speedFactor) {
    float angularSpeed = config_.maxAngularSpeed * speedFactor;
    state_.angularVelocity += angularSpeed * static_cast<float>(timestep);
}

void AgentBody::turnRight(double timestep, float speedFactor) {
    float angularSpeed = -config_.maxAngularSpeed * speedFactor;
    state_.angularVelocity += angularSpeed * static_cast<float>(timestep);
}

void AgentBody::lookLeft(double timestep, float speedFactor) {
    // For now, look left is same as turn left
    turnLeft(timestep, speedFactor);
}

void AgentBody::lookRight(double timestep, float speedFactor) {
    // For now, look right is same as turn right
    turnRight(timestep, speedFactor);
}

void AgentBody::lookUp(double timestep, float speedFactor) {
    // For now, look up adds to orientation (tilt)
    state_.angularVelocity += config_.maxAngularSpeed * speedFactor * static_cast<float>(timestep);
}

void AgentBody::lookDown(double timestep, float speedFactor) {
    // For now, look down subtracts from orientation (tilt)
    state_.angularVelocity -= config_.maxAngularSpeed * speedFactor * static_cast<float>(timestep);
}

void AgentBody::interact(double timestep, float speedFactor) {
    // For interaction, apply a small force in the direction of orientation
    float rad = state_.orientation;
    float force = config_.maxSpeed * speedFactor * 2.0f;
    state_.velocityX += std::cos(rad) * force * static_cast<float>(timestep);
    state_.velocityY += std::sin(rad) * force * static_cast<float>(timestep);
}

void AgentBody::executeAction(MotorCommand command, double timestep) {
    switch (command) {
        case MotorCommand::MoveForward:
            moveForward(static_cast<float>(timestep));
            break;
        case MotorCommand::MoveBackward:
            moveBackward(static_cast<float>(timestep));
            break;
        case MotorCommand::TurnLeft:
            turnLeft(static_cast<float>(timestep));
            break;
        case MotorCommand::TurnRight:
            turnRight(static_cast<float>(timestep));
            break;
        case MotorCommand::LookLeft:
            lookLeft(static_cast<float>(timestep));
            break;
        case MotorCommand::LookRight:
            lookRight(static_cast<float>(timestep));
            break;
        case MotorCommand::LookUp:
            lookUp(static_cast<float>(timestep));
            break;
        case MotorCommand::LookDown:
            lookDown(static_cast<float>(timestep));
            break;
        case MotorCommand::Interact:
            interact(static_cast<float>(timestep));
            break;
        case MotorCommand::PickUp:
        case MotorCommand::Drop:
        case MotorCommand::Use:
            // These would require more complex implementation
            break;
        case MotorCommand::Wait:
            // No movement
            break;
        case MotorCommand::Stop:
            // Emergency stop
            state_.velocityX = 0.0f;
            state_.velocityY = 0.0f;
            state_.angularVelocity = 0.0f;
            break;
        case MotorCommand::Hover:
            // Maintain current position
            break;
        default:
            // Unknown command, ignore
            break;
    }
}

void AgentBody::executeMovementAction(MotorCommand command, double timestep) {
    executeAction(command, timestep);
}

void AgentBody::executeSensorAction(MotorCommand command, double timestep) {
    executeAction(command, timestep);
}

void AgentBody::executeInteractionAction(MotorCommand command, double timestep) {
    executeAction(command, timestep);
}

void AgentBody::executeMetaAction(MotorCommand command, double timestep) {
    executeAction(command, timestep);
}

bool AgentBody::validatePosition(float x, float y) const {
    return x >= config_.minPosition && x <= config_.maxPosition &&
           y >= config_.minPosition && y <= config_.maxPosition;
}

bool AgentBody::validateEnergy(float energy) const {
    return energy >= 0.0f && energy <= 1.0f;
}

bool AgentBody::validateHealth(float health) const {
    return health >= 0.0f && health <= 1.0f;
}

bool AgentBody::validateOrientation(float orientation) const {
    return !std::isnan(orientation) && std::abs(orientation) < 2.0f * M_PI;
}

std::string AgentBody::serializeState() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6);
    ss << state_.x << " " << state_.y << " " << state_.orientation << " "
       << state_.velocityX << " " << state_.velocityY << " " << state_.angularVelocity << " "
       << state_.energy << " " << state_.health << " " << state_.age << " "
       << state_.isMoving << " " << state_.isTurning << " " << state_.lastActionTime;
    return ss.str();
}

PhysicalState AgentBody::deserializeState(const std::string& data) const {
    PhysicalState state;
    std::istringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() >= 13) {
        state.x = std::stof(tokens[0]);
        state.y = std::stof(tokens[1]);
        state.orientation = std::stof(tokens[2]);
        state.velocityX = std::stof(tokens[3]);
        state.velocityY = std::stof(tokens[4]);
        state.angularVelocity = std::stof(tokens[5]);
        state.energy = std::stof(tokens[6]);
        state.health = std::stof(tokens[7]);
        state.age = std::stod(tokens[8]);
        state.isMoving = tokens[9] == "1";
        state.isTurning = tokens[10] == "1";
        state.lastActionTime = std::stod(tokens[11]);
    }
    
    return state;
}

std::string AgentBody::serializeConfig() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6);
    ss << config_.defaultPositionX << " " << config_.defaultPositionY << " " << config_.defaultOrientation << " "
       << config_.maxPosition << " " << config_.minPosition << " "
       << config_.maxSpeed << " " << config_.maxAngularSpeed << " " << config_.acceleration << " " << config_.deceleration << " "
       << config_.defaultEnergy << " " << config_.defaultHealth << " " << config_.energyDrainRate << " " << config_.healthDecayRate << " "
       << config_.moveThreshold << " " << config_.turnThreshold << " " << config_.actionTimeThreshold << " "
       << config_.energyRegenerationRate << " " << config_.healthRegenerationRate << " " << config_.energyCostPerAction << " " << config_.healthCostPerCollision << " "
       << config_.boundaryElasticity << " " << config_.collisionDamping << " " << static_cast<int>(config_.wrapAroundWorld);
    return ss.str();
}

AgentBody::Config AgentBody::deserializeConfig(const std::string& data) const {
    Config config;
    std::istringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() >= 23) {
        config.defaultPositionX = std::stof(tokens[0]);
        config.defaultPositionY = std::stof(tokens[1]);
        config.defaultOrientation = std::stof(tokens[2]);
        config.maxPosition = std::stof(tokens[3]);
        config.minPosition = std::stof(tokens[4]);
        config.maxSpeed = std::stof(tokens[5]);
        config.maxAngularSpeed = std::stof(tokens[6]);
        config.acceleration = std::stof(tokens[7]);
        config.deceleration = std::stof(tokens[8]);
        config.defaultEnergy = std::stof(tokens[9]);
        config.defaultHealth = std::stof(tokens[10]);
        config.energyDrainRate = std::stof(tokens[11]);
        config.healthDecayRate = std::stof(tokens[12]);
        config.moveThreshold = std::stof(tokens[13]);
        config.turnThreshold = std::stof(tokens[14]);
        config.actionTimeThreshold = std::stof(tokens[15]);
        config.energyRegenerationRate = std::stof(tokens[16]);
        config.healthRegenerationRate = std::stof(tokens[17]);
        config.energyCostPerAction = std::stof(tokens[18]);
        config.healthCostPerCollision = std::stof(tokens[19]);
        config.boundaryElasticity = std::stof(tokens[20]);
        config.collisionDamping = std::stof(tokens[21]);
        config.wrapAroundWorld = (tokens[22] == "1");
    }
    
    return config;
}

std::string AgentBody::serialize() const {
    std::stringstream ss;
    ss << "AGENTBODY_V1\n";
    ss << "STATE\n" << serializeState() << "\n";
    ss << "CONFIG\n" << serializeConfig() << "\n";
    return ss.str();
}

AgentBody AgentBody::deserialize(const std::string& data) {
    AgentBody agent;
    
    std::istringstream ss(data);
    std::string line;
    
    if (!std::getline(ss, line)) {
        throw std::runtime_error("Invalid serialization data: missing header");
    }
    
    if (line != "AGENTBODY_V1") {
        throw std::runtime_error("Unsupported serialization version: " + line);
    }
    
    std::string section;
    while (std::getline(ss, line)) {
        if (line == "STATE") {
            agent.state_ = agent.deserializeState(line);
        } else if (line == "CONFIG") {
            // Read config (simplified - would need to handle properly in real implementation)
            std::string configData;
            if (std::getline(ss, configData)) {
                agent.config_ = agent.deserializeConfig(configData);
            }
        }
    }
    
    return agent;
}

void AgentBody::compactState() {
    // Remove any redundant historical data
    state_.previousX = state_.x;
    state_.previousY = state_.y;
    state_.previousOrientation = state_.orientation;
}

void AgentBody::clearHistory() {
    // Clear any stored history
    // In a full implementation, this would clear more extensive history
}

void AgentBody::optimizeMemoryUsage() {
    // Optimize memory usage
    compactState();
    clearHistory();
    
    // In a full implementation, this could also:
    // - Release unused memory
    // - Optimize data structures
    // - Clean up cached data
}

void AgentBody::logMovement() const {
    std::stringstream ss;
    ss << "Movement: (" << std::fixed << std::setprecision(2) << state_.x << ", " << state_.y << ") "
       << "Vel: (" << state_.velocityX << ", " << state_.velocityY << ") "
       << "AngVel: " << state_.angularVelocity << " "
       << "Speed: " << getSpeed();
    NLM_LOG_INFO(ss.str());
}

void AgentBody::logEnergyHealth() const {
    std::stringstream ss;
    ss << "Energy: " << std::fixed << std::setprecision(2) << state_.energy << 
       " Health: " << state_.health << 
       " Age: " << state_.age;
    NLM_LOG_INFO(ss.str());
}

void AgentBody::logPerformance() const {
    std::stringstream ss;
    ss << "Performance: " << std::fixed << std::setprecision(2) << 
       perfMetrics_.getTotalTime() << "s total, " <<
       perfMetrics_.getEfficiency() * 100.0f << "% efficiency, "
       << perfMetrics_.signalCount << " signals processed, "
       << perfMetrics_.memoryUsage << " bytes memory";
    NLM_LOG_INFO(ss.str());
}

size_t AgentBody::memoryUsage() const {
    size_t total = 0;
    
    // Estimate memory usage of current state
    total += sizeof(PhysicalState);
    total += sizeof(Config);
    total += sizeof(std::shared_ptr<AgentBrain>);
    total += sizeof(std::shared_ptr<World>);
    total += sizeof(PerformanceMetrics);
    
    // Estimate memory usage of objects pointed to
    if (brain_) {
        // Rough estimate of brain memory usage
        total += sizeof(std::shared_ptr<Brain>) * 100; // Approximate
    }
    
    if (world_) {
        total += sizeof(std::shared_ptr<SimpleWorld>) * 50; // Approximate
    }
    
    return total;
}

} // namespace nlm