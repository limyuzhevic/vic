# NLM Unified User Guide

## Quick Start

NLM (熙然) is a brain-inspired spiking neural network simulator that lets you create artificial brains that can learn, remember, and make decisions.

### Three Main Interfaces:

1. **Python API** (`pynlm`) - Simple and high-level
2. **C++ API** (`nlm`) - Flexible and powerful  
3. **Unified Interface** (`nlm_phase6`) - Comprehensive command-line tool

---

## 1. Quick Python Example (30 seconds)

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)

print(f"Simulation complete! {brain.getTotalSpikeCount()} spikes recorded")
```

---

## 2. Quick C++ Command-Line Example (2 minutes)

```bash
# Build project first
mkdir build && cd build && cmake .. && make -j4

# Run unified interface with custom settings
./nlm --mode run --steps 1000 --neurons 500 --regions 2 --no-checkpointing

# Test integration
./nlm --mode test

# Performance benchmark  
./nlm --mode benchmark --steps 10000 --output benchmark_results.txt

# Profile with settings
./nlm --mode profile --steps 2000 --verbose --output profile_results.txt
```

---

## 3. Unified Interface Features

### Command-Line Modes:

- **`run`** - Complete integration experiment (default)
- **`test`** - Integration verification tests
- **`benchmark`** - Performance testing
- **`profile`** - Performance profiling

### Key Options:

```bash
# Core settings
--steps N              Number of simulation steps
--neurons N            Number of neurons (default: 500)
--regions N            Number of brain regions (default: 1)
--connection-prob P    Connection probability (default: 0.1)

# Memory systems
--no-checkpointing      Disable checkpoint save/load
--checkpoint-path P     Checkpoint file path
--no-replay            Disable memory replay
--no-development       Disable developmental changes

# Output and logging
--output FILENAME      Save results to file
-v, --verbose          Enable verbose output
-h, --help             Show help
--version              Show version
```

### Example Configurations:

```bash
# Standard research run
./nlm --steps 5000 --neurons 1000

# Large-scale benchmark
./nlm --mode benchmark --steps 20000 --neurons 3000 --regions 4

# Fast development testing
./nlm --mode test --verbose

# Profile with specific settings
./nlm --mode profile --steps 1000 --connection-prob 0.05
```

---

## 4. Python API Reference

### Basic Usage

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Modify settings if needed
config.set("brain.neuron_count", 1000)

# 3. Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# 4. Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")

# 5. Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
```

### Complete Agent Example

```python
import pynlm

def run_complete_simulation(steps=1000):
    """Run a full brain-world-agent simulation."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation loop
    for step in range(steps):
        world.update(0.1)
        
        # Sense world
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides
        action = agent.decodeMotorCommand()
        
        # Take action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log occasionally
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, world, agent

# Run simulation
brain, world, agent = run_complete_simulation(500)
```

### Key Classes and Methods

#### Brain

```python
# Creation and initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step)              # One simulation step
brain.step(step, time)       # Step with timestamp
brain.reset()                # Reset brain state

# State management
brain.save("checkpoint.bin") # Save to file
brain.load("checkpoint.bin") # Load from file

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getDevelopmentalStage()    # Current developmental stage
```

#### AgentBrain

```python
# Creation and initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Processing
agent.processSensoryInput(percept)     # Process sensory data
brain.decodeMotorCommand()           # Get action decision

# Neuromodulation
agent.applyRewardModulation(reward, predicted)  # Learn from reward

# Development
agent.updateDevelopment(timestep)     # Age brain
agent.getDevelopmentalStage()        # Get current stage

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)           
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Get system levels
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld

```python
# Creation and configuration
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()                    # Reset world
world.update(timestep)           # Update world
world.setRandomSeed(42)          # Set random seed

# Agent interaction
world.setAgentStart(x, y)       # Set agent start position
world.applyMotorCommand(action, time)  # Execute action

# State access
percept = world.getSensoryPercept()  # Get sensory input
body = world.getAgentBody()          # Get agent body

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
```

---

## 5. C++ API Reference

### Basic Usage

```cpp
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"

// Create configuration
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);

// Create and initialize brain
auto brain = std::make_shared<Brain>(config);
if (!brain->initialize()) {
    std::cerr << "Brain initialization failed!" << std::endl;
    return 1;
}

// Run simulation
for (size_t step = 0; step < 1000; ++step) {
    brain->step(step);
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": " 
                  << brain->getFiringNeuronCount() << " firing" << std::endl;
    }
}

// Access systems
auto* memory = brain->getWorkingMemory();
auto* dopamine = brain->getDopamine();
auto* curiosity = brain->getCuriosity();
```

### Advanced Configuration

```cpp
// Configure with file or program
std::shared_ptr<Config> config = Config::createFromFile("configs/default.cfg");

// Override specific settings
config->set("neuron_count", 2000);
config->set("connection_probability", 0.05);
config->set("stdp_ltp_weight", 0.015);

// Create brain with custom config
Brain brain(config);
```

---

## 6. Unified Interface Examples

### Research Run

```bash
# Quick integration test
./nlm --mode test

# Standard research run with 5000 steps
./nlm --steps 5000 --neurons 1000 --verbose

# Memory-focused run with checkpointing
./nlm --steps 2000 --enable-checkpointing --checkpoint-path ./my_brain.bin
```

### Development and Testing

```bash
# Debug with verbose output
./nlm --mode test --verbose

# Benchmark with custom settings
./nlm --mode benchmark --steps 10000 --neurons 2000 --output benchmark_results.txt

# Profile with performance monitoring
./nlm --mode profile --steps 1000 --connection-prob 0.05 --verbose
```

### Integration with Scripts

```bash
# Run and save results automatically
./nlm --steps 3000 --output simulation_results.txt

# Quick test for CI/CD
./nlm --mode test --quiet

# Full profile with detailed output
./nlm --mode profile --steps 5000 --verbose --output profile_output.txt
```

---

## 7. Configuration Options

### Core Brain Settings

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Synapse connectivity probability |
| `simulation_timestep` | float | 0.001 | Simulation time step (seconds) |

### Learning Systems

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse removal |

### Neuromodulation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |
| `curiosity_exploration_rate` | float | 0.1 | Rate of curiosity-driven exploration |
| `novelty_threshold` | float | 0.5 | Novelty detection threshold |

### Development

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_synaptogenesis_rate` | float | 0.001 | Developmental synaptogenesis rate |
| `development_pruning_rate` | float | 0.0001 | Developmental pruning rate |
| `replay_interval` | int | 100 | Memory replay interval (steps) |
| `consolidation_interval` | int | 1000 | Memory consolidation interval |

---

## 8. Troubleshooting

### Python API Issues

**"ModuleNotFoundError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
# Or for development
pip install -e .
```

**"AttributeError: 'Brain' object has no attribute 'step'"**
```python
# Make sure to initialize first
brain.initialize()  # Called automatically by createBrain, but explicit is better
```

**Slow performance**
```python
# For faster simulations, consider:
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduce neuron count
config.set("brain.connection_probability", 0.05)  # Reduce connectivity
```

### C++ Command-Line Issues

**"Command not found: ./nlm"**
```bash
# Build the project first
mkdir build && cd build && cmake .. && make -j4
```

**"Unknown argument: --invalid-option"**
```bash
./nlm --help  # See all available options
```

**"Permission denied"**
```bash
chmod +x nlm_phase6  # If you renamed the executable
```

### General Issues

**"Simulation doesn't run properly"**
```python
# Check this basic pattern:
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!
for step in range(10):
    brain.step(step)
```

**"Memory systems not working"**
```python
# Check integration status
print(f"Working Memory: {brain.getWorkingMemory() != None}")
print(f"Episodic Memory: {brain.getEpisodicMemory() != None}")
print(f"Dopamine: {brain.getDopamine() != None}")
```

---

## 9. Examples and Tutorials

### Tutorial 1: First Brain

This tutorial creates your first brain and runs a simple simulation.

```python
import pynlm

print("=== Tutorial 1: First Brain ===")

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simple simulation
for step in range(20):
    brain.step(step)
    if step % 5 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("✓ Tutorial 1 complete!")
```

### Tutorial 2: Complete Agent

This tutorial creates a complete brain-world-agent system.

```python
import pynlm

print("=== Tutorial 2: Complete Agent ===")

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()
world.setAgentStart(5.0, 5.0)

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Run agent
for step in range(50):
    world.update(0.1)
    
    # Sense and act
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"✓ Agent completed {50} steps")
print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
```

### Tutorial 3: Command-Line Usage

This tutorial shows how to use the unified command-line interface.

```bash
# Create build directory and build
cd /path/to/nlm
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run help to see all options
./nlm --help

# Run integration test
./nlm --mode test

# Run quick simulation
./nlm --steps 100 --neurons 100 --regions 1

# Save results
./nlm --steps 500 --output results.txt
```

---

## 10. Advanced Features

### Memory Systems

NLM includes three memory systems:

1. **Working Memory**: Short-term active processing
2. **Episodic Memory**: Experience storage and replay
3. **Associative Memory**: Pattern associations

All memory systems are automatically integrated with neural processing.

### Neuromodulation

The brain includes several neuromodulators:

- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration motivation
- **Novelty**: Surprise detection
- **Prediction Error**: Learning from prediction failures

### Development

Brains go through developmental stages:

1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Specialized connections
3. **Maturation**: Stabilized performance
4. **Adult**: Fine-tuning and maintenance

### Checkpointing

Brains can save and load their state:

```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state later
new_brain = pynlm.createBrain(config)
new_brain.initialize()
new_brain.load("my_brain.bin")
```

---

## 11. Performance Tips

### Python Performance

```python
# For faster simulations:
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Reduce neurons
config.set("brain.connection_probability", 0.05)  # Reduce connections
config.set("plasticity.stdp.learning_rate", 0.001)  # Reduce plasticity computation

# For batch processing
brains = []
for i in range(10):
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Process all brains
for brain in brains:
    for step in range(100):
        brain.step(step)
```

### C++ Performance

```cpp
// Use multiple threads for batch processing
std::vector<std::thread> threads;
for (int i = 0; i < num_brains; ++i) {
    threads.emplace_back([i]() {
        Brain brain(config);
        brain.initialize();
        for (size_t step = 0; step < max_steps; ++step) {
            brain.step(step);
        }
    });
}

for (auto& t : threads) {
    t.join();
}
```

### Memory Management

```cpp
// Reuse brains instead of creating new ones
Brain brain1(config);
brain1.initialize();
brain1.step(0);

Brain brain2(config);
brain2.initialize();
brain2.step(0);  // Each brain maintains its own state
```

---

## 12. Migration Guide

### From Old to New API

#### Brain Creation

**Old:**
```python
brain = pynlm.createBrain()
brain.initialize()
```

**New:**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation Loop

**Old:**
```python
for i in range(100):
    brain.step(i)
```

**New:**
```python
for step in range(100):
    brain.step(step)
    # More options:
    brain.step(step, step * 0.1)  # With timestamp
```

#### Agent Usage

**Old:**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**New:**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)  # Enable learning
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
```

---

## 13. Frequently Asked Questions

### Q: What Python versions are supported?
A: Python 3.8+ is supported. Python 3.9+ recommended for best performance.

### Q: What's the difference between `nlm`, `nlm_phase6_demo`, and `nlm_phase6_text_demo`?
A: 
- `nlm`: Unified interface with command-line options
- `nlm_phase6_demo`: Original Phase 6 integration demo
- `nlm_phase6_text_demo`: Phase 6 with text/sense capabilities

### Q: Can I use the Python API with the C++ core?
A: Yes! The Python bindings (`pynlm`) interface directly with the C++ core for maximum performance.

### Q: How do I get started with research?
A: Use the unified interface (`nlm`) with custom configuration:
```bash
./nlm --steps 1000 --neurons 500 --output research_results.txt
```

### Q: What about parallel processing?
A: The C++ core supports parallel processing through SIMD and multi-threading. Use large neuron counts for better performance.

### Q: How do I debug my simulations?
A: Use verbose mode in the unified interface:
```bash
./nlm --verbose --mode test
```

Or in Python:
```python
import pynlm
pynlm.Logger.setLevel(pynlm.LogLevel.Debug)
```

---

## 14. Conclusion

NLM provides a comprehensive brain simulation platform with multiple interfaces:

1. **Python API**: Simple and high-level for rapid prototyping
2. **C++ API**: Flexible and powerful for performance-critical applications
3. **Unified Interface**: Command-line tool for standardized experiments

Choose the interface that best fits your needs:

- **Research/Experiments**: Use the unified interface (`nlm`) with command-line options
- **Quick Prototyping**: Use the Python API (`pynlm`) for rapid development
- **Performance**: Use the C++ API directly for maximum control

All interfaces provide access to the same integrated brain systems including memory, neuromodulation, development, and prediction.

---

**Ready to start?**

```bash
# Build the project
cd /path/to/nlm
mkdir build && cd build
cmake .. && make -j4

# Get help
./nlm --help

# Run a simple test
./nlm --mode test
```

Or in Python:

```python
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(10):
    brain.step(i)
print("Success!")
```

Happy brain simulation! 🧠✨