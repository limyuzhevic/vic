# Plugin Architecture for NLM

This document describes the plugin system for extending the NLM (Neural Learning Machine) framework with custom components.

## Overview

The NLM plugin architecture allows users to extend the framework with custom components without modifying the core codebase. This enables:

- Custom sensor implementations
- Specialized neural network architectures  
- Alternative world simulation models
- Custom neuromodulation systems
- Domain-specific learning algorithms
- Performance optimizations
- Integration with external systems

## Plugin Types

### 1. Sensor Plugins
Extend or add new sensory modalities (vision, touch, internal, proprioception, audio).

### 2. Neural Network Plugins
Implement custom brain architectures, neuron types, or synaptic models.

### 3. World Plugins
Create alternative world simulation models with different physics, environments, or dynamics.

### 4. Neuromodulation Plugins
Implement custom neuromodulation systems (dopamine, curiosity, novelty, prediction error).

### 5. Memory System Plugins
Add specialized memory systems or modify existing memory architectures.

### 6. Learning Algorithm Plugins
Implement custom plasticity rules, learning algorithms, or training methods.

### 7. Performance Plugins
Add performance optimizations, parallel processing, or hardware-specific implementations.

## Plugin Interface

### Core Plugin Interface

```cpp
// Base plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};
```

### Sensor Plugin Interface

```cpp
// Sensor plugin interface for custom sensory modalities
class ISensorPlugin : public IPlugin {
public:
    // Sensor data structure
    struct SensorData {
        std::vector<float> values;
        std::string name;
        double timestamp;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    // Core sensor methods
    virtual SensorData read() = 0;
    virtual size_t getExpectedSize() const = 0;
    virtual std::string getSensorType() const = 0;
    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;
};
```

### Neural Network Plugin Interface

```cpp
// Neural network plugin interface
class INeuralNetworkPlugin : public IPlugin {
public:
    // Neuron state
    struct NeuronState {
        double membranePotential;
        double restingPotential;
        double threshold;
        double refractoryTime;
        std::vector<double> synapticWeights;
        std::vector<double> eligibilityTraces;
    };
    
    // Core neural network methods
    virtual std::vector<NeuronState> process(const std::vector<NeuronState>& input) = 0;
    virtual void reset() = 0;
    virtual size_t getNeuronCount() const = 0;
    virtual std::string getArchitecture() const = 0;
    virtual double getLearningRate() const = 0;
    virtual void setLearningRate(double rate) = 0;
};
```

### World Plugin Interface

```cpp
// World plugin interface
class IWorldPlugin : public IPlugin {
public:
    // Agent state
    struct AgentState {
        double x;
        double y;
        double orientation;
        double energy;
        double health;
        double age;
        bool isMoving;
        bool isTurning;
    };
    
    // Core world methods
    virtual void update(double timestep) = 0;
    virtual AgentState getAgentState() const = 0;
    virtual std::vector<double> getSensoryInput() const = 0;
    virtual void applyAction(const std::string& action, double timestep) = 0;
    virtual double getSimulationTime() const = 0;
    virtual void reset() = 0;
    virtual std::string getWorldType() const = 0;
};
```

## Plugin Manager

The PluginManager class coordinates all plugins in the system:

```cpp
class PluginManager {
public:
    // Plugin registration
    static PluginManager& getInstance();
    
    // Plugin discovery
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    
    // Plugin control
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool isPluginEnabled(const std::string& pluginName) const;
    
    // Plugin access
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName);
    std::vector<std::shared_ptr<ISensorPlugin>> getSensorPlugins() const;
    std::vector<std::shared_ptr<INeuralNetworkPlugin>> getNeuralNetworkPlugins() const;
    std::vector<std::shared_ptr<IWorldPlugin>> getWorldPlugins() const;
    
    // Configuration
    bool configurePlugin(const std::string& pluginName, const Config& config);
    Config getPluginConfig(const std::string& pluginName) const;
    
    // Lifecycle management
    void initializeAllPlugins(const Config& globalConfig);
    void shutdownAllPlugins();
    bool isPluginLoaded(const std::string& pluginName) const;
    
private:
    PluginManager();
    ~PluginManager();
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
};
```

## Plugin Development Guide

### 1. Creating a Custom Sensor Plugin

```cpp
class CustomVisionPlugin : public ISensorPlugin {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    CustomVisionPlugin();
    ~CustomVisionPlugin() override;
    
    std::string getName() const override { return "CustomVision"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Custom vision sensor with enhanced resolution and spectral sensitivity"; 
    }
    
    bool initialize(const Config& config) override;
    void shutdown() override;
    
    SensorData read() override;
    size_t getExpectedSize() const override { return 512 * 512; }  // 512x512 resolution
    std::string getSensorType() const override { return "vision"; }
    bool isActive() const override;
    void setActive(bool active) override;
};
```

### 2. Creating a Custom Neural Network Plugin

```cpp
class CustomCNNPlugin : public INeuralNetworkPlugin {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    CustomCNNPlugin();
    ~CustomCNNPlugin() override;
    
    std::string getName() const override { return "CustomCNN"; }
    std::string getVersion() const override { return "2.1.0"; }
    std::string getDescription() const override {
        return "Convolutional neural network optimized for visual processing"; 
    }
    
    bool initialize(const Config& config) override;
    void shutdown() override;
    
    std::vector<NeuronState> process(const std::vector<NeuronState>& input) override;
    void reset() override;
    size_t getNeuronCount() const override;
    std::string getArchitecture() const override { return "CNN"; }
    double getLearningRate() const override;
    void setLearningRate(double rate) override;
    
    // Custom CNN methods
    void setFilterSize(int size);
    void setPoolSize(int size);
    void enableBatchNormalization(bool enable);
};
```

### 3. Creating a Custom World Plugin

```cpp
class CustomPhysicsWorldPlugin : public IWorldPlugin {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    CustomPhysicsWorldPlugin();
    ~CustomPhysicsWorldPlugin() override;
    
    std::string getName() const override { return "CustomPhysicsWorld"; }
    std::string getVersion() const override { return "1.3.0"; }
    std::string getDescription() const override {
        return "World simulation with advanced physics and dynamics"; 
    }
    
    bool initialize(const Config& config) override;
    void shutdown() override;
    
    void update(double timestep) override;
    AgentState getAgentState() const override;
    std::vector<double> getSensoryInput() const override;
    void applyAction(const std::string& action, double timestep) override;
    double getSimulationTime() const override;
    void reset() override;
    std::string getWorldType() const override { return "physics"; }
    
    // Custom physics methods
    void setGravity(double gx, double gy);
    void setFriction(double coefficient);
    void enableCollisionDetection(bool enable);
    void setTimeStep(double dt);
};
```

## Plugin Configuration

Plugins are configured through the main NLM configuration system:

```ini
[plugins]
# Enable custom plugins
plugin.custom_vision.enabled = true
plugin.custom_cnn.enabled = true
plugin.custom_physics.enabled = true

[plugins.custom_vision]
# Plugin-specific configuration
vision.resolution = 512
vision.spectral_sensitivity = true
vision.color_space = "rgb"

[plugins.custom_cnn]
# Neural network configuration
cnn.filters = 64
cnn.layers = 10
cnn.activation = "relu"
cnn.learning_rate = 0.001

[plugins.custom_physics]
# Physics simulation configuration
physics.gravity_x = 0.0
physics.gravity_y = -9.8
physics.friction = 0.7
physics.collision_detection = true
```

## Plugin Loading and Initialization

### Loading Plugins at Runtime

```python
import pynlm

# Load custom plugins at startup
def load_custom_plugins():
    plugin_manager = pynlm.PluginManager.getInstance()
    
    # Load custom vision plugin
    if plugin_manager.loadPlugin("path/to/custom_vision_plugin.so"):
        plugin_manager.enablePlugin("custom_vision")
    
    # Load custom neural network plugin
    if plugin_manager.loadPlugin("path/to/custom_cnn_plugin.so"):
        plugin_manager.enablePlugin("custom_cnn")
    
    # Load custom world plugin
    if plugin_manager.loadPlugin("path/to/custom_physics_plugin.so"):
        plugin_manager.enablePlugin("custom_physics")

# Configure plugins
config = pynlm.createDefaultConfig()
config.set("plugin.custom_vision.resolution", 512)
config.set("plugin.custom_cnn.learning_rate", 0.001)
config.set("plugin.custom_physics.gravity_y", -9.8)

plugin_manager.configurePlugin("custom_vision", config);
plugin_manager.configurePlugin("custom_cnn", config);
plugin_manager.configurePlugin("custom_physics", config);
```

### Using Plugins in Simulations

```python
def run_plugin_enhanced_simulation():
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create enhanced world with custom plugins
    world = pynlm.createSimpleWorld()
    
    # Configure world to use custom plugins
    world.configure(
        width=100,
        height=100,
        visionWidth=64,
        visionHeight=64,
        useCustomVision=True,
        useCustomPhysics=True
    )
    
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print plugin-specific metrics
        if step % 100 == 0:
            custom_vision = pynlm.PluginManager.getInstance().getPlugin("custom_vision")
            if custom_vision:
                print(f"Custom Vision Active: {custom_vision.isEnabled()}")
            
            custom_physics = pynlm.PluginManager.getInstance().getPlugin("custom_physics")
            if custom_physics:
                print(f"Custom Physics Active: {custom_physics.isEnabled()}")
```

## Plugin Development Best Practices

### 1. Plugin Naming Conventions

- Use lowercase with underscores for plugin names
- Prefix domain-specific plugins with your organization or project name
- Avoid generic names that might conflict with built-in plugins

Examples:
- `custom_vision` (your custom vision sensor)
- `research_cnn_v2` (research team's CNN implementation)
- `mobile_optimization` (performance optimization for mobile devices)

### 2. Error Handling

Plugins should implement robust error handling:

```cpp
bool CustomPlugin::initialize(const Config& config) {
    try {
        // Initialize plugin
        // Load configuration
        // Setup internal state
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        Logger::getGlobal().error("Failed to initialize CustomPlugin: " + std::string(e.what()));
        return false;
    }
}
```

### 3. Resource Management

Plugins should properly manage system resources:

```cpp
void CustomPlugin::shutdown() {
    try {
        // Clean up resources
        // Save state if needed
        // Close file handles
        // Release memory
        
        Logger::getGlobal().info("CustomPlugin shutdown completed successfully");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Error during CustomPlugin shutdown: " + std::string(e.what()));
        // Continue with cleanup even if some operations fail
    }
}
```

### 4. Thread Safety

Plugins should be thread-safe if used in multi-threaded environments:

```cpp
SensorData CustomVisionPlugin::read() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Read sensor data
    // Update internal state
    // Apply any necessary filters or transformations
    
    return data;
}
```

## Plugin Marketplace

The NLM ecosystem includes a collection of community-developed plugins:

### Available Plugins

1. **Enhanced Vision Plugin**
   - Provides high-resolution vision (512x512)
   - Spectral sensitivity for different lighting conditions
   - Multi-modal vision (color, infrared, depth)

2. **Optimized CNN Plugin**
   - Deep convolutional neural networks
   - Batch normalization and dropout
   - Custom activation functions

3. **Advanced Physics Plugin**
   - Realistic physics simulation
   - Collision detection and response
   - Fluid dynamics integration

4. **Memory-Efficient Plugin**
   - Optimized memory usage for embedded systems
   - Quantized neural networks
   - Model compression techniques

5. **Real-time Plugin**
   - Ultra-low latency processing
   - Hardware acceleration support
   - Real-time inference capabilities

### Plugin Distribution

Plugins can be distributed through:

1. **Native Libraries**
   - Shared libraries (.so, .dll, .dylib)
   - Compiled against specific NLM version

2. **Python Bindings**
   - Python modules (.py, .so)
   - Easy integration with Python ecosystem

3. **Container Images**
   - Docker containers with plugins pre-installed
   - Easy deployment and scaling

4. **Package Managers**
   - NLM Package Manager (nlm-pkg)
   - pip (for Python plugins)
   - apt/yum (for system packages)

## Migration Guide

### From Version 0.1.x to 0.2.0

1. **Plugin Interface Changes**
   - Updated IPlugin interface with new methods
   - Additional configuration options added
   - Enhanced error handling requirements

2. **Backward Compatibility**
   - Old plugins continue to work with adapter layer
   - Automatic plugin migration utility available
   - Deprecation warnings for outdated features

3. **New Plugin Examples**
   - Updated documentation with new examples
   - Migration guide for custom plugin developers
   - Tool to help migrate old plugins to new architecture

### Upgrading Plugins

```bash
# Check for plugin compatibility
nlm plugin check path/to/plugin.so

# Upgrade plugins to latest version
nlm plugin upgrade path/to/plugin.so

# Get migration instructions
nlm plugin migrate path/to/plugin.so
```

## Future Enhancements

The plugin architecture is designed to be extensible for future enhancements:

1. **Machine Learning Plugins**
   - AutoML integration
   - Model training and optimization plugins
   - Hyperparameter tuning plugins

2. **Visualization Plugins**
   - Advanced 3D visualization
   - Real-time monitoring dashboards
   - Export and reporting plugins

3. **Safety and Verification Plugins**
   - Model validation and verification
   - Safety compliance checking
   - Performance benchmarking

4. **Integration Plugins**
   - Database integration
   - Cloud service connectors
   - API gateway plugins

## Conclusion

The NLM plugin architecture provides a flexible and extensible framework for extending the neural learning machine with custom components. Whether you're developing specialized sensors, novel neural architectures, or alternative world simulations, the plugin system allows you to enhance and customize NLM for your specific needs while maintaining the core system's stability and performance.

By following the best practices outlined in this guide and leveraging the plugin marketplace, you can create powerful, domain-specific solutions that push the boundaries of what's possible with neural learning machines.