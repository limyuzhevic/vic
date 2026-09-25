# NLM Factory Pattern Implementation

This document describes the comprehensive factory pattern implementation for the NLM (Neural Learning Machine) system.

## Overview

The factory pattern implementation provides a centralized, flexible, and maintainable approach to object creation in the NLM system. It improves object creation, dependency injection, testability, and performance through several key features:

## Key Components

### 1. Base Factory Interface

**File:** `src/core/Factory/FactoryManager.hpp`

The `ObjectFactory` abstract base class defines the interface for all concrete factories:

```cpp
class ObjectFactory {
public:
    virtual ~ObjectFactory() = default;
    
    // Pure virtual creation methods for all object types
    virtual std::unique_ptr<Config> createConfig(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<RandomGenerator> createRandomGenerator(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<Logger> createLogger(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<Brain> createBrain(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<World> createWorld(const ObjectParams& params = {}) = 0;
    virtual std::unique_ptr<Experiment> createExperiment(const ObjectParams& params = {}) = 0;
    
    // Factory metadata and lifecycle
    virtual std::string getFactoryName() const = 0;
    virtual std::string getSupportedType() const = 0;
    virtual void configure(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual bool isInitialized() const = 0;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void registerFactory() = 0;
    virtual std::unique_ptr<ObjectFactory> clone() const = 0;
};
```

### 2. Factory Manager

**File:** `src/core/Factory/FactoryManager.hpp` and `src/core/Factory/FactoryManager.cpp`

The `FactoryManager` class provides:

- **Singleton Pattern:** Global access point for factory management
- **Factory Registry:** Centralized management of multiple factories
- **Object Creation:** Factory-based object creation with dependency injection
- **Object Pooling:** Performance optimization through reusable objects
- **Lifecycle Management:** Initialize and shutdown factory lifecycle
- **Thread Safety:** Mutex-protected operations for concurrent access
- **Configuration Support:** Configuration-based factory creation
- **Testing Support:** Factory testing capabilities

### 3. Concrete Factories

Six concrete factory classes implement the `ObjectFactory` interface:

#### ConfigFactory
**File:** `src/core/Config/ConfigFactory.hpp` and `src/core/Config/ConfigFactory.cpp`

Creates `Config` objects with different configurations:
- Default config creation
- Config from defaults map
- Config from file (JSON/YAML/simple format)
- Config from command line arguments
- Profile management
- Template management

#### RandomGeneratorFactory
**File:** `src/core/Random/RandomGeneratorFactory.hpp` and `src/core/Random/RandomGeneratorFactory.cpp`

Creates `RandomGenerator` objects with different seeds and distributions:
- Deterministic generator with explicit seed
- Time-based generator (non-deterministic)
- Secure generator with cryptographically secure randomness
- Generator with different distribution types

#### LoggerFactory
**File:** `src/core/Logger/LoggerFactory.hpp` and `src/core/Logger/LoggerFactory.cpp`

Creates `Logger` objects with different log levels and output configurations:
- Console logger with optional colors
- File logger with configurable output paths
- Multi-logger for multiple output streams
- Asynchronous logger for high-performance logging
- Custom logger with user-defined handlers

#### BrainFactory
**File:** `src/brain/BrainFactory.hpp` and `src/brain/BrainFactory.cpp`

Creates `Brain` objects with different neural architectures:
- Basic brain with default configuration
- Structured brain with predefined regions
- Connected brain with synaptic connections
- Plastic brain with specific plasticity mechanisms
- Adaptive brain with different adaptation strategies

#### WorldFactory
**File:** `src/world/WorldFactory.hpp` and `src/world/WorldFactory.cpp`

Creates `World` objects with different simulation environments:
- Simple world with basic properties
- Dynamic world with evolving properties
- Simulation world with specific simulation types
- Environment world with different environmental conditions
- Custom world with arbitrary parameters

#### ExperimentFactory
**File:** `src/experiments/ExperimentFactory.hpp` and `src/experiments/ExperimentFactory.cpp`

Creates `Experiment` objects for different test scenarios:
- Basic experiment with default settings
- Phase experiment for specific developmental phases
- Ablation experiment for studying neural ablation
- Scaling benchmark for performance testing
- Custom experiment with specific names and configurations

## Factory Features

### 1. Object Creation

Factory-based object creation with parameterized initialization:

```cpp
ObjectParams params;
params.setParam("seed", uint64_t(12345));
params.setParam("type", std::string("deterministic"));
params.setParam("level", int(static_cast<int>(LogLevel::Info)));

auto config = factoryManager.createConfig("default_config", params);
auto randomGenerator = factoryManager.createRandomGenerator("default_random", params);
auto logger = factoryManager.createLogger("default_logger", params);
```

### 2. Dependency Injection

Configuration and dependencies injected through `ObjectParams`:

```cpp
struct ObjectParams {
    std::unordered_map<std::string, std::any> params;
    std::string name;
    bool enableObjectPool = false;
    size_t poolSize = 100;
    bool threadSafe = true;
    
    template<typename T>
    T getParam(const std::string& key, const T& defaultValue = T{}) const;
    
    void setParam(const std::string& key, const std::any& value);
};
```

### 3. Object Pooling

Performance optimization through reusable objects:

```cpp
// Acquire object from pool
auto pooledConfig = factoryManager.acquireFromPool<Config>("");

// Use object
if (pooledConfig) {
    // Object operations
}

// Release back to pool
factoryManager.releaseToPool<Config>("", std::move(pooledConfig));
```

### 4. Factory Lifecycle Management

Proper factory lifecycle with initialization and shutdown:

```cpp
// Initialize all factories
factoryManager.initializeAllFactories();

// Check if factory is initialized
if (factoryManager.isInitialized("default_config")) {
    // Factory is ready for use
}

// Shutdown all factories
factoryManager.shutdownAllFactories();
```

### 5. Factory Discovery and Registration

Automatic factory discovery using registration macros:

```cpp
// In each concrete factory class
REGISTER_FACTORY(ConfigFactory, "default_config", "Config", true)
REGISTER_FACTORY(RandomGeneratorFactory, "default_random", "RandomGenerator", true)
REGISTER_FACTORY(LoggerFactory, "default_logger", "Logger", true)
// ... etc
```

### 6. Configuration-Based Creation

Create objects from configuration keys:

```cpp
// Register configuration
std::unordered_map<std::string, std::any> config;
config["type"] = std::string("deterministic");
config["seed"] = uint64_t(12345);
factoryManager.registerConfig("random_factory_default", config);

// Create from configuration
auto randomGenerator = factoryManager.createRandomGeneratorFromConfig("random_factory_default");
```

## Integration Points

### 1. Updated main.cpp

The main application has been updated to use the factory pattern:

- Replaced direct object creation with factory-based creation
- Added command-line factory configuration options
- Added factory testing capabilities
- Maintained backward compatibility

### 2. Command-Line Factory Configuration

New command-line options for factory configuration:

```
--config-factory ID       Config factory ID for dependency injection
--random-factory ID       Random generator factory ID for dependency injection
--logger-factory ID       Logger factory ID for dependency injection
--brain-factory ID        Brain factory ID for dependency injection
--world-factory ID        World factory ID for dependency injection
--experiment-factory ID   Experiment factory ID for dependency injection
```

### 3. Updated Tests

Comprehensive factory testing in `tests/test_factory.cpp`:

- Factory manager singleton testing
- Factory registration testing
- Factory configuration testing
- Object creation testing
- Object pooling testing
- Factory lifecycle testing
- Factory discovery testing
- Factory test capabilities testing

## Benefits

### 1. Improved Testability

- Easy mocking of factories in unit tests
- Dependency injection for isolated testing
- Factory configuration for different test scenarios
- Object pooling for performance testing

### 2. Enhanced Flexibility

- Factory switching without code changes
- Parameterized object creation
- Configuration-based configuration
- Extensible factory architecture

### 3. Better Performance

- Object pooling reduces allocation overhead
- Factory-specific optimizations
- Thread-safe operations for concurrent access
- Efficient resource management

### 4. Improved Maintainability

- Centralized factory management
- Clear separation of concerns
- Consistent object creation patterns
- Reduced code duplication

### 5. Enhanced Reliability

- Proper error handling and logging
- Thread-safe operations
- Factory lifecycle management
- Configuration validation

## Usage Examples

### Basic Usage

```cpp
#include "core/Factory/FactoryManager.hpp"

// Get the factory manager
FactoryManager& factoryManager = FactoryManager::getInstance();

// Create objects with default factories
auto config = factoryManager.createConfig();
auto randomGenerator = factoryManager.createRandomGenerator();
auto logger = factoryManager.createLogger();
```

### Advanced Usage with Dependency Injection

```cpp
// Configure factories
ObjectParams configParams;
configParams.setParam("source", std::string("file"));
configParams.setParam("merge", bool(false));

ObjectParams randomParams;
randomParams.setParam("seed", uint64_t(12345));
randomParams.setParam("type", std::string("deterministic"));

ObjectParams loggerParams;
loggerParams.setParam("level", int(static_cast<int>(LogLevel::Info)));
loggerParams.setParam("type", std::string("console"));

// Create objects with dependency injection
auto config = factoryManager.createConfig("default_config", configParams);
auto randomGenerator = factoryManager.createRandomGenerator("default_random", randomParams);
auto logger = factoryManager.createLogger("default_logger", loggerParams);
```

### Object Pooling

```cpp
// Enable object pooling
ObjectParams params;
params.enableObjectPool = true;
params.poolSize = 50;

// Create object with pooling
auto pooledConfig = factoryManager.createConfig("", params);

// Use object (automatically returned to pool when destroyed)
if (pooledConfig) {
    // Object operations
}
```

### Configuration-Based Creation

```cpp
// Register configuration
std::unordered_map<std::string, std::any> brainConfig;
brainConfig["type"] = std::string("structured");
brainConfig["regions"] = std::vector<std::string>{"default", "visual", "motor"};
factoryManager.registerConfig("brain_factory_structured", brainConfig);

// Create from configuration
auto brain = factoryManager.createBrainFromConfig("brain_factory_structured");
```

## Testing

Run the factory tests to verify the implementation:

```bash
# Compile and run factory tests
# (Assuming proper build system is configured)
```

The `test_factory.cpp` file includes comprehensive tests for all factory features:

- Factory manager singleton tests
- Factory registration tests
- Factory configuration tests
- Object creation tests
- Object pooling tests
- Factory lifecycle tests
- Factory discovery tests
- Factory test capabilities tests

## Migration Guide

### From Direct Object Creation to Factory Pattern

**Before:**

```cpp
// Direct object creation
auto config = std::make_unique<Config>();
auto randomGenerator = std::make_unique<RandomGenerator>(seed);
auto logger = std::make_unique<Logger>();
```

**After:**

```cpp
// Factory-based object creation
FactoryManager& factoryManager = FactoryManager::getInstance();

ObjectParams configParams;
configParams.setParam("source", std::string("file"));

ObjectParams randomParams;
randomParams.setParam("seed", seed);

ObjectParams loggerParams;
loggerParams.setParam("level", int(static_cast<int>(LogLevel::Info)));

auto config = factoryManager.createConfig("", configParams);
auto randomGenerator = factoryManager.createRandomGenerator("", randomParams);
auto logger = factoryManager.createLogger("", loggerParams);
```

### Factory Switching

**Before:**

```cpp
// Hard-coded factory selection
auto config = ConfigFactory::createDefaultConfig();
```

**After:**

```cpp
// Factory selection via configuration
ObjectParams params;
params.setParam("factoryId", std::string("advanced_config"));

auto config = factoryManager.createConfig("advanced_config", params);
```

## Conclusion

The NLM factory pattern implementation provides a robust, flexible, and maintainable solution for object creation in the NLM system. It improves testability, performance, and maintainability while maintaining backward compatibility. The implementation follows modern C++ best practices and provides a solid foundation for future enhancements.

## Files Created/Modified

### Created Files:

1. `src/core/Factory/FactoryManager.hpp` - Base factory interface and factory manager
2. `src/core/Factory/FactoryManager.cpp` - Factory manager implementation
3. `src/core/Config/ConfigFactory.hpp` - Config factory interface
4. `src/core/Config/ConfigFactory.cpp` - Config factory implementation
5. `src/core/Random/RandomGeneratorFactory.hpp` - RandomGenerator factory interface
6. `src/core/Random/RandomGeneratorFactory.cpp` - RandomGenerator factory implementation
7. `src/core/Logger/LoggerFactory.hpp` - Logger factory interface
8. `src/core/Logger/LoggerFactory.cpp` - Logger factory implementation
9. `src/brain/BrainFactory.hpp` - Brain factory interface
10. `src/brain/BrainFactory.cpp` - Brain factory implementation
11. `src/world/WorldFactory.hpp` - World factory interface
12. `src/world/WorldFactory.cpp` - World factory implementation
13. `src/experiments/ExperimentFactory.hpp` - Experiment factory interface
14. `src/experiments/ExperimentFactory.cpp` - Experiment factory implementation
15. `src/main.cpp` - Updated to use factory pattern
16. `tests/test_factory.cpp` - Comprehensive factory tests

### Modified Files:

1. `src/main.cpp` - Updated to use factory pattern for object creation

This factory pattern implementation significantly improves the NLM system's architecture by providing a centralized, flexible, and maintainable approach to object creation.