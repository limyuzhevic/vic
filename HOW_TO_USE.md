# NLM (Neural Learning Machine) - C++ Documentation

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
| `nlm_test` | Executable | Unit test suite |
| `libnlm_core.a` | Static Library | Core brain components |
| `libnlm_agent.a` | Static Library | Agent system |
| `libnlm_world.a` | Static Library | World simulation |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Part 2: Using the NLM C++ API

### Basic Usage

#### Creating a Brain

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

// Create a default configuration
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
config->set("region_count", 1);

// Create and initialize the brain
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
```

#### Running a Simulation

```cpp
// Run simulation for 1000 steps
for (int step = 0; step < 1000; ++step) {
    brain->step(step);
    
    // Log statistics periodically
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
    }
}
```

#### Creating a Simple World

```cpp
#include "world/SimpleWorld.hpp"

// Create a simple 2D world
auto world = std::make_unique<nlm::SimpleWorld>();

// Configure world dimensions
world->initialize(16, 16);
world->reset();

// Set agent starting position
world->setAgentStart(10.0, 10.0);

// Run world update
world->update(0.1);
```

#### Complete Agent Example

```cpp
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

def run_agent_simulation(int num_steps = 1000) {
    // 1. Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // 2. Create brain and initialize
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // 3. Create agent brain interface
    auto agent = std::make_unique<nlm::AgentBrain>(brain);
    
    // 4. Create and configure world
    auto world = std::make_unique<nlm::SimpleWorld>();
    world->initialize(16, 16);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // 5. Initialize agent with world
    agent->initialize(*world);
    
    // 6. Enable subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // 7. Run simulation loop
    for (int step = 0; step < num_steps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input from world
        auto percept = world->observe(brain->getRegions()[0].get());
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command from brain activity
        auto motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyAction(brain->getRegions()[0].get(), motor_cmd);
        
        // Apply reward modulation
        auto reward = world->computeReward(brain->getRegions()[0].get());
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Print progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << "Firing neurons: " << brain->getFiringNeuronCount() << " "
                      << "Curiosity: " << agent->getCuriosityLevel() << " "
                      << "Novelty: " << agent->getNoveltyLevel() << " "
                      << "Dev Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        }
    }
}
```

### Key Classes and Methods

#### Brain Class

```cpp
class Brain {
public:
    // Creation and initialization
    Brain(std::shared_ptr<Config> config);
    bool initialize();
    void reset();
    
    // Simulation
    void step(int step);           // Run one simulation step
    void step(int step, double time);  // Run step with timestamp
    
    // Input/Output
    void receiveSensoryInput(const SensoryPercept& percept);
    MotorCommand produceAction();  // Get motor action
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    double getAverageFiringRate() const;
    double getExcitationInhibitionRatio() const;
    
    // State
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
    
    // Regions
    RegionId addRegion(const std::string& name);
    BrainRegion* getRegion(RegionId id);
    
    // Systems
    WorkingMemory* getWorkingMemory() const;
    EpisodicMemory* getEpisodicMemory() const;
    DopamineSystem* getDopamine() const;
    CuriositySystem* getCuriosity() const;
    NoveltySystem* getNovelty() const;
    PredictionSystem* getPredictionSystem() const;
    Planner* getPlanner() const;
    ConceptFormation* getConceptFormation() const;
    Attention* getAttention() const;
    DevelopmentSystem* getDevelopmentSystem() const;
    
    // Development
    void setDevelopmentalStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Debug
    void logStatus() const;
};
```

#### Config Class

```cpp
class Config {
public:
    Config();
    ~Config();
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Command line
    bool loadFromArgs(int argc, char** argv);
    
    // Query
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Modify
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
};
```

#### AgentBrain Class

```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialization
    void initialize(const SimpleWorld& world);
    
    // Sensory processing
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    MotorCommand decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    
    // Development
    void updateDevelopment(double timestep);
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Subsystem enable/disable
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Statistics
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Reset
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
};
```

#### SimpleWorld Class

```cpp
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Creation
    static std::unique_ptr<SimpleWorld> create();
    
    // Configuration
    void initialize(int width, int height);
    void configure(int width, int height, int visionWidth = 8, int visionHeight = 8);
    
    // Control
    void reset();
    void update(double timestep);
    void setRandomSeed(uint64_t seed);
    
    // Agent interaction
    void setAgentStart(double x, double y);
    void applyAction(Brain* brain, MotorCommand cmd);
    
    // State access
    SensoryPercept observe(Brain* brain);
    Body* getAgentBody();
    
    // World objects
    void addObject(const WorldObject& object);
    void removeObject(int x, int y);
    bool isValidPosition(int x, int y);
    
    // Properties
    int getWidth() const;
    int getHeight() const;
    double getSimulationTime() const;
    double getMaxEnergy() const;
    void setMaxEnergy(double energy);
};
```

### Enumerations

#### Neuron Types

```cpp
enum class NeuronType {
    Excitatory,     // Standard LIF neuron with positive weights
    Inhibitory,     // LIF neuron that suppresses activity
    Sensory,        // Receives external input
    Motor,          // Produces motor output
    Modulatory,      // Affects plasticity and excitability
    Internal        // Internal state representation
};
```

#### Developmental Stages

```cpp
enum class DevelopmentalStage {
    Initial,        // Newborn - high plasticity
    CriticalPeriod, // Sensitive period - rapid learning
    Maturation,     // Child - developing skills
    Adult,          // Mature - stable performance
    Aging           // Old age - declining plasticity
};
```

#### Action Types

```cpp
enum class MotorCommand {
    MoveForward,    // Move forward in current direction
    MoveBackward,   // Move backward
    TurnLeft,       // Rotate left
    TurnRight,      // Rotate right
    LookLeft,       // Look left direction
    LookRight,      // Look right direction
    Interact,       // Interact with nearby object
    Eat,            // Consume resource
    Wait,           // No action taken
    Attack,         // Attack nearby target
    Explore,        // Random movement
    Learn           // Learn from environment
};
```

#### World Object Types

```cpp
enum class WorldObjectType {
    Empty,          // Empty space
    Resource,       // Consumable resource
    Hazard,         // Dangerous object
    Wall,           // Impassable barrier
    Marker,         // Special location
};
```

### Configuration Options

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.15 | Probability of connection between neurons |
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse removal |
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |
| `reward_discount_factor` | float | 0.99 | Reward discount factor |

#### Environment Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment_name` | string | "GridWorld" | Environment type |
| `environment_width` | int | 16 | World width |
| `environment_height` | int | 16 | World height |
| `visualization_enabled` | bool | false | Enable visualization |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_reward_modulation` | bool | true | Enable reward-based learning |
| `enable_curiosity` | bool | true | Enable curiosity-driven exploration |
| `enable_novelty_detection` | bool | true | Enable novelty detection |
| `curiosity_threshold` | float | 0.3 | Curiosity activation threshold |

#### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_development` | bool | true | Enable developmental changes |
| `development_duration` | double | 600.0 | Total development time |
| `development_synaptogenesis_rate` | float | 0.001 | Synaptogenesis rate during development |
| `development_pruning_rate` | float | 0.0001 | Pruning rate during development |

#### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `enable_checkpointing` | bool | true | Enable checkpoint save/load |
| `enable_replay` | bool | true | Enable memory replay |
| `max_memory_episodes` | int | 100 | Maximum stored episodes |
| `working_memory_size` | int | 50 | Working memory capacity |

### Command Line Interface

The NLM executable supports the following command line options:

```bash
./nlm [OPTIONS]

Basic Options:
  --help, -h                    Show this help message
  --version, -v                 Show version information
  --config <file>              Load configuration from file (default: configs/default.cfg)
  --batch, -b <steps>           Run batch simulation with specified steps
  --benchmark                   Run performance benchmark
  --demo                        Run Phase 6 integration demo
  --quick-test                  Run quick integration verification
  --memory-test                 Test memory integration
  --neuromod-test               Test neuromodulation integration
  --checkpoint-test             Test checkpoint save/load
  --replay-test                 Test replay system

Simulation Options:
  --steps <num>                 Number of simulation steps (default: 1000)
  --neurons <num>               Number of neurons (default: 500)
  --regions <num>               Number of brain regions (default: 1)
  --seed <seed>                 Random seed (default: 42)
  --timestep <time>             Simulation timestep (default: 0.001)

Output Options:
  --quiet, -q                   Minimal output
  --verbose, -V                 Detailed output
  --progress <interval>         Show progress every N steps
  --metrics <file>              Export metrics to file

Environment Options:
  --enable-checkpointing        Enable checkpoint save/load
  --enable-replay               Enable memory replay
  --enable-development          Enable developmental changes
  --enable-curiosity            Enable curiosity-driven exploration
  --enable-reward-modulation    Enable reward-based learning

Advanced Options:
  --performance-mode            Optimized for maximum performance
  --memory-mode                 Optimized for memory usage
  --adaptive-timestep          Use adaptive timestep adjustment
```

### Example Programs

#### Example 1: Minimal Brain Simulation

```cpp
#include "brain/Brain.hpp"
#include <iostream>

int main() {
    // Create and initialize
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Simulate
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    std::cout << "Simulation complete!" << std::endl;
    std::cout << "Neurons firing: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
```

#### Example 2: Environment Interaction

```cpp
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>

int main() {
    // Setup
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_unique<nlm::AgentBrain>(brain);
    auto world = std::make_unique<nlm::SimpleWorld>();
    world->initialize(10, 10);
    world->reset();
    agent->initialize(*world);
    
    // Run episode
    for (int step = 0; step < 500; ++step) {
        world->update(0.1);
        auto percept = world->observe(brain->getRegions()[0].get());
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyAction(brain->getRegions()[0].get(), action);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    std::cout << "Agent simulation complete!" << std::endl;
    return 0;
}
```

#### Example 3: Complete Agent with Learning

```cpp
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>

void run_advanced_simulation() {
    // Create configuration with advanced settings
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("enable_reward_modulation", true);
    config->set("enable_curiosity", true);
    config->set("enable_development", true);
    config->set("enable_checkpointing", true);
    
    // Create brain and agent
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_unique<nlm::AgentBrain>(brain);
    auto world = std::make_unique<nlm::SimpleWorld>();
    world->initialize(20, 20, 8, 8);
    world->reset();
    agent->initialize(*world);
    
    // Enable advanced features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Run advanced simulation
    for (int step = 0; step < 2000; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->observe(brain->getRegions()[0].get());
        
        // Process and act
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyAction(brain->getRegions()[0].get(), action);
        
        // Learning
        auto reward = world->computeReward(brain->getRegions()[0].get());
        agent->applyRewardModulation(reward, 0.0);
        agent->updateDevelopment(0.1);
        
        // Detailed logging
        if (step % 200 == 0) {
            std::cout << "=== Step " << step << " ===" << std::endl;
            std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
            std::cout << "Curiosity level: " << agent->getCuriosityLevel() << std::endl;
            std::cout << "Novelty level: " << agent->getNoveltyLevel() << std::endl;
            std::cout << "Developmental stage: " 
                      << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            std::cout << "Working memory: " 
                      << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        }
    }
    
    std::cout << "=== Advanced simulation complete ===" << std::endl;
}

int main() {
    run_advanced_simulation();
    return 0;
}
```

### Building and Running Tests

After building with CMake, you can run the test suite:

```bash
# Run all tests
ctest

# Run tests with verbose output
ctest -V

# Run specific test
ctest -R "test_name"

# Run tests with custom output
ctest --output-on-failure
```

### Troubleshooting

#### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

#### Runtime Issues

**ImportError: No module named 'nlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](docs/ROADMAP.md) - Development roadmap