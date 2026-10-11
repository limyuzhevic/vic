# NLM API Reference Documentation

## Overview

The NLM (Neural Learning Machine) API provides comprehensive access to neural simulation capabilities through both C++ and Python interfaces. This documentation describes the public API for creating, configuring, and managing neural systems, along with detailed parameter descriptions and usage examples.

## API Structure

NLM offers two primary interfaces:

1. **C++ API** - Low-level, high-performance interface for advanced users
2. **Python API** - High-level, user-friendly interface for most applications

Both interfaces provide equivalent functionality with different abstractions and APIs.

## C++ API

### Core Classes

#### Brain Class

The `Brain` class is the central interface for neural computation in NLM.

**Header:** `#include <nlm/brain/Brain.hpp>`

```cpp
namespace nlm {

class Brain {
public:
    // Construction and initialization
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Sensory processing
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Action generation
    std::unique_ptr<Action> produceAction();
    
    // Plasticity and development
    void updatePlasticity();
    void develop();
    
    // State management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Memory systems (Phase 4/6)
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system (Phase 4/6)
    PredictionSystem* getPredictionSystem();
    
    // Cognitive systems (Phase 4/6)
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    SelfModel* getSelfModel();
    SocialLearning* getSocialLearning();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Novelty* getNovelty();
    Curiosity* getCuriosity();
    PredictionError* getPredictionErrorSignal();
    
    // Core system access
    SpikeSystem* getSpikeSystem();
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Statistics and monitoring
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Inter-region connectivity
    void addInterRegionConnection(RegionId source, RegionId target,
                                float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Error handling and monitoring
    void logStatus() const;
    static bool isBrainHealthy(const Brain& brain);
    static std::string getBrainHealthReport(const Brain& brain);
    static void setGracefulDegradationEnabled(bool enabled);
    static bool isGracefulDegradationEnabled();
    
    // Protected methods for subclasses
protected:
    bool initializeSystem(void* system, const std::string& systemName);
    void cleanupSystem(void* system, const std::string& systemName);
    bool validateConfiguration(const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

#### Config Class

The `Config` class provides configuration management for neural systems.

**Header:** `#include <nlm/core/Config/Config.hpp>`

```cpp
namespace nlm {

class Config {
public:
    // Core configuration operations
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    std::optional<ConfigValue> get(const std::string& key) const;
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Type-safe configuration access
    template<typename T>
    std::optional<T> getTyped(const std::string& key) const;
    
    template<typename T>
    T getOrTyped(const std::string& key, const T& defaultValue) const;
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Format support
    bool loadFromJSON(const std::string& jsonStr);
    bool loadFromYAML(const std::string& yamlStr);
    bool saveToJSON(std::string& output) const;
    bool saveToYAML(std::string& output) const;
    
    // Configuration builder pattern
    class ConfigurationBuilder;
    ConfigurationBuilder* getBuilder() const;
    std::shared_ptr<Config> buildFromBuilder(
        std::function<void(ConfigurationBuilder*)> builderSetup);
    
    // Validation
    ValidationResult validate() const;
    ValidationResult validate(const ValidationConfig& config) const;
    
    // Change notifications
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    
    // Configuration sources
    ConfigSource getSource(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    
    // Statistics
    size_t getEntryCount() const;
    std::string getStatistics() const;
    
    // Factory functions
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createConfiguration();
    
    // Friend classes for builder access
    friend class ConfigurationBuilder;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

#### AgentBrain Class

The `AgentBrain` class connects NLM brain to the world environment.

**Header:** `#include <nlm/agent/AgentBrain.hpp>`

```cpp
namespace nlm {

class AgentBrain {
public:
    // Construction
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialization
    void initialize(const SimpleWorld& world);
    
    // Sensory processing
    size_t getSensoryInputSize() const;
    size_t getMotorOutputSize() const;
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    MotorCommand decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Development
    void updateDevelopment(double timestep);
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Neuromodulation state
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Configuration
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    // Reset
    void reset();
    
    // Access
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
private:
    // Motor decoding
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
```

#### SimpleWorld Class

The `SimpleWorld` class provides environment interface for NLM.

**Header:** `#include <nlm/world/SimpleWorld.hpp>`

```cpp
namespace nlm {

class SimpleWorld {
public:
    // Construction
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    void configure(int width, int height,
                   int visionWidth = 8, int visionHeight = 8,
                   int touchWidth = 4, int touchHeight = 4);
    
    // State management
    void reset();
    void update(double timestep);
    void setRandomSeed(unsigned int seed);
    
    // Agent interaction
    void setAgentStart(double x, double y);
    void applyAction(const MotorCommand& cmd, double currentTime);
    
    // State access
    SensoryPercept getSensoryPercept() const;
    AgentBody getAgentBody() const;
    
    // World objects
    void addObject(const WorldObject& obj);
    void removeObject(double x, double y);
    bool isValidPosition(double x, double y) const;
    
    // Properties
    int getWidth() const;
    int getHeight() const;
    double getSimulationTime() const;
    double getMaxEnergy() const;
    void setMaxEnergy(double energy);
    
    // Statistics
    size_t getObjectCount() const;
    size_t getAgentStepCount() const;
    
    // Factory functions
    static std::unique_ptr<SimpleWorld> create(int width = 20, int height = 20);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

#### Error Handling Classes

**NLMError Base Class:**

```cpp
namespace nlm {

class NLMError : public std::exception {
protected:
    std::string message_;
    std::string context_;
    int code_;
    std::string sourceFile_;
    int sourceLine_;
    std::chrono::system_clock::time_point timestamp_;
    
public:
    NLMError(const std::string& message, const std::string& context = "",
             int code = 0, const std::string& file = "", int line = 0);
    
    const char* what() const noexcept override;
    std::string getMessage() const;
    std::string getContext() const;
    int getCode() const;
    std::string getSourceFile() const;
    int getSourceLine() const;
    std::string getFormattedTimestamp() const;
};
```

**Specific Error Classes:**

```cpp
class ConfigError : public NLMError {
    enum class Code {
        InvalidKey = 1000,
        MissingRequiredValue = 1001,
        TypeMismatch = 1002,
        FileNotFound = 1003,
        ParseError = 1004,
        ValidationFailed = 1005
    };
};

class MemoryError : public NLMError {
    enum class Code {
        AllocationFailed = 2000,
        InvalidPointer = 2001,
        BufferOverflow = 2002,
        ResourceExhausted = 2003
    };
};

class BrainError : public NLMError {
    enum class Code {
        InitializationFailed = 3000,
        SimulationError = 3001,
        InvalidConfiguration = 3002,
        MemorySystemError = 3003
    };
};

class LoggerError : public NLMError {
    enum class Code {
        WriteFailed = 4000,
        FileError = 4001,
        InitializationFailed = 4002
    };
};

class WorldError : public NLMError {
    enum class Code {
        BoundaryViolation = 5000,
        CollisionError = 5001,
        InvalidState = 5002
    };
};

class ValidationError : public NLMError {
    enum class Code {
        InvalidInput = 6000,
        RangeViolation = 6001,
        MissingDependency = 6002
    };
};
```

### Error Handling Macros

```cpp
// Error throwing macros
#define NLM_THROW_ERROR(type, message, context, code) \
    do { \
        std::string ctx = nlm::ErrorContext::getInstance().getCurrentContext(); \
        if (!ctx.empty()) ctx = ctx + " -> " + context; else ctx = context; \
        nlm::getErrorReporter().reportError(type(message, ctx, code, __FILE__, __LINE__)); \
        throw type(message, ctx, code, __FILE__, __LINE__); \
    } while(0)

#define NLM_THROW_CONFIG_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::ConfigError, message, context, code)

#define NLM_THROW_BRAIN_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::BrainError, message, context, code)

// Error reporting macros
#define NLM_LOG_ERROR(message) nlm::logError(nlm::NLMError(message))
#define NLM_LOG_WARNING(message) nlm::logWarning(message)
```

### Type Definitions and Enums

#### Neuron Types

```cpp
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Sensory,
    Motor,
    Modulatory,
    Internal
};
```

#### Action Types

```cpp
enum class ActionType {
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact,
    Eat,
    Wait,
    Jump
};
```

#### Developmental Stages

```cpp
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};
```

#### World Object Types

```cpp
enum class WorldObjectType {
    Empty,
    Resource,
    Hazard,
    Wall,
    Marker,
    Agent
};
```

#### Configuration Sources

```cpp
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Builder
};
```

### Free Functions

#### Factory Functions

```cpp
// Configuration creation
std::shared_ptr<Config> createConfiguration();
std::shared_ptr<Config> createDefaultConfig();

// Brain creation
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);

// Agent creation
std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain);

// World creation
std::unique_ptr<SimpleWorld> createSimpleWorld();

// Error reporting
void setErrorReporter(std::unique_ptr<ErrorReporter> reporter);
ErrorReporter& getErrorReporter();
```

#### Error Reporting Functions

```cpp
void logError(const NLMError& error);
void logWarning(const std::string& message, const std::string& context = "");
void translateNLMException(const NLMError& e);
```

#### Error Context Management

```cpp
class ErrorContext {
public:
    static ErrorContext& getInstance();
    
    void pushContext(const std::string& operation);
    void popContext();
    std::string getCurrentContext() const;
    void clear();
};
```

### Memory Management Utilities

```cpp
// Safe memory allocation
void* safe_malloc(size_t size, const std::string& context = "");
void safe_free(void* ptr, const std::string& context = "");

// Smart pointer wrappers
template<typename T>
class SafePtr {
public:
    SafePtr(T* ptr = nullptr, const std::string& operation = "");
    ~SafePtr();
    
    T* get() const;
    T* operator->() const;
    T& operator*() const;
    operator bool() const;
    bool operator!() const;
    
private:
    T* ptr_;
    std::string operation_;
};

// Template constructors
template<typename T, typename... Args>
T* safe_new(Args&&... args);

template<typename T>
void safe_delete(T* ptr, const std::string& context = "");
```

## Python API

The Python API provides a simplified interface to NLM functionality, focusing on ease of use while maintaining access to core neural simulation capabilities.

### Python Module Structure

```python
import pynlm
from typing import List, Optional, Dict, Any

# Main classes
class Brain:
    """Main brain interface for neural computation."""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """Initialize brain with configuration."""
        pass
    
    def initialize(self) -> bool:
        """Initialize the brain with configuration."""
        pass
    
    def step(self, step_number: int) -> None:
        """Run one simulation step."""
        pass
    
    def reset(self) -> None:
        """Reset brain state."""
        pass
    
    def receive_sensory_input(self, sensory_input: Dict[str, Any]) -> None:
        """Inject sensory data into brain."""
        pass
    
    def produce_action(self) -> Dict[str, Any]:
        """Generate motor action from brain activity."""
        pass
    
    def get_total_neuron_count(self) -> int:
        """Get total number of neurons."""
        pass
    
    def get_total_synapse_count(self) -> int:
        """Get total number of synapses."""
        pass

class Config:
    """Configuration management."""
    
    @staticmethod
    def create_default_config() -> Dict[str, Any]:
        """Create default configuration."""
        pass
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None):
        pass
    
    def set(self, key: str, value: Any) -> None:
        pass
    
    def get(self, key: str, default: Any = None) -> Any:
        pass
    
    def has(self, key: str) -> bool:
        pass
    
    def load_from_file(self, filepath: str) -> bool:
        pass
    
    def save_to_file(self, filepath: str) -> bool:
        pass

class AgentBrain:
    """Agent-brain interface."""
    
    def __init__(self, brain: Brain):
        pass
    
    def initialize(self, world: Any) -> None:
        pass
    
    def process_sensory_input(self, percept: Any) -> None:
        pass
    
    def decode_motor_command(self) -> Any:
        pass
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float) -> None:
        pass

class SimpleWorld:
    """Environment interface."""
    
    def __init__(self):
        pass
    
    def configure(self, width: int, height: int, 
                  vision_width: int = 8, vision_height: int = 8) -> None:
        pass
    
    def reset(self) -> None:
        pass
    
    def update(self, timestep: float) -> None:
        pass
    
    def get_sensory_percept(self) -> Any:
        pass
```

### Python Enumerations

```python
# Neuron types
class NeuronType:
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    SENSORY = "sensory"
    MOTOR = "motor"
    MODULATORY = "modulatory"
    INTERNAL = "internal"

# Developmental stages
class DevelopmentalStage:
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"

# Action types
class ActionType:
    MOVE_FORWARD = "move_forward"
    TURN_LEFT = "turn_left"
    INTERACT = "interact"
    EAT = "eat"
    # ... more

# World object types
class WorldObjectType:
    EMPTY = "empty"
    RESOURCE = "resource"
    HAZARD = "hazard"
    WALL = "wall"
    MARKER = "marker"
```

### Python Factory Functions

```python
# Brain creation
brain = pynlm.create_brain(config)
agent = pynlm.create_agent_brain(brain)
world = pynlm.create_simple_world()

# Configuration
config = pynlm.create_default_config()

# Type conversions
brain = pynlm.from_cpp_brain(cpp_brain)
cpp_brain = pynlm.to_cpp_brain(python_brain)
```

### Python Error Handling

```python
# Exception hierarchy
class NLMError(Exception):
    """Base NLM exception."""

class ConfigError(NLMError):
    """Configuration error."""

class MemoryError(NLMError):
    """Memory error."""

class BrainError(NLMError):
    """Brain error."""

# Error handling
if "debug" in os.environ:
    pynlm.enable_debug_logging(True)

# Try-except blocks
try:
    brain = pynlm.create_brain(config)
    brain.initialize()
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

## API Reference Summary

### Core Functionality

| Class | Description | Primary Methods |
|-------|-------------|----------------|
| Brain | Main neural computation interface | `initialize()`, `step()`, `produceAction()` |
| Config | Configuration management | `loadFromFile()`, `saveToFile()`, `get()` |
| AgentBrain | Agent-environment interface | `processSensoryInput()`, `decodeMotorCommand()` |
| SimpleWorld | Environment simulation | `update()`, `getSensoryPercept()` |

### Error Handling

| Exception | Description | Common Codes |
|-----------|-------------|-------------|
| NLMError | Base exception | Various |
| ConfigError | Configuration errors | InvalidKey, FileNotFound |
| MemoryError | Memory allocation errors | AllocationFailed |
| BrainError | Brain simulation errors | InitializationFailed |
| LoggerError | Logging errors | WriteFailed |
| WorldError | World simulation errors | BoundaryViolation |
| ValidationError | Input validation errors | InvalidInput |

### Type Aliases

| Type | Description |
|------|-------------|
| NeuronId | Neuron identifier |
| SynapseId | Synapse identifier |
| RegionId | Brain region identifier |
| SimulationStep | Simulation step counter |
| Timestamp | Time value |
| MembranePotential | Neuron membrane potential |
| ActionType | Motor action type |
| DevelopmentalStage | Brain development stage |

### File Structure

```
Include Files:
├── nlm/brain/Brain.hpp
├── nlm/core/Config/Config.hpp
├── nlm/agent/AgentBrain.hpp
├── nlm/world/SimpleWorld.hpp
├── nlm/core/ErrorHandling/Error.hpp
├── nlm/core/Logger/Logger.hpp
└── nlm/experiments/Phase4Experiment.hpp

Source Files:
├── src/brain/Brain.cpp
├── src/core/Config/Config.cpp
├── src/agent/AgentBrain.cpp
├── src/world/SimpleWorld.cpp
├── src/core/ErrorHandling/Error.cpp
└── src/experiments/Phase4Experiment.cpp
```

## API Usage Examples

### Example 1: Simple Neural Network

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

int main() {
    // Create configuration
    auto config = nlm::createConfiguration();
    
    // Configure brain
    auto builder = config->getBuilder();
    builder->addInt("nlm.brain.neuron_count", 1000, "Number of neurons");
    builder->addDouble("nlm.brain.synapse_density", 0.1, "Synapse density");
    
    auto finalConfig = builder->build();
    
    // Create brain
    nlm::Brain brain(finalConfig);
    brain.initialize();
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    return 0;
}
```

### Example 2: Agent-Environment Simulation

```python
import pynlm

# Create components
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
agent = pynlm.create_agent_brain(brain)
world = pynlm.create_simple_world()

# Configure world
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

# Initialize agent
agent.initialize(world)
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    world.apply_action(agent.decode_motor_command(), step * 0.1)
```

### Example 3: Configuration Management

```cpp
#include <nlm/core/Config/Config.hpp>

void exampleConfiguration() {
    // Create configuration
    auto config = nlm::createConfiguration();
    
    // Add configuration using builder
    auto builder = config->getBuilder();
    builder->addInt("nlm.brain.neuron_count", 1000, "Number of neurons");
    builder->addBool("nlm.plasticity.enabled", true, "Enable plasticity");
    builder->addDouble("nlm.neuromodulation.curiosity.weight", 0.5, "Curiosity weight");
    
    auto finalConfig = builder->build();
    
    // Use configuration
    if (auto neuronCount = finalConfig->getTyped<int64_t>("nlm.brain.neuron_count")) {
        // Valid access
        std::cout << "Neuron count: " << *neuronCount << std::endl;
    }
    
    // Save configuration
    finalConfig->saveToFile("my_config.json");
}
```

### Example 4: Error Handling

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

void exampleErrorHandling() {
    try {
        // Create configuration
        auto config = nlm::createConfiguration();
        auto builder = config->getBuilder();
        builder->addInt("nlm.brain.neuron_count", 0, "Invalid neuron count");
        auto finalConfig = builder->build();
        
        // This should throw validation error
        auto neuronCount = finalConfig->getTyped<int64_t>("nlm.brain.neuron_count");
        
    } catch (const nlm::ValidationError& e) {
        std::cerr << "Validation error: " << e.what() << std::endl;
        std::cerr << "Context: " << e.getContext() << std::endl;
    } catch (const nlm::ConfigError& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
    } catch (const nlm::NLMError& e) {
        std::cerr << "NLM error: " << e.what() << std::endl;
    }
}
```

## Cross-Reference

### Related Documentation

- **Architecture.md** - System architecture and component relationships
- **Configuration.md** - Detailed configuration system usage
- **ErrorHandling.md** - Comprehensive error handling principles
- **Performance.md** - Performance optimization strategies
- **Phase6Guide.md** - Phase 6 integration concepts
- **Development.md** - Development guidelines and best practices

### API Evolution

The NLM API is designed for:

1. **Backward Compatibility**: Existing code continues to work
2. **Forward Compatibility**: New features are additive
3. **Type Safety**: Type-safe accessors prevent runtime errors
4. **Error Handling**: Comprehensive error handling with informative messages
5. **Performance**: Efficient implementation with minimal overhead

## Conclusion

The NLM API provides comprehensive access to neural simulation capabilities through both C++ and Python interfaces. It combines low-level control for advanced users with high-level abstractions for accessibility. The API is designed for performance, correctness, and extensibility, making it suitable for research, education, and production use.

Key API characteristics:

- **Dual Interface**: C++ for performance, Python for ease of use
- **Error Handling**: Comprehensive error reporting with informative messages
- **Type Safety**: Type-safe accessors and validation
- **Configuration**: Flexible configuration system with JSON/YAML support
- **Memory Management**: Safe memory allocation and smart pointers
- **Thread Safety**: Thread-safe operations for concurrent access
- **Extensibility**: Clean patterns for adding new components
- **Documentation**: Comprehensive API documentation with examples

The NLM API is production-ready and provides a solid foundation for neural simulation research and applications.