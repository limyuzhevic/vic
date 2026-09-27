# NLM Advanced Commands System Documentation

## Overview

The NLM (Neural Learning Machine) Advanced Commands System provides a comprehensive interface for extending and customizing the brain's functionality through a command-based architecture. This system enables advanced users to create custom components, manage extensions, and execute complex operations without modifying core source code.

## Key Features

### 1. Component Factory System
- **Template-based creation**: Support for neuromodulators, memory systems, and cognitive functions
- **Parameterized configuration**: Custom component configuration with type-safe values
- **Automatic registration**: Components automatically integrate with the brain system

### 2. Extension Management
- **Plugin architecture**: Dynamic loading of extensions from files or modules
- **Extension points**: Hooks for custom integration and behavior modification
- **Lifecycle management**: Load, initialize, shutdown, and unload operations

### 3. Event System
- **Event-driven architecture**: Event system for decoupled extension communication
- **Typed events**: Support for different event data types
- **Event listeners**: Multiple listeners for single events
- **Event history**: Event recording for debugging and analysis

### 4. Command Registry
- **Categorized commands**: Organized commands for different functionality (Component, Extension, Plugin, Brain, System, Configuration)
- **Parameter support**: Commands with typed parameters
- **Brain integration**: Commands that require or interact with the brain instance

## Command Categories

### Component Commands
| Command | Description | Parameters |
|---------|-------------|------------|
| `create_neuromodulator` | Create a custom neuromodulator component | name, type, level, decay_rate |
| `create_memory_system` | Create a custom memory system component | name, type, capacity, max_episodes |
| `create_cognitive_function` | Create a custom cognitive function component | name, type, planning_depth, inhibition_strength |

### Extension Commands
| Command | Description | Parameters |
|---------|-------------|------------|
| `load_extension` | Load an extension from a module or file | name, module_path, auto_init, parameters |
| `unload_extension` | Unload an extension | name |
| `list_extensions` | List loaded extensions | category, state |

### System Commands
| Command | Description | Parameters |
|---------|-------------|------------|
| `system_status` | Get system status and statistics | detailed |
| `extension_point_status` | Get extension point status and handlers | point_name |
| `event_system_status` | Get event system status and listeners | (none) |

### Configuration Commands
| Command | Description | Parameters |
|---------|-------------|------------|
| `configuration` | Configuration management for extensions | operation, name, value |
| `plugin_management` | Plugin management commands | operation, plugin_name, parameters |

## Extension Point System

### Available Extension Points

1. **brain_start**: Called when brain starts
   - Parameters: {step, time}

2. **brain_step**: Called on each brain step
   - Parameters: {step, time}

3. **brain_stop**: Called when brain stops
   - Parameters: {step, time}

4. **neuron_added**: Called when a neuron is added
   - Parameters: {neuron_id, region}

5. **neuron_spike**: Called when a neuron spikes
   - Parameters: {neuron_id, time}

6. **synapse_formed**: Called when a synapse is formed
   - Parameters: {synapse_id, pre_id, post_id}

7. **plasticity_applied**: Called when plasticity is applied
   - Parameters: {weight_change, location}

### Extension Point Handler Execution

Extension point handlers execute in priority order (higher priority first). Handlers can:
- Modify system behavior
- Log information
- Validate operations
- Trigger custom actions

```cpp
// Register extension point handler
bool registerExtensionPointHandler(
    const std::string& pointName,
    ExtensionPointHandler handler,
    const std::string& handlerName = "",
    int priority = 0
);
```

## Event System

### Event Types

| Event Name | Description | Data Type |
|------------|-------------|-----------|
| `event_brain_initialized` | Brain initialization event | std::any |
| `event_brain_reset` | Brain reset event | std::any |
| `event_memory_updated` | Memory update event | std::any |
| `event_prediction_computed` | Prediction computation event | std::any |
| `event_cognition_executed` | Cognition execution event | std::any |
| `event_neuromodulation_updated` | Neuromodulation update event | std::any |

### Event Lifecycle

1. **Emission**: Extensions emit events using `emitEvent()`
2. **Delivery**: All registered event listeners receive events
3. **History**: Events are recorded for debugging and analysis

```cpp
// Emit an event
bool emitEvent(
    const std::string& eventName,
    const std::any& data = {},
    const std::string& sourceExtension = ""
);
```

## Component Factory System

### Factory Architecture

The component factory uses C++ templates for type-safe component creation:

```cpp
// Create component using factory template
std::shared_ptr<ComponentType> createComponent(
    const std::string& type,
    const std::unordered_map<std::string, std::string>& params = {}
);
```

### Component Types Supported

1. **Neuromodulators**
   - Dopamine, Acetylcholine, Norepinephrine
   - Serotonin, Curiosity, Novelty
   - PredictionError

2. **Memory Systems**
   - WorkingMemory (NeuralWorkingMemory)
   - EpisodicMemory (NeuralEpisodicMemory)
   - AssociativeMemory (NeuralAssociativeMemory)

3. **Cognitive Functions**
   - NeuralPlanner
   - ConceptFormation
   - AttentionalSelection

## Plugin Architecture

### Plugin Lifecycle

1. **Loading**: Extensions are loaded from specified paths
2. **Initialization**: Extensions receive global configuration and brain reference
3. **Registration**: Extensions register components, handlers, and listeners
4. **Execution**: Extensions participate in simulation via extension points and events
5. **Unloading**: Extensions are cleaned up and unloaded

### Extension Discovery

Automatic extension discovery is supported:
- File-based discovery: Extensions with `.nlmext`, `.so`, or `.dll` extensions
- Directory scanning: Recursive extension loading from paths
- Priority-based loading: Extensions loaded by priority order

## Advanced Brain Interface

### Integration with Brain

The Advanced Brain Interface integrates with the Brain class through:

1. **Command execution**: Execute advanced commands through Brain interface
2. **Component creation**: Create custom components via factory
3. **Extension management**: Load and manage extensions
4. **Event handling**: Emit and receive extension events
5. **Extension points**: Execute custom handlers at key simulation points

```cpp
// Initialize advanced interface
brain->initializeAdvancedInterface(config);

// Execute advanced command
std::vector<std::string> args = {"my_neuromodulator", "dopamine", "0.5", "0.01"};
brain->executeAdvancedCommand("create_neuromodulator", args);

// Load extensions from path
brain->loadExtensionsFromPath("./extensions");
```

## Configuration System

### Advanced Configuration

Advanced configuration supports complex hierarchical structures:

```cpp
struct AdvancedBrainInterfaceConfig {
    bool enableAdvancedCommands = true;
    bool enableExtensionSystem = true;
    bool enableComponentFactory = true;
    bool enableEventSystem = true;
    bool enablePluginArchitecture = true;
    bool autoLoadExtensions = true;
    std::string extensionPath = "./extensions";
    bool enableLogging = true;
    bool enableDebugMode = false;
};
```

### Runtime Configuration

Runtime configuration management enables:
- Dynamic parameter changes
- Component-specific configuration
- Extension-specific settings
- Performance tuning

## Usage Examples

### Example 1: Creating Custom Neuromodulators

```cpp
// Create a custom neuromodulator
std::unordered_map<std::string, std::string> params;
params["type"] = "dopamine";
params["level"] = "0.7";
params["decay_rate"] = "0.015";

brain->createComponent("neuromodulator", params);
```

### Example 2: Loading Extensions

```cpp
// Load all extensions from a directory
brain->loadExtensionsFromPath("./extensions");

// Get extension system status
std::string status = brain->getAdvancedInterfaceStatus();
```

### Example 3: Executing Advanced Commands

```cpp
// Execute an advanced brain command
std::vector<std::string> args = {"my_neuromodulator", "dopamine", "0.5", "0.01"};
std::unordered_map<std::string, std::any> params;
brain->executeAdvancedCommand("create_neuromulator", args, params);
```

### Example 4: Registering Extension Point Handlers

```cpp
// Register an extension point handler
bool success = brain->registerExtensionPointHandler(
    "brain_step",
    [](const std::string& pointName, const std::any& data, const std::unordered_map<std::string, std::any>& params) {
        // Custom handler implementation
        // data contains step information
        // params may contain additional configuration
        return true;
    },
    "my_extension",
    100  // High priority
);
```

### Example 5: Emitting Events

```cpp
// Emit an event
bool success = brain->emitEvent(
    "neuromodulator_created",
    static_cast<void*>(neuromodulator.get()),
    "my_extension"
);
```

## Benefits

### 1. Extensibility
- Custom components without core code changes
- Plugin architecture for modular extensions
- Extension points for behavior modification

### 2. Customization
- Parameterized component creation
- Custom neuromodulators and memory systems
- Custom cognitive functions

### 3. Integration
- Seamless integration with existing Brain architecture
- Shared data representations
- Consistent temporal dynamics

### 4. Maintainability
- Clean separation of concerns
- Well-defined interfaces
- Backward compatibility maintained

### 5. Flexibility
- Dynamic loading and runtime extension
- Priority-based handler execution
- Event-driven architecture

## Technical Details

### Memory Management
- Smart pointers for automatic memory management
- Extension lifecycle management
- Resource cleanup on shutdown

### Thread Safety
- Mutex protection for shared data
- Atomic operations for extension management
- Thread-safe event emission

### Error Handling
- Comprehensive error reporting
- Graceful failure handling
- Extension error recovery

### Performance Considerations
- Minimal overhead for standard operation
- Optimized for frequent operations
- Caching for repeated lookups

## Performance Optimization

### Component Creation
- Template-based factory minimizes runtime overhead
- Parameter validation reduces runtime errors
- Component caching improves repeated creation performance

### Extension Management
- Lazy loading of extensions
- Extension point handler caching
- Efficient extension point execution

### Event System
- Event history for debugging
- Listener batching for performance
- Event queue optimization

## Testing and Validation

### Command Testing
- Command parameter validation
- Command execution error handling
- Extension point handler testing

### Component Testing
- Component factory validation
- Component lifecycle management
- Component integration testing

### Extension Testing
- Extension loading and initialization
- Extension point registration
- Extension event handling

## Future Enhancements

### 1. Distributed Extensions
- Network-based extension loading
- Distributed plugin architecture
- Remote extension management

### 2. Extension Marketplace
- Extension catalog and discovery
- Extension versioning
- Extension dependency management

### 3. Advanced Configuration
- Schema-based configuration
- Configuration inheritance
- Configuration validation

### 4. Performance Monitoring
- Real-time performance metrics
- Extension performance profiling
- System-wide performance analysis

## Conclusion

The NLM Advanced Commands System provides a powerful framework for extending and customizing the brain simulator. It enables advanced users to:

1. Create custom components without modifying core code
2. Implement complex behaviors through extensions
3. Integrate custom functionality through extension points
4. Maintain clean separation of concerns
5. Scale the system with performance optimizations

This system transforms NLM from a fixed neural simulator into a flexible, extensible artificial brain platform capable of supporting diverse research and application scenarios.
