#pragma once

#include "BrainSystem.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace nlm {

// System registry for managing brain systems
class SystemRegistry {
public:
    // Get the singleton instance
    static SystemRegistry& getInstance() {
        static SystemRegistry instance;
        return instance;
    }
    
    // Register a system factory
    void registerSystem(const std::string& name, SystemFactory factory) {
        systemFactories_[name] = factory;
    }
    
    // Create and initialize a system
    std::unique_ptr<BrainSystem> createSystem(const std::string& name, Brain* brain) {
        auto it = systemFactories_.find(name);
        if (it != systemFactories_.end()) {
            auto system = it->second(brain);
            if (system) {
                system->setInitialized(true);
            }
            return system;
        }
        return nullptr;
    }
    
    // Check if a system is registered
    bool isSystemRegistered(const std::string& name) const {
        return systemFactories_.find(name) != systemFactories_.end();
    }
    
    // Get all registered system names
    std::vector<std::string> getRegisteredSystemNames() const {
        std::vector<std::string> names;
        names.reserve(systemFactories_.size());
        for (const auto& pair : systemFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Clear all registered systems
    void clear() {
        systemFactories_.clear();
    }
    
    // Get system type by name
    std::string getSystemType(const std::string& name) const {
        auto it = systemFactories_.find(name);
        if (it != systemFactories_.end()) {
            return it->first;
        }
        return "";
    }
    
private:
    SystemRegistry() = default;
    ~SystemRegistry() = default;
    
    // Prevent copying
    SystemRegistry(const SystemRegistry&) = delete;
    SystemRegistry& operator=(const SystemRegistry&) = delete;
    
    // Map of system name to factory function
    std::unordered_map<std::string, SystemFactory> systemFactories_;
};

} // namespace nlm
