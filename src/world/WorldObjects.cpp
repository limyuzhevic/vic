#include "WorldObjects.hpp"

namespace nlm {

void PlantBehavior::update(float dt, SimpleWorld& world) {
    age_ += dt;
    
    // Check environmental conditions
    bool hasWater = false;
    bool hasLight = false;
    
    // Check nearby water objects
    for (const auto& obj : world.getObjects()) {
        if (obj.active && obj.type == WorldObjectType::Water) {
            float dx = obj.x - x;  // Need access to parent's x/y
            float dy = obj.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < obj.radius + 1.0f) {
                hasWater = true;
                break;
            }
        }
    }
    
    // Simple light check - TODO: Implement proper vision system
    hasLight = true; // Placeholder
    
    if (hasWater && hasLight) {
        // Grow if conditions are met
        if (age_ > 10.0f) {  // Mature plants grow slower
            state_ = ObjectState::Growing;
            value_ += growthRate_ * dt;
        }
    } else {
        // Stress or die
        value_ -= decayRate_ * dt;
        if (value_ <= 0.0f) {
            state_ = ObjectState::Dying;
        }
    }
}

bool PlantBehavior::interact(AgentBody& agent, float interactionDistance) {
    return false; // Plants don't actively interact with agents
}

void AnimalBehavior::update(float dt, SimpleWorld& world) {
    energyLevel_ -= metabolismRate_ * dt;
    
    // Random movement
    if (energyLevel_ > 0) {
        // Simple movement - TODO: Implement proper AI
        float moveX = (nextRandom() % 100 - 50) / 50.0f;
        float moveY = (nextRandom() % 100 - 50) / 50.0f;
        // Update position - TODO: Implement proper position system
    }
    
    // Check for food (resources) nearby
    for (const auto& obj : world.getObjects()) {
        if (obj.active && obj.type == WorldObjectType::Resource) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < obj.radius + interactionDistance) {
                energyLevel_ += 10.0f * (obj.value / 10.0f); // Gain energy from food
                break;
            }
        }
    }
    
    // Reproduction
    if (energyLevel_ > 80.0f && reproductionRate_ > (nextRandom() % 1000) / 1000.0f) {
        // Create offspring - TODO: Implement
        energyLevel_ = 40.0f; // Cost of reproduction
    }
    
    if (energyLevel_ <= 0.0f) {
        state_ = ObjectState::Dying;
    }
}

bool AnimalBehavior::interact(AgentBody& agent, float interactionDistance) {
    float dx = agent.x - x;
    float dy = agent.y - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < interactionDistance) {
        if (isHerbivore_ && hasResourceNearby(world)) {
            // Eat resource
            return true;
        } else if (isPredator_ && agent.energy > 50) {
            // Attack agent - TODO: Implement combat
            return true;
        }
    }
    return false;
}

void RockBehavior::update(float dt, SimpleWorld& world) {
    // Rocks are static, just check for breaking
    if (breakageRate_ > 0 && (nextRandom() % 1000) / 1000.0f < breakageRate_ * dt) {
        if (isBreakable_ && !isDestroyed_) {
            // Break into smaller rocks - TODO: Implement
            value_ *= 0.5f;
            if (value_ < 1.0f) {
                state_ = ObjectState::Destroyed;
            }
        }
    }
}

bool RockBehavior::interact(AgentBody& agent, float interactionDistance) {
    float dx = agent.x - x;
    float dy = agent.y - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < interactionDistance) {
        // Can be picked up if breakable
        if (isBreakable_ && value_ > 5.0f) {
            return true; // Can be collected
        }
    }
    return false;
}

void WaterBehavior::update(float dt, SimpleWorld& world) {
    // Temperature changes based on environment
    // Simplified thermal dynamics
    float ambientTemp = 20.0f; // TODO: Get from environment
    
    if (temperature_ < ambientTemp) {
        temperature_ += 0.1f * dt; // Heat up
    } else if (temperature_ > ambientTemp) {
        temperature_ -= 0.1f * dt; // Cool down
    }
    
    // Phase changes
    if (temperature_ <= freezingPoint_ && state_ == ObjectState::Active) {
        state_ = ObjectState::Transforming; // Will become ice
    } else if (temperature_ >= boilingPoint_) {
        // Evaporate
        state_ = ObjectState::Dying;
    } else if (evaporationRate_ > 0 && (nextRandom() % 1000) / 1000.0f < evaporationRate_ * dt) {
        // Small chance to evaporate
        if (temperature_ > 30.0f) {
            value_ -= 0.1f * dt;
            if (value_ <= 0.0f) {
                state_ = ObjectState::Dying;
            }
        }
    }
}

bool WaterBehavior::interact(AgentBody& agent, float interactionDistance) {
    float dx = agent.x - x;
    float dy = agent.y - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < interactionDistance) {
        // Can be used for hydration
        return true;
    }
    return false;
}

void FireBehavior::update(float dt, SimpleWorld& world) {
    // Consume fuel
    fuelConsumption_ += 0.1f * dt;
    
    // Spread to nearby flammable objects
    spreadRate_ = 0.05f; // Base spread rate
    
    for (auto& obj : world.getObjects()) {
        if (!obj.active || obj.type == WorldObjectType::Fire) continue;
        
        float dx = obj.x - x;
        float dy = obj.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < obj.radius + 2.0f) { // Fire spread range
            // Check if object is flammable
            bool isFlammable = checkFlammability(obj.type);
            if (isFlammable && (nextRandom() % 1000) / 1000.0f < spreadRate_ * dt) {
                // Ignite the object
                obj.type = WorldObjectType::Fire;
                obj.value = 2.0f;
            }
        }
    }
    
    // Decrease heat over time
    heat_ *= 0.95f;
    
    if (heat_ < 1.0f || fuelConsumption_ >= 1.0f) {
        state_ = ObjectState::Dying;
    }
}

bool FireBehavior::interact(AgentBody& agent, float interactionDistance) {
    float dx = agent.x - x;
    float dy = agent.y - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < interactionDistance) {
        // Deal damage to agent
        return true;
    }
    return false;
}

void GeneratorBehavior::update(float dt, SimpleWorld& world) {
    // Generate energy
    energyGenerated_ += energyOutput_ * dt;
    
    // Check fuel status
    if (fuelLevel_ <= 0) {
        state_ = ObjectState::Inactive;
        energyGenerated_ = 0.0f;
        return;
    }
    
    // Consume fuel
    fuelLevel_ -= 1.0f * dt; // 1 unit per second
}

bool GeneratorBehavior::interact(AgentBody& agent, float interactionDistance) {
    float dx = agent.x - x;
    float dy = agent.y - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < interactionDistance) {
        // Can transfer energy to agent
        return true;
    }
    return false;
}

void StructureBehavior::update(float dt, SimpleWorld& world) {
    // Structures are static but can change state
    if (!canPass_ && (nextRandom() % 1000) / 1000.0f < 0.001f) {
        // Randomly toggle passability
        canPass_ = !canPass_;
    }
}

bool StructureBehavior::interact(AgentBody& agent, float interactionDistance) {
    float dx = agent.x - x;
    float dy = agent.y - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < interactionDistance && isDoor_) {
        // Door can be opened/closed
        canPass_ = !canPass_;
        return true;
    }
    return false;
}

void WeatherBehavior::update(float dt, SimpleWorld& world) {
    // Update weather effects over time
    switch (weatherType_) {
        case WeatherType::Clear:
            // Stable weather
            break;
        case WeatherType::Rain:
            precipitationRate_ = 0.3f;
            windSpeed_ = 5.0f + (nextRandom() % 100) / 20.0f;
            break;
        case WeatherType::Storm:
            precipitationRate_ = 1.0f;
            windSpeed_ = 15.0f + (nextRandom() % 100) / 10.0f;
            break;
        default:
            break;
    }
}

bool WeatherBehavior::interact(AgentBody& agent, float interactionDistance) {
    // Weather affects all agents in the world
    return true;
}

void TimeBehavior::update(float dt, SimpleWorld& world) {
    age_ += dt;
    
    // Decay over time
    value_ -= decayRate_ * dt;
    
    if (age_ >= lifespan_) {
        state_ = ObjectState::Dying;
    }
}

bool TimeBehavior::interact(AgentBody& agent, float interactionDistance) {
    return false; // Time objects don't interact directly
}

// Static function implementations
std::unique_ptr<WorldObjectBehavior> WorldObjectFactory::createObjectBehavior(WorldObjectType type) {
    switch (type) {
        case WorldObjectType::Plant:
            return std::make_unique<PlantBehavior>();
        case WorldObjectType::Animal:
            return std::make_unique<AnimalBehavior>();
        case WorldObjectType::Rock:
            return std::make_unique<RockBehavior>();
        case WorldObjectType::Water:
            return std::make_unique<WaterBehavior>();
        case WorldObjectType::Fire:
            return std::make_unique<FireBehavior>();
        case WorldObjectType::EnergySource:
            return std::make_unique<GeneratorBehavior>();
        case WorldObjectType::Structure:
            return std::make_unique<StructureBehavior>();
        case WorldObjectType::Weather:
            return std::make_unique<WeatherBehavior>();
        case WorldObjectType::Clock:
            return std::make_unique<TimeBehavior>();
        default:
            return nullptr;
    }
}

WorldObject WorldObjectFactory::createWorldObject(WorldObjectType type, float x, float y, float radius, float value) {
    WorldObject obj(x, y, type, value, radius);
    return obj;
}

std::string WorldObjectFactory::getTypeName(WorldObjectType type) {
    switch (type) {
        case WorldObjectType::Empty: return "Empty";
        case WorldObjectType::Resource: return "Resource";
        case WorldObjectType::Hazard: return "Hazard";
        case WorldObjectType::Wall: return "Wall";
        case WorldObjectType::Marker: return "Marker";
        case WorldObjectType::Plant: return "Plant";
        case WorldObjectType::Animal: return "Animal";
        case WorldObjectType::Rock: return "Rock";
        case WorldObjectType::Water: return "Water";
        case WorldObjectType::Fire: return "Fire";
        case WorldObjectType::EnergySource: return "EnergySource";
        case WorldObjectType::Structure: return "Structure";
        case WorldObjectType::Weather: return "Weather";
        case WorldObjectType::Clock: return "Clock";
        default: return "Unknown";
    }
}

} // namespace nlm