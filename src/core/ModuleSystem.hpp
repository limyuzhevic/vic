/*
 * NLM Module System - Code Organization Framework
 * Provides modular code organization, module loading, and management for neural system
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <any>
#include <stdexcept>
#include <mutex>
#include <filesystem>

namespace nlm {

// Module Version
struct ModuleVersion {
    int major;
    int minor;
    int patch;
    std::string prerelease;
    
    ModuleVersion(int major = 0, int minor = 0, int patch = 0, 
                  const std::string& prerelease = "")
        : major(major), minor(minor), patch(patch), prerelease(prerelease) {}
    
    bool operator==(const ModuleVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator!=(const ModuleVersion& other) const {
        return !(*this == other);
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << major << "." << minor << "." << patch;
        if (!prerelease.empty()) {
            ss << "-" << prerelease;
        }
        return ss.str();
    }
};

// Module Category
enum class ModuleCategory {
    Core,
    Brain,
    Memory,
    Plasticity,
    Neuromodulation,
    Prediction,
    Cognition,
    Sensory,
    Motor,
    World,
    Development,
    Visualization,
    Experimental,
    Utility,
    Test
};

// Module Metadata
struct ModuleMetadata {
    std::string name;
    std::string description;
    ModuleVersion version;
    ModuleCategory category;
    std::vector<std::string> dependencies;
    std::vector<std::string> conflictingModules;
    bool enabled = true;
    bool loaded = false;
    bool initialized = false;
    std::map<std::string, std::any> metadata;
    
    ModuleMetadata() = default;
    ModuleMetadata(const std::string& name, const std::string& desc,
                   const ModuleVersion& version, ModuleCategory category)
        : name(name), description(desc), version(version), category(category),
          enabled(true), loaded(false), initialized(false) {}
};

// Module State
enum class ModuleState {
    Unloaded,
    Loading,
    Loaded,
    Initializing,
    Initialized,
    ShuttingDown,
    Error,
    Disabled
};

// Module Interface
class IModule {
public:
    virtual ~IModule() = default;
    
    virtual const ModuleMetadata& getMetadata() const = 0;
    virtual ModuleState getState() const = 0;
    
    virtual bool load() = 0;
    virtual bool unload() = 0;
    virtual bool initialize(const std::map<std::string, std::any>& config) = 0;
    virtual void shutdown() = 0;
    
    virtual bool isLoaded() const { return getState() == ModuleState::Loaded; }
    virtual bool isInitialized() const { return getState() == ModuleState::Initialized; }
    
    // Module-specific configuration
    virtual void configure(const std::map<std::string, std::any>& config) {}
    
    // Get module services
    virtual std::map<std::string, std::any> getServices() const { return {}; }
    
    // Get module extension points
    virtual std::vector<std::string> getExtensionPoints() const { return {}; }
    
    // Get module dependencies
    virtual std::vector<std::string> getDependencies() const { return {}; }
    
    // Get module conflicts
    virtual std::vector<std::string> getConflicts() const { return {}; }
};

// Module Loader Interface
class IModuleLoader {
public:
    virtual ~IModuleLoader() = default;
    
    virtual bool canLoad(const std::string& modulePath) const = 0;
    virtual std::unique_ptr<IModule> loadModule(const std::string& modulePath) = 0;
    virtual bool unloadModule(std::unique_ptr<IModule>& module) = 0;
};

// File Module Loader
class FileModuleLoader : public IModuleLoader {
public:
    FileModuleLoader();
    ~FileModuleLoader() override;
    
    bool canLoad(const std::string& modulePath) const override;
    std::unique_ptr<IModule> loadModule(const std::string& modulePath) override;
    bool unloadModule(std::unique_ptr<IModule>& module) override;
    
    // Set module directory
    void setModuleDirectory(const std::string& directory) { moduleDirectory_ = directory; }
    
    // Get module directory
    std::string getModuleDirectory() const { return moduleDirectory_; }
    
private:
    std::string moduleDirectory_;
    std::map<std::string, std::string> extensionToLoader;
};

// Module Registry Interface
class IModuleRegistry {
public:
    virtual ~IModuleRegistry() = default;
    
    // Module lifecycle management
    virtual bool loadModule(const std::string& moduleName) = 0;
    virtual bool unloadModule(const std::string& moduleName) = 0;
    virtual bool enableModule(const std::string& moduleName) = 0;
    virtual bool disableModule(const std::string& moduleName) = 0;
    
    // Module discovery and registration
    virtual bool registerModule(std::unique_ptr<IModule> module) = 0;
    virtual bool unregisterModule(const std::string& moduleName) = 0;
    
    // Query modules
    virtual std::vector<std::string> getModuleNames() const = 0;
    virtual std::vector<std::string> getModuleNames(ModuleCategory category) const = 0;
    virtual std::vector<std::string> getModuleNames(const std::vector<ModuleCategory>& categories) const = 0;
    virtual ModuleMetadata getModuleMetadata(const std::string& moduleName) const = 0;
    virtual ModuleState getModuleState(const std::string& moduleName) const = 0;
    
    // Module dependency resolution
    virtual std::vector<std::string> resolveDependencies(const std::string& moduleName) const = 0;
    virtual bool checkConflicts(const std::string& moduleName) const = 0;
    
    // Module configuration
    virtual bool configureModule(const std::string& moduleName,
                               const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getModuleConfig(const std::string& moduleName) const = 0;
    
    // Module services
    virtual bool registerService(const std::string& moduleName,
                               const std::string& serviceName,
                               std::any service) = 0;
    virtual std::any getService(const std::string& serviceName) = 0;
    virtual bool hasService(const std::string& serviceName) const = 0;
    
    // Module extension points
    virtual bool registerExtensionPoint(const std::string& moduleName,
                                       const std::string& extensionPointName,
                                       const std::type_index& type,
                                       bool optional = false) = 0;
    virtual std::vector<std::string> getExtensionPoints(const std::string& moduleName) const = 0;
    
    // Module management
    virtual bool initializeAllModules() = 0;
    virtual void shutdownAllModules() = 0;
    virtual bool reloadModule(const std::string& moduleName) = 0;
    virtual std::string getSystemStatus() const = 0;
};

// Default Module Registry Implementation
class ModuleRegistry : public IModuleRegistry {
public:
    ModuleRegistry();
    ~ModuleRegistry() override;
    
    // Module lifecycle management
    bool loadModule(const std::string& moduleName) override;
    bool unloadModule(const std::string& moduleName) override;
    bool enableModule(const std::string& moduleName) override;
    bool disableModule(const std::string& moduleName) override;
    
    // Module discovery and registration
    bool registerModule(std::unique_ptr<IModule> module) override;
    bool unregisterModule(const std::string& moduleName) override;
    
    // Query modules
    std::vector<std::string> getModuleNames() const override;
    std::vector<std::string> getModuleNames(ModuleCategory category) const override;
    std::vector<std::string> getModuleNames(const std::vector<ModuleCategory>& categories) const override;
    ModuleMetadata getModuleMetadata(const std::string& moduleName) const override;
    ModuleState getModuleState(const std::string& moduleName) const override;
    
    // Module dependency resolution
    std::vector<std::string> resolveDependencies(const std::string& moduleName) const override;
    bool checkConflicts(const std::string& moduleName) const override;
    
    // Module configuration
    bool configureModule(const std::string& moduleName,
                       const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getModuleConfig(const std::string& moduleName) const override;
    
    // Module services
    bool registerService(const std::string& moduleName,
                       const std::string& serviceName,
                       std::any service) override;
    std::any getService(const std::string& serviceName) override;
    bool hasService(const std::string& serviceName) const override;
    
    // Module extension points
    bool registerExtensionPoint(const std::string& moduleName,
                               const std::string& extensionPointName,
                               const std::type_index& type,
                               bool optional = false) override;
    std::vector<std::string> getExtensionPoints(const std::string& moduleName) const override;
    
    // Module management
    bool initializeAllModules() override;
    void shutdownAllModules() override;
    bool reloadModule(const std::string& moduleName) override;
    std::string getSystemStatus() const override;
    
    // Global instance access
    static ModuleRegistry& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Module Manager - High-level module management
class ModuleManager {
public:
    ModuleManager();
    ~ModuleManager();
    
    // Initialize the module system
    bool initialize();
    
    // Shutdown the module system
    void shutdown();
    
    // Get the global module registry
    IModuleRegistry& getRegistry() { return registry_; }
    
    // Load modules from configuration
    bool loadModulesFromConfig(const std::string& configFile);
    
    // Save module configuration
    bool saveModuleConfig(const std::string& configFile) const;
    
    // Get module status
    std::string getModuleStatus(const std::string& moduleName) const;
    std::string getSystemStatus() const;
    
    // Module lifecycle management
    bool loadModule(const std::string& moduleName);
    bool unloadModule(const std::string& moduleName);
    bool initializeModule(const std::string& moduleName,
                         const std::map<std::string, std::any>& config = {});
    bool shutdownModule(const std::string& moduleName);
    
    // Module dependency management
    bool checkModuleDependencies(const std::string& moduleName) const;
    std::vector<std::string> getUnsatisfiedDependencies(const std::string& moduleName) const;
    
    // Global instance access
    static ModuleManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    IModuleRegistry& registry_;
};

// Module Configuration
struct ModuleConfig {
    std::string name;
    bool enabled = true;
    std::map<std::string, std::any> settings;
    std::map<std::string, std::any> dependencies;
    std::vector<std::string> requiredExtensionPoints;
    
    ModuleConfig() = default;
    ModuleConfig(const std::string& name) : name(name) {}
};

// Module System
class ModuleSystem {
public:
    ModuleSystem();
    ~ModuleSystem();
    
    // Initialize the module system
    bool initialize();
    
    // Shutdown the module system
    void shutdown();
    
    // Get the global module manager
    ModuleManager& getManager() { return manager_; }
    
    // Register core modules
    void registerCoreModules();
    
    // Load all modules
    bool loadAllModules();
    
    // Get system information
    std::string getSystemInfo() const;
    
    // Global instance access
    static ModuleSystem& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ModuleManager& manager_;
};

} // namespace nlm