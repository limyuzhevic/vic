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

// World Setup and Configuration

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
    initializeAgent();
    resetRandomNumberGenerator();
    clearAndSetupObjects();
    generateVision();
}

void SimpleWorld::setAgentStart(float x, float y) {
    agent_.x = x;
    agent_.y = y;
}

void SimpleWorld::initializeAgent() {
    agent_.reset();
    agent_.x = urand(rngState_, 2.0f, width_ - 2.0f);
    agent_.y = urand(rngState_, 2.0f, height_ - 2.0f);
    agent_.energy = maxEnergy_;
}

void SimpleWorld::resetRandomNumberGenerator() {
    rngState_ = rngSeed_;
}

void SimpleWorld::clearAndSetupObjects() {
    objects_.clear();
    placeResourceObjects();
    placeHazardObjects();
}

void SimpleWorld::placeResourceObjects() {
    const int resourceCount = 5;
    for (int i = 0; i < resourceCount; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Resource, 10.0f, 0.4f);
    }
}

void SimpleWorld::placeHazardObjects() {
    const int hazardCount = 3;
    for (int i = 0; i < hazardCount; ++i) {
        float ox = urand(rngState_, 1.0f, width_ - 1.0f);
        float oy = urand(rngState_, 1.0f, height_ - 1.0f);
        objects_.emplace_back(ox, oy, WorldObjectType::Hazard, -15.0f, 0.3f);
    }
}

// Agent Movement and Physics

void SimpleWorld::update(double timestep) {
    simTime_ += timestep;
    updateAgentPosition(timestep);
    handleBoundaryCollision();
    updateEnergyAndAge(timestep);
    generateVision();
    checkCollisions();
}

void SimpleWorld::updateAgentPosition(double timestep) {
    agent_.x += agent_.velocityX * timestep;
    agent_.y += agent_.velocityY * timestep;
    agent_.orientation += agent_.angularVelocity * timestep;
    normalizeOrientation();
}

void SimpleWorld::normalizeOrientation() {
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
}

void SimpleWorld::handleBoundaryCollision() {
    if (agent_.x < 0.0f) {
        agent_.x = 0.0f;
        agent_.velocityX = 0.0f;
    }
    if (agent_.x >= width_) {
        agent_.x = width_ - 0.01f;
        agent_.velocityX = 0.0f;
    }
    if (agent_.y < 0.0f) {
        agent_.y = 0.0f;
        agent_.velocityY = 0.0f;
    }
    if (agent_.y >= height_) {
        agent_.y = height_ - 0.01f;
        agent_.velocityY = 0.0f;
    }
}

void SimpleWorld::updateEnergyAndAge(double timestep) {
    agent_.energy -= energyDecayRate_;
    if (agent_.energy < 0.0f) agent_.energy = 0.0f;
    agent_.age += timestep;
}

// Object Management

void SimpleWorld::interact() {
    ActionResult result;
    result.reward = 0.0f;
    result.success = false;
    
    const float interactRange = 1.0f;
    
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < interactRange + obj.radius) {
            consumeObject(obj, result);
            break;
        }
    }
    
    if (!result.success) {
        result.message = "no_object";
    }
    
    return result;
}

void SimpleWorld::consumeObject(WorldObject& obj, ActionResult& result) {
    obj.active = false;
    
    if (obj.type == WorldObjectType::Resource) {
        agent_.energy = std::min(maxEnergy_, agent_.energy + resourceEnergyGain_);
        result.reward = obj.value;
        result.success = true;
        result.message = "consumed_resource";
    } else if (obj.type == WorldObjectType::Hazard) {
        agent_.health -= 0.2f;
        result.reward = obj.value;
        result.success = true;
        result.message = "hit_hazard";
    }
}

// Utility Functions

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

bool SimpleWorld::isValidPosition(float x, float y) const {
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

uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

// Vision System

void SimpleWorld::generateVision() {
    std::vector<float> vision = generateVisionRays();
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    std::vector<float> touch = generateTouchSensors();
    sensory_.setTouch(touch);
    
    std::vector<float> internal = generateInternalSignals();
    sensory_.setInternal(internal);
    
    std::vector<float> proprioception = generateProprioception();
    sensory_.setProprioception(proprioception);
    
    sensory_.setAudio({});
}

std::vector<float> SimpleWorld::generateVisionRays() {
    const int rayCount = 16;
    const float fov = M_PI / 2.0f;
    const float maxRange = 8.0f;
    
    std::vector<float> vision(rayCount, 1.0f);
    
    for (int i = 0; i < rayCount; ++i) {
        float angle = calculateVisionRayAngle(i, rayCount, fov);
        float intensity = castVisionRay(angle, maxRange);
        vision[i] = intensity;
    }
    
    return vision;
}

float SimpleWorld::castVisionRay(float angle, float maxRange) {
    float dx = std::cos(angle);
    float dy = std::sin(angle);
    
    float t = 0.0f;
    while (t < maxRange) {
        float px = agent_.x + dx * t;
        float py = agent_.y + dy * t;
        
        if (!isValidPosition(px, py)) {
            return t / maxRange;
        }
        
        WorldObject* obj = getObjectAt(px, py);
        if (obj != nullptr) {
            return calculateVisionIntensity(obj, t, maxRange);
        }
        
        t += 0.1f;
    }
    
    return 1.0f;
}

float SimpleWorld::calculateVisionIntensity(WorldObject* obj, float distance, float maxRange) {
    float intensity = (maxRange - distance) / maxRange;
    
    if (obj->type == WorldObjectType::Resource) {
        return intensity * 0.8f;
    } else if (obj->type == WorldObjectType::Hazard) {
        return intensity * 0.3f;
    } else {
        return 0.5f;
    }
}

float SimpleWorld::calculateVisionRayAngle(int rayIndex, int totalRays, float fov) {
    return agent_.orientation - fov / 2.0f + (fov * rayIndex / (totalRays - 1));
}

std::vector<float> SimpleWorld::generateTouchSensors() {
    const int touchCount = 8;
    std::vector<float> touch(touchCount, 0.0f);
    
    for (int i = 0; i < touchCount; ++i) {
        float angle = calculateTouchSensorAngle(i, touchCount);
        touch[i] = castTouchRay(angle);
    }
    
    return touch;
}

float SimpleWorld::castTouchRay(float angle) {
    float dx = std::cos(angle);
    float dy = std::sin(angle);
    
    float t = 0.0f;
    while (t < 1.5f) {
        float px = agent_.x + dx * t;
        float py = agent_.y + dy * t;
        
        if (!isValidPosition(px, py) || getObjectAt(px, py) != nullptr) {
            return 1.0f - t / 1.5f;
        }
        
        t += 0.1f;
    }
    
    return 0.0f;
}

float SimpleWorld::calculateTouchSensorAngle(int sensorIndex, int totalSensors) {
    return agent_.orientation + 2.0f * M_PI * sensorIndex / totalSensors;
}

std::vector<float> SimpleWorld::generateInternalSignals() {
    std::vector<float> internal(4, 0.0f);
    internal[0] = agent_.energy / maxEnergy_;
    internal[1] = agent_.health;
    internal[2] = agent_.isMoving ? 1.0f : 0.0f;
    internal[3] = agent_.isTurning ? 1.0f : 0.0f;
    return internal;
}

std::vector<float> SimpleWorld::generateProprioception() {
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = agent_.x / width_;
    proprioception[1] = agent_.y / height_;
    proprioception[2] = normalizeOrientationTo01(agent_.orientation);
    proprioception[3] = normalizeVelocity(agent_.velocityX);
    proprioception[4] = normalizeVelocity(agent_.velocityY);
    proprioception[5] = normalizeVelocity(agent_.angularVelocity);
    return proprioception;
}

float SimpleWorld::normalizeOrientationTo01(float orientation) {
    return (orientation + M_PI) / (2.0f * M_PI);
}

float SimpleWorld::normalizeVelocity(float velocity) {
    return std::abs(velocity) / 5.0f;
}

// Collision Detection and Interaction

void SimpleWorld::checkCollisions() {
    WorldObject* obj = getObjectAt(agent_.x, agent_.y);
    if (obj != nullptr && obj->type == WorldObjectType::Hazard) {
        agent_.health -= 0.01f;
    }
}

// Motor command application (maintained original structure)

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    ActionResult result;
    const float moveSpeed = 3.0f;
    const float turnSpeed = 2.0f;
    
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
                result.reward = -0.01f;
                result.success = true;
            } else {
                result.reward = -0.05f;
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

} // namespace nlm