# NLM C++ Developer Guide

## Overview

NLM (Neural Learning Machine) is primarily written in C++20 and provides a complete neural simulation framework for advanced developers and researchers. The C++ API offers maximum performance, fine-grained control, and access to all internal systems.

This guide covers:

- Project structure and build system
- C++ API documentation
- Development workflow
- Best practices and patterns
- Testing and debugging
- Performance optimization

## Project Structure

### Main Components

```
NLM/
├── src/
│   ├── core/           # Core utilities (Config, Random, Logger, SimulationClock)
│   ├── brain/          # Neural components (Brain, Neuron, Synapse, NeuralPopulation)
│   ├── dynamics/       # Neural dynamics (LIF neurons, spike system)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian, structural)
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (dopamine, curiosity, etc.)
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── agent/         # Agent systems (AgentBrain, AgentBody)
│   ├── experiments/   # Experiment framework
│   └── visualization/ # Visualization
├── python/             # Python bindings (pybind11)
├── tests/             # Unit tests
├── configs/           # Configuration files
├── docs/             # Documentation
└── CMakeLists.txt     # Build configuration
```

### Key Directories

#### Core Components (`src/core/`)

- **Config**: JSON-based configuration system with command-line support
- **Random**: Deterministic random number generator with seeds
- **Logger**: Structured logging system with different levels
- **SimulationClock**: Time management for discrete events

#### Brain Components (`src/brain/`)

- **Brain**: Main brain coordinator
- **Neuron**: Leaky Integrate-and-Fire neuron implementation
- **Synapse**: Synaptic connections with plasticity support
- **NeuralPopulation**: Groups of neurons with shared properties
- **NeuralRegion**: Brain regions containing multiple populations

#### Advanced Systems

NLM includes sophisticated systems implemented across multiple phases:

- **Phase 2**: Neural computation (LIF, plasticity, structural changes)
- **Phase 3**: World interaction (sensory-motor loop)
- **Phase 4**: Cognitive capabilities (working memory, prediction, attention)
- **Phase 5**: Performance optimizations (SIMD, parallel processing)
- **Phase 6**: Integration and functionality verification

## Building from Source

### Prerequisites

```bash
# Required tools
gcc 10+ or clang 12+ or MSVC 2019+
CMake 3.16 or higher
Python 3.8+ (for Python bindings)
Git
make/ninja

# Optional for development
pybind11 >= 2.11.0
```

### Build Steps

#### Standard Release Build

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake with Release optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (using all CPU cores)
make -j$(nproc)

# Run unit tests (if built)
ctest --output-on-failure
```

#### Debug Build

```bash
# Create build directory
cd build

# Configure with Debug symbols and assertions
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

#### Python Bindings Build

```bash
# From project root
pip install pybind11 scikit-build-core pytest numpy
pip install .  # Install Python bindings

# Or build just Python bindings
cd python
python -m pip install -e .  # Install in development mode
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `pynlm` | Python Module | Python bindings |

### Build Options

#### CMake Configuration Options

```bash
# Custom compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Enable specific features
cmake .. -DENABLE_TESTING=ON -DENABLE_PYTHON_BINDINGS=ON

# Build configuration types
cmake .. -DCMAKE_BUILD_TYPE=Debug      # Debug with symbols
cmake .. -DCMAKE_BUILD_TYPE=Release   # Optimized

# Parallel build options
cmake .. -j$(nproc)  # Use all cores for configuration
```

#### Project-Specific Options

```bash
# Run Phase 6 integration test (comprehensive system verification)
./nlm_phase6_demo

# List available tests
./nlm_test --list-tests

# Run specific tests
./nlm_test --test basic_connectivity,plasticity,stdp_verification

# Load custom configuration
./nlm --config=my_config.cfg --test=all
```

## C++ API Documentation

### Core Types

#### Type System

NLM uses strongly-typed identifiers for type safety:

```cpp
// Neuron identifier
NeuronId neuronId;                    // Default constructor
NeuronId neuronId(42);               // With value
uint64_t index = neuronId.index();  // Extract index

// Synapse identifier  
SynapseId synapseId;

// Region identifier
RegionId regionId;

// Population identifier
PopulationId populationId;

// Equality comparison
if (neuronId == otherId) { ... }
if (neuronId != otherId) { ... }
```

#### Enumeration Types

```cpp
// Neuron types
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Synapse types
enum class SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Developmental stages
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Firing states
enum class FiringState {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Action types
enum class ActionType {
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    TurnLeft,
    TurnRight,
    Look,
    LookUp,
    LookDown,
    Interact,
    Eat,
    Drink,
    Rest,
    Wait,
    Custom
};
```

### Core Classes

#### Config Class (`src/core/Config/`)

```cpp
class Config {
public:
    // Creation and basic operations
    Config();                                    // Default constructor
    bool loadFromFile(const std::string& filepath);  // Load JSON config
    void loadFromArgs(int argc, char** argv);       // Load from command line
    void saveToFile(const std::string& filepath);   // Save to JSON
    bool has(const std::string& key) const;         // Check if key exists
    std::vector<std::string> getKeys() const;       // Get all keys
    void clear();                                   // Clear all entries
    std::string summary() const;                    // Configuration summary

    // Set configuration values
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Default);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Default);

    // Get configuration values with defaults
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;

    // Configuration sources for tracking
    enum class ConfigSource {
        Default,
        File,
        CommandLine,
        Programmatic
    };
};
```

#### Brain Class (`src/brain/Brain/`)

```cpp
class Brain {
public:
    // Construction
    Brain(std::shared_ptr<Config> config);           // Create with config

    // Initialization and control
    bool initialize();                              // Initialize brain
    void reset();                                   // Reset to initial state
    void step(SimulationStep step);                 // Step 1
    void step(SimulationStep step, Timestamp time); // Step with timestamp

    // Input/output processing
    void receiveSensoryInput(const std::shared_ptr<SensoryInput>& input);
    std::shared_ptr<Action> produceAction();        // Get motor action
    void injectCurrent(NeuronId neuron, float current);
    void injectCurrentToNeurons(NeuronType type, float current);

    // State management
    void save(const std::string& filepath) const;   // Save state
    void load(const std::string& filepath);         // Load state

    // Region management
    RegionId addRegion(const std::string& name = "");
    std::shared_ptr<NeuralRegion> getRegion(RegionId id) const;
    std::vector<RegionId> getRegionIds() const;
    size_t getRegionCount() const;

    // Statistics and monitoring
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    double getAverageFiringRate() const;
    double getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getTotalEventCount() const;

    // Development
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);

    // Configuration access
    std::shared_ptr<Config> getConfig() const;

    // Status logging
    void logStatus() const;
};
```

#### AgentBrain Class (`src/agent/AgentBrain/`)

```cpp
class AgentBrain {
public:
    // Construction
    AgentBrain(std::shared_ptr<Brain> brain);

    // Initialization
    void initialize(std::shared_ptr<SimpleWorld> world);

    // Sensory processing
    std::shared_ptr<SensoryPercept> getSensoryInputSize() const;
    void processSensoryInput(const std::shared_ptr<SensoryPercept>& percept);

    // Motor decoding
    std::shared_ptr<Action> decodeMotorCommand();
    std::shared_ptr<SensoryPercept> getSensoryInputSize() const;

    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    void updateDevelopment(Timestamp timestep);

    // Development and learning
    DevelopmentalStage getDevelopmentalStage() const;
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;

    // Subsystem enable/disable
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);

    // Subsystem status
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;

    // Utility
    std::shared_ptr<Brain> getBrain() const;
    void reset();  // Reset agent for new episode
};
```

### Advanced Classes

#### Neuron Class (`src/brain/Neuron/`)

```cpp
class Neuron {
public:
    // Neuron properties
    float getMembranePotential() const;
    void setMembranePotential(float voltage);
    float getThreshold() const;
    float getRestingPotential() const;
    float getRefractoryPeriod() const;
    FiringState getFiringState() const;
    NeuronType getNeuronType() const;

    // Input and dynamics
    void injectCurrent(float current);
    bool update(double dt);  // Returns true if neuron spiked
    void reset();  // Reset to resting state

    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool structural);

    // Developmental properties
    void setDevelopmentStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentStage() const;
};
```

#### Synapse Class (`src/brain/Synapse/`)

```cpp
class Synapse {
public:
    // Properties
    float getWeight() const;
    void setWeight(float weight);
    float getDelay() const;
    SynapseType getSynapseType() const;
    bool isPlastic() const;

    // Plasticity mechanisms
    void enablePlasticity(bool hebbian, bool stdp, bool structural);
    void updatePlasticity(float preSpikeTime, float postSpikeTime);
    void applySTDP(float preTime, float postTime);
    void applyHebbian(float preActivity, float postActivity);

    // Structural plasticity
    void grow();      // Increase probability of connection
    void prune();     // Decrease probability of connection
    float getGrowthRate() const;
    float getPruningRate() const;
};
```

### Utility Functions

#### Simulation Clock

```cpp
class SimulationClock {
public:
    SimulationClock(double timestep);
    Timestamp getCurrentTime() const;
    SimulationStep getCurrentStep() const;
    double getTimestep() const;
    void advance(double dt);
    void step();
};
```

#### Random Number Generator

```cpp
class Random {
public:
    Random();
    Random(uint64_t seed);
    void setSeed(uint64_t seed);
    uint64_t getSeed() const;
    
    // Random number generation
    int64_t uniformInt(int64_t min, int64_t max);
    double uniformDouble(double min, double max);
    float uniformFloat(float min, float max);
    bool bernoulli(double probability);
    
    // Gaussian distribution
    double gaussian(double mean, double stddev);
    
    // Random selection
    template<typename T>
    T& pickRandom(std::vector<T>& container);
};
```

#### Logger

```cpp
class Logger {
public:
    static Logger& getGlobal();
    void addLogger(std::shared_ptr<Logger> logger);
    void log(LogLevel level, const std::string& message);
    
    // Log levels
    enum class LogLevel {
        Error,
        Warning,
        Info,
        Debug
    };
};

// Convenience macros
#define NLM_LOG_ERROR(msg) Logger::getGlobal().log(Logger::LogLevel::Error, msg)
#define NLM_LOG_WARNING(msg) Logger::getGlobal().log(Logger::LogLevel::Warning, msg)
#define NLM_LOG_INFO(msg) Logger::getGlobal().log(Logger::LogLevel::Info, msg)
#define NLM_LOG_DEBUG(msg) Logger::getGlobal().log(Logger::LogLevel::Debug, msg)
```

## Development Workflow

### Setting Up the Development Environment

#### Install Dependencies

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git python3 python3-pip

# Install Python dependencies for development
pip install pybind11 scikit-build-core pytest numpy

# Install development tools
pip install cmake make ninja-build
```

#### Clone and Build

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create development build
mkdir build-dev
cd build-dev
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON

# Build
make -j$(nproc)
```

### Running Tests

#### Unit Tests

```bash
# Run all tests
cd build
cmake --build . --target test

# Or use ctest
cd build
ctest --output-on-failure

# Run specific test targets
cd build
make nlm_test  # Run test executable
./nlm_test --list-tests  # List all available tests
```

#### Available Test Targets

| Test Target | Description |
|-------------|-------------|
| `test_types.cpp` | Type system tests |
| `test_random.cpp` | Random number generator tests |
| `test_config.cpp` | Configuration system tests |
| `test_clock.cpp` | Simulation clock tests |
| `test_neuron.cpp` | Neuron dynamics tests |
| `test_synapse.cpp` | Synapse and plasticity tests |
| `test_stdp.cpp` | STDP mechanism tests |
| `test_brain.cpp` | Brain integration tests |

#### Running Tests with Configuration

```cpp
// Example from src/main.cpp
// Parse command line arguments for test selection
std::string testSelection = "all"; // Default: run all tests
for (int i = 1; i < argc; ++i) {
    std::string arg(argv[i]);
    if (arg == "--test" || arg == "-t") {
        if (i + 1 < argc) {
            testSelection = argv[++i];
        }
    } else if (arg == "--list-tests") {
        // List available tests and exit
        return 0;
    }
}

// Apply test selection
testRunner.setTestSelection(testSelection);

// Run tests
if (!testRunner.runAllTests()) {
    return 1; // Some tests failed
}
```

### Debugging and Development

#### Debugging with GDB

```bash
# Start debugging
gdb ./nlm_test

# Set breakpoints
break TestRunner::runTest
break Brain::initialize

# Run program
run

# Step through code
next          # Execute next line
step          # Step into function calls
continue     # Continue execution
print var    # Print variable value
```

#### Logging and Output

```cpp
// Enable verbose logging
auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
logger->addLogger(consoleLogger);
Logger::setGlobal(logger);

// Custom logging format
struct CustomLogger : public Logger {
    void log(LogLevel level, const std::string& message) override {
        std::cout << "[CUSTOM] " << message << std::endl;
    }
};
```

#### Memory Debugging

```bash
# Use valgrind for memory leak detection
valgrind --leak-check=full ./nlm_test

# Use AddressSanitizer (clang/gcc)
g++ -g -fsanitize=address -o nlm_test nlm_test.cpp
./nlm_test
```

### Contributing Code

#### Code Style Guidelines

NLM follows a consistent code style:

1. **C++20**: Use modern C++20 features where appropriate
2. ** noexcept**: Use noexcept for non-throwing functions
3. ** RAII**: Use resource acquisition is initialization
4. ** Smart Pointers**: Prefer std::shared_ptr and std::unique_ptr
5. ** Const Correctness**: Use const where possible
6. ** Naming**: snake_case for functions, CamelCase for classes
7. ** Documentation**: Use Doxygen-style comments for public APIs

#### Pull Request Workflow

1. **Fork** the repository
2. **Create** a feature branch
3. **Commit** changes with descriptive messages
4. **Test** locally
5. **Push** to branch
6. **Create** pull request
7. **Review** and iterate

#### Code Review Checklist

- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] Code follows project style guidelines
- [ ] Documentation is updated
- [ ] No memory leaks
- [ ] Thread safety considerations
- [ ] Performance impact assessed

## Performance Optimization

### SIMD Vectorization

NLM uses SIMD for performance-critical operations:

```cpp
// Example SIMD usage (requires compiler support)
void processNeuronsSIMD(const std::vector<Neuron*>& neurons, double dt) {
    // Compiler auto-vectorizes when optimization is enabled
    for (size_t i = 0; i < neurons.size(); i += SIMD_WIDTH) {
        __m256 voltages = _mm256_loadu_ps(&neurons[i]->voltage);
        __m256 currents = _mm256_loadu_ps(&neurons[i]->current);
        // Vectorized computations
        __m256 new_voltages = computeDynamics(voltages, currents, dt);
        _mm256_storeu_ps(&neurons[i]->voltage, new_voltages);
    }
}
```

### Parallel Processing

```cpp
// Parallel processing for large networks
void simulateInParallel(const std::vector<std::shared_ptr<Brain>>& brains) {
    #pragma omp parallel for
    for (int i = 0; i < brains.size(); ++i) {
        brains[i]->step(i);
    }
}
```

### Memory Optimization

```cpp
// Memory pools for frequently allocated objects
class SynapsePool {
    std::vector<std::unique_ptr<Synapse>> pool_;
    size_t current_index_ = 0;
public:
    Synapse* acquire() {
        if (current_index_ >= pool_.size()) {
            pool_.push_back(std::make_unique<Synapse>());
        }
        return pool_[current_index_++].get();
    }
    
    void reset() {
        current_index_ = 0;
    }
};
```

### Profiling

```bash
# Use gprof for profiling
g++ -g -O2 -pg src/main.cpp -o nlm_test
./nlm_test
/gprof nlm_test gmon.out > profile.txt

# Use perf (Linux)
sud perf record -g ./nlm_test
sud perf report
```

## Build System Details

### CMake Configuration

The project uses CMake for build configuration. Key features:

1. **Modern CMake**: Uses target-based approach
2. **Configurable Build Types**: Debug, Release, RelWithDebInfo, MinSizeRel
3. **Cross-platform**: Supports Linux, macOS, Windows
4. **Language Standards**: Enforces C++20

#### CMakeLists.txt Structure

```cmake
# Minimum CMake version
cmake_minimum_required(VERSION 3.16)

# Project declaration
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(ENABLE_TESTING "Build unit tests" ON)
option(ENABLE_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/brain)
add_subdirectory(src/agent)
add_subdirectory(src/world)
add_subdirectory(src/sensory)
add_subdirectory(src/motor)
add_subdirectory(src/environment)
add_subdirectory(src/experiments)

# Add executables
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PRIVATE nlm_core nlm_agent)

# Add Python bindings
if(ENABLE_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Add tests
if(ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
```

### Configuration System

NLM uses JSON-based configuration with multiple sources:

1. **Default Configuration**: Built-in defaults
2. **File Configuration**: JSON files
3. **Command Line**: Override from command line
4. **Programmatic**: Set at runtime

#### Configuration File Format

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "neuromod_dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
}
```

## Advanced Development Topics

### Multi-threading Considerations

NLM uses thread-safe patterns for parallel processing:

```cpp
// Thread-safe configuration
class ThreadSafeConfig {
    std::mutex mutex_;
    std::shared_ptr<Config> config_;
public:
    void set(const std::string& key, auto value) {
        std::lock_guard lock(mutex_);
        config_->set(key, value);
    }
    
    auto get(const std::string& key) const {
        std::lock_guard lock(mutex_);
        return config_->getOr(key, T{});
    }
};
```

### Custom Extensions

#### Adding New Neuron Dynamics

```cpp
// Create new neuron type
class AdaptiveLIFNeuron : public Neuron {
public:
    void update(double dt) override {
        // Custom adaptive dynamics
        // Include adaptation currents, threshold modulation, etc.
    }
    
    void enableAdaptation(bool enable) {
        adaptation_enabled_ = enable;
    }
};

// Register with factory
class NeuronFactory {
public:
    using NeuronCreator = std::function<std::unique_ptr<Neuron>()>;
    
    static void registerNeuronType(const std::string& name, NeuronCreator creator) {
        creators_[name] = std::move(creator);
    }
    
    static std::unique_ptr<Neuron> createNeuron(const std::string& name) {
        auto it = creators_.find(name);
        if (it != creators_.end()) {
            return it->second();
        }
        throw std::runtime_error("Unknown neuron type: " + name);
    }
    
private:
    static std::unordered_map<std::string, NeuronCreator> creators_;
};
```

### Testing Framework

NLM uses a custom test framework with parameterized tests:

```cpp
// Test case structure
class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool run(std::shared_ptr<Brain> brain) = 0;
    virtual bool isEnabled() const { return true; }
};

// Test runner
class TestRunner {
public:
    TestRunner(std::shared_ptr<Brain> brain);
    void registerTest(const std::string& name, 
                     std::function<bool(std::shared_ptr<Brain>)> testFunction,
                     const std::string& description = "");
    bool runTest(const std::string& name);
    bool runAllTests();
    std::vector<TestInfo> getTests() const;
    
    // Test selection
    void setTestSelection(const std::string& selection);
    void restoreOriginalTests();
    void storeOriginalTests();
};
```

## Conclusion

The NLM C++ framework provides a powerful foundation for neural computation research and development. It combines:

- **Performance**: Modern C++ with SIMD and parallel processing
- **Flexibility**: Modular architecture for custom extensions
- **Robustness**: Comprehensive testing and configuration management
- **Research**: Multiple plasticity mechanisms and cognitive capabilities

For the most up-to-date information, refer to:

- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)