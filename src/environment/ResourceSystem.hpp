#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

namespace nlm {

// Resource type enum
enum class ResourceType {
    ENERGY,
    FOOD,
    MATERIALS,
    WATER,
    OXYGEN,
    ORGANIC_MATTER,
    MINERAL,
    FUEL
};

// Resource item structure
struct ResourceItem {
    ResourceType type;
    float quantity;           // Amount of resource (units)
    float value;              // Resource value (utility)
    float decayRate;          // Decay rate per second (0-1)
    float maxQuantity;        // Maximum capacity
    float regenerationRate;   // Regeneration rate per second (0-1)
    std::string name;         // Human-readable name
    
    ResourceItem() : type(ResourceType::ENERGY), quantity(0.0f), value(1.0f), 
                     decayRate(0.0f), maxQuantity(100.0f), regenerationRate(0.0f), name("Unknown") {}
    
    ResourceItem(ResourceType t, float q, float v, float d = 0.0f, float maxq = 100.0f, float r = 0.0f, const std::string& n = "")
        : type(t), quantity(std::min(q, maxq)), value(v), decayRate(d), 
          maxQuantity(maxq), regenerationRate(r), name(n) {}
    
    // Check if resource is depleted
    bool isEmpty() const {
        return quantity <= 0.0f;
    }
    
    // Check if resource is full
    bool isFull() const {
        return quantity >= maxQuantity;
    }
    
    // Add resource quantity
    void add(float amount) {
        quantity = std::min(quantity + amount, maxQuantity);
    }
    
    // Remove resource quantity
    void remove(float amount) {
        quantity = std::max(0.0f, quantity - amount);
    }
    
    // Update resource state (decay and regeneration)
    void update(float dt) {
        // Apply decay
        if (decayRate > 0.0f) {
            quantity *= std::pow(1.0f - decayRate, dt);
        }
        
        // Apply regeneration
        if (regenerationRate > 0.0f && !isFull()) {
            add(regenerationRate * dt);
        }
    }
    
    // Get usable quantity (considering decay)
    float getUsableQuantity() const {
        return std::max(0.0f, quantity);
    }
    
    // Get utility value
    float getUtility() const {
        return quantity * value;
    }
};

// Resource node in environment (where resources are located)
struct ResourceNode {
    float x;
    float y;
    ResourceType resourceType;
    float radius;
    float quantity;
    float maxQuantity;
    float regenerationRate;
    float lastRegeneration;
    std::vector<std::shared_ptr<ResourceNode>> connections;
    bool active;
    
    ResourceNode() : x(0.0f), y(0.0f), radius(0.5f), quantity(0.0f), maxQuantity(0.0f), 
                     regenerationRate(0.0f), lastRegeneration(0.0f), active(true) {}
    
    ResourceNode(float px, float py, ResourceType type, float pradius, float maxq, float regen)
        : x(px), y(py), resourceType(type), radius(pradius), quantity(maxq),
          maxQuantity(maxq), regenerationRate(regen), lastRegeneration(0.0f), active(true) {}
    
    // Regenerate resource
    void regenerate(float dt) {
        if (active && regenerationRate > 0.0f && quantity < maxQuantity) {
            float gain = regenerationRate * dt;
            quantity = std::min(quantity + gain, maxQuantity);
            lastRegeneration = 0.0f;
        }
    }
    
    // Check if node has sufficient resources
    bool hasSufficient(float required) const {
        return quantity >= required;
    }
    
    // Extract resources from node
    float extract(float amount) {
        float extracted = std::min(amount, quantity);
        quantity -= extracted;
        return extracted;
    }
};

// Resource system for managing environmental resources
class ResourceSystem {
public:
    ResourceSystem() = default;
    ~ResourceSystem() = default;
    
    // Initialize resource system
    void initialize() {
        resources_.clear();
        resourceNodes_.clear();
        agentResources_.clear();
        
        // Create default resource nodes
        createDefaultResourceNodes();
        
        // Initialize agent resources
        for (int i = 0; i < 4; ++i) { // 4 resources per agent
            ResourceItem resource;
            switch (static_cast<ResourceType>(i)) {
                case ResourceType::ENERGY:
                    resource = ResourceItem(ResourceType::ENERGY, 50.0f, 1.0f, 0.01f, 100.0f, 0.1f, "Energy");
                    break;
                case ResourceType::FOOD:
                    resource = ResourceItem(ResourceType::FOOD, 30.0f, 2.0f, 0.02f, 60.0f, 0.05f, "Food");
                    break;
                case ResourceType::WATER:
                    resource = ResourceItem(ResourceType::WATER, 40.0f, 1.5f, 0.01f, 80.0f, 0.08f, "Water");
                    break;
                case ResourceType::MATERIALS:
                    resource = ResourceItem(ResourceType::MATERIALS, 20.0f, 1.2f, 0.0f, 40.0f, 0.02f, "Materials");
                    break;
                default:
                    resource = ResourceItem(ResourceType::ENERGY, 0.0f, 1.0f);
                    break;
            }
            agentResources_.push_back(resource);
        }
    }
    
    // Update resource system
    void update(float dt, const EnvironmentalConditions& conditions) {
        // Update resource nodes
        updateResourceNodes(dt, conditions);
        
        // Update agent resources
        for (auto& resource : agentResources_) {
            resource.update(dt);
        }
        
        // Environmental effects on resources
        applyEnvironmentalEffects(dt, conditions);
    }
    
    // Add resource node
    void addResourceNode(const ResourceNode& node) {
        resourceNodes_.push_back(std::make_shared<ResourceNode>(node));
    }
    
    // Get resources at position (within radius)
    std::vector<ResourceNode*> getResourcesAt(float x, float y, float radius) {
        std::vector<ResourceNode*> nearbyNodes;
        
        for (auto& node : resourceNodes_) {
            float dx = node->x - x;
            float dy = node->y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= radius && node->active) {
                nearbyNodes.push_back(node.get());
            }
        }
        
        return nearbyNodes;
    }
    
    // Get resource by type from nearest node
    float getResourceByType(ResourceType type, float x, float y, float radius) {
        float bestAmount = 0.0f;
        
        for (auto& node : resourceNodes_) {
            if (!node->active || node->resourceType != type) continue;
            
            float dx = node->x - x;
            float dy = node->y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= radius) {
                bestAmount = std::max(bestAmount, node->quantity);
            }
        }
        
        return bestAmount;
    }
    
    // Collect resources from nearby nodes
    std::vector<ResourceItem> collectResources(float x, float y, float collectionRadius) {
        std::vector<ResourceItem> collected;
        
        // Find nearby nodes
        auto nearbyNodes = getResourcesAt(x, y, collectionRadius);
        
        // Extract resources from each node
        for (auto* node : nearbyNodes) {
            if (!node->active) continue;
            
            // Determine what to collect based on node type
            switch (node->resourceType) {
                case ResourceType::ENERGY:
                    collected.push_back(ResourceItem(ResourceType::ENERGY, 
                        node->extract(10.0f), 1.0f, 0.01f, 100.0f, 0.1f, "Energy"));
                    break;
                case ResourceType::FOOD:
                    collected.push_back(ResourceItem(ResourceType::FOOD, 
                        node->extract(5.0f), 2.0f, 0.02f, 60.0f, 0.05f, "Food"));
                    break;
                case ResourceType::WATER:
                    collected.push_back(ResourceItem(ResourceType::WATER, 
                        node->extract(8.0f), 1.5f, 0.01f, 80.0f, 0.08f, "Water"));
                    break;
                case ResourceType::MATERIALS:
                    collected.push_back(ResourceItem(ResourceType::MATERIALS, 
                        node->extract(3.0f), 1.2f, 0.0f, 40.0f, 0.02f, "Materials"));
                    break;
                default:
                    break;
            }
        }
        
        return collected;
    }
    
    // Add collected resources to agent inventory
    void addToAgentInventory(const std::vector<ResourceItem>& resources) {
        for (const auto& collected : resources) {
            // Find matching resource type in agent inventory
            bool found = false;
            for (auto& agentResource : agentResources_) {
                if (agentResource.type == collected.type) {
                    agentResource.add(collected.quantity);
                    found = true;
                    break;
                }
            }
            
            // If not found, add as new resource
            if (!found) {
                agentResources_.push_back(collected);
            }
        }
    }
    
    // Get agent resource by type
    float getAgentResource(ResourceType type) const {
        for (const auto& resource : agentResources_) {
            if (resource.type == type) {
                return resource.getUsableQuantity();
            }
        }
        return 0.0f;
    }
    
    // Check if agent has sufficient resource
    bool hasAgentResource(ResourceType type, float required) const {
        return getAgentResource(type) >= required;
    }
    
    // Use agent resource (consume)
    bool useAgentResource(ResourceType type, float amount) {
        for (auto& resource : agentResources_) {
            if (resource.type == type) {
                if (resource.getUsableQuantity() >= amount) {
                    resource.remove(amount);
                    return true;
                }
                return false;
            }
        }
        return false;
    }
    
    // Get all agent resources
    const std::vector<ResourceItem>& getAgentResources() const {
        return agentResources_;
    }
    
    // Get resource node at position
    ResourceNode* getResourceNodeAt(float x, float y, float radius) {
        for (auto& node : resourceNodes_) {
            float dx = node->x - x;
            float dy = node->y - y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance <= radius) {
                return node.get();
            }
        }
        return nullptr;
    }
    
    // Get resource statistics
    float getResourceStatistics(ResourceType type) const {
        float total = 0.0f;
        for (const auto& node : resourceNodes_) {
            if (node->resourceType == type && node->active) {
                total += node->quantity;
            }
        }
        return total;
    }
    
    // Generate resources based on environmental conditions
    void generateResources(const EnvironmentalConditions& conditions) {
        // Generate resources based on weather and time
        float timeFactor = conditions.timeOfDay / 24.0f;
        
        // Temperature-based resource generation
        if (conditions.temperature > 20.0f && conditions.temperature < 30.0f) {
            // Optimal temperature for resource growth
            addResourceNode(ResourceNode(15.0f, 15.0f, ResourceType::FOOD, 0.3f, 50.0f, 0.5f));
            addResourceNode(ResourceNode(5.0f, 5.0f, ResourceType::MATERIALS, 0.4f, 30.0f, 0.3f));
        } else if (conditions.temperature < 10.0f) {
            // Cold weather, less resource growth
            addResourceNode(ResourceNode(15.0f, 15.0f, ResourceType::FOOD, 0.3f, 20.0f, 0.2f));
        }
        
        // Rain-based resource generation
        if (conditions.humidity > 70.0f) {
            addResourceNode(ResourceNode(10.0f, 10.0f, ResourceType::WATER, 0.5f, 100.0f, 1.0f));
        }
        
        // Light-based resource generation
        if (conditions.lightLevel > 0.5f) {
            addResourceNode(ResourceNode(20.0f, 20.0f, ResourceType::MATERIALS, 0.4f, 40.0f, 0.4f));
        }
        
        // Pollution-based resource changes
        if (conditions.pollutionLevel > 0.5f) {
            addResourceNode(ResourceNode(8.0f, 8.0f, ResourceType::MATERIALS, 0.3f, 10.0f, 0.1f)); // Reduced growth
        }
    }
    
private:
    std::vector<ResourceItem> agentResources_;           // Agent's resource inventory
    std::vector<std::shared_ptr<ResourceNode>> resourceNodes_; // Environmental resource nodes
    
    // Create default resource nodes
    void createDefaultResourceNodes() {
        // Food resources
        addResourceNode(ResourceNode(10.0f, 10.0f, ResourceType::FOOD, 0.3f, 50.0f, 0.3f));
        addResourceNode(ResourceNode(15.0f, 15.0f, ResourceType::FOOD, 0.3f, 50.0f, 0.3f));
        addResourceNode(ResourceNode(5.0f, 5.0f, ResourceType::FOOD, 0.3f, 50.0f, 0.3f));
        
        // Water resources
        addResourceNode(ResourceNode(8.0f, 12.0f, ResourceType::WATER, 0.4f, 80.0f, 0.5f));
        addResourceNode(ResourceNode(12.0f, 8.0f, ResourceType::WATER, 0.4f, 80.0f, 0.5f));
        
        // Materials resources
        addResourceNode(ResourceNode(3.0f, 18.0f, ResourceType::MATERIALS, 0.4f, 40.0f, 0.2f));
        addResourceNode(ResourceNode(18.0f, 3.0f, ResourceType::MATERIALS, 0.4f, 40.0f, 0.2f));
        
        // Energy resources
        addResourceNode(ResourceNode(18.0f, 18.0f, ResourceType::ENERGY, 0.5f, 60.0f, 0.4f));
        
        // Material resources
        addResourceNode(ResourceNode(18.0f, 10.0f, ResourceType::MINERAL, 0.3f, 30.0f, 0.1f));
    }
    
    // Update resource nodes
    void updateResourceNodes(float dt, const EnvironmentalConditions& conditions) {
        for (auto& node : resourceNodes_) {
            node->regenerate(dt);
            
            // Environmental effects on nodes
            applyEnvironmentalEffectsToNode(*node, conditions);
        }
    }
    
    // Apply environmental effects to specific resource node
    void applyEnvironmentalEffectsToNode(ResourceNode& node, const EnvironmentalConditions& conditions) {
        // Temperature effects
        if (conditions.temperature > 35.0f) {
            // High temperature damages resources
            node.quantity *= 0.95f;
        } else if (conditions.temperature < 5.0f) {
            // Low temperature slows regeneration
            // (no effect on quantity for now)
        }
        
        // Humidity effects
        if (conditions.humidity > 80.0f) {
            // High humidity increases food growth
            if (node.resourceType == ResourceType::FOOD) {
                node.regenerationRate *= 1.2f;
            }
        }
        
        // Light effects
        if (conditions.lightLevel < 0.2f) {
            // Darkness slows regeneration
            node.regenerationRate *= 0.7f;
        }
        
        // Pollution effects
        if (conditions.pollutionLevel > 0.5f) {
            // Pollution damages resources
            node.quantity *= 0.98f;
            if (node.resourceType == ResourceType::MATERIALS) {
                node.regenerationRate *= 0.8f;
            }
        }
    }
    
    // Apply environmental effects to agent resources
    void applyEnvironmentalEffects(float dt, const EnvironmentalConditions& conditions) {
        for (auto& resource : agentResources_) {
            // Temperature effects on agent resources
            if (conditions.temperature > 30.0f && resource.type == ResourceType::FOOD) {
                // Heat spoilage
                resource.decayRate += 0.01f * dt;
            } else if (conditions.temperature < 10.0f && resource.type == ResourceType::WATER) {
                // Water freezing
                resource.decayRate += 0.005f * dt;
            }
            
            // Humidity effects on agent resources
            if (conditions.humidity > 80.0f && resource.type == ResourceType::MATERIALS) {
                // Moisture damage
                resource.decayRate += 0.008f * dt;
            }
            
            // Light effects on agent resources
            if (conditions.lightLevel < 0.2f && resource.type == ResourceType::FOOD) {
                // Food spoilage in darkness
                resource.decayRate += 0.005f * dt;
            }
        }
    }
};

} // namespace nlm