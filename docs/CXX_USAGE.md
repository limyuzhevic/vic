# NLM - C++ Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers implemented in C++. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

NLM simulates all of this using real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (class `Brain`)
2. **World** - The environment the brain lives in (class `SimpleWorld`)
3. **Agent** - The bridge connecting brain to world (class `AgentBrain`)

---

## Quick Start (Command Line Examples)

### Example 1: Basic Neural Computation (Phase 2)

```bash
# Compile and run Phase 2 - Real Neural Computation
mkdir build && cd build
cmake ..
make

# Run with default settings
./nlm-phase2

# Run with custom configuration
./nlm-phase2 --config=my_config.cfg

# Run with custom parameters
./nlm-phase2 --neuron_count=2000 --region_count=2
```

### Example 2: World Interaction Demo (Phase 3)

```bash
# Run Phase 3 - World Interaction Demo
./nlm-phase3demo

# Run with custom number of episodes and steps
./nlm-phase3demo --episodes=10 --steps=500

# Show help
./nlm-phase3demo --help
```

### Example 3: Emerging Cognition (Phase 4)

```bash
# Run Phase 4 - Emerging Cognition Demo
./nlm-phase4demo

# Run with custom number of trials
./nlm-phase4demo --trials=25 --neuron_count=500

# Show help
./nlm-phase4demo --help
```

### Example 4: Integration Test (Phase 6)

```bash
# Run Phase 6 - Integration Demo
./nlm-phase6demo

# Run with custom configuration
./nlm-phase6demo --neuron-count=1000 --max-steps=5000

# Run with development mechanisms disabled
./nlm-phase6demo --disable-development --disable-checkpointing

# Show help
./nlm-phase6demo --help
```

---

## Simple Analogy

| Real World | NLM C++ Code |
|------------|--------------|
| You see something | `const SensoryPercept& percept = world.getSensoryPercept()` |
| Your brain processes it | `agentBrain.processSensoryInput(percept)` |
| You decide to act | `MotorCommand cmd = agentBrain.decodeMotorCommand()` |
| You do the action | `ActionResult result = world.applyMotorCommand(cmd, time)` |
| Your brain learns from results | `agentBrain.applyRewardModulation(result.reward, prediction)` |

---

## The Simplest Possible Example

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // One line to create a virtual brain
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize it
    brain->initialize();
    
    // Make it active for 100 steps
    for (int i = 0; i < 100; ++i) {
        brain->step(i);  // One step of thinking
    }
    
    std::cout << "Done! Your brain thought " 
              << brain->getTotalSpikeCount() << " times" << std::endl;
    
    return 0;
}
```

---

## Phase-by-Phase Overview

### Phase 1: Brain Structure
**What you learn**: The basic brain anatomy and components

**Key classes**:
- `nlm::Brain` - The main artificial brain
- `nlm::Neuron` - Individual brain cells
- `nlm::Synapse` - Connections between neurons

### Phase 2: Real Neural Computation
**What you learn**: Real spiking neuron dynamics and plasticity

**What you see**:
- LIF neuron membrane potential changes
- STDP synaptic weight modifications
- Event-driven spike propagation
- Structural plasticity (formation/pruning)

**How to run**: `./nlm-phase2`

### Phase 3: World Interaction
**What you learn**: How brains interact with environments

**Key concepts**:
- Sensory perception
- Motor actions
- Reward learning
- Developmental processes

**How to run**: `./nlm-phase3demo --episodes=N --steps=M`

### Phase 4: Emerging Cognition
**What you learn**: Complex cognitive abilities emerge from neural dynamics

**Abilities that emerge**:
- Temporal prediction
- Working memory
- Episodic recall
- Concept formation
- Neural attention
- Multi-step planning
- Self-model development
- Social learning
- Continual learning

**How to run**: `./nlm-phase4demo --trials=N`

### Phase 5: Social Learning
**What you learn**: How brains learn from observing others

**Key mechanisms**:
- Imitation learning
- Observational reinforcement
- Social modulation

**How to run**: `./nlm-phase5demo` (when implemented)

### Phase 6: System Integration
**What you learn**: How all brain systems work together

**Integration tested**:
- Working Memory integration
- Episodic Memory integration
- Neuromodulation integration
- Prediction integration
- Development integration
- Checkpointing and replay

**How to run**: `./nlm-phase6demo [OPTIONS]`

---

## Configuration Management

NLM uses a flexible configuration system for all parameters.

### Configuration Files

**Format**: `key = value` (one per line, optional comments with #)

**Example config file** (`config.cfg`):
```
# NLM Configuration File
neuron_count = 1000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Command Line Configuration

Command line arguments override configuration file values:

```bash
./nlm-phase2 \
    --config=my_config.cfg \
    --neuron_count=2000 \
    --random_seed=123 \
    --stdp_ltp_weight=0.05
```

### Available Configuration Options

All executables support these common options:
- `--help, -h` - Show help message
- `--config=FILE` - Load configuration from file
- `--seed=N` - Set random seed

Additional options are available for each phase:

**Phase 2**:
- `--neuron_count=N` - Set neuron count
- `--region_count=N` - Set region count
- `--connection_probability=P` - Set connection probability
- `--simulation_timestep=DT` - Set timestep

**Phase 3**:
- `--episodes=N` - Number of episodes
- `--steps=N` - Steps per episode

**Phase 4**:
- `--trials=N` - Number of trials per experiment
- `--neuron_count=N` - Set neuron count

**Phase 6**:
- `--neuron-count=N` - Set neuron count
- `--max-steps=N` - Set maximum steps
- `--enable-checkpointing/--disable-checkpointing` - Enable/disable checkpointing
- `--enable-replay/--disable-replay` - Enable/disable replay
- `--enable-development/--disable-development` - Enable/disable development

---

## Building and Running

### Prerequisites

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install build-essential cmake g++ git

# Install additional dependencies if needed
```

### Build Instructions

```bash
# Create build directory and configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build all targets
 cmake --build build --parallel

# Or use traditional approach
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running Demos

After building, executables will be in the `build/` directory:

```bash
# Phase 2: Basic neural computation
./build/nlm-phase2

# Phase 3: World interaction
./build/nlm-phase3demo

# Phase 4: Emerging cognition
./build/nlm-phase4demo

# Phase 6: Integration test
./build/nlm-phase6demo
```

### Running Tests

Unit tests are available:

```bash
# Run all tests
cd build
ctest --output-on-failure

# Or run specific test suites
ctest -R "phase[234]" --output-on-failure
```

---

## Troubleshooting

### Common Issues

**Issue**: "Error: Unknown argument: --unknown-option"
**Solution**: Check help message with `--help` for valid options

**Issue**: "Failed to initialize brain!"
**Solution**: Check configuration values, especially neuron_count and region_count

**Issue**: Configuration file not found
**Solution**: Ensure the file path is correct or use absolute paths

**Issue**: Memory usage too high
**Solution**: Reduce neuron_count or region_count in configuration

### Debug Mode

For development, enable verbose output:

```bash
./nlm-phase6demo --verbose -v
```

### Configuration Validation

To validate a configuration file:

```bash
cat config.cfg | ./nlm-phase2 --config=- 2>&1 | head -20
```

---

## Learning and Development

### For Researchers

NLM is ideal for studying:
- Neural plasticity mechanisms
- Memory formation
- Learning algorithms
- Cognitive development
- Social learning

### For Developers

NLM provides:
- Modern C++17 API
- Flexible configuration system
- Modular architecture
- Comprehensive logging
- Plugin-ready design

### For Students

Start with:
1. Phase 2 examples for basic neural dynamics
2. Phase 3 for world interaction
3. Phase 4 for understanding emergent cognition
4. Phase 6 for system integration

---

## Advanced Topics

### Configuration System

The configuration system supports:
- Numeric types (int, double, float)
- String values
- Source tracking (file, command-line, default)
- Configuration validation

### Brain Architecture

Key brain components:
- `Brain` - Main controller
- `SpikeSystem` - Neuron communication
- `MemorySystem` - Storage and recall
- `PredictionSystem` - Forward models
- `DevelopmentSystem` - Adaptive plasticity

### Performance Optimization

For large-scale simulations:
- Use `--neuron_count` to control brain size
- Adjust `--max-steps` for simulation length
- Enable checkpointing for long runs
- Monitor memory usage with system tools

---

## Examples Summary

| Executable | Purpose | Primary Arguments |
|------------|---------|-------------------|
| `nlm-phase2` | Real neural computation | `--config`, `--neuron_count` |
| `nlm-phase3demo` | World interaction | `--episodes`, `--steps` |
| `nlm-phase4demo` | Emerging cognition | `--trials`, `--neuron_count` |
| `nlm-phase6demo` | System integration | `--neuron-count`, `--max-steps` |


---

## Getting Help

**Documentation**
- This guide: CXX_USAGE.md
- Python equivalent: easy_usage.md (297 pages)
- Architecture: docs/ARCHITECTURE.md
- Research methods: docs/RESEARCH_METHODS.md

**Community**
- Issue tracking: Check GitHub issues
- Development: Join contributing guidelines
- Support: Check documentation and examples

**Development**
- Code style: C++17 best practices
- Testing: Unit tests with CTest
- Building: CMake configuration
- Debugging: Verbose logging and debug builds

---

This documentation provides a comprehensive guide to using NLM in C++. For the complete 297-page Python documentation (`easy_usage.md`), please see the Python version for more detailed examples and advanced topics.