#include "AgentBody.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

// AgentBody methods are defined directly in the header, but we can add implementation
// for missing functionality or provide additional helper methods.

// Additional AgentBody methods that can be implemented in .cpp
void AgentBody::update(double timestep) {
    // Update physics
    x += velocityX * timestep;
    y += velocityY * timestep;
    orientation += angularVelocity * timestep;
    
    // Normalize orientation
    while (orientation > 2.0 * M_PI) orientation -= 2.0 * M_PI;
    while (orientation < 0.0) orientation += 2.0 * M_PI;
    
    // Update age
    age += timestep;
    
    // Energy decay based on movement
    if (isMoving || isTurning) {
        energy *= (1.0f - 0.1f * timestep);  // 10% energy loss per second when moving
    }
    
    // Clamp energy to valid range
    energy = std::clamp(energy, 0.0f, 1.0f);
    
    // Health degradation over time
    health = std::max(0.0f, health - 0.0001f * timestep);
    
    // Set moving flags based on velocity
    isMoving = (std::abs(velocityX) > 0.01f || std::abs(velocityY) > 0.01f);
    isTurning = (std::abs(angularVelocity) > 0.01f);
}

void AgentBody::applyAction(const MotorCommand& command, double timestep, float force, float torque) {
    switch (command) {
        case MotorCommand::MoveForward:
            velocityX += force * cos(orientation);
            velocityY += force * sin(orientation);
            break;
            
        case MotorCommand::MoveBackward:
            velocityX -= force * cos(orientation);
            velocityY -= force * sin(orientation);
            break;
            
        case MotorCommand::TurnLeft:
            angularVelocity += torque;
            break;
            
        case MotorCommand::TurnRight:
            angularVelocity -= torque;
            break;
            
        case MotorCommand::LookLeft:
            // For simple model, just apply a small turn
            angularVelocity -= torque * 0.5f;
            break;
            
        case MotorCommand::LookRight:
            angularVelocity += torque * 0.5f;
            break;
            
        case MotorCommand::Interact:
            // Interaction effects - could trigger events in environment
            break;
            
        case MotorCommand::Wait:
            // No change
            break;
    }
    
    // Apply velocity damping/friction
    velocityX *= (1.0f - 0.05f * timestep);
    velocityY *= (1.0f - 0.05f * timestep);
    angularVelocity *= (1.0f - 0.05f * timestep);
    
    // Clamp velocities
    float maxVel = 10.0f;
    float vel = sqrt(velocityX * velocityX + velocityY * velocityY);
    if (vel > maxVel) {
        float ratio = maxVel / vel;
        velocityX *= ratio;
        velocityY *= ratio;
    }
    
    float maxAngVel = 2.0f * M_PI;
    angularVelocity = std::clamp(angularVelocity, -maxAngVel, maxAngVel);
    
    // Update action time
    lastActionTime += timestep;
}

void AgentBody::collideWithObject(float x, float y, float radius) {
    // Simple circle-circle collision
    float dx = this->x - x;
    float dy = this->y - y;
    float distance = sqrt(dx * dx + dy * dy);
    float minDistance = radius + 0.5f; // Agent radius ~0.5
    
    if (distance < minDistance && distance > 0.0f) {
        float overlap = minDistance - distance;
        float nx = dx / distance;
        float ny = dy / distance;
        
        // Resolve collision
        this->x += nx * overlap * 0.5f;
        this->y += ny * overlap * 0.5f;
        
        // Reflect velocity
        float dotProduct = velocityX * nx + velocityY * ny;
        velocityX -= 2.0f * dotProduct * nx;
        velocityY -= 2.0f * dotProduct * ny;
        
        // Reduce speed slightly
        float speed = sqrt(velocityX * velocityX + velocityY * velocityY);
        if (speed > 0.0f) {
            velocityX *= 0.8f;
            velocityY *= 0.8f;
        }
        
        // Set health penalty
        health -= 0.1f;
    }
}

ActionResult AgentBody::getActionResult(float reward, bool success, const std::string& message) {
    return ActionResult(reward, success, message);
}

bool AgentBody::isNear(float x, float y, float distance) const {
    float dx = this->x - x;
    float dy = this->y - y;
    float distSq = dx * dx + dy * dy;
    return distSq < distance * distance;
}

float AgentBody::getDistanceTo(float x, float y) const {
    float dx = this->x - x;
    float dy = this->y - y;
    return sqrt(dx * dx + dy * dy);
}

float AgentBody::getAngleTo(float x, float y) const {
    float dx = x - this->x;
    float dy = y - this->y;
    float angle = atan2(dy, dx);
    while (angle < 0.0f) angle += 2.0f * M_PI;
    while (angle >= 2.0f * M_PI) angle -= 2.0f * M_PI;
    return angle;
}

void AgentBody::teleport(float newX, float newY) {
    x = newX;
    y = newY;
}

void AgentBody::teleport(float newX, float newY, float newOrientation) {
    x = newX;
    y = newY;
    orientation = newOrientation;
    // Normalize orientation
    while (orientation > 2.0 * M_PI) orientation -= 2.0 * M_PI;
    while (orientation < 0.0) orientation += 2.0 * M_PI;
}

std::string AgentBody::getStateString() const {
    std::ostringstream oss;
    oss << "Position: (" << x << ", " << y << ") "
        << "Orientation: " << orientation << " rad "
        << "Velocity: (" << velocityX << ", " << velocityY << ", " << angularVelocity << ") "
        << "Energy: " << energy << " "
        << "Health: " << health << " "
        << "Age: " << age;
    return oss.str();
}

// MotorSystem implementation (missing file)
MotorSystem::MotorSystem() {
    // Initialize motor parameters
    maxForce = 10.0f;
    maxTorque = 5.0f;
    forceDecay = 0.95f;
    torqueDecay = 0.95f;
}

MotorSystem::~MotorSystem() = default;

void MotorSystem::update(double timestep) {
    // Decay accumulated forces/torques
    currentForceX *= forceDecay;
    currentForceY *= forceDecay;
    currentTorque *= torqueDecay;
    
    // Apply decay to all motor neurons if they exist
    for (auto& motor : motorNeurons) {
        motor->decayActivity(timestep);
    }
}

Action MotorSystem::getAction() {
    // This is a placeholder - the actual action should come from motor neurons
    // through the AgentBrain decodeMotorCommand() method
    return Action(ActionType::Wait);
}

bool MotorSystem::addMotorNeuron(Neuron* neuron) {
    if (!neuron) return false;
    
    // Check if neuron is motor type
    // Note: This requires access to neuron type checking
    motorNeurons.push_back(neuron);
    return true;
}

bool MotorSystem::removeMotorNeuron(Neuron* neuron) {
    auto it = std::find(motorNeurons.begin(), motorNeurons.end(), neuron);
    if (it != motorNeurons.end()) {
        motorNeurons.erase(it);
        return true;
    }
    return false;
}

size_t MotorSystem::getMotorNeuronCount() const {
    return motorNeurons.size();
}

void MotorSystem::clear() {
    motorNeurons.clear();
    currentForceX = 0.0f;
    currentForceY = 0.0f;
    currentTorque = 0.0f;
}

// Action implementation
Action::Action() : type(ActionType::Wait), parameters() {
}

Action::Action(ActionType type) : type(type), parameters() {
}

Action::Action(ActionType type, const std::vector<float>& parameters) 
    : type(type), parameters(parameters) {
}

Action::~Action() = default;

ActionType Action::getType() const {
    return type;
}

void Action::setType(ActionType newType) {
    type = newType;
}

const std::vector<float>& Action::getParameters() const {
    return parameters;
}

void Action::setParameters(const std::vector<float>& params) {
    parameters = params;
}

std::string Action::getName() const {
    static const std::string typeNames[] = {
        "MoveForward", "MoveBackward", "MoveLeft", "MoveRight", 
        "TurnLeft", "TurnRight", "Look", "LookUp", "LookDown",
        "Interact", "Eat", "Drink", "Rest", "Wait", "Custom"
    };
    
    if (static_cast<size_t>(type) < sizeof(typeNames) / sizeof(typeNames[0])) {
        return typeNames[static_cast<size_t>(type)];
    }
    return "Unknown";
}

std::unique_ptr<Action> Action::clone() const {
    return std::make_unique<Action>(type, parameters);
}

} // namespace nlm
