#pragma once

#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include "PhysicsSystem.hpp"
#include "EnvironmentStateManager.hpp"
#include "WeatherSystem.hpp"
#include "ResourceSystem.hpp"
#include "HazardSystem.hpp"
#include "../../agent/AgentBody.hpp"
#include "../../agent/SensoryPercept.hpp"
#include "../../motor/Action.hpp"
#include "../../sensory/SensoryInput.hpp"

namespace nlm {

// Real environment implementation that integrates all systems
class RealEnvironment {
public:
    RealEnvironment() = default;
    ~RealEnvironment() = default;
    
    // Initialize environment with settings
    void initialize(float width, float height) {
        width_ = width;
        height_ = height;
        
        // Initialize all subsystems
        physicsSystem_.reset(new PhysicsSystem());
        environmentState_.reset(new EnvironmentStateManager());
        weatherSystem_.reset(new WeatherSystem());
        resourceSystem_.reset(new ResourceSystem());
        hazardSystem_.reset(new HazardSystem());
        
        // Setup initial conditions
        EnvironmentalConditions initialConditions;
        initialConditions.temperature = 20.0f;
        initialConditions.humidity = 50.0f;
        initialConditions.pressure = 1013.25f;
        initialConditions.lightLevel = 1.0f;
        initialConditions.windSpeed = 0.0f;
        initialConditions.windDirection = 0.0f;
        initialConditions.timeOfDay = 12.0f;
        initialConditions.dayNightCycle = 1.0f;
        initialConditions.season = 2.0f;
        initialConditions.radiationLevel = 0.0f;
        initialConditions.pollutionLevel = 0.0f;
        initialConditions.visibility = 100.0f;
        
        environmentState_->initialize(initialConditions);
        
        // Initialize other systems
        weatherSystem_->initialize();
        resourceSystem_->initialize();
        hazardSystem_->initialize();
        
        // Generate initial environmental state
        generateEnvironmentalState(initialConditions);
        
        // Generate initial resources
        resourceSystem_->generateResources(initialConditions);
        
        // Generate initial hazards
        hazardSystem_->generateHazards(initialConditions);
        
        // Reset state
        reset();
    }
    
    // Reset environment to initial state
    void reset() {
        simTime_ = 0.0;
        
        // Reset agent
        agent_.reset();
        
        // Clear physics bodies
        physicsSystem_->clear();
        
        // Reset environment state
        EnvironmentalConditions initialConditions;
        initialConditions.temperature = 20.0f;
        initialConditions.humidity = 50.0f;
        initialConditions.pressure = 1013.25f;
        initialConditions.lightLevel = 1.0f;
        initialConditions.windSpeed = 0.0f;
        initialConditions.windDirection = 0.0f;
        initialConditions.timeOfDay = 12.0f;
        initialConditions.dayNightCycle = 1.0f;
        initialConditions.season = 2.0f;
        initialConditions.radiationLevel = 0.0f;
        initialConditions.pollutionLevel = 0.0f;
        initialConditions.visibility = 100.0f;
        
        environmentState_->initialize(initialConditions);
        
        // Reset weather
        weatherSystem_->initialize();
        
        // Reset resources and hazards
        resourceSystem_->initialize();
        hazardSystem_->initialize();
        
        // Generate resources and hazards based on initial conditions
        resourceSystem_->generateResources(initialConditions);
        hazardSystem_->generateHazards(initialConditions);
        
        // Generate environmental state
        generateEnvironmentalState(initialConditions);
        
        // Generate sensory percept
        generateSensoryPercept();
    }
    
    // Update environment state
    void update(double dt) {
        float deltaTime = static_cast<float>(dt);
        
        // Update simulation time
        simTime_ += deltaTime;
        
        // Update environment state
        environmentState_->update(deltaTime, environmentState_->getEvents());
        
        // Update weather system
        weatherSystem_->update(deltaTime, environmentState_->getCurrentConditions());
        
        // Update resource system
        resourceSystem_->update(deltaTime, environmentState_->getCurrentConditions());
        
        // Update hazard system
        hazardSystem_->update(deltaTime, environmentState_->getCurrentConditions());
        
        // Update physics simulation
        physicsSystem_->update(deltaTime);
        
        // Generate sensory percept
        generateSensoryPercept();
    }
    
    // Apply motor command to agent
    ActionResult applyMotorCommand(MotorCommand cmd, double currentTime) {
        // Convert agent to rigid body for physics
        RigidBody agentBody;
        agentBody.x = agent_.x;
        agentBody.y = agent_.y;
        agentBody.orientation = agent_.orientation;
        agentBody.mass = 1.0f; // Agent mass
        agentBody.radius = 0.3f;
        agentBody.linearVelocity = PhysicsVector(agent_.velocityX, agent_.velocityY, agent_.angularVelocity);
        agentBody.angularVelocity = agent_.angularVelocity;
        agentBody.friction = 0.5f;
        agentBody.restitution = 0.3f;
        agentBody.active = true;
        
        // Apply motor command
        ActionResult result;
        switch (cmd) {
            case MotorCommand::MoveForward:
                agentBody.applyForce(std::cos(agentBody.orientation) * 10.0f, 
                                    std::sin(agentBody.orientation) * 10.0f);
                agent_.isMoving = true;
                break;
            case MotorCommand::MoveBackward:
                agentBody.applyForce(-std::cos(agentBody.orientation) * 5.0f, 
                                    -std::sin(agentBody.orientation) * 5.0f);
                agent_.isMoving = true;
                break;
            case MotorCommand::TurnLeft:
                agentBody.torque = -2.0f;
                agent_.isTurning = true;
                break;
            case MotorCommand::TurnRight:
                agentBody.torque = 2.0f;
                agent_.isTurning = true;
                break;
            case MotorCommand::Wait:
                agentBody.linearVelocity.x *= 0.9f;
                agentBody.linearVelocity.y *= 0.9f;
                agentBody.angularVelocity *= 0.9f;
                agent_.isMoving = false;
                agent_.isTurning = false;
                break;
            default:
                break;
        }
        
        // Add agent body to physics system
        physicsSystem_->addRigidBody(std::make_shared<RigidBody>(agentBody));
        
        // Run physics simulation
        physicsSystem_->update(deltaTime);
        
        // Remove agent body from physics system
        std::shared_ptr<RigidBody> agentBodyPtr;
        for (auto& body : physicsSystem_->getBodies()) {
            if (body.get() == &agentBody) {
                agentBodyPtr = body;
                break;
            }
        }
        if (agentBodyPtr) {
            physicsSystem_->removeRigidBody(agentBodyPtr.get());
        }
        
        // Update agent state
        updateAgentFromPhysics(agent_, agentBody);
        
        // Check for interactions
        if (cmd == MotorCommand::Interact) {
            result = interactWithEnvironment();
        } else {
            result = generateActionResult(cmd);
        }
        
        return result;
    }
    
    // Get current sensory percept
    const SensoryPercept& getSensoryPercept() const {
        return sensory_;
    }
    
    // Get agent body state
    const AgentBody& getAgentBody() const {
        return agent_;
    }
    
    // Get environmental conditions
    const EnvironmentalConditions& getEnvironmentalConditions() const {
        return environmentState_->getCurrentConditions();
    }
    
    // Get current weather condition
    WeatherCondition getWeatherCondition() const {
        return weatherSystem_->getCurrentWeather();
    }
    
    // Get agent resources
    const std::vector<ResourceItem>& getAgentResources() const {
        return resourceSystem_->getAgentResources();
    }
    
    // Get available environmental resources at agent position
    std::vector<ResourceNode*> getAvailableResources(float radius) {
        return resourceSystem_->getResourcesAt(agent_.x, agent_.y, radius);
    }
    
    // Check if position is safe (no hazards)
    bool isSafePosition(float x, float y, float radius = 0.3f) const {
        return hazardSystem_->isSafePosition(x, y, radius);
    }
    
    // Get environmental feedback
    const EnvironmentalFeedback& getEnvironmentalFeedback() const {
        return environmentState_->getFeedback();
    }
    
    // Set agent starting position
    void setAgentStart(float x, float y) {
        agent_.x = x;
        agent_.y = y;
    }
    
    // Get simulation time
    double getSimulationTime() const {
        return simTime_;
    }
    
    // Get environment name
    const char* getName() const {
        return "RealEnvironment";
    }
    
    // Generate environmental state from conditions
    void generateEnvironmentalState(const EnvironmentalConditions& conditions) {
        // This could generate environmental state objects based on conditions
        // For now, just store conditions
        currentEnvironmentalConditions_ = conditions;
    }
    
    // Generate sensory percept for agent
    void generateSensoryPercept() {
        const EnvironmentalConditions& conditions = environmentState_->getCurrentConditions();
        
        // Generate vision from agent perspective
        std::vector<float> vision = generateVision(conditions);
        
        // Generate touch sensors
        std::vector<float> touch = generateTouch();
        
        // Generate internal signals
        std::vector<float> internal = generateInternalSignals();
        
        // Generate proprioception
        std::vector<float> proprioception = generateProprioception();
        
        // Set sensory percept
        sensory_.setVision(vision);
        sensory_.setTouch(touch);
        sensory_.setInternal(internal);
        sensory_.setProprioception(proprioception);
        sensory_.setAudio({});
        sensory_.setTimestamp(simTime_);
    }
    
    // Generate vision based on agent perspective
    std::vector<float> generateVision(const EnvironmentalConditions& conditions) {
        std::vector<float> vision(16, 1.0f); // Default: far/empty
        
        // Simple raycasting vision (can be enhanced)
        const int rayCount = 16;
        const float fov = M_PI / 2.0f; // 90 degree FOV
        const float maxRange = 8.0f;
        
        for (int i = 0; i < rayCount; ++i) {
            // Angle for this ray
            float angle = agent_.orientation - fov / 2.0f + (fov * i / (rayCount - 1));
            
            // Cast ray
            float dx = std::cos(angle);
            float dy = std::sin(angle);
            
            float t = 0.0f;
            while (t < maxRange) {
                float px = agent_.x + dx * t;
                float py = agent_.y + dy * t;
                
                // Check boundary
                if (px < 0.0f || px >= width_ || py < 0.0f || py >= height_) {
                    vision[i] = t / maxRange;
                    break;
                }
                
                // Check for hazards
                std::vector<Hazard*> hazards = hazardSystem_->getHazardsAt(px, py, 0.5f);
                if (!hazards.empty()) {
                    float hazardValue = 0.3f; // Dark for hazard
                    vision[i] = hazardValue * (maxRange - t) / maxRange;
                    break;
                }
                
                // Check for resources
                std::vector<ResourceNode*> resources = resourceSystem_->getResourcesAt(px, py, 0.3f);
                if (!resources.empty()) {
                    float resourceValue = 0.8f; // Bright for resource
                    vision[i] = resourceValue * (maxRange - t) / maxRange;
                    break;
                }
                
                t += 0.1f;
            }
        }
        
        return vision;
    }
    
    // Generate touch sensor readings
    std::vector<float> generateTouch() {
        std::vector<float> touch(8, 0.0f);
        
        for (int i = 0; i < 8; ++i) {
            float angle = agent_.orientation + 2.0f * M_PI * i / 8.0f;
            float dx = std::cos(angle);
            float dy = std::sin(angle);
            
            float t = 0.0f;
            while (t < 1.5f) {
                float px = agent_.x + dx * t;
                float py = agent_.y + dy * t;
                
                // Check boundary
                if (px < 0.0f || px >= width_ || py < 0.0f || py >= height_) {
                    touch[i] = 1.0f - t / 1.5f;
                    break;
                }
                
                // Check for obstacles
                if (!hazardSystem_->isSafePosition(px, py, 0.0f)) {
                    touch[i] = 1.0f - t / 1.5f;
                    break;
                }
                
                t += 0.1f;
            }
        }
        
        return touch;
    }
    
    // Generate internal signals
    std::vector<float> generateInternalSignals() {
        std::vector<float> internal(4, 0.0f);
        
        internal[0] = agent_.energy / maxEnergy_;  // Energy level
        internal[1] = agent_.health;               // Health
        internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
        internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
        
        return internal;
    }
    
    // Generate proprioception
    std::vector<float> generateProprioception() {
        std::vector<float> proprioception(6, 0.0f);
        
        proprioception[0] = agent_.x / width_;     // Normalized X
        proprioception[1] = agent_.y / height_;    // Normalized Y
        proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
        proprioception[3] = std::abs(agent_.velocityX) / 5.0f;  // Speed X
        proprioception[4] = std::abs(agent_.velocityY) / 5.0f;  // Speed Y
        proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;  // Turn rate
        
        return proprioception;
    }
    
    // Interact with environment
    ActionResult interactWithEnvironment() {
        ActionResult result;
        result.reward = 0.0f;
        result.success = false;
        
        // Collect resources
        float radius = 1.0f;
        std::vector<ResourceItem> collected = resourceSystem_->collectResources(agent_.x, agent_.y, radius);
        
        if (!collected.empty()) {
            resourceSystem_->addToAgentInventory(collected);
            result.success = true;
            
            // Calculate total reward from collected resources
            for (const auto& resource : collected) {
                result.reward += resource.getUtility() * 0.1f;
            }
            
            result.message = "collected_resources";
        }
        
        // Check for hazards
        std::vector<Hazard*> hazards = hazardSystem_->getHazardsAt(agent_.x, agent_.y, radius);
        if (!hazards.empty()) {
            result.success = true;
            result.reward -= hazards.size() * 5.0f;  // Damage from hazards
            result.message = "encountered_hazards";
        }
        
        // Check for obstacles
        if (!isSafePosition(agent_.x, agent_.y, 0.0f)) {
            result.success = true;
            result.reward -= 1.0f;  // Collision cost
            result.message = "collided_with_obstacle";
        }
        
        return result;
    }
    
    // Generate action result
    ActionResult generateActionResult(MotorCommand cmd) const {
        ActionResult result;
        result.reward = 0.0f;
        result.success = true;
        
        // Small cost for movement
        if (cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward) {
            result.reward = -0.01f;
        } else if (cmd == MotorCommand::TurnLeft || cmd == MotorCommand::TurnRight) {
            result.reward = -0.005f;
        } else {
            result.reward = 0.0f;
        }
        
        return result;
    }
    
    // Update agent from physics body
    void updateAgentFromPhysics(AgentBody& agent, const RigidBody& body) {
        agent.x = body.x;
        agent.y = body.y;
        agent.orientation = body.orientation;
        agent.velocityX = body.linearVelocity.x;
        agent.velocityY = body.linearVelocity.y;
        agent.angularVelocity = body.angularVelocity;
        
        // Apply boundary conditions
        if (agent.x < 0.0f) agent.x = 0.0f;
        if (agent.x >= width_) agent.x = width_ - 0.01f;
        if (agent.y < 0.0f) agent.y = 0.0f;
        if (agent.y >= height_) agent.y = height_ - 0.01f;
        
        // Normalize orientation
        while (agent.orientation > M_PI) agent.orientation -= 2.0f * M_PI;
        while (agent.orientation < -M_PI) agent.orientation += 2.0f * M_PI;
    }
    
    // Get current time of day
    float getTimeOfDay() const {
        return environmentState_->getTimeOfDay();
    }
    
    // Check if it's daytime
    bool isDaytime() const {
        return environmentState_->isDaytime();
    }
    
    // Check if it's night
    bool isNight() const {
        return environmentState_->isNight();
    }
    
    // Get environmental stress on agents
    float getEnvironmentalStress() const {
        return environmentState_->calculateEnvironmentalStress();
    }
    
    // Get weather system
    const WeatherSystem* getWeatherSystem() const {
        return weatherSystem_.get();
    }
    
    // Get resource system
    const ResourceSystem* getResourceSystem() const {
        return resourceSystem_.get();
    }
    
    // Get hazard system
    const HazardSystem* getHazardSystem() const {
        return hazardSystem_.get();
    }
    
private:
    float width_;
    float height_;
    double simTime_;
    float maxEnergy_;
    float energyDecayRate_;
    
    // Agent
    AgentBody agent_;
    
    // Environmental state
    EnvironmentalConditions currentEnvironmentalConditions_;
    
    // Subsystems
    std::unique_ptr<PhysicsSystem> physicsSystem_;
    std::unique_ptr<EnvironmentStateManager> environmentState_;
    std::unique_ptr<WeatherSystem> weatherSystem_;
    std::unique_ptr<ResourceSystem> resourceSystem_;
    std::unique_ptr<HazardSystem> hazardSystem_;
    
    // Sensory state
    SensoryPercept sensory_;
}; 

} // namespace nlm