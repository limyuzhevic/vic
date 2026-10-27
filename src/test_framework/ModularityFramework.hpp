// Modularity and Extensibility Improvements for NLM Phase 2
// Provides advanced plugin architecture and extensibility for test framework

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <iostream>
#include <fstream>

// Forward declarations
namespace nlm {
    class Brain;
    class Config;
}

// Plugin interface for extensibility
class ITestPlugin {
public:
    virtual ~ITestPlugin() = default;
    
    // Get plugin metadata
    virtual std::string getPluginName() const = 0;
    virtual std::string getPluginVersion() const = 0;
    virtual std::string getPluginDescription() const = 0;
    virtual std::string getPluginAuthor() const = 0;
    
    // Get supported test types
    virtual std::vector<std::string> getSupportedTestTypes() const = 0;
    
    // Create test instance
    virtual std::shared_ptr<ITestPlugin> createTest(const std::string& testType) = 0;
    
    // Check if plugin can handle specific test type
    virtual bool canHandleTestType(const std::string& testType) const = 0;
    
    // Get configuration requirements
    virtual std::vector<std::string> getRequiredConfigKeys() const = 0;
    virtual std::vector<std::string> getOptionalConfigKeys() const = 0;
    
    // Initialize plugin with configuration
    virtual bool initialize(const nlm::Config& config) = 0;
    
    // Clean up plugin
    virtual void cleanup() = 0;
    
    // Get plugin status
    virtual bool isInitialized() const = 0;
    virtual std::string getStatus() const = 0;
};

// Abstract test factory interface
class ITestFactory {
public:
    virtual ~ITestFactory() = default;
    
    virtual std::shared_ptr<ITestPlugin> createTest(const std::string& testType) = 0;
    virtual bool canCreateTest(const std::string& testType) const = 0;
    virtual std::vector<std::string> getSupportedTestTypes() const = 0;
    virtual std::string getFactoryName() const = 0;
    virtual std::string getFactoryVersion() const = 0;
};

// Plugin manager for loading and managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager() = default;
    
    // Load plugin from file
    bool loadPlugin(const std::string& pluginPath);
    
    // Load plugin from directory
    bool loadPluginsFromDirectory(const std::string& directoryPath);
    
    // Unload plugin
    bool unloadPlugin(const std::string& pluginName);
    
    // Get plugin by name
    std::shared_ptr<ITestPlugin> getPlugin(const std::string& pluginName);
    
    // Get all plugins
    std::vector<std::shared_ptr<ITestPlugin>> getAllPlugins() const;
    
    // Create test using plugin
    std::shared_ptr<ITestPlugin> createTest(const std::string& pluginName, 
                                            const std::string& testType);
    
    // Create test using factory
    std::shared_ptr<ITestPlugin> createTestFromFactory(const std::string& testType);
    
    // Check if plugin can handle test type
    bool canCreateTest(const std::string& testType) const;
    
    // Get all supported test types
    std::vector<std::string> getAllSupportedTestTypes() const;
    
    // Get plugin names
    std::vector<std::string> getPluginNames() const;
    
    // Initialize all plugins
    bool initializeAllPlugins(const nlm::Config& config);
    
    // Cleanup all plugins
    void cleanupAllPlugins();
    
    // Get plugin metadata
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        bool initialized;
        std::string status;
        std::string path;
        std::vector<std::string> supportedTestTypes;
    };
    
    std::vector<PluginInfo> getPluginInfo() const;
    
    // Save plugin state
    bool savePluginState(const std::string& stateFile) const;
    
    // Load plugin state
    bool loadPluginState(const std::string& stateFile);
    
    // Export plugin configuration
    bool exportPluginConfiguration(const std::string& configFile) const;
    
    // Import plugin configuration
    bool importPluginConfiguration(const std::string& configFile);
    
private:
    std::map<std::string, std::shared_ptr<ITestPlugin>> plugins_;
    std::map<std::string, std::shared_ptr<ITestFactory>> factories_;
    std::map<std::string, std::string> pluginPaths_;
    std::map<std::string, bool> pluginInitialized_;
    
    std::string discoverPluginPath(const std::string& pluginName);
    bool loadPluginFromPath(const std::string& pluginPath);
    std::string generatePluginName(const std::string& pluginPath);
};

// Configuration-driven test execution
class ConfigurationDrivenTestRunner {
public:
    ConfigurationDrivenTestRunner(std::shared_ptr<PluginManager> pluginManager);
    ~ConfigurationDrivenTestRunner() = default;
    
    // Execute test based on configuration
    bool executeTestFromConfig(const nlm::Config& config, 
                              std::shared_ptr<nlm::Brain> brain);
    
    // Execute all tests from configuration
    bool executeAllTestsFromConfig(const nlm::Config& config,
                                  std::shared_ptr<nlm::Brain> brain);
    
    // Execute test with specific configuration
    bool executeTest(const std::string& testType, 
                    const nlm::Config& config,
                    std::shared_ptr<nlm::Brain> brain);
    
    // Get test configuration for specific test type
    nlm::Config getTestConfiguration(const std::string& testType,
                                    const nlm::Config& baseConfig) const;
    
    // Validate configuration for test execution
    bool validateTestConfiguration(const std::string& testType,
                                  const nlm::Config& config) const;
    
    // Get default configuration for test type
    nlm::Config getDefaultConfiguration(const std::string& testType) const;
    
    // Get configuration schema for test type
    std::string getConfigurationSchema(const std::string& testType) const;
    
    // Set configuration factory
    void setConfigurationFactory(std::function<nlm::Config(const std::string&)> factory) {
        configFactory_ = factory;
    }
    
    // Get configuration factory
    std::function<nlm::Config(const std::string&)> getConfigurationFactory() const {
        return configFactory_;
    }
    
    // Register configuration factory
    void registerConfigurationFactory(const std::string& testType,
                                     std::function<nlm::Config(const nlm::Config&)> factory) {
        configFactories_[testType] = factory;
    }
    
    // Apply configuration to brain
    bool applyConfigurationToBrain(const nlm::Config& config, 
                                 std::shared_ptr<nlm::Brain> brain);
    
    // Get execution statistics
    struct ExecutionStats {
        std::string testType;
        double executionTime;
        bool success;
        std::string errorMessage;
        size_t neuronsProcessed;
        size_t spikesGenerated;
        std::map<std::string, double> performanceMetrics;
    };
    
    std::vector<ExecutionStats> getExecutionStatistics() const {
        return executionStats_;
    }
    
    // Clear execution statistics
    void clearExecutionStatistics() {
        executionStats_.clear();
    }
    
private:
    std::shared_ptr<PluginManager> pluginManager_;
    std::function<nlm::Config(const std::string&)> configFactory_;
    std::map<std::string, std::function<nlm::Config(const nlm::Config&)>> configFactories_;
    std::vector<ExecutionStats> executionStats_;
    
    bool executeSingleTest(const std::string& testType,
                          const nlm::Config& config,
                          std::shared_ptr<nlm::Brain> brain,
                          ExecutionStats& stats);
};

// Dependency injection container for test components
class TestComponentContainer {
public:
    TestComponentContainer();
    ~TestComponentContainer() = default;
    
    // Register component factory
    template<typename T>
    void registerComponent(const std::string& name, std::function<std::shared_ptr<T>()> factory) {
        componentFactories_[std::type_index(typeid(T))][name] = factory;
    }
    
    // Get component
    template<typename T>
    std::shared_ptr<T> getComponent(const std::string& name) {
        auto typeIndex = std::type_index(typeid(T));
        if (componentFactories_.find(typeIndex) != componentFactories_.end()) {
            auto& factories = componentFactories_[typeIndex];
            if (factories.find(name) != factories.end()) {
                return std::dynamic_pointer_cast<T>(factories[name]());
            }
        }
        return nullptr;
    }
    
    // Register brain factory
    void registerBrainFactory(std::function<std::shared_ptr<nlm::Brain>(const nlm::Config&)> factory) {
        brainFactory_ = factory;
    }
    
    // Get brain
    std::shared_ptr<nlm::Brain> getBrain(const nlm::Config& config) {
        if (brainFactory_) {
            return brainFactory_(config);
        }
        return nullptr;
    }
    
    // Register configuration provider
    void registerConfigurationProvider(std::function<nlm::Config()> provider) {
        configProvider_ = provider;
    }
    
    // Get configuration
    nlm::Config getConfiguration() {
        if (configProvider_) {
            return configProvider_();
        }
        return nlm::Config();
    }
    
    // Register test runner
    void registerTestRunner(std::function<bool(const std::string&, std::shared_ptr<nlm::Brain>)> runner) {
        testRunner_ = runner;
    }
    
    // Get test runner
    std::function<bool(const std::string&, std::shared_ptr<nlm::Brain>)> getTestRunner() const {
        return testRunner_;
    }
    
    // Execute test with dependency injection
    bool executeTestWithInjection(const std::string& testType, const nlm::Config& config) {
        if (!testRunner_) {
            return false;
        }
        
        auto brain = getBrain(config);
        if (!brain) {
            return false;
        }
        
        if (!brain->initialize()) {
            return false;
        }
        
        return testRunner_(testType, brain);
    }
    
private:
    std::map<std::type_index, std::map<std::string, std::function<void*()>>> componentFactories_;
    std::function<std::shared_ptr<nlm::Brain>(const nlm::Config&)> brainFactory_;
    std::function<nlm::Config()> configProvider_;
    std::function<bool(const std::string&, std::shared_ptr<nlm::Brain>)> testRunner_;
};

// Hot-swappable test module interface
class ITestModule {
public:
    virtual ~ITestModule() = default;
    
    // Get module name
    virtual std::string getModuleName() const = 0;
    
    // Get module version
    virtual std::string getModuleVersion() const = 0;
    
    // Initialize module
    virtual bool initialize(const nlm::Config& config) = 0;
    
    // Clean up module
    virtual void cleanup() = 0;
    
    // Reload module
    virtual bool reload(const nlm::Config& config) = 0;
    
    // Execute module functionality
    virtual bool execute(const std::string& operation, const nlm::Config& config) = 0;
    
    // Get module status
    virtual std::string getStatus() const = 0;
    
    // Get module configuration
    virtual nlm::Config getConfiguration() const = 0;
    
    // Set module configuration
    virtual void setConfiguration(const nlm::Config& config) = 0;
    
    // Check if module supports operation
    virtual bool supportsOperation(const std::string& operation) const = 0;
    
    // Get supported operations
    virtual std::vector<std::string> getSupportedOperations() const = 0;
};

// Hot-swappable module manager
class HotSwappableModuleManager {
public:
    HotSwappableModuleManager();
    ~HotSwappableModuleManager() = default;
    
    // Load module
    bool loadModule(const std::string& moduleName, std::shared_ptr<ITestModule> module);
    
    // Unload module
    bool unloadModule(const std::string& moduleName);
    
    // Reload module
    bool reloadModule(const std::string& moduleName, const nlm::Config& config);
    
    // Get module
    std::shared_ptr<ITestModule> getModule(const std::string& moduleName);
    
    // Get all modules
    std::vector<std::shared_ptr<ITestModule>> getAllModules() const;
    
    // Execute module operation
    bool executeModuleOperation(const std::string& moduleName,
                               const std::string& operation,
                               const nlm::Config& config);
    
    // Execute module operation on all modules
    std::vector<bool> executeModuleOperationAll(const std::string& operation,
                                               const nlm::Config& config);
    
    // Get module names
    std::vector<std::string> getModuleNames() const;
    
    // Check if module exists
    bool hasModule(const std::string& moduleName) const;
    
    // Initialize all modules
    bool initializeAllModules(const nlm::Config& config);
    
    // Cleanup all modules
    void cleanupAllModules();
    
    // Get module status
    std::string getModuleStatus(const std::string& moduleName) const;
    
    // Get all module statuses
    std::map<std::string, std::string> getAllModuleStatuses() const;
    
    // Export module state
    bool exportModuleState(const std::string& stateFile) const;
    
    // Import module state
    bool importModuleState(const std::string& stateFile);
    
    // Save module configuration
    bool saveModuleConfiguration(const std::string& configFile) const;
    
    // Load module configuration
    bool loadModuleConfiguration(const std::string& configFile);
    
private:
    std::map<std::string, std::shared_ptr<ITestModule>> modules_;
    std::map<std::string, bool> moduleInitialized_;
    std::map<std::string, nlm::Config> moduleConfigurations_;
    
    std::string generateModulePath(const std::string& moduleName);
    bool loadModuleFromPath(const std::string& modulePath, const std::string& moduleName);
};
