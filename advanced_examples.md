# NLM Advanced User Examples and Tutorials

# Advanced NLM User Guide

This document provides advanced examples and tutorials for using NLM (Neural Learning Machine) for sophisticated neural simulations.

## Advanced Python API Usage

### 1. Custom Brain Configuration

```python
import pynlm
import json

# Create custom brain configuration
config = pynlm.createDefaultConfig()

# Advanced neuron configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.08)
config.set("brain.initial_weight_mean", 0.3)
config.set("brain.initial_weight_std", 0.05)
config.set("brain.v_thresh", -45.0)
config.set("brain.v_rest", -65.0)
config.set("brain.v_reset", -70.0)

# Advanced plasticity configuration
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("plasticity.structural.enable", true)

# Advanced neuromodulation configuration
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.curiosity.strength", 0.8)
config.set("neuromod.novelty.enable", true)
config.set("neuromod.novelty.threshold", 0.2)

# Save advanced configuration
config.saveToFile("advanced_config.json")

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

### 2. Agent-Based Learning with Curiosity

```python
import pynlm
import numpy as np

class AdvancedAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        
        # Enable advanced learning capabilities
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        self.agent.enablePrediction(True)
        self.agent.enableSocialLearning(True)
    
    def runEpisode(self, max_steps=1000):
        """Run a complete learning episode with curiosity-driven exploration"""
        self.world.reset()
        self.world.setAgentStart(10.0, 10.0)
        
        episode_reward = 0.0
        curiosity_history = []
        novelty_history = []
        
        for step in range(max_steps):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain simulation
            self.brain.step(step)
            
            # Decode motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            episode_reward += reward
            
            # Apply reward modulation with prediction error
            prediction_error = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, prediction_error)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record curiosity and novelty levels
            curiosity_history.append(self.agent.getCuriosityLevel())
            novelty_history.append(self.agent.getNoveltyLevel())
            
            # Print progress every 100 steps
            if step % 100 == 0:
                print(f"Step {step}:")
                print(f"  Reward: {reward:.3f}, Total: {episode_reward:.3f}")
                print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
                print(f"  Prediction Error: {prediction_error:.3f}")
                print(f"  Developmental Stage: {self.agent.getDevelopmentalStage()}")
                print(f"  Average Firing: {self.brain.getAverageFiringRate():.3f} Hz")
        
        # Calculate episode statistics
        avg_curiosity = np.mean(curiosity_history) if curiosity_history else 0.0
        avg_novelty = np.mean(novelty_history) if novelty_history else 0.0
        
        return {
            'total_reward': episode_reward,
            'avg_curiosity': avg_curiosity,
            'avg_novelty': avg_novelty,
            'final_development_stage': self.agent.getDevelopmentalStage(),
            'total_steps': max_steps
        }

# Complete advanced agent simulation
def runAdvancedAgentSimulation():
    """Run a complete advanced agent simulation with all learning capabilities"""
    
    # Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.12)
    config.set("neuromod.curiosity.enable", true)
    config.set("neuromod.curiosity.strength", 0.9)
    config.set("neuromod.novelty.enable", true)
    config.set("plasticity.stdp.enable", true)
    config.set("plasticity.stdp.learning_rate", 0.008)
    config.set("plasticity.hebbian.enable", true)
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    agent.enableSocialLearning(True)
    agent.enableContinualLearning(True)
    
    # Run multiple episodes with progressive difficulty
    all_results = []
    for episode in range(5):
        print(f"\n=== Episode {episode + 1} ===")
        result = runEpisode(brain, agent, world, max_steps=500 + episode * 100)
        all_results.append(result)
        
        # Gradually increase world complexity
        if episode % 2 == 0:
            world.configure(width=20 + episode * 2, height=20 + episode * 2)
    
    # Analyze results
    print("\n=== Simulation Results ===")
    total_reward = sum(r['total_reward'] for r in all_results)
    avg_curiosity = sum(r['avg_curiosity'] for r in all_results) / len(all_results)
    final_stage = all_results[-1]['final_development_stage']
    
    print(f"Total Reward: {total_reward:.3f}")
    print(f"Average Curiosity: {avg_curiosity:.3f}")
    print(f"Final Developmental Stage: {final_stage:.3f}")
    print(f"Average Steps per Episode: {np.mean([r['total_steps'] for r in all_results]):.1f}")
    
    return all_results
```

### 3. Parallel Experiment Execution

```python
import pynlm
import time
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

def runSingleExperiment(experiment_id, config):
    """Run a single experiment with given configuration"""
    print(f"Starting experiment {experiment_id}...")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=16, height=16)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run experiment
    start_time = time.time()
    for step in range(1000):
        world.update(0.01)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    end_time = time.time()
    
    return {
        'experiment_id': experiment_id,
        'total_reward': agent.getNeuromodulationLevel(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'duration': end_time - start_time,
        'success': True
    }

# Parallel experiment execution
def runParallelExperiments(num_experiments=4, use_processes=False):
    """Run multiple experiments in parallel"""
    
    # Create base configuration
    base_config = pynlm.createDefaultConfig()
    base_config.set("brain.neuron_count", 500)
    base_config.set("neuromod.curiosity.enable", True)
    
    if use_processes:
        executor = ProcessPoolExecutor(max_workers=num_experiments)
    else:
        executor = ThreadPoolExecutor(max_workers=num_experiments)
    
    # Create experiment configurations
    experiment_configs = []
    for i in range(num_experiments):
        config = base_config.copy()
        config.set("random_seed", 42 + i)
        config.set("brain.neuron_count", 500 + i * 100)
        experiment_configs.append(config)
    
    # Submit experiments
    futures = []
    for i, config in enumerate(experiment_configs):
        future = executor.submit(runSingleExperiment, i, config)
        futures.append(future)
    
    # Collect results
    results = []
    for future in futures:
        results.append(future.result())
    
    executor.shutdown()
    
    # Analyze results
    total_time = sum(r['duration'] for r in results)
    avg_reward = sum(r['total_reward'] for r in results) / len(results)
    avg_curiosity = sum(r['curiosity_level'] for r in results) / len(results)
    
    print(f"Parallel execution completed:")
    print(f"  Total experiments: {len(results)}")
    print(f"  Total time: {total_time:.2f}s")
    print(f"  Average time per experiment: {total_time/len(results):.2f}s")
    print(f"  Average reward: {avg_reward:.3f}")
    print(f"  Average curiosity: {avg_curiosity:.3f}")
    
    return results
```

### 4. Advanced Configuration and Custom Environments

```python
import pynlm
import json
from typing import Dict, Any

class CustomWorldConfig:
    def __init__(self):
        self.width = 30
        self.height = 30
        self.obstacle_density = 0.3
        self.resource_locations = [(5, 5), (25, 25), (15, 20)]
        self.hazard_locations = [(10, 10), (20, 15)]
        self.vision_width = 6
        self.vision_height = 6
    
    def createCustomWorld(self, brain):
        """Create a custom world with obstacles and hazards"""
        world = pynlm.createCustomWorld()
        
        # Configure world dimensions
        world.configure(
            width=self.width,
            height=self.height,
            visionWidth=self.vision_width,
            visionHeight=self.vision_height
        )
        
        # Add obstacles
        for x in range(self.width):
            for y in range(self.height):
                if self.isObstacle(x, y):
                    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, value=1.0))
                
                if self.isHazard(x, y):
                    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=1.0))
                
                if self.isResource(x, y):
                    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=2.0))
        
        return world
    
    def isObstacle(self, x, y):
        # Simple obstacle pattern: create some walls
        if x % 3 == 0 and y % 3 == 0:
            return True
        if x < 5 or x > self.width - 5 or y < 5 or y > self.height - 5:
            return True
        return False
    
    def isHazard(self, x, y):
        return any(abs(x - rx) <= 2 and abs(y - ry) <= 2 
                  for rx, ry in self.hazard_locations)
    
    def isResource(self, x, y):
        return any(abs(x - rx) <= 2 and abs(y - ry) <= 2 
                  for rx, ry in self.resource_locations)

class AdvancedConfigBuilder:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def setBrainParameters(self, neuron_count=1000, synapse_density=0.1):
        self.config.set("brain.neuron_count", neuron_count)
        self.config.set("brain.synapse_density", synapse_density)
        return self
    
    def setPlasticity(self, stdp_enabled=True, hebbian_enabled=True, structural_enabled=True):
        self.config.set("plasticity.stdp.enable", stdp_enabled)
        self.config.set("plasticity.hebbian.enable", hebbian_enabled)
        self.config.set("plasticity.structural.enable", structural_enabled)
        return self
    
    def setNeuromodulation(self, dopamine_scale=1.0, curiosity_enabled=True, novelty_enabled=True):
        self.config.set("neuromod.dopamine.scale", dopamine_scale)
        self.config.set("neuromod.curiosity.enable", curiosity_enabled)
        self.config.set("neuromod.novelty.enable", novelty_enabled)
        return self
    
    def setDevelopment(self, development_enabled=True, critical_period_end=1000):
        self.config.set("development.enabled", development_enabled)
        self.config.set("development.critical_period_end", critical_period_end)
        return self
    
    def setLearning(self, learning_rate=0.01, exploration_rate=0.1):
        self.config.set("learning.rate", learning_rate)
        self.config.set("learning.exploration_rate", exploration_rate)
        return self
    
    def addCustomParameters(self, **kwargs):
        for key, value in kwargs.items():
            self.config.set(key, value)
        return self
    
    def build(self):
        return self.config
    
    def save(self, filename):
        self.config.saveToFile(filename)

# Example usage of advanced configuration builder
class AdvancedEnvironmentSimulation:
    def __init__(self, config):
        self.config = config
        self.custom_world = CustomWorldConfig()
        
        # Create brain and agent
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.createWorld())
        
        # Enable all advanced features
        self.setupAdvancedFeatures()
    
    def createWorld(self):
        return self.custom_world.createCustomWorld(self.brain)
    
    def setupAdvancedFeatures(self):
        """Enable all advanced simulation features"""
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        self.agent.enablePrediction(True)
        self.agent.enableSocialLearning(True)
        self.agent.enableContinualLearning(True)
        self.agent.enableReplayLearning(True)
    
    def runAdvancedSimulation(self, max_steps=2000):
        """Run advanced simulation with custom environment"""
        print("Starting Advanced Environment Simulation")
        print(f"Configuration: {self.config.getKeys()}")
        print(f"World size: {self.custom_world.width}x{self.custom_world.height}")
        
        simulation_results = {
            'step_rewards': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'development_stages': [],
            'episodes_completed': 0
        }
        
        for step in range(max_steps):
            # Update world
            self.agent.getBrain().step(step)
            
            # Get current state
            percept = self.agent.getBrain().getCurrentPercept()
            self.agent.processSensoryInput(percept)
            
            # Get action and apply to world
            action = self.agent.decodeMotorCommand()
            # world.applyAction(action, step * 0.01)
            
            # Record metrics
            simulation_results['curiosity_levels'].append(self.agent.getCuriosityLevel())
            simulation_results['novelty_levels'].append(self.agent.getNoveltyLevel())
            simulation_results['development_stages'].append(self.agent.getDevelopmentalStage())
            
            # Print progress
            if step % 200 == 0:
                avg_curiosity = sum(simulation_results['curiosity_levels'][-50:]) / min(50, len(simulation_results['curiosity_levels']))
                avg_novelty = sum(simulation_results['novelty_levels'][-50:]) / min(50, len(simulation_results['novelty_levels']))
                
                print(f"Step {step}: "
                      f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty={self.agent.getNoveltyLevel():.3f}, "
                      f"Development={self.agent.getDevelopmentalStage():.3f}, "
                      f"AvgCuriosity={avg_curiosity:.3f}")
        
        print("Advanced simulation completed successfully")
        return simulation_results
```

### 5. Performance Analysis and Benchmarking

```python
import pynlm
import time
import statistics
from typing import List, Dict, Any

class PerformanceAnalyzer:
    def __init__(self):
        self.benchmark_results = []
        self.memory_samples = []
        self.cpu_samples = []
    
    def runPerformanceBenchmark(self, config, steps=1000, neurons=500):
        """Run performance benchmark with given configuration"""
        print(f"Running performance benchmark with {neurons} neurons for {steps} steps...")
        
        # Update config
        config_copy = config.copy()
        config_copy.set("brain.neuron_count", neurons)
        
        # Create brain
        brain = pynlm.createBrain(config_copy)
        brain.initialize()
        
        # Measure performance
        start_time = time.time()
        start_memory = get_memory_usage()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        end_memory = get_memory_usage()
        
        # Calculate metrics
        duration = end_time - start_time
        steps_per_second = steps / duration
        memory_increase = end_memory - start_memory
        avg_firing_rate = brain.getAverageFiringRate()
        
        result = {
            'neurons': neurons,
            'steps': steps,
            'duration': duration,
            'steps_per_second': steps_per_second,
            'memory_increase': memory_increase,
            'avg_firing_rate': avg_firing_rate,
            'config_params': config_copy.getKeys()
        }
        
        self.benchmark_results.append(result)
        print(f"  Steps/sec: {steps_per_second:.1f}")
        print(f"  Memory increase: {memory_increase} bytes")
        print(f"  Avg firing rate: {avg_firing_rate:.3f} Hz")
        
        return result
    
    def runScalabilityTest(self, config, neuron_counts=None):
        """Test how performance scales with different neuron counts"""
        if neuron_counts is None:
            neuron_counts = [100, 250, 500, 1000, 2000]
        
        print("Running scalability test...")
        
        for neuron_count in neuron_counts:
            result = self.runPerformanceBenchmark(config, steps=500, neurons=neuron_count)
            print(f"  Neurons: {neuron_count}, Performance: {result['steps_per_second']:.1f} steps/sec")
        
        return self.benchmark_results
    
    def analyzeResults(self):
        """Analyze and report benchmark results"""
        if not self.benchmark_results:
            print("No benchmark results to analyze")
            return
        
        print("\n=== Performance Analysis ===")
        
        # Performance summary
        print("Performance Summary:")
        for result in self.benchmark_results:
            print(f"  {result['neurons']} neurons: {result['steps_per_second']:.1f} steps/sec")
        
        # Calculate averages
        avg_steps_per_second = statistics.mean(r['steps_per_second'] for r in self.benchmark_results)
        max_steps_per_second = max(r['steps_per_second'] for r in self.benchmark_results)
        
        print(f"\nAverage performance: {avg_steps_per_second:.1f} steps/sec")
        print(f"Maximum performance: {max_steps_per_second:.1f} steps/sec")
        
        # Scalability analysis
        if len(self.benchmark_results) >= 2:
            print("\nScalability Analysis:")
            for i in range(1, len(self.benchmark_results)):
                prev = self.benchmark_results[i-1]
                curr = self.benchmark_results[i]
                ratio = curr['steps_per_second'] / prev['steps_per_second']
                neuron_ratio = curr['neurons'] / prev['neurons']
                print(f"  Neurons {prev['neurons']} -> {curr['neurons']}: "
                      f"Performance ratio: {ratio:.2f}, Neuron ratio: {neuron_ratio:.2f}")
        
        # Memory efficiency
        print("\nMemory Efficiency:")
        for result in self.benchmark_results:
            if result['neurons'] > 0:
                memory_efficiency = result['steps_per_second'] / result['memory_increase']
                print(f"  Neurons: {result['neurons']}, Efficiency: {memory_efficiency:.2f} steps/sec/byte")
        
        return self.benchmark_results

# Comprehensive benchmarking utility
class BenchmarkingSuite:
    def __init__(self):
        self.analyzer = PerformanceAnalyzer()
    
    def runFullBenchmarkSuite(self):
        """Run comprehensive benchmark suite"""
        print("=== NLM Performance Benchmark Suite ===")
        
        # Create base configuration
        base_config = pynlm.createDefaultConfig()
        
        # Test 1: Scalability with different neuron counts
        print("\n1. Scalability Test")
        self.analyzer.runScalabilityTest(base_config)
        
        # Test 2: Different plasticity configurations
        print("\n2. Plasticity Configuration Test")
        plasticity_configs = [
            {"stdp_enabled": True, "hebbian_enabled": True, "structural_enabled": True},
            {"stdp_enabled": True, "hebbian_enabled": False, "structural_enabled": False},
            {"stdp_enabled": False, "hebbian_enabled": True, "structural_enabled": False},
        ]
        
        for i, plasticity_settings in enumerate(plasticity_configs):
            config = base_config.copy()
            config.set("plasticity.stdp.enable", plasticity_settings["stdp_enabled"])
            config.set("plasticity.hebbian.enable", plasticity_settings["hebbian_enabled"])
            config.set("plasticity.structural.enable", plasticity_settings["structural_enabled"])
            
            result = self.analyzer.runPerformanceBenchmark(config, neurons=500)
            print(f"  Configuration {i+1}: {plasticity_settings}")
        
        # Test 3: Neuromodulation variations
        print("\n3. Neuromodulation Test")
        neuromod_configs = [
            {"curiosity_enabled": True, "novelty_enabled": True, "prediction_enabled": True},
            {"curiosity_enabled": False, "novelty_enabled": True, "prediction_enabled": False},
            {"curiosity_enabled": True, "novelty_enabled": False, "prediction_enabled": True},
        ]
        
        for i, neuromod_settings in enumerate(neuromod_configs):
            config = base_config.copy()
            config.set("neuromod.curiosity.enable", neuromod_settings["curiosity_enabled"])
            config.set("neuromod.novelty.enable", neuromod_settings["novelty_enabled"])
            config.set("neuromod.prediction.enable", neuromod_settings["prediction_enabled"])
            
            result = self.analyzer.runPerformanceBenchmark(config, neurons=500)
            print(f"  Neuromodulation {i+1}: {neuromod_settings}")
        
        # Analyze all results
        print("\n" + "="*50)
        self.analyzer.analyzeResults()
        
        return self.analyzer.benchmark_results

# Example usage
if __name__ == "__main__":
    # Create benchmark suite
    suite = BenchmarkingSuite()
    
    # Run comprehensive benchmarks
    results = suite.runFullBenchmarkSuite()
    
    print("\nBenchmark suite completed successfully!")
```

## Advanced Features Documentation

### Advanced Experiment Features

#### 1. Parallel Experiment Execution
- **What it is**: Run multiple experiments simultaneously using multiple threads or processes
- **When to use**: When you need to test multiple hypotheses or reduce overall experiment time
- **Key settings**: Number of threads, batch size, synchronization method

#### 2. Advanced Configuration Management
- **What it is**: Create and manage complex configurations programmatically
- **When to use**: When you need precise control over brain parameters and experimental conditions
- **Key settings**: Configuration inheritance, validation, saving/loading

#### 3. Performance Profiling
- **What it is**: Monitor and analyze system performance during experiments
- **When to use**: When optimizing brain performance or identifying bottlenecks
- **Key settings**: Memory monitoring, CPU profiling, execution time measurement

#### 4. Custom Environment Creation
- **What it is**: Design custom worlds with obstacles, resources, and hazards
- **When to use**: When you need specific environmental conditions for testing
- **Key settings**: World dimensions, object types, sensory ranges

#### 5. Advanced Learning Algorithms
- **What it is**: Implement and test different learning mechanisms
- **When to use**: When exploring different approaches to machine learning
- **Key settings**: Learning rates, exploration strategies, reward mechanisms

## Quick Start Guide for Advanced Users

### Step 1: Set Up Advanced Environment
```python
import pynlm
import numpy as np

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("neuromod.curiosity.enable", True)
config.set("plasticity.stdp.enable", True)
config.set("development.enabled", True)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent.initialize(world)

# Enable advanced features
agent.enableAllAdvancedFeatures()
```

### Step 2: Run Advanced Simulation
```python
# Run curiosity-driven exploration
for step in range(1000):
    world.update(0.01)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Log advanced metrics
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development Stage: {agent.getDevelopmentalStage():.3f}")
```

### Step 3: Analyze Results
```python
# Analyze learning progress
learning_curve = [agent.getCuriosityLevel() for _ in range(1000)]
avg_curiosity = np.mean(learning_curve)
max_curiosity = np.max(learning_curve)

# Save results
results = {
    'learning_curve': learning_curve,
    'avg_curiosity': avg_curiosity,
    'max_curiosity': max_curiosity,
    'final_development': agent.getDevelopmentalStage()
}

import json
with open('advanced_results.json', 'w') as f:
    json.dump(results, f, indent=2)
```

## Troubleshooting and Common Issues

### Issue 1: High Memory Usage
**Solution**: 
- Reduce neuron count
- Disable unnecessary features
- Use sparse connectivity
- Implement memory-efficient data structures

### Issue 2: Slow Performance
**Solution**:
- Reduce simulation steps
- Disable profiling during performance runs
- Use fewer threads
- Optimize configuration parameters

### Issue 3: Unstable Learning
**Solution**:
- Adjust learning rates
- Enable stability mechanisms
- Use smaller network sizes
- Implement regularization

## Advanced Configuration Parameters

### Brain Configuration
- `brain.neuron_count`: Number of neurons (default: 1000)
- `brain.synapse_density`: Synapse density (default: 0.1)
- `brain.connection_probability`: Connection probability (default: 0.05)
- `brain.v_thresh`: Neuron threshold (default: -50.0)
- `brain.v_rest`: Resting potential (default: -70.0)
- `brain.v_reset`: Reset potential (default: -75.0)

### Plasticity Configuration
- `plasticity.stdp.enable`: Enable STDP (default: true)
- `plasticity.stdp.learning_rate`: STDP learning rate (default: 0.001)
- `plasticity.hebbian.enable`: Enable Hebbian learning (default: true)
- `plasticity.hebbian.learning_rate`: Hebbian learning rate (default: 0.01)
- `plasticity.structural.enable`: Enable structural plasticity (default: true)

### Neuromodulation Configuration
- `neuromod.dopamine.scale`: Dopamine scale (default: 1.0)
- `neuromod.curiosity.enable`: Enable curiosity (default: true)
- `neuromod.curiosity.strength`: Curiosity strength (default: 0.8)
- `neuromod.novelty.enable`: Enable novelty detection (default: true)
- `neuromod.novelty.threshold`: Novelty threshold (default: 0.2)

### Development Configuration
- `development.enabled`: Enable development (default: true)
- `development.critical_period_end`: Critical period end (default: 1000)
- `development.synaptogenesis_rate`: Synaptogenesis rate (default: 0.001)
- `development.pruning_rate`: Pruning rate (default: 0.0001)

## Advanced Tips and Tricks

### 1. Configuration Templates
Create reusable configuration templates for different experiment types:

```python
class ExperimentTemplate:
    def __init__(self, name, config):
        self.name = name
        self.config = config
    
    def apply(self, base_config):
        for key in self.config.getKeys():
            base_config.set(key, self.config.get(key))
        return base_config
```

### 2. Experiment Comparison
Compare different experiment configurations systematically:

```python
def compareExperiments(configs, experiment_name):
    results = []
    for i, config in enumerate(configs):
        print(f"Running {experiment_name} {i+1}")
        result = runExperiment(config)
        results.append(result)
    
    return results
```

### 3. Performance Optimization
- Use batch processing for large simulations
- Implement checkpointing for long experiments
- Use parallel execution for multiple independent experiments
- Monitor and optimize memory usage

### 4. Advanced Analysis
- Track learning curves over time
- Analyze neural activity patterns
- Compare experimental results statistically
- Visualize results with advanced plotting

## Next Steps

For more advanced features and customizations, consider:

1. **Implementing custom neuron models**: Replace LIF neurons with more complex models
2. **Developing custom plasticity rules**: Create novel learning mechanisms
3. **Building custom neuromodulation systems**: Design specialized reward systems
4. **Creating custom environments**: Design specific ecological contexts
5. **Implementing advanced analysis tools**: Develop custom analysis and visualization tools

The NLM framework provides a solid foundation for advanced neural simulation research. These examples and guides should help you get started with creating sophisticated, realistic neural systems that can learn and adapt to complex environments.

For questions or additional examples, consult the comprehensive documentation and API reference.