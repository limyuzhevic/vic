#pragma once

// world/WorldCore.hpp
// Core world utilities and shared functionality
// This header provides the foundation for the modular world architecture

#include "SimpleWorld.hpp"
#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>

namespace nlm {

// Forward declarations
class SimpleWorld;
class AgentBody;
class SensoryPercept;

// World core state management
class WorldCore {
public:
    // Core world operations
    static bool validateWorldConfiguration(const std::shared_ptr<const Config>& config);
    static void initializeWorld(SimpleWorld& world, const std::vector<WorldObject>& initialObjects);
    static void updateWorld(SimpleWorld& world, double timestep);
    static void resetWorld(SimpleWorld& world);
    static void configureAgentStart(SimpleWorld& world, float x, float y);
    
    // Object management utilities
    static void addObjectToWorld(SimpleWorld& world, const WorldObject& obj);
    static void removeObjectFromWorld(SimpleWorld& world, float x, float y);
    static WorldObject* getObjectAtPosition(SimpleWorld& world, float x, float y);
    static bool isPositionValid(SimpleWorld& world, float x, float y);
    static void generateWorldObjects(SimpleWorld& world, size_t objectCount);
    static void configureWorldGeometry(SimpleWorld& world, float width, float height);
    
    // Sensory utilities
    static void updateSensoryPercept(SimpleWorld& world);
    static void generateVision(SimpleWorld& world);
    static void processVisionData(SimpleWorld& world, const std::vector<float>& visionData);
    static void processTouchData(SimpleWorld& world, const std::vector<float>& touchData);
    static void processInternalData(SimpleWorld& world, const std::vector<float>& internalData);
    static void processProprioceptionData(SimpleWorld& world, const std::vector<float>& proprioceptionData);
    static void computeWorldNovelty(SimpleWorld& world);
    static void updateWorldCuriosity(SimpleWorld& world, float noveltyLevel);
    
    // Motor and interaction utilities
    static ActionResult applyAgentMotorCommand(SimpleWorld& world, MotorCommand cmd, double currentTime);
    static ActionResult checkAgentCollisions(SimpleWorld& world);
    static ActionResult handleAgentInteraction(SimpleWorld& world);
    static bool isAgentAlive(SimpleWorld& world);
    static void updateAgentEnergy(SimpleWorld& world, double timestep);
    
    // Statistics utilities
    static std::string formatWorldStatistics(const SimpleWorld& world);
    static float calculateVisionQuality(SimpleWorld& world);
    static size_t getObjectCount(SimpleWorld& world);
    static float getWorldComplexity(SimpleWorld& world);
    static double getSimulationTime(SimpleWorld& world);
    
    // Configuration utilities
    static bool validateWorldConfig(const std::shared_ptr<const Config>& config);
    static void applyDefaultWorldSettings(std::shared_ptr<Config> config);
    
    // Reward and energy utilities
    static void applyWorldRewards(SimpleWorld& world, float reward);
    static void updateAgentEnergy(SimpleWorld& world, float energyChange);
    static bool isEnergyCritical(SimpleWorld& world);
    static void handleResourceCollection(SimpleWorld& world);
    static void handleHazardEncounter(SimpleWorld& world, float damage);
    
    // Time and reproduction utilities
    static void setWorldRandomSeed(SimpleWorld& world, uint64_t seed);
    static uint64_t getWorldRandomSeed(SimpleWorld& world);
    static double getWorldSimulationTime(SimpleWorld& world);
    static void advanceWorldTime(SimpleWorld& world, double timestep);
    static void resetWorldTime(SimpleWorld& world);
};

// World utilities
namespace WorldUtils {
    // Object utilities
    std::string worldObjectTypeToString(WorldObjectType type);
    WorldObjectType stringToWorldObjectType(const std::string& str);
    bool isWorldObjectTypeValid(WorldObjectType type);
    
    // Vision utilities
    std::vector<float> normalizeVisionData(const std::vector<float>& visionData, 
                                          size_t width, size_t height);
    float calculateVisionEntropy(const std::vector<float>& visionData);
    float calculateVisionComplexity(const std::vector<float>& visionData);
    
    // Object interaction utilities
    float calculateDistance(float x1, float y1, float x2, float y2);
    bool checkCollision(const WorldObject& obj1, const WorldObject& obj2);
    ActionResult determineInteractionResult(WorldObjectType agentType, 
                                          WorldObjectType objectType);
    
    // Statistics utilities
    float calculateWorldCrowdingFactor(const std::vector<WorldObject>& objects);
    float calculateWorldResourceDensity(const std::vector<WorldObject>& objects);
    float calculateWorldHazardDensity(const std::vector<WorldObject>& objects);
    
    // Energy utilities
    float calculateEnergyFromObject(WorldObjectType type, float value);
    float calculateEnergyCostForMovement(float distance);
    float calculateEnergyCostForInteraction(WorldObjectType objectType);
    bool isEnergyCritical(float energy, float maxEnergy);
    
    // Time utilities
    double getCurrentWorldTime();
    std::chrono::steady_clock::time_point getWorldTimePoint();
    double timeSinceWorldStart(const std::chrono::steady_clock::time_point& start);
    bool isWorldTimeToBeReproduced(uint64_t seed, double time);
}

} // namespace nlm
