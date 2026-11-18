# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

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
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

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

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

> Phase 5 note: `pip install .` now genuinely builds `pynlm` (verified:
> fresh venv, `pip install .`, then `import pynlm` and the easy_usage
> Example 1 flow run). Previously the root CMakeLists never built the
> module, so `pip install .` succeeded yet `import pynlm` failed. A
> plain C++ `cmake` build still does NOT include pynlm unless you pass
> `-DNLM_BUILD_PYTHON_BINDINGS=ON` (and have pybind11 installed); the
> standalone `cmake -S python -B build` route also still works.

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters for custom simulations
config.set("brain.neuron_count", 500)    # Smaller for faster testing
config.set("brain.synapse_density", 0.15)  # 15% connection probability
config.set("plasticity.stdp.learning_rate", 0.01)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Verify brain is properly initialized
assert brain.getTotalNeuronCount() > 0
assert brain.getTotalSynapseCount() > 0
```

#### Running a Simulation

```python
import pynlm
import time

# Create brain with optimized configuration for quick testing
config = pynlm.createDefaultConfig()

# Performance optimizations for faster testing
config.set("brain.neuron_count", 200)        # Fewer neurons for faster simulation
config.set("brain.synapse_density", 0.08)    # Lower connectivity
config.set("plasticity.stdp.learning_rate", 0.01)  # Slower learning
config.set("plasticity.hebbian.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Track simulation statistics
simulation_start = time.time()
step_count = 0
total_spikes = 0
learning_moments = 0

print("Starting simulation...")
print(f"Configuration: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")

# Run simulation loop
try:
    for step in range(500):  # 500 simulation steps
        brain.step(step)
        step_count += 1
        
        # Track statistics
        total_spikes += brain.getFiringNeuronCount()
        
        # Detect learning moments (when E/I ratio changes significantly)
        current_ei_ratio = brain.getExcitationInhibitionRatio()
        if step > 0 and abs(current_ei_ratio - learning_ei_ratio) > 0.1:
            learning_moments += 1
        learning_ei_ratio = current_ei_ratio
        
        # Print progress every 100 steps
        if step % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, {firing_rate:.1f} Hz avg")
            
except KeyboardInterrupt:
    print("\nSimulation interrupted by user")
except Exception as e:
    print(f"Simulation error: {e}")
    raise

simulation_end = time.time()
print("\nSimulation complete!")
print(f"Total steps: {step_count}")
print(f"Simulation time: {simulation_end - simulation_start:.2f} seconds")
print(f"Total firing events: {total_spikes}")
print(f"Learning moments detected: {learning_moments}")
print(f"Final average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Save brain state for later analysis
brain.save("simulation_checkpoint.nlc")
print("Brain state saved to simulation_checkpoint.nlc")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example with Error Handling

```python
import pynlm
import time

class NLMEnvironment:
    """Simple test environment with rewards and hazards."""
    
    def __init__(self, width=10, height=10):
        self.width = width
        self.height = height
        self.agent_x = width // 2
        self.agent_y = height // 2
        self.reset()
    
    def reset(self):
        """Reset environment to initial state."""
        self.agent_x = self.width // 2
        self.agent_y = self.height // 2
        self.steps = 0
        self.energy = 100.0
        self.hazards_hit = 0
    
    def update(self, timestep=0.1):
        """Update environment based on agent actions."""
        self.steps += 1
        self.energy -= timestep  # Energy consumption
        return timestep
    
    def getSensoryPercept(self):
        """Get sensory input from environment."""
        percept = pynlm.SensoryPercept()
        
        # Add vision data (simple distance to obstacles)
        vision = []
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                nx = self.agent_x + dx
                ny = self.agent_y + dy
                if 0 <= nx < self.width and 0 <= ny < self.height:
                    if (dx, dy) == (0, 0):
                        vision.append(1.0)  # Current position
                    elif (nx == 0 or nx == self.width-1 or 
                          ny == 0 or ny == self.height-1):
                        vision.append(1.0)  # Wall
                    else:
                        vision.append(0.0)  # Empty space
                else:
                    vision.append(0.0)  # Out of bounds
        
        percept.setVision(vision)
        
        # Add internal signals (energy, hunger, etc.)
        internal = [self.energy / 100.0]  # Normalized energy
        percept.setInternal(internal)
        
        return percept
    
    def applyMotorCommand(self, action, current_time):
        """Apply agent motor command to environment."""
        # Simple movement logic
        if action.type == pynlm.ActionType.MoveForward:
            if action.direction_x > 0:
                self.agent_x = min(self.agent_x + 1, self.width - 1)
            elif action.direction_x < 0:
                self.agent_x = max(self.agent_x - 1, 0)
            elif action.direction_y > 0:
                self.agent_y = min(self.agent_y + 1, self.height - 1)
            elif action.direction_y < 0:
                self.agent_y = max(self.agent_y - 1, 0)
        
        # Check for hazards
        if self.agent_x == self.width // 2 and self.agent_y == self.height // 2:
            # Center area has hazards
            self.hazards_hit += 1
            self.energy -= 20.0  # Damage from hazard
    
    def getEpisodeReward(self):
        """Get total reward for completed episode."""
        reward = self.energy - (self.steps * 0.5) - (self.hazards_hit * 10.0)
        return reward

def run_advanced_agent_simulation(num_steps=1000):
    """Run advanced agent simulation with comprehensive monitoring."""
    
    print("=== Advanced NLM Agent Simulation ===")
    
    # 1. Create optimized configuration
    config = pynlm.createDefaultConfig()
    
    # Enhanced configuration for complex behavior
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 0.8)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure environment
    world = NLMEnvironment(width=10, height=10)
    world.configure(width=10, height=10, visionWidth=3, visionHeight=3)
    world.reset()
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    
    # 7. Configure development tracking
    development_stages = []
    curiosity_levels = []
    novelty_levels = []
    
    # 8. Run simulation loop with comprehensive monitoring
    episode_start = time.time()
    successful_actions = 0
    reward_history = []
    
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
    print(f"Max steps: {num_steps}, Environment: {world.width}x{world.height}")
    
    try:
        for step in range(num_steps):
            # Update environment
            world.update(0.1)
            
            # Get sensory input from environment
            percept = world.getSensoryPercept()
            
            # Process sensory input in brain
            agent.processSensoryInput(percept)
            
            # Run brain step with neural computation
            brain.step(step)
            
            # Decode motor command from brain activity
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward modulation based on environment outcome
            episode_reward = world.getEpisodeReward()
            predicted_reward = agent.getPredictedReward()
            agent.applyRewardModulation(episode_reward, predicted_reward)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record metrics
            development_stages.append(brain.getDevelopmentalStage())
            curiosity_levels.append(agent.getCuriosityLevel())
            novelty_levels.append(agent.getNoveltyLevel())
            reward_history.append(episode_reward)
            
            # Track successful actions (simple heuristic)
            if brain.getFiringNeuronCount() > 10 and episode_reward > 0:
                successful_actions += 1
            
            # Print comprehensive progress
            if step % 100 == 0:
                firing_rate = brain.getAverageFiringRate()
                current_stage = development_stages[-1]
                curiosity = curiosity_levels[-1]
                novelty = novelty_levels[-1]
                reward = reward_history[-1]
                
                print(f"Step {step}:")
                print(f"  Neural Activity: {brain.getFiringNeuronCount()} firing neurons ({firing_rate:.1f} Hz)")
                print(f"  Development: {current_stage}")
                print(f"  Neuromodulation: curiosity={curiosity:.3f}, novelty={novelty:.3f}")
                print(f"  Environment: energy={world.energy:.1f}, hazards={world.hazards_hit}, reward={reward:.1f}")
                print(f"  Action Success: {successful_actions}/{step + 1} successful actions")
                print()
                
        episode_end = time.time()
        
        # 9. Analyze results
        print("=== Simulation Analysis ===")
        
        # Calculate statistics
        avg_curiosity = sum(curiosity_levels) / len(curiosity_levels)
        avg_novelty = sum(novelty_levels) / len(novelty_levels)
        avg_reward = sum(reward_history) / len(reward_history)
        
        final_stage = development_stages[-1]
        total_spikes = brain.getTotalSpikeCount()
        ei_ratio = brain.getExcitationInhibitionRatio()
        
        print(f"Total simulation time: {episode_end - episode_start:.2f} seconds")
        print(f"Steps completed: {num_steps}")
        print(f"Successful actions: {successful_actions}/{num_steps} ({(successful_actions/num_steps)*100:.1f}%)")
        print(f"Total neural spikes: {total_spikes}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"E/I ratio: {ei_ratio:.2f}")
        print(f"Average curiosity: {avg_curiosity:.3f}")
        print(f"Average novelty: {avg_novelty:.3f}")
        print(f"Average episode reward: {avg_reward:.2f}")
        print(f"Final developmental stage: {final_stage}")
        
        # Determine development level
        if final_stage == pynlm.DevelopmentalStage.Aging:
            print("Development: Mature, stable learning")
        elif final_stage == pynlm.DevelopmentalStage.CriticalPeriod:
            print("Development: Sensitive period, rapid learning")
        elif final_stage == pynlm.DevelopmentalStage.Maturation:
            print("Development: Maturing, balanced learning")
        else:
            print("Development: Initial phase, high plasticity")
        
        # Save brain state for analysis
        brain.save("advanced_agent_checkpoint.nlc")
        print("\nBrain state saved to advanced_agent_checkpoint.nlc")
        
        return {
            "success": True,
            "steps_completed": num_steps,
            "total_spikes": total_spikes,
            "final_ei_ratio": ei_ratio,
            "average_reward": avg_reward,
            "success_rate": successful_actions / num_steps,
            "simulation_time": episode_end - episode_start,
            "development_stage": final_stage
        }
        
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
        return {
            "success": False,
            "reason": "user_interrupt",
            "steps_completed": step
        }
    except Exception as e:
        print(f"\nSimulation error: {e}")
        import traceback
        traceback.print_exc()
        return {
            "success": False,
            "reason": str(e),
            "steps_completed": step
        }

# Run the advanced simulation
results = run_advanced_agent_simulation(500)
print(f"\nSimulation results: {results}")
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Configuration Options

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
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

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

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

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

**Python bindings not building:**
```bash
# Force build with Python bindings
pip install .
# Or build with explicit flag
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
```

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Configuration file errors:**
- Ensure config files are in proper format
- Check for duplicate keys (e.g., `dopamine_baseline` appears twice in old configs)
- Verify config syntax: `key = value` format

**Missing or invalid config keys:**
- Use `config.has("key")` to check if a config key exists
- Use `config.getOr("key", default_value)` for safe access
- Common keys: `brain.neuron_count`, `brain.synapse_density`, `plasticity.stdp.learning_rate`

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration values are within valid ranges

**Performance issues:**
- Too many neurons for system resources: reduce `brain.neuron_count`
- Complex connectivity: decrease `brain.synapse_density`
- Use performance optimizations: `config.set("performance.enable_simd", true)`

### Python API Issues

**Version compatibility:**
```python
# Check Python and NLM compatibility
import sys
import pynlm

print(f"Python: {sys.version}")
print(f"NLM: {pynlm.__version__}")
print(f"Python bindings available: {hasattr(pynlm, 'Brain')}")
```

**Missing optional dependencies:**
- Some advanced features may require additional libraries
- Check documentation for optional dependencies
- Use try/except blocks for graceful fallback

**Memory issues:**
- Large configurations may exceed memory limits
- Use memory-efficient settings: `config.set("brain.neuron_count", 100)`
- Monitor memory usage during long simulations

### Advanced Configuration Issues

**Invalid neuromodulation settings:**
- `curiosity_threshold` should be between 0.0 and 1.0
- `novelty_threshold` should be between 0.ávio and 1.0
- `dopamine_baseline` affects overall excitability

**Development stage issues:**
- Development stages should be sequential: Initial → CriticalPeriod → Maturation → Adult
- Check that `development_stage_rate` is appropriate for your simulation
- Ensure development system is properly initialized

**Memory system configuration:**
- `working_memory_size` and `episodic_capacity` should be appropriate for task
- Too small: risk of losing important information
- Too large: memory overflow and performance degradation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
