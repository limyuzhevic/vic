# NLM - Enhanced Easy Usage Guide for Beginners

## Quick Start with Enhanced CLI

NLM (熙然) is a **brain-inspired spiking neural network simulator** featuring **Phase 6: FINAL INTEGRATION** with **ADVANCED CLI FEATURES**. This represents the culmination of years of development, combining neural dynamics, memory systems, cognitive capabilities, and developmental processes into a coherent artificial brain.

### What's New in Phase 6?

- **✅ All Systems Integrated**: Memory, prediction, cognition, neuromodulation connected to neural processing
- **✅ Neuromodulation Coordination**: Dopamine, curiosity, novelty, prediction error integrated
- **✅ Advanced CLI Features**: Comprehensive configuration, benchmarking, profiling, multi-mode execution
- **✅ Error Recovery**: Graceful shutdown with recovery strategies
- **✅ Scripted Operations**: Batch mode, parameter sweep, random search, multi-run
- **✅ Real-time Monitoring**: System metrics and learning statistics

## Installation (Updated for Enhanced CLI)

### Prerequisites

- C++20 compatible compiler (GCC 11+, Clang 14+, or MSVC 2022+)
- CMake 3.20 or higher
- Python 3.8+ (for Python bindings)
- pybind11 2.11+ (for Python bindings)

### Build Instructions

```bash
# Standard build with advanced CLI support
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_CLI=ON
make -j$(nproc)

# Build with additional optimization flags
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_ADVANCED_CLI=ON \
         -DENABLE_PROFILING=ON \
         -DENABLE_MEMORY_POOL=ON
make -j$(nproc)

# Install
cmake --install .
```

## Enhanced CLI Quick Start

### Example 1: Simple Brain Simulation

```bash
./nlm --mode phase2 --neurons 500 --steps 1000
```

**Output:** Basic neural computation demonstration showing spike generation and synaptic dynamics.

### Example 2: Environment Interaction

```bash
./nlm --mode phase3 --neurons 1000 --steps 2000 \
     --regions 4 --connection-prob 0.15
```

**Output:** Agent learns through interaction with environment using reward-modulated plasticity.

### Example 3: Cognitive Development

```bash
./nlm --mode phase4 --neurons 2000 --steps 5000 \
     --development --regions 8
```

**Output:** Neural system develops cognitive abilities including planning and concept formation.

### Example 4: Complete Integration

```bash
./nlm --mode phase6 --neurons 2000 --steps 10000 \
     --enable-checkpointing --enable-replay \
     --enable-system-monitoring --verbose
```

**Output:** Comprehensive integration test verifying all Phase 6 systems work together.

## Advanced Configuration Examples

### Using Configuration File

```bash
# Create and use configuration file
./nlm --mode phase6 --config-file experiment_config.json
```

### Performance Benchmarking

```bash
# Run comprehensive benchmark
./nlm --mode phase6 --detailed-benchmark --benchmark-compare
```

### Scripted Operations

```bash
# Batch execution with delays
./nlm --mode phase6 --enable-scripted-ops --batch-mode --batch-delay 500

# Parameter sweep for optimization
./nlm --mode phase6 --enable-scripted-ops --parameter-sweep

# Random search for best configuration
./nlm --mode phase6 --enable-scripted-ops --random-search --random-seeds 10

# Multi-run statistical analysis
./nlm --mode phase6 --enable-scripted-ops --multi-run-mode --multi-run-count 50
```

## The 3 Things You Need to Know (Enhanced)

1. **Enhanced Brain** - The virtual brain that thinks with all Phase 6 systems integrated
2. **Enhanced World** - The environment with advanced interaction capabilities
3. **Enhanced Agent** - The bridge connecting brain to world with neuromodulation

---

## Quick Start (Copy & Paste) - Enhanced

### Example 1: Simplest Brain with Enhanced CLI

```python
import pynlm

# Step 1: Make a brain with enhanced configuration
config = pynlm.createDefaultConfig()
# Enable all Phase 6 features
config.set("development.enable", True)
config.set("neuromodulation.dopamine.enable", True)
config.set("neuromodulation.curiosity.enable", True)
config.set("memory.working_memory.capacity", 1000)
config.set("memory.episodic_memory.capacity", 5000)

brain = pynlm.createBrain(config)

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think with enhanced processing
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your enhanced brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Enhanced Brain in Complex World

```python
import pynlm

# Create everything with Phase 6 enhancements
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run enhanced simulation for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Enhanced brain sees it
    brain.step(step)  # Enhanced brain thinks
    action = agent.decodeMotorCommand()  # Enhanced brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Enhanced simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Development stage:", brain.getDevelopmentalStage())
```

---

## Simple Analogy (Enhanced)

| Real World | Enhanced NLM Code |
|------------|-------------------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it with memory | `agent.processSensoryInput(percept)` |
| You decide to act with planning | `action = agent.decodeMotorCommand()` |
| You do the action in world | `world.applyMotorCommand(action, time)` |
| Your brain learns with all systems | `agent.applyRewardModulation(reward, predicted)` |

---

## Enhanced Common Patterns

### Pattern 1: Run Enhanced Simulation

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain with sensory processing
        agent.processSensoryInput(percept)
        
        # 4. Enhanced brain thinks with all systems
        brain.step(step)
        
        # 5. Get action from enhanced brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Enhanced learning from consequences
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)

# Use it like this:
run_enhanced_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Enhanced Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Memory capacity:", brain.getWorkingMemoryCapacity())
print("Episodic episodes:", brain.getEpisodicMemoryCount())
print("Dopamine level:", agent.getNeuromodulationLevel())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
```

### Pattern 3: Enable All Enhanced Brain Features

```python
# These are all available with Phase 6:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enablePrediction(True)            # Predict future events
agent.enableWorkingMemory(True)         # Store temporary information
agent.enableEpisodicMemory(True)        # Store experience episodes
agent.enableAttention(True)             # Focus processing
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses, **including all Phase 6 systems**.

### `brain.initialize()`
Starts up the brain with **complete initialization** of all systems.

### Enhanced `brain.step(step_number)`
Makes the brain process one moment in time. The enhanced brain:
- Checks each neuron with LIF dynamics
- Sends signals between connected neurons with delays
- Updates connections based on **STDP, Hebbian, and structural plasticity**
- **Manages working memory** for temporary storage
- **Encodes episodic memory** from experiences
- **Updates development state** for maturation
- **Applies neuromodulation** from prediction error

### `world.update(time)`
Updates the enhanced virtual world with **complex physics and agent interaction**.

### Enhanced `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain with **predictive coding**.

### Enhanced `agent.decodeMotorCommand()`
Reads the enhanced brain's motor neurons to decide what action to take using **planning and attention**.

---

## Enhanced Mini Projects

### Project 1: Silent Enhanced Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Enhanced silent brain test done!")
```

### Project 2: Enhanced Brain Watching a Complex World

```python
import pynlm

# Setup with enhanced features
config = pynlm.createDefaultConfig()
config.set("development.enable", True)
config.set("memory.working_memory.capacity", 500)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world with enhanced processing
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched enhanced world for 30 steps")
print("Development stage:", brain.getDevelopmentalStage())
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Enhanced Agent

```python
import pynlm

# Setup with full Phase 6 features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enablePrediction(True)

# Run enhanced agent with full cognitive loop
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Development: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Enhanced agent simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

---

## Enhanced Quick Reference

| What you want | Enhanced Code |
|--------------|---------------|
| Create enhanced brain | `pynlm.createBrain(config)` with Phase 6 config |
| Start enhanced brain | `brain.initialize()` |
| Make enhanced brain think | `brain.step(step)` with full processing |
| Create enhanced world | `pynlm.createSimpleWorld()` with advanced physics |
| Create enhanced agent | `pynlm.createAgentBrain(brain)` with cognitive integration |
| Get enhanced brain stats | `brain.getFiringNeuronCount()`, `brain.getDevelopmentalStage()` |
| See enhanced world | `world.getSensoryPercept()` with predictive input |
| Make enhanced action | `agent.decodeMotorCommand()` with planning |

---

## Enhanced Troubleshooting

**"My enhanced brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try enabling development for more activity
- Check if neuromodulation is enabled

**"The enhanced agent isn't learning"**
- Did you call `agent.enableRewardModulation(True)`?
- Check that `world.update()` is being called
- Verify agent's `processSensoryInput()` is connected

**"Memory system not working"**
- Enhanced systems need time to "warm up" - try more steps
- Check that `brain.getWorkingMemory()` returns valid data
- Verify episodic memory capacity settings

---

## Next Steps with Enhanced System

When you're comfortable:

1. **Read `ENHANCED_CLI_GUIDE.md`** - Comprehensive CLI reference
2. **Read `HOW_TO_USE.md`** - Advanced C++ usage guide
3. **Read `docs/ARCHITECTURE.md`** - Enhanced system architecture
4. **Experiment with different configurations!**

**New capabilities in enhanced version:**
- Complete Phase 6 integration
- Advanced CLI with 30+ command-line options
- Error recovery and scripted operations
- Real-time performance monitoring
- Configuration file support with validation
- Statistical analysis and benchmarking

That's it! You're now ready to use the **enhanced NLM system** with all Phase 6 capabilities.