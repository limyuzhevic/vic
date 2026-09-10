#pragma once

#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class RandomGenerator;

// Common component interface for all brain subsystems
class BrainComponentBase {
public:
    virtual ~BrainComponentBase() = default;
    
    // Initialize component with brain reference
    virtual bool initialize(Brain* brain) = 0;
    
    // Update component state for current timestep
    virtual void update(const TimestepDuration& dt) = 0;
    
    // Reset component to initial state
    virtual void reset() = 0;
    
    // Log component status
    virtual void logStatus() const = 0;
    
    // Get component name
    virtual const char* getName() const = 0;
    
    // Has component been initialized?
    bool isInitialized() const { return initialized_; }
    
protected:
    bool initialized_ = false;
};

// Common configuration interface
class ConfigurationManager {
public:
    ConfigurationManager() = default;
    virtual ~ConfigurationManager() = default;
    
    // Load configuration from file
    virtual bool loadFromFile(const std::string& filepath) = 0;
    
    // Get parameter by key and type
    virtual void* getParameter(const std::string& key) = 0;
    
    // Get parameter with default value
    template<typename T>
    T getOr(const std::string& key, T defaultValue) {
        void* value = getParameter(key);
        return value ? *static_cast<T*>(value) : defaultValue;
    }
    
    // Set parameter
    virtual void setParameter(const std::string& key, const void* value) = 0;
    
    // Get configuration source
    virtual const std::string& getSource() const = 0;
};

} // namespace nlm
