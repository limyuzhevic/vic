# NLM (熙然) - Comprehensive User Guide

## Version 0.1.0 (Phase 6)

NLM is an experimental artificial developmental brain simulator that demonstrates how neural systems can integrate and develop capabilities through interaction with environments. This guide covers everything from installation to advanced usage.

---

## Quick Start (5-Minute Setup)

### Step 1: Build from Source

```bash
# Create build directory and configure
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build everything
make -j$(nproc)
```

### Step 2: Run the Integration Demo

```bash
# Run the Phase 6 integration test (as recommended in README)
./nlm_phase6_demo

# Or use the enhanced main executable
./nlm --phase demo --progress --verbose
```

### Step 3: Try the Python Interface

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few steps
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.getTotalNeuronCount()} neurons and {brain.getTotalSpikeCount()} spikes")
```

---

## Building the Project

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

#### Standard Build (Recommended for Production)

```bash
cd /path/to/nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Development Build (With Debug Symbols)

```bash
cd /path/to/nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Install to System

```bash
cd /path/to/nlm/build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Build Outputs

After successful compilation, you'll have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Enhanced main executable with CLI interface |
| `nlm_phase2_demo` | Executable | Phase 2 neural computation demo |
| `nlm_phase3_demo` | Executable | Phase 3 world interaction demo |
| `nlm_phase4_demo` | Executable | Phase 4 cognition demo |
| `nlm_phase5_demo` | Executable | Phase 5 lifetime learning demo |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase5` | Static Library | Phase 5 experiments |
| `nlm_phase6` | Static Library | Phase 6 experiments |
| `pynlm` | Python Module | Python bindings |

### Running Tests

```bash
cd /path/to/nlm/build
ctest --output-on-failure
```

---

## Python Bindings

NLM provides Python bindings through `pynlm` module, making it accessible to Python developers.

### Installation

#### From Source (Recommended for Development)

```bash
cd /path/to/nlm
pip install pybind11 scikit-build-core pytest numpy
pip install .  # Install C++ extensions
```

#### In Development Mode

```bash
cd /path/to/nlm
pip install -e .  # Install in development mode
```

### Basic Usage

#### Creating and Initializing a Brain

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Optionally modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simple Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Complete Agent Simulation

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(timestep=0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes input
    brain.step(step)
    
    # Get motor command
    motor_cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development Stage: {brain.getDevelopmentalStage()}")
```