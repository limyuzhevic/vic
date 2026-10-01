# NLM - Getting Started Guide

This guide provides step-by-step instructions for new users to get started with NLM (Neural Learning Machine). It covers the essentials from installation through basic usage to understanding the system architecture.

## 🎯 What You'll Learn

By the end of this guide, you will be able to:

1. **Install and set up NLM** - Build from source and install the Python bindings
2. **Create and run a basic simulation** - Get your first brain working
3. **Understand the NLM architecture** - Learn about the main components and how they fit together
4. **Configure a brain for simulation** - Set up parameters and options
5. **Run experiments** - Execute simulations and analyze results
6. **Troubleshoot common issues** - Fix problems that new users typically encounter

## 📋 Prerequisites

Before you begin, ensure you have:

### Required Software

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: Make or Ninja

### Recommended (but optional)

- **Git**: For cloning the repository
- **Text Editor**: VS Code, CLion, or similar IDE
- **Python Packages**: For using NLM programmatically

## 🚀 Quick Start (30-Second Version)

If you want the absolute quickest way to test NLM:

```bash
# Clone and build
mkdir nlm_workspace && cd nlm_workspace
git clone https://github.com/your-repo/nlm.git
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Test the demo
./nlm_phase6_demo
```

That's it! You've just run the Phase 6 integration test.

## 📖 Step 1: Installation

### Option A: Complete Build from Source

This is recommended for full functionality and development:

```bash
# 1. Clone the repository (if not already done)
git clone https://github.com/your-repo/nlm.git
cd nlm

# 2. Create build directory
mkdir build
cd build

# 3. Configure CMake
# Release build for best performance
cmake .. -DCMAKE_BUILD_TYPE=Release

# Optional: Specify compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# 4. Build the project
make -j$(nproc)

# 5. Install (optional)
# sudo make install  # On Unix systems
```

### Option B: Quick Python Setup

If you only want the Python bindings:

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy matplotlib

# Build and install Python package
pip install .

# Or install in development mode
pip install -e .
```

### Option C: Using Docker (Easiest)

```bash
# If Docker is available, use the pre-built image
docker pull your-repo/nlm:latest
docker run -it your-repo/nlm:latest /bin/bash
```

## ✅ Verification

After installation, verify everything works:

### Test C++ Executables

```bash
# List available executables
cd build
echo "Available executables:"
ls -la

# Run Phase 6 demo (integration test)
./nlm_phase6_demo

# Run unit tests (if built)
./nlm_test
```

### Test Python Bindings

```python
import pynlm

# Check version
print(f"NLM Version: {pynlm.__version__}")

# Create default configuration
config = pynlm.createDefaultConfig()
print(f"Config created: {config is not None}")

# Create brain
brain = pynlm.createBrain(config)
print(f"Brain created: {brain is not None}")

# Initialize
brain.initialize()
print(f"Brain initialized: {brain is not None}")

print("✅ Python bindings working correctly!")
```

## 🧠 Step 2: Understanding NLM Components

NLM consists of several key components that work together:

### The Main Components

| Component | Purpose | Key Files | Description |
|-----------|---------|-----------|-------------|
| **Brain** | The neural processing core | `src/brain/`, `Brain.hpp` | LIF neurons, synapses, plasticity |
| **Agent** | Interface between brain and world | `src/agent/`, `AgentBrain.hpp` | Sensory processing, action selection |
| **World** | The simulation environment | `src/world/`, `SimpleWorld.hpp` | Agent's sensory and motor interface |
| **Memory** | Learning and experience storage | `src/memory/`, `Memory.hpp` | Working, episodic, semantic memory |
| **Prediction** | Expectation and planning | `src/prediction/`, `PredictionSystem.hpp` | Temporal sequence learning |
| **Development** | Maturation and aging | `src/development/`, `DevelopmentSystem.hpp` | Synaptogenesis, pruning, maturation |

### The Brain Loop (How It All Works)

```
WORLD (Environment)
     ↓
SENSORY INPUT (World observes agent)
     ↓
AGENT (Sensory processing)
     ↓
BRAIN (Neural computation)
     ↓
ACTION SELECTION (Motor decoding)
     ↓
WORLD (Apply action)
     ↓
WORLD CONSEQUENCE (Reward, cost)
     ↓
NEUROMODULATION (Dopamine, curiosity)
     ↓
PLASTICITY (Learning)
     ↓
MEMORY / DEVELOPMENT (Storage, maturation)
     ↓
CHANGED BRAIN
```

## 💻 Step 3: Creating Your First Brain

### Basic Brain Creation (Pure C++)

```cpp
// Include NLM headers
#include "src/brain/Brain.hpp"
#include "src/config/Config.hpp"

int main() {
    // 1. Create configuration
    auto config = std::make_shared<Config>();
    
    // 2. Configure basic parameters
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    config->set("plasticity.stdp.enable", true);
    
    // 3. Create brain
    Brain brain(config);
    
    // 4. Initialize
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // 5. Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Log occasionally
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": " 
                     << brain.getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    return 0;
}
```

### Basic Brain Creation (Python)

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Optional: Customize configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("✅ Simulation complete!")
```

## 🌍 Step 4: Adding Environment Interaction

### Creating a World and Agent

```python
import pynlm

# 1. Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create agent
agent = pynlm.createAgentBrain(brain)

# 3. Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,           # World width
    height=20,          # World height  
    visionWidth=8,      # Agent's visual field width
    visionHeight=8      # Agent's visual field height
)

# 4. Initialize agent with world
agent.initialize(world)

# 5. Enable important subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time

# 6. Reset world to starting state
world.reset()
world.setAgentStart(10.0, 10.0)         # Start in center

# 7. Run simulation
for step in range(500):
    # Update world
    world.update(timestep=0.1)
    
    # Get what agent sees
    percept = world.getSensoryPercept()
    
    # Tell brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Brain decides what to do
    action = agent.decodeMotorCommand()
    
    # Do the action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

## 🔧 Step 5: Configuration Deep Dive

### Key Configuration Categories

#### Brain Configuration

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    }
}
```

#### Plasticity Configuration

```json
{
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0001
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.01,
            "pruning_threshold": 0.1
        }
    }
}
```

#### Neuromodulation Configuration

```json
{
    "neuromod": {
        "dopamine": {
            "scale": 1.0,
            "plasticity_modulation": true
        },
        "curiosity": {
            "enable": true,
            "novelty_sensitivity": 1.0
        },
        "novelty": {
            "enable": true,
            "threshold": 0.1
        }
    }
}
```

### Configuration Methods

#### Set Configuration Values

```python
# Set individual values
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)

# Set nested values
config.set("neuromod.dopamine.scale", 2.0)

# Load from file
config.loadFromFile("my_config.json")

# Save to file  
config.saveToFile("my_config.json")
```

#### Get Configuration Values

```python
# Check if key exists
if config.has("brain.neuron_count"):
    count = config.getInt("brain.neuron_count")

# Get all keys
keys = config.getKeys()
print("All config keys:", keys)

# Get section keys
brain_keys = config.getKeys("brain")
print("Brain config keys:", brain_keys)
```

## 📊 Step 6: Monitoring and Analysis

### Getting Brain Statistics

```python
# Core statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Network properties
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Average path length: {brain.getAveragePathLength():.2f}")

# Memory statistics
print(f"Working memory active: {brain.getWorkingMemoryActiveCount()}")
print(f"Episodic memories: {brain.getEpisodicMemoryCount()}")

# Development
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
print(f"Age: {brain.getBrainAge():.1f} steps")

# Neuromodulation
print(f"Dopamine level: {brain.getDopamineLevel():.3f}")
print(f"Curiosity level: {brain.getCuriosityLevel():.3f}")
print(f"Novelty level: {brain.getNoveltyLevel():.3f}")
```

### Saving and Loading Brain State

```python
# Save checkpoint
success = brain.save("my_checkpoint.bin")
if success:
    print("✅ Brain saved successfully")
else:
    print("❌ Failed to save brain")

# Load checkpoint  
success = brain.load("my_checkpoint.bin")
if success:
    print("✅ Brain loaded successfully")
else:
    print("❌ Failed to load brain")
```

## 🔌 Step 7: Enabling Advanced Features

### Available Subsystems

NLM has several optional subsystems that can be enabled:

#### 1. Reward Modulation
```python
agent.enableRewardModulation(True)
# Learn from rewards and prediction errors
```

#### 2. Structural Plasticity
```python
agent.enableStructuralPlasticity(True)
# Grow new synapses and prune weak ones
```

#### 3. Development
```python
agent.enableDevelopment(True)
# Brain matures over time with age-dependent plasticity
```

#### 4. Curiosity
```python
agent.enableCuriosity(True)
# Explore novel situations and learn from surprises
```

#### 5. Prediction
```python
# Prediction is enabled by default in Phase 4+
brain.getPredictionSystem().enable(true)
```

#### 6. Memory Systems
```python
# Working memory
brain.getWorkingMemory().enable(true)

# Episodic memory  
brain.getEpisodicMemory().enable(true)

# Semantic memory
brain.getSemanticMemory().enable(true)

# Procedural memory
brain.getProceduralMemory().enable(true)
```

## 🚨 Step 8: Troubleshooting Common Issues

### Issue 1: "Brain.initialize() failed"

**Symptoms**: Brain creation fails, crashes, or shows no activity

**Solutions**:

1. **Check CMake configuration**:
   ```bash
   cd build
   cat CMakeCache.txt | grep -i python
   ```

2. **Verify dependencies**:
   ```bash
   # Check for Python
   python3 --version
   pip3 --version
   
   # Check for required Python packages
   pip3 list | grep -E "(pybind11|scikit-build)"
   ```

3. **Clean rebuild**:
   ```bash
   rm -rf build
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

### Issue 2: "ImportError: No module named 'pynlm'"

**Symptoms**: Python cannot import NLM

**Solutions**:

1. **Verify installation**:
   ```bash
   pip3 list | grep pynlm
   ```

2. **Force reinstall**:
   ```bash
   pip3 install --force-reinstall .
   ```

3. **Check Python path**:
   ```bash
   python3 -c "import sys; print(sys.path)"
   ```

### Issue 3: "Simulation runs but nothing happens"

**Symptoms**: No firing, no learning, no visible output

**Solutions**:

1. **Increase simulation steps**:
   ```python
   for step in range(5000):  # Increased from 1000
       brain.step(step)
   ```

2. **Check neuron configuration**:
   ```python
   # Ensure enough neurons
   config.set("brain.neuron_count", 5000)
   
   # Check connectivity
   config.set("brain.synapse_density", 0.2)
   ```

3. **Verify initialization**:
   ```python
   # Make sure initialize() was called
   if not brain.initialize():
       print("Initialization failed!")
       return
   ```

### Issue 4: "Memory usage too high"

**Symptoms**: System slows down or crashes during long simulations

**Solutions**:

1. **Reduce brain size**:
   ```python
   config.set("brain.neuron_count", 500)  # Reduce from 1000
   ```

2. **Disable memory subsystems**:
   ```python
   agent.enableWorkingMemory(False)
   agent.enableEpisodicMemory(False)
   ```

3. **Use checkpointing**:
   ```python
   # Save periodically to free memory
   if step % 1000 == 0:
       brain.save(f"checkpoint_{step}.bin")
   ```

## 📚 Step 9: Next Steps

### For New Users

1. **Complete Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
2. **Try Example 1**: `examples/basic-examples/basic-example-1.md`
3. **Explore API Reference**: `docs/api-reference.md`
4. **Study Configuration Guide**: `docs/configuration.md`
5. **Learn Best Practices**: `docs/best-practices.md`

### For Developers

1. **Read Architecture Docs**: `docs/ARCHITECTURE.md`, `docs/SCIENCE.md`
2. **Review Code Examples**: Study the source code in `src/`
3. **Explore Plugin System**: `docs/plugin-guide.md`
4. **Performance Tuning**: `docs/performance-tuning.md`
5. **System Administration**: `docs/administrator-guide.md`

### For Researchers

1. **Experiment Framework**: `docs/experiments.md`
2. **Performance Analysis**: `docs/performance.md`
3. **Scaling Studies**: `docs/scaling.md`
4. **Validation Results**: `docs/PHASE6_FINAL_RESULTS.md`
5. **Scientific Background**: `docs/SCIENCE.md`

## 🎯 Quick Reference

### Most Common Commands

| Task | Command |
|------|---------|
| **Install** | `pip install .` (Python) or `cd build && make -j$(nproc)` (C++) |
| **Test C++** | `./nlm_phase6_demo` or `./nlm_test` |
| **Test Python** | `python3 -c "import pynlm; print(pynlm.__version__)"` |
| **Run demo** | `python3 examples/visualization_example.py` |

### Essential Functions

| Function | Description |
|----------|-------------|
| `pynlm.createBrain(config)` | Create a brain instance |
| `brain.initialize()` | Initialize the brain |
| `brain.step(step)` | Run one simulation step |
| `pynlm.createAgentBrain(brain)` | Create agent interface |
| `agent.processSensoryInput(percept)` | Feed sensory data |
| `agent.decodeMotorCommand()` | Get action from brain |
| `brain.getFiringNeuronCount()` | Get currently firing neurons |
| `brain.save(filename)` | Save brain state |
| `brain.load(filename)` | Load brain state |

### Configuration Keys (Common)

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## 🏆 Success!

You now have a working NLM installation and understand the basic concepts. Continue exploring the documentation to master all features!

**Remember**: NLM is a research project. The documentation may contain experimental features and incomplete systems. Focus on the core neural computation and learning capabilities for now.

Happy experimenting! 🧠✨