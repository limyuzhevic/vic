#pragma once

#include "../Plugin.hpp"
#include <random>

namespace nlm {

class WorkingMemoryPlugin : public Plugin {
public:
    const PluginMetadata& getMetadata() const override {
        static PluginMetadata metadata(
            "WorkingMemory",
            "Short-term working memory implementation for active information maintenance",
            "NLM Team",
            PluginVersion(1, 0, 0),
            PluginType::Memory
        );
        return metadata;
    }
    
    bool onInitialize() override {
        // Working memory parameters
        capacity_ = 100;           // Maximum number of items
        decay_rate_ = 0.01f;       // Decay rate per timestep
        competition_strength_ = 0.5f;  // Competition strength for winner-take-all
        rehearsal_interval_ = 10;      // Rehearsal interval (steps)
        
        return true;
    }
    
    void enable() override {
        Plugin::enable();
        // Register extension point for memory system creation
        if (getManager()) {
            getManager()->registerExtensionPoint("memory_system", PluginType::Memory,
                [this](std::shared_ptr<Plugin>) { return true; });
        }
    }
    
    // Working memory item structure
    struct MemoryItem {
        size_t id;
        NeuronId neuronId;
        float activation;
        float strength;
        float lastRehearsed;
        std::vector<float> content;
    };
    
    // Add item to working memory
    void addMemoryItem(NeuronId neuronId, const std::vector<float>& content, float initialStrength = 1.0f) {
        MemoryItem item;
        item.id = nextId_++;
        item.neuronId = neuronId;
        item.activation = initialStrength;
        item.strength = initialStrength;
        item.lastRehearsed = currentTime_;
        item.content = content;
        
        items_.push_back(item);
        
        // Decay all items
        for (auto& i : items_) {
            i.strength *= (1.0f - decay_rate_);
            i.activation = i.strength;
        }
        
        // Apply competition if enabled
        if (competition_strength_ > 0.0f) {
            applyCompetition();
        }
    }
    
    // Get active memory items
    std::vector<MemoryItem> getActiveMemoryItems() const {
        std::vector<MemoryItem> active;
        for (const auto& item : items_) {
            if (item.activation > 0.1f) {  // Threshold for active items
                active.push_back(item);
            }
        }
        return active;
    }
    
    // Rehearse memory items (strengthen them)
    void rehearseMemoryItem(size_t itemId) {
        for (auto& item : items_) {
            if (item.id == itemId) {
                item.strength *= 1.5f;
                item.activation = item.strength;
                item.lastRehearsed = currentTime_;
                return;
            }
        }
    }
    
    // Clear decayed items
    void cleanupDecayedItems() {
        auto it = std::remove_if(items_.begin(), items_.end(),
            [this](const MemoryItem& item) {
                float timeSinceRehearsal = currentTime_ - item.lastRehearsed;
                return item.strength < 0.01f || timeSinceRehearsal > rehearsal_interval_ * 1000.0f;
            });
        items_.erase(it, items_.end());
    }
    
    // Get extension points
    std::vector<std::string> getExtensionPoints() const override {
        return {"memory_system", "item_storage", "information_maintenance"};
    }
    
    // Get services
    std::vector<std::string> getProvidedServices() const override {
        return {"working_memory", "short_term_storage", "active_information"};
    }
    
    // Update method
    void update(float dt) override {
        currentTime_ += dt;
        cleanupDecayedItems();
    }
    
    // Configuration methods
    void setCapacity(size_t capacity) { capacity_ = capacity; }
    size_t getCapacity() const { return capacity_; }
    
    void setDecayRate(float rate) { decay_rate_ = rate; }
    float getDecayRate() const { return decay_rate_; }
    
    void setCompetitionStrength(float strength) { competition_strength_ = strength; }
    float getCompetitionStrength() const { return competition_strength_; }
    
private:
    void applyCompetition() {
        if (items_.size() <= 1) return;
        
        // Simple winner-take-all competition
        size_t winner = 0;
        float maxStrength = 0.0f;
        
        for (size_t i = 0; i < items_.size(); ++i) {
            if (items_[i].strength > maxStrength) {
                maxStrength = items_[i].strength;
                winner = i;
            }
        }
        
        // Reduce all other items' strength
        for (size_t i = 0; i < items_.size(); ++i) {
            if (i != winner) {
                items_[i].strength *= (1.0f - competition_strength_);
                items_[i].activation = items_[i].strength;
            }
        }
    }
    
    std::vector<MemoryItem> items_;
    size_t capacity_ = 100;
    float decay_rate_ = 0.01f;
    float competition_strength_ = 0.5f;
    float rehearsal_interval_ = 10.0f;  // in simulation steps
    size_t nextId_ = 0;
    float currentTime_ = 0.0f;
};

} // namespace nlm