#pragma once

#include <string>

namespace nlm {

class INeuralSystem {
public:
    virtual ~INeuralSystem() = default;
    
    // Initialize the neural system
    virtual bool initialize(std::shared_ptr<Config> config) = 0;
    
    // Update the neural system
    virtual void update(double dt) = 0;
    
    // Get system name
    virtual std::string getName() const = 0;
    
    // Check if system is initialized
    virtual bool isInitialized() const = 0;
    
    // Save system state
    virtual bool save(const std::string& filepath) const = 0;
    
    // Load system state
    virtual bool load(const std::string& filepath) = 0;
};

class IUpdatable {
public:
    virtual ~IUpdatable() = default;
    
    // Update with time delta
    virtual void update(double dt) = 0;
    
    // Check if update is needed
    virtual bool needsUpdate() const = 0;
    
    // Mark for update
    virtual void markForUpdate() = 0;
    
    // Clear update flag
    virtual void clearUpdateFlag() = 0;
};

class ISaveable {
public:
    virtual ~ISaveable() = default;
    
    // Save state to file
    virtual bool save(const std::string& filepath) const = 0;
    
    // Load state from file
    virtual bool load(const std::string& filepath) = 0;
    
    // Get version string for compatibility checking
    virtual std::string getVersion() const = 0;
};

} // namespace nlm
