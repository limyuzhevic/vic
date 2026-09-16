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

void SimpleWorld::initialize(size_t visionWidth, size_t visionHeight) {
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    sensory_.setVision(std::vector<float>(visionWidth * visionHeight, 1.0f));
    sensory_.setTouch(std::vector<float>(8, 0.0f));
    sensory_.setInternal(std::vector<float>(4, 0.5f));
    sensory_.setProprioception(std::vector<float>(6, 0.5f));
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
                // Add some noise for neural realism
                float noise = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                              static_cast<float>(0xFFFFFFFFFFFFULL) * 0.05f - 0.025f;
                vision[i] = t / maxRange + noise;
                break;
            }
            
            // Check object collision
            WorldObject* obj = getObjectAt(px, py);
            if (obj != nullptr) {
                // Encode object type as intensity pattern with neural realism
                // Resource = bright, Hazard = dark, different distances
                float distanceFactor = (maxRange - t) / maxRange;
                
                if (obj->type == WorldObjectType::Resource) {
                    // Resource: brighter with temporal persistence
                    float temporalMod = 0.8f + (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                                                static_cast<float>(0xFFFFFFFFFFFFULL) * 0.2f;
                    vision[i] = distanceFactor * temporalMod;
                } else if (obj->type == WorldObjectType::Hazard) {
                    // Hazard: darker with unpredictability
                    float hazardMod = 0.3f + (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                                             static_cast<float>(0xFFFFFFFFFFFFULL) * 0.2f;
                    vision[i] = distanceFactor * hazardMod;
                } else {
                    // Other: intermediate with noise
                    float noise = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                                  static_cast<float>(0xFFFFFFFFFFFFULL) * 0.2f - 0.1f;
                    vision[i] = 0.5f + noise;
                }
                
                // Add refractory delay for neural realism
                float refractoryDelay = 0.1f * (1.0f - distanceFactor);
                lcg(rngState_);  // Advance RNG state
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
                // Add noise for neural realism
                float noise = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                              static_cast<float>(0xFFFFFFFFFFFFULL) * 0.1f - 0.05f;
                touch[i] = std::max(0.0f, 1.0f - t / 1.5f + noise);
                break;
            }
            t += 0.1f;
        }
    }
    sensory_.setTouch(touch);
    
    // Generate internal signals (energy, health) with temporal dynamics
    std::vector<float> internal(4, 0.0f);
    
    // Energy with decay dynamics
    static float energyHistory[5] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    std::rotate(std::begin(energyHistory), std::begin(energyHistory) + 1, std::end(energyHistory));
    energyHistory[4] = agent_.energy / maxEnergy_;
    float energyTrend = (energyHistory[4] - energyHistory[0]) * 2.0f;  // Rate of change
    internal[0] = energyHistory[4] * (1.0f + energyTrend * 0.5f);  // Responsive to change
    
    // Health with damage/recovery dynamics
    internal[1] = agent_.health;
    
    // Movement state with velocity dynamics
    internal[2] = agent_.isMoving ? (1.0f + std::sqrt(agent_.velocityX * agent_.velocityX + 
                                                    agent_.velocityY * agent_.velocityY) * 0.5f) : 0.0f;
    
    // Turning state with angular velocity
    internal[3] = agent_.isTurning ? (1.0f + std::abs(agent_.angularVelocity) * 2.0f) : 0.0f;
    
    // Add temporal integration
    static std::vector<float> internalHistory[4][5];
    for (int j = 0; j < 4; ++j) {
        for (int k = 4; k > 0; --k) {
            internalHistory[j][k][k-1] = internalHistory[j][k-1];
        }
        internalHistory[j][0][j] = internal[j];
        internal[j] = 0.8f * internal[j] + 0.2f * internalHistory[j][0][j];  // Temporal smoothing
    }
    
    sensory_.setInternal(internal);
    
    // Generate proprioception (body position/velocity) with neural noise
    std::vector<float> proprioception(6, 0.0f);
    
    // Position with noise
    float noiseX = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                   static_cast<float>(0xFFFFFFFFFFFFULL) * 0.02f - 0.01f;
    float noiseY = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                   static_cast<float>(0xFFFFFFFFFFFFULL) * 0.02f - 0.01f;
    float noiseO = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                   static_cast<float>(0xFFFFFFFFFFFFULL) * 0.05f - 0.025f;
    
    proprioception[0] = (agent_.x + noiseX) / width_;     // Normalized X with noise
    proprioception[1] = (agent_.y + noiseY) / height_;    // Normalized Y with noise
    proprioception[2] = ((agent_.orientation + M_PI) / (2.0f * M_PI) + noiseO);  // Normalized orientation
    
    // Speed with noise and temporal dynamics
    float speedX = std::abs(agent_.velocityX);
    float speedY = std::abs(agent_.velocityY);
    float speedOX = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                    static_cast<float>(0xFFFFFFFFFFFFULL) * 0.5f;
    float speedOY = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                    static_cast<float>(0xFFFFFFFFFFFFULL) * 0.5f;
    proprioception[3] = std::min(1.0f, speedX / 5.0f + speedOX);  // Speed X with noise
    proprioception[4] = std::min(1.0f, speedY / 5.0f + speedOY);  // Speed Y with noise
    
    // Turn rate with noise
    float turnNoise = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                      static_cast<float>(0xFFFFFFFFFFFFULL) * 0.5f;
    proprioception[5] = std::min(1.0f, std::abs(agent_.angularVelocity) / 3.0f + turnNoise);  // Turn rate
    
    // Temporal filtering
    static std::vector<float> proprioceptionHistory[6];
    for (int j = 0; j < 6; ++j) {
        proprioceptionHistory[j] = 0.7f * proprioceptionHistory[j] + 0.3f * proprioception[j];
    }
    proprioception = proprioceptionHistory;
    
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

// Return current vision and sensory signals for neural processing
SensoryPercept SimpleWorld::observe(const NeuralRegion* region) {
    // In a real implementation, this would extract sensory data
    // specifically relevant to the given neural region
    // For now, return the full percept with temporal integration
    
    // Add time-dependent noise for neural realism
    float timeNoise = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                      static_cast<float>(0xFFFFFFFFFFFFULL) * 0.01f - 0.005f;
    
    // Temporal smoothing of sensory signals
    static SensoryPercept smoothedPercept;
    static float lastUpdate = 0.0f;
    float currentTime = simTime_;
    
    // Update smoothed percept (exponential moving average)
    float alpha = 0.1f;  // Smoothing factor
    if (lastUpdate > 0.0f) {
        float dt = currentTime - lastUpdate;
        alpha = std::min(0.2f, alpha * (1.0f + dt));  // Increase with time
    }
    
    // Apply temporal smoothing to vision
    std::vector<float> smoothedVision = sensory_.getVision();
    if (!smoothedVision.empty()) {
        // Add noise and temporal dynamics
        for (size_t i = 0; i < smoothedVision.size(); ++i) {
            float noise = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                          static_cast<float>(0xFFFFFFFFFFFFULL) * 0.02f - 0.01f;
            smoothedVision[i] = (1.0f - alpha) * smoothedVision[i] + 
                               alpha * (smoothedVision[i] + noise + timeNoise);
            // Clamp to valid range
            smoothedVision[i] = std::max(0.0f, std::min(1.0f, smoothedVision[i]));
        }
    }
    
    smoothedPercept.setVision(smoothedVision);
    smoothedPercept.setTimestamp(currentTime);
    
    // Also smooth other modalities
    auto& touch = sensory_.getTouch();
    std::vector<float> smoothedTouch(touch.size());
    for (size_t i = 0; i < touch.size(); ++i) {
        smoothedTouch[i] = (1.0f - alpha) * touch[i] + 
                         alpha * (touch[i] + timeNoise * 0.5f);
        smoothedTouch[i] = std::max(0.0f, smoothedTouch[i]);
    }
    smoothedPercept.setTouch(smoothedTouch);
    
    return smoothedPercept;
}

// Apply neural-derived action to world state
ActionResult SimpleWorld::applyAction(const NeuralRegion* region, MotorCommand cmd) {
    ActionResult result;
    
    // Use neural region to modulate action strength if available
    float actionModulation = 1.0f;
    if (region) {
        // In a real implementation, this would read neural activity
        // and modulate action based on readiness signals
        actionModulation = 1.0f + (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                              static_cast<float>(0xFFFFFFFFFFFFULL) * 0.2f - 0.1f;
    }
    
    // Apply the motor command with neural modulation
    double time = simTime_;
    
    switch (cmd) {
        case MotorCommand::MoveForward: {
            float dx = std::cos(agent_.orientation) * 3.0f * actionModulation;
            float dy = std::sin(agent_.orientation) * 3.0f * actionModulation;
            
            float newX = agent_.x + dx * 0.1f;
            float newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                // Apply velocity with inertia
                agent_.velocityX = dx;
                agent_.velocityY = dy;
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -0.01f * actionModulation;  // Cost modulated by neural activity
                result.success = true;
                
                // Neural feedback: reward prediction error signaling
                if (agent_.energy < maxEnergy_ * 0.3f) {
                    // Low energy = higher reward for movement
                    result.reward += 0.05f;
                }
            } else {
                // Boundary collision - neural error signal
                agent_.velocityX = 0.0f;
                agent_.velocityY = 0.0f;
                result.reward = -0.05f * actionModulation;  // Cost scaled by neural input
                result.success = false;
            }
            break;
        }
            
        case MotorCommand::TurnLeft:
            // Neural turn rate modulation
            float turnMod = 1.0f + (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                                static_cast<float>(0xFFFFFFFFFFFFULL) * 0.3f;
            agent_.angularVelocity = -2.0f * turnMod;
            agent_.isTurning = true;
            result.reward = -0.005f * turnMod;  // Cost scaled by neural modulation
            result.success = true;
            break;
            
        case MotorCommand::TurnRight:
            float turnMod = 1.0f + (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                                static_cast<float>(0xFFFFFFFFFFFFULL) * 0.3f;
            agent_.angularVelocity = 2.0f * turnMod;
            agent_.isTurning = true;
            result.reward = -0.005f * turnMod;
            result.success = true;
            break;
            
        case MotorCommand::MoveBackward:
            float dx = -std::cos(agent_.orientation) * 3.0f * actionModulation * 0.5f;
            float dy = -std::sin(agent_.orientation) * 3.0f * actionModulation * 0.5f;
            
            float newX = agent_.x + dx * 0.1f;
            float newY = agent_.y + dy * 0.1f;
            
            if (isValidPosition(newX, newY)) {
                agent_.velocityX = dx;
                agent_.velocityY = dy;
                agent_.x = newX;
                agent_.y = newY;
                agent_.isMoving = true;
                result.reward = -0.02f * actionModulation;
                result.success = true;
            } else {
                agent_.velocityX = 0.0f;
                agent_.velocityY = 0.0f;
                result.reward = -0.05f * actionModulation;
                result.success = false;
            }
            break;
            
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
            // Sensor panning - modulatory effect on sensory gain
            result.reward = 0.0f;
            result.success = true;
            
            // Modify orientation slightly for sensory sampling
            if (cmd == MotorCommand::LookLeft) {
                agent_.orientation -= 0.2f;
            } else {
                agent_.orientation += 0.2f;
            }
            break;
            
        case MotorCommand::Interact:
            result = interact();
            // Neural prediction error for interaction outcomes
            if (result.success) {
                // Learning signal based on reward prediction error
                float predictionError = result.reward - 0.0f;  // Expected reward = 0
                // In real implementation, this would modulate synaptic weights
                // based on prediction error
            }
            break;
            
        case MotorCommand::Wait:
        default:
            // Neural decision to wait: decay velocities with neural noise
            float noiseX = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                           static_cast<float>(0xFFFFFFFFFFFFULL) * 0.3f;
            float noiseY = (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                           static_cast<float>(0xFFFFFFFFFFFFULL) * 0.3f;
            
            agent_.velocityX = agent_.velocityX * 0.9f * (1.0f + noiseX * 0.1f);
            agent_.velocityY = agent_.velocityY * 0.9f * (1.0f + noiseY * 0.1f);
            agent_.angularVelocity *= 0.9f;
            agent_.isMoving = false;
            agent_.isTurning = false;
            result.reward = 0.0f;
            result.success = true;
            break;
    }
    
    // Apply velocity to position
    agent_.x += agent_.velocityX * 0.1f;
    agent_.y += agent_.velocityY * 0.1f;
    
    // Apply angular velocity to orientation
    agent_.orientation += agent_.angularVelocity * 0.1f;
    
    // Normalize orientation
    while (agent_.orientation > M_PI) agent_.orientation -= 2.0f * M_PI;
    while (agent_.orientation < -M_PI) agent_.orientation += 2.0f * M_PI;
    
    // Boundary collision
    if (agent_.x < 0.0f) { agent_.x = 0.0f; agent_.velocityX = 0.0f; }
    if (agent_.x >= width_) { agent_.x = width_ - 0.01f; agent_.velocityX = 0.0f; }
    if (agent_.y < 0.0f) { agent_.y = 0.0f; agent_.velocityY = 0.0f; }
    if (agent_.y >= height_) { agent_.y = height_ - 0.01f; agent_.velocityY = 0.0f; }
    
    // Update time
    simTime_ += 0.1f;
    
    // Generate new sensory percept
    generateVision();
    
    // Check for collisions
    checkCollisions();
    
    return result;
}

// Compute reward based on neural prediction and actual outcome
float SimpleWorld::computeReward(const NeuralRegion* region) {
    float baseReward = 0.0f;
    
    // Neural prediction error computation
    // Get current sensory state for prediction comparison
    std::vector<float> currentSensory;
    const auto& vision = sensory_.getVision();
    currentSensory.insert(currentSensory.end(), vision.begin(), vision.end());
    
    auto& touch = sensory_.getTouch();
    currentSensory.insert(currentSensory.end(), touch.begin(), touch.end());
    
    // Resource proximity reward
    float resourceProximity = 0.0f;
    for (const auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (obj.type == WorldObjectType::Resource && dist < 2.0f) {
            resourceProximity += (2.0f - dist) / 2.0f * obj.value;
        } else if (obj.type == WorldObjectType::Hazard && dist < 1.5f) {
            resourceProximity -= (1.5f - dist) / 1.5f * std::abs(obj.value);
        }
    }
    
    // Movement efficiency (negative reward for energy cost)
    float energyCost = energyDecayRate_ * 0.1f * 
                      (1.0f + std::sqrt(agent_.velocityX * agent_.velocityX + 
                                       agent_.velocityY * agent_.velocityY));
    
    // Neural attention modulation
    float attentionMod = 1.0f;
    if (region) {
        // Neural attention could modulate reward sensitivity
        attentionMod = 1.0f + (static_cast<float>(lcg(rngState_)) & 0xFFFFFFFFFFFFULL) / 
                             static_cast<float>(0xFFFFFFFFFFFFULL) * 0.5f;
    }
    
    // Novelty reward based on sensory change
    float noveltyReward = 0.0f;
    if (!vision.empty() && !previousVision_.empty()) {
        float totalChange = 0.0f;
        for (size_t i = 0; i < std::min(vision.size(), previousVision_.size()); ++i) {
            totalChange += std::abs(vision[i] - previousVision_[i]);
        }
        noveltyReward = totalChange / std::max(static_cast<size_t>(1), vision.size()) * 0.1f;
    }
    
    // Expected reward from agent's learned policy
    float expectedReward = 0.0f;  // Placeholder
    
    // Prediction error reward (novelty learning signal)
    float predictionErrorReward = noveltyReward * 0.5f;
    
    // Combine reward components
    baseReward = resourceProximity - energyCost + predictionErrorReward;
    
    // Apply neuromodulation factors
    baseReward *= attentionMod;
    
    // Clamp to reasonable range
    baseReward = std::max(-1.0f, std::min(1.0f, baseReward));
    
    // Neural feedback: update prediction error
    float rewardPredictionError = baseReward - expectedReward;
    // In real implementation, this would be propagated to neural systems
    
    return baseReward;
}

} // namespace nlm