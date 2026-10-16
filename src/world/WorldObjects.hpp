#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cmath>
#include <random>
#include "../agent/AgentBody.hpp"
#include "../agent/AgentBrain.hpp"

namespace nlm {

// Object state enumeration
enum class ObjectState {
    Active,
    Inactive,
    Destroyed,
    Growing,
    Dying,
    Transforming
};

// Material properties for objects
struct MaterialProperties {
    float density = 1.0f;           // Mass per volume
    float conductivity = 0.0f;      // Thermal conductivity
    float hardness = 0.0f;          // Resistance to damage
    float flammability = 0.0f;      // Susceptibility to fire
    float elasticity = 0.0f;        // Springiness
    float viscosity = 0.0f;         // Fluid resistance
    float reflectivity = 0.0f;       // Light reflection
    std::string name;               // Material identifier
};

// Physical properties for dynamic objects
struct PhysicalProperties {
    float mass = 1.0f;
    float volume = 1.0f;
    float inertia = 0.0f;
    float friction = 0.0f;
    float restitution = 0.0f;
    float damping = 0.0f;
    float buoyancy = 0.0f;
};

// Environmental requirements
struct EnvironmentalRequirements {
    float minTemperature = 0.0f;
    float maxTemperature = 100.0f;
    float minHumidity = 0.0f;
    float maxHumidity = 1.0f;
    float minPressure = 0.0f;
    float maxPressure = 1.0f;
    std::vector<WorldObjectType> compatibleObjects;
};

// Base behavior class for world objects
class WorldObjectBehavior {
public:
    virtual ~WorldObjectBehavior() = default;
    
    // Update object state
    virtual void update(float dt, SimpleWorld& world) = 0;
    
    // Handle agent interaction
    virtual bool interact(AgentBody& agent, float interactionDistance) = 0;
    
    // Get object properties
    virtual const MaterialProperties& getMaterial() const { return material_; }
    virtual const PhysicalProperties& getPhysical() const { return physical_; }
    virtual const EnvironmentalRequirements& getRequirements() const { return envRequirements_; }
    
    // Check if object can be destroyed
    virtual bool canBeDestroyed() const { return true; }
    
    // Get object value
    virtual float getValue() const { return value_; }
    
    // Get object state
    virtual ObjectState getState() const { return state_; }
    
protected:
    MaterialProperties material_;
    PhysicalProperties physical_;
    EnvironmentalRequirements envRequirements_;
    float value_ = 0.0f;
    ObjectState state_ = ObjectState::Active;
    
    void setMaterial(const MaterialProperties& material) { material_ = material; }
    void setPhysical(const PhysicalProperties& physical) { physical_ = physical; }
    void setRequirements(const EnvironmentalRequirements& requirements) { envRequirements_ = requirements; }
    void setValue(float value) { value_ = value; }
    void setState(ObjectState state) { state_ = state; }
};

// Plant behavior - growing, photosynthesizing, consuming resources
class PlantBehavior : public WorldObjectBehavior {
public:
    PlantBehavior() {
        MaterialProperties mat;
        mat.density = 0.5f;
        mat.conductivity = 0.1f;
        mat.hardness = 0.3f;
        mat.flammability = 0.1f;
        mat.charge = 0.0f;
        mat.name = "Plant"; 
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 0.2f;
        phys.volume = 0.1f;
        phys.inertia = 0.05f;
        phys.friction = 0.4f;
        phys.restitution = 0.3f;
        phys.damping = 0.1f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 5.0f; // Initial value
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    float getGrowthRate() const { return growthRate_; }
    float getWaterNeeds() const { return waterNeeds_; }
    float getLightNeeds() const { return lightNeeds_; }
    
private:
    float growthRate_ = 0.05f;
    float waterNeeds_ = 0.3f;
    float lightNeeds_ = 0.4f;
    float age_ = 0.0f;
};

// Animal behavior - moving, eating, reproducing, evolving
class AnimalBehavior : public WorldObjectBehavior {
public:
    AnimalBehavior() {
        MaterialProperties mat;
        mat.density = 1.0f;
        mat.conductivity = 0.2f;
        mat.hardness = 0.6f;
        mat.flammability = 0.2f;
        mat.charge = 0.0f;
        mat.name = "Animal";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 5.0f;
        phys.volume = 2.0f;
        phys.inertia = 2.0f;
        phys.friction = 0.6f;
        phys.restitution = 0.2f;
        phys.damping = 0.3f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 20.0f;
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    bool isPredator() const { return isPredator_; }
    bool isHerbivore() const { return isHerbivore_; }
    float getEnergyLevel() const { return energyLevel_; }
    
private:
    bool isPredator_ = false;
    bool isHerbivore_ = false;
    float energyLevel_ = 50.0f;
    float metabolismRate_ = 0.1f;
    float reproductionRate_ = 0.02f;
};

// Rock behavior - solid, breakable, mineral-rich
class RockBehavior : public WorldObjectBehavior {
public:
    RockBehavior() {
        MaterialProperties mat;
        mat.density = 2.5f;
        mat.conductivity = 0.1f;
        mat.hardness = 0.9f;
        mat.flammability = 0.01f;
        mat.charge = 0.0f;
        mat.name = "Rock";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 10.0f;
        phys.volume = 3.0f;
        phys.inertia = 5.0f;
        phys.friction = 0.8f;
        phys.restitution = 0.1f;
        phys.damping = 0.05f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 15.0f;
        mineralContent_ = 0.3f;
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    float getMineralContent() const { return mineralContent_; }
    bool isBreakable() const { return isBreakable_; }
    
private:
    float mineralContent_ = 0.0f;
    bool isBreakable_ = true;
    float hardness_ = 0.9f;
};

// Water behavior - flowing, evaporating, freezing, melting
class WaterBehavior : public WorldObjectBehavior {
public:
    WaterBehavior() {
        MaterialProperties mat;
        mat.density = 1.0f;
        mat.conductivity = 0.6f;
        mat.hardness = 0.0f;
        mat.flammability = 0.0f;
        mat.viscosity = 0.001f;
        mat.charge = 0.0f;
        mat.name = "Water";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 1.0f;
        phys.volume = 1.0f;
        phys.inertia = 0.1f;
        phys.friction = 0.3f;
        phys.restitution = 0.0f;
        phys.damping = 0.02f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.95f;
        setPhysical(phys);
        
        value_ = 1.0f;
        temperature_ = 20.0f; // Celsius
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    float getTemperature() const { return temperature_; }
    bool isLiquid() const { return state_ == ObjectState::Active; }
    
private:
    float temperature_ = 20.0f;
    float evaporationRate_ = 0.01f;
    float freezingPoint_ = 0.0f;
    float boilingPoint_ = 100.0f;
};

// Fire behavior - spreading, burning, consuming objects
class FireBehavior : public WorldObjectBehavior {
public:
    FireBehavior() {
        MaterialProperties mat;
        mat.density = 0.1f;
        mat.conductivity = 0.8f;
        mat.hardness = 0.0f;
        mat.flammability = 1.0f;
        mat.charge = 0.0f;
        mat.name = "Fire";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 0.5f;
        phys.volume = 0.5f;
        phys.inertia = 0.05f;
        phys.friction = 0.1f;
        phys.restitution = 0.0f;
        phys.damping = 0.05f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 2.0f;
        heat_ = 100.0f;
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    float getHeat() const { return heat_; }
    float getSpreadRate() const { return spreadRate_; }
    
private:
    float heat_ = 100.0f;
    float spreadRate_ = 0.05f;
    float oxygenNeeded_ = 0.1f;
    float fuelConsumption_ = 0.02f;
};

// Generator behavior - energy production, resource management
class GeneratorBehavior : public WorldObjectBehavior {
public:
    GeneratorBehavior() {
        MaterialProperties mat;
        mat.density = 1.5f;
        mat.conductivity = 0.3f;
        mat.hardness = 0.7f;
        mat.flammability = 0.1f;
        mat.charge = 0.0f;
        mat.name = "Generator";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 8.0f;
        phys.volume = 4.0f;
        phys.inertia = 3.0f;
        phys.friction = 0.5f;
        phys.restitution = 0.2f;
        phys.damping = 0.1f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 50.0f;
        energyOutput_ = 10.0f;
        fuelCapacity_ = 100.0f;
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    float getEnergyOutput() const { return energyOutput_; }
    float getFuelLevel() const { return fuelLevel_; }
    
private:
    float energyOutput_ = 10.0f;
    float fuelLevel_ = 50.0f;
    float fuelCapacity_ = 100.0f;
    float efficiency_ = 0.8f;
};

// Structure behavior - buildings, doors, passages
class StructureBehavior : public WorldObjectBehavior {
public:
    StructureBehavior() {
        MaterialProperties mat;
        mat.density = 3.0f;
        mat.conductivity = 0.1f;
        mat.hardness = 0.95f;
        mat.flammability = 0.05f;
        mat.charge = 0.0f;
        mat.name = "Structure";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 20.0f;
        phys.volume = 8.0f;
        phys.inertia = 10.0f;
        phys.friction = 0.7f;
        phys.restitution = 0.1f;
        phys.damping = 0.05f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 100.0f;
        isDoor_ = false;
        passageWidth_ = 1.0f;
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    bool isDoor() const { return isDoor_; }
    float getPassageWidth() const { return passageWidth_; }
    
private:
    bool isDoor_ = false;
    float passageWidth_ = 1.0f;
    bool canPass_ = true;
};

// Weather behavior - environmental effects
class WeatherBehavior : public WorldObjectBehavior {
public:
    WeatherBehavior() {
        MaterialProperties mat;
        mat.density = 0.0f;
        mat.conductivity = 0.5f;
        mat.hardness = 0.0f;
        mat.flammability = 0.0f;
        mat.charge = 0.0f;
        mat.name = "Weather";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 0.1f;
        phys.volume = 1.0f;
        phys.inertia = 0.01f;
        phys.friction = 0.0f;
        phys.restitution = 0.0f;
        phys.damping = 0.0f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 0.0f;
        weatherType_ = WeatherType::Clear;
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    WeatherType getWeatherType() const { return weatherType_; }
    float getWindSpeed() const { return windSpeed_; }
    float getPrecipitationRate() const { return precipitationRate_; }
    
private:
    WeatherType weatherType_ = WeatherType::Clear;
    float windSpeed_ = 0.0f;
    float precipitationRate_ = 0.0f;
    float temperature_ = 20.0f;
    float humidity_ = 0.5f;
};

// Time-based behavior - dynamic, aging, decaying
class TimeBehavior : public WorldObjectBehavior {
public:
    TimeBehavior() {
        MaterialProperties mat;
        mat.density = 1.0f;
        mat.conductivity = 0.1f;
        mat.hardness = 0.5f;
        mat.flammability = 0.0f;
        mat.charge = 0.0f;
        mat.name = "TimeObject";
        setMaterial(mat);
        
        PhysicalProperties phys;
        phys.mass = 1.0f;
        phys.volume = 1.0f;
        phys.inertia = 0.1f;
        phys.friction = 0.3f;
        phys.restitution = 0.2f;
        phys.damping = 0.05f;
        phys.charge = 0.0f;
        phys.buoyancy = 0.0f;
        setPhysical(phys);
        
        value_ = 0.0f;
        lifespan_ = 100.0f;
        decayRate_ = 0.01f;
    }
    
    void update(float dt, SimpleWorld& world) override;
    bool interact(AgentBody& agent, float interactionDistance) override;
    
    float getAge() const { return age_; }
    float getLifespan() const { return lifespan_; }
    bool isExpired() const { return age_ >= lifespan_; }
    
private:
    float age_ = 0.0f;
    float lifespan_ = 100.0f;
    float decayRate_ = 0.01f;
};

// Weather type enumeration
enum class WeatherType {
    Clear,
    Cloudy,
    Rain,
    Snow,
    Storm,
    Fog,
    Wind
};

// Object factory for creating different types of objects
class WorldObjectFactory {
public:
    static std::unique_ptr<WorldObjectBehavior> createObjectBehavior(WorldObjectType type);
    static WorldObject createWorldObject(WorldObjectType type, float x, float y, float radius, float value);
    static std::string getTypeName(WorldObjectType type);
};

} // namespace nlm