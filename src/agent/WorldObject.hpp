#pragma once

// WorldObject.hpp - World object representation
// This header defines the WorldObject class for representing objects in the simulation world

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

/**
 * @class WorldObject
 * @brief Representation of objects in the simulation world
 * 
 * The WorldObject class represents physical objects within the simulation world,
 * such as resources, hazards, walls, markers, and empty spaces. These objects
 * are part of the environment interface and provide information to the agent
 * about the state of the world.
 * 
 * World objects are rendered and presented to the brain only for informational
 * purposes - the brain doesn't directly receive object information, only
 * processed sensory data from the environment.
 */
class WorldObject {
public:
    WorldObject();
    ~WorldObject();
    
    // Disable copying, enable moving
    WorldObject(const WorldObject&) = delete;
    WorldObject& operator=(const WorldObject&) = delete;
    WorldObject(WorldObject&&) noexcept;
    WorldObject& operator=(WorldObject&&) noexcept;
    
    // Position in world coordinates
    float getX() const;
    void setX(float x);
    
    float getY() const;
    void setY(float y);
    
    // Object type
    WorldObjectType getType() const;
    void setType(WorldObjectType type);
    
    // Value (e.g., reward value for resources, hazard level for hazards)
    float getValue() const;
    void setValue(float value);
    
    // Physical dimensions
    float getRadius() const;
    void setRadius(float radius);
    
    // Object state
    bool isActive() const;
    void setActive(bool active);
    
    // Object name/identifier (for debugging)
    std::string getName() const;
    void setName(const std::string& name);
    
    // Check if object is at specific position
    bool atPosition(float x, float y) const;
    
    // Get distance to another object
    float distanceTo(const WorldObject& other) const;
    
    // Check if object overlaps with another
    bool overlapsWith(const WorldObject& other) const;
    
    // Get object summary for logging/debugging
    std::string getSummary() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm