#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// World object - represents an object in the world
// Position and type only - no semantic labels
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

// World object - represents an object in the world
// Position and type only - no semantic labels
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

// Simple spatial index for world object management
struct SpatialIndex {
    struct Cell {
        std::vector<WorldObject*> objects;
        Cell() = default;
    };
    
    size_t gridWidth;
    size_t gridHeight;
    float cellSize;
    std::vector<std::vector<Cell>> grid;
    
    SpatialIndex(float worldWidth, float worldHeight, float cellSize)
        : gridWidth(static_cast<size_t>(std::ceil(worldWidth / cellSize)))
        , gridHeight(static_cast<size_t>(std::ceil(worldHeight / cellSize)))
        , cellSize(cellSize)
        , grid(gridWidth, std::vector<Cell>(gridHeight)) {}
    
    size_t getCellX(float x) const {
        return static_cast<size_t>(std::floor(x / cellSize));
    }
    
    size_t getCellY(float y) const {
        return static_cast<size_t>(std::floor(y / cellSize));
    }
    
    void addObject(WorldObject* obj) {
        if (!obj || !obj->active) return;
        
        size_t xCell = getCellX(obj->x);
        size_t yCell = getCellY(obj->y);
        
        if (xCell < gridWidth && yCell < gridHeight) {
            grid[xCell][yCell].objects.push_back(obj);
        }
    }
    
    void removeObject(WorldObject* obj) {
        if (!obj || !obj->active) return;
        
        size_t xCell = getCellX(obj->x);
        size_t yCell = getCellY(obj->y);
        
        if (xCell < gridWidth && yCell < gridHeight) {
            auto& cell = grid[xCell][yCell];
            for (auto it = cell.objects.begin(); it != cell.objects.end(); ++it) {
                if (*it == obj) {
                    cell.objects.erase(it);
                    return;
                }
            }
        }
    }
    
    std::vector<WorldObject*> getObjectsInRange(float minX, float minY, float maxX, float maxY) {
        std::vector<WorldObject*> result;
        
        size_t minXCell = getCellX(minX);
        size_t maxXCell = getCellX(maxX);
        size_t minYCell = getCellY(minY);
        size_t maxYCell = getCellY(maxY);
        
        for (size_t x = minXCell; x <= maxXCell && x < gridWidth; ++x) {
            for (size_t y = minYCell; y <= maxYCell && y < gridHeight; ++y) {
                for (WorldObject* obj : grid[x][y].objects) {
                    if (obj && obj->active) {
                        result.push_back(obj);
                    }
                }
            }
        }
        
        return result;
    }
    
    void update() {
        // Rebuild spatial index - simple optimization for small grids
        grid.clear();
        grid.resize(gridWidth, std::vector<Cell>(gridHeight));
        
        for (auto& obj : objects_) {
            if (obj.active) {
                addObject(&obj);
            }
        }
    }
};

// Simple 2D world for NLM development
// Provides sensory experience without semantic labels
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
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
    float getResourceEnergyGain() const { return resourceEnergyGain_; }
    
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
    
    // Spatial indexing for improved performance
    void rebuildSpatialIndex() {
        if (spatialIndex_) {
            spatialIndex_->update();
        }
    }
    
    // World geometry
    float width_;
    float height_;
    
    // Agent
    AgentBody agent_;
    
    // Objects in world
    std::vector<WorldObject> objects_;
    
    // Spatial index for faster object queries
    std::unique_ptr<SpatialIndex> spatialIndex_;
    
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
};

} // namespace nlm
