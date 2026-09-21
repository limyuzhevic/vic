#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// World object - represents an object in the world
// Position and type only - no semantic labels
// 
// This structure represents a physical object in the simulated world.
// It contains only geometric and type information - no semantic labels
// or meaning. This is crucial for the NLM architecture because the brain
// receives no semantic information about objects - only their positions,
// types, and properties. This forces the brain to learn through interaction
// and discovery.
// 
// World objects include:
// - Resources: Objects that provide positive reward when collected
// - Hazards: Objects that cause damage when collided with
// - Walls: Boundaries that block movement
// - Markers: Visual aids for navigation
// - Empty: Placeholder for no object
// 
// World objects are part of the environment, not the brain. They exist
// independently of the agent's perception and provide the basis for
// interaction and learning.
// 
// Each world object has:
// - Position (x, y): Where the object is located
// - Radius: Size for collision detection
// - Type: Object category (Resource, Hazard, etc.)
// - Value: Reward value (positive for resources, negative for hazards)
// - Active: Whether the object exists or has been collected/destroyed
// 
// Example usage:
//   // Create a resource at position (5.0, 3.0) with radius 0.5
//   WorldObject resource(5.0f, 3.0f, WorldObjectType::Resource, 10.0f);
//   
//   // Create a wall (boundary)
//   WorldObject wall(0.0f, 0.0f, WorldObjectType::Wall, 0.0f, 2.0f);
//   
//   // Add to world
//   world.addObject(resource);
//   world.addObject(wall);

struct WorldObject {
    float x;
    float y;
    float radius;
    WorldObjectType type;
    float value;        // Reward value (for resources) or damage (for hazards)
    bool active;
    
    WorldObject() 
        : x(0.0f), y(0.0f), radius(0.5f), type(WorldObjectType::Empty)
        , value(0.0f), active(true) {}
    
    WorldObject(float px, float py, WorldObjectType t, float v = 0.0f, float r = 0.5f)
        : x(px), y(py), radius(r), type(t), value(v), active(true) {}
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
// 
// The SimpleWorld is the simulated environment where the agent lives and
// learns. It provides low-level sensory data and physical simulation
// without giving the agent semantic understanding of objects or actions.
// 
// Key characteristics:
// - 2D geometry with continuous coordinates
// - Physics simulation (movement, collisions)
// - Energy system (depletion over time)
// - Sensory perception (vision, touch, internal)
// - Object interaction (collision, consumption, etc.)
// - No semantic labels - everything is position+type only
// 
// The world is fundamental to the NLM architecture because it provides
// the only source of sensory information (through SimpleWorld) and is
// the environment where actions have consequences. The agent must learn
// through trial and error in this environment.
// 
// Typical usage:
//   // Create and configure world
//   auto world = std::make_unique<SimpleWorld>();
//   world->configure(100.0f, 100.0f, 16, 16);  // 100x100m, 16x16 vision
//   
//   // Place objects
//   world->addObject(WorldObject(50.0f, 50.0f, WorldObjectType::Resource, 10.0f));
//   world->addObject(WorldObject(80.0f, 20.0f, WorldObjectType::Hazard, -5.0f));
//   
//   // Set agent start
//   world->setAgentStart(10.0f, 10.0f);
//   
//   // Simulation loop
//   for (int step = 0; step < 1000; ++step) {
//       // Agent perceives world
//       SensoryPercept percept = world->getSensoryPercept();
//       
//       // Agent acts
//       Action action = brain->produceAction();
//       ActionResult result = world->applyMotorCommand(action, step);
//       
//       // Update world
//       world->update(step);
//   }
// 
// The SimpleWorld is designed to be minimal and focused, providing
// only what's necessary for the NLM to function. Complex physics,
// graphics, and semantics are intentionally excluded.

// Configuration
     void configure(float width, float height, size_t visionWidth, size_t visionHeight);
     
     // Reset world to initial state
     void reset();
     
     // Set agent starting position
     void setAgentStart(float x, float y);
     
     // Update world state (call each simulation step)
     void update(double timestep);
     
     // Apply motor command to agent
     ActionResult applyMotorCommand(MotorCommand cmd, double currentTime);
     
     // Get current sensory percept for the agent
     const SensoryPercept& getSensoryPercept() const { return sensory_; }
     
     // Get agent body state
     const AgentBody& getAgentBody() const { return agent_; }
     
     // Add object to world
     void addObject(const WorldObject& obj);
     
     // Remove object at position
     void removeObject(float x, float y);
     
     // Check if position is valid (not wall)
     bool isValidPosition(float x, float y) const;
     
     // Get object at position (if any)
     WorldObject* getObjectAt(float x, float y);
     
     // World dimensions
     float getWidth() const { return width_; }
     float getHeight() const { return height_; }
     
     // Energy settings
     float getMaxEnergy() const { return maxEnergy_; }
     void setMaxEnergy(float e) { maxEnergy_ = e; }
     float getEnergyDecayRate() const { return energyDecayRate_; }
     void setEnergyDecayRate(float r) { energyDecayRate_ = r; }
     
     // Time
     double getSimulationTime() const { return simTime_; }
     
     // Seed for reproducibility
     void setRandomSeed(uint64_t seed);
     uint64_t getRandomSeed() const { return rngSeed_; }
     
private:
    // Generate vision from current view
    void generateVision();
    
    // Check for collisions
    void checkCollisions();
    
    // Interact with nearby objects
    ActionResult interact();
    
    // World geometry
    float width_;
    float height_;
    
    // Agent
    AgentBody agent_;
    
    // Objects in world
    std::vector<WorldObject> objects_;
    
    // Sensory state
    SensoryPercept sensory_;
    
    // Vision configuration
    size_t visionWidth_;
    size_t visionHeight_;
    
    // Energy system
    float maxEnergy_;
    float energyDecayRate_;
    float resourceEnergyGain_;
    
    // Simulation time
    double simTime_;
    
    // Random seed
    uint64_t rngSeed_;
    
    // Simple RNG state
    uint64_t rngState_;
    uint64_t nextRandom();

} // namespace nlm
