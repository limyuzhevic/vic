# NLM (Neural Learning Machine) - HOW TO USE

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API (NEW FEATURES)

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

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

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

## Part 3: Using the Enhanced NLM Python API

The NLM Python API has been significantly enhanced with new Pythonic features, error handling, and utility functions.

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
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

### NEW: Enhanced Features

#### 1. Context Manager for Resource Management

```python
import pynlm

# Use context manager for automatic cleanup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

with pynlm.brain_context(brain) as brain_ctx:
    brain_ctx.get().initialize()
    print(f"Brain initialized with {brain_ctx.get().getTotalNeuronCount()} neurons")
    
    # Run simulation
    for i in range(50):
        brain_ctx.get().step(i)
    
    print(f"Final spike count: {brain_ctx.get().getTotalSpikeCount()}")
    # Brain is automatically reset when exiting context
```

#### 2. Iterator Support for Brain Regions

```python
import pynlm

# Create brain and add regions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add different brain regions
cortex_id = brain.addRegion("cortex")
hippocampus_id = brain.addRegion("hippocampus")

# Use region iterator
print("Brain regions:")
for region in brain.getRegions():
    print(f"  {region.get_name()}: {region.get_id().value} neurons")

# Filter regions by neuron count
large_regions = [r for r in brain.getRegions() if r.get_neuron_count() > 100]
print(f"\nRegions with >100 neurons: {len(large_regions)}")
```

#### 3. Simulation Runner for Controlled Execution

```python
import pynlm

# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(200, 200, 16, 16)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Use simulation runner for controlled execution
runner = pynlm.create_simulation(brain, world, agent)

if runner.start(1000, 0.05):  # Run 1000 steps
    print("Starting simulation...")
    step = 0
    while runner.is_running():
        if not runner.step():
            break
        step += 1
        
        if step % 100 == 0:
            stats = runner.get_stats()
            print(f"Step {step}: "
                  f"Neurons={stats['firing_neurons']}, "
                  f"Curiosity={stats['curiosity_level']:.3f}, "
                  f"Progress={runner.get_progress():.1%}")
    
    print(f"Simulation completed after {step} steps")
    print(f"Final stats: {runner.get_stats()}")
else:
    print("Failed to start simulation")
```

#### 4. Input Validation

```python
import pynlm

# Validate configuration values
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

# Check if required keys exist
if config.has("brain.neuron_count"):
    neuron_count = pynlm.get_int(config, "brain.neuron_count")
    if neuron_count <= 0:
        raise ValueError("Neuron count must be positive")
    elif neuron_count > 10000:
        print("Warning: Large neuron count may impact performance")

# Validate world boundaries
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)
world.setAgentStart(50, 50)

# Validate agent position is within world
if not world.isValidPosition(50, 50):
    raise ValueError("Agent start position is invalid")

# Validate using Validator class
if not pynlm.Validator.validate_position(50, 50, 100, 100):
    print("Position validation failed")
```

#### 5. Enhanced Utility Functions

```python
import pynlm

# Vector operations
values = [1.0, 2.0, 3.0, 4.0, 5.0]
print(f"Sum: {pynlm.sum(values)}")
print(f"Average: {pynlm.average(values)}")
print(f"Is 5 even? {pynlm.is_even(5)}")
print(f"Is 5 odd? {pynlm.is_odd(5)}")

# Statistics formatting
stats = {"neurons": 1000, "spikes": 5000, "rate": 0.5}
print(f"Formatted stats: {pynlm.format_stats(stats)}")
```

#### 6. Configuration Helpers

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.enable", True)

# Use helpers for safe access
try:
    neuron_count = pynlm.get_int(config, "brain.neuron_count")
    stdp_enabled = pynlm.get_bool(config, "plasticity.stdp.enable")
    print(f"Neuron count: {neuron_count}, STDP enabled: {stdp_enabled}")
except Exception as e:
    print(f"Error accessing config: {e}")

# Helper for getting string values
try:
    name = pynlm.get_string(config, "nonexistent.key")
except Exception as e:
    print(f"Key error: {e}")
```

#### 7. Convenience Functions

```python
import pynlm

# Create a complete agent with defaults
brain, world, agent = pynlm.create_agent_with_defaults()
print(f"Created agent with {brain.getTotalNeuronCount()} neurons")

# Run a simple episode
pynlm.run_agent_episode(agent, world, 100)
```

#### 8. Progress Tracking

```python
import pynlm
import time

# Create progress tracker
progress_tracker = pynlm.create_progress_tracker(1000)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    progress_tracker.update(step + 1)
    
    if (step + 1) % 100 == 0:
        print(f"Progress: {progress_tracker.get_percentage():.1%}, "
              f"Remaining: {progress_tracker.get_remaining_steps()} steps")
```

#### 9. Event System

```python
import pynlm

# Create event system
event_system = pynlm.create_event_system()

# Define event handler
def handle_step_event(event):
    step = event.get_step()
    if step % 100 == 0:
        print(f"Milestone: Step {step} completed")

event_system.add_event_handler(handle_step_event)

# Run simulation with events
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(500):
    brain.step(step)
    
    # Create and dispatch step event
    step_event = pynlm.StepEvent(step, step * 0.1)
    event_system.dispatch_event(step_event)
```

#### 10. Performance Profiling

```python
import pynlm
import time

# Create profiler
profiler = pynlm.create_profiler()

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

profiler.start()
for step in range(500):
    brain.step(step)
profiler.stop()

print(f"Total time: {profiler.get_total_time():.3f}s")
print(f"Average per step: {profiler.get_average_time_per_step():.6f}s")
print(f"Steps per second: {1/profiler.get_average_time_per_step():.1f}")
```

### Complete Agent Example

```python
import pynlm

def run_complete_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Use simulation runner for controlled execution
    runner = pynlm.create_simulation(brain, world, agent)
    
    if runner.start(num_steps):
        print(f"Starting complete agent simulation ({num_steps} steps)...")
        
        for step in range(num_steps):
            if not runner.step():
                break
            
            if step % 100 == 0:
                stats = runner.get_stats()
                print(f"Step {step}:")
                print(f"  Firing neurons: {stats['firing_neurons']}")
                print(f"  Curiosity: {stats['curiosity_level']:.3f}")
                print(f"  Novelty: {stats['novelty_level']:.3f}")
                print(f"  Dev Stage: {stats['development_stage']}")
        
        print(f"Simulation complete!")
        print(f"Final stats: {runner.get_stats()}")
    else:
        print("Failed to start simulation")
    
    return runner

# Run the simulation
simulation = run_complete_agent_simulation(1000)
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
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state

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
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
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

### Configuration Options

#### Brain Configuration

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

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### New Convenience Functions

#### `brain_context(brain)`
Create a context manager for a brain that ensures proper cleanup.

#### `create_simulation(brain, world=None, agent=None)`
Create a simulation runner for controlled execution.

#### `create_agent_with_defaults()`
Create a brain, world, and agent with default settings.

#### `run_agent_episode(agent, world, num_steps, enable_learning=True)`
Run a complete agent episode with optional learning.

#### `create_simple_brain()`
Create a simple brain with default configuration.

#### `create_simple_world(width, height, vision_width, vision_height)`
Create a simple world with specified dimensions.

#### `clone_config(config)`
Create a copy of a configuration.

#### `create_progress_tracker(total_steps)`
Create a progress tracker for long simulations.

#### `create_event_system()`
Create an event system for event-driven processing.

#### `create_profiler()`
Create a profiler for performance monitoring.

### Advanced Usage Examples

#### 1. Complete Learning Agent

```python
import pynlm

def train_learning_agent(epochs=10, steps_per_epoch=200):
    """Train an agent over multiple epochs with learning enabled."""
    
    # Create agent with all learning systems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(100, 100, 12, 12)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting training for {epochs} epochs...")
    
    for epoch in range(epochs):
        print(f"\nEpoch {epoch + 1}/{epochs}")
        
        runner = pynlm.create_simulation(brain, world, agent)
        if runner.start(steps_per_epoch):
            while runner.is_running():
                if not runner.step():
                    break
            
            epoch_stats = runner.get_stats()
            print(f"  Final curiosity: {epoch_stats['curiosity_level']:.3f}")
            print(f"  Final novelty: {epoch_stats['novelty_level']:.3f}")
            print(f"  Final development stage: {epoch_stats['development_stage']}")
            
            # Reset for next epoch
            world.reset()
            agent.reset()
        else:
            print(f"Failed to start epoch {epoch + 1}")
    
    print("\nTraining completed!")
    return epoch_stats

# Run training
training_stats = train_learning_agent(epochs=5, steps_per_epoch=100)
```

#### 2. Performance Benchmarking

```python
import pynlm
import time

def benchmark_configuration(config_name, config_params, steps=500):
    """Benchmark a specific brain configuration."""
    
    config = pynlm.createDefaultConfig()
    for key, value in config_params.items():
        config.set(key, value)
    
    # Run benchmark
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    profiler = pynlm.create_profiler()
    profiler.start()
    
    for step in range(steps):
        brain.step(step)
    
    profiler.stop()
    
    # Calculate performance metrics
    total_time = profiler.get_total_time()
    avg_time_per_step = profiler.get_average_time_per_step()
    steps_per_second = steps / total_time if total_time > 0 else 0
    neurons_per_second = brain.getTotalSpikeCount() / total_time if total_time > 0 else 0
    
    return {
        'config_name': config_name,
        'total_time': total_time,
        'avg_time_per_step': avg_time_per_step,
        'steps_per_second': steps_per_second,
        'neurons_per_second': neurons_per_second,
        'final_spikes': brain.getTotalSpikeCount(),
        'final_firing': brain.getFiringNeuronCount(),
        'final_rate': brain.getAverageFiringRate(),
        'synapse_density': brain.getTotalSynapseCount() / brain.getTotalNeuronCount()
    }

# Compare different configurations
configs = {
    'small_fast': {'brain.neuron_count': 500, 'brain.synapse_density': 0.05},
    'medium_balanced': {'brain.neuron_count': 1000, 'brain.synapse_density': 0.1},
    'large_complex': {'brain.neuron_count': 2000, 'brain.synapse_density': 0.2},
}

results = []
for config_name, params in configs.items():
    print(f"Benchmarking {config_name}...")
    result = benchmark_configuration(config_name, params, 300)
    results.append(result)
    print(f"  Steps/sec: {result['steps_per_second']:.1f}")
    print(f"  Neurons/sec: {result['neurons_per_second']:.0f}")
    print(f"  Final firing rate: {result['final_rate']:.3f}")

# Find best configuration
best = max(results, key=lambda x: x['steps_per_second'])
print(f"\nBest configuration: {best['config_name']}")
print(f"Performance: {best['steps_per_second']:.1f} steps/sec")
```

#### 3. Multi-Agent Simulation

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor
import time

class NLMEnvironment:
    """Shared environment for multiple agents."""
    
    def __init__(self, width, height, num_agents):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 8, 8)
        self.agents = []
        
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            self.agents.append(agent)
            
            # Place agents at different starting positions
            x = (i + 1) * (width // (num_agents + 1))
            y = (i + 1) * (height // (num_agents + 1))
            self.world.setAgentStart(x, y)
            agent.initialize(self.world)
            
            # Enable learning for social interaction
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
    
    def get_sensory_input(self, agent_index):
        """Get sensory input for specific agent."""
        # Each agent gets its own view of the world
        return self.world.getSensoryPercept()
    
    def apply_actions(self, actions):
        """Apply motor commands from all agents."""
        for i, action in enumerate(actions):
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    def update(self, timestep):
        """Update world state."""
        self.world.update(timestep)
    
    def get_stats(self):
        """Get environment statistics."""
        stats = {
            'world_time': self.world.getSimulationTime(),
            'agent_count': len(self.agents),
            'total_objects': self.world.getObjectCount() if hasattr(self.world, 'getObjectCount') else 0
        }
        
        # Add agent-specific stats
        for i, agent in enumerate(self.agents):
            stats[f'agent_{i}_curiosity'] = agent.getCuriosityLevel()
            stats[f'agent_{i}_novelty'] = agent.getNoveltyLevel()
        
        return stats

def run_agent_episode(agent, env, steps, agent_id):
    """Run episode for a single agent."""
    print(f"Agent {agent_id} starting episode...")
    
    # Reset agent and world
    env.world.reset()
    agent.reset()
    
    # Run episode
    for step in range(steps):
        # Get sensory input
        percept = env.get_sensory_input(agent_id)
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        env.apply_actions([action] + [pynlm.Action()] * (len(env.agents) - 1))
        
        # Update world
        env.update(0.1)
        
        # Apply neuromodulation
        curiosity = agent.getCuriosityLevel()
        agent.applyRewardModulation(curiosity * 0.1, 0.0)
    
    print(f"Agent {agent_id} completed episode")
    return agent.get_total_spike_count()

# Create multi-agent environment
env = NLMEnvironment(100, 100, 3)

# Run episodes for all agents
start_time = time.time()

with ThreadPoolExecutor(max_workers=3) as executor:
    futures = []
    
    for i, agent in enumerate(env.agents):
        future = executor.submit(run_agent_episode, agent, env, 100, i)
        futures.append(future)
    
    # Wait for all episodes to complete
    results = [future.result() for future in futures]

elapsed = time.time() - start_time
print(f"\nMulti-agent simulation completed in {elapsed:.1f}s")
print(f"Agent spike counts: {results}")
print(f"Environment stats: {env.get_stats()}")
```

## Installation Troubleshooting

### Common Issues and Solutions

**Issue: CMake cannot find Python**
```bash
# Install required dependencies first
pip install scikit-build-core pybind11

# Configure with explicit Python path
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Issue: Compilation errors**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

**Issue: ImportError: No module named 'pynlm'**
```bash
# Force reinstall
pip install --force-reinstall .
```

**Issue: Segmentation faults**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

## What's New in Enhanced Version

### Key Improvements:

1. **Error Handling**: Proper exception handling for all Python bindings
2. **Input Validation**: Comprehensive input validation with meaningful error messages
3. **Pythonic Features**: Context managers, iterators, and generator-style patterns
4. **Simulation Control**: `SimulationRunner` class for controlled execution
5. **Utility Functions**: Enhanced mathematical and statistical utilities
6. **Configuration Helpers**: Safe access functions for configuration values
7. **Progress Tracking**: Built-in progress tracking for long simulations
8. **Event System**: Event-driven programming model
9. **Performance Profiling**: Built-in profiling capabilities
10. **Convenience Functions**: High-level factory functions for common patterns
11. **Memory Management**: Improved memory safety and automatic cleanup
12. **Documentation**: Enhanced documentation with comprehensive examples

### Breaking Changes:

- Some methods now raise exceptions instead of returning bool for errors
- Iterator patterns may differ from existing code patterns
- Enhanced API is backward compatible for existing functionality

## Migration Guide

### From Old to New API

**Old Pattern**:
```python
# Error handling - old way (implicit)
if brain.initialize() and brain.step(0):
    print("Success")
```

**New Pattern**:
```python
# Error handling - new way (explicit)
try:
    brain.initialize()
    brain.step(0)
    print("Success")
except RuntimeError as e:
    print(f"Error: {e}")
```

**Old Pattern**:
```python
# Manual cleanup
brain.reset()
del brain
```

**New Pattern**:
```python
# Automatic cleanup with context manager
with pynlm.brain_context(brain) as brain_ctx:
    brain_ctx.get().initialize()
    # ... use brain ...
    # Automatic cleanup when exiting context
```

## License

MIT

## Authors

Research project — See docs for scientific background.
