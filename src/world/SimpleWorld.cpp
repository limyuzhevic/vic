#include "SimpleWorld.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>
#include <numeric>
#include <map>

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

// Improved random distribution with more sophisticated sampling
static float normalRandom(uint64_t& state, float mean, float stddev) {
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    float u1 = dist(state);
    float u2 = dist(state);
    float z0 = std::sqrt(-2.0f * std::log(std::max(u1 * u1, 0.0001f))) * std::cos(2.0f * M_PI * u2);
    return mean + stddev * z0;
}

SimpleWorld::SimpleWorld()
    : width_(20.0f)
    , height_(20.0f)
    , visionWidth_(16)
    , visionHeight_(16)
    , maxEnergy_(100.0f)
    , energyDecayRate_(0.01f)
    , resourceEnergyGain_(30.0f)
    , hazardDamage_(0.2f)
    , simTime_(0.0)
    , rngSeed_(42)
    , rngState_(42)
    , objectDensity_(1.0f)
    , wallThickness_(0.5f)
    , enableDynamicObjects_(true)
    , physicsIterations_(3)
    , smoothingFactor_(0.7f)
    , enableMapFeatures_(false)
    , environmentComplexity_(1.0f)
    , objectNoise_(0.1f)
    , boundarySafety_(0.5f)
    , collisionResolution_(1)
    , enableAttractionForces_(false)
    , repulsionStrength_(1.0f)
    , enableTerrainFeatures_(false)
    , terrainRoughness_(0.5f)
    , enableWindSimulation_(false)
    , windStrength_(0.0f)
    , windVariation_(0.0f)
    , enableTemperatureEffects_(false)
    , temperature_(20.0f)
    , thermalNoise_(1.0f)
{
    agent_.reset();
    
    // Initialize advanced state variables
    lastAgentPosition_ = {agent_.x, agent_.y};
    velocityHistory_.reserve(100);
    objectGenerationTime_ = 0.0;
    environmentSeed_ = rngSeed_;
    
    // Setup advanced random generators
    std::seed_seq seed{rngSeed_};
    legacyRng_.seed(seed);
    mt19937_64 rng_obj{seed};
    standard_normal_.seed(seed);
}

SimpleWorld::~SimpleWorld() = default;

void SimpleWorld::configure(float width, float height, size_t visionWidth, size_t visionHeight) {
    width_ = width;
    height_ = height;
    visionWidth_ = visionWidth;
    visionHeight_ = visionHeight;
    
    // Recalculate based on new dimensions
    if (enableTerrainFeatures_) {
        generateTerrain(width, height);
    }
}

void SimpleWorld::setRandomSeed(uint64_t seed) {
    rngSeed_ = seed;
    rngState_ = seed;
    
    // Reinitialize all random generators
    std::seed_seq seed_seq{rngSeed_};
    legacyRng_.seed(seed_seq);
    mt19937_64 rng_obj{seed_seq};
    standard_normal_.seed(seed_seq);
}

void SimpleWorld::reset() {
    simTime_ = 0.0;
    agent_.reset();
    
    // Initialize with more sophisticated placement
    float safeX = width_ * (boundarySafety_ + 0.5f) / 2.0f;
    float safeY = height_ * (boundarySafety_ + 0.5f) / 2.0f;
    
    agent_.x = legacyRng_() % static_cast<int>(width_ * (1.0f - boundarySafety_)) + static_cast<float>(safeX);
    agent_.y = legacyRng_() % static_cast<int>(height_ * (1.0f - boundarySafety_)) + static_cast<float>(safeY);
    
    agent_.energy = maxEnergy_;
    
    // Enhanced object management with better lifecycle
    rngState_ = rngSeed_;  // Reset RNG for reproducible object placement
    
    // Clear and regenerate objects with advanced strategies
    objects_.clear();
    generateAdvancedEnvironment();
    
    // Generate initial vision
    generateVision();
}

void SimpleWorld::addObject(const WorldObject& obj) {
    // Enhanced object addition with validation
    if (!isValidPosition(obj.x, obj.y)) {
        std::cerr << "Warning: Object position (" << obj.x << ", " << obj.y 
                 << ") is outside world boundaries" << std::endl;
        return;
    }
    
    // Check for excessive clustering
    if (enableMapFeatures_) {
        checkObjectClustering(obj);
    }
    
    objects_.push_back(obj);
    
    // Update environment state
    if (enableDynamicObjects_) {
        updateEnvironmentDynamics();
    }
}

void SimpleWorld::generateAdvancedEnvironment() {
    // Enhanced environment generation with procedural generation
    std::vector<std::pair<float, float>> resourcePositions;
    std::vector<std::pair<float, float>> hazardPositions;
    
    // Generate resource objects with noise-based distribution
    float resourceArea = width_ * height_ * (objectDensity_ * 0.3f);
    int numResources = static_cast<int>(std::sqrt(resourceArea) * 2.0f);
    
    for (int i = 0; i < numResources; ++i) {
        // Use Perlin-like noise for organic placement
        float noiseX = legacyRng_() % 1000 / 1000.0f;
        float noiseY = legacyRng_() % 1000 / 1000.0f;
        
        float x = width_ * (boundarySafety_ + (1.0f - boundarySafety_) * noiseX);
        float y = height_ * (boundarySafety_ + (1.0f - boundarySafety_) * noiseY);
        
        // Ensure not too close to agent
        float distToAgent = std::sqrt((x - agent_.x) * (x - agent_.x) + 
                                     (y - agent_.y) * (y - agent_.y));
        if (distToAgent < 3.0f) {
            // Place further away
            x = agent_.x + (x - agent_.x) * 2.5f;
            y = agent_.y + (y - agent_.y) * 2.5f;
        }
        
        // Add resource
        resourcePositions.emplace_back(x, y);
        objects_.emplace_back(x, y, WorldObjectType::Resource, 
                              resourceEnergyGain_ * (0.5f + legacyRng_() % 100 / 200.0f), 
                              0.4f * (1.0f - objectNoise_));
    }
    
    // Generate hazard objects with clustering
    float hazardArea = width_ * height_ * (objectDensity_ * 0.2f);
    int numHazards = static_cast<int>(std::sqrt(hazardArea) * 1.5f);
    
    for (int i = 0; i < numHazards; ++i) {
        // Cluster hazards in groups
        float clusterX = legacyRng_() % 1000 / 1000.0f;
        float clusterY = legacyRng_() % 1000 / 1000.0f;
        
        // Add some random clustering
        for (int j = 0; j < 3; ++j) {
            float offsetX = (legacyRng_() % 200 - 100) / 100.0f;
            float offsetY = (legacyRng_() % 200 - 100) / 100.0f;
            
            float x = width_ * (boundarySafety_ + (1.0f - boundarySafety_) * 
                               (clusterX + offsetX * objectNoise_));
            float y = height_ * (boundarySafety_ + (1.0f - boundarySafety_) * 
                               (clusterY + offsetY * objectNoise_));
            
            // Ensure not too close to agent
            float distToAgent = std::sqrt((x - agent_.x) * (x - agent_.x) + 
                                         (y - agent_.y) * (y - agent_.y));
            if (distToAgent < 3.0f) {
                x = agent_.x + (x - agent_.x) * 2.5f;
                y = agent_.y + (y - agent_.y) * 2.5f;
            }
            
            hazardPositions.emplace_back(x, y);
            objects_.emplace_back(x, y, WorldObjectType::Hazard,
                                  -hazardDamage_ * (1.0f + legacyRng_() % 100 / 100.0f),
                                  0.3f * (1.0f - objectNoise_));
        }
    }
    
    // Generate terrain features if enabled
    if (enableTerrainFeatures_) {
        generateTerrainFeatures();
    }
    
    // Generate landmarks/markers if enabled
    if (enableMapFeatures_) {
        generateMapFeatures();
    }
}

void SimpleWorld::generateTerrain(float width, float height) {
    // Generate procedural terrain with smooth transitions
    terrainMap_.clear();
    terrainMap_.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
    
    // Simple height map generation using diamond-square algorithm
    const int resolution = 10;  // Lower resolution for performance
    float scale = 50.0f;
    
    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x < resolution; ++x) {
            float nx = x / static_cast<float>(resolution - 1);
            float ny = y / static_cast<float>(resolution - 1);
            
            // Simple fractal noise (placeholder for Perlin noise)
            float noise = (legacyRng_() % 1000 / 1000.0f) * 2.0f - 1.0f;
            float height_val = (noise + 1.0f) * 0.5f * terrainRoughness_;
            
            // Interpolate to full resolution
            for (int iy = y * resolution / height; iy < std::min((y + 1) * resolution / height, height); ++iy) {
                for (int ix = x * resolution / width; ix < std::min((x + 1) * resolution / width, width); ++ix) {
                    terrainMap_[iy * static_cast<size_t>(width) + ix] = height_val;
                }
            }
        }
    }
}

void SimpleWorld::generateTerrainFeatures() {
    // Generate hills, valleys, and obstacles based on terrain
    float hillProbability = 0.3f * environmentComplexity_;
    
    for (auto& obj : objects_) {
        float terrainHeight = terrainMap_[static_cast<size_t>(obj.x) * static_cast<size_t>(width_) + 
                                          static_cast<size_t>(obj.y)];
        
        // Modify object properties based on terrain
        if (terrainHeight > 0.7f) {
            // Hills - smaller radius, higher value
            obj.radius *= 0.7f;
            if (obj.type == WorldObjectType::Resource) {
                obj.value *= 1.5f;
            }
        } else if (terrainHeight < 0.3f) {
            // Valleys - smaller radius
            obj.radius *= 0.8f;
        }
        
        // Add terrain-based features
        if (legacyRng_() % 100 < static_cast<int>(hillProbability * 100)) {
            // Create terrain features
            float featureX = obj.x + (legacyRng_() % 200 - 100) / 100.0f * width_ * 0.1f;
            float featureY = obj.y + (legacyRng_() % 200 - 100) / 100.0f * height_ * 0.1f;
            
            if (isValidPosition(featureX, featureY)) {
                objects_.emplace_back(featureX, featureY, WorldObjectType::Marker, 
                                     terrainHeight * 20.0f, obj.radius * 0.5f);
            }
        }
    }
}

void SimpleWorld::generateMapFeatures() {
    // Generate navigation aids and landmarks
    // Create a waypoint system for exploration
    
    std::vector<std::pair<float, float>> waypoints;
    
    // Generate waypoints in clusters
    for (int cluster = 0; cluster < 5; ++cluster) {
        float clusterX = legacyRng_() % 1000 / 1000.0f * width_;
        float clusterY = legacyRng_() % 1000 / 1000.0f * height_;
        
        for (int i = 0; i < 3; ++i) {
            float offsetX = (legacyRng_() % 200 - 100) / 100.0f * width_ * 0.05f;
            float offsetY = (legacyRng_() % 200 - 100) / 100.0f * height_ * 0.05f;
            
            float x = clusterX + offsetX;
            float y = clusterY + offsetY;
            
            if (isValidPosition(x, y)) {
                waypoints.emplace_back(x, y);
                objects_.emplace_back(x, y, WorldObjectType::Marker, 5.0f, 0.2f);
            }
        }
    }
    
    // Store waypoints for pathfinding
    // waypoints_ would be updated in a full implementation
}

void SimpleWorld::checkObjectClustering(const WorldObject& newObj) {
    // Prevent excessive clustering of similar objects
    int similarCount = 0;
    int totalCount = 0;
    
    for (const auto& obj : objects_) {
        totalCount++;
        if (std::abs(obj.type - newObj.type) < 0.5f) {  // Same type
            float dist = std::sqrt((obj.x - newObj.x) * (obj.x - newObj.x) +
                                  (obj.y - newObj.y) * (obj.y - newObj.y));
            if (dist < 3.0f) {
                similarCount++;
            }
        }
    }
    
    if (totalCount > 0 && static_cast<float>(similarCount) / totalCount > 0.7f) {
        std::cerr << "Warning: High clustering of similar objects detected" << std::endl;
    }
}

void SimpleWorld::updateEnvironmentDynamics() {
    // Update environment based on agent state and time
    if (enableDynamicObjects_) {
        // Smooth agent motion
        float dx = agent_.x - lastAgentPosition_.first;
        float dy = agent_.y - lastAgentPosition_.second;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > 0.01f) {
            velocityHistory_.push_back(distance / physicsIterations_);
            if (velocityHistory_.size() > 10) {
                velocityHistory_.pop_front();
            }
        }
        
        lastAgentPosition_ = {agent_.x, agent_.y};
        
        // Update object states based on dynamics
        for (auto& obj : objects_) {
            if (obj.active && obj.type == WorldObjectType::Resource) {
                // Resources slowly regenerate
                obj.value = std::min(obj.value + 0.01f, 10.0f);
            }
            
            if (obj.type == WorldObjectType::Hazard) {
                // Hazards may move or change properties over time
                if (legacyRng_() % 100 < 5) {  // 5% chance per frame
                    float angle = legacyRng_() % 360 * M_PI / 180.0f;
                    float speed = legacyRng_() % 100 / 1000.0f;
                    obj.x += std::cos(angle) * speed;
                    obj.y += std::sin(angle) * speed;
                }
            }
        }
    }
}

void SimpleWorld::updatePhysics(double timestep) {
    // Advanced physics simulation with multiple iterations
    for (int iteration = 0; iteration < physicsIterations_; ++iteration) {
        // Update agent physics
        updateAgentPhysics(timestep / physicsIterations_);
        
        // Update object interactions
        updateObjectInteractions(timestep / physicsIterations_);
        
        // Apply environmental forces
        if (enableWindSimulation_) {
            applyWindForces(timestep / physicsIterations_);
        }
        
        if (enableTemperatureEffects_) {
            applyTemperatureEffects(timestep / physicsIterations_);
        }
    }
    
    // Smooth position history for better predictions
    if (velocityHistory_.size() > 1) {
        float avgVelocity = std::accumulate(velocityHistory_.begin(), velocityHistory_.end(), 0.0f) / velocityHistory_.size();
        if (avgVelocity > 0.0f) {
            smoothingFactor_ = std::min(smoothingFactor_ * 1.1f, 0.95f);
        }
    }
}

void SimpleWorld::updateAgentPhysics(double timestep) {
    // Enhanced agent physics with drag, buoyancy, and friction
    
    // Apply drag force proportional to velocity squared
    float dragForce = 0.5f * agent_.velocityX * agent_.velocityX + 
                    0.5f * agent_.velocityY * agent_.velocityY;
    
    if (dragForce > 0.0f) {
        float speed = std::sqrt(agent_.velocityX * agent_.velocityX + agent_.velocityY * agent_.velocityY);
        if (speed > 0.0f) {
            float dragCoeff = 0.1f + 0.5f * speed;
            agent_.velocityX *= (1.0f - dragCoeff * timestep);
            agent_.velocityY *= (1.0f - dragCoeff * timestep);
        }
    }
    
    // Apply friction when not moving
    if (agent_.velocityX == 0.0f && agent_.velocityY == 0.0f && agent_.angularVelocity == 0.0f) {
        agent_.isMoving = false;
        agent_.isTurning = false;
    }
    
    // Normalize orientation with smoothing
    if (agent_.angularVelocity != 0.0f) {
        float targetOrientation = agent_.orientation + agent_.angularVelocity * timestep;
        
        // Smooth orientation change
        float orientationDiff = std::fmod(targetOrientation - agent_.orientation + M_PI, 2.0f * M_PI) - M_PI;
        agent_.orientation += orientationDiff * smoothingFactor_;
    }
}

void SimpleWorld::updateObjectInteractions(double timestep) {
    // Update interactions between objects and agent
    
    for (auto& obj : objects_) {
        if (!obj.active) continue;
        
        // Check for agent-object interactions
        float dx = agent_.x - obj.x;
        float dy = agent_.y - obj.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        // Apply attraction/repulsion forces
        if (enableAttractionForces_) {
            if (dist < 5.0f) {
                float force = (dist < 1.0f) ? (1.0f - dist) * repulsionStrength_ : 0.0f;
                float angle = std::atan2(dy, dx);
                agent_.velocityX += std::cos(angle) * force * timestep;
                agent_.velocityY += std::sin(angle) * force * timestep;
            }
        }
        
        // Update object state based on distance
        if (dist < obj.radius * 2.0f && obj.type == WorldObjectType::Resource) {
            // Resource influence on agent
            if (dist < obj.radius) {
                agent_.energy = std::min(maxEnergy_, agent_.energy + obj.value * timestep);
            }
        }
    }
}

void SimpleWorld::applyWindForces(double timestep) {
    // Apply wind forces to agent and objects
    float windX = windStrength_ * (0.5f + legacyRng_() % 100 / 200.0f);
    float windY = windStrength_ * (0.5f + legacyRng_() % 100 / 200.0f);
    
    // Apply to agent (reduces with mass)
    float windForce = std::sqrt(windX * windX + windY * windY);
    agent_.velocityX += windX * timestep * 0.5f;  // Reduced effect
    agent_.velocityY += windY * timestep * 0.5f;
    
    // Apply to objects
    for (auto& obj : objects_) {
        if (obj.active && obj.type == WorldObjectType::Resource) {
            obj.x += windX * timestep * 0.1f;
            obj.y += windY * timestep * 0.1f;
        }
    }
}

void SimpleWorld::applyTemperatureEffects(double timestep) {
    // Apply temperature-based effects to energy and metabolism
    float temperatureEffect = (temperature_ - 20.0f) / 20.0f;  // Normalized from -1 to 1
    
    // Energy consumption varies with temperature
    energyDecayRate_ = 0.01f + std::abs(temperatureEffect) * 0.02f;
    
    // Temperature noise
    float thermalNoise = 1.0f + standard_normal_() * thermalNoise_;
    energyDecayRate_ *= thermalNoise;
    
    // Metabolism rate affects agent
    if (temperatureEffect > 0.5f) {
        // High temperature - faster metabolism
        agent_.energy -= 0.001f * timestep;
    } else if (temperatureEffect < -0.5f) {
        // Low temperature - slower metabolism
        agent_.energy -= 0.0005f * timestep;
    }
}

ActionResult SimpleWorld::applyMotorCommand(MotorCommand cmd, double currentTime) {
    ActionResult result = SimpleWorld::applyMotorCommand(cmd, currentTime);
    
    // Update physics after basic action
    updatePhysics(0.1);  // Assuming 0.1 timestep
    
    return result;
}

void SimpleWorld::generateVision() {
    // Enhanced vision with terrain and feature detection
    SimpleWorld::generateVision();  // Call parent implementation
    
    // Add terrain-based vision effects
    if (enableTerrainFeatures_) {
        enhanceVisionWithTerrain();
    }
    
    // Add feature detection
    if (enableMapFeatures_) {
        detectFeaturesInVision();
    }
}

void SimpleWorld::enhanceVisionWithTerrain() {
    // Modify vision based on terrain features
    auto vision = sensory_.getVision();
    
    for (size_t i = 0; i < vision.size(); ++i) {
        float x = agent_.x + (i % static_cast<int>(visionWidth_)) * 0.1f;
        float y = agent_.y + (i / static_cast<int>(visionWidth_)) * 0.1f;
        
        if (x >= 0 && x < width_ && y >= 0 && y < height_) {
            size_t idx = static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x);
            if (idx < terrainMap_.size()) {
                float terrain = terrainMap_[idx];
                // Apply terrain effects to vision
                if (terrain > 0.8f) {
                    // Hills - can obscure vision
                    vision[i] *= 0.9f;
                } else if (terrain < 0.2f) {
                    // Valleys - may create mirages
                    vision[i] *= (1.0f + terrain * 0.1f);
                }
            }
        }
    }
    
    sensory_.setVision(vision);
}

void SimpleWorld::detectFeaturesInVision() {
    // Detect landmarks and features in vision for navigation
    auto vision = sensory_.getVision();
    
    std::vector<float> featureMap(vision.size(), 0.0f);
    
    for (size_t i = 0; i < vision.size(); ++i) {
        float x = agent_.x + (i % static_cast<int>(visionWidth_)) * 0.1f;
        float y = agent_.y + (i / static_cast<int>(visionWidth_)) * 0.1f;
        
        // Check for map features (markers/landmarks)
        WorldObject* obj = getObjectAt(x, y);
        if (obj != nullptr && obj->type == WorldObjectType::Marker) {
            featureMap[i] = 1.0f;  // Mark as detected feature
        }
    }
    
    // Feature processing could be used for path planning
    // featureMap_ would be updated in a full implementation
}

void SimpleWorld::checkCollisions() {
    SimpleWorld::checkCollisions();  // Call parent implementation
    
    // Enhanced collision detection with physics
    if (enableDynamicObjects_) {
        checkDynamicCollisions();
    }
}

void SimpleWorld::checkDynamicCollisions() {
    // More sophisticated collision detection
    for (size_t i = 0; i < objects_.size(); ++i) {
        if (!objects_[i].active) continue;
        
        for (size_t j = i + 1; j < objects_.size(); ++j) {
            if (!objects_[j].active) continue;
            
            float dx = objects_[i].x - objects_[j].x;
            float dy = objects_[i].y - objects_[j].y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < (objects_[i].radius + objects_[j].radius)) {
                // Handle collision
                handleObjectCollision(objects_[i], objects_[j]);
            }
        }
        
        // Check agent-object collision with physics
        float dx = agent_.x - objects_[i].x;
        float dy = agent_.y - objects_[i].y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < (objects_[i].radius + 0.3f)) {
            // Physics-based collision response
            handleAgentObjectCollision(objects_[i]);
        }
    }
}

void SimpleWorld::handleObjectCollision(WorldObject& obj1, WorldObject& obj2) {
    // Physics-based collision response
    float dx = obj1.x - obj2.x;
    float dy = obj1.y - obj2.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < 0.0001f) return;  // Avoid division by zero
    
    float overlap = (obj1.radius + obj2.radius) - dist;
    float pushX = (dx / dist) * overlap * 0.5f;
    float pushY = (dy / dist) * overlap * 0.5f;
    
    // Apply push forces
    obj1.x -= pushX;
    obj1.y -= pushY;
    obj2.x += pushX;
    obj2.y += pushY;
    
    // If objects are movable, adjust velocities
    if (enableDynamicObjects_) {
        // Simple elastic collision
        float mass1 = obj1.radius;
        float mass2 = obj2.radius;
        
        float totalMass = mass1 + mass2;
        if (totalMass > 0.0f) {
            float impulse = 2.0f * overlap / totalMass;
            obj1.x -= pushX * mass2 / totalMass;
            obj1.y -= pushY * mass2 / totalMass;
            obj2.x += pushX * mass1 / totalMass;
            obj2.y += pushY * mass1 / totalMass;
        }
    }
}

void SimpleWorld::handleAgentObjectCollision(const WorldObject& obj) {
    // Physics-based agent-object collision
    float dx = agent_.x - obj.x;
    float dy = agent_.y - obj.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < 0.0001f) return;
    
    float overlap = (obj.radius + 0.3f) - dist;
    
    if (obj.type == WorldObjectType::Resource) {
        // Push agent away
        float pushX = (dx / dist) * overlap;
        float pushY = (dy / dist) * overlap;
        
        agent_.x += pushX;
        agent_.y += pushY;
        
        // Apply impulse to agent
        float impulse = overlap * 0.5f;
        agent_.velocityX += (pushX / dist) * impulse;
        agent_.velocityY += (pushY / dist) * impulse;
        
        agent_.isMoving = true;
    } else if (obj.type == WorldObjectType::Hazard) {
        // Hazard causes damage and bounces agent
        float pushX = (dx / dist) * overlap * 0.5f;
        float pushY = (dy / dist) * overlap * 0.5f;
        
        agent_.x += pushX;
        agent_.y += pushY;
        
        agent_.velocityX += (pushX / dist) * obj.value * 0.1f;
        agent_.velocityY += (pushY / dist) * obj.value * 0.1f;
        
        agent_.health -= std::abs(obj.value) * 0.01f;
        agent_.isMoving = true;
    }
}

float SimpleWorld::getEnvironmentalComplexity() const {
    // Calculate current environmental complexity
    float complexity = environmentComplexity_;
    
    // Add complexity based on object density
    if (width_ > 0 && height_ > 0) {
        float objectDensity = objects_.size() / (width_ * height_);
        complexity += objectDensity * 10.0f;
    }
    
    // Add complexity based on feature diversity
    std::map<WorldObjectType, int> typeCount;
    for (const auto& obj : objects_) {
        if (obj.active) {
            typeCount[obj.type]++;
        }
    }
    
    float featureDiversity = 0.0f;
    if (!typeCount.empty()) {
        float maxCount = 0.0f;
        for (const auto& pair : typeCount) {
            maxCount = std::max(maxCount, static_cast<float>(pair.second));
        }
        featureDiversity = 1.0f - (maxCount / objects_.size());
    }
    
    complexity += featureDiversity * 5.0f;
    
    return std::clamp(complexity, 0.0f, 100.0f);
}

void SimpleWorld::updateEnvironmentalParameters(float timeScale) {
    // Dynamically update environment parameters over time
    objectDensity_ += (legacyRng_() % 200 - 100) / 10000.0f * timeScale;
    objectDensity_ = std::clamp(objectDensity_, 0.5f, 3.0f);
    
    terrainRoughness_ += (legacyRng_() % 200 - 100) / 10000.0f * timeScale;
    terrainRoughness_ = std::clamp(terrainRoughness_, 0.1f, 2.0f);
    
    if (enableWindSimulation_) {
        windStrength_ += (legacyRng_() % 200 - 100) / 5000.0f * timeScale;
        windStrength_ = std::clamp(windStrength_, 0.0f, 5.0f);
        
        windVariation_ += (legacyRng_() % 200 - 100) / 10000.0f * timeScale;
        windVariation_ = std::clamp(windVariation_, 0.0f, 1.0f);
    }
    
    if (enableTemperatureEffects_) {
        temperature_ += (legacyRng_() % 400 - 200) / 10000.0f * timeScale;
        temperature_ = std::clamp(temperature_, 0.0f, 40.0f);
    }
}

float SimpleWorld::getMissionProgress(float targetReward) const {
    // Calculate mission progress based on current state
    float progress = 0.0f;
    
    // Based on agent energy
    progress += (agent_.energy / maxEnergy_) * 0.3f;
    
    // Based on agent health
    progress += agent_.health * 0.2f;
    
    // Based on object collection (if we tracked this)
    float collectedValue = 0.0f;
    for (const auto& obj : objects_) {
        if (!obj.active && obj.type == WorldObjectType::Resource) {
            collectedValue += obj.value;
        }
    }
    progress += std::min(collectedValue / 100.0f, 1.0f) * 0.5f;
    
    return std::min(progress, 1.0f);
}

// Update the update method to call the new physics system
void SimpleWorld::update(double timestep) {
    // Call parent update for basic functionality
    SimpleWorld::update(timestep);
    
    // Update advanced physics
    updatePhysics(timestep);
    
    // Update environmental parameters
    updateEnvironmentalParameters(timestep);
    
    // Generate vision after physics update
    generateVision();
    
    // Check for collisions
    checkCollisions();
}

} // namespace nlm