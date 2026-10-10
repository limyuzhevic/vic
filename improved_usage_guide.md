# NLM (Neural Learning Machine) - IMPROVED USAGE GUIDE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Using the NLM command-line interface (CLI)
3. Running experimental demonstrations

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings, if available)

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
| `nlm` | Executable | Main integration demo (Phase 6) |
| `nlm_phase3_demo` | Executable | Phase 3 world interaction demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 experimental cognition demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 advanced experimental features |
| `nlm_phase6_demo` | Executable | Phase 6 full integration test |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |

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

---

## Part 2: Using the NLM Command-Line Interface

NLM provides a comprehensive command-line interface for running simulations and experiments.

### Basic Usage

```bash
# Run the main integration demo (Phase 6)
./nlm

# Run Phase 3 world interaction demo
./nlm_phase3_demo [num_episodes] [steps_per_episode]

# Run Phase 4 experimental cognition demo
./nlm_phase4_demo [num_trials]

# Run Phase 5 advanced experimental features
./nlm_phase5_demo [options]

# Run Phase 6 integration test
./nlm_phase6_demo
```

### Phase 5 Demo Options

```bash
# Run lifelong learning demo with 10k neurons
./nlm_phase5_demo --demo-type 1 --neurons 10000

# Run scaling analysis demo
./nlm_phase5_demo --demo-type 2 --neurons 5000

# Run social learning demo
./nlm_phase5_demo --demo-type 3 --neurons 8000

# Get help information
./nlm_phase5_demo --help

# Show version information
./nlm_phase5_demo --version
```

### Command Help

The NLM command system provides:

1. **Help System**: Context-sensitive help for all commands
2. **Tab Completion**: Automatic command and argument completion
3. **Configuration Management**: Runtime configuration modification
4. **Experiment Control**: Start/stop/experiment management
5. **Progress Monitoring**: Real-time progress display

### Configuration Files

Configuration files should be in INI format:

```ini
# NLM Configuration File

[brain]
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001

[plasticity]
enable_stdp = true
enable_hebbian = true
enable_structural = true

[neuromodulation]
enable_dopamine = true
enable_curiosity = true
enable_novelty = true

[simulation]
steps_per_episode = 1000
num_episodes = 5
enable_checkpointing = true
enable_replay = false

[world]
width = 20
height = 20
vision_width = 8
vision_height = 8
```

---

## Part 3: Using the NLM C++ API

### Basic Usage

The NLM library provides several key classes for simulation:

#### Brain Class

```cpp
// Creation
nlm::Brain brain(config);

// Initialization
brain.initialize();

// Simulation
brain.step(step_number);           // Run one simulation step
brain.step(step_number, time);     // Run step with timestamp
brain.reset();                     // Reset brain state

// Input/Output
brain.receiveSensoryInput(sensory_input);  // Inject sensory data
action = brain.produceAction();             // Get motor action

// Statistics
brain.getTotalNeuronCount();       // Total neurons
brain.getTotalSynapseCount();      // Total synapses
brain.getFiringNeuronCount();     // Currently firing
brain.getAverageFiringRate();      // Average firing rate

// State
brain.save("checkpoint.bin");      // Save state
brain.load("checkpoint.bin");      // Load state
```

#### Config Class

```cpp
// Create
auto config = std::make_shared<nlm::Config>();

// Query
if (config->has("brain.neuron_count")) {
    keys = config->getKeys();
}

// Modify
config->set("brain.neuron_count", 1000);
config->set("learning.rate", 0.001);

// File I/O
config->loadFromFile("config.json");
config->saveToFile("config.json");
```

#### AgentBrain Class

```cpp
// Creation
nlm::AgentBrain agent(brain);

// Initialization
agent.initialize(world);

// Sensory processing
agent.processSensoryInput(percept);

// Motor decoding
motor_cmd = agent.decodeMotorCommand();

// Neuromodulation
agent.applyRewardModulation(reward, predicted_reward);

// Development
agent.updateDevelopment(timestep);
developmental_stage = agent.getDevelopmentalStage();

// Subsystem enable/disable
agent.enableRewardModulation(true/false);
agent.enableStructuralPlasticity(true/false);
agent.enableDevelopment(true/false);
agent.enableCuriosity(true/false);

// Statistics
agent.getCuriosityLevel();
agent.getNoveltyLevel();
agent.getPredictionError();
agent.getNeuromodulationLevel();
```

#### SimpleWorld Class

```cpp
// Creation
nlm::SimpleWorld world;

// Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8);

// Control
world.reset();
world.update(timestep);
world.setRandomSeed(42);

// Agent interaction
world.setAgentStart(x, y);
world.applyMotorCommand(motor_cmd, current_time);

// State access
percept = world.getSensoryPercept();
body = world.getAgentBody();

// World objects
world.addObject(nlm::WorldObject(x, y, nlm::WorldObjectType::Resource, value=1.0));
world.removeObject(x, y);
world.isValidPosition(x, y);

// Properties
world.getWidth();
world.getHeight();
world.getSimulationTime();
```

### Complete Agent Example

```cpp
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

void runAgentSimulation(size_t numSteps = 1000) {
    """Run a complete agent simulation with brain and world."""
    
    // 1. Create configuration
    auto config = std::make_shared<Config>();
    
    // 2. Create brain and initialize
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // 3. Create agent brain interface
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // 4. Create and configure world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // 5. Initialize agent with world
    agent->initialize(world);
    
    // 6. Enable subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    // 7. Run simulation loop
    for (size_t step = 0; step < numSteps; ++step) {
        // Update world
        world->update(timestep=0.1);
        
        // Get sensory input from world
        const auto& percept = world->getSensoryPercept();
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command from brain activity
        auto motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Apply reward modulation
        auto reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0;
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Print progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ":" << std::endl;
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Curiosity: " << agent->getCuriosityLevel() << std::endl;
            std::cout << "  Novelty: " << agent->getNoveltyLevel() << std::endl;
            std::cout << "  Dev Stage: " << brain->getDevelopmentalStage() << std::endl;
        }
    }
}
```

---

## Part 4: Command Line Tools

### Available Executables

1. **`./nlm`** - Main integration demo (Phase 6)
   - Runs complete integration test
   - Verifies all brain systems are connected
   - Demonstrates final integrated brain functionality

2. **`./nlm_phase3_demo`** - Phase 3 world interaction demo
   - Shows experience -> neural activity -> action -> consequence -> learning
   - Demonstrates sensory processing and motor control
   - Simple agent behavior in a world

3. **`./nlm_phase4_demo`** - Phase 4 experimental cognition demo
   - Demonstrates temporal prediction, working memory, episodic recall
   - Shows concept formation, attention, planning, self-model
   - Social learning and continual learning capabilities

4. **`./nlm_phase5_demo`** - Phase 5 advanced experimental features
   - Lifelong learning and continual adaptation
   - Memory consolidation and episodic memory
   - Scaling analysis and performance benchmarking
   - Social learning and imitation

5. **`./nlm_phase6_demo`** - Phase 6 integration test
   - Comprehensive integration verification
   - Tests all brain systems and their connections
   - Checkpoint save/load functionality

### Command Examples

```bash
# Run the main integration demo
./nlm

# Run Phase 3 demo with custom parameters
./nlm_phase3_demo 10 500

# Run Phase 4 demo with specific trials
./nlm_phase4_demo 100

# Run Phase 5 demo (choose from 4 demo types)
./nlm_phase5_demo --demo-type 1 --neurons 10000

# Run Phase 6 integration test
./nlm_phase6_demo
```

---

## Part 5: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 6: Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Part 7: Example Scripts

### Minimal Example

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create and initialize
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Simulate
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    std::cout << "Simulation complete!" << std::endl;
    return 0;
}
```

### Environment Interaction Example

```cpp
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Setup
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    auto agent = std::make_shared<AgentBrain>(brain);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=10, height=10, visionWidth=8, visionHeight=8);
    world->reset();
    agent->initialize(world);
    
    // Run episode
    for (int i = 0; i < 500; ++i) {
        world->update(0.1);
        agent->processSensoryInput(world->getSensoryPercept());
        brain->step(i);
        world->applyMotorCommand(agent->decodeMotorCommand(), world->getSimulationTime());
    }
    
    std::cout << "Environment interaction complete!" << std::endl;
    return 0;
}
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Conclusion

NLM provides a comprehensive framework for brain simulation and artificial intelligence research. The improved command system, enhanced build configuration, and better documentation make it accessible to users of all skill levels, from beginners to advanced researchers.

The key improvements include:
1. **Updated main executable**: Now demonstrates Phase 6 integration instead of Phase 2
2. **New Phase 5 demo executable**: Provides access to advanced experimental features
3. **Improved command system**: Enhanced CLI with help and configuration options
4. **Better documentation**: Fixed incorrect Python API references
5. **Enhanced build system**: More flexible executable options and installation targets

These improvements make the NLM project more accessible, maintainable, and extensible for research and educational purposes.