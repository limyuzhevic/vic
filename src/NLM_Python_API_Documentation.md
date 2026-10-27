# NLM (Neural Learning Machine) - Enhanced Python API Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project for creating virtual brains that can learn, remember things, and make decisions. The system implements real spiking neural computation using Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics, STDP and Hebbian plasticity, and multiple neuromodulation systems.

## Quick Start Guide

### Basic Usage Pattern

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

## Enhanced Command-Line Interface

### New Features in Phase 2

The NLM executable now supports comprehensive command-line configuration:

#### Basic Options:
```bash
nlm --help                    # Show help message
nlm --verbose                 # Enable debug logging
nlm --benchmark               # Run performance benchmark
```

#### Test Configuration:
```bash
nlm --no-basic-test           # Skip basic connectivity test
nlm --no-plasticity-test      # Skip plasticity learning experiment
nlm --no-stdp-test            # Skip STDP verification
```

#### Simulation Parameters:
```bash
nlm --steps 5000              # Run 5000 simulation steps
nlm --neurons 2000            # Use 2000 neurons (more complex brain)
nlm --connection-prob 0.05    # Lower connection density
nlm --plasticity 2.0          # Increase plasticity strength
nlm --novelty-threshold 0.05   # Adjust novelty detection sensitivity
```

#### Learning Configuration:
```bash
nlm --no-curiosity            # Disable curiosity-driven exploration
nlm --no-development          # Disable developmental processes
nlm --stdp-ltp 0.03           # Increase STDP LTP weight
nlm --stdp-ltd 0.025          # Increase STDP LTD weight
```

#### I/O Options:
```bash
nlm --config my_config.cfg    # Use custom configuration file
nlm --output results.txt      # Save detailed results to file
nlm --save-brain              # Save brain state after execution
nlm --seed 12345              # Set random seed for reproducibility
```

### Example Command Lines:

```bash
# Simple test with custom parameters
nlm --verbose --steps 3000 --plasticity 1.5

# Focused learning experiment
nlm --no-basic-test --plasticity 2.0 --stdp-ltp 0.05

# Full benchmark with saving
nlm --benchmark --output benchmark_results.csv --save-brain

# Reproducible experiment
nlm --seed 42 --steps 1000 --connection-prob 0.1 --output experiment1.txt
```

## Advanced Usage Patterns

### 1. Complete Agent Simulation

```python
import pynlm
import time

def run_advanced_agent_simulation(duration=1000):
    """Advanced agent simulation with all subsystems enabled."""
    
    # 1. Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent and world interface
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    agent.initialize(world)
    
    # 4. Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 5. Enable brain subsystems
    brain.enablePredictionSystem(True)
    brain.enableWorkingMemory(True)
    brain.enableEpisodicMemory(True)
    brain.enablePlanning(True)
    brain.enableAttention(True)
    
    # 6. Run simulation with detailed monitoring
    start_time = time.time()
    metrics = {
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'developmental_stages': [],
        'rewards': [],
        'predictions': []
    }
    
    for step in range(duration):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted_reward = 0.0  # Could integrate with prediction system
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics['firing_rates'].append(brain.getFiringNeuronCount())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        metrics['developmental_stages'].append(brain.getDevelopmentalStage())
        metrics['rewards'].append(reward)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Reward: {reward:.3f}")
    
    elapsed_time = time.time() - start_time
    print(f"\nAdvanced simulation completed in {elapsed_time:.2f} seconds")
    print(f"Average firing rate: {sum(metrics['firing_rates'])/len(metrics['firing_rates']):.1f} neurons")
    print(f"Total reward: {sum(metrics['rewards']):.3f}")
    
    return metrics
```

### 2. Configuration Customization

```python
import pynlm

def create_custom_config():
    """Create a custom configuration for specific use cases."""
    
    # Start with default config
    config = pynlm.createDefaultConfig()
    
    # Modify for different brain types
    
    # Fast, small brain for quick testing
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    config.set("simulation.timestep", 0.001)
    
    # Complex, slow brain for detailed learning
    # config.set("brain.neuron_count", 5000)
    # config.set("brain.synapse_density", 0.2)
    # config.set("simulation.timestep", 0.0005)
    # config.set("plasticity.stdp.learning_rate", 0.002)
    
    # Development-focused brain
    config.set("development.enabled", True)
    config.set("development.critical_period", True)
    config.set("plasticity.structural.enable", True)
    
    # Exploration-focused brain
    config.set("neuromodulation.curiosity.enabled", True)
    config.set("neuromodulation.novelty.enabled", True)
    config.set("neuromodulation.dopamine.sensitivity", 1.5)
    
    return config

# Usage
config = create_custom_config()
brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. Batch Processing and Experimentation

```python
import pynlm
import os
import json

def run_parameter_sweep(param_sets, output_dir="experiments"):
    """Run multiple experiments with different parameter combinations."""
    
    os.makedirs(output_dir, exist_ok=True)
    results = []
    
    for i, params in enumerate(param_sets):
        print(f"Running experiment {i+1}/{len(param_sets)}: {params}")
        
        # Create config
        config = pynlm.createDefaultConfig()
        for key, value in params.items():
            config.set(key, value)
        
        # Create brain and run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(params.get('steps', 1000)):
            brain.step(step)
        
        # Collect results
        result = {
            'parameters': params,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio()
        }
        
        results.append(result)
        
        # Save to file
        with open(f"{output_dir}/experiment_{i:03d}.json", 'w') as f:
            json.dump(result, f, indent=2)
    
    # Save summary
    summary = {
        'total_experiments': len(param_sets),
        'results': results
    }
    
    with open(f"{output_dir}/summary.json", 'w') as f:
        json.dump(summary, f, indent=2)
    
    return results

# Example parameter sweep
param_sets = [
    {'brain.neuron_count': 500, 'plasticity.stdp.learning_rate': 0.001},
    {'brain.neuron_count': 1000, 'plasticity.stdp.learning_rate': 0.001},
    {'brain.neuron_count': 500, 'plasticity.stdp.learning_rate': 0.005},
    {'brain.neuron_count': 1000, 'plasticity.stdp.learning_rate': 0.005},
]

# Run experiment
results = run_parameter_sweep(param_sets)
```

## Performance Optimization Guide

### 1. Configuration for Performance

```python
import pynlm

def create_performance_config():
    """Create configuration optimized for performance."""
    config = pynlm.createDefaultConfig()
    
    # Performance optimizations
    config.set("performance.optimization_level", 3)
    config.set("performance.use_memory_pools", True)
    config.set("performance.use_event_queues", True)
    config.set("performance.enable_simd", True)
    config.set("performance.parallel_threads", 4)
    
    # Memory management
    config.set("memory.enable_allocation_pools", True)
    config.set("memory.pool_size", 1024 * 1024)  # 1MB per pool
    
    return config
```

### 2. Benchmarking Utilities

```python
import pynlm
import time
import statistics

def benchmark_brain(config, num_runs=5, duration=1000):
    """Benchmark brain performance with multiple runs."""
    
    times = []
    firing_rates = []
    
    for run in range(num_runs):
        print(f"Benchmark run {run+1}/{num_runs}")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(duration):
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        times.append(elapsed)
        firing_rates.append(brain.getFiringNeuronCount())
        
        print(f"  Time: {elapsed:.2f}s, Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Calculate statistics
    avg_time = statistics.mean(times)
    std_time = statistics.stdev(times) if len(times) > 1 else 0.0
    avg_firing = statistics.mean(firing_rates)
    
    print(f"\nBenchmark Results (averaged over {num_runs} runs):")
    print(f"Average time: {avg_time:.2f}s ± {std_time:.2f}s")
    print(f"Average firing neurons: {avg_firing:.1f}")
    print(f"Steps per second: {duration/avg_time:.1f} Hz")
    
    return {
        'average_time': avg_time,
        'time_std': std_time,
        'average_firing': avg_firing,
        'steps_per_second': duration/avg_time
    }
```

### 3. Memory Usage Monitoring

```python
import pynlm
import tracemalloc

def monitor_memory_usage(config, duration=1000):
    """Monitor memory usage during simulation."""
    
    # Start memory tracing
    tracemalloc.start()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with periodic memory checks
    for step in range(duration):
        brain.step(step)
        
        if step % 100 == 0:
            current, peak = tracemalloc.get_traced_memory()
            print(f"Step {step}: Current memory = {current / 1024 / 1024:.2f} MB, "
                  f"Peak memory = {peak / 1024 / 1024:.2f} MB")
    
    # Final memory stats
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    
    print(f"\nMemory Usage Summary:")
    print(f"Final memory usage: {current / 1024 / 1024:.2f} MB")
    print(f"Peak memory usage: {peak / 1024 / 1024:.2f} MB")
    
    return {
        'final_memory_mb': current / 1024 / 1024,
        'peak_memory_mb': peak / 1024 / 1024
    }
```

## Debugging and Development Tools

### 1. Brain State Inspection

```python
import pynlm

def inspect_brain_state(brain, step_description=""):
    """Inspect current brain state for debugging."""
    
    print(f"\n=== Brain State Inspection {step_description} ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Check subsystems
    if brain.getPredictionSystem():
        print("Prediction system: ENABLED")
    else:
        print("Prediction system: DISABLED")
    
    if brain.getWorkingMemory():
        print("Working memory: ENABLED")
    else:
        print("Working memory: DISABLED")
    
    if brain.getEpisodicMemory():
        print("Episodic memory: ENABLED")
    else:
        print("Episodic memory: DISABLED")
    
    # Check neuromodulation
    if brain.getDopamine():
        dopamine = brain.getDopamine()
        print(f"Dopamine level: {dopamine.getCurrentLevel():.3f}")
    
    if brain.getCuriosity():
        curiosity = brain.getCuriosity()
        print(f"Curiosity level: {curiosity.getCurrentLevel():.3f}")
    
    if brain.getNovelty():
        novelty = brain.getNovelty()
        print(f"Novelty level: {novelty.getCurrentLevel():.3f}")
```

### 2. Step-by-Step Debugging

```python
import pynlm

def debug_simulation(config, world, agent, max_steps=100):
    """Debug simulation step by step with interactive inspection."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent.initialize(world)
    
    print("Starting step-by-step debug mode")
    print("Enter 'q' to quit, 'n' for next step, 'p' to pause")
    
    for step in range(max_steps):
        print(f"\n--- Step {step} ---")
        
        # Update world
        world.update(timestep=0.1)
        percept = world.getSensoryPercept()
        
        # Get sensor readings
        print(f"Vision: {percept.getVision()[:5]}... (showing first 5 values)")
        print(f"Touch sensors: {percept.getTouch()}")
        print(f"Internal signals: {percept.getInternal()}")
        
        # Process input
        agent.processSensoryInput(percept)
        print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"Prediction error: {agent.getPredictionError():.3f}")
        
        # Run brain step
        brain.step(step)
        print(f"Brain stats: {brain.getFiringNeuronCount()} firing neurons")
        
        # Get action
        action = agent.decodeMotorCommand()
        print(f"Selected action: {action}")
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        print(f"Developmental stage: {agent.getDevelopmentalStage()}")
        
        # Check for debug commands
        cmd = input("Debug command (q/n/p): ").strip().lower()
        if cmd == 'q':
            break
        elif cmd == 'p':
            input("Paused. Press Enter to continue...")
        
        # Save state periodically
        if step % 10 == 0:
            brain.save(f"debug_state_step_{step}.bin")
    
    print("Debug simulation completed")
```

## Troubleshooting Guide

### Common Issues and Solutions

**Issue: Brain doesn't initialize**
- **Solution**: Always call `brain.initialize()` before `brain.step()`
- **Additional info**: Check configuration parameters are valid

**Issue: Agent isn't moving**
- **Solution**: Ensure `world.update()` is called before `agent.decodeMotorCommand()`
- **Additional info**: Check that `agent.initialize(world)` was called

**Issue: Low firing rates**
- **Solution**: Increase simulation steps for "warm-up" period
- **Additional info**: Ensure sensory input is connected to neurons

**Issue: Memory usage too high**
- **Solution**: Use configuration to enable memory pools
- **Additional info**: Check for memory leaks in custom configurations

**Issue: Experiments not reproducible**
- **Solution**: Set random seed using `--seed` or `config.set("random_seed", value)`
- **Additional info**: Use consistent timestep across all simulations

### Performance Tips

1. **For faster simulations**: Use `--neurons 500 --steps 1000`
2. **For learning experiments**: Use `--plasticity 2.0 --no-basic-test`
3. **For debugging**: Use `--verbose` and run with fewer steps
4. **For production**: Use `--benchmark --save-brain`

## Integration Examples

### 1. Multi-Agent System

```python
import pynlm

def create_multi_agent_system(num_agents=3, shared_world_size=(50, 50)):
    """Create a system with multiple agents sharing a world."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=shared_world_size[0], 
                   height=shared_world_size[1],
                   visionWidth=8, visionHeight=8)
    world.reset()
    
    agents = []
    brains = []
    
    # Create multiple agents with shared world
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("random_seed", 42 + i)  # Different seeds
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Position agents at different starting points
        start_x = shared_world_size[0] * (i + 1) / (num_agents + 1)
        start_y = shared_world_size[1] * (i + 1) / (num_agents + 1)
        world.setAgentStart(start_x, start_y)
        
        agents.append(agent)
        brains.append(brain)
    
    return world, agents, brains
```

### 2. Real-Time Control Interface

```python
import pynlm
import threading
import time

def real_time_controller(world, agent, brain, control_callback):
    """Real-time control interface for interactive experiments."""
    
    def control_loop():
        while True:
            # Get current state
            percept = world.getSensoryPercept()
            state = {
                'vision': percept.getVision(),
                'touch': percept.getTouch(),
                'internal': percept.getInternal(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dev_stage': agent.getDevelopmentalStage()
            }
            
            # Apply control callback
            control_command = control_callback(state)
            
            # Apply to world
            if control_command:
                world.applyMotorCommand(control_command, world.getSimulationTime())
            
            time.sleep(0.1)  # 10 Hz control rate
    
    # Start control thread
    controller_thread = threading.Thread(target=control_loop, daemon=True)
    controller_thread.start()
    
    # Main simulation loop
    for step in range(1000):
        world.update(timestep=0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
        
        if step % 100 == 0:
            print(f"Step {step}: Curiosity {agent.getCuriosityLevel():.3f}, "
                  f"Novelty {agent.getNoveltyLevel():.3f}")
```

## API Reference Summary

### Core Classes

#### Brain Class
- `createBrain(config)` - Create brain with configuration
- `initialize()` - Initialize brain state
- `step(step_number, time=None)` - Run simulation step
- `reset()` - Reset brain to initial state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `getTotalNeuronCount()` - Get total neurons
- `getTotalSynapseCount()` - Get total synapses
- `getFiringNeuronCount()` - Get firing neurons
- `getTotalSpikeCount()` - Get total spikes
- `getAverageFiringRate()` - Get average firing rate

#### Config Class
- `createDefaultConfig()` - Create default configuration
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value
- `has(key)` - Check if configuration has key
- `loadFromFile(filepath)` - Load from file
- `saveToFile(filepath)` - Save to file

#### AgentBrain Class
- `createAgentBrain(brain)` - Create agent interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward
- `updateDevelopment(timestep)` - Update development
- `enableXXX(True/False)` - Enable/disable subsystems

#### SimpleWorld Class
- `createSimpleWorld()` - Create simple world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `update(timestep)` - Update world
- `getSensoryPercept()` - Get sensory percept
- `applyMotorCommand(action, time)` - Apply motor command

### Enumerations

#### NeuronType
- `NeuronType.Excitatory`
- `NeuronType.Inhibitory`
- `NeuronType.Sensory`
- `NeuronType.Motor`
- `NeuronType.Modulatory`
- `NeuronType.Internal`

#### DevelopmentalStage
- `DevelopmentalStage.Initial`
- `DevelopmentalStage.CriticalPeriod`
- `DevelopmentalStage.Maturation`
- `DevelopmentalStage.Adult`
- `DevelopmentalStage.Aging`

#### ActionType
- `ActionType.MoveForward`
- `ActionType.MoveBackward`
- `ActionType.TurnLeft`
- `ActionType.TurnRight`
- `ActionType.Interact`
- `ActionType.LookLeft`
- `ActionType.LookRight`
- `ActionType.Eat`
- `ActionType.Wait`

#### WorldObjectType
- `WorldObjectType.Empty`
- `WorldObjectType.Resource`
- `WorldObjectType.Hazard`
- `WorldObjectType.Wall`
- `WorldObjectType.Marker`

## Best Practices

### 1. Configuration Management
```python
# Best practice: Create config once and reuse
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)

# Don't recreate config for each simulation
```

### 2. Error Handling
```python
import pynlm

def safe_simulation(config, duration=1000):
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(duration):
            brain.step(step)
            
        return brain
    except Exception as e:
        print(f"Simulation failed: {e}")
        return None
```

### 3. Resource Management
```python
import pynlm

def managed_simulation(config, duration=1000):
    brain = None
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(duration):
            brain.step(step)
            
        return brain
    finally:
        # Clean up resources
        if brain:
            brain.save("checkpoint_final.bin")  # Save progress
```

### 4. Reproducibility
```python
import pynlm

def reproducible_experiment(seed=42, steps=1000):
    # Set random seed for reproducibility
    config = pynlm.createDefaultConfig()
    config.set("random_seed", seed)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return brain
```

## Future Enhancements

The following enhancements are planned for future releases:

1. **Advanced Python Bindings**: Enhanced pybind11 interface with more Pythonic APIs
2. **GPU Acceleration**: CUDA support for faster neural computation
3. **Distributed Computing**: Multi-node simulation capabilities
4. **Web Interface**: Real-time visualization and control via web browser
5. **ROS Integration**: Integration with Robot Operating System
6. **Advanced Learning Algorithms**: Reinforcement learning, genetic algorithms
7. **Virtual Reality**: Immersive simulation environments
8. **Cloud Computing**: Scalable simulation on cloud platforms

## Conclusion

NLM provides a comprehensive platform for creating artificial developmental brains. With the enhanced command-line interface, extensive Python API, and comprehensive documentation, researchers and developers can build sophisticated neural systems for experimentation and research.

The system supports everything from simple demonstrations to complex multi-agent simulations, with extensive configuration options and performance optimization features.

For the latest updates, examples, and community support, visit the NLM documentation and forums.