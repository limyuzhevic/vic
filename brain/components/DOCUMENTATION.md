# BrainComponentFactory System Documentation

## Overview

The BrainComponentFactory system provides a comprehensive, thread-safe framework for creating, managing, and optimizing neural network components in the NLM project. This system enables configuration-driven component creation with full support for dependency injection, cloning, serialization, and lifecycle management.

## Key Components

### 1. BrainComponentFactory (Abstract Base Class)

The abstract factory interface for creating brain components:

```cpp
class BrainComponentFactory {
public:
    virtual ~BrainComponentFactory() = default;
    
    // Create component from configuration
    virtual std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::string& componentId = "") = 0;
    
    // Create component with dependencies
    virtual std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
        const std::string& componentId = "") = 0;
    
    // Create component from template
    virtual std::unique_ptr<BrainComponent> createComponentFromTemplate(
        const std::string& templateName,
        const std::map<std::string, std::any>& parameters = {}) = 0;
    
    // Clone an existing component
    virtual std::unique_ptr<BrainComponent> cloneComponent(
        const BrainComponent& component) = 0;
    
    // Factory information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
};
```

### 2. BrainComponent (Abstract Base Class)

The abstract base class for all brain components:

```cpp
class BrainComponent {
public:
    virtual ~BrainComponent() = default;
    
    // Component identity
    virtual std::string getTypeName() const = 0;
    virtual std::string getComponentId() const = 0;
    virtual ComponentType getComponentType() const = 0;
    
    // Lifecycle management
    virtual bool initialize(const std::shared_ptr<Config>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Configuration management
    virtual void updateConfiguration(const std::shared_ptr<Config>& config) = 0;
    
    // Cloning support
    virtual std::unique_ptr<BrainComponent> clone() const = 0;
    virtual bool canClone() const = 0;
    
    // Serialization support
    virtual std::string serialize() const = 0;
    virtual bool deserialize(const std::string& data) = 0;
    virtual bool canSerialize() const = 0;
    
    // Validation
    virtual bool validateConfiguration(const std::shared_ptr<Config>& config) const = 0;
    
    // Dependency management
    virtual const std::vector<std::string>& getDependencies() const = 0;
    virtual const std::vector<std::string>& getDependents() const = 0;
    
    // Performance optimization
    virtual bool isOptimized() const = 0;
    virtual void optimize() = 0;
};
```

### 3. ComponentRegistry (Singleton)

Manages component type factories and templates:

```cpp
class ComponentRegistry {
public:
    static ComponentRegistry& getInstance();
    
    // Register factories and templates
    bool registerFactory(const std::string& factoryName,
                        std::unique_ptr<BrainComponentFactory> factory);
    
    bool registerComponentType(const std::string& componentType,
                              std::function<std::unique_ptr<BrainComponent>(const std::shared_ptr<Config>&)> factory,
                              const std::string& factoryName = "");
    
    bool registerTemplate(const ComponentTemplate& templateData);
    
    // Component lookup
    std::unique_ptr<BrainComponentFactory> getFactory(const std::string& componentType) const;
    const ComponentTemplate* getTemplate(const std::string& templateName) const;
    
    // Query support
    bool isComponentTypeSupported(const std::string& componentType) const;
    std::vector<std::string> getSupportedComponentTypes() const;
    std::vector<std::string> getFactoryNames() const;
    
    // Management
    void clear();
};
```

### 4. BrainComponentManager

Manages component lifecycle and configuration:

```cpp
class BrainComponentManager {
public:
    BrainComponentManager();
    ~BrainComponentManager();
    
    // Component creation
    std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::string& componentId = "");
    
    std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
        const std::string& componentId = "");
    
    // Lifecycle management
    bool initializeComponent(BrainComponent& component);
    bool shutdownComponent(BrainComponent& component);
    bool updateComponentConfiguration(BrainComponent& component, const std::shared_ptr<Config>& config);
    
    // Component lifecycle tracking
    bool registerComponent(std::unique_ptr<BrainComponent> component);
    bool unregisterComponent(const std::string& componentId);
    std::shared_ptr<BrainComponent> getComponent(const std::string& componentId) const;
    std::vector<std::shared_ptr<BrainComponent>> getAllComponents() const;
};
```

### 5. BrainFactory

Specialized factory for creating complete brain instances:

```cpp
class BrainFactory {
public:
    BrainFactory();
    
    // Brain creation
    std::unique_ptr<Brain> createBrain(const std::shared_ptr<Config>& config);
    std::unique_ptr<Brain> createBrainFromTemplate(
        const std::string& templateName,
        const std::map<std::string, std::any>& parameters = {});
    
    // Brain component integration
    bool addComponentToBrainConfig(std::shared_ptr<Config>& brainConfig,
                                  const std::string& componentType,
                                  const std::map<std::string, std::any>& parameters = {});
    
    // Brain component templates
    std::vector<std::string> getBrainComponentTemplates() const;
    bool validateBrainConfiguration(const std::shared_ptr<Config>& config) const;
};
```

## Component Templates

Component templates allow for reusable component definitions:

```cpp
struct ComponentTemplate {
    std::string name;
    std::string description;
    std::string componentType;
    std::shared_ptr<Config> defaultConfig;
    std::map<std::string, std::any> prototypeParameters;
    bool isCloneable = true;
    bool isSerializable = true;
    std::vector<std::string> dependencies;
    std::map<std::string, std::any> metadata;
};
```

Built-in templates:
- **DefaultNeuronTemplate**: Standard neuron with default LIF parameters
- **DefaultSynapseTemplate**: Standard synapse with excitatory/inhibitory types
- **DefaultPopulationTemplate**: Neural population configuration
- **DefaultRegionTemplate**: Neural region configuration

## Configuration-Driven Creation

Components are created based on configuration objects:

```cpp
// Create neuron from configuration
auto neuron = manager.createComponent("Neuron", config, "neuron_1");

// Create synapse with dependencies
auto synapse = manager.createComponent("Synapse", config, dependencies);

// Create from template
auto component = factory.createComponentFromTemplate("DefaultNeuronTemplate", params);
```

## Thread Safety

All factory operations are thread-safe:

```cpp
// ComponentRegistry thread safety
registry.lock();
// ... registry operations ...
registry.unlock();

// BrainComponentManager thread safety
manager.lock();
// ... manager operations ...
manager.unlock();
```

## Error Handling

The system uses NLMException-based error handling:

```cpp
class BrainComponentFactoryException : public NLMException {
public:
    enum class FactoryErrorCode {
        CreationFailed = 1000,
        ComponentNotFound = 1001,
        InvalidConfiguration = 1002,
        ComponentAlreadyRegistered = 1003,
        ComponentDependencyError = 1004,
        SerializationFailed = 1005,
        ThreadSafetyViolation = 1006,
        CloneFailed = 1007
    };
};
```

## Integration with ConfigurationManager

The factory system integrates seamlessly with the ConfigurationManager:

```cpp
// Load configuration from file
ConfigurationManager configManager;
if (configManager.loadFromFile("brain_config.json")) {
    // Create components using loaded configuration
    auto neuron = manager.createComponent("Neuron", configManager.getOr("neuron_config", std::shared_ptr<Config>()));
    
    // Create brain with configuration
    auto brain = brainFactory.createBrain(configManager.getGlobal());
}
```

## Performance Optimization

The system includes performance optimization features:

```cpp
// Optimize all components
manager.optimizeAllComponents();

// Clean up inactive components
manager.cleanupInactiveComponents();
```

## Serialization Support

Components can be serialized and deserialized:

```cpp
// Serialize component
std::string data = manager.serializeComponent(*neuron);

// Deserialize component
auto deserialized = manager.deserializeComponent(data);
```

## Usage Example

Here's a complete example of using the BrainComponentFactory system:

```cpp
#include "brain/components/BrainComponentFactory.hpp"
#include "core/Config/ConfigurationManager.hpp"

int main() {
    try {
        // Initialize configuration
        auto& configManager = ConfigurationManager::getGlobal();
        configManager.loadFromFile("brain_config.json");
        
        // Get component manager
        auto& manager = BrainComponentManager();
        
        // Create components from configuration
        auto neuron = manager.createComponent("Neuron", 
            configManager.getOr("neuron_config", std::shared_ptr<Config>()), 
            "neuron_1");
        
        auto synapse = manager.createComponent("Synapse",
            configManager.getOr("synapse_config", std::shared_ptr<Config>()),
            "synapse_1");
        
        // Create brain with integrated components
        auto brain = BrainFactory().createBrain(configManager.getGlobal());
        
        // Clone components
        auto clonedNeuron = manager.cloneComponent(*neuron);
        
        // Serialize and deserialize
        std::string data = manager.serializeComponent(*neuron);
        auto deserialized = manager.deserializeComponent(data);
        
        // Optimize components
        manager.optimizeAllComponents();
        
        return 0;
        
    } catch (const NLMException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

## Key Benefits

1. **Configuration-Driven**: Components are created based on configuration, enabling flexible setup
2. **Thread-Safe**: All operations are thread-safe for concurrent access
3. **Extensible**: Easy to add new component types and factories
4. **Optimized**: Built-in performance optimization and lifecycle management
5. **Integrated**: Seamless integration with ConfigurationManager
6. **Robust**: Comprehensive error handling and validation
7. **Reusable**: Component templates and cloning support reusability
8. **Serializable**: Components can be saved and restored
9. **Managed**: Complete lifecycle management with cleanup and optimization
10. **Production-Ready**: Designed for high-performance neural network applications

## Files

- `BrainComponentFactory.hpp`: Abstract factory interfaces and component classes
- `BrainComponentFactory.cpp`: Implementation of factory system
- `TestBrainComponentFactory.cpp`: Test application demonstrating system usage

## Building and Running Tests

```bash
# Build the system
cd /workspace/app
mkdir -p build
cd build
cmake ..
make

# Run tests
./TestBrainComponentFactory
```

## Dependencies

- nlohmann/json (for serialization)
- Standard C++17 libraries
- NLM core libraries (Brain, Synapse, NeuralPopulation, NeuralRegion, etc.)
- ConfigurationManager
- NLMErrors (exception handling)

## License

This system is part of the NLM (Neural Learning Machine) project.
See LICENSE file for details.
