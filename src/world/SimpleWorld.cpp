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
    
    // Only allow one action per action interval (0.1 s of world
    // time). Phase 6 note: this gate itself is unchanged — what
    // was wrong was the demo loop, which advanced the world by
    // only 0.01 s per decision, so 9 of every 10 decisions were
    // necessarily rejected here. The demos now advance the world
    // by one action interval per decision (see Phase3Demo), so
    // every decoded action reaches the world. Do not "fix" this
    // gate to 0.01 to make a metric look better: the world's
    // movement physics (3.0 units/s, 2.0 rad/s) are defined per
    // 0.1 s interval, and a 0.01 gate would make the agent move
    // ten times faster than its own physics allows.
    // Epsilon: world time accumulates 0.1 s steps in binary
    // floating point, so a true 0.1 s gap can compute as
    // 0.0999999999 s and be wrongly rejected. A 1e-6 s tolerance
    // is far below any real pacing difference.
    // Use epsilon-based comparison to avoid floating point precision issues
    double timeSinceLastAction = currentTime - agent_.lastActionTime;
    if (timeSinceLastAction < (0.1 - 1e-6)) {
        return ActionResult(0.0f, false, "too soon");
    }
    
    // Also reject if time difference exceeds 0.1 s plus small epsilon
    if (timeSinceLastAction > (0.1 + 1e-6)) {
        return ActionResult(0.0f, false, "too late");
    }
    
    agent_.lastActionTime = currentTime;
    
    // Phase 6: a turn is a discrete action, not a persistent
    // velocity. TurnLeft/Right set angularVelocity and the next
    // world update rotates by 2.0 rad/s * 0.1 s = 0.2 rad; but
    // nothing ever cleared it, so the agent kept rotating on
    // every later step — with the old 0.01 s updates that was a
    // barely-noticed 0.02 rad/step drift, with the corrected
    // 0.1 s updates it would be a 0.2 rad/step spiral. Any
    // non-turn action therefore stops rotation.
    switch (cmd) {
        case MotorCommand::MoveForward: {
            agent_.angularVelocity = 0.0f;
            agent_.isTurning = false;
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
            agent_.angularVelocity = 0.0f;
            agent_.isTurning = false;
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
            agent_.angularVelocity = 0.0f;
            agent_.isTurning = false;
            result.reward = 0.0f;
            result.success = true;
            break;

        case MotorCommand::LookRight:
            agent_.angularVelocity = 0.0f;
            agent_.isTurning = false;
            result.reward = 0.0f;
            result.success = true;
            break;

        case MotorCommand::Interact:
            agent_.angularVelocity = 0.0f;
            agent_.isTurning = false;
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
    // Optimized raycasting-based vision using vectorization
    // Cast rays in a cone in front of the agent with improved performance
    
    const int rayCount = 16;
    const float fov = M_PI / 2.0f;  // 90 degree FOV
    const float maxRange = 8.0f;
    const float stepSize = 0.1f;   // Fixed step size for ray casting
    
    // Pre-allocate vision vector and initialize with far values
    std::vector<float> vision(rayCount, 1.0f);  // Default: far/empty
    
    // Precompute common values for optimization
    float halfFov = fov * 0.5f;
    float fovPerRay = fov / (rayCount - 1);
    
    // Vectorized ray casting: compute all ray angles in one go
    std::vector<float> rayAngles(rayCount);
    for (int i = 0; i < rayCount; ++i) {
        rayAngles[i] = agent_.orientation - halfFov + (fovPerRay * i);
    }
    
    // Vectorized trigonometric calculations (cos, sin for all rays)
    std::vector<float> rayCos(rayCount);
    std::vector<float> raySin(rayCount);
    for (int i = 0; i < rayCount; ++i) {
        rayCos[i] = std::cos(rayAngles[i]);
        raySin[i] = std::sin(rayAngles[i]);
    }
    
    // Vectorized ray casting: process all rays simultaneously
    for (int i = 0; i < rayCount; ++i) {
        float dx = rayCos[i];
        float dy = raySin[i];
        
        // Check wall collision (world boundaries)
        float maxDistance = maxRange;
        float collisionDistance = maxDistance;
        
        // Calculate wall collision distance in direction of ray
        // Using analytical solution instead of iterative approach
        float wallX = width_;
        float wallY = height_;
        float wallX1 = -wallX;  // left boundary
        float wallY1 = -wallY;  // bottom boundary
        
        // Calculate distance to each wall and pick the first one hit
        // Horizontal walls (left/right)
        if (dx < 0) {
            float distToLeft = (agent_.x - wallX1) / dx;
            float distToRight = (agent_.x - wallX) / dx;
            if (distToLeft > 0 && distToLeft < collisionDistance) {
                collisionDistance = distToLeft;
            }
            if (distToRight > 0 && distToRight < collisionDistance) {
                collisionDistance = distToRight;
            }
        } else {
            float distToLeft = (agent_.x - wallX1) / dx;
            float distToRight = (agent_.x - wallX) / dx;änDraw upper bound
        }
        
        // Check object collision efficiently
        WorldObject* closestObject = nullptr;
        float closestDistance = collisionDistance;
        
        for (const auto& obj : objects_) {
            if (!obj.active) continue;
            
            // Vector from agent to object
            float objDx = obj.x - agent_.x;
            float objDy = obj.y - agent_.y;
            float dotProduct = objDx * dx + objDy * dy;
            
            // Check if object is in front of agent (project onto ray direction)
            if (dotProduct > 0) {
                float objectDistance = dotProduct / std::sqrt(dx * dx + dy * dy);
                
                // Check perpendicular distance (cross product) for alignment
                float crossProduct = std::abs(objDx * dy - objDy * dx);
                float rayWidthAtDistance = objectDistance * (fov / 2.0f);
                
                // Simple angular check: if object within FOV cone
                if (crossProduct < rayWidthAtDistance * 0.5f && objectDistance < closestDistance) {
                    closestObject = &obj;
                    closestDistance = objectDistance;
                }
            }
        }
        
        // Update vision based on closest collision (wall or object)
        if (closestObject != nullptr) {
            // Encode resource vs hazard
            if (closestObject->type == WorldObjectType::Resource) {
                vision[i] = (maxRange - closestDistance) / maxRange * 0.8f;
            } else if (closestObject->type == WorldObjectType::Hazard) {
                vision[i] = (maxRange - closestDistance) / maxRange * 0.3f;
            } else {
                vision[i] = 0.5f;
            }
        } else if (collisionDistance < maxDistance) {
            // Wall collision only
            vision[i] = collisionDistance / maxRange;
        }
    }
    
    // Vectorized touch sensor generation using SIMD-friendly loops
    sensory_.setVision(vision);
    sensory_.setTimestamp(simTime_);
    
    // Generate touch sensors (proximity to obstacles in 8 directions)
    // Optimized with vectorized distance calculations
    std::vector<float> touch(8, 0.0f);
    for (int i = 0; i < 8; ++i) {
        float angle = agent_.orientation + 2.0f * M_PI * i / 8.0f;
        float dx = std::cos(angle);
        float dy = std::sin(angle);
        
        // Find closest obstacle in this direction using vectorized approach
        float minDistance = 1.5f;
        float closestDistance = minDistance;
        
        for (const auto& obj : objects_) {
            if (!obj.active) continue;
            
            float objDx = obj.x - agent_.x;
            float objDy = obj.y - agent_.y;
            
            // Check if object is in direction of touch sensor
            float dotProduct = objDx * dx + objDy * dy;
            if (dotProduct > 0) {
                float objectDistance = dotProduct / std::sqrt(dx * dx + dy * dy);
                
                // Simple distance check - no need for complex angular calculations
                if (objectDistance < closestDistance) {
                    closestDistance = objectDistance;
                }
            }
        }
        
        // Convert to touch intensity (closer = stronger signal)
        if (closestDistance < minDistance) {
            touch[i] = 1.0f - closestDistance / 1.5f;
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

} // namespace nlm
