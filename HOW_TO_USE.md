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

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

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

### Complete Agent Example (Enhanced)

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration with error handling
    try:
        config = pynlm.createDefaultConfig()
        print(f"✓ Configuration created: {config.summary()}")
    except Exception as e:
        print(f"✗ Failed to create configuration: {e}")
        return
    
    # 2. Create brain with enhanced error handling
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"✗ Failed to initialize brain: {e}")
        return
    
    # 3. Create agent brain interface
    try:
        agent = pynlm.createAgentBrain(brain)
    except Exception as e:
        print(f"✗ Failed to create agent: {e}")
        return
    
    # 4. Create and configure world
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        print("✓ World configured")
    except Exception as e:
        print(f"✗ Failed to configure world: {e}")
        return
    
    # 5. Initialize agent with world
    try:
        agent.initialize(world)
        print("✓ Agent initialized")
    except Exception as e:
        print(f"✗ Failed to initialize agent: {e}")
        return
    
    # 6. Enable learning subsystems with validation
    try:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        print("✓ Learning subsystems enabled")
    except Exception as e:
        print(f"✗ Failed to enable subsystems: {e}")
        # Continue without all subsystems for basic functionality
    
    # 7. Enhanced simulation loop with error handling and logging
    try:
        for step in range(num_steps):
            try:
                # Update world
                world.update(timestep=0.1)
                
                # Get sensory input from world
                percept = world.getSensoryPercept()
                
                # Process sensory input in brain
                agent.processSensoryInput(percept)
                
                # Run brain step with timestamp
                brain.step(step, step * 0.1)
                
                # Decode motor command from brain activity
                motor_cmd = agent.decodeMotorCommand()
                
                # Apply motor command to world
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Apply reward modulation with validation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development if enabled
                agent.updateDevelopment(0.1)
                
                # Print progress periodically with enhanced metrics
                if step % 100 == 0:
                    print(f"Step {step}:")
                    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
                    if agent.isRewardModulationEnabled():
                        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                        print(f"  Prediction error: {agent.getPredictionError():.3f}")
                    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
                    
            except Exception as e:
                print(f"✗ Error at step {step}: {e}")
                # Continue with next step for robustness
                continue
        
        print(f"✓ Simulation completed: {num_steps} steps")
        
    except KeyboardInterrupt:
        print("\n⏸ Simulation paused by user")
    except Exception as e:
        print(f"✗ Simulation failed: {e}")

# Run the simulation with enhanced error handling
try:
    run_agent_simulation(1000)
    print("\n🎉 Agent simulation finished successfully!")
except Exception as e:
    print(f"\n💥 Agent simulation failed: {e}")
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

### Advanced Usage Patterns

```python
import pynlm
import time

def run_production_simulation(config, world, agent, num_steps=5000, 
                            checkpoint_interval=1000, log_interval=500):
    """Production-ready simulation with checkpointing and detailed logging."""
    
    brain = agent.getBrain()
    start_time = time.time()
    
    try:
        for step in range(num_steps):
            # Update environment
            world.update(timestep=0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step, step * 0.1)
            
            # Get action and execute
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply learning with error handling
            try:
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            except Exception as e:
                # Log learning errors but continue simulation
                print(f"  Warning: Learning failed at step {step}: {e}")
            
            # Detailed logging
            if step % log_interval == 0:
                elapsed = time.time() - start_time
                print(f"  Step {step}/{num_steps} ({elapsed:.1f}s, "
                      f"{step/elapsed:.1f} steps/sec)")
                print(f"    Stats: {brain.getFiringNeuronCount()} firing, "
                      f"{agent.getCuriosityLevel():.2f} curiosity, "
                      f"{agent.getNoveltyLevel():.2f} novelty")
                
                # Check if learning is progressing
                if step > 0 and step % (log_interval * 2) == 0:
                    current_rate = brain.getAverageFiringRate()
                    print(f"    Brain rate: {current_rate:.2f} Hz")
            
            # Checkpoint periodically
            if step % checkpoint_interval == 0 and step > 0:
                try:
                    checkpoint_file = f"checkpoint_step_{step:06d}.bin"
                    brain.save(checkpoint_file)
                    print(f"  Checkpoint saved: {checkpoint_file}")
                except Exception as e:
                    print(f"  Warning: Checkpoint failed: {e}")
        
        total_time = time.time() - start_time
        print(f"✓ Simulation completed in {total_time:.1f}s "
              f"({num_steps/total_time:.1f} steps/sec)")
        
        # Final stats
        final_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        print(f"  Final stats: {final_rate:.2f} Hz average, {total_spikes} total spikes")
        
        return True
        
    except KeyboardInterrupt:
        print(f"\n⏸ Simulation interrupted at step {step}")
        return False
    except Exception as e:
        print(f"\n💥 Simulation failed: {e}")
        return False

def run_comparison_simulation(config, world_template, agent_template, 
                             scenarios, base_scenario="baseline"):
    """Run multiple scenarios for comparison."""
    
    results = {}
    
    for scenario_name in scenarios:
        print(f"\n=== Running Scenario: {scenario_name} ===")
        
        # Create fresh world and agent for each scenario
        world = pynlm.createSimpleWorld()
        world.configure(world_template['width'], world_template['height'],
                       world_template['visionWidth'], world_template['visionHeight'])
        world.reset()
        world.setAgentStart(world_template['start_x'], world_template['start_y'])
        
        agent = pynlm.createAgentBrain(agent_template.getBrain())
        agent.initialize(world)
        
        # Apply scenario-specific settings
        if scenario_name != base_scenario:
            # Example: Modify world parameters based on scenario
            if 'width' in world_template:
                world.configure(world_template['width'] + 5, world_template['height'] + 5,
                               world_template['visionWidth'], world_template['visionHeight'])
            
            # Enable/disable specific features
            if 'reward_modulation' in world_template:
                agent.enableRewardModulation(world_template['reward_modulation'])
        
        # Run simulation
        success = run_production_simulation(
            config, world, agent, num_steps=2000, log_interval=500
        )
        
        if success:
            brain = agent.getBrain()
            results[scenario_name] = {
                'final_firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'success': True
            }
        else:
            results[scenario_name] = {
                'final_firing_rate': 0.0,
                'total_spikes': 0,
                'success': False
            }
    
    # Compare results
    print(f"\n=== Scenario Comparison ===")
    for scenario, result in results.items():
        status = "✓" if result['success'] else "✗"
        print(f"{status} {scenario}: {result['final_firing_rate']:.2f} Hz, "
              f"{result['total_spikes']} spikes")
    
    return results

# Production-ready setup function
def setup_production_environment(neuron_count=3000, enable_learning=True):
    """Set up a production-ready NLM environment with optimal configuration."""
    
    # Create optimized configuration
    config = pynlm.createDefaultConfig()
    
    # Optimize for production performance
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.connection_probability", 0.06)
    config.set("brain.initial_weight_mean", 0.25)
    config.set("brain.initial_weight_std", 0.12)
    
    # Fine-tune plasticity for production stability
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.0015)
    config.set("plasticity.stdp.tau_plus", 22.0)
    config.set("plasticity.stdp.tau_minus", 22.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.0008)
    config.set("plasticity.structural.enable", enable_learning)
    
    # Configure neuromodulation for exploration-exploitation balance
    config.set("neuromod.dopamine.scale", 1.1)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.sensitivity", 0.7)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.15)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all features
    agent = pynlm.createAgentBrain(brain)
    
    print(f"✓ Production environment setup complete")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Learning features: {agent.isRewardModulationEnabled()}, "
          f"{agent.isStructuralPlasticityEnabled()}, "
          f"{agent.isDevelopmentEnabled()}, {agent.isCuriosityEnabled()}")
    
    return config, agent

# Example usage patterns
print("=== NLM Advanced Usage Patterns ===")
print()
print("1. Production Simulation:")
print("   Use run_production_simulation() for stable, long-running simulations")
print("   Includes checkpointing, detailed logging, and error recovery")
print()
print("2. Scenario Comparison:")
print("   Use run_comparison_simulation() to test different configurations")
print("   Useful for hyperparameter optimization and A/B testing")
print()
print("3. Production Setup:")
print("   Use setup_production_environment() for deployment-ready configurations")
print("   Optimized for performance and stability")
print()
```
