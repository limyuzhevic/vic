#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <any>
#include <concepts>

namespace nlm {

// Error handling for plugins
class PluginError : public std::runtime_error {
public:
    enum class Type {
        Unknown,
        Loading,
        Unloading,
        Initialization,
        Shutdown,
        InvalidInterface,
        DependencyMissing,
        VersionMismatch
    };

    PluginError(const std::string& message, Type type = Type::Unknown)
        : std::runtime_error(message), errorType(type) {}

    Type getType() const { return errorType; }

private:
    Type errorType;
};

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string contact;
    std::string pluginInterfaceVersion;
    std::unordered_map<std::string, std::string> dependencies;
};

// Plugin lifecycle states
enum class PluginState {
    Unloaded,
    Loading,
    Loaded,
    Initializing,
    Initialized,
    ShuttingDown,
    Unloading
};

// Base plugin interface - defines the contract for all plugins
class PluginBase {
public:
    virtual ~PluginBase() = default;

    // Get plugin metadata
    virtual const PluginMetadata& getMetadata() const = 0;

    // Get current plugin state
    virtual PluginState getState() const = 0;

    // Initialize the plugin
    virtual void initialize() = 0;

    // Shutdown the plugin
    virtual void shutdown() = 0;

    // Check if plugin is valid
    virtual bool isValid() const = 0;

    // Get plugin-specific data
    virtual std::any getData(const std::string& key) const = 0;

    // Set plugin-specific data
    virtual void setData(const std::string& key, const std::any& data) = 0;

    // Plugin identifier
    virtual std::string getId() const = 0;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<PluginBase>()>;

// Plugin loader interface for dynamic loading
class PluginLoader {
public:
    virtual ~PluginLoader() = default;

    // Load a plugin from file
    virtual std::unique_ptr<PluginBase> loadPlugin(const std::string& path) = 0;

    // Get loader name
    virtual std::string getName() const = 0;

    // Check if loader can handle this file type
    virtual bool canLoad(const std::string& path) const = 0;
};

// Thread safety helper
class ThreadSafePluginManager {
protected:
    mutable std::mutex mutex_;

    template<typename T>
    T lockGuard() {
        return std::lock_guard<std::mutex>(mutex_);
    }
};

} // namespace nlm
