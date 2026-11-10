# NLM Configuration Examples and Usage Guide

This file complements `easy_usage.md` and provides comprehensive examples for using the NLM Python interface.

## Configuration File Examples

### 1. Basic Configuration
Save this as `basic_config.json`:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "enable_development": false,
    "enable_curiosity": false,
    "enable_reward_modulation": false,
    "enable_structural_plasticity": false
}
```

### 2. Learning Configuration
Save this as `learning_config.json`:

```json
{
    "random_seed": 123,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.02,
    "stdp_tau": 25.0,
    "enable_development": true,
    "enable_curiosity": true,
    "enable_reward_modulation": true,
    "enable_structural_plasticity": true,
    "development_critical_period": 100,
    "development_maturation_rate": 0.01,
    "curiosity_threshold": 0.5,
    "reward_scale": 1.0
}
```

### 3. High-Performance Configuration
Save this as `performance_config.json`:

```json
{
    "random_seed": 999,
    "simulation_timestep": 0.0005,
    "neuron_count": 2000,
    "region_count": 4,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.01,
    "stdp_tau": 15.0,
    "enable_development": true,
    "enable_curiosity": false,
    "enable_reward_modulation": true,
    "enable_structural_plasticity": false,
    "simd_enabled": true,
    "parallel_processing": true,
    "memory_pool_size": 1048576
}
```

## Python Configuration Examples

### 1. Creating Advanced Configurations Programmatically

```python
import pynlm

# Create basic configuration
config1 = pynlm.createDefaultConfig()
config1.loadFromFile("basic_config.json")
brain1 = pynlm.createBrain(config1)

# Create advanced configuration using helper function
config2 = pynlm.createAdvancedConfig(
    neuronCount=1000,
    connectionProb=0.2,
    stdpLTP=0.03,
    stdpLTD=0.02,
    enableDevelopment=True,
    enableCuriosity=True,
    enableRewardModulation=True
)
brain2 = pynlm.createBrain(config2)

# Create visualization configuration
viz_config = pynlm.createVisualizationConfig(
    width=800,
    height=600,
    showNeurons=True,
    showSynapses=True,
    showActivity=True
)
```

### 2. Loading and Saving Configurations

```python
import pynlm
import json
import os

# Save current configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
config = brain.getConfig()
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Modify configuration programmatically
new_config.set("neuron_count", 2000)
new_config.set("enable_curiosity", True)
new_config.saveToFile("custom_config.json")

# Get configuration summary
print(config.summary())
```

## Advanced Usage Examples

### 1. Complete Simulation Pipeline

```python
import pynlm
import time
import json
import os

def run_complete_simulation(config_file="learning_config.json",
                           duration_steps=1000,
                           save_interval=100,
                           output_dir="simulation_results"):
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    # Load configuration
    config = pynlm.createDefaultConfig()
    if os.path.exists(config_file):
        config.loadFromFile(config_file)
    
    # Create simulation components
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.95)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Track simulation data
    simulation_data = {
        'config': config_file,
        'duration': duration_steps,
        'start_time': time.time(),
        'steps': [],
        'performance_metrics': []
    }
    
    print(f"Starting simulation with {duration_steps} steps...")
    
    for step in range(duration_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if available
        if hasattr(result, 'reward'):
            reward = result.reward
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
        
        # Record step data
        simulation_data['steps'].append({
            'step': step,
            'time': world.getSimulationTime(),
            'spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'reward': reward if 'reward' in locals() else 0.0,
            'action': action.getType() if hasattr(action, 'getType') else None
        })
        
        # Save checkpoint periodically
        if step % save_interval == 0 and step > 0:
            checkpoint_file = f"{output_dir}/checkpoint_step_{step}.bin"
            brain.save(checkpoint_file)
            print(f"Checkpoint saved: {checkpoint_file}")
    
    # Analyze and save results
    simulation_data['end_time'] = time.time()
    simulation_data['total_duration'] = simulation_data['end_time'] - simulation_data['start_time']
    simulation_data['performance_metrics'] = pynlm.analyzeBrainActivity(brain)
    
    # Export data
    pynlm.exportBrainData(brain, f"{output_dir}/brain_data.csv")
    
    # Save simulation data
    with open(f"{output_dir}/simulation_data.json", 'w') as f:
        json.dump(simulation_data, f, indent=2)
    
    # Save brain snapshot
    pynlm.saveBrainSnapshot(brain, f"{output_dir}/final_brain.bin", 
                           f"Simulation completed after {duration_steps} steps")
    
    print(f"\n✓ Simulation completed successfully!")
    print(f"  Total duration: {simulation_data['total_duration']:.2f} seconds")
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    print(f"  Results saved to: {output_dir}/")
    
    return simulation_data

# Example usage
if __name__ == "__main__":
    results = run_complete_simulation(
        config_file="learning_config.json",
        duration_steps=500,
        save_interval=100,
        output_dir="experiment_1"
    )
```

### 2. Training Multiple Sessions

```python
import pynlm
import os
import json

def create_training_config(base_config_file="learning_config.json"):
    """Create configuration for training"""
    config = pynlm.createDefaultConfig()
    
    if os.path.exists(base_config_file):
        config.loadFromFile(base_config_file)
    
    # Modify for training
    config.set("enable_curiosity", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_development", True)
    
    return config

def run_training_session(session_id, config, world, max_steps=200):
    """Run a single training episode"""
    # Create fresh brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"\nTraining Session {session_id}: Starting with {brain.getTotalNeuronCount()} neurons")
    
    session_data = {
        'session_id': session_id,
        'steps': [],
        'final_brain_stats': {},
        'performance_metrics': {}
    }
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning if reward available
        if hasattr(result, 'reward'):
            reward = result.reward
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
        
        # Record data
        session_data['steps'].append({
            'step': step,
            'spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'reward': reward if 'reward' in locals() else 0.0
        })
        
        # End session if brain is too damaged
        if brain.getFiringNeuronCount() == 0:
            print(f"Session {session_id} ended early at step {step}")
            break
    
    # Record final stats
    session_data['final_brain_stats'] = pynlm.analyzeBrainActivity(brain)
    session_data['performance_metrics'] = {
        'total_steps': len(session_data['steps']),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate()
    }
    
    # Save brain snapshot
    snapshot_file = f"training_session_{session_id}_brain.bin"
    pynlm.saveBrainSnapshot(brain, snapshot_file, f"Training session {session_id}")
    
    print(f"Session {session_id} completed: {len(session_data['steps'])} steps, "
          f"{brain.getTotalSpikeCount()} spikes")
    
    return session_data, brain

def run_training_experiment(num_sessions=5, steps_per_session=100):
    """Run complete training experiment"""
    print("=== NLM Training Experiment ===")
    
    # Create training environment
    config = create_training_config()
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=6, visionHeight=6)
    world.reset()
    
    all_sessions = []
    
    for session_id in range(num_sessions):
        session_data, brain = run_training_session(
            session_id, config, world, steps_per_session
        )
        all_sessions.append(session_data)
        
        # Brief pause between sessions
        if session_id < num_sessions - 1:
            time.sleep(0.5)
    
    # Analyze experiment results
    avg_final_firing = sum(s['final_brain_stats']['avg_firing_rate'] 
                          for s in all_sessions) / len(all_sessions)
    total_experiment_spikes = sum(s['final_brain_stats']['total_spikes'] 
                                  for s in all_sessions)
    
    print(f"\n=== Training Experiment Complete ===")
    print(f"Sessions completed: {num_sessions}")
    print(f"Average final firing rate: {avg_final_firing:.2f}")
    print(f"Total spikes across all sessions: {total_experiment_spikes}")
    
    # Save experiment results
    experiment_results = {
        'num_sessions': num_sessions,
        'steps_per_session': steps_per_session,
        'sessions': all_sessions,
        'summary': {
            'avg_final_firing_rate': avg_final_firing,
            'total_experiment_spikes': total_experiment_spikes,
            'config_used': config_file
        }
    }
    
    with open(f"training_experiment_{num_sessions}_sessions.json", 'w') as f:
        json.dump(experiment_results, f, indent=2)
    
    print(f"Experiment results saved to training_experiment_{num_sessions}_sessions.json")
    
    return all_sessions

# Example usage
if __name__ == "__main__":
    training_sessions = run_training_experiment(
        num_sessions=3,
        steps_per_session=100
    )
```

### 3. Benchmarking Different Configurations

```python
import pynlm
import time

def benchmark_configuration(config_name, config, test_steps=1000):
    """Benchmark a specific configuration"""
    print(f"\nBenchmarking configuration: {config_name}")
    
    # Create brain with this configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warmup
    for step in range(100):
        brain.step(step)
    
    # Benchmark
    start_time = time.time()
    for step in range(100, 100 + test_steps):
        brain.step(step)
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Collect metrics
    metrics = {
        'config_name': config_name,
        'total_time_ms': duration * 1000,
        'avg_time_per_step_ms': (duration * 1000) / test_steps,
        'spikes_per_second': brain.getTotalSpikeCount() / duration,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'neurons_firing': brain.getFiringNeuronCount()
    }
    
    print(f"  Time per step: {metrics['avg_time_per_step_ms']:.2f} ms")
    print(f"  Spikes per second: {metrics['spikes_per_second']:.0f}")
    print(f"  Final firing rate: {metrics['final_firing_rate']:.2f}")
    
    return metrics

def compare_configurations():
    """Compare multiple configurations"""
    print("=== Configuration Comparison ===")
    
    # Define configurations to compare
    configs_to_test = {
        'basic': pynlm.createAdvancedConfig(
            neuronCount=500,
            connectionProb=0.15,
            stdpLTP=0.02,
            stdpLTD=0.015,
            enableDevelopment=False,
            enableCuriosity=False,
            enableRewardModulation=False
        ),
        'learning': pynlm.createAdvancedConfig(
            neuronCount=1000,
            connectionProb=0.2,
            stdpLTP=0.03,
            stdpLTD=0.02,
            enableDevelopment=True,
            enableCuriosity=True,
            enableRewardModulation=True
        ),
        'performance': pynlm.createAdvancedConfig(
            neuronCount=2000,
            connectionProb=0.1,
            stdpLTP=0.01,
            stdpLTD=0.01,
            enableDevelopment=True,
            enableCuriosity=False,
            enableRewardModulation=True
        )
    }
    
    # Benchmark each configuration
    results = []
    for config_name, config in configs_to_test.items():
        metrics = benchmark_configuration(config_name, config, test_steps=500)
        results.append(metrics)
    
    # Compare results
    print(f"\n=== Configuration Comparison Results ===")
    for metrics in results:
        print(f"\n{metrics['config_name'].upper()}:")
        print(f"  Performance: {metrics['avg_time_per_step_ms']:.2f} ms/step")
        print(f"  Learning: {metrics['final_firing_rate']:.2f} avg firing rate")
        print(f"  Efficiency: {metrics['spikes_per_second']:.0f} spikes/sec")
    
    # Determine best configuration
    best_performance = min(results, key=lambda x: x['avg_time_per_step_ms'])
    best_learning = max(results, key=lambda x: x['final_firing_rate'])
    
    print(f"\nBest Performance: {best_performance['config_name']}")
    print(f"Best Learning: {best_learning['config_name']}")
    
    return results

# Example usage
if __name__ == "__main__":
    comparison_results = compare_configurations()
```

## Troubleshooting Guide

### Common Issues and Solutions

1. **Brain Not Firing Neurons**
   - Check if `brain.initialize()` has been called
   - Ensure `brain.step()` is being called
   - Verify that sensory input is being processed
   - Check configuration parameters (neuron count, connection probability)

2. **Agent Not Taking Actions**
   - Ensure agent is initialized with world: `agent.initialize(world)`
   - Check if brain is producing motor commands
   - Verify world is receiving motor commands
   - Check agent configuration (reward modulation, curiosity)

3. **Simulation Performance Issues**
   - Reduce neuron count for faster testing
   - Decrease simulation steps
   - Disable CPU-intensive features (curiosity, development)
   - Use performance configuration

4. **File I/O Errors**
   - Check file permissions
   - Ensure output directories exist
   - Handle exceptions properly

5. **Configuration Loading Issues**
   - Verify JSON format is correct
   - Check that all required keys are present
   - Use default configuration if loading fails

### Example Error Handling

```python
import pynlm
import traceback

def safe_simulation_run():
    try:
        # Create components
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        traceback.print_exc()
        return False

# Use safe simulation
success = safe_simulation_run()
if success:
    print("✓ Simulation completed successfully")
else:
    print("✗ Simulation failed")
```

## Advanced Features Summary

### New Python API Features Added:

1. **Configuration Helpers**:
   - `createAdvancedConfig()` - Create configurations with preset parameters
   - `createVisualizationConfig()` - Create visualization configurations

2. **Simulation Utilities**:
   - `runCompleteSimulation()` - Run brain-world simulation loops
   - `createTrainingSimulation()` - Setup training environments

3. **Data Management**:
   - `saveBrainSnapshot()` - Save brain state with metadata
   - `loadBrainSnapshot()` - Load brain state
   - `exportBrainData()` - Export brain data to files

4. **Performance Monitoring**:
   - `benchmarkBrain()` - Benchmark brain performance
   - `analyzeBrainActivity()` - Analyze brain activity statistics

5. **Visualization Support**:
   - `createBrainPlotData()` - Create plotting data for visualization

### File Downloads:

- Configuration examples: `configs/nlm_configs.ini`
- Basic usage examples: `easy_usage.md`
- Advanced usage examples: `HOW_TO_USE.md`

## Quick Start for New Users

1. **Install and Setup**:
   ```bash
   pip install pynlm
   ```

2. **Basic Usage**:
   ```python
   import pynlm
   
   # Create brain
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   brain.initialize()
   
   # Run a few steps
   for step in range(10):
       brain.step(step)
   
   print("Brain has", brain.getTotalNeuronCount(), "neurons")
   ```

3. **Complete Example**:
   ```python
   import pynlm
   
   # Create simulation components
   config = pynlm.createAdvancedConfig(
       neuronCount=1000,
       enableCuriosity=True,
       enableRewardModulation=True
   )
   
   brain = pynlm.createBrain(config)
   world = pynlm.createSimpleWorld()
   agent = pynlm.createAgentBrain(brain)
   
   world.configure(20, 20, 8, 8)
   world.reset()
   agent.initialize(world)
   
   # Run simulation
   for step in range(100):
       world.update(0.1)
       agent.processSensoryInput(world.getSensoryPercept())
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())
   
   print("✓ Simulation complete!")
   ```

## Getting Help

- **Documentation**: Check `easy_usage.md` and `HOW_TO_USE.md`
- **Configuration Examples**: See `configs/nlm_configs.ini`
- **Source Code**: Review the Python bindings in `python/bindings.cpp`
- **Tests**: Look at existing test files in `tests/`

## Next Steps

1. **Experiment with different configurations**
2. **Use the training examples for learning simulations**
3. **Benchmark different setups for performance optimization**
4. **Extend the Python API with custom functions**
5. **Contribute to the project and add new features**

This comprehensive guide should help you get the most out of the NLM Python interface and explore all the advanced features available!