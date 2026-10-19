#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

// Simple linear congruential generator
static uint64_t lcg(uint64_t& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state;
}

static float urand(uint64_t& state, float min, float max) {
    uint64_t r = lcg(state);
    float normalized = (r & 0xFFFFFFFFFFFFULL) / static_cast<float>(0xFFFFFFFFFFFFULL);
    return min + normalized * (max - min);
}

SimpleWorld::SimpleWorld()
    : width_(20.0f)
    , height_(20.0f)
    , visionWidth_(16)
    , visionHeight_(16)
    , maxEnergy_(100.0f)
    , energyDecayRate_(0.01f)
    , resourceEnergyGain_(30.0f)
    , simTime_(0.0)
    , rngSeed_(42)
    , rngState_(42)
{
    agent_.reset();
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
}

void SimpleWorld::reset() {
    simTime_ = 0.0;
    agent_.reset();
    agent_.x = urand(rngState_, 2.0f, width_ - 2.0f);
    agent_.y = urand(rngState_, 2.0f, height_ - 2.0f);
    agent_.energy = maxEnergy_;
    rngState_ = rngSeed_;  // Reset RNG for reproducible object placement
    
    objects_.clear();
    
    // Place some resource objects (positive reward when touched)
    for (int i = 0; i < 5; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Resource, 10.0f, 0.4f);
    }
    
    // Place some hazard objects (negative reward when touched)
    for (int i = 0; i < 3; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Hazard, -15.0f, 0.3f);
    }
    
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::update(double timestep) {
    simTime_ += timestep;
    
    // Apply velocity to position
    agent_.x += agent_.velocityX * timestep;
    agent_.y += agent_.velocityY * timestep;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * timestep;
    
    // Normalize orientation to [-PI, PI]
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
    // Boundary collision
    if (agent_.x < 0.0f) { agent_.x = 0.0f; agent_.velocityX = 0.0f; }
    if (agent_.x >= width_) { agent_.x = width_ - 0.01f; agent_.velocityX = 0.0f; }
    if (agent_.y < 0.0f) { agent_.y = 0.0f; agent_.velocityY = 0.0f; }
    if (agent_.y >= height_) { agent_.y = height_ - 0.01f; agent_.velocityY = 0.0f; }
    
    // Energy decay
    agent_.energy -= energyDecayRate_;
    if (agent_.energy < 0.0f) agent_.energy = 0.0f;
    
    // Update age
    agent_.age += timestep;
    
    // Generate vision
    generateVision();
    
    // Check collisions
    checkCollisions();
}

uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    ActionResult result;
    const float moveSpeed = 3.0f;
    const float turnSpeed = 2.0f;
    
    // Only allow one action per step
    if (currentTime - agent_.lastActionTime < 0.1) {
        return ActionResult(0.0f, false, "too soon");
    }
    
    agent_.lastActionTime = currentTime;
    
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float dx = std::cos(agent_.orientation) * moveSpeed;
            float dy = std::sin(agent_.orientation) * moveSpeed;
            
            float newX = agent_.x + dx * 0.1f;
            float newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -0.01f;  // Small cost for movement
                result.success = true;
            } else {
                result.reward = -0.05f;  // Cost for hitting wall
                result.success = false;
            }
            break;
        }
        
        case MotorCommand::MoveBackward: {
            float dx = -std::cos(agent_.orientation) * moveSpeed * 0.5f;
            float dy = -std::sin(agent_.orientation) * moveSpeed * 0.5f;
            
            float newX = agent_.x + dx * 0.1f;
            float newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -0.02f;
                result.success = true;
            } else {
                result.reward = -0.05f;
                result.success = false;
            }
            break;
        }
        
        case MotorCommand::TurnLeft:
            agent_.angularVelocity = -turnSpeed;
            agent_.isTurning = true;
            result.reward = -0.005f;
            result.success = true;
            break;
            
        case MotorCommand::TurnRight:
            agent_.angularVelocity = turnSpeed;
            agent_.isTurning = true;
            result.reward = -0.005f;
            result.success = true;
            break;
            
        case MotorCommand::LookLeft:
            // Pan sensor (no movement)
            result.reward = 0.0f;
            result.success = true;
            break;
            
        case MotorCommand::LookRight:
            result.reward = 0.0f;
            result.success = true;
            break;
            
        case MotorCommand::Interact:
            result = interact();
            break;
            
        case MotorCommand::Wait:
        default:
            // Decay velocities
            agent_.velocityX *= 0.9f;
            agent_.velocityY *= 0.9f;
            agent_.angularVelocity *= 0.9f;
            agent_.isMoving = false;
            agent_.isTurning = false;
            result.reward = 0.0f;
            result.success = true;
            break;
    }
    
    return result;
}

ActionResult SimpleWorld::interact() {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    // Check for objects in interaction range
    const float interactRange = 1.0f;
    
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < interactRange + obj.radius) {
            obj.active = false;  // Consume the object
            
            if (obj.type == WorldObjectType::Resource) {
                agent_.energy = std::min(maxEnergy_, agent_.energy + resourceEnergyGain_);
                result.reward = obj.value;  // Positive reward
                result.success = true;
                result.message = "consumed_resource";
            } else if (obj.type == WorldObjectType::Hazard) {
                agent_.health -= 0.2f;
                result.reward = obj.value;  // Negative reward
                result.success = true;
                result.message = "hit_hazard";
            }
            
            // Only interact with one object at a time
            break;
        }
    }
    
    if (!result.success) {
        result.message = "no_object";
    }
    
    return result;
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    // Check world boundaries
    if (x < 0.0f || x >= width_ || y < 0.0f || y >= height_) {
        return false;
    }
    return true;
}

WorldObject* SimpleWorld::getObjectAt(float x, float y) {
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            return &obj;
        }
    }
    return nullptr;
}

void SimpleWorld::addObject(const WorldObject& obj) {
    objects_.push_back(obj);
}

void SimpleWorld::removeObject(float x, float y) {
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius) {
            obj.active = false;
            return;
        }
    }
}

void SimpleWorld::generateVision() {
    // Simple raycasting-based vision
    // Cast rays in a cone in front of the agent
    
    const int rayCount = 16;
    const float fov = M_PI / 2.0f;  // 90 degree FOV
    const float maxRange = 8.0f;
    
    std::vector<float> vision(rayCount, 1.0f);  // Default: far/empty
    
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
            
            // Check wall collision
            if (!isValidPosition(px, py)) {
                vision[i] = t / maxRange;
                break;
            }
            
            // Check object collision
            WorldObject* obj = getObjectAt(px, py);
            if (obj != nullptr) {
                // Encode object type as intensity pattern
                // Resource = bright, Hazard = dark, different distances
                if (obj->type == WorldObjectType::Resource) {
                    vision[i] = (maxRange - t) / maxRange * 0.8f;  // Bright
                } else if (obj->type == WorldObjectType::Hazard) {
                    vision[i] = (maxRange - t) / maxRange * 0.3f;  // Dark
                } else {
                    vision[i] = 0.5f;
                }
                break;
            }
            
            t += 0.1f;
        }
    }
    
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // Generate touch sensors (proximity to obstacles in 8 directions)
    std::vector<float> touch(8, 0.0f);
    for (int i = 0; i < 8; ++i) {
        float angle = agent_.orientation + 2.0f * M_PI * i / 8.0f;
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        float t = 0.0f;
        while (t < 1.5f) {
            float px = agent_.x + dx * t;
            float py = agent_.y + dy * t;
            
            if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
                touch[i] = 1.0f - t / 1.5f;  // Closer = stronger signal
                break;
            }
            t += 0.1f;
        }
    }
    sensory_.setTouch(touch);
    
    // Generate internal signals (energy, health)
    std::vector<float> internal(4, 0.0f);
    internal[0] = agent_.energy / maxEnergy_;  // Energy level
    internal[1] = agent_.health;               // Health
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;  // Movement state
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;  // Turning state
    sensory_.setInternal(internal);
    
    // Generate proprioception (body position/velocity)
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = agent_.x / width_;     // Normalized X
    proprioception[1] = agent_.y / height_;    // Normalized Y
    proprioception[2] = (agent_.orientation + M_PI) / (2.0f * M_PI);  // Normalized orientation
    proprioception[3] = std::abs(agent_.velocityX) / 5.0f;  // Speed X
    proprioception[4] = std::abs(agent_.velocityY) / 5.0f;  // Speed Y
    proprioception[5] = std::abs(agent_.angularVelocity) / 3.0f;  // Turn rate
    sensory_.setProprioception(proprioception);
    
    // No audio for now
    sensory_.setAudio({});
}

void SimpleWorld::checkCollisions() {
    // Passive collision check (for damage when moving into objects)
    WorldObject* obj = getObjectAt(agent_.x, agent_.y);
    if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
        agent_.health -= 0.01f;  // Passive damage
    }
}

// ==============================================================================
// PERFORMANCE AND DEBUGGING TOOLS (NLM Phase 8)
// ==============================================================================

#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

// Simple performance measurement
struct PerformanceTimer {
    std::chrono::high_resolution_clock::time_point start;
    std::string name;
    
    PerformanceTimer(const std::string& n) : name(n) {
        start = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        NLM_LOG_INFO("[PERF] " + name + ": " + std::to_string(duration) + " us");
    }
};

// Configuration parser for procedural content
void parseConfigValue(const std::string& value, float& result) {
    try {
        result = std::stof(value);
    } catch (...) {
        result = 1.0f;
    }
}

void parseConfigValue(const std::string& value, int& result) {
    try {
        result = std::stoi(value);
    } catch (...) {
        result = 1;
    }
}

void parseConfigValue(const std::string& value, std::string& result) {
    result = value;
}

// Validation utilities
bool isValidName(const std::string& name) {
    return !name.empty() && std::all_of(name.begin(), name.end(), 
        [](char c) { return std::isalnum(c) || c == '_'; });
}

// Advanced world operations
void SimpleWorld::setPhysicsParameters(float gravity, float friction, float restitution) {
    NLM_LOG_INFO("Setting physics parameters: gravity=" + std::to_string(gravity) + 
                ", friction=" + std::to_string(friction) + ", restitution=" + 
                std::to_string(restitution));
    
    // Apply physics parameters
    // In a real implementation, this would update physics simulation parameters
    // For now, we just log the configuration
}

void SimpleWorld::addProceduralContent(const std::string& type, float density) {
    NLM_LOG_INFO("Adding procedural content: type=" + type + ", density=" + std::to_string(density));
    
    if (!isValidName(type)) {
        NLM_LOG_ERROR("Invalid procedural content type: " + type);
        return;
    }
    
    // Generate procedural content based on type
    if (type == "resources") {
        int numResources = static_cast<int>(width_ * height_ * density * 0.3f);  // Resources are 30% of density
        for (int i = 0; i < numResources; ++i) {
            float ox = urand(rngState_, 1.0f, width_ - 1.0f);
            float oy = urand(rngState_, 1.0f, height_ - 1.0f);
            objects_.emplace_back(ox, oy, WorldObjectType::Resource, 10.0f, 0.4f);
        }
        NLM_LOG_INFO("  Added " + std::to_string(numResources) + " resources");
    } 
    else if (type == "hazards") {
        int numHazards = static_cast<int>(width_ * height_ * density * 0.2f);  // Hazards are 20% of density
        for (int i = 0; i < numHazards; ++i) {
            float ox = urand(rngState_, 1.0f, width_ - 1.0f);
            float oy = urand(rngState_, 1.0f, height_ - 1.0f);
            objects_.emplace_back(ox, oy, WorldObjectType::Hazard, -15.0f, 0.3f);
        }
        NLM_LOG_INFO("  Added " + std::to_string(numHazards) + " hazards");
    }
    
    else if (type == "markers") {
        int numMarkers = static_cast<int>(width_ * height_ * density * 0.05f);  // Markers are 5% of density
        for (int i = 0; i < numMarkers; ++i) {
            float ox = urand(rngState_, 1.0f, width_ - 1.0f);
            float oy = urand(rngState_, 1.0f, height_ - 1.0f);
            objects_.emplace_back(ox, oy, WorldObjectType::Marker, 5.0f, 0.2f);
        }
        NLM_LOG_INFO("  Added " + std::to_string(numMarkers) + " markers");
    }
    
    else {
        NLM_LOG_WARNING("Unknown procedural content type: " + type);
    }
}

std::map<std::string, float> SimpleWorld::getEnvironmentStats() const {
    NLM_LOG_INFO("Collecting environment statistics");
    
    std::map<std::string, float> stats;
    
    // Basic environment metrics
    stats["width"] = width_;
    stats["height"] = height_;
    stats["simulation_time"] = static_cast<float>(simTime_);
    
    // Object statistics
    size_t activeObjects = 0;
    size_t resourceObjects = 0;
    size_t hazardObjects = 0;
    size_t markerObjects = 0;
    
    for (const auto& obj : objects_) {
        if (obj.active) {
            activeObjects++;
            switch (obj.type) {
                case WorldObjectType::Resource: resourceObjects++; break;
                case WorldObjectType::Hazard: hazardObjects++; break;
                case WorldObjectType::Marker: markerObjects++; break;
                default: break;
            }
        }
    }
    
    stats["total_objects"] = static_cast<float>(objects_.size());
    stats["active_objects"] = static_cast<float>(activeObjects);
    stats["resources"] = static_cast<float>(resourceObjects);
    stats["hazards"] = static_cast<float>(hazardObjects);
    stats["markers"] = static_cast<float>(markerObjects);
    
    // Agent statistics
    stats["agent_energy"] = agent_.energy;
    stats["agent_health"] = agent_.health;
    stats["agent_age"] = static_cast<float>(agent_.age);
    stats["agent_x"] = agent_.x;
    stats["agent_y"] = agent_.y;
    stats["agent_orientation"] = agent_.orientation;
    
    // Density calculations
    float area = width_ * height_;
    stats["resource_density"] = area > 0 ? resourceObjects / area : 0.0f;
    stats["hazard_density"] = area > 0 ? hazardObjects / area : 0.0f;
    
    return stats;
}

std::unique_ptr<SimpleWorld> SimpleWorld::createClone() const {
    NLM_LOG_INFO("Creating world clone");
    
    auto clone = std::make_unique<SimpleWorld>();
    
    // Copy configuration
    clone->configure(width_, height_, visionWidth_, visionHeight_);
    clone->setMaxEnergy(maxEnergy_);
    clone->setEnergyDecayRate(energyDecayRate_);
    clone->setRandomSeed(rngSeed_);
    
    // Copy agent state
    clone->agent_ = agent_;
    
    // Copy objects
    clone->objects_ = objects_;
    
    // Copy vision sensor
    clone->sensory_ = sensory_;
    
    NLM_LOG_INFO("World clone created");
    return clone;
}

// Analysis tools
std::map<std::string, std::vector<float>> SimpleWorld::getResourceDistribution() const {
    NLM_LOG_INFO("Analyzing resource distribution");
    
    std::map<std::string, std::vector<float>> distribution;
    
    // Collect resources by type
    std::vector<float> resourcePositionsX;
    std::vector<float> resourcePositionsY;
    std::vector<float> resourceValues;
    
    for (const auto& obj : objects_) {
        if (obj.active && obj.type == WorldObjectType::Resource) {
            resourcePositionsX.push_back(obj.x);
            resourcePositionsY.push_back(obj.y);
            resourceValues.push_back(obj.value);
        }
    }
    
    distribution["resource_x"] = resourcePositionsX;
    distribution["resource_y"] = resourcePositionsY;
    distribution["resource_values"] = resourceValues;
    
    // Calculate statistics
    if (!resourceValues.empty()) {
        float totalValue = 0.0f;
        float minValue = resourceValues[0];
        float maxValue = resourceValues[0];
        
        for (float val : resourceValues) {
            totalValue += val;
            minValue = std::min(minValue, val);
            maxValue = std::max(maxValue, val);
        }
        
        float avgValue = totalValue / resourceValues.size();
        distribution["resource_total"] = {totalValue};
        distribution["resource_average"] = {avgValue};
        distribution["resource_min"] = {minValue};
        distribution["resource_max"] = {maxValue};
    }
    
    return distribution;
}

// Performance and debugging
bool SimpleWorld::profiler(double& totalTime, double& cpuTime, double& wallTime) const {
    NLM_LOG_INFO("Profiling world simulation");
    
    // In a real implementation, this would capture actual performance metrics
    // For now, we use placeholder values
    
    totalTime = simTime_;
    cpuTime = simTime_ * 0.9;  // 90% CPU time (placeholder)
    wallTime = simTime_ * 1.1;  // 110% wall time (placeholder)
    
    NLM_LOG_INFO("Profile complete: total=" + std::to_string(totalTime) + 
                "s, cpu=" + std::to_string(cpuTime) + "s, wall=" + 
                std::to_string(wallTime) + "s");
    
    return true;
}

// Simulation control
bool SimpleWorld::runEpisode(int steps, bool verbose) {
    NLM_LOG_INFO("Running episode: " + std::to_string(steps) + " steps");
    
    if (steps <= 0) {
        NLM_LOG_ERROR("Episode steps must be positive, got " + std::to_string(steps));
        return false;
    }
    
    double originalTimeScale = 1.0;  // In a real implementation, this would be stored
    
    // Store current state for potential rollback
    AgentBody originalAgent = agent_;
    std::vector<WorldObject> originalObjects = objects_;
    
    // Run episode
    for (int step = 0; step < steps; ++step) {
        if (verbose) {
            NLM_LOG_INFO("  Step " + std::to_string(step + 1) + "/" + std::to_string(steps));
        }
        
        // Check for termination conditions
        if (agent_.energy <= 0.0f || agent_.health <= 0.0f) {
            NLM_LOG_INFO("Episode terminated: agent exhausted");
            break;
        }
        
        // Update world
        update(0.1);  // Fixed timestep for episode
    }
    
    NLM_LOG_INFO("Episode complete");
    return true;
}

void SimpleWorld::setTimeScale(float scale) {
    NLM_LOG_INFO("Setting time scale: " + std::to_string(scale));
    
    if (scale < 0.0f) {
        NLM_LOG_ERROR("Time scale must be non-negative, got " + std::to_string(scale));
        return;
    }
    
    // In a real implementation, this would affect the timestep in the simulation
    // For now, we just log the configuration
}

void SimpleWorld::setExplorationRate(float rate) {
    NLM_LOG_INFO("Setting exploration rate: " + std::to_string(rate));
    
    if (rate < 0.0f || rate > 1.0f) {
        NLM_LOG_ERROR("Exploration rate must be between 0.0 and 1.0, got " + std::to_string(rate));
        return;
    }
    
    // In a real implementation, this would affect agent behavior
    // For now, we just log the configuration
}

void SimpleWorld::executeScenario(const std::string& scenarioName) {
    NLM_LOG_INFO("Executing scenario: " + scenarioName);
    
    if (!isValidName(scenarioName)) {
        NLM_LOG_ERROR("Invalid scenario name: " + scenarioName);
        return;
    }
    
    // Reset world for scenario execution
    reset();
    
    // Configure based on scenario
    if (scenarioName == "tutorial") {
        NLM_LOG_INFO("  Tutorial scenario: basic navigation and resource collection");
        addProceduralContent("resources", 0.1f);
        setAgentStart(1.0f, 1.0f);
    }
    
    else if (scenarioName == "survival") {
        NLM_LOG_INFO("  Survival scenario: high hazard density, low resources");
        addProceduralContent("hazards", 0.3f);
        addProceduralContent("resources", 0.05f);
        setAgentStart(width_ * 0.1f, height_ * 0.1f);
    }
    
    else if (scenarioName == "collection") {
        NLM_LOG_INFO("  Collection scenario: dense resources, no hazards");
        addProceduralContent("resources", 0.4f);
        addProceduralContent("markers", 0.1f);
        setAgentStart(width_ * 0.5f, height_ * 0.5f);
    }
    
    else {
        NLM_LOG_WARNING("Unknown scenario: " + scenarioName);
        // Default scenario
        addProceduralContent("resources", 0.2f);
        addProceduralContent("hazards", 0.1f);
    }
    
    NLM_LOG_INFO("Scenario " + scenarioName + " executed");
}
