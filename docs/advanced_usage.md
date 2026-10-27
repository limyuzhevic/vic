# NLM Python API - Advanced Usage and Best Practices

This file provides advanced guidance for using the NLM Python API effectively.
It covers optimization strategies, debugging techniques, and advanced usage patterns.

## Advanced API Patterns

### 1. Simulation Control and Timing

```python
import pynlm
import time

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Advanced timing control
start_time = time.time()
simulation_start = world.getSimulationTime()

for step in range(1000):
    # Control timestep based on simulation requirements
    timestep = 0.1 if step % 100 == 0 else 0.01  # Variable timestep
    world.update(timestep)
    
    # Use exact time for precision
    current_time = world.getSimulationTime()
    
    # Process sensory input with time information
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain step with precise timing
    brain.step(step, current_time)
    
    # Adaptive action selection
    action = agent.decodeMotorCommand()
    
    # Apply with timing
    world.applyMotorCommand(action, current_time)
    
    # Log performance
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {elapsed:.2f}s real, {current_time:.2f}s sim, "
              f"Firing={brain.getFiringNeuronCount()}")

elapsed_real = time.time() - start_time
print(f"Simulation completed in {elapsed_real:.2f} seconds")
```

### 2. Learning System Optimization

```python
import pynlm

# Create learning-optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.exploration_rate", 0.3)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Optimize learning system activation
agent.enableRewardModulation(True)
agent.enableCuriosity(True, 0.3)  # Custom curiosity rate
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Learning schedule
learning_phases = [
    {"steps": 200, "curiosity": 0.5, "reward_weight": 1.0},
    {"steps": 300, "curiosity": 0.3, "reward_weight": 0.7},
    {"steps": 500, "curiosity": 0.2, "reward_weight": 0.5}
]

for phase in learning_phases:
    print(f"Learning phase: curiosity={phase['curiosity']}, reward_weight={phase['reward_weight']}")
    
    # Adjust neuromodulation levels
    # Note: This would require API extensions for fine control
    
    for step in range(phase["steps"]):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step, step * 0.1)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
```

### 3. Advanced Error Handling and Validation

```python
import pynlm
import traceback

class NLMSimulationError(Exception):
    """Custom exception for NLM simulation errors"""
    pass

class NLMValidator:
    """Validate NLM configurations and states"""
    
    @staticmethod
    def validate_config(config):
        """Validate configuration settings"""
        errors = []
        warnings = []
        
        # Check required parameters
        required_keys = [
            "brain.neuron_count",
            "brain.synapse_density", 
            "simulation_timestep"
        ]
        
        for key in required_keys:
            if not config.has(key):
                warnings.append(f"Missing config key: {key}")
        
        # Validate neuron count
        neuron_count = config.getOr("brain.neuron_count", 1000)
        if neuron_count <= 0:
            errors.append("brain.neuron_count must be positive")
        elif neuron_count > 10000:
            warnings.append("brain.neuron_count is large, may impact performance")
        
        # Validate synapse density
        density = config.getOr("brain.synapse_density", 0.1)
        if density < 0 or density > 1:
            errors.append("brain.synapse_density must be between 0 and 1")
        
        return errors, warnings
    
    @staticmethod
    def validate_brain_state(brain):
        """Validate brain state"""
        issues = []
        
        if brain.getTotalNeuronCount() == 0:
            issues.append("Brain has no neurons")
        
        # Check for potential instability
        avg_rate = brain.getAverageFiringRate()
        if avg_rate > 100:  # Hz, very high firing rate
            issues.append(f"High firing rate: {avg_rate:.1f} Hz, may indicate instability")
        
        return issues

# Usage example
def safe_simulation(config_path=None):
    """Example of safe simulation with validation"""
    try:
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        if config_path:
            if config.loadFromFile(config_path):
                print(f"Loaded config from {config_path}")
            else:
                print(f"Warning: Could not load config from {config_path}")
        
        # Validate configuration
        validator = NLMValidator()
        config_errors, config_warnings = validator.validate_config(config)
        
        for warning in config_warnings:
            print(f"Warning: {warning}")
        
        for error in config_errors:
            raise NLMSimulationError(f"Configuration error: {error}")
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            raise NLMSimulationError("Brain initialization failed")
        
        # Validate brain state
        brain_issues = validator.validate_brain_state(brain)
        for issue in brain_issues:
            print(f"Warning: {issue}")
        
        print("Simulation setup complete and validated")
        return brain
        
    except NLMSimulationError as e:
        print(f"Simulation setup failed: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        traceback.print_exc()
        return None

# Run safe simulation
brain = safe_simulation()
if brain:
    print("Brain ready for simulation")
```

### 4. Performance Monitoring and Profiling

```python
import pynlm
import time
import statistics

class SimulationProfiler:
    """Profile simulation performance"""
    
    def __init__(self):
        self.step_times = []
        self.firing_rates = []
        self.memory_usage = []
        self.spike_counts = []
    
    def record_step(self, step_num, brain, agent, world):
        """Record metrics for a simulation step"""
        # Record timing
        step_start = time.time()
        
        # Run simulation step (would be actual simulation)
        # This is a placeholder for actual simulation logic
        
        step_time = time.time() - step_start
        self.step_times.append(step_time)
        
        # Record brain metrics
        self.firing_rates.append(brain.getFiringNeuronCount())
        self.spike_counts.append(brain.getTotalSpikeCount())
        
        # Simulate memory usage
        self.memory_usage.append({
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'working_memory': agent.getCuriosityLevel()
        })
    
    def get_statistics(self):
        """Get performance statistics"""
        stats = {}
        
        if self.step_times:
            stats['step_time'] = {
                'mean': statistics.mean(self.step_times),
                'median': statistics.median(self.step_times),
                'min': min(self.step_times),
                'max': max(self.step_times),
                'std': statistics.stdev(self.step_times) if len(self.step_times) > 1 else 0
            }
        
        if self.firing_rates:
            stats['firing_rate'] = {
                'mean': statistics.mean(self.firing_rates),
                'median': statistics.median(self.firing_rates),
                'min': min(self.firing_rates),
                'max': max(self.firing_rates)
            }
        
        return stats
    
    def report(self):
        """Print performance report"""
        stats = self.get_statistics()
        
        print("=" * 60)
        print("SIMULATION PERFORMANCE REPORT")
        print("=" * 60)
        
        if 'step_time' in stats:
            st = stats['step_time']
            print(f"Step Time (seconds):")
            print(f"  Mean: {st['mean']*1000:.2f} ms")
            print(f"  Median: {st['median']*1000:.2f} ms")
            print(f"  Min: {st['min']*1000:.2f} ms")
            print(f"  Max: {st['max']*1000:.2f} ms")
            print(f"  Std Dev: {st['std']*1000:.2f} ms")
        
        if 'firing_rate' in stats:
            fr = stats['firing_rate']
            print(f"\nFiring Rate (neurons):")
            print(f"  Mean: {fr['mean']:.1f}")
            print(f"  Median: {fr['median']:.1f}")
            print(f"  Min: {fr['min']:.1f}")
            print(f"  Max: {fr['max']:.1f}")
        
        total_steps = len(self.step_times)
        print(f"\nTotal Steps: {total_steps}")
        if total_steps > 0:
            total_time = sum(self.step_times)
            print(f"Total Simulation Time: {total_time:.2f} seconds")
            print(f"Steps per Second: {total_steps/total_time:.1f}")

# Usage example
profiler = SimulationProfiler()

# Run simulation while collecting metrics
for step in range(1000):
    # Your simulation code here
    # This is a simplified example
    if step % 10 == 0:
        # Record metrics (placeholder)
        profiler.record_step(step, None, None, None)
    
    if step % 100 == 0:
        print(f"Step {step} completed")

# Generate performance report
profiler.report()
```

### 5. Advanced World Configuration

```python
import pynlm

class WorldDesigner:
    """Design custom world configurations"""
    
    @staticmethod
    def createResourceDenseWorld(width=50, height=50):
        """Create a world with many resources"""
        world = pynlm.createSimpleWorld()
        world.configure(width, height, 12, 12)
        
        # Add resources throughout the world
        for x in range(10, width, 15):
            for y in range(10, height, 15):
                value = 10.0 + (x + y) * 0.1  # Varied resource values
                world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value))
        
        # Add some hazards
        world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Hazard, 20.0))
        world.addObject(pynlm.WorldObject(width-5, height-5, pynlm.WorldObjectType.Hazard, 20.0))
        
        # Add walls
        for x in range(0, width, 20):
            world.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall, 0.0, 1.0))
            world.addObject(pynlm.WorldObject(x, height-1, pynlm.WorldObjectType.Wall, 0.0, 1.0))
        
        for y in range(0, height, 20):
            world.addObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall, 0.0, 1.0))
            world.addObject(pynlm.WorldObject(width-1, y, pynlm.WorldObjectType.Wall, 0.0, 1.0))
        
        return world
    
    @staticmethod
    def createComplexEnvironment(width=60, height=60):
        """Create a complex environment with various objects"""
        world = pynlm.createSimpleWorld()
        world.configure(width, height, 15, 15)
        
        # Create maze-like structure
        for x in range(0, width, 5):
            for y in range(0, height, 5):
                if x % 15 == 0 and y % 15 == 0:
                    # Empty space
                    pass
                elif x % 5 == 0 or y % 5 == 0:
                    # Walls
                    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, 0.0, 0.8))
                else:
                    # Randomly place objects
                    import random
                    rand = random.Random(12345 + x + y)  # Deterministic
                    r = rand.random()
                    if r < 0.3:
                        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 5.0 + rand.random() * 10))
                    elif r < 0.4:
                        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, 5.0 + rand.random() * 10))
        
        return world

# Usage example
print("1. Creating resource-dense world...")
resource_world = WorldDesigner.createResourceDenseWorld(40, 40)

print("2. Creating complex environment...")
complex_world = WorldDesigner.createComplexEnvironment(50, 50)

print("3. World configuration summary:")
print(f"   Resource world: {resource_world.getWidth()} x {resource_world.getHeight()} with vision {resource_world.getWidth()/resource_world.getWidth():.1f}m/pixel")
print(f"   Complex world: {complex_world.getWidth()} x {complex_world.getHeight()} with vision {complex_world.getWidth()/complex_world.getWidth():.1f}m/pixel")
```

### 6. Batch Processing and Experiment Automation

```python
import pynlm
import os
import json
from pathlib import Path

class ExperimentRunner:
    """Run automated experiments with parameter sweeps"""
    
    def __init__(self, output_dir="experiments"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
    
    def run_parameter_sweep(self, base_config, parameters, description):
        """Run parameter sweep experiment"""
        print(f"Running parameter sweep: {description}")
        
        results = []
        param_names = list(parameters.keys())
        
        # Generate all parameter combinations
        import itertools
        param_combinations = itertools.product(*parameters.values())
        
        for i, combination in enumerate(param_combinations):
            param_dict = dict(zip(param_names, combination))
            
            print(f"  Experiment {i+1}: {param_dict}")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Apply base configuration
            if base_config.get("brain.neuron_count"):
                config.set("brain.neuron_count", base_config["brain.neuron_count"])
            
            # Apply parameter-specific settings
            for key, value in param_dict.items():
                if isinstance(value, float):
                    config.set(key, value)
                elif isinstance(value, int):
                    config.set(key, value)
                elif isinstance(value, bool):
                    config.set(key, value)
            
            # Run experiment
            brain = pynlm.createBrain(config)
            if not brain.initialize():
                print(f"    Failed to initialize brain")
                continue
            
            # Simple simulation for demonstration
            total_spikes = 0
            for step in range(100):
                brain.step(step)
                total_spikes = brain.getTotalSpikeCount()
            
            # Record results
            result = {
                "parameters": param_dict,
                "total_spikes": total_spikes,
                "final_firing_rate": brain.getAverageFiringRate(),
                "experiment_id": i,
                "description": description
            }
            
            results.append(result)
            print(f"    Spikes: {total_spikes}, Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        # Save results
        result_file = self.output_dir / f"{description.lower().replace(' ', '_')}_results.json"
        with open(result_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"  Saved {len(results)} results to {result_file}")
        return results
    
    def run_learning_comparison(self, brains_config, num_trials=5):
        """Compare learning performance across different configurations"""
        print(f"Comparing learning performance across {len(brains_config)} configurations")
        
        comparison_results = []
        
        for i, config in enumerate(brains_config):
            print(f"  Configuration {i+1}/{len(brains_config)}")
            
            # Create brain
            brain = pynlm.createBrain(config)
            if not brain.initialize():
                print(f"    Failed to initialize")
                continue
            
            # Run learning experiment
            learning_metrics = []
            for trial in range(num_trials):
                # Create world and agent for this trial
                world = pynlm.createSimpleWorld()
                world.configure(20, 20, 8, 8)
                world.reset()
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
                
                # Run trial
                for step in range(200):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    result = world.applyMotorCommand(action, world.getSimulationTime())
                    agent.applyRewardModulation(result.reward, 0.0)
                    agent.updateDevelopment(0.1)
                
                # Record metrics
                trial_metrics = {
                    "trial": trial,
                    "final_curiosity": agent.getCuriosityLevel(),
                    "final_novelty": agent.getNoveltyLevel(),
                    "final_prediction_error": agent.getPredictionError(),
                    "total_spikes": brain.getTotalSpikeCount(),
                    "average_firing_rate": brain.getAverageFiringRate()
                }
                learning_metrics.append(trial_metrics)
            
            # Calculate summary statistics
            summary = {
                "config_id": i,
                "config_name": config.get("name", f"Config_{i}"),
                "trials": num_trials,
                "mean_curiosity": statistics.mean([m["final_curiosity"] for m in learning_metrics]),
                "mean_novelty": statistics.mean([m["final_novelty"] for m in learning_metrics]),
                "mean_prediction_error": statistics.mean([m["final_prediction_error"] for m in learning_metrics]),
                "mean_total_spikes": statistics.mean([m["total_spikes"] for m in learning_metrics]),
                "mean_firing_rate": statistics.mean([m["average_firing_rate"] for m in learning_metrics]),
                "all_metrics": learning_metrics
            }
            
            comparison_results.append(summary)
            print(f"    Mean curiosity: {summary['mean_curiosity']:.3f}")
            print(f"    Mean novelty: {summary['mean_novelty']:.3f}")
            print(f"    Mean prediction error: {summary['mean_prediction_error']:.3f}")
        
        # Save comparison results
        result_file = self.output_dir / "learning_comparison_results.json"
        with open(result_file, 'w') as f:
            json.dump(comparison_results, f, indent=2)
        
        print(f"  Saved comparison results to {result_file}")
        return comparison_results

# Usage example
experiment_runner = ExperimentRunner()

# Example parameter sweep
base_config = {"brain.neuron_count": 1000}
parameters = {
    "brain.synapse_density": [0.05, 0.1, 0.15, 0.2],
    "plasticity.stdp.learning_rate": [0.0001, 0.001, 0.01]
}

# Run sweep (smaller example for demonstration)
results = experiment_runner.run_parameter_sweep(
    {"brain.neuron_count": 500},  # Smaller config for faster testing
    {"brain.synapse_density": [0.05, 0.1]},
    "Parameter_Sweep_Demo"
)
```

### 7. Integration with External Systems

```python
import pynlm
import threading
import queue
from datetime import datetime

class NLMSimulator:
    """Advanced simulator with external system integration"""
    
    def __init__(self):
        self.simulation_queue = queue.Queue()
        self.result_queue = queue.Queue()
        self.simulation_thread = None
        self.running = False
        self.profiler = None
    
    def start_simulation(self, config, world, agent):
        """Start simulation in separate thread"""
        self.running = True
        self.profiler = SimulationProfiler()
        
        def run_simulation():
            while self.running:
                try:
                    # Get simulation parameters from queue
                    params = self.simulation_queue.get(timeout=1.0)
                    
                    if params['action'] == 'step':
                        # Run simulation step
                        step = params.get('step', 0)
                        
                        # Record metrics
                        self.profiler.record_step(step, None, None, None)
                        
                        # Send update
                        self.result_queue.put({
                            'type': 'step_complete',
                            'step': step,
                            'time': time.time()
                        })
                    
                    elif params['action'] == 'stop':
                        break
                        
                except queue.Empty:
                    continue
                except Exception as e:
                    self.result_queue.put({
                        'type': 'error',
                        'error': str(e)
                    })
        
        self.simulation_thread = threading.Thread(target=run_simulation)
        self.simulation_thread.daemon = True
        self.simulation_thread.start()
    
    def send_step(self, step):
        """Send step command to simulation thread"""
        self.simulation_queue.put({'action': 'step', 'step': step})
    
    def stop(self):
        """Stop simulation"""
        self.running = False
        self.simulation_queue.put({'action': 'stop'})
        
        if self.simulation_thread:
            self.simulation_thread.join(timeout=5.0)
        
        # Generate final report
        if self.profiler:
            self.profiler.report()
    
    def get_results(self):
        """Get results from simulation"""
        results = []
        while not self.result_queue.empty():
            results.append(self.result_queue.get())
        return results

# Example usage
# simulator = NLMSimulator()
# simulator.start_simulation(config, world, agent)
# 
# for step in range(100):
#     simulator.send_step(step)
#     results = simulator.get_results()
#     for result in results:
#         if result['type'] == 'error':
#             print(f"Error: {result['error']}")
#         elif result['type'] == 'step_complete':
#             print(f"Step {result['step']} completed")
# 
# simulator.stop()
```

## Best Practices Summary

1. **Configuration Validation**: Always validate configuration before running simulations
2. **Error Handling**: Implement comprehensive error handling for production use
3. **Performance Monitoring**: Use profilers to track simulation performance
4. **Resource Management**: Use checkpointing for long-running simulations
5. **Parameter Sweeps**: Automate experiments with parameter sweeps
6. **Integration**: Consider multi-threaded architectures for complex simulations
7. **Documentation**: Document all custom configurations and parameters
8. **Testing**: Use test suites to verify functionality across different scenarios

## Next Steps

1. **Build enhanced bindings**: Compile the enhanced_bindings.cpp module
2. **Test enhanced API**: Verify all enhanced functionality works correctly
3. **Update documentation**: Document the enhanced API in the main documentation
4. **Performance optimization**: Optimize the enhanced bindings for production use
5. **Add more examples**: Create additional examples demonstrating advanced features

The enhanced API provides powerful tools for advanced NLM usage while maintaining compatibility with the existing pynlm module. Use the enhanced features when you need additional functionality beyond the basic API.