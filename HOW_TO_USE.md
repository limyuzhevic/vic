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

## Part 3: Using the Python Library (Enhanced)

### Basic Usage

#### Creating a Brain with Advanced Configuration (New)

```python
import pynlm

# Create optimized configuration for research
config = pynlm.createResearchConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Research brain created: {brain.getTotalNeuronCount()} neurons")
print(f"STDP enabled: {brain.getConfig().has('plasticity.stdp.enable')}")
```

#### Using Performance Monitoring (New)

```python
import pynlm

# Create brain with performance monitoring
config = pynlm.createOptimizedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate while monitoring performance
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        # Get performance statistics
        stats = brain.getBrainPerformanceStats()
        print(f"Step {step}:")
        print(f"  CPU time: {stats['cpu_time_ms']:.2f}ms")
        print(f"  Memory usage: {stats['memory_usage_mb']:.2f}MB")
        print(f"  Neural activity: {stats['avg_firing_rate']:.3f} Hz")
        print(f"  Plasticity events: {stats['plasticity_events']}")

# Generate memory usage report
report = pynlm.memoryUsageReport()
print(f"\nPerformance Report:\n{report}")
```

#### Advanced Simulation Control (New)

```python
import pynlm
import time

# Create brain with advanced control features
config = pynlm.createBrainConfig(neuron_count=500, enable_development=True)
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced simulation controls
pause_brain = False
step_size = 0.1

for step in range(200):
    if pause_brain:
        print("Brain paused - waiting for resume command")
        time.sleep(0.1)
        continue
    
    brain.step(step)
    
    # Advanced step control
    if step % 50 == 0:
        # Adaptive step sizing
        firing_rate = brain.getAverageFiringRate()
        if firing_rate < 1.0:
            step_size = 0.2  # Speed up when low activity
        else:
            step_size = 0.1
    
    if step % 30 == 0:
        # Get simulation statistics
        world_stats = world.getWorldSimulationStats()
        print(f"Step {step}: {world_stats['steps_per_second']:.2f} steps/s")

# Pause brain for inspection
brain.pauseBrain()
print("Brain paused for inspection")

# Resume and continue
brain.resumeBrain()
print("Brain resumed")
```

#### Neural Network Analysis and Visualization (New)

```python
import pynlm
import matplotlib.pyplot as plt

# Create brain and collect data for analysis
config = pynlm.createResearchConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect activation data over time
activation_history = []
for step in range(300):
    brain.step(step)
    activation_map = brain.getActivationMap()
    activation_history.append(activation_map)

# Analyze network topology
network_topology = brain.getNetworkTopology()
print("Network Analysis:")
print(f"  Total neurons: {network_topology['total_neurons']}")
print(f"  Total synapses: {network_topology['total_synapses']}")
print(f"  Connectivity density: {network_topology['connectivity_density']:.4f}")
print(f"  Average clustering: {network_topology['avg_clustering_coefficient']:.4f}")
print(f"  Characteristic path length: {network_topology['characteristic_path_length']:.4f}")

# Analyze developmental patterns
development_analysis = brain.analyzeBrainDevelopment()
print(f"\nDevelopmental Analysis:")
print(f"  Development stage: {development_analysis['current_stage']}")
print(f"  Plasticity rate: {development_analysis['plasticity_rate']:.4f}")
print(f"  Neural maturation: {development_analysis['neural_maturation']:.4f}")

# Generate visualization
try:
    connectivity_plot = brain.generateNeuronConnectivityPlot()
    plt.figure(figsize=(12, 8))
    plt.imshow(connectivity_plot, cmap='viridis')
    plt.title('Neural Network Connectivity')
    plt.colorbar(label='Connection strength')
    plt.savefig('network_connectivity.png')
    print("Connectivity plot saved as 'network_connectivity.png'")
except Exception as e:
    print(f"Visualization generation failed: {e}")
```

#### Experience Replay and Memory Management (New)

```python
import pynlm

# Create agent with memory management
config = pynlm.createBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world=None)  # No world for this test

# Enable memory management features
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)

# Simulate to build up experiences
for step in range(200):
    brain.step(step)
    
    # Simulate some learning
    if step % 10 == 0:
        reward = 1.0 if step % 20 == 0 else 0.0
        predicted = 0.5
        agent.applyRewardModulation(reward, predicted)

# Access experience replay
try:
    experiences = agent.getExperienceReplay(max_experiences=50)
    print(f"Retrieved {len(experiences)} experiences")
    
    for i, exp in enumerate(experiences[:5]):  # Show first 5
        print(f"  Experience {i+1}:")
        print(f"    Reward: {exp['reward']}")
        print(f"    Prediction error: {exp['prediction_error']}")
        print(f"    Time step: {exp['time_step']}")
        
except Exception as e:
    print(f"Experience replay failed: {e}")
```

#### Benchmarking and Validation (New)

```python
import pynlm
import time

# Benchmark different configurations
print("Benchmarking Brain Configurations")
print("=" * 50)

configs = [
    ("Small", {"neuron_count": 100, "synapse_density": 0.05}),
    ("Medium", {"neuron_count": 500, "synapse_density": 0.1}),
    ("Large", {"neuron_count": 1000, "synapse_density": 0.15}),
]

for name, params in configs:
    start_time = time.time()
    config = pynlm.createResearchConfig()
    for key, value in params.items():
        config.set(f"brain.{key}", value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(200):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Get benchmark results
    benchmark_result = pynlm.simulationStepBenchmark(
        brain, 200, "performance_test", validate_config=True
    )
    
    print(f"\n{name} Brain ({params['neuron_count']} neurons):")
    print(f"  Time: {elapsed:.2f}s")
    print(f"  Steps/s: {benchmark_result['steps_per_second']:.2f}")
    print(f"  Memory: {benchmark_result['memory_usage_mb']:.2f}MB")
    print(f"  Config valid: {benchmark_result['config_valid']}")

# Validate configuration
validation_result = pynlm.validateBrainConfig(config)
print(f"\nConfiguration Validation:")
print(f"  Valid: {validation_result['valid']}")
print(f"  Issues: {validation_result['issues']}")
print(f"  Recommendations: {validation_result['recommendations']}")
```

#### Complete Reinforcement Learning Example (Advanced)

```python
import pynlm

def run_advanced_rl_episode(num_steps=1000):
    """Complete reinforcement learning episode with monitoring and analysis."""
    
    # 1. Create optimized learning environment
    brain, world, agent = pynlm.createLearningEnvironment(
        width=20,
        height=20,
        vision_width=10,
        vision_height=10,
        enable_reward_modulation=True,
        enable_curiosity=True,
        enable_structural_plasticity=True,
        enable_development=True,
        neuron_count=1500,
        learning_rate=0.01,
        exploration_rate=0.3,
        development_stages=7
    )
    
    # 2. Run learning loop with monitoring
    learning_metrics = {
        'rewards': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': [],
        'action_counts': {}
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get percept and process it
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Record metrics
        learning_metrics['rewards'].append(reward)
        learning_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        learning_metrics['novelty_levels'].append(agent.getNoveltyLevel())
        learning_metrics['development_stages'].append(brain.getDevelopmentalStage())
        
        action_type = str(action.getType())
        learning_metrics['action_counts'][action_type] = learning_metrics['action_counts'].get(action_type, 0) + 1
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print(f"  Action distribution: {learning_metrics['action_counts']}")
    
    # 3. Analyze learning results
    final_reward = learning_metrics['rewards'][-1]
    avg_reward = sum(learning_metrics['rewards']) / len(learning_metrics['rewards'])
    final_curiosity = learning_metrics['curiosity_levels'][-1]
    final_development = learning_metrics['development_stages'][-1]
    
    # Get agent learning metrics
    agent_metrics = agent.getLearningMetrics()
    
    # Generate performance report
    performance_report = {
        'total_steps': num_steps,
        'final_reward': final_reward,
        'average_reward': avg_reward,
        'final_curiosity': final_curiosity,
        'final_development_stage': final_development,
        'agent_metrics': agent_metrics,
        'action_distribution': learning_metrics['action_counts']
    }
    
    print(f"\nRL Episode Complete!")
    print(f"Total steps: {num_steps}")
    print(f"Final reward: {final_reward:.3f}")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Final curiosity: {final_curiosity:.3f}")
    print(f"Final development stage: {final_development}")
    
    return performance_report, learning_metrics

# Run the complete RL example
performance, metrics = run_advanced_rl_episode(1000)
print(f"\nPerformance Report:\n{performance}")
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

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
