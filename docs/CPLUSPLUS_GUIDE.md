# NLM C++ Development Guide

## Overview

This comprehensive guide provides C++ developers with everything they need to know to effectively develop, extend, and contribute to the NLM neural simulation framework. The guide covers installation, build configuration, coding standards, advanced features, and best practices for the C++ implementation.

## Installation and Setup

### Prerequisites

Before installing NLM, ensure you have:

- **CMake 3.22 or higher**
- **C++ compiler with C++20 support** (GCC 9+, Clang 10+, MSVC 2019+)
- **Python development headers** (optional, for Python bindings)
- **OpenMP support** (optional, for performance optimizations)

### Platform-Specific Setup

#### Linux/Ubuntu/Debian

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libomp-dev \
    python3-dev \
    python3-pip \
    valgrind \
    gdb

# Install Python dependencies (optional)
pip3 install pybind11 numpy matplotlib scipy
```

#### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake git omp python
```

#### Windows

```bash
# Install Visual Studio with C++ workload
# Download and install CMake from cmake.org
# Install Python from python.org

# Clone repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake (adjust paths as needed)
cmake .. -G "Visual Studio 16 2019" -A x64

# Build with MSBuild
msbuild ALL_BUILD.vcxproj
```

### Building from Source

#### Basic Build

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project (adjust parallel job count)
make -j$(nproc)

# Install locally (optional)
sudo make install  # or use local prefix
```

#### Build Types

```bash
# Debug build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# Release build (optimized)
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Development build (with extra debugging)
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make -j$(nproc)
```

#### Build Configuration Options

```bash
# Configure with specific options
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_TESTS=ON \
    -DENABLE_BENCHMARKS=ON \
    -DENABLE_PYTHON_BINDINGS=ON \
    -DPYTHON_EXECUTABLE=/usr/bin/python3 \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DNLM_ENABLE_OPENMP=ON \
    -DNLM_ENABLE_SIMD=ON \
    -DNLM_ENABLE_MEMORY_POOL=ON
```

### Building Python Bindings

#### Separate Python Build

```bash
# Create Python bindings build directory
cd build
cmake -DPYTHON_BINDING_REQUIRED=ON ..
make -j$(nproc)

# Install Python module
cmake --install . --prefix ./install
```

#### Using External pybind11

```bash
# Clone pybind11 (if not system-installed)
git clone https://github.com/pybind/pybind11.git
cd pybind11
cmake -B build -H.
cmake --build build --install

# Then build NLM with pybind11 path
cd ..
mkdir build
cd build
cmake -Dpybind11_DIR=/path/to/pybind11/build ..
make -j$(nproc)
```

### Running Tests

#### C++ Tests

```bash
# Run all tests
./bin/nlm_tests

# Run specific test category
./bin/nlm_tests --gtest_filter=NeuronTest.*

# Run performance tests
./bin/nlm_tests --gtest_filter=PerformanceTest.*

# Run with verbose output
./bin/nlm_tests --gtest_list_tests
```

#### Python Tests

```bash
# Activate Python environment
python3 -m venv venv
source venv/bin/activate

# Install dependencies
pip install pybind11 numpy matplotlib scipy

# Build and install Python module
cd python
cmake -B build -H.
cmake --build build
make install

# Run Python tests
python3 -m pytest
```

#### Benchmarks

```bash
# Run benchmarks
./bin/nlm_benchmarks

# Run specific benchmark
./bin/nlm_benchmarks --benchmark_filter=LIFBenchmark

# Generate benchmark report
./bin/nlm_benchmarks --benchmark_out=benchmark_results.json
```

### Build Outputs

#### Generated Files

```
build/
├── lib/                           # Libraries
│   ├── libnlm_core.a              # Core library
│   ├── libnlm_agent.a             # Agent library
│   ├── libnlm_world.a             # World library
│   └── libpynlm.cpython-310.so    # Python module
├── bin/                           # Executables
│   ├── nlm_tests                 # Test suite
│   ├── nlm_benchmarks            # Benchmark suite
│   └── simple_world_demo         # Example executable
├── docs/                          # Documentation
├── python/                        # Python module
└── ...                           # CMake generated files
```

## Programming with NLM

### Basic Structure

#### Creating a Simple Neural Network

```cpp
#include <nlm.h>
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 1000);
    config->set("neuron_count", 100);
    config->set("synapse_density", 0.1f);
    
    // Create brain with configuration
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    std::cout << "Starting simulation..." << std::endl;
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.01f);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                     << brain->getActiveNeuronCount() << " active neurons, "
                     << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    // Save results
    brain->save("results/brain_state.bin");
    
    return 0;
}
```

#### Creating a Custom Agent

```cpp
#include <nlm.h>
#include <iostream>

class CustomAgent {
public:
    CustomAgent(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world) {}
    
    void initialize() {
        agent_brain_ = std::make_shared<AgentBrain>(brain_, world_);
        agent_brain_->initialize(*world_);
    }
    
    ActionStep runStep() {
        // Get sensory input
        SensoryPercept percept = world_->getSensoryPercept(world_->getAgentBody());
        
        // Process input
        if (agent_brain_->processSensoryInput(percept)) {
            // Generate motor command
            Action action = agent_brain_->decodeMotorCommand();
            
            // Apply action
            ActionResult result = world_->applyMotorCommand(action, world_->getSimulationTime());
            
            // Learn from result
            agent_brain_->applyRewardModulation(result.getReward(), 0.0f);
            
            return ActionStep{action, result};
        }
        
        return ActionStep{Action(), ActionResult()};
    }
    
    void updateDevelopment(int timestep) {
        agent_brain_->updateDevelopment(timestep);
    }
    
    DevelopmentalStage getDevelopmentalStage() const {
        return agent_brain_->getDevelopmentalStage();
    }
    
    float getEnergy() const {
        return world_->getAgentBody().getEnergy();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_brain_;
};

int main() {
    // Create components
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 500);
    
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(50, 50, 10, 10);
    
    // Create agent
    CustomAgent agent(brain, world);
    agent.initialize();
    
    // Run simulation
    for (int step = 0; step < 500; ++step) {
        ActionStep result = agent.runStep();
        agent.updateDevelopment(step);
        
        brain->step(step, step * 0.01f);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ", Energy: " 
                     << agent.getEnergy() << ", Stage: "
                     << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        }
    }
    
    return 0;
}
```

### Advanced Features

#### 1. Using Context Managers

```cpp
#include <nlm.h>

void useContextManager() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("simulation_steps", 100);
    
    // Use brain as context manager
    {
        Brain brain(config);
        if (!brain.initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        // Do work with brain
        for (int step = 0; step < 100; ++step) {
            brain.step(step, step * 0.01f);
        }
        
        // Brain is automatically reset when going out of scope
        std::cout << "Brain automatically reset" << std::endl;
    }
    
    // Brain has been cleaned up
    std::cout << "Context manager usage completed" << std::endl;
}
```

#### 2. Custom Memory Pool

```cpp
#include <nlm.h>
#include <nlm/performance/MemoryPool.hpp>

class OptimizedBrain {
public:
    OptimizedBrain(const Config& config) {
        // Configure memory pool for large simulations
        performance::PerformanceConfig perfConfig;
        perfConfig.maxNeurons = 1000000;  // 1 million neurons
        perfConfig.maxSynapses = 10000000;  // 10 million synapses
        perfConfig.neuronPoolSize = 2 * perfConfig.maxNeurons;
        perfConfig.synapsePoolSize = 2 * perfConfig.maxSynapses;
        perfConfig.enableMemoryPool = true;
        
        auto& monitor = performance::PerformanceMonitor::getInstance(perfConfig);
        memoryPool_ = std::make_unique<performance::MemoryPool>(
            perfConfig.neuronPoolSize, perfConfig.synapsePoolSize
        );
        
        // Create brain with custom configuration
        brain_ = std::make_shared<Brain>(config);
    }
    
    void step(int step, float time) {
        // Use memory pool for efficient allocation
        if (memoryPool_) {
            // Custom memory allocation strategy
            // ...
        }
        
        brain_->step(step, time);
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<performance::MemoryPool> memoryPool_;
};
```

#### 3. Event-Driven Architecture

```cpp
#include <nlm.h>
#include <nlm/performance/EventQueue.hpp>

class EventDrivenSimulation {
public:
    EventDrivenSimulation(std::shared_ptr<Brain> brain) : brain_(brain) {
        // Configure event queue
        eventQueue_ = std::make_unique<performance::EventQueue>(16384);
        
        // Set up event handlers
        brain_->getSpikeSystem()->registerEventHandler(
            [this](NeuronId neuronId, Timestamp time) {
                onNeuronSpike(neuronId, time);
            }
        );
    }
    
    void run(int maxSteps) {
        brain_->initialize();
        
        for (int step = 0; step < maxSteps; ++step) {
            // Process pending events
            processEvents(step * 0.01f);
            
            // Run brain step
            brain_->step(step, step * 0.01f);
            
            // Deliver scheduled events
            deliverEvents(step * 0.01f);
            
            // Check for termination conditions
            if (shouldTerminate(step)) {
                break;
            }
        }
    }
    
private:
    void onNeuronSpike(NeuronId neuronId, Timestamp time) {
        // Schedule delayed events for this spike
        for (float delay : getDelayValues(neuronId)) {
            eventQueue_->pushDelayed(neuronId, time, delay);
        }
    }
    
    void processEvents(float currentTime) {
        auto events = eventQueue_->processEvents(currentTime);
        for (NeuronId neuronId : events) {
            // Handle neuron event
            handleNeuronEvent(neuronId, currentTime);
        }
    }
    
    void deliverEvents(float currentTime) {
        auto events = eventQueue_->deliverEvents(currentTime);
        for (NeuronId neuronId : events) {
            // Deliver spike to target neurons
            deliverSpike(neuronId, currentTime);
        }
    }
    
    // Other private methods...
    
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<performance::EventQueue> eventQueue_;
};
```

#### 4. SIMD Vectorization

```cpp
#include <nlm.h>
#include <nlm/performance/SIMDVectorization.hpp>

class SIMDProcessor {
public:
    SIMDProcessor() {
        // Check SIMD capabilities
        simd_ = std::make_unique<performance::SIMDVectorizer>();
        
        if (simd_->hasAVX512()) {
            std::cout << "Using AVX-512 (16 neurons per iteration)" << std::endl;
        } else if (simd_->hasAVX2()) {
            std::cout << "Using AVX2 (8 neurons per iteration)" << std::endl;
        } else if (simd_->hasSSE()) {
            std::cout << "Using SSE (4 neurons per iteration)" << std::endl;
        } else {
            std::cout << "Using scalar fallback" << std::endl;
        }
    }
    
    void processNeurons(std::vector<Neuron>& neurons, const std::vector<float>& currents,
                       Timestamp time, float dt) {
        // Use SIMD for vectorized LIF updates
        simd_->updateNeurons(neurons, currents, time, dt, 16);
        
        // Use SIMD for spike processing
        simd_->processSpikes(neurons, synapses, spikeEvents, time, dt);
    }
    
private:
    std::unique_ptr<performance::SIMDVectorizer> simd_;
};
```

#### 5. Parallel Processing

```cpp
#include <nlm.h>
#include <nlm/performance/ParallelProcessing.hpp>

class ParallelNeuralProcessor {
public:
    ParallelNeuralProcessor(size_t numThreads = std::thread::hardware_concurrency()) {
        processor_ = std::make_unique<performance::ParallelProcessor>(numThreads);
    }
    
    void processNeurons(std::vector<Neuron>& neurons, std::vector<Synapse>& synapses,
                       const EventQueue& spikeEvents, Timestamp time, float dt) {
        // Use parallel processing for neural computation
        processor_->processNeurons(neurons, synapses, spikeEvents, time, dt);
    }
    
    template<typename Iterator, typename Function>
    void parallelFor(Iterator begin, Iterator end, Function func) {
        processor_->parallelFor(begin, end, func);
    }
    
    template<typename Iterator, typename Function, typename Reduction>
    Reduction parallelForReduce(Iterator begin, Iterator end, Function func, Reduction init) {
        return processor_->parallelForReduce(begin, end, func, init);
    }
    
private:
    std::unique_ptr<performance::ParallelProcessor> processor_;
};
```

### Layer-Specific Development

#### Core Layer

```cpp
// Core layer development examples

// 1. Types and Enums
namespace nlm {
    enum class NeuronType { Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal };
    enum class SynapseType { Excitatory, Inhibitory, Modulatory, Electrical, GapJunction };
    enum class DevelopmentalStage { Initial, CriticalPeriod, Maturation, Adult, Aging };
    enum class FiringState { Resting, Active, Refractory, Inhibited };
}

// 2. Configuration System
class Config {
public:
    Config();
    ~Config();
    
    // Type-safe getters
    template<typename T>
    T get(const std::string& key, T defaultValue = T()) const;
    
    template<typename T>
    void set(const std::string& key, T value);
    
    // File operations
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Query operations
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
};
```

#### Brain Layer

```cpp
// Brain layer development examples

// 1. Brain class
class Brain {
public:
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Initialization and stepping
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Sensory and motor interfaces
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // State management
    void reset();
    void save(const std::string& filepath);
    void load(const std::string& filepath);
    
    // Query operations
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    float getAverageFiringRate() const;
    DevelopmentalStage getDevelopmentalStage() const;
};
```

#### Dynamics Layer

```cpp
// Dynamics layer development examples

// 1. Neural Dynamics base class
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    virtual void update(Neuron& neuron, float dt, Timestamp time) = 0;
    virtual std::string getName() const = 0;
};

// 2. LIF Dynamics implementation
class LIFDynamics : public NeuralDynamics {
public:
    LIFDynamics(float threshold = 1.0f, float reset = 0.0f, float tau = 20.0f);
    
    void update(Neuron& neuron, float dt, Timestamp time) override;
    std::string getName() const override { return "LIF"; }
    
    // Parameters
    float getThreshold() const { return threshold_; }
    void setThreshold(float threshold) { threshold_ = threshold; }
    
private:
    float threshold_;
    float reset_;
    float tau_;
};
```

#### Plasticity Layer

```cpp
// Plasticity layer development examples

// 1. PlasticityRule base class
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    virtual void apply(Synapse& synapse, const Neuron& pre, const Neuron& post) = 0;
    virtual std::string getName() const = 0;
};

// 2. STDP implementation
class STDP : public PlasticityRule {
public:
    STDP(float learningRate = 0.01f, float tauPre = 20.0f, float tauPost = 20.0f);
    
    void apply(Synapse& synapse, const Neuron& pre, const Neuron& post) override;
    std::string getName() const override { return "STDP"; }
    
    // Parameters
    void setLearningRate(float rate) { learningRate_ = rate; }
    float getLearningRate() const { return learningRate_; }
    
private:
    float learningRate_;
    float tauPre_;
    float tauPost_;
};
```

#### Development Layer

```cpp
// Development layer development examples

// 1. DevelopmentSystem class
class DevelopmentSystem {
public:
    DevelopmentSystem();
    
    // Stage management
    void setCurrentStage(DevelopmentalStage stage);
    DevelopmentalStage getCurrentStage() const;
    bool advanceStage();
    bool isInStage(DevelopmentalStage stage) const;
    
    // Plasticity modifiers
    float getPlasticityModulator(PlasticityType type) const;
    void setPlasticityModulator(PlasticityType type, float value);
    
    // Critical periods
    bool isInCriticalPeriod() const;
    float getCriticalPeriodFactor() const;
    
    // Update
    void update(Timestamp time, float dt);
    
private:
    DevelopmentalStage currentStage_;
    std::map<PlasticityType, float> plasticityModifiers_;
    bool inCriticalPeriod_;
    float criticalPeriodFactor_;
};
```

#### Memory Layer

```cpp
// Memory layer development examples

// 1. NeuralWorkingMemory class
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    
    // Persistent activity management
    void storePattern(const std::vector<NeuronId>& neurons, float strength = 1.0f);
    std::vector<NeuronId> retrievePattern(float threshold = 0.5f) const;
    void updatePattern(const std::vector<NeuronId>& neurons, float dt);
    
    // Competition and decay
    void applyWinnerTakeAll(float threshold);
    void decayAll(float rate);
    
private:
    std::vector<std::pair<std::vector<NeuronId>, float>> patterns_;
    float globalDecayRate_;
};
```

#### Prediction Layer

```cpp
// Prediction layer development examples

// 1. NeuralPrediction class
class NeuralPrediction {
public:
    NeuralPrediction();
    
    // Temporal sequence learning
    void learnSequence(const std::vector<Timestamp>& eventTimes, const std::vector<Action>& actions);
    std::vector<Action> predictFutureActions(Timestamp horizon, size_t steps = 10) const;
    
    // Multi-step prediction
    std::vector<Timestamp> predictEvents(Timestamp startTime, size_t horizon, size_t steps) const;
    
    // Error computation
    float computePredictionError(Timestamp predictedTime, Timestamp actualTime, Action actualAction) const;
    void updateWithError(float error);
    
private:
    std::vector<TemporalSequence> sequences_;
    float predictionAccuracy_;
};
```

#### Cognitive Layer

```cpp
// Cognitive layer development examples

// 1. AttentionalSelection class
class AttentionalSelection {
public:
    AttentionalSelection();
    
    // Competitive selection
    NeuronId selectTarget(const std::vector<NeuronId>& candidates, float topPercent = 0.1f) const;
    std::vector<float> computeSelectionWeights(const std::vector<NeuronId>& candidates) const;
    
    // Bias modulation
    void setTopDownBias(float bias);
    void setBottomUpBias(float bias);
    void updateWithNeuromodulation(float neuromodulationLevel);
    
private:
    float topDownBias_;
    float bottomUpBias_;
    float neuromodulationLevel_;
};
```

#### Sensory Layer

```cpp
// Sensory layer development examples

// 1. Vision class
class Vision : public SensoryInput {
public:
    Vision();
    Vision(size_t width, size_t height, size_t channels = 3);
    
    // Data management
    void setData(const std::vector<float>& data);
    const std::vector<float>& getData() const;
    void setData(const std::vector<std::vector<float>>& imageData);
    
    // Processing
    void applyFilter(const std::vector<float>& kernel);
    void enhanceContrast(float factor);
    void reduceNoise(float sigma);
    
private:
    size_t width_;
    size_t height_;
    size_t channels_;
    std::vector<float> data_;
};
```

#### Motor Layer

```cpp
// Motor layer development examples

// 1. Action class
class Action {
public:
    Action();
    Action(ActionType type);
    Action(ActionType type, const std::vector<float>& parameters);
    
    // Type management
    ActionType getType() const;
    void setType(ActionType type);
    const std::string& getTypeName() const;
    
    // Parameters
    const std::vector<float>& getParameters() const;
    void setParameters(const std::vector<float>& parameters);
    void addParameter(float value);
    size_t getParameterCount() const;
    float getParameter(size_t index) const;
    
    // Utility
    std::string toString() const;
    Action clone() const;
    
private:
    ActionType type_;
    std::vector<float> parameters_;
};
```

#### Environment Layer

```cpp
// Environment layer development examples

// 1. SimpleWorld class
class SimpleWorld {
public:
    SimpleWorld();
    
    // Configuration
    void configure(int width, int height, int visionWidth = 10, int visionHeight = 10);
    int getWidth() const;
    int getHeight() const;
    
    // Agent management
    void setAgentStart(float x, float y);
    void setAgent(const AgentBody& agent);
    AgentBody& getAgent();
    const AgentBody& getAgent() const;
    
    // Object management
    void addObject(const WorldObject& object);
    void removeObject(float x, float y);
    std::vector<WorldObject> getObjectsInRange(float x, float y, float range) const;
    
    // Physics simulation
    void update(float dt);
    void applyPhysics(float dt);
    void resolveCollisions();
    
    // Utilities
    bool isValidPosition(float x, float y) const;
    float distance(float x1, float y1, float x2, float y2) const;
    float angle(float x1, float y1, float x2, float y2) const;
    
private:
    int width_;
    int height_;
    AgentBody agent_;
    std::vector<WorldObject> objects_;
    float simulationTime_;
};
```

#### Experiment Layer

```cpp
// Experiment layer development examples

// 1. Experiment class
class Experiment {
public:
    Experiment();
    Experiment(const std::string& name);
    
    // Identification
    const std::string& getName() const;
    void setName(const std::string& name);
    ExperimentId getId() const;
    
    // Data collection
    void recordStep(int step, const Brain& brain, const SimpleWorld& world);
    void recordMetrics(const std::map<std::string, float>& metrics);
    std::map<std::string, float> getMetrics() const;
    std::vector<std::pair<int, std::map<std::string, float>>> getHistory() const;
    
    // Analysis
    float getMetric(const std::string& name) const;
    float getAverage(const std::string& name) const;
    float getStandardDeviation(const std::string& name) const;
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    std::string name_;
    ExperimentId id_;
    std::vector<std::pair<int, std::map<std::string, float>>> history_;
};
```

## Code Style and Conventions

### C++ Style Guide

#### Naming Conventions

```cpp
// Class names: CamelCase
class NeuralNetwork {
public:
    void initialize();
    void simulate(float dt);
};

// Method names: camelCase
void NeuralNetwork::addLayer(Layer* layer);

// Variables: camelCase
int numNeurons;
float synapticWeight;

// Constants: UPPER_SNAKE_CASE
const float PI = 3.14159265f;
const int MAX_SYNAPSES = 1000000;

// Enums: CamelCase
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};
```

#### File Organization

```cpp
// Include guards
#ifndef NLM_BRAIN_HPP
#define NLM_BRAIN_HPP

// Include order
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"

// Using declarations
using nlm::NeuronId;
using nlm::RegionId;
```

#### Code Structure

```cpp
// Function organization
namespace nlm {
    // Core components
    class Config;
    class Brain;
    
    // Layer components
    namespace core {
        class RandomGenerator;
        class SimulationClock;
    }
    
    namespace brain {
        class Neuron;
        class Synapse;
        class NeuralPopulation;
    }
    
    // More layers...
}

// Implementation details
class nlm::Config {
public:
    Config() = default;
    ~Config() = default;
    
    // Operations
    void set(const std::string& key, int value);
    int get(const std::string& key) const;
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
private:
    std::map<std::string, std::variant<int, float, std::string, bool>> data_;
};
```

#### Comments and Documentation

```cpp
/**
 * @class Config
 * @brief Manages configuration settings for NLM components.
 * 
 * The Config class provides a type-safe interface for storing and retrieving
 * configuration parameters. It supports basic types (int, float, string, bool)
 * and provides file I/O capabilities for persistence.
 * 
 * @note Configuration keys are case-sensitive strings. Type safety is enforced
 *       at the interface level, with appropriate conversion when possible.
 * 
 * @see https://nlm.readthedocs.io for more information
 */
class Config {
public:
    /**
     * @brief Creates a new configuration object.
     * 
     * Initializes an empty configuration that can be populated with
     * various parameter types.
     */
    Config() = default;
    
    /**
     * @brief Destroys the configuration object.
     */
    ~Config() = default;
    
    /**
     * @brief Sets a configuration parameter.
     * 
     * @tparam T Type of the parameter (int, float, string, or bool)
     * @param key Configuration parameter name
     * @param value Parameter value to store
     * 
     * @throws std::bad_variant_access if type T is not supported
     */
    template<typename T>
    void set(const std::string& key, T value);
    
    /**
     * @brief Retrieves a configuration parameter.
     * 
     * @tparam T Expected parameter type (must match what was set)
     * @param key Configuration parameter name
     * @param defaultValue Value to return if key doesn't exist
     * @return Parameter value, or defaultValue if key not found
     * 
     * @note If the key doesn't exist, returns defaultValue without throwing.
     *       For required parameters, check has() first.
     */
    template<typename T>
    T get(const std::string& key, T defaultValue = T()) const;
    
    /**
     * @brief Checks if a configuration parameter exists.
     * 
     * @param key Configuration parameter name
     * @return true if parameter exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Loads configuration from a file.
     * 
     * @param filepath Path to configuration file (JSON, TOML, or custom format)
     * @return true if file loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Saves configuration to a file.
     * 
     * @param filepath Path to save configuration file
     * @return true if file saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Gets all configuration keys.
     * 
     * @return Vector of all configuration parameter names
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clears all configuration parameters.
     */
    void clear();
    
    /**
     * @brief Gets a human-readable summary of configuration.
     * 
     * @return String containing configuration information
     */
    std::string summary() const;
    
    /**
     * @brief Validates configuration for errors.
     * 
     * @return Map of parameter names to error messages for invalid parameters
     */
    std::map<std::string, std::string> validate() const;
    
private:
    std::map<std::string, std::variant<int, float, std::string, bool>> data_;
};
```

#### Error Handling

```cpp
#include <stdexcept>
#include <string>

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message)
        : std::runtime_error(message) {}
};

class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& message)
        : std::runtime_error(message) {}
};

class NeuronError : public std::runtime_error {
public:
    explicit NeuronError(const std::string& message)
        : std::runtime_error(message) {}
};

// Usage example
try {
    Config config;
    config.set("invalid_key", nullptr);  // Type error
} catch (const ConfigError& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
}
```

### Testing Best Practices

#### Unit Testing

```cpp
#include <gtest/gtest.h>
#include <nlm.h>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<Config>();
    }
    
    void TearDown() override {
        config_.reset();
    }
    
    std::shared_ptr<Config> config_;
};

TEST_F(ConfigTest, DefaultConstruction) {
    Config config;
    EXPECT_TRUE(config.getKeys().empty());
}

TEST_F(ConfigTest, BasicSetGet) {
    config_->set("test_int", 42);
    EXPECT_TRUE(config_->has("test_int"));
    EXPECT_EQ(config_->get<int>("test_int"), 42);
    EXPECT_EQ(config_->get("test_int", 0), 42);
}

TEST_F(ConfigTest, TypeSafety) {
    config_->set("test_int", 42);
    config_->set("test_float", 3.14f);
    config_->set("test_string", "hello");
    config_->set("test_bool", true);
    
    EXPECT_EQ(config_->get<int>("test_int"), 42);
    EXPECT_DOUBLE_EQ(config_->get<double>("test_float"), 3.14);
    EXPECT_EQ(config_->get<std::string>("test_string"), "hello");
    EXPECT_TRUE(config_->get<bool>("test_bool"));
}
```

#### Integration Testing

```cpp
#include <gtest/gtest.h>
#include <nlm.h>

class BrainIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = std::make_shared<Config>();
        config_->set("simulation_steps", 100);
        config_->set("neuron_count", 10);
        
        brain_ = std::make_shared<Brain>(config_);
        brain_->initialize();
    }
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

TEST_F(BrainIntegrationTest, BasicStep) {
    brain_->step(0, 0.0f);
    EXPECT_GT(brain_->getTotalNeuronCount(), 0);
}

TEST_F(BrainIntegrationTest, SensoryInput) {
    Vision vision(10, 10, 3);
    std::vector<float> data(300, 0.5f);
    vision.setData(data);
    
    brain_->receiveSensoryInput(vision);
    brain_->step(1, 0.01f);
    
    EXPECT_GT(brain_->getActiveNeuronCount(), 0);
}
```

#### Performance Testing

```cpp
#include <gtest/gtest.h>
#include <nlm/performance/PerformanceMonitor.hpp>

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        performance::PerformanceConfig perfConfig;
        perfConfig.maxNeurons = 1000;
        perfConfig.maxSynapses = 10000;
        perfConfig.enableSIMD = true;
        perfConfig.numThreads = 1;
        
        monitor_ = std::make_shared<performance::PerformanceMonitor>(perfConfig);
    }
    
    std::shared_ptr<performance::PerformanceMonitor> monitor_;
};

TEST_F(PerformanceTest, LFIBenchmark) {
    double expectedTime = 0.001;  // seconds per neuron
    
    // Measure performance
    double measuredTime = monitor_->getAverageTime("lif_update");
    
    // Assert performance requirement
    ASSERT_LT(measuredTime, expectedTime * 100);  // 100x tolerance for test environment
}
```

## Build System Configuration

### CMake Configuration

#### Core Configuration

```cmake
# Core CMake configuration
cmake_minimum_required(VERSION 3.22)
project(NLM VERSION 0.5.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(CMAKE_COMPILER_IS_GNUC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
elseif(MSVC)
    add_compile_options(/W4 /permissive-)
endif()

# Find packages
find_package(Threads REQUIRED)
find_package(OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Installation paths
include(GNUInstallDirs)

# Build configurations
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
```

#### Feature Options

```cmake
# Optional features
option(ENABLE_TESTS "Build tests" ON)
option(ENABLE_BENCHMARKS "Build benchmarks" ON)
option(ENABLE_PYTHON_BINDINGS "Build Python bindings" ON)
option(ENABLE_OPENMP "Enable OpenMP support" ON)
option(ENABLE_SIMD "Enable SIMD optimizations" ON)
option(ENABLE_MEMORY_POOL "Enable memory pools" ON)
option(ENABLE_PROFILING "Enable profiling support" OFF)

# Python configuration
if(ENABLE_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 REQUIRED CONFIG)
endif()
```

#### Target Configuration

```cmake
# Core library
add_library(nlm_core STATIC
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # ... other core sources
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(nlm_core PUBLIC cxx_std_20)

# Brain library
add_library(nlm_brain STATIC
    src/brain/Brain.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    # ... other brain sources
)

target_link_libraries(nlm_brain PUBLIC nlm_core)

# Agent library
add_library(nlm_agent STATIC
    src/agent/AgentBrain.cpp
    src/agent/SensoryPercept.cpp
    src/agent/AgentBody.cpp
    # ... other agent sources
)

target_link_libraries(nlm_agent PUBLIC nlm_brain)

# World library
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
    src/world/WorldObject.cpp
    # ... other world sources
)

target_link_libraries(nlm_world PUBLIC nlm_brain)

# Python bindings
if(ENABLE_PYTHON_BINDINGS)
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
endif()
```

#### Installation Configuration

```cmake
# Install libraries
install(TARGETS nlm_core
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS nlm_brain
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS nlm_agent
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS nlm_world
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
)

# Install Python module
if(ENABLE_PYTHON_BINDINGS)
    install(TARGETS pynlm
        LIBRARY DESTINATION ${Python_SITEARCH}
        ARCHIVE DESTINATION ${Python_SITEARCH}
        RUNTIME DESTINATION ${Python_SITEARCH}
    )
endif()

# Install configuration
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    DESTINATION ${Python_SITEARCH}/nlm/cmake
)
```

### Build Commands

#### Basic Build

```bash
# Create build directory and configure
mkdir build && cd build
cmake ..

# Build all targets
make -j$(nproc)

# Build specific target
make nlm_brain
```

#### Cross-Compilation

```bash
# Cross-compile for ARM
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/arm.cmake ..
make -j$(nproc)

# Cross-compile for Windows
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/windows.cmake ..
make -j$(nproc)
```

#### Package Creation

```bash
# Create DEB package (Debian/Ubuntu)
cmake -DBUILD_DEB=ON ..
make package

# Create RPM package (Fedora/RHEL/CentOS)
cmake -DBUILD_RPM=ON ..
make package

# Create TGZ archive
make DESTDIR=./install package
```

## Development Tools

### IDE Configuration

#### VS Code

Create `.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "NLM Debug",
            "compilerPath": "/usr/bin/g++",
            "intellisenseMode": "gcc-x64",
            "includePath": [
                "${workspaceFolder}/src",
                "${workspaceFolder}/include"
            ],
            "defines": [
                "NLM_VERSION=0.5.0",
                "NLM_ENABLE_TESTS",
                "NLM_ENABLE_PYTHON_BINDINGS"
            ],
            "browse": {
                "path": [
                    "${workspaceFolder}/src"
                ]
            }
        }
    ]
}
```

Create `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build NLM",
            "type": "shell",
            "command": "mkdir -p build && cd build && cmake .. && make -j$(nproc)",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "Run Tests",
            "type": "shell",
            "command": "cd build && ./bin/nlm_tests",
            "group": "test"
        }
    ]
}
```

#### CLion

Create `CMakeLists.txt` for IntelliJ IDEA:

```cmake
# Add to main CMakeLists.txt
cmake_minimum_required(VERSION 3.22)
project(NLM VERSION 0.5.0 LANGUAGES CXX)

# Enable CMake tools integration
include(${CMAKE_BINARY_DIR}/cmake/ide.cmake OPTIONAL)

# Code analysis
include(CMakeLint.cmake)
```

### Code Analysis

#### Clang-Tidy

```bash
# Run clang-tidy on source files
cd build
make nlm_core

# Run clang-tidy checks
clang-tidy-12 src/brain/Neuron.cpp -- -I./src -I/usr/include -std=c++20
```

Create `clang-tidy` configuration file:

```ini
Checks: "-
    cppcoreguidelines-*
    modernize-*
    performance-*
    portability-*
    readability-*
    google-*
    hicpp-*
    mpi-*
    ,
    cppcoreguidelines-avoid-magic-numbers,
    cppcoreguidelines-avoid-c-arrays,
    cppcoreguidelines-avoid-cout,
    cppcoreguidelines-avoid-do-while,
    cppcoreguidelines-interfaces-global-init,
    cppcoreguidelines-owning-resources,
    cppcoreguidelines-special-member-functions,
    modernize-avoid-c-arrays,
    modernize-make-shared,
    modernize-use-auto,
    modernize-use-nullptr,
    performance-unnecessary-value-copy,
    readability-avoid-const-params,
    readability-container-size-simple,
    readability-function-size,
    readability-named-parameter,
    readability-redundant-access-specifiers,
    readability-simplify-logical-ops,
    readability-static-definitions,
    readability-unique-objects,
"
```

#### Clang-Format

```bash
# Format source files
find src include -name "*.cpp" -o -name "*.hpp" -o -name "*.h" |
    xargs clang-format-12 -i

# Create .clang-format configuration
```

### Code Coverage

#### Coverage Requirements

```ini
# .coveragerc configuration
[run]
    source = src
    omit =
        */tests/*
        */benchmarks/*
        */examples/*

[report]
    exclude_lines =
        pragma: no cover
        def __repr__
        if __name__ == .__main__.:.
        raise AssertionError
        raise NotImplementedError
    include = 
        src/*
    show_missing = true
    precision = 2
```

#### Coverage Commands

```bash
# Generate coverage report
cd build
make nlm_tests
./bin/nlm_tests --gtest_filter="*Test.*" --gtest_brief=1

# Generate code coverage
cd build
gcovr --root ../ --exclude tests --exclude benchmarks --exclude examples

# Generate HTML report
cd build
gcovr --root ../ --exclude tests --exclude benchmarks --exclude examples --html --html-details=coverage.html
```

### Documentation Generation

#### Doxygen

```cmake
# Add to CMakeLists.txt
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)
    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generate Doxygen documentation"
    )
endif()
```

#### Sphinx

```cmake
# Add to Python bindings CMakeLists.txt
find_package(Python REQUIRED COMPONENTS Interpreter Development MODULE)

# Add to Python module target
pybind11_add_module(pynlm python/bindings.cpp)

# Install documentation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../docs/python_api.rst
    DESTINATION ${Python_SITEARCH}/nlm
)
```

### Static Analysis

#### SonarQube

```bash
# Analyze with SonarQube
sonar-scanner \
    -Dsonar.projectKey=NLM \
    -Dsonar.projectName="NLM Neural Learning Machine" \
    -Dsonar.sources=src \
    -Dsonar.tests=build/test \
    -Dsonar.cxx.cppcheck.includePaths=src \
    -Dsonar.cxx.cppcheck.defines=NLM_VERSION=0.5.0
```

## Performance Optimization

### Optimization Guidelines

#### 1. Memory Optimization

```cpp
// Use memory pools for large simulations
#include <nlm/performance/MemoryPool.hpp>

// Configure memory pool
performance::PerformanceConfig config;
config.maxNeurons = 1000000;
config.maxSynapses = 10000000;
config.enableMemoryPool = true;

// Create memory pool
auto memoryPool = std::make_shared<performance::MemoryPool>(
    config.maxNeurons, config.maxSynapses
);

// Use memory pool for neuron allocation
NeuronId neuronId = memoryPool->allocateNeuron();
Neuron& neuron = memoryPool->getNeuron(neuronId);
```

#### 2. CPU Optimization

```cpp
// Use SIMD for vectorized operations
#include <nlm/performance/SIMDVectorization.hpp>

// Configure SIMD processor
performance::SIMDVectorizer simd;

// Use SIMD for LIF updates
std::vector<Neuron> neurons(1000);
std::vector<float> currents(1000, 1.0f);
simd.updateNeurons(neurons, currents, time, dt);
```

#### 3. Parallel Processing

```cpp
// Use parallel processing for simulation
#include <nlm/performance/ParallelProcessing.hpp>

// Configure parallel processor
performance::ParallelProcessor processor(std::thread::hardware_concurrency());

// Use parallel processing for neural computation
processor.processNeurons(neurons, synapses, spikeEvents, time, dt);
```

#### 4. I/O Optimization

```cpp
// Use efficient serialization formats
#include <nlohmann/json.hpp>

// JSON serialization
void saveBrainToJSON(const Brain& brain, const std::string& filepath) {
    nlohmann::json j;
    
    // Convert brain state to JSON
    j["neurons"] = brain.getNeuronCount();
    j["synapses"] = brain.getTotalSynapseCount();
    j["firing_rate"] = brain.getAverageFiringRate();
    
    // Write to file
    std::ofstream file(filepath);
    file << j.dump(2);
}

// Binary serialization for performance
void saveBrainToBinary(const Brain& brain, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing");
    }
    
    // Efficient binary write
    brain.serialize(file);
}
```

### Profiling and Benchmarking

#### Profiling

```bash
# Profile with gprof
cd build
make prof
./bin/nlm_tests --gtest_filter="*Test.*"
gprof ./bin/nlm_tests gmon.out

# Profile with perf (Linux)
cd build
perf record ./bin/nlm_tests
perf report

# Profile with valgrind
valgrind --tool=callgrind ./bin/nlm_tests --gtest_filter="*Test.*"
callgrind_annotate callgrind.out.12345
```

#### Benchmarking

```cpp
#include <nlm/performance/PerformanceMonitor.hpp>
#include <benchmark/benchmark.h>

// Custom benchmark
static void BM_LIFUpdate(benchmark::State& state) {
    // Setup
    std::vector<Neuron> neurons(state.range(0));
    std::vector<float> currents(state.range(0), 1.0f);
    
    // Benchmark
    for (auto _ : state) {
        // Process neurons
        for (size_t i = 0; i < neurons.size(); ++i) {
            neurons[i].stepLIF(0.0f, 0.01f);
        }
    }
    
    state.SetItemsProcessed(state.range(0) * state.iterations());
}

BENCHMARK(BM_LIFUpdate)->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_MAIN();
```

## Security Considerations

### Secure Coding Practices

#### Input Validation

```cpp
// Validate all inputs
void validateSensoryInput(const SensoryInput& input) {
    if (!input.isValid()) {
        throw std::invalid_argument("Invalid sensory input");
    }
    
    const auto& data = input.getData();
    if (data.size() > MAX_INPUT_SIZE) {
        throw std::length_error("Input data too large");
    }
}
```

#### Memory Safety

```cpp
// Use smart pointers
std::shared_ptr<Config> config = std::make_shared<Config>();
std::unique_ptr<Brain> brain = std::make_unique<Brain>(config);

// Use bounds-checked arrays
std::vector<float> safeArray(1000);
for (size_t i = 0; i < 1000; ++i) {
    safeArray[i] = getSafeValue(i);  // Bounds-checked
}
```

#### Error Handling

```cpp
// Comprehensive error handling
try {
    Config config;
    config.set("key", value);
    
    Brain brain(config);
    brain.initialize();
    
    brain.step(0, 0.0f);
    
} catch (const std::invalid_argument& e) {
    std::cerr << "Invalid argument: " << e.what() << std::endl;
    return 1;
} catch (const std::runtime_error& e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
    return 1;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
    return 1;
}
```

## Migration Guide

### From Version 0.4.x to 0.5.0

#### Breaking Changes

1. **C++ Standard**: Updated to C++20
2. **Memory Management**: Changed from raw pointers to smart pointers
3. **Configuration**: Enhanced type safety
4. **Python Bindings**: Improved error handling and documentation

#### Migration Steps

```cpp
// Old code (0.4.x)
Brain brain;
brain.initialize();

// New code (0.5.0)
auto config = std::make_shared<Config>();
Brain brain(config);
brain.initialize();
```

### From C++ to Python

#### C++ to Python Migration Example

```cpp
// C++ version
#include <nlm.h>

int main() {
    Config config;
    config.set("steps", 100);
    
    Brain brain(config);
    brain.initialize();
    
    for (int i = 0; i < 100; ++i) {
        brain.step(i, i * 0.01f);
    }
    
    return 0;
}
```

```python
# Python equivalent
import pynlm

config = pynlm.create_brain_config("migration_test", 100)
brain = pynlm.create_brain(config)
brain.initialize()

for step in range(100):
    brain.step(step, step * 0.01)
```

## Conclusion

The NLM C++ development framework provides a comprehensive foundation for neural simulation research and development. This guide covers all aspects of working with the NLM codebase, from basic installation to advanced optimization techniques.

Key takeaways:

1. **Modern C++**: Use C++20 features and modern C++ practices
2. **Memory Safety**: Prefer smart pointers and memory pools for large simulations
3. **Performance**: Use SIMD, parallel processing, and efficient I/O
4. **Testing**: Comprehensive unit and integration testing
5. **Documentation**: Maintain clear documentation and comments
6. **Security**: Validate inputs and handle errors properly
7. **Optimization**: Profile and optimize critical paths
8. **Maintainability**: Follow code style guidelines and keep code clean

The NLM framework is designed to be:
- **Extensible**: Easy to add new components and features
- **Performant**: Optimized for large-scale neural simulations
- **Flexible**: Support various neural architectures and models
- **Robust**: Comprehensive error handling and validation
- **Documented**: Complete API documentation and examples

Happy developing with NLM!
