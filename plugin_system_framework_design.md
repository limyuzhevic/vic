Plugin System Framework Design Document

# NLM Plugin System Framework

## Overview

The NLM Plugin System provides a modular architecture for extending and customizing the NLM brain simulation. It enables runtime loading of plugins that can provide services, modify behavior, or extend functionality without requiring core system modifications.

## Architecture Overview

The plugin system follows the existing NLM architectural patterns:

```
Core Architecture:
├── PluginSystem (Central Coordinator)
├── Plugin Interface
├── Service Registry
├── Plugin Loading Mechanism
└── Execution Engine

Integration Points:
├── Brain Integration (runtime loading/unloading)
├── Service-based Communication
├── Template-based Development
└── Extension Points for Plugin Execution
```

## Class Design

### 1. PluginSystem Class

**Public Methods:**
- `PluginSystem(std::shared_ptr<Config> config)` - Constructor
- `~PluginSystem()` - Destructor
- `bool initialize()` - Initialize plugin system
- `bool loadPlugin(const std::string& pluginPath)` - Load a plugin from file
- `bool unloadPlugin(const std::string& pluginName)` - Unload a plugin
- `bool enablePlugin(const std::string& pluginName)` - Enable a plugin
- `bool disablePlugin(const std::string& pluginName)` - Disable a plugin
- `bool isPluginLoaded(const std::string& pluginName) const` - Check if plugin is loaded
- `bool isPluginEnabled(const std::string& pluginName) const` - Check if plugin is enabled
- `std::vector<std::string> getLoadedPlugins() const` - Get list of loaded plugins
- `std::vector<std::string> getEnabledPlugins() const` - Get list of enabled plugins
- `std::vector<ServiceInfo> getAvailableServices() const` - Get all available services
- `template<typename T> std::shared_ptr<T> getService(const std::string& serviceName)` - Get a service by type
- `template<typename T> bool callService(const std::string& serviceName, const T& request, T& response)` - Call a service
- `bool executePlugin(const std::string& pluginName, const ExecutionContext& context)` - Execute a plugin
- `bool generatePluginTemplate(const std::string& templateType, const std::string& outputPath)` - Generate plugin template
- `void setBrainReference(Brain* brain)` - Set reference to the brain for integration

**Private Methods:**
- `bool validatePluginSignature(const std::string& pluginPath)` - Validate plugin integrity
- `void cleanupPluginResources(const std::string& pluginName)` - Cleanup plugin resources
- `void updatePluginState(const std::string& pluginName, PluginState newState)` - Update plugin state
- `void broadcastPluginEvent(const PluginEvent& event)` - Broadcast events to plugins

**Data Members:**
- `std::unordered_map<std::string, std::unique_ptr<Plugin>> loadedPlugins_` - Loaded plugins
- `std::unordered_set<std::string> enabledPlugins_` - Enabled plugins
- `std::unordered_map<std::string, std::unique_ptr<Service>> serviceRegistry_` - Service registry
- `std::unordered_map<std::string, PluginMetadata> pluginMetadata_` - Plugin metadata
- `std::unordered_map<std::string, PluginState> pluginStates_` - Plugin states
- `std::shared_ptr<Config> config_` - Configuration
- `Brain* brain_` - Reference to the brain
- `std::vector<PluginLoadListener> loadListeners_` - Plugin load listeners
- `std::vector<PluginEventListener> eventListeners_` - Event listeners

### 2. Plugin Class

**Public Methods:**
- `virtual ~Plugin()` - Destructor
- `virtual const PluginMetadata& getMetadata() const` - Get plugin metadata
- `virtual bool initialize()` - Initialize plugin
- `virtual void shutdown()` - Shutdown plugin
- `virtual bool isInitialized() const` - Check if plugin is initialized
- `virtual std::vector<ServiceInfo> getProvidedServices() const` - Get provided services
- `virtual bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response)` - Handle service request
- `virtual bool onPluginEvent(const PluginEvent& event)` - Handle plugin events
- `virtual bool execute(const ExecutionContext& context)` - Execute plugin functionality
- `virtual void onBrainUpdate(const BrainUpdateEvent& event)` - Handle brain updates
- `virtual std::string getName() const` - Get plugin name
- `virtual std::string getVersion() const` - Get plugin version

**Protected Methods:**
- `void registerService(const ServiceInfo& serviceInfo)` - Register a service (called by plugin)
- `void unregisterService(const std::string& serviceName)` - Unregister a service
- `void emitEvent(const PluginEvent& event)` - Emit an event

### 3. Service Class

**Public Methods:**
- `virtual ~Service()` - Destructor
- `virtual const ServiceInfo& getInfo() const` - Get service information
- `virtual bool initialize()` - Initialize service
- `virtual void shutdown()` - Shutdown service
- `virtual bool isInitialized() const` - Check if service is initialized
- `virtual bool handleRequest(const ServiceRequest& request, ServiceResponse& response)` - Handle service request
- `virtual std::string getName() const` - Get service name
- `virtual ServiceType getType() const` - Get service type

### 4. PluginMetadata Structure

```cpp
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string pluginPath;
    std::vector<std::string> dependencies;
    bool requiresBrain;
    bool autoEnable;
    PluginType type;
    std::map<std::string, std::string> metadata;
    
    PluginMetadata()
        : version("1.0.0")
        , author("Unknown")
        , requiresBrain(false)
        , autoEnable(false)
        , type(PluginType::General) {}
};
```

### 5. ServiceInfo Structure

```cpp
struct ServiceInfo {
    std::string name;
    std::string description;
    ServiceType type;
    std::type_index interfaceType;
    bool isSingleton;
    std::map<std::string, std::string> metadata;
    
    ServiceInfo()
        : isSingleton(false)
        , type(ServiceType::Unknown) {}
};
```

### 6. ExecutionContext Structure

```cpp
struct ExecutionContext {
    std::string pluginName;
    std::string operation;
    std::map<std::string, std::string> arguments;
    std::map<std::string, std::any> typedArguments;
    std::string targetComponent;
    Timestamp timestamp;
    std::string source;
    std::map<std::string, std::any> result;
    
    ExecutionContext()
        : timestamp(0.0) {}
};
```

### 7. PluginEvent Structure

```cpp
struct PluginEvent {
    PluginEventType type;
    std::string pluginName;
    std::map<std::string, std::any> data;
    Timestamp timestamp;
    
    PluginEvent()
        : timestamp(0.0) {}
};
```

### 8. ServiceRequest/ServiceResponse Structures

```cpp
struct ServiceRequest {
    std::string serviceName;
    std::string operation;
    std::map<std::string, std::any> parameters;
    Timestamp timestamp;
    std::string requesterId;
    
    ServiceRequest()
        : timestamp(0.0) {}
};

struct ServiceResponse {
    bool success;
    std::map<std::string, std::any> data;
    std::string errorMessage;
    Timestamp timestamp;
    
    ServiceResponse()
        : success(false)
        , timestamp(0.0) {}
};
```

## Plugin Management Approach

### Load/Unload Process

1. **Validation**: Check plugin signature, dependencies, and metadata
2. **Resource Allocation**: Allocate memory and resources
3. **Initialization**: Call plugin's initialization method
4. **Service Registration**: Register provided services with the plugin system
5. **State Update**: Update plugin state to 'Loaded'
6. **Event Notification**: Notify listeners of successful load

2. **Unload Process**:
   1. **Shutdown**: Call plugin's shutdown method
   2. **Resource Cleanup**: Free allocated resources
   3. **Service Unregistration**: Remove plugin's services
   4. **State Update**: Update plugin state to 'Unloaded'
   5. **Event Notification**: Notify listeners of successful unload

### Enable/Disable Process

1. **Enable**:
    - Enable plugin by adding to enabled set
    - Call plugin's enable handler
    - Notify listeners of enable event

2. **Disable**:
    - Remove plugin from enabled set
    - Call plugin's disable handler
    - Notify listeners of disable event

## Plugin Development Approach

### Plugin Templates

1. **Basic Plugin Template**:
```cpp
// BasicPlugin.hpp
#pragma once
#include "../Plugin.hpp"

class BasicPlugin : public Plugin {
public:
    virtual const PluginMetadata& getMetadata() const override;
    virtual bool initialize() override;
    virtual void shutdown() override;
    virtual std::vector<ServiceInfo> getProvidedServices() const override;
    virtual bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) override;
    virtual bool execute(const ExecutionContext& context) override;
};

// BasicPlugin.cpp
#include "BasicPlugin.hpp"
#include "../PluginSystem.hpp"

const PluginMetadata& BasicPlugin::getMetadata() const {
    static PluginMetadata metadata = {
        .name = "BasicPlugin",
        .version = "1.0.0",
        .description = "A basic plugin template",
        .author = "NLM Team",
        .requiresBrain = false,
        .autoEnable = false
    };
    return metadata;
}

// ... implementation
```

2. **Service Plugin Template**:
```cpp
// ServicePlugin.hpp
#pragma once
#include "../Plugin.hpp"
#include "../Service.hpp"

class SampleServicePlugin : public Plugin {
private:
    std::unique_ptr<Service> sampleService_;
public:
    virtual bool initialize() override;
    virtual std::vector<ServiceInfo> getProvidedServices() const override;
};
```

### Code Generation

1. **Plugin Generator Class**: Handles template generation
2. **Supported Templates**:
   - Basic Plugin (service, non-service)
   - Service Plugin
   - Brain Extension Plugin
   - Event Handler Plugin
   - Data Processor Plugin

## Plugin Execution Approach

### Execution Process

1. **Validation**: Validate execution context and plugin state
2. **Preparation**: Prepare execution environment
3. **Execution**: Call plugin's execute method
4. **Result Collection**: Collect and store results
5. **Logging**: Log execution details
6. **Error Handling**: Handle execution errors

### Argument Handling

1. **String Arguments**: Standard string-based arguments
2. **Typed Arguments**: Type-safe arguments using std::any
3. **Component Targets**: Specify target components for execution
4. **Execution Context**: Include timestamp, source, and metadata

## File Structure and Dependencies

```
/plugin_system/
├── include/
│   ├── PluginSystem.hpp
│   ├── Plugin.hpp
│   ├── Service.hpp
│   ├── PluginTypes.hpp
│   └── PluginExceptions.hpp
├── src/
│   ├── PluginSystem.cpp
│   ├── Plugin.cpp
│   ├── Service.cpp
│   ├── PluginLoader.cpp
│   ├── ServiceRegistry.cpp
│   └── ExecutionEngine.cpp
├── plugins/
│   ├── examples/
│   │   ├── basic_plugin/
│   │   │   ├── BasicPlugin.cpp
│   │   │   └── BasicPlugin.hpp
│   │   ├── service_plugin/
│   │   │   ├── SampleServicePlugin.cpp
│   │   │   └── SampleServicePlugin.hpp
│   │   └── brain_extension/
│   │       ├── PerceptionEnhancer.cpp
│   │       └── PerceptionEnhancer.hpp
├── templates/
│   ├── basic_plugin.template
│   ├── service_plugin.template
│   └── brain_extension.template
└── generated/
    ├── README.md
    └── plugin_index.json
```

### Dependencies

1. **Core Dependencies**:
   - nlm::core (Config, Logger, Types)
   - nlm::brain (Brain integration)

2. **Optional Dependencies**:
   - Platform-specific (Windows, Linux, macOS)
   - Plugin format (shared library, static library, dynamic loading)

## Implementation Approach

### Plugin Loading and Service Registration

1. **Dynamic Library Loading**:
   - Use platform-specific dynamic loading APIs
   - Support for both shared libraries (.so/.dll) and static libraries
   - Plugin symbol exports (e.g., `createPlugin`, `getPluginVersion`)

2. **Service Registration Process**:
   - Plugins register services during initialization
   - Service names are globally unique
   - Multiple implementations of the same service can coexist
   - Service type checking using `std::type_index`

3. **Brain Integration**:
   - PluginSystem holds a weak reference to Brain
   - Plugins can access Brain methods and data
   - Brain can call plugin services during its execution
   - Plugin events are propagated to Brain lifecycle events

4. **Thread Safety**:
   - Plugin operations are thread-safe
   - Service calls are synchronized
   - Event emission is thread-safe

### Error Handling

1. **Plugin Errors**:
   - PluginLoadException for loading errors
   - PluginInitializationException for initialization errors
   - PluginExecutionException for execution errors

2. **Service Errors**:
   - ServiceNotFoundException
   - ServiceInvocationException
   - ServiceInvalidRequestException

3. **System Errors**:
   - PluginSystemException for system-level errors
   - ConfigurationException for configuration errors

## Plugin Development Best Practices

1. **Plugin Safety**:
   - Validate all inputs
   - Handle exceptions gracefully
   - Cleanup resources in destructor
   - Avoid static initialization order issues

2. **Performance Considerations**:
   - Minimize logging overhead
   - Cache frequently accessed data
   - Use efficient data structures
   - Profile plugin performance

3. **Testing**:
   - Write unit tests for plugins
   - Test plugin loading/unloading
   - Test service integration
   - Test plugin execution with various inputs

## Example Plugin Usage

```cpp
// In main application code
void ApplicationMain() {
    auto pluginSystem = std::make_unique<PluginSystem>(config);
    pluginSystem->initialize();
    
    // Load a plugin
    if (pluginSystem->loadPlugin("./plugins/basic_plugin.dll")) {
        pluginSystem->enablePlugin("BasicPlugin");
    }
    
    // Use plugin service
    SomeData request;
    SomeResponse response;
    pluginSystem->callService<SomeService>("BasicService", request, response);
    
    // Execute plugin
    ExecutionContext context;
    context.pluginName = "BasicPlugin";
    context.operation = "processData";
    context.arguments["input"] = "test_data";
    pluginSystem->executePlugin("BasicPlugin", context);
}
```

## Integration with NLM System

The plugin system integrates with the NLM system through:

1. **Brain Integration**: PluginSystem can be initialized with a Brain reference
2. **Service Callbacks**: Brain can call plugin services during execution
3. **Event Propagation**: Plugin events can be triggered by Brain events
4. **Dynamic Extension**: New functionality can be added without recompiling core system

This design provides a robust, extensible plugin system that follows NLM's architectural principles while meeting all specified requirements.