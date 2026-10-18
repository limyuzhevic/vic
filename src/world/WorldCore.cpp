#include "world/WorldCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <chrono>
#include <random>

namespace nlm {

bool WorldCore::validateWorldConfiguration(const std::shared_ptr<const Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Configuration is null in WorldCore validation");
        return false;
    }
    
    // Validate world-specific configuration
    bool valid = true;
    
    auto width = config->getOr<float>("world_width", 100.0f);
    if (width <= 0.0f || width > 10000.0f) {
        NLM_LOG_ERROR("Invalid world_width: " + std::to_string(width));
        valid = false;
    }
    
    auto height = config->getOr<float>("world_height", 100.0f);
    if (height <= 0.0f || height > 10000.0f) {
        NLM_LOG_ERROR("Invalid world_height: " + std::to_string(height));
        valid = false;
    }
    
    auto visionWidth = config->getOr<size_t>("vision_width", 16);
    if (visionWidth == 0 || visionWidth > 256) {
        NLM_LOG_ERROR("Invalid vision_width: " + std::to_string(visionWidth));
        valid = false;
    }
    
    auto visionHeight = config->getOr<size_t>("vision_height", 16);
    if (visionHeight == 0 || visionHeight > 256) {
        NLM_LOG_ERROR("Invalid vision_height: " + std::to_string(visionHeight));
        valid = false;
    }
    
    auto maxEnergy = config->getOr<float>("max_energy", 100.0f);
    if (maxEnergy <= 0.0f || maxEnergy > 1000.0f) {
        NLM_LOG_ERROR("Invalid max_energy: " + std::to_string(maxEnergy));
        valid = false;
    }
    
    return valid;
}

void WorldCore::initializeWorld(SimpleWorld& world, const std::vector<WorldObject>& initialObjects) {
    world.reset();
    
    for (const auto& obj : initialObjects) {
        world.addObject(obj);
    }
    
    NLM_LOG_INFO("World initialized with " + std::to_string(initialObjects.size()) + " objects");
}

void WorldCore::updateWorld(SimpleWorld& world, double timestep) {
    // Update world state
    world.update(timestep);
    
    // Update sensory perception
    world.getSensoryPercept();  // This will regenerate vision
    
    NLM_LOG_INFO("World updated: time = " + std::to_string(world.getSimulationTime()));
}

void WorldCore::resetWorld(SimpleWorld& world) {
    world.reset();
    NLM_LOG_INFO("World reset");
}

void WorldCore::configureAgentStart(SimpleWorld& world, float x, float y) {
    world.setAgentStart(x, y);
}

void WorldCore::addObjectToWorld(SimpleWorld& world, const WorldObject& obj) {
    world.addObject(obj);
}

void WorldCore::removeObjectFromWorld(SimpleWorld& world, float x, float y) {
    world.removeObject(x, y);
}

WorldObject* WorldCore::getObjectAtPosition(SimpleWorld& world, float x, float y) {
    return world.getObjectAt(x, y);
}

bool WorldCore::isPositionValid(SimpleWorld& world, float x, float y) {
    return world.isValidPosition(x, y);
}

void WorldCore::generateWorldObjects(SimpleWorld& world, size_t objectCount) {
    // Generate random objects in the world
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> widthDist(0.0f, world.getWidth());
    std::uniform_real_distribution<float> heightDist(0.0f, world.getHeight());
    
    for (size_t i = 0; i < objectCount; ++i) {
        WorldObjectType type;
        float value = 0.0f;
        
        // Determine object type
        int typeChoice = std::uniform_int_distribution<int>(0, 3)(gen);
        switch (typeChoice) {
            case 0: type = WorldObjectType::Resource; value = 10.0f + std::uniform_real_distribution<float>(0.0f, 40.0f)(gen); break;
            case 1: type = WorldObjectType::Hazard; value = -5.0f - std::uniform_real_distribution<float>(0.0f, 10.0f)(gen); break;
            case 2: type = WorldObjectType::Empty; break;
            default: type = WorldObjectType::Empty; break;
        }
        
        float radius = 0.5f + std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
        float objX = widthDist(gen);
        float objY = heightDist(gen);
        
        // Ensure object is within valid bounds
        if (objX >= 0.0f && objX < world.getWidth() && 
            objY >= 0.0f && objY < world.getHeight()) {
            
            WorldObject obj(objX, objY, type, value, radius);
            world.addObject(obj);
        }
    }
}

void WorldCore::configureWorldGeometry(SimpleWorld& world, float width, float height) {
    // This would require modifying world dimensions, which might not be directly supported
    // For now, log the requested dimensions
    NLM_LOG_INFO("World geometry configured: " + std::to_string(width) + " x " + std::to_string(height));
}

void WorldCore::updateSensoryPercept(SimpleWorld& world) {
    // Update sensory percept by regenerating vision
    world.getSensoryPercept();
}

void WorldCore::generateVision(SimpleWorld& world) {
    // Generate vision from current view
    world.generateVision();
}

void WorldCore::processVisionData(SimpleWorld& world, const std::vector<float>& visionData) {
    // Process vision data (simplified - just update agent's vision data)
    // In reality, this would involve accessing agent's sensory neurons
    
    // For demonstration, just log the vision data
    if (!visionData.empty()) {
        NLM_LOG_INFO("Vision data processed: " + std::to_string(visionData.size()) + " values");
    }
}

void WorldCore::processTouchData(SimpleWorld& world, const std::vector<float>& touchData) {
    // Process touch data
    if (!touchData.empty()) {
        NLM_LOG_INFO("Touch data processed: " + std::to_string(touchData.size()) + " values");
    }
}

void WorldCore::processInternalData(SimpleWorld& world, const std::vector<float>& internalData) {
    // Process internal data
    if (!internalData.empty()) {
        NLM_LOG_INFO("Internal data processed: " + std::to_string(internalData.size()) + " values");
    }
}

void WorldCore::processProprioceptionData(SimpleWorld& world, const std::vector<float>& proprioceptionData) {
    // Process proprioception data
    if (!proprioceptionData.empty()) {
        NLM_LOG_INFO("Proprioception data processed: " + std::to_string(proprioceptionData.size()) + " values");
    }
}

void WorldCore::computeWorldNovelty(SimpleWorld& world) {
    // Compute novelty based on current sensory data
    auto& sensory = world.getSensoryPercept();
    auto vision = sensory.getVision();
    
    if (!vision.empty()) {
        float novelty = calculateVisionEntropy(vision);
        world.getAgentBody().setNoveltyLevel(novelty);
        NLM_LOG_INFO("World novelty computed: " + std::to_string(novelty));
    }
}

void WorldCore::updateWorldCuriosity(SimpleWorld& world, float noveltyLevel) {
    // Update world curiosity based on novelty
    if (world.getAgentBody().isCuriosityEnabled()) {
        float curiosityLevel = noveltyLevel * 2.0f;
        curiosityLevel = std::clamp(curiosityLevel, 0.0f, 1.0f);
        world.getAgentBody().setCuriosityLevel(curiosityLevel);
        NLM_LOG_INFO("World curiosity updated: " + std::to_string(curiosityLevel));
    }
}

ActionResult WorldCore::applyAgentMotorCommand(SimpleWorld& world, MotorCommand cmd, double currentTime) {
    return world.applyMotorCommand(cmd, currentTime);
}

ActionResult WorldCore::checkAgentCollisions(SimpleWorld& world) {
    return world.applyMotorCommand(MotorCommand::Wait, 0.0);  // Placeholder
}

ActionResult WorldCore::handleAgentInteraction(SimpleWorld& world) {
    return world.interact();
}

bool WorldCore::isAgentAlive(SimpleWorld& world) {
    return world.getAgentBody().getEnergy() > 0.0f;
}

void WorldCore::updateAgentEnergy(SimpleWorld& world, double timestep) {
    // Update agent energy over time
    float energyChange = -world.getEnergyDecayRate() * timestep;
    world.getAgentBody().setEnergy(world.getAgentBody().getEnergy() + energyChange);
    
    // Ensure energy doesn't go below zero
    if (world.getAgentBody().getEnergy() < 0.0f) {
        world.getAgentBody().setEnergy(0.0f);
    }
}

std::string WorldCore::formatWorldStatistics(const SimpleWorld& world) {
    std::ostringstream oss;
    oss << "=== World Statistics ===\n";
    oss << "Width: " << world.getWidth() << "\n";
    oss << "Height: " << world.getHeight() << "\n";
    oss << "Objects: " << world.getObjectCount() << "\n";
    oss << "Agent energy: " << world.getAgentBody().getEnergy() << "/" << world.getMaxEnergy() << "\n";
    oss << "Simulation time: " << world.getSimulationTime() << "\n";
    oss << "Vision: " << world.getVisionWidth() << " x " << world.getVisionHeight() << "\n";
    oss << "Random seed: " << world.getRandomSeed() << "\n";
    
    return oss.str();
}

float WorldCore::calculateVisionQuality(SimpleWorld& world) {
    // Calculate vision quality based on sensory data
    auto& sensory = world.getSensoryPercept();
    auto vision = sensory.getVision();
    
    if (vision.empty()) return 0.0f;
    
    float maxVal = *std::max_element(vision.begin(), vision.end());
    float minVal = *std::min_element(vision.begin(), vision.end());
    return (maxVal - minVal) / std::max(1.0f, maxVal + 1.0f);
}

size_t WorldCore::getObjectCount(SimpleWorld& world) {
    // This would require modifying SimpleWorld to expose object count
    // For now, return a placeholder
    return 0;
}

float WorldCore::getWorldComplexity(SimpleWorld& world) {
    // Calculate world complexity based on object distribution
    // This is a simplified calculation
    float complexity = 0.0f;
    
    auto& objects = world.getObjects();
    if (objects.size() > 1) {
        complexity = std::min(1.0f, static_cast<float>(objects.size()) / 100.0f);
    }
    
    return complexity;
}

double WorldCore::getSimulationTime(SimpleWorld& world) {
    return world.getSimulationTime();
}

bool WorldCore::validateWorldConfig(const std::shared_ptr<const Config>& config) {
    if (!config) {
        return false;
    }
    
    // Validate world-specific configuration
    auto width = config->getOr<float>("world_width", 100.0f);
    auto height = config->getOr<float>("world_height", 100.0f);
    auto visionWidth = config->getOr<size_t>("vision_width", 16);
    auto visionHeight = config->getOr<size_t>("vision_height", 16);
    auto maxEnergy = config->getOr<float>("max_energy", 100.0f);
    
    if (width <= 0.0f || height <= 0.0f || visionWidth == 0 || visionHeight == 0 || maxEnergy <= 0.0f) {
        NLM_LOG_ERROR("Invalid world configuration parameters");
        return false;
    }
    
    return true;
}

void WorldCore::applyDefaultWorldSettings(std::shared_ptr<Config> config) {
    if (!config) {
        return;
    }
    
    // Apply default world settings
    config->set("world_width", 100.0f);
    config->set("world_height", 100.0f);
    config->set("vision_width", 16);
    config->set("vision_height", 16);
    config->set("max_energy", 100.0f);
    config->set("energy_decay_rate", 1.0f);
    config->set("resource_energy_gain", 10.0f);
    config->set("hazard_damage", 5.0f);
}

void WorldCore::applyWorldRewards(SimpleWorld& world, float reward) {
    // Apply rewards to the world (e.g., collect resources)
    float energyChange = reward;
    world.getAgentBody().setEnergy(world.getAgentBody().getEnergy() + energyChange);
    
    // Cap energy at maximum
    if (world.getAgentBody().getEnergy() > world.getMaxEnergy()) {
        world.getAgentBody().setEnergy(world.getMaxEnergy());
    }
    
    NLM_LOG_INFO("World reward applied: " + std::to_string(reward) + " energy");
}

void WorldCore::updateAgentEnergy(SimpleWorld& world, float energyChange) {
    // Update agent energy
    world.getAgentBody().setEnergy(world.getAgentBody().getEnergy() + energyChange);
    
    // Ensure energy doesn't go below zero
    if (world.getAgentBody().getEnergy() < 0.0f) {
        world.getAgentBody().setEnergy(0.0f);
    }
}

bool WorldCore::isEnergyCritical(SimpleWorld& world) {
    float energyRatio = world.getAgentBody().getEnergy() / world.getMaxEnergy();
    return energyRatio < 0.1f;  // Critical when energy is less than 10%
}

void WorldCore::handleResourceCollection(SimpleWorld& world) {
    // Handle resource collection when agent is near resources
    auto& objects = world.getObjects();
    auto& agent = world.getAgentBody();
    
    for (auto& obj : objects) {
        if (obj.active && obj.type == WorldObjectType::Resource) {
            float distance = calculateDistance(agent.getX(), agent.getY(), obj.x, obj.y);
            if (distance < (agent.getRadius() + obj.radius)) {
                // Collect resource
                float energyGain = std::max(0.0f, obj.value);
                agent.setEnergy(agent.getEnergy() + energyGain);
                
                // Remove resource
                obj.active = false;
                
                NLM_LOG_INFO("Resource collected: " + std::to_string(energyGain) + " energy");
                break;  // Only collect first resource found
            }
        }
    }
}

void WorldCore::handleHazardEncounter(SimpleWorld& world, float damage) {
    // Handle hazard encounter when agent is near hazards
    auto& objects = world.getObjects();
    auto& agent = world.getAgentBody();
    
    for (auto& obj : objects) {
        if (obj.active && obj.type == WorldObjectType::Hazard) {
            float distance = calculateDistance(agent.getX(), agent.getY(), obj.x, obj.y);
            if (distance < (agent.getRadius() + obj.radius)) {
                // Apply damage
                agent.setEnergy(agent.getEnergy() + damage);
                
                NLM_LOG_INFO("Hazard encountered: " + std::to_string(damage) + " damage");
                break;  // Only process first hazard found
            }
        }
    }
}

void WorldCore::setWorldRandomSeed(SimpleWorld& world, uint64_t seed) {
    world.setRandomSeed(seed);
}

uint64_t WorldCore::getWorldRandomSeed(SimpleWorld& world) {
    return world.getRandomSeed();
}

double WorldCore::getWorldSimulationTime(SimpleWorld& world) {
    return world.getSimulationTime();
}

void WorldCore::advanceWorldTime(SimpleWorld& world, double timestep) {
    // Advance world time by the specified timestep
    double currentTime = world.getSimulationTime();
    world.setSimulationTime(currentTime + timestep);
}

void WorldCore::resetWorldTime(SimpleWorld& world) {
    world.setSimulationTime(0.0);
}

// WorldUtils namespace functions
std::string WorldUtils::worldObjectTypeToString(WorldObjectType type) {
    switch (type) {
        case WorldObjectType::Empty: return "Empty";
        case WorldObjectType::Resource: return "Resource";
        case WorldObjectType::Hazard: return "Hazard";
        case WorldObjectType::Agent: return "Agent";
        case WorldObjectType::Wall: return "Wall";
        default: return "Unknown";
    }
}

WorldObjectType WorldUtils::stringToWorldObjectType(const std::string& str) {
    if (str == "Empty") return WorldObjectType::Empty;
    if (str == "Resource") return WorldObjectType::Resource;
    if (str == "Hazard") return WorldObjectType::Hazard;
    if (str == "Agent") return WorldObjectType::Agent;
    if (str == "Wall") return WorldObjectType::Wall;
    return WorldObjectType::Empty;
}

bool WorldUtils::isWorldObjectTypeValid(WorldObjectType type) {
    return type >= WorldObjectType::Empty && type <= WorldObjectType::Wall;
}

std::vector<float> WorldUtils::normalizeVisionData(const std::vector<float>& visionData, 
                                                  size_t width, size_t height) {
    if (visionData.empty()) return visionData;
    
    std::vector<float> normalized;
    normalized.reserve(visionData.size());
    
    float minVal = *std::min_element(visionData.begin(), visionData.end());
    float maxVal = *std::max_element(visionData.begin(), visionData.end());
    
    for (float val : visionData) {
        if (maxVal > minVal) {
            normalized.push_back((val - minVal) / (maxVal - minVal));
        } else {
            normalized.push_back(0.5f);  // Mid gray if all values are equal
        }
    }
    
    return normalized;
}

float WorldUtils::calculateVisionEntropy(const std::vector<float>& visionData) {
    if (visionData.empty()) return 0.0f;
    
    // Calculate Shannon entropy of vision data
    float total = std::accumulate(visionData.begin(), visionData.end(), 0.0f);
    if (total == 0.0f) return 0.0f;
    
    float entropy = 0.0f;
    for (float val : visionData) {
        if (val > 0.0f) {
            float p = val / total;
            entropy -= p * std::log2(p);
        }
    }
    
    return entropy;
}

float WorldUtils::calculateVisionComplexity(const std::vector<float>& visionData) {
    if (visionData.empty()) return 0.0f;
    
    // Calculate complexity based on variance
    float mean = std::accumulate(visionData.begin(), visionData.end(), 0.0f) / visionData.size();
    float variance = 0.0f;
    for (float val : visionData) {
        variance += (val - mean) * (val - mean);
    }
    variance /= visionData.size();
    
    return std::sqrt(variance);
}

float WorldUtils::calculateDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

bool WorldUtils::checkCollision(const WorldObject& obj1, const WorldObject& obj2) {
    float distance = calculateDistance(obj1.x, obj1.y, obj2.x, obj2.y);
    return distance < (obj1.radius + obj2.radius);
}

ActionResult WorldUtils::determineInteractionResult(WorldObjectType agentType, 
                                                  WorldObjectType objectType) {
    switch (objectType) {
        case WorldObjectType::Resource:
            return ActionResult::Success;  // Collect resource
        case WorldObjectType::Hazard:
            return ActionResult::Damage;  // Take damage
        case WorldObjectType::Empty:
            return ActionResult::None;  // No interaction
        case WorldObjectType::Agent:
            return ActionResult::Collision;  // Collision with other agent
        case WorldObjectType::Wall:
            return ActionResult::Blocked;  // Blocked by wall
        default:
            return ActionResult::None;
    }
}

float WorldUtils::calculateWorldCrowdingFactor(const std::vector<WorldObject>& objects) {
    if (objects.size() <= 1) return 0.0f;
    
    // Calculate crowding factor based on object proximity
    float totalCrowding = 0.0f;
    size_t pairCount = 0;
    
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            float distance = calculateDistance(
                objects[i].x, objects[i].y,
                objects[j].x, objects[j].y
            );
            if (distance < 10.0f) {  // Consider objects within 10 units as crowded
                totalCrowding += 1.0f - (distance / 10.0f);
                pairCount++;
            }
        }
    }
    
    if (pairCount == 0) return 0.0f;
    return totalCrowding / pairCount;
}

float WorldUtils::calculateWorldResourceDensity(const std::vector<WorldObject>& objects) {
    size_t resourceCount = 0;
    for (const auto& obj : objects) {
        if (obj.type == WorldObjectType::Resource) {
            resourceCount++;
        }
    }
    
    return static_cast<float>(resourceCount) / std::max(1.0f, static_cast<float>(objects.size()));
}

float WorldUtils::calculateWorldHazardDensity(const std::vector<WorldObject>& objects) {
    size_t hazardCount = 0;
    for (const auto& obj : objects) {
        if (obj.type == WorldObjectType::Hazard) {
            hazardCount++;
        }
    }
    
    return static_cast<float>(hazardCount) / std::max(1.0f, static_cast<float>(objects.size()));
}

float WorldUtils::calculateEnergyFromObject(WorldObjectType type, float value) {
    switch (type) {
        case WorldObjectType::Resource:
            return std::max(0.0f, value);
        case WorldObjectType::Hazard:
            return std::min(0.0f, value);
        default:
            return 0.0f;
    }
}

float WorldUtils::calculateEnergyCostForMovement(float distance) {
    // Calculate energy cost based on distance moved
    return distance * 0.1f;  // Simple linear cost
}

float WorldUtils::calculateEnergyCostForInteraction(WorldObjectType objectType) {
    // Calculate energy cost for interacting with different object types
    switch (objectType) {
        case WorldObjectType::Resource:
            return 1.0f;  // Small cost to collect
        case WorldObjectType::Hazard:
            return 5.0f;  // High cost to avoid/damage
        case WorldObjectType::Agent:
            return 2.0f;  // Medium cost to interact
        case WorldObjectType::Wall:
            return 0.0f;  // No cost to collide with wall (blocked)
        default:
            return 0.0f;
    }
}

bool WorldUtils::isEnergyCritical(float energy, float maxEnergy) {
    return energy / maxEnergy < 0.1f;
}

// Time utilities
double WorldUtils::getCurrentWorldTime() {
    auto now = std::chrono::steady_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration<double>(epoch).count();
}

std::chrono::steady_clock::time_point WorldUtils::getWorldTimePoint() {
    return std::chrono::steady_clock::now();
}

double WorldUtils::timeSinceWorldStart(const std::chrono::steady_clock::time_point& start) {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - start).count();
}

bool WorldUtils::isWorldTimeToBeReproduced(uint64_t seed, double time) {
    // Check if time should be reproduced based on seed
    // This is a simplified implementation
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    return dis(gen) < 0.5;  // 50% chance for reproducibility
}

} // namespace nlm
