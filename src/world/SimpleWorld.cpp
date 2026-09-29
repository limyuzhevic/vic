#include "SimpleWorld.hpp"
#include "VisionSystem.hpp"
#include "PhysicsSystem.hpp"
#include "CollisionSystem.hpp"
#include "Environment.hpp"
#include "WorldConstants.hpp"
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
    : width_(WorldConstants::DEFAULT_WIDTH)
    , height_(WorldConstants::DEFAULT_HEIGHT)
    , visionWidth_(WorldConstants::DEFAULT_VISION_WIDTH)
    , visionHeight_(WorldConstants::DEFAULT_VISION_HEIGHT)
    , maxEnergy_(WorldConstants::DEFAULT_MAX_ENERGY)
    , energyDecayRate_(WorldConstants::DEFAULT_ENERGY_DECAY_RATE)
    , resourceEnergyGain_(WorldConstants::DEFAULT_RESOURCE_ENERGY_GAIN)
    , simTime_(WorldConstants::DEFAULT_SIMULATION_TIME)
    , rngSeed_(WorldConstants::DEFAULT_RANDOM_SEED)
    , rngState_(WorldConstants::DEFAULT_RANDOM_SEED)
{
    agent_.reset();
    initializeSystems();
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::initializeSystems() {
    visionSystem_ = std::make_unique<VisionSystem>();
    physicsSystem_ = std::make_unique<PhysicsSystem>();
    collisionSystem_ = std::make_unique<CollisionSystem>();
    environment_ = std::make_unique<Environment>();
}

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    // Validate configuration parameters
    if (validateConfiguration()) {
        width_ = width;
        height_ = height;
        visionWidth_ = visionWidth;
        visionHeight_ = visionHeight;
        sensory_.resetToDefault();
    }
}

bool SimpleWorld::validateConfiguration() const {
    return width_ > 0 && height_ > 0 &&
           visionWidth_ > 0 && visionHeight_ > 0 &&
           maxEnergy_ > 0 && energyDecayRate_ >= 0;
}

void SimpleWorld::validateBounds(float x, float y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        std::cerr << "Warning: Position out of bounds (" << x << ", " << y << ")" << std::endl;
    }
}

float SimpleWorld::clampToBounds(float x, float y, float value) const {
    if (x < 0) return 0;
    if (x >= width_) return width_ - 1;
    if (y < 0) return 0;
    if (y >= height_) return height_ - 1;
    return value;
}

void SimpleWorld::reset() {
    simTime_ = WorldConstants::DEFAULT_SIMULATION_TIME;
    agent_.reset();
    
    // Set agent to random position within bounds
    agent_.x = urand(rngState_, 2.0f, width_ - 2.0f);
    agent_.y = urand(rngState_, 2.0f, height_ - 2.0f);
    agent_.energy = maxEnergy_;
    
    rngState_ = rngSeed_;  // Reset RNG for reproducible object placement
    
    objects_.clear();
    
    // Use environment to initialize world
    environment_->initializeRandomWorld(agent_, objects_, width_, height_, rngState_);
    
    // Generate initial vision
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    validateBounds(x, y);
    agent_.x = clampToBounds(x, y, x);
    agent_.y = clampToBounds(x, y, y);
}

void SimpleWorld::update(double timestep) {
    // Validate timestep
    if (timestep <= 0) {
        std::cerr << "Warning: Invalid timestep " << timestep << std::endl;
        return;
    }
    
    simTime_ += timestep;
    
    // Update physics
    updatePhysics(timestep);
    
    // Update energy system
    updateEnergy(timestep);
    
    // Update age
    updateAge(timestep);
    
    // Generate vision
    generateVision();
    
    // Check collisions
    updateCollisions();
}

void SimpleWorld::updatePhysics(double timestep) {
    physicsSystem_->updateAgentPhysics(agent_, width_, height_, timestep);
}

void SimpleWorld::updateEnergy(double timestep) {
    agent_.energy -= energyDecayRate_;
    if (agent_.energy < 0.0f) agent_.energy = 0.0f;
}

void SimpleWorld::updateAge(double timestep) {
    agent_.age += timestep;
}

void SimpleWorld::updateCollisions() {
    collisionSystem_->checkAgentObjectCollisions(agent_, objects_);
}

uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    ActionResult result;
    
    // Validate action timing
    if (currentTime - agent_.lastActionTime < WorldConstants::MIN_TIME_BETWEEN_ACTIONS) {
        return ActionResult(0.0f, false, "too soon");
    }
    
    agent_.lastActionTime = currentTime;
    
    // Use physics system to apply motor command
    physicsSystem_->applyMotorCommand(agent_, cmd, 
                                     WorldConstants::MOVE_SPEED, 
                                     WorldConstants::TURN_SPEED);
    
    // Handle special commands
    if (cmd == MotorCommand::LookLeft || cmd == MotorCommand::LookRight) {
        result.reward = 0.0f;
        result.success = true;
    } else if (cmd == MotorCommand::Interact) {
        result = interact();
    } else {
        result.reward = WorldConstants::MOVEMENT_COST;
        result.success = true;
    }
    
    return result;
}

ActionResult SimpleWorld::interact() {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    // Check for objects in interaction range
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < WorldConstants::INTERACT_RANGE + obj.radius) {
            obj.active = false;  // Consume the object
            
            if (obj.type == WorldObjectType::Resource) {
                agent_.energy = std::min(maxEnergy_, agent_.energy + 
                                      WorldConstants::RESOURCE_ENERGY_GAIN);
                result.reward = obj.value;
                result.success = true;
                result.message = "consumed_resource";
            } else if (obj.type == WorldObjectType::Hazard) {
                agent_.health -= 0.2f;
                result.reward = obj.value;
                result.success = true;
                result.message = "hit_hazard";
            }
            
            break;
        }
    }
    
    if (!result.success) {
        result.message = "no_object";
    }
    
    return result;
}

void SimpleWorld::generateVision() {
    // Use vision system to generate all sensory data
    if (visionSystem_) {
        visionSystem_->generateVision(agent_, *this, sensory_, simTime_);
    }
}

void SimpleWorld::checkCollisions() {
    // Use collision system
    collisionSystem_->checkAgentObjectCollisions(agent_, objects_);
}

bool SimpleWorld::isValidPosition(float x, float y) const {
    return physicsSystem_->isValidPosition(x, y, width_, height_);
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
    if (validateConfiguration()) {
        objects_.push_back(obj);
    }
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

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
}

} // namespace nlm