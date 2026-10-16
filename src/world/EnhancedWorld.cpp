// Implementation of enhanced world simulation

#include "SimpleWorld.cpp"
#include "WorldObjects.cpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Helper function to check distance between two objects
static float distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

// Helper function to check if a point is within a circle
static bool pointInCircle(float px, float py, float cx, float cy, float radius) {
    return distance(px, py, cx, cy) <= radius;
}

// Helper function to normalize angle
static float normalizeAngle(float angle) {
    while (angle > M_PI) angle -= 2.0f * M_PI;
    while (angle < -M_PI) angle += 2.0f * M_PI;
    return angle;
}

void SimpleWorld::update(double timestep) {
    // Call parent update for basic functionality
    SimpleWorld::update(timestep);
    
    // Update enhanced features if configured
    if (config_.enableWeather) {
        updateEnvironment(static_cast<float>(timestep));
    }
    
    if (config_.enableEcosystem) {
        updateObjectBehaviors(static_cast<float>(timestep));
        updateAgentState(static_cast<float>(timestep));
    }
    
    if (config_.enableTimeProgression) {
        updateTimeProgression(static_cast<float>(timestep));
    }
    
    // Update physics
    updatePhysics(static_cast<float>(timestep));
}

void SimpleWorld::updateEnvironment(float dt) {
    // Simple environmental update - day/night cycle
    dayNightCycle_ += config_.timeScale * dt * 0.001f;
    
    // Update weather
    if (environment_.isStorm) {
        environment_.precipitation += 0.1f * dt;
        environment_.windSpeed += 0.05f * dt;
    }
    
    // Seasonal changes
    season_ += 0.01f * dt;
    if (season_ > 1.0f) season_ = -1.0f;
    
    // Temperature changes based on time of day
    float tempVariation = std::sin(dayNightCycle_ * 2.0f) * 10.0f;
    environment_.temperature = 20.0f + tempVariation + (season_ * 5.0f);
    
    // Update environmental effects on objects
    for (auto& obj : objects_) {
        if (obj.behavior && obj.active) {
            // Environment affects object behavior
            if (obj.type == WorldObjectType::Water) {
                // Water evaporates with heat
                if (environment_.temperature > 30.0f) {
                    obj.value -= 0.1f * dt;
                    if (obj.value <= 0.0f) {
                        obj.active = false;
                    }
                }
            }
        }
    }
}

void SimpleWorld::updateAgentState(float dt) {
    // Update agent's memory and skills based on interactions
    // This is a simplified version - full implementation would track all experiences
    
    // Curiosity drives exploration
    advancedAgent_.curiosity = std::min(1.0f, advancedAgent_.curiosity + 0.01f * dt);
    
    // Motivation based on goals
    if (!advancedAgent_.goals.empty()) {
        float goalMotivation = 0.0f;
        for (const auto& goal : advancedAgent_.goals) {
            goalMotivation += goal.second;
        }
        advancedAgent_.motivation = goalMotivation / static_cast<float>(advancedAgent_.goals.size());
    }
    
    // Emotional state influenced by recent experiences
    advancedAgent_.emotionalState = std::min(1.0f, advancedAgent_.emotionalState + 0.005f * dt);
}

void SimpleWorld::updateObjectBehaviors(float dt) {
    // Update behaviors of all active objects
    for (auto& obj : objects_) {
        if (obj.active && obj.behavior) {
            obj.behavior->update(dt, *this);
            
            // Check object state changes
            if (obj.state == ObjectState::Dying && obj.value <= 0.0f) {
                obj.active = false;
            }
        }
    }
}

void SimpleWorld::spawnObject(WorldObjectType type, float x, float y, float radius) {
    // Create enhanced object with behavior
    WorldObjectEnhanced obj;
    obj.x = x;
    obj.y = y;
    obj.radius = radius;
    obj.type = type;
    obj.active = true;
    obj.state = ObjectState::Active;
    
    // Set initial value based on type
    switch (type) {
        case WorldObjectType::Resource:
            obj.value = 10.0f + nextRandom() % 50;
            break;
        case WorldObjectType::Hazard:
            obj.value = -15.0f;
            break;
        case WorldObjectType::Water:
            obj.value = 5.0f;
            break;
        case WorldObjectType::Plant:
            obj.value = 5.0f;
            break;
        case WorldObjectType::Animal:
            obj.value = 20.0f;
            break;
        case WorldObjectType::Rock:
            obj.value = 15.0f;
            break;
        case WorldObjectType::Fire:
            obj.value = 2.0f;
            break;
        case WorldObjectType::EnergySource:
            obj.value = 50.0f;
            break;
        default:
            obj.value = 0.0f;
    }
    
    // Create behavior based on object type
    obj.behavior = WorldObjectFactory::createObjectBehavior(type);
    
    objects_.push_back(obj);
}

void SimpleWorld::spawnEnvironmentalEffect(WorldObjectType type, float x, float y) {
    // Spawn environmental effects (weather, etc.)
    spawnObject(type, x, y, 2.0f);
}

void SimpleWorld::updateMultimodalSenses() {
    // Update enhanced sensory inputs
    
    // Generate audio signals based on environment
    generateAudioSignals();
    
    // Generate chemical signals
    generateChemicalSignals();
    
    // Generate thermal signals
    generateThermalSignals();
}

void SimpleWorld::generateAudioSignals() {
    // Simplified audio generation - in a full implementation, this would
    // include complex acoustic simulations
    
    // Agent movement sound
    if (agent_.isMoving) {
        // Add to audio signal vector
    }
    
    // Object collisions
    for (const auto& obj : objects_) {
        if (obj.active && obj.type == WorldObjectType::Hazard) {
            // Hazard makes sound
        }
    }
    
    // Weather sounds
    if (environment_.weather == WeatherType::Rain) {
        // Rain sound
    } else if (environment_.weather == WeatherType::Storm) {
        // Storm sound
    }
}

void SimpleWorld::generateChemicalSignals() {
    // Generate chemical smell/taste signals
    
    std::vector<float> chemical(8, 0.0f); // 8 chemical sensors
    
    // Check nearby chemical sources
    for (const auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = distance(dx, dy, 0, 0);
        
        if (dist < obj.radius + 3.0f) {
            // Different chemicals based on object type
            if (obj.type == WorldObjectType::Water) {
                chemical[0] += 1.0f / (dist + 0.1f); // Water
            } else if (obj.type == WorldObjectType::Hazard) {
                chemical[1] += std::abs(obj.value) / (dist + 0.1f); // Poison
            } else if (obj.type == WorldObjectType::Fire) {
                chemical[2] += obj.value / (dist + 0.1f); // Smoke
            }
        }
    }
    
    // Set chemical signals in sensory percept
    // TODO: Add chemical signal support to SensoryPercept
}

void SimpleWorld::generateThermalSignals() {
    // Generate thermal sensing
    
    std::vector<float> thermal(4, 0.0f); // 4 thermal sensors
    
    // Check nearby heat sources
    for (const auto& obj : objects_) {
        if (!obj.active) continue;
        
        float dx = obj.x - agent_.x;
        float dy = obj.y - agent_.y;
        float dist = distance(dx, dy, 0, 0);
        
        if (dist < obj.radius + 5.0f) {
            // Different temperatures based on object type
            float temp = 0.0f;
            if (obj.type == WorldObjectType::Fire) {
                temp = obj.value; // Heat from fire
            } else if (obj.type == WorldObjectType::Water) {
                temp = 20.0f; // Room temperature water
            }
            
            float intensity = temp / (dist + 0.1f);
            if (intensity > thermal[0]) {
                thermal[0] = intensity;
            }
        }
    }
    
    // Set thermal signals in sensory percept
    // TODO: Add thermal signal support to SensoryPercept
}

void SimpleWorld::updatePhysics(float dt) {
    // Update rigid body dynamics
    updateRigidBodyDynamics(dt);
    
    // Update fluid dynamics
    updateFluidDynamics(dt);
    
    // Update particle systems
    updateParticleSystems(dt);
}

void SimpleWorld::updateFluidDynamics(float dt) {
    // Simplified fluid dynamics for water objects
    for (auto& obj : objects_) {
        if (!obj.active || obj.type != WorldObjectType::Water) continue;
        
        // Water flow and turbulence
        // In a full implementation, this would use Navier-Stokes equations
        // or simplified particle-based fluid simulation
        
        // Simple diffusion
        if (environment_.temperature > 30.0f) {
            obj.value -= 0.1f * dt; // Evaporation
        }
        
        // Random movement (diffusion)
        float moveX = (nextRandom() % 100 - 50) / 50.0f * 0.1f;
        float moveY = (nextRandom() % 100 - 50) / 50.0f * 0.1f;
        obj.x += moveX * dt;
        obj.y += moveY * dt;
        
        // Keep water in bounds
        if (obj.x < 0.0f) obj.x = 0.0f;
        if (obj.x >= width_) obj.x = width_ - 0.01f;
        if (obj.y < 0.0f) obj.y = 0.0f;
        if (obj.y >= height_) obj.y = height_ - 0.01f;
    }
}

void SimpleWorld::updateRigidBodyDynamics(float dt) {
    // Update rigid body dynamics for dynamic objects
    for (auto& obj : objects_) {
        if (!obj.active || !obj.behavior) continue;
        
        // Simple physics simulation
        if (obj.type == WorldObjectType::Animal || 
            obj.type == WorldObjectType::Rock ||
            obj.type == WorldObjectType::Structure) {
            
            // Apply gravity
            float gravityForce = physics_.gravity * obj.behavior->getPhysical().mass;
            
            // Simple collision detection
            bool collision = false;
            for (const auto& other : objects_) {
                if (!other.active || &other == &obj) continue;
                
                float dist = distance(obj.x, obj.y, other.x, other.y);
                if (dist < obj.radius + other.radius) {
                    collision = true;
                    
                    // Simple collision response
                    if (collision) {
                        // Resolve collision (simplified)
                        float overlap = (obj.radius + other.radius) - dist;
                        float pushX = (other.x - obj.x) / dist * overlap * 0.5f;
                        float pushY = (other.y - obj.y) / dist * overlap * 0.5f;
                        
                        obj.x -= pushX;
                        obj.y -= pushY;
                    }
                    break;
                }
            }
        }
    }
}

void SimpleWorld::updateParticleSystems(float dt) {
    // Update particle systems (smoke, dust, debris)
    // This would include particle emission, movement, and decay
    
    // For now, just maintain particle list
    particleSystems_.erase(
        std::remove_if(particleSystems_.begin(), particleSystems_.end(),
            [](const WorldObjectEnhanced& obj) { return !obj.active; }),
        particleSystems_.end()
    );
}

void SimpleWorld::updateTimeProgression(float dt) {
    // Update day/night cycle
    updateDayNightCycle(dt);
    
    // Update seasons
    updateSeasons(dt);
    
    // Age objects
    simTime_ += dt;
    worldTime_ += dt * config_.timeScale;
    
    // Age-dependent changes
    for (auto& obj : objects_) {
        if (obj.active && obj.behavior) {
            // Age affects behavior
            if (obj.type == WorldObjectType::Plant && obj.age_ > 10.0f) {
                // Mature plants have different properties
            }
        }
    }
}

void SimpleWorld::updateDayNightCycle(float dt) {
    // Update day/night cycle
    dayNightCycle_ += config_.timeScale * dt * 0.001f;
    
    // Adjust lighting based on time of day
    // In a full implementation, this would affect vision and thermal properties
}

void SimpleWorld::updateSeasons(float dt) {
    // Update seasonal changes
    season_ += 0.01f * dt;
    if (season_ > 1.0f) season_ = -1.0f;
    
    // Update environment based on season
    switch (weather_) {
        case WeatherType::Clear:
        case WeatherType::Cloudy:
            environment_.temperature += season_ * 2.0f * dt;
            break;
        case WeatherType::Rain:
            environment_.humidity = 0.7f + season_ * 0.3f;
            break;
        case WeatherType::Snow:
            environment_.temperature -= 5.0f + season_ * 5.0f;
            break;
        case WeatherType::Storm:
            environment_.windSpeed += std::abs(season_) * 2.0f * dt;
            break;
        default:
            break;
    }
}

InteractionResult SimpleWorld::craft(const std::vector<WorldObjectEnhanced>& materials) {
    InteractionResult result;
    result.action = "craft";
    
    // Simple crafting recipes
    if (materials.size() >= 2) {
        // Check if materials can be crafted
        bool canCraft = false;
        for (const auto& material : materials) {
            if (material.type == WorldObjectType::Rock || material.type == WorldObjectType::Resource) {
                canCraft = true;
            }
        }
        
        if (canCraft) {
            result.success = true;
            result.reward = 10.0f;
            // Create crafted object
            WorldObjectEnhanced crafted;
            crafted.type = WorldObjectType::Structure;
            crafted.value = 25.0f;
            crafted.active = true;
            result.producedObjects.push_back(crafted);
        }
    }
    
    return result;
}

InteractionResult SimpleWorld::construct(const WorldObjectEnhanced& blueprint, float x, float y) {
    InteractionResult result;
    result.action = "construct";
    
    // Check if we have materials for construction
    std::vector<WorldObjectEnhanced> materials;
    for (const auto& obj : objects_) {
        if (obj.active && obj.type == WorldObjectType::Resource) {
            materials.push_back(obj);
        }
    }
    
    if (materials.size() >= 3) { // Need 3 resources for simple structure
        result.success = true;
        result.reward = 20.0f;
        
        // Create structure
        WorldObjectEnhanced structure;
        structure.type = WorldObjectType::Structure;
        structure.x = x;
        structure.y = y;
        structure.value = blueprint.value + 10.0f;
        structure.active = true;
        structure.isDoor_ = (blueprint.type == WorldObjectType::Structure && blueprint.value > 30.0f);
        
        result.producedObjects.push_back(structure);
        
        // Remove resources used for construction
        for (int i = 0; i < 3 && !materials.empty(); ++i) {
            result.consumedObjects.push_back(materials[i]);
        }
    }
    
    return result;
}

InteractionResult SimpleWorld::deconstruct(WorldObjectEnhanced& obj) {
    InteractionResult result;
    result.action = "deconstruct";
    
    if (obj.active && obj.type == WorldObjectType::Structure) {
        result.success = true;
        result.reward = obj.value * 0.5f;
        
        // Break structure into components
        std::vector<WorldObjectEnhanced> components;
        
        // Create components based on structure type
        if (obj.isDoor_) {
            // Door breaks into wood and hardware
            WorldObjectEnhanced wood;
            wood.type = WorldObjectType::Resource;
            wood.value = obj.value * 0.7f;
            wood.active = true;
            components.push_back(wood);
            
            WorldObjectEnhanced hardware;
            hardware.type = WorldObjectType::Rock;
            hardware.value = obj.value * 0.3f;
            hardware.active = true;
            components.push_back(hardware);
        }
        
        result.producedObjects = components;
        obj.active = false;
    }
    
    return result;
}

InteractionResult SimpleWorld::transferEnergy(WorldObjectEnhanced& from, WorldObjectEnhanced& to, float amount) {
    InteractionResult result;
    result.action = "transfer_energy";
    
    if (from.active && to.active && from.type == WorldObjectType::EnergySource) {
        float availableEnergy = from.value;
        float energyTransfer = std::min(amount, availableEnergy);
        
        if (energyTransfer > 0) {
            result.success = true;
            result.reward = energyTransfer;
            
            // Transfer energy
            from.value -= energyTransfer;
            to.value += energyTransfer;
            
            result.consumedObjects.push_back(from);
            result.producedObjects.push_back(to);
        }
    }
    
    return result;
}

float SimpleWorld::getTotalResourceValue() const {
    float total = 0.0f;
    for (const auto& obj : objects_) {
        if (obj.active) {
            total += std::max(0.0f, obj.value);
        }
    }
    return total;
}

int SimpleWorld::getPopulationCount(WorldObjectType type) const {
    int count = 0;
    for (const auto& obj : objects_) {
        if (obj.active && obj.type == type) {
            count++;
        }
    }
    return count;
}

float SimpleWorld::getAverageHealth() const {
    // Calculate average health of all agents
    if (objects_.empty()) return 1.0f;
    
    float totalHealth = 0.0f;
    int count = 0;
    
    for (const auto& obj : objects_) {
        if (obj.active && obj.behavior) {
            // Use agent's health for agents
            if (obj.type == WorldObjectType::Animal || obj.type == WorldObjectType::Agent) {
                totalHealth += agent_.health;
                count++;
            }
        }
    }
    
    return count > 0 ? totalHealth / count : 1.0f;
}

float SimpleWorld::get biodiversityIndex() const {
    // Calculate biodiversity (Shannon diversity index)
    std::unordered_map<WorldObjectType, int> counts;
    
    for (const auto& obj : objects_) {
        if (obj.active) {
            counts[obj.type]++;
        }
    }
    
    float total = static_cast<float>(objects_.size());
    float diversity = 0.0f;
    
    for (const auto& pair : counts) {
        float proportion = pair.second / total;
        if (proportion > 0.0f) {
            diversity -= proportion * std::log(proportion);
        }
    }
    
    return diversity / std::log(static_cast<float>(counts.size()));
}

void SimpleWorld::setGoal(const std::string& goal, float priority) {
    advancedAgent_.goals[goal] = priority;
}

void SimpleWorld::learnBehavior(const std::string& behavior, float successRate) {
    advancedAgent_.learnedBehaviors.push_back(behavior);
    // Store success rate for future use
    // In a full implementation, this would update behavior probabilities
}

// Implementation of specific object behaviors

// Plant behavior implementation
void PlantBehavior::update(float dt, SimpleWorld& world) {
    age_ += dt;
    
    // Check for water nearby
    bool hasWater = false;
    for (const auto& obj : world.getObjects()) {
        if (obj.active && obj.type == WorldObjectType::Water) {
            float dx = obj.x - x;  // Access to parent x,y - TODO: Fix
            float dy = obj.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < obj.radius + 1.0f) {
                hasWater = true;
                break;
            }
        }
    }
    
    // Check for light (simplified)
    bool hasLight = true;
    
    if (hasWater && hasLight) {
        // Grow if conditions are good
        if (age_ > 10.0f) {
            state_ = ObjectState::Growing;
            value_ += growthRate_ * dt;
        }
    } else {
        // Stress
        value_ -= decayRate_ * dt;
        if (value_ <= 0.0f) {
            state_ = ObjectState::Dying;
        }
    }
}

bool PlantBehavior::interact(AgentBody& agent, float interactionDistance) {
    return false; // Plants don't actively interact
}

// Animal behavior implementation (simplified)
void AnimalBehavior::update(float dt, SimpleWorld& world) {
    energyLevel_ -= metabolismRate_ * dt;
    
    // Simple random movement
    if (energyLevel_ > 0) {
        // Move randomly
        float moveX = (nextRandom() % 100 - 50) / 50.0f * 0.1f;
        float moveY = (nextRandom() % 100 - 50) / 50.0f * 0.1f;
        // Update position - TODO: Implement proper position system
    }
    
    // Look for food (resources)
    for (const auto& obj : world.getObjects()) {
        if (obj.active && obj.type == WorldObjectType::Resource) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < obj.radius + 1.0f) {
                energyLevel_ += 10.0f * (obj.value / 10.0f);
                break;
            }
        }
    }
    
    // Reproduction
    if (energyLevel_ > 80.0f && reproductionRate_ > (nextRandom() % 1000) / 1000.0f) {
        // Create offspring - TODO: Implement
        energyLevel_ = 40.0f;
    }
    
    if (energyLevel_ <= 0.0f) {
        state_ = ObjectState::Dying;
    }
}

bool AnimalBehavior::interact(AgentBody& agent, float interactionDistance) {
    return false;
}

// Other behavior implementations would follow similar patterns...

} // namespace nlm

} // namespace nlm

// Global function to access RNG state (for behaviors that need it)
uint64_t SimpleWorld::nextRandom() {
    return lcg(rngState_);
}

// Include the header for WorldObjects
#include "WorldObjects.hpp"