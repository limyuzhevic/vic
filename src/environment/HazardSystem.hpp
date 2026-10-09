#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// Hazard type enum
enum class HazardType {
    STATIC_HAZARD,
    MOVING_HAZARD,
    AREA_EFFECT_HAZARD,
    TOXIC_PILE,
    FIRE,
    LAVA,
    ELECTRICAL,
    RADIATION,
    BIOLOGICAL,
    TERRAIN_COLLAPSE,
    PREDATOR,
    PEST,
WATER_HAZARD
};

// Hazard instance
struct Hazard {
    HazardType type;
    float x;
    float y;
    float radius;
    float damagePerSecond;      // Damage per second (0 for non-damaging)
    float detectionRadius;      // Detection radius for agents
    std::string name;           // Human-readable name
    std::string description;    // Hazard description
    bool active;                // Whether hazard is active
    bool detectable;            // Whether hazard can be detected by sensors
    float duration;             // Duration (0 for infinite)
    float age;                  // Age of hazard
    
    Hazard() : type(HazardType::STATIC_HAZARD), x(0.0f), y(0.0f), radius(0.5f),
               damagePerSecond(0.0f), detectionRadius(2.0f), active(true),
               detectable(true), duration(0.0f), age(0.0f) {}
    
    Hazard(HazardType t, float px, float py, float pradius, float damage, float detection,
           const std::string& n, const std::string& desc)
        : type(t), x(px), y(py), radius(pradius), damagePerSecond(damage),
          detectionRadius(detection), name(n), description(desc),
          active(true), detectable(true), duration(0.0f), age(0.0f) {}
    
    // Check if agent is within hazard range
    bool affectsAgent(float ax, float ay, float agentRadius = 0.3f) const {
        float dx = ax - x;
        float dy = ay - y;
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance <= (radius + agentRadius);
    }
    
    // Update hazard (age, move, etc.)
    void update(float dt) {
        age += dt;
        
        if (type == HazardType::MOVING_HAZARD) {
            // Move hazard (simple example: move in circle)
            float angle = age * 0.5f; // Circular motion
            x = std::cos(angle) * 8.0f + 10.0f; // Circle around center
            y = std::sin(angle) * 6.0f + 10.0f;
        } else if (type == HazardType::WATER hazard) {
            // Water hazard spreads
            radius = std::min(radius + 0.1f * dt, 2.0f);
        }
    }
    
    // Check if hazard is expired
    bool isExpired() const {
        return duration > 0.0f && age > duration;
    }
    
    // Check if hazard is still active
    bool isActive() const {
        return active && !isExpired();
    }
};

// Navigation obstacle
struct NavigationObstacle {
    float x;
    float y;
    float width;
    float height;
    float friction;             // Friction coefficient
    bool passable;             // Whether agent can pass through
    std::string type;          // Type of obstacle (wall, pit, etc.)
    
    NavigationObstacle() : x(0.0f), y(0.0f), width(1.0f), height(1.0f),
                           friction(0.5f), passable(false), type("wall") {}
    
    NavigationObstacle(float px, float py, float w, float h, bool passable = false)
        : x(px), y(py), width(w), height(h),
          friction(passable ? 0.1f : 0.9f), passable(passable), type("obstacle") {}
    
    // Check if agent collides with obstacle
    bool collidesWith(float ax, float ay, float agentRadius = 0.3f) const {
        float closestX = std::max(x - width/2, std::min(ax, x + width/2));
        float closestY = std::max(y - height/2, std::min(ay, y + height/2));
        
        float distanceX = ax - closestX;
        float distanceY = ay - closestY;
        float distanceSquared = distanceX * distanceX + distanceY * distanceY;
        
        return distanceSquared < agentRadius * agentRadius;
    }
    
    // Check if line segment intersects obstacle
    bool intersects(float x1, float y1, float x2, float y2) const {
        // Simple AABB intersection test
        float minX = x - width/2;
        float maxX = x + width/2;
        float minY = y - height/2;
        float maxY = y + height/2;
        
        // Check if line segment bounding box intersects obstacle
        return !(x2 < minX || x1 > maxX || y2 < minY || y1 > maxY);
    }
};

// Hazard system for dangerous areas and obstacles
class HazardSystem {
public:
    HazardSystem() = default;
    ~HazardSystem() = default;
    
    // Initialize hazard system
    void initialize() {
        hazards_.clear();
        obstacles_.clear();
        
        // Create default hazards
        createDefaultHazards();
    }
    
    // Update hazard system
    void update(float dt, const EnvironmentalConditions& conditions) {
        // Update all hazards
        for (auto& hazard : hazards_) {
            hazard.update(dt);
            
            // Environmental effects on hazards
            applyEnvironmentalEffectsToHazard(*hazard, conditions);
        }
        
        // Remove expired hazards
        hazards_.erase(
            std::remove_if(hazards_.begin(), hazards_.end(),
                [](const std::shared_ptr<Hazard>& hazard) {
                    return !hazard->isActive();
                }),
            hazards_.end()
        );
        
        // Update obstacles (not needed for static obstacles)
    }
    
    // Add hazard
    void addHazard(const Hazard& hazard) {
        hazards_.push_back(std::make_shared<Hazard>(hazard));
    }
    
    // Add navigation obstacle
    void addNavigationObstacle(const NavigationObstacle& obstacle) {
        obstacles_.push_back(obstacle);
    }
    
    // Get hazards at position
    std::vector<Hazard*> getHazardsAt(float x, float y, float radius) {
        std::vector<Hazard*> nearbyHazards;
        
        for (auto& hazard : hazards_) {
            if (!hazard->isActive()) continue;
            
            float dx = hazard->x - x;
            float dy = hazard->y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= hazard->detectionRadius) {
                nearbyHazards.push_back(hazard.get());
            }
        }
        
        return nearbyHazards;
    }
    
    // Check if position is safe (no hazards or obstacles)
    bool isSafePosition(float x, float y, float radius = 0.3f) const {
        // Check navigation obstacles
        for (const auto& obstacle : obstacles_) {
            if (obstacle.collidesWith(x, y, radius)) {
                return false;
            }
        }
        
        // Check hazards
        for (const auto& hazard : hazards_) {
            if (hazard->isActive() && hazard->affectsAgent(x, y, radius)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Calculate damage from nearby hazards
    float calculateHazardDamage(float x, float y) const {
        float totalDamage = 0.0f;
        
        for (const auto& hazard : hazards_) {
            if (!hazard->isActive() || hazard->damagePerSecond <= 0.0f) continue;
            
            float dx = hazard->x - x;
            float dy = hazard->y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= hazard->radius) {
                // Damage is higher closer to hazard center
                float damageMultiplier = 1.0f - distance / hazard->radius;
                totalDamage += hazard->damagePerSecond * damageMultiplier;
            }
        }
        
        return totalDamage;
    }
    
    // Find safe path to target position
    std::vector<std::pair<float, float>> findSafePath(float startX, float startY, float targetX, float targetY,
                                                      float agentRadius = 0.3f) {
        std::vector<std::pair<float, float>> path;
        path.push_back(std::make_pair(startX, startY));
        
        // Simple pathfinding (A* would be better but more complex)
        float stepSize = 0.5f;
        float dx = targetX - startX;
        float dy = targetY - startY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance <= stepSize) {
            path.push_back(std::make_pair(targetX, targetY));
            return path;
        }
        
        float steps = std::ceil(distance / stepSize);
        float stepX = dx / steps;
        float stepY = dy / steps;
        
        float currentX = startX;
        float currentY = startY;
        
        for (int i = 1; i < steps; ++i) {
            currentX += stepX;
            currentY += stepY;
            
            // Check if position is safe
            if (!isSafePosition(currentX, currentY, agentRadius)) {
                // Find alternative path
                currentX = path.back().first;
                currentY = path.back().second;
                
                // Try to go around obstacle
                float rightX = currentX + stepY;
                float rightY = currentY - stepX;
                if (isSafePosition(rightX, rightY, agentRadius)) {
                    currentX = rightX;
                    currentY = rightY;
                } else {
                    float leftX = currentX - stepY;
                    float leftY = currentY + stepX;
                    if (isSafePosition(leftX, leftY, agentRadius)) {
                        currentX = leftX;
                        currentY = leftY;
                    } else {
                        // Can't find safe path
                        return path;
                    }
                }
            }
            
            path.push_back(std::make_pair(currentX, currentY));
        }
        
        path.push_back(std::make_pair(targetX, targetY));
        return path;
    }
    
    // Get obstacle at position
    const NavigationObstacle* getObstacleAt(float x, float y) const {
        for (const auto& obstacle : obstacles_) {
            if (obstacle.collidesWith(x, y, 0.0f)) {
                return &obstacle;
            }
        }
        return nullptr;
    }
    
    // Check if line segment intersects any obstacle
    bool intersectsObstacle(float x1, float y1, float x2, float y2) const {
        for (const auto& obstacle : obstacles_) {
            if (obstacle.intersects(x1, y1, x2, y2)) {
                return true;
            }
        }
        return false;
    }
    
    // Generate hazards based on environmental conditions
    void generateHazards(const EnvironmentalConditions& conditions) {
        // Generate hazards based on weather and terrain
        if (conditions.humidity > 80.0f && conditions.temperature > 20.0f) {
            // Swamp/trap hazards
            addHazard(Hazard(HazardType::WATER hazard, 5.0f, 5.0f, 1.0f, 0.5f, 3.0f, "Swamp", "Slow, damaging terrain"));
            addHazard(Hazard(HazardType::BIOLOGICAL, 15.0f, 15.0f, 0.8f, 1.0f, 5.0f, "Puddle", "Biological hazard"));
        } else if (conditions.temperature > 30.0f) {
            // Fire hazards
            addHazard(Hazard(HazardType::FIRE, 10.0f, 10.0f, 1.0f, 2.0f, 4.0f, "Fire", "Burning hazard"));
            addHazard(Hazard(HazardType::LAVA, 20.0f, 20.0f, 1.5f, 3.0f, 6.0f, "Lava", "Extreme heat hazard"));
        } else if (conditions.windSpeed > 10.0f) {
            // Flying debris hazards
            addHazard(Hazard(HazardType::MOVING_HAZARD, 5.0f, 5.0f, 0.5f, 0.3f, 8.0f, "Debris", "Flying debris"));
            addHazard(Hazard(HazardType::PREDATOR, 12.0f, 12.0f, 0.6f, 0.8f, 6.0f, "Predator", "Aggressive hazard"));
        } else if (conditions.pollutionLevel > 0.7f) {
            // Toxic hazards
            addHazard(Hazard(HazardType::TOXIC_PILE, 8.0f, 8.0f, 1.2f, 0.7f, 5.0f, "Toxic Pool", "Poisonous water"));
            addHazard(Hazard(HazardType::RADIATION, 18.0f, 18.0f, 1.0f, 1.5f, 7.0f, "Radiation Zone", "High radiation area"));
        } else if (conditions.lightLevel < 0.2f) {
            // Night hazards
            addHazard(Hazard(HazardType::PREDATOR, 7.0f, 7.0f, 0.7f, 0.5f, 6.0f, "Night Predator", "Active at night"));
            addHazard(Hazard(HazardType::PEST, 3.0f, 3.0f, 0.4f, 0.2f, 4.0f, "Pest Swarm", "Insects"));
        }
        
        // Terrain hazards (static)
        addNavigationObstacle(NavigationObstacle(2.0f, 2.0f, 1.5f, 0.5f, true));  // Pit
        addNavigationObstacle(NavigationObstacle(15.0f, 15.0f, 2.0f, 2.0f, false)); // Wall
        addNavigationObstacle(NavigationObstacle(8.0f, 5.0f, 1.0f, 3.0f, false));  // Barrier
        addNavigationObstacle(NavigationObstacle(18.0f, 10.0f, 3.0f, 1.5f, false)); // Large wall
    }
    
    // Clear all hazards and obstacles
    void clearHazards() {
        hazards_.clear();
        obstacles_.clear();
    }
    
private:
    std::vector<std::shared_ptr<Hazard>> hazards_;          // Active hazards
    std::vector<NavigationObstacle> obstacles_;           // Navigation obstacles
    
    // Create default hazards
    void createDefaultHazards() {
        // Default wall around edges
        addNavigationObstacle(NavigationObstacle(0.0f, 10.0f, 20.0f, 0.5f, false));  // Left wall
        addNavigationObstacle(NavigationObstacle(10.0f, 0.0f, 20.0f, 0.5f, false));  // Bottom wall
        addNavigationObstacle(NavigationObstacle(20.0f, 10.0f, 20.0f, 0.5f, false)); // Right wall
        addNavigationObstacle(NavigationObstacle(10.0f, 20.0f, 20.0f, 0.5f, false)); // Top wall
        
        // Default hazard in center (can be removed/reset)
        addHazard(Hazard(HazardType::TOXIC_PILE, 10.0f, 10.0f, 1.0f, 0.0f, 3.0f, "Toxic Pool", "Poisonous water"));
    }
    
    // Apply environmental effects to hazard
    void applyEnvironmentalEffectsToHazard(Hazard& hazard, const EnvironmentalConditions& conditions) {
        if (!hazard.isActive()) return;
        
        // Temperature effects on hazards
        if (hazard.type == HazardType::FIRE && conditions.temperature < 10.0f) {
            // Fire extinguishes in cold
            hazard.active = false;
        } else if (hazard.type == HazardType::LAVA && conditions.temperature < 20.0f) {
            // Lava solidifies in cold
            hazard.active = false;
        }
        
        // Humidity effects
        if (hazard.type == HazardType::FIRE && conditions.humidity > 80.0f) {
            // Fire suppressed by humidity
            hazard.active = false;
        }
        
        // Light effects
        if (conditions.lightLevel < 0.2f) {
            // Some hazards become more active at night
            if (hazard.type == HazardType::PREDATOR) {
                hazard.damagePerSecond *= 1.5f;
            } else if (hazard.type == HazardType::PEST) {
                hazard.active = true;
            }
        }
        
        // Pollution effects
        if (conditions.pollutionLevel > 0.7f && hazard.type == HazardType::TOXIC_PILE) {
            // Toxic pools get stronger with pollution
            hazard.damagePerSecond *= 1.2f;
        }
        
        // Wind effects
        if (hazard.type == HazardType::MOVING_HAZARD && conditions.windSpeed > 15.0f) {
            // Wind affects moving hazards
            // (no implementation needed for now)
        }
    }
};

} // namespace nlm