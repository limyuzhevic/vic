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

### Advanced Agent Examples

#### Multi-Agent System

```python
import pynlm

def create_multi_agent_system(config, num_agents=3):
    """Create a system with multiple interacting agents."""
    
    # Create shared brain configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brains
    agents = []
    for i in range(num_agents):
        agent = pynlm.createAgentBrain(brain)
        agents.append(agent)
    
    return brain, agents

def run_multi_agent_episode(brain, agents, world, num_steps):
    """Run an episode with multiple agents."""
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Each agent processes sensory input
        for i, agent in enumerate(agents):
            # Offset agent starting positions
            body = world.getAgentBody()
            body.x = 10.0 + i * 2.0
            body.y = 10.0 + i * 2.0
            
            agent.processSensoryInput(world.getSensoryPercept())
        
        # Run brain step (shared among all agents)
        brain.step(step)
        
        # Each agent decodes motor commands
        for agent in agents:
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward based on agent performance
            reward = calculate_agent_reward(agent, world)
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
    
    return brain, agents

def calculate_agent_reward(agent, world):
    """Calculate reward for an agent based on performance."""
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    # Simple reward function
    reward = (curiosity * 0.4 + novelty * 0.3 + prediction_error * 0.3)
    
    # Bonus for exploration
    if curiosity > 0.7:
        reward += 0.5
    
    return max(0.0, reward)
```

#### Brain State Management

```python
import pynlm
import time

def save_brain_states(brain, num_saves=5):
    """Save brain state at regular intervals."""
    
    for i in range(num_saves):
        timestamp = int(time.time())
        filename = f"brain_checkpoint_{i}_{timestamp}.bin"
        
        brain.save(filename)
        print(f"Saved brain state to {filename}")
        
        # Run some simulation
        for step in range(100):
            brain.step(step)
        
        # Load back the state
        brain.load(filename)
        print(f"Loaded brain state from {filename}")

def compare_brain_states(config):
    """Compare different brain states and analyze differences."""
    
    # Create two brains with same initial configuration
    brain1 = pynlm.createBrain(config)
    brain2 = pynlm.createBrain(config)
    
    brain1.initialize()
    brain2.initialize()
    
    # Run different simulations
    for step in range(200):
        brain1.step(step)
        brain2.step(step)
    
    # Compare states
    print("=== Brain State Comparison ===")
    print(f"Brain 1 - Neurons: {brain1.getTotalNeuronCount()}, Spikes: {brain1.getTotalSpikeCount()}")
    print(f"Brain 2 - Neurons: {brain2.getTotalNeuronCount()}, Spikes: {brain2.getTotalSpikeCount()}")
    
    # Calculate differences
    neuron_diff = abs(brain1.getTotalNeuronCount() - brain2.getTotalNeuronCount())
    spike_diff = abs(brain1.getTotalSpikeCount() - brain2.getTotalSpikeCount())
    
    print(f"Differences - Neurons: {neuron_diff}, Spikes: {spike_diff}")
    
    return neuron_diff, spike_diff
```

#### Performance Monitoring

```python
import pynlm
import time

def monitor_performance(brain, world, agent, num_steps):
    """Monitor performance metrics during simulation."""
    
    metrics = {
        'steps': [],
        'firing_rates': [],
        'spike_counts': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'simulation_times': []
    }
    
    start_time = time.time()
    
    for step in range(num_steps):
        step_start = time.time()
        
        # Record metrics before step
        metrics['steps'].append(step)
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['spike_counts'].append(brain.getTotalSpikeCount())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        metrics['prediction_errors'].append(agent.getPredictionError())
        
        # Run step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
        
        step_time = time.time() - step_start
        metrics['simulation_times'].append(step_time)
        
        # Print progress every 10% of simulation
        if step % (num_steps // 10) == 0:
            avg_step_time = sum(metrics['simulation_times']) / len(metrics['simulation_times'])
            print(f"Progress: {step}/{num_steps} steps, "
                  f"Avg step time: {avg_step_time:.4f}s")
    
    total_time = time.time() - start_time
    metrics['total_time'] = total_time
    metrics['avg_step_time'] = total_time / num_steps
    
    print(f"\n=== Performance Summary ===")
    print(f"Total time: {total_time:.2f}s")
    print(f"Average step time: {metrics['avg_step_time']:.4f}s")
    print(f"Steps per second: {num_steps/total_time:.2f}")
    
    return metrics
```

### Advanced Configuration Techniques

#### Nested Configuration

```python
import pynlm

def create_advanced_config():
    """Create a configuration with nested parameters."""
    
    config = pynlm.createDefaultConfig()
    
    # Modify nested configuration paths
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.08)
    config.set("brain.initial_weight_mean", 0.6)
    config.set("brain.initial_weight_std", 0.15)
    
    # Plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 22.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Development settings
    config.set("development.synaptogenesis_rate", 0.0005)
    config.set("development.pruning_rate", 0.0002)
    config.set("development.maturation_rate", 0.001)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.dopamine.baseline", 0.15)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Memory systems
    config.set("memory.working.capacity", 100)
    config.set("memory.episodic.capacity", 50)
    config.set("memory.associative.learning_rate", 0.01)
    
    # Prediction system
    config.set("prediction.model_type", std::string("neural"))
    config.set("prediction.horizon", 10)
    config.set("prediction.uncertainty_weight", 0.5)
    
    # Environment
    config.set("environment.type", std::string("gridworld"))
    config.set("environment.width", 30)
    config.set("environment.height", 30)
    config.set("environment.sampling_rate", 0.5)
    
    return config
```

#### Configuration Validation

```python
import pynlm

def validate_configuration(config):
    """Validate configuration for potential issues."""
    
    issues = []
    
    # Check neuron count
    neuron_count = config.getOr<int64_t>("neuron_count", 1000)
    if neuron_count < 10:
        issues.append("Neuron count too low - consider at least 10 neurons")
    elif neuron_count > 100000:
        issues.append("Neuron count very high - may cause performance issues")
    
    # Check connection probability
    conn_prob = config.getOr<float>("connection_probability", 0.1)
    if conn_prob < 0.001:
        issues.append("Connection probability very low - may result in isolated neurons")
    elif conn_prob > 0.5:
        issues.append("Connection probability very high - may cause over-connectivity")
    
    # Check timestep
    timestep = config.getOr<double>("simulation_timestep", 0.001)
    if timestep <= 0:
        issues.append("Simulation timestep must be positive")
    elif timestep > 0.1:
        issues.append("Simulation timestep large - may cause numerical instability")
    
    # Check plasticity parameters
    stdp_rate = config.getOr<float>("plasticity.stdp.learning_rate", 0.001)
    if stdp_rate > 0.1:
        issues.append("STDP learning rate very high - may cause weight explosion")
    
    # Check development rates
    synaptogenesis = config.getOr<float>("development.synaptogenesis_rate", 0.001)
    pruning = config.getOr<float>("development.pruning_rate", 0.0001)
    
    if synaptogenesis > pruning * 10:
        issues.append("Synaptogenesis rate much higher than pruning rate - may cause uncontrolled growth")
    
    # Check neuromodulation baseline
    dopamine_baseline = config.getOr<float>("neuromod.dopamine.baseline", 0.0)
    if dopamine_baseline > 1.0:
        issues.append("Dopamine baseline too high - may cause over-modulation")
    
    return issues

def safe_config_from_preset(preset_name):
    """Create a configuration with validation."""
    
    config = pynlm.createDefaultConfig()
    
    # Apply preset
    if preset_name == "minimal":
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
    elif preset_name == "research":
        config.set("neuron_count", 5000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.05)
    elif preset_name == "development":
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.15)
    elif preset_name == "quicktest":
        config.set("neuron_count", 50)
        config.set("region_count", 1)
        config.set("connection_probability", 0.2)
    
    # Validate and warn
    issues = validate_configuration(config)
    if issues:
        print(f"Configuration warnings for preset '{preset_name}':")
        for issue in issues:
            print(f"  - {issue}")
    
    return config
```

### Best Practices

#### Code Organization

```python
import pynlm

# Organize code into reusable components
def create_simulation_framework(config):
    """Create a reusable simulation framework."""
    
    # Shared components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Initialize world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable subsystems based on configuration
    if config.has("neuromod.curiosity.enable"):
        agent.enableCuriosity(config.getOr<bool>("neuromod.curiosity.enable", True))
    
    if config.has("neuromod.reward_modulation.enable"):
        agent.enableRewardModulation(config.getOr<bool>("neuromod.reward_modulation.enable", True))
    
    return brain, agent, world

def run_reproducible_experiment(config, num_runs=3, steps_per_run=500):
    """Run experiments with reproducible seeds."""
    
    results = []
    
    for run in range(num_runs):
        # Use different random seed for each run
        seed = 42 + run * 1000
        config.set("random_seed", static_cast<int64_t>(seed), nlm.ConfigSource::CommandLine)
        
        # Create fresh simulation
        brain, agent, world = create_simulation_framework(config)
        
        # Run simulation
        for step in range(steps_per_run):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            if step % 100 == 0:
                agent.applyRewardModulation(0.1, 0.0)
                agent.updateDevelopment(0.1)
        
        # Collect results
        result = {
            'run': run,
            'seed': seed,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        }
        
        results.append(result)
        print(f"Run {run} completed with seed {seed}")
        
        # Save brain state for analysis
        brain.save(f"experiment_run_{run}_seed_{seed}.bin")
    
    return results

# Usage example
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDevelopmentConfig()
    
    # Run reproducible experiments
    results = run_reproducible_experiment(config, num_runs=3, steps_per_run=500)
    
    # Analyze results
    print("\n=== Experiment Results Summary ===")
    for result in results:
        print(f"Run {result['run']} (seed {result['seed']}): "
              f"Firing rate: {result['final_firing_rate']:.3f}, "
              f"Spikes: {result['total_spikes']}, "
              f"Curiosity: {result['curiosity']:.3f}")
```

### Error Handling and Recovery

```python
import pynlm
import traceback

def safe_simulation_run(brain, world, agent, num_steps):
    """Run simulation with error handling and recovery."""
    
    try:
        for step in range(num_steps):
            try:
                # Update world
                world.update(0.1)
                
                # Process sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Decode action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Try to recover
                brain.reset()
                brain.initialize()
                agent.reset()
                world.reset()
                
                # Continue simulation
                continue
        
        print("Simulation completed successfully!")
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        traceback.print_exc()
        return False

def checkpoint_and_resume(config, initial_steps=1000, checkpoint_interval=500):
    """Run simulation with checkpointing and resume capability."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    checkpoint_file = "simulation_checkpoint.bin"
    
    try:
        # Try to load checkpoint
        if os.path.exists(checkpoint_file):
            print("Loading checkpoint...")
            brain.load(checkpoint_file)
        
        # Run simulation
        for step in range(initial_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
            
            # Save checkpoint periodically
            if step > 0 and step % checkpoint_interval == 0:
                print(f"Saving checkpoint at step {step}")
                brain.save(checkpoint_file)
                print(f"Checkpoint saved to {checkpoint_file}")
        
        print("Simulation completed!")
        
    except KeyboardInterrupt:
        print("\nSimulation interrupted. Saving checkpoint...")
        brain.save(checkpoint_file)
        print(f"Checkpoint saved to {checkpoint_file}")
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        traceback.print_exc()
        
        # Try to resume from checkpoint
        if os.path.exists(checkpoint_file):
            print("Attempting to resume from checkpoint...")
            brain.load(checkpoint_file)
            print("Successfully resumed from checkpoint")
        else:
            print("No checkpoint available for resumption")
    
    return brain, agent, world
```

### Performance Optimization Tips

```python
import pynlm
import time
import os

def optimize_configuration(config, num_steps=1000):
    """Optimize configuration for better performance."""
    
    optimizations = []
    
    # Adjust neuron count based on available memory
    neuron_count = config.getOr<int64_t>("neuron_count", 1000)
    
    if neuron_count > 5000:
        config.set("neuron_count", 2000)
        optimizations.append(f"Reduced neuron count from {neuron_count} to 2000")
    
    # Adjust region count for better parallelization
    region_count = config.getOr<int64_t>("region_count", 1)
    if region_count > 4:
        config.set("region_count", 2)
        optimizations.append(f"Reduced region count from {region_count} to 2")
    
    # Adjust timestep for stability
    timestep = config.getOr<double>("simulation_timestep", 0.001)
    if timestep < 0.0001:
        config.set("simulation_timestep", 0.0005)
        optimizations.append(f"Increased timestep from {timestep} to 0.0005 for stability")
    elif timestep > 0.01:
        config.set("simulation_timestep", 0.001)
        optimizations.append(f"Decreased timestep from {timestep} to 0.001 for stability")
    
    # Check memory usage
    import psutil
    process = psutil.Process(os.getpid())
    memory_mb = process.memory_info().rss / 1024 / 1024
    
    if memory_mb > 2000:  # More than 2GB
        config.set("neuron_count", config.getOr<int64_t>("neuron_count", 1000) // 2)
        optimizations.append(f"Reduced memory usage: {memory_mb:.0f}MB -> estimated lower")
    
    if optimizations:
        print("Configuration optimizations applied:")
        for opt in optimizations:
            print(f"  - {opt}")
    
    return config

def benchmark_configuration(config, num_steps=500):
    """Benchmark configuration performance."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Warm-up run
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
    
    # Benchmark run
    start_time = time.time()
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    steps_per_second = num_steps / total_time
    avg_step_time = total_time / num_steps
    
    print(f"Benchmark Results:")
    print(f"  Total steps: {num_steps}")
    print(f"  Total time: {total_time:.2f}s")
    print(f"  Steps per second: {steps_per_second:.2f}")
    print(f"  Average step time: {avg_step_time:.4f}s")
    
    return steps_per_second, avg_step_time
```
