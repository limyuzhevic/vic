#pragma once

#include <string>
#include <functional>
#include <any>
#include <memory>
#include "Config.hpp"

namespace nlm {

// Configuration change event
class ConfigChangeEvent {
public:
    enum class Type {
        ValueChanged,
        KeyAdded,
        KeyRemoved,
        ConfigValidated,
        ConfigSaved,
        ConfigLoaded
    };

    ConfigChangeEvent(Type type, const std::string& key = "",
                     const std::any& oldValue = std::any(),
                     const std::any& newValue = std::any(),
                     const std::string& source = "",
                     const std::string& message = "");

    Type getType() const { return type; }
    const std::string& getKey() const { return key; }
    const std::any& getOldValue() const { return oldValue; }
    const std::any& getNewValue() const { return newValue; }
    const std::string& getSource() const { return source; }
    const std::string& getMessage() const { return message; }

    // Check if event is valid
    bool isValid() const;

    // Convert event to string representation
    std::string toString() const;

private:
    Type type;
    std::string key;
    std::any oldValue;
    std::any newValue;
    std::string source;
    std::string message;
};

// Configuration change listener interface
class ConfigChangeListener {
public:
    virtual ~ConfigChangeListener() = default;
    
    // Handle configuration change event
    virtual void onConfigChanged(const ConfigChangeEvent& event) = 0;
    
    // Get listener name
    virtual std::string getName() const = 0;
};

// Configuration validation callback
class ConfigValidationCallback {
public:
    virtual ~ConfigValidationCallback() = default;
    
    // Called before configuration validation
    virtual bool beforeValidate(const ConfigChangeEvent& event) = 0;
    
    // Called after configuration validation
    virtual void afterValidate(const ConfigChangeEvent& event, bool success) = 0;
    
    // Get callback name
    virtual std::string getName() const = 0;
};

// Configuration update callback
class ConfigUpdateCallback {
public:
    virtual ~ConfigUpdateCallback() = default;
    
    // Called when configuration is updated
    virtual void onUpdate(const ConfigChangeEvent& event) = 0;
    
    // Check if callback should be called for this event
    virtual bool shouldHandle(const ConfigChangeEvent& event) const = 0;
    
    // Get callback name
    virtual std::string getName() const = 0;
};

// Configuration error callback
class ConfigErrorCallback {
public:
    virtual ~ConfigErrorCallback() = default;
    
    // Called when configuration error occurs
    virtual void onError(const ConfigChangeEvent& event, const std::string& errorMessage) = 0;
    
    // Get callback name
    virtual std::string getName() const = 0;
};

// Utility functions for working with callbacks
namespace CallbackUtils {
    std::string generateCallbackId();
    
    template<typename T>
    std::function<void(const ConfigChangeEvent&)> wrapCallback(
        const std::shared_ptr<T>& callback, void (T::*method)(const ConfigChangeEvent&));
    
    template<typename T>
    std::function<bool(const ConfigChangeEvent&)> wrapShouldHandle(
        const std::shared_ptr<T>& callback, bool (T::*method)(const ConfigChangeEvent&) const);
}

} // namespace nlm