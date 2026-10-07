# NLM Enhanced Documentation
# Advanced Neural Simulation Framework

## Overview

This document provides comprehensive documentation for the enhanced NLM (Neural Learning Machine) framework, including advanced features for experienced users, performance optimization, and parallel processing capabilities.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Advanced Configuration](#advanced-configuration)
3. [Performance Optimization](#performance-optimization)
4. [Parallel Processing](#parallel-processing)
5. [Memory Management](#memory-management)
6. [Neuromodulation Systems](#neuromodulation-systems)
7. [Cognition Systems](#cognition-systems)
8. [Development System](#development-system)
9. [Error Handling and Validation](#error-handling-and-validation)
10. [Research Tools](#research-tools)
11. [Performance Benchmarking](#performance-benchmarking)
12. [Advanced Python API](#advanced-python-api)
13. [Command Line Interface](#command-line-interface)
14. [Configuration Files](#configuration-files)
15. [Troubleshooting](#troubleshooting)

## Quick Start

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(1000):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Advanced Usage

```python
import pynlm
import time

# Create an advanced brain with all systems
config = pynlm.createAdvancedConfig()
brain = pynlm.createAdvancedBrain(config)

# Create world and agent for environment interaction
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Setup environment
world.configure(30, 30, 12, 12)
world.setAgentStart(15, 15)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run a complete simulation
for step in range(5000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Advanced simulation complete: {brain.getTotalSpikeCount()} spikes")
```

## Advanced Configuration

### Configuration Types

#### Minimal Configuration
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("simulation.timestep", 0.001)
config.set("simulation.max_steps", 100)
```

#### Standard Configuration
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1f)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("memory.working_memory.enable", True)
config.set("memory.episodic_memory.enable", True)
```

#### Advanced Configuration
```python
config = pynlm.createAdvancedConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15f)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("memory.working_memory.enable", True)
config.set("memory.episodic_memory.enable", True)
config.set("memory.associative_memory.enable", True)
config.set("neuromodulation.enable", True)
config.set("cognition.planning.enable", True)
config.set("cognition.concept.enable", True)
config.set("cognition.attention.enable", True)
config.set("development.enable", True)
config.set("simulation.timestep", 0.0005)
```

#### Research Configuration
```python
config = pynlm.createResearchConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.1f)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("memory.working_memory.enable", True)
config.set("memory.episodic_memory.enable", True)
config.set("memory.associative_memory.enable", True)
config.set("neuromodulation.enable", True)
config.set("cognition.planning.enable", True)
config.set("cognition.concept.enable", True)
config.set("cognition.attention.enable", True)
config.set("development.enable", True)
config.set("simulation.timestep", 0.001)
config.set("simulation.max_steps", 50000)
```

### Configuration Validation

```python
def validate_config(config):
    errors = []
    warnings = []
    
    # Validate neuron count
    neuron_count = config.get("brain.neuron_count", 1000)
    if neuron_count <= 0:
        errors.append("brain.neuron_count must be positive")
    
    # Validate timestep
    timestep = config.get("simulation.timestep", 0.001)
    if timestep <= 0.0 || timestep > 0.1:
        warnings.append("simulation.timestep should be positive and less than 0.1")
    
    # Validate memory capacity
    if config.has("memory.working_memory.capacity"):
        capacity = config.get("memory.working_memory.capacity", 100)
        if capacity <= 0:
            errors.append("working memory capacity must be positive")
    
    return errors, warnings

# Example usage
errors, warnings = validate_config(config)
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
        
if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

## Performance Optimization

### Performance Monitoring

```python
import pynlm
import time

# Create advanced brain
config = pynlm.createAdvancedConfig()
brain = pynlm.createAdvancedBrain(config)

# Performance benchmark
start_time = time.time()
results = pynlm.benchmarkPerformance(brain, 5000)
end_time = time.time()

print("Performance Results:")
print(f"  Total time: {results['total_time_ms']:.2f} ms")
print(f"  Steps per second: {results['steps_per_second']:.2f}")
print(f"  Neurons processed: {results['neurons_per_second']:.2f}/s")
print(f"  Synapses processed: {results['synapses_per_second']:.2f}/s")
print(f"  Spikes generated: {results['spikes_per_step']:.2f} per step")
```

### Performance Tips

1. **Use Advanced Configuration**: For best performance, use `createAdvancedConfig()` instead of `createDefaultConfig()`.

2. **Optimize Timestep**: Smaller timesteps improve accuracy but reduce speed. Use `0.001` ms for balance.

3. **Enable Memory Systems**: Advanced memory systems improve learning but add overhead. Enable only needed systems.

4. **Use Parallel Processing**: For large simulations, enable parallel processing.

5. **Monitor Memory Usage**: Use `brain.logStatus()` to monitor memory consumption.

### Performance Configuration

```python
# Performance-optimized configuration
config = pynlm.createAdvancedConfig()
config.set("brain.neuron_count", 2000)  # Balance size and speed
config.set("simulation.timestep", 0.001)  # Optimal timestep
config.set("memory.working_memory.enable", True)  # Important for learning
config.set("memory.episodic_memory.enable", False)  # Disable for speed
config.set("cognition.enable", False)  # Disable for maximum speed
config.set("development.enable", False)  # Disable for stability
```

## Parallel Processing

### Parallel Processing Configuration

```python
# Enable parallel processing
config = pynlm.createAdvancedConfig()
config.set("parallel.enable", True)
config.set("parallel.threads", std::thread::hardware_concurrency())
config.set("parallel.neuron_distribution", True)
config.set("parallel.synapse_distribution", True)
config.set("parallel.memory_distribution", True)

brain = pynlm.createAdvancedBrain(config)
```

### Parallel Processing Benefits

1. **Neuron Processing**: Distribute neuron processing across multiple threads.
2. **Synapse Processing**: Parallelize synaptic weight updates.
3. **Memory Access**: Concurrent access to memory systems.
4. **Event Processing**: Parallel spike event processing.

### Parallel Usage Examples

```python
def parallel_simulation(brain, world, agent, steps, threads=4):
    import threading
    import queue
    
    result_queue = queue.Queue()
    step_queue = queue.Queue()
    
    # Distribute steps across threads
    for step in range(steps):
        step_queue.put(step)
    
    def worker():
        local_spike_count = 0
        while not step_queue.empty():
            try:
                step = step_queue.get_nowait()
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                local_spike_count += brain.getTotalSpikeCount()
            except queue.Empty:
                break
        result_queue.put(local_spike_count)
    
    # Start worker threads
    threads_list = []
    for _ in range(threads):
        thread = threading.Thread(target=worker)
        thread.start()
        threads_list.append(thread)
    
    # Wait for completion
    for thread in threads_list:
        thread.join()
    
    # Aggregate results
    total_spikes = sum(result_queue.queue)
    return total_spikes
```

## Memory Management

### Memory System Configuration

```python
# Configure memory systems
config = pynlm.createAdvancedConfig()
config.set("memory.working_memory.enable", True)
config.set("memory.working_memory.capacity", 500)
config.set("memory.working_memory.decay_rate", 0.01f)

config.set("memory.episodic_memory.enable", True)
config.set("memory.episodic_memory.max_episodes", 5000)
config.set("memory.episodic_memory.consolidation_threshold", 0.8f)

config.set("memory.associative_memory.enable", True)
config.set("memory.associative_memory.association_strength", 0.5f)
```

### Memory Monitoring

```python
def monitor_memory_usage(brain):
    status = brain.getNeuralActivityStats()
    memory_stats = brain.getWorkingMemoryStats()
    episodic_stats = brain.getEpisodicMemoryStats()
    
    print("Memory Usage:")
    print(f"  Working Memory: {memory_stats.get('active_traces', 0)} traces")
    print(f"  Episodic Memory: {memory_stats.get('episode_count', 0)} episodes")
    print(f"  Associative Memory: {memory_stats.get('association_count', 0)} associations")
    print(f"  Total Memory: {memory_stats.get('total_memory_mb', 0)} MB")
    
    return memory_stats
```

## Neuromodulation Systems

### Neuromodulation Configuration

```python
# Configure neuromodulation systems
config = pynlm.createAdvancedConfig()
config.set("neuromodulation.dopamine.scale", 1.0f)
config.set("neuromodulation.curiosity.threshold", 0.1f)
config.set("neuromodulation.novelty.decay_rate", 0.05f)
config.set("neuromodulation.prediction.error_threshold", 0.2f)
```

### Neuromodulation Usage

```python
def enhanced_simulation(brain, world, agent):
    # Get neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    
    # Custom neuromodulation
    dopamine.setLevel(0.5f)  # Moderate dopamine level
    curiosity.setNoveltyWeight(0.6f)  # Favor novelty-based curiosity
    curiosity.setPredictionErrorWeight(0.4f)  # Favor prediction-error-based curiosity
    
    # Run simulation with enhanced neuromodulation
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Inject neuromodulation signals
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        dopamine.signalReward(reward)
        
        novelty_level = novelty.getLevel()
        prediction_error_level = prediction_error.getMagnitude()
        curiosity.update(novelty_level, prediction_error_level, 0.001)
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## Cognition Systems

### Cognitive System Configuration

```python
# Configure cognitive systems
config = pynlm.createAdvancedConfig()
config.set("cognition.planning.depth", 5)
config.set("cognition.planning.horizon", 10)
config.set("cognition.attention.inhibition_strength", 0.5f)
config.set("cognition.attention.excitation_strength", 1.5f)
config.set("cognition.concept.learning_rate", 0.01f)
```

### Cognitive Usage

```python
def cognitive_simulation(brain, world, agent):
    # Get cognitive systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    # Configure cognitive parameters
    planner.setPlanningDepth(10)
    attention.setInhibitionStrength(0.3f)
    attention.setExcitationStrength(2.0f)
    
    # Run simulation with cognitive enhancement
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Process cognitive information
        brain.processSensoryInput(world.getSensoryPercept())
        
        # Update cognitive systems
        planner.update(brain.getSensoryPercept())
        concept_formation.update(brain.getSensoryPercept())
        attention.update(brain.getSensoryPercept())
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## Development System

### Development System Configuration

```python
# Configure development system
config = pynlm.createAdvancedConfig()
config.set("development.stage", "Maturation")
config.set("development.plasticity_modulation", True)
config.set("development.neuromodulation_effect", True)
config.set("development.adaptation_rate", 0.01f)
```

### Development Usage

```python
def developmental_simulation(brain, world, agent):
    # Get development system
    development = brain.getDevelopmentSystem()
    
    # Set development stage
    development.setDevelopmentalStage("Maturation")
    
    # Run simulation with development
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Update development
        agent.updateDevelopment(0.1)
        brain.develop()
        
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## Error Handling and Validation

### Error Handling

```python
def safe_simulation(brain, steps):
    try:
        for step in range(steps):
            brain.step(step)
        return True, "Simulation completed successfully"
    except std::runtime_error as e:
        return False, f"Runtime error: {e}"
    except std::exception as e:
        return False, f"Error: {e}"
    except ...:
        return False, "Unknown error occurred"

# Usage
 success, message = safe_simulation(brain, 1000)
if success:
    print(message)
else:
    print(f"Simulation failed: {message}")
    # Handle error
```

### Configuration Validation

```python
def validate_configuration(config):
    """Validate configuration and return list of errors and warnings."""
    errors = []
    warnings = []
    
    # Check required fields
    if not config.has("brain.neuron_count"):
        errors.append("Missing required field: brain.neuron_count")
    
    # Validate field values
    neuron_count = config.get("brain.neuron_count", 0)
    if neuron_count <= 0:
        errors.append("brain.neuron_count must be positive")
    
    timestep = config.get("simulation.timestep", 0.0)
    if timestep <= 0.0:
        errors.append("simulation.timestep must be positive")
    
    # Check for conflicts
    if config.get("memory.episodic_memory.enable", True) and \
       config.get("memory.associative_memory.enable", True):
        warnings.append("Both episodic and associative memory enabled - may increase memory usage")
    
    return errors, warnings

# Usage
errors, warnings = validate_configuration(config)
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
        
if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

## Research Tools

### Experiment Management

```python
class ExperimentManager:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.experiments = {}
        self.results = {}
    
    def create_experiment(self, name, parameters):
        """Create a new experiment."""
        experiment = {
            'name': name,
            'brain': self.brain,
            'world': self.world,
            'agent': self.agent,
            'parameters': parameters,
            'start_time': time.time(),
            'status': 'initialized'
        }
        self.experiments[name] = experiment
        return experiment
    
    def run_experiment(self, name, steps):
        """Run an experiment and store results."""
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
        
        experiment = self.experiments[name]
        experiment['status'] = 'running'
        
        start_time = time.time()
        
        for step in range(steps):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        end_time = time.time()
        
        # Store results
        result = {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_firing_rate': self.brain.getAverageFiringRate(),
            'steps_completed': steps,
            'duration_ms': (end_time - start_time) * 1000,
            'steps_per_second': steps / (end_time - start_time),
            'spikes_per_step': self.brain.getTotalSpikeCount() / steps
        }
        
        self.results[name] = result
        experiment['status'] = 'completed'
        experiment['result'] = result
        
        return result
    
    def compare_experiments(self, experiment_names):
        """Compare multiple experiments."""
        comparison = {}
        
        for name in experiment_names:
            if name in self.results:
                comparison[name] = self.results[name]
        
        # Create comparison table
        print("Experiment Comparison:")
        print("Name\t\tSpikes\t\tFiring Rate\t\tDuration (ms)\t\tSteps/sec")
        print("=" * 80)
        
        for name, result in comparison.items():
            print(f"{name}\t\t{result['total_spikes']}\t\t{result['final_firing_rate']:.4f}\t\t"
                  f"{result['duration_ms']:.2f}\t\t{result['steps_per_second']:.2f}")
        
        return comparison
```

### Data Analysis

```python
class DataAnalyzer:
    def __init__(self, results):
        self.results = results
    
    def analyze_trends(self, metric='firing_rate'):
        """Analyze trends in results."""
        trends = {}
        
        for name, result in self.results.items():
            if metric in result:
                trends[name] = result[metric]
        
        # Simple trend analysis
        sorted_trends = sorted(trends.items(), key=lambda x: x[1])
        
        print(f"Trend analysis for {metric}:")
        for name, value in sorted_trends:
            print(f"  {name}: {value:.4f}")
        
        return trends
    
    def generate_report(self):
        """Generate a comprehensive report."""
        report = {
            'summary': {
                'total_experiments': len(self.results),
                'total_spikes': sum(r.get('total_spikes', 0) for r in self.results.values()),
                'average_firing_rate': sum(r.get('final_firing_rate', 0) for r in self.results.values()) / len(self.results),
                'total_duration_ms': sum(r.get('duration_ms', 0) for r in self.results.values())
            },
            'detailed_results': self.results,
            'recommendations': self.generate_recommendations()
        }
        
        return report
    
    def generate_recommendations(self):
        """Generate recommendations based on results."""
        recommendations = []
        
        if len(self.results) < 2:
            return recommendations
        
        # Find best performing experiment
        best_name = max(self.results.items(), 
                      key=lambda x: x[1].get('steps_per_second', 0))
        
        recommendations.append(f"Best performance: {best_name[0]} with {best_name[1].get('steps_per_second', 0):.2f} steps/sec")
        
        # Check for consistency
        firing_rates = [r.get('final_firing_rate', 0) for r in self.results.values()]
        if max(firing_rates) / min(firing_rates) > 2.0:
            recommendations.append("High variance in firing rates - consider optimizing configuration")
        
        return recommendations
```

## Performance Benchmarking

### Benchmark Suite

```python
class BenchmarkSuite:
    def __init__(self):
        self.benchmarks = {}
    
    def add_benchmark(self, name, func, **kwargs):
        """Add a benchmark function."""
        self.benchmarks[name] = {
            'function': func,
            'kwargs': kwargs,
            'results': None
        }
    
    def run_benchmarks(self):
        """Run all benchmarks."""
        for name, benchmark in self.benchmarks.items():
            print(f"Running benchmark: {name}")
            start_time = time.time()
            
            result = benchmark['function'](**benchmark['kwargs'])
            
            end_time = time.time()
            
            benchmark['results'] = {
                'result': result,
                'duration_ms': (end_time - start_time) * 1000,
                'timestamp': time.time()
            }
            
            print(f"  Completed in {benchmark['results']['duration_ms']:.2f} ms")
    
    def generate_report(self):
        """Generate benchmark report."""
        report = {
            'timestamp': time.time(),
            'benchmarks': {},
            'summary': self.generate_summary()
        }
        
        for name, benchmark in self.benchmarks.items():
            report['benchmarks'][name] = {
                'duration_ms': benchmark['results']['duration_ms'],
                'result': benchmark['results']['result']
            }
        
        return report
    
    def generate_summary(self):
        """Generate benchmark summary."""
        if not self.benchmarks:
            return {}
        
        total_duration = sum(b['results']['duration_ms'] for b in self.benchmarks.values())
        
        return {
            'total_benchmarks': len(self.benchmarks),
            'total_duration_ms': total_duration,
            'average_duration_ms': total_duration / len(self.benchmarks),
            'fastest_benchmark': min(self.benchmarks.items(), 
                                   key=lambda x: x[1]['results']['duration_ms']),
            'slowest_benchmark': max(self.benchmarks.items(), 
                                  key=lambda x: x[1]['results']['duration_ms'])
        }
```

### Performance Profiling

```python
def profile_performance(brain, steps):
    """Profile performance of brain simulation."""
    import cProfile
    import pstats
    import io
    
    # Create profiler
    pr = cProfile.Profile()
    
    # Run simulation with profiling
    pr.enable()
    for step in range(steps):
        brain.step(step)
    pr.disable()
    
    # Generate statistics
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats(20)  # Top 20 functions
    
    # Get profile results
    profile_results = s.getvalue()
    
    # Analyze performance
    analysis = {
        'total_steps': steps,
        'profile': profile_results,
        'bottlenecks': identify_bottlenecks(profile_results),
        'recommendations': generate_recommendations(profile_results)
    }
    
    return analysis

def identify_bottlenecks(profile):
    """Identify performance bottlenecks."""
    bottlenecks = []
    
    # Look for functions with high cumulative time
    lines = profile.split('\n')
    for line in lines:
        if 'ncalls' in line and 'tottime' in line:
            # Parse bottleneck information
            # (implementation depends on cProfile output format)
            pass
    
    return bottlenecks

def generate_recommendations(profile):
    """Generate performance recommendations."""
    recommendations = []
    
    # Analyze profile and generate recommendations
    recommendations.append("Consider optimizing frequently called functions")
    recommendations.append("Enable parallel processing for large simulations")
    recommendations.append("Use performance-optimized configuration")
    
    return recommendations
```

## Advanced Python API

### Core Classes

#### Brain Class
```python
class Brain:
    def __init__(self, config):
        self._brain = pynlm.createBrain(config)
    
    def initialize(self):
        return self._brain.initialize()
    
    def step(self, step):
        return self._brain.step(step)
    
    def step_with_time(self, step, time):
        return self._brain.step(step, time)
    
    def get_total_spike_count(self):
        return self._brain.getTotalSpikeCount()
    
    def get_firing_rate(self):
        return self._brain.getAverageFiringRate()
    
    def produce_action(self):
        return self._brain.produceAction()
    
    def reset(self):
        return self._brain.reset()
    
    def save(self, filepath):
        return self._brain.save(filepath)
    
    def load(self, filepath):
        return self._brain.load(filepath)
    
    # Advanced methods
    def get_spike_system(self):
        return self._brain.getSpikeSystem()
    
    def get_stdp(self):
        return self._brain.getSTDP()
    
    def get_hebbian(self):
        return self._brain.getHebbian()
    
    def get_structural_plasticity(self):
        return self._brain.getStructuralPlasticity()
    
    def get_working_memory(self):
        return self._brain.getWorkingMemory()
    
    def get_episodic_memory(self):
        return self._brain.getEpisodicMemory()
    
    def get_associative_memory(self):
        return self._brain.getAssociativeMemory()
    
    def get_prediction_system(self):
        return self._brain.getPredictionSystem()
    
    def get_planner(self):
        return self._brain.getPlanner()
    
    def get_concept_formation(self):
        return self._brain.getConceptFormation()
    
    def get_attention(self):
        return self._brain.getAttention()
    
    def get_development_system(self):
        return self._brain.getDevelopmentSystem()
    
    def get_dopamine(self):
        return self._brain.getDopamine()
    
    def get_curiosity(self):
        return self._brain.getCuriosity()
    
    def get_novelty(self):
        return self._brain.getNovelty()
    
    def get_prediction_error(self):
        return self._brain.getPredictionErrorSignal()
    
    def get_random_generator(self):
        return self._brain.getRandomGenerator()
    
    def log_status(self):
        return self._brain.logStatus()
```

#### AgentBrain Class
```python
class AgentBrain:
    def __init__(self, brain):
        self._agent = pynlm.createAgentBrain(brain)
    
    def initialize(self, world):
        return self._agent.initialize(world)
    
    def process_sensory_input(self, percept):
        return self._agent.processSensoryInput(percept)
    
    def decode_motor_command(self):
        return self._agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward, predicted_reward):
        return self._agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep):
        return self._agent.updateDevelopment(timestep)
    
    def get_developmental_stage(self):
        return self._agent.getDevelopmentalStage()
    
    def get_neuromodulation_level(self):
        return self._agent.getNeuromodulationLevel()
    
    def get_curiosity_level(self):
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self):
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self):
        return self._agent.getPredictionError()
    
    def reset(self):
        return self._agent.reset()
    
    def get_brain(self):
        return self._agent.getBrain()
    
    # Advanced methods
    def enable_reward_modulation(self, enable):
        return self._agent.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable):
        return self._agent.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable):
        return self._agent.enableDevelopment(enable)
    
    def enable_curiosity(self, enable):
        return self._agent.enableCuriosity(enable)
    
    def is_reward_modulation_enabled(self):
        return self._agent.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self):
        return self._agent.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self):
        return self._agent.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self):
        return self._agent.isCuriosityEnabled()
```

#### Configuration Class
```python
class Config:
    def __init__(self):
        self._config = pynlm.createDefaultConfig()
    
    def load_from_file(self, filepath):
        return self._config.loadFromFile(filepath)
    
    def load_from_args(self, argc, argv):
        return self._config.loadFromArgs(argc, argv)
    
    def save_to_file(self, filepath):
        return self._config.saveToFile(filepath)
    
    def has(self, key):
        return self._config.has(key)
    
    def get_keys(self):
        return self._config.getKeys()
    
    def clear(self):
        return self._config.clear()
    
    def get(self, key, default_value=None):
        value = self._config.get(key)
        return value if value is not None else default_value
    
    def set(self, key, value, source=None):
        return self._config.set(key, value, source)
    
    # Advanced methods
    def set_int(self, key, value, source=None):
        return self._config.set(key, value, source)
    
    def set_float(self, key, value, source=None):
        return self._config.set(key, value, source)
    
    def set_string(self, key, value, source=None):
        return self._config.set(key, value, source)
    
    def set_bool(self, key, value, source=None):
        return self._config.set(key, value, source)
    
    def get_int(self, key, default_value=0):
        return self._config.getOr(key, default_value)
    
    def get_float(self, key, default_value=0.0):
        return self._config.getOr(key, default_value)
    
    def get_string(self, key, default_value=""):
        return self._config.getOr(key, default_value)
    
    def get_bool(self, key, default_value=False):
        return self._config.getOr(key, default_value)
    
    def validate(self):
        # Custom validation logic
        errors = []
        warnings = []
        # Implement validation
        return errors, warnings
    
    def to_dict(self):
        # Convert to Python dictionary
        result = {}
        for key in self._config.getKeys():
            result[key] = self._config.get(key)
        return result
    
    @classmethod
    def from_dict(cls, config_dict):
        # Create config from dictionary
        config = cls()
        for key, value in config_dict.items():
            config.set(key, value)
        return config
```

#### World and Agent Classes
```python
class WorldManager:
    def __init__(self):
        self._world = pynlm.createSimpleWorld()
    
    def configure(self, width, height, vision_width, vision_height):
        return self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self):
        return self._world.reset()
    
    def set_agent_start(self, x, y):
        return self._world.setAgentStart(x, y)
    
    def update(self, timestep):
        return self._world.update(timestep)
    
    def apply_motor_command(self, cmd, current_time):
        return self._world.applyMotorCommand(cmd, current_time)
    
    def get_sensory_percept(self):
        return self._world.getSensoryPercept()
    
    def get_agent_body(self):
        return self._world.getAgentBody()
    
    # Advanced methods
    def get_state(self):
        return {
            'x': self._world.getAgentBody().x,
            'y': self._world.getAgentBody().y,
            'orientation': self._world.getAgentBody().orientation,
            'simulation_time': self._world.getSimulationTime(),
            'width': self._world.getWidth(),
            'height': self._world.getHeight()
        }
    
    def set_state(self, state):
        body = self._world.getAgentBody()
        body.x = state['x']
        body.y = state['y']
        body.orientation = state['orientation']
    
    def add_object(self, obj):
        return self._world.addObject(obj)
    
    def remove_object(self, x, y):
        return self._world.removeObject(x, y)
    
    def is_valid_position(self, x, y):
        return self._world.isValidPosition(x, y)
```

### Utility Functions

```python
# Factory functions for common setups
def create_minimal_setup(neuron_count=100, steps=100):
    """Create a minimal simulation setup."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("simulation.max_steps", steps)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return {
        'config': config,
        'brain': brain,
        'world': None,
        'agent': None
    }

def create_advanced_setup(neuron_count=5000, steps=10000):
    """Create an advanced simulation setup with all systems."""
    config = pynlm.createAdvancedConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("simulation.max_steps", steps)
    
    brain = pynlm.createAdvancedBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Setup environment
    world.configure(30, 30, 12, 12)
    world.setAgentStart(15, 15)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return {
        'config': config,
        'brain': brain,
        'world': world,
        'agent': agent
    }

def create_research_setup(neuron_count=2000, steps=50000):
    """Create a research-grade simulation setup."""
    config = pynlm.createResearchConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("simulation.max_steps", steps)
    
    brain = pynlm.createResearchBrain(config)
    brain.initialize()
    
    return {
        'config': config,
        'brain': brain,
        'world': None,
        'agent': None
    }

# Helper functions for simulation
def run_simulation(brain, steps):
    """Run a standard simulation."""
    for step in range(steps):
        brain.step(step)
    
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'steps_completed': steps
    }

def run_advanced_simulation(brain, world, agent, steps):
    """Run an advanced simulation with world interaction."""
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'steps_completed': steps,
        'world_width': world.getWidth(),
        'world_height': world.getHeight()
    }

# Analysis functions
def analyze_simulation_results(results):
    """Analyze simulation results."""
    analysis = {
        'summary': {
            'total_spikes': sum(r.get('total_spikes', 0) for r in results),
            'average_firing_rate': sum(r.get('final_firing_rate', 0) for r in results) / len(results),
            'total_steps': sum(r.get('steps_completed', 0) for r in results)
        },
        'trends': calculate_trends(results),
        'anomalies': detect_anomalies(results),
        'recommendations': generate_recommendations(results)
    }
    
    return analysis

def calculate_trends(results):
    """Calculate trends in results."""
    # Implementation depends on specific analysis needs
    return {}

def detect_anomalies(results):
    """Detect anomalies in results."""
    anomalies = []
    # Implementation depends on specific anomaly detection logic
    return anomalies

def generate_recommendations(results):
    """Generate recommendations based on results."""
    recommendations = []
    # Implementation depends on specific recommendation logic
    return recommendations
```

## Command Line Interface

### Command Line Options

```bash
# Basic usage
./nlm --neuron_count 1000 --simulation_steps 1000

# Advanced configuration
./nlm --neuron_count 5000 --simulation_steps 10000 \
     --enable_stdp --enable_hebbian --enable_structural \
     --enable_memory --enable_neuromodulation \
     --enable_cognition --enable_development

# Performance benchmarking
./nlm --neuron_count 2000 --simulation_steps 5000 --benchmark

# Save and load state
./nlm --neuron_count 1000 --simulation_steps 2000 \
     --save_state brain_state.bin

# Load state and continue
./nlm --neuron_count 1000 --simulation_steps 2000 \
     --load_state brain_state.bin

# Advanced debugging
./nlm --neuron_count 1000 --simulation_steps 1000 \
     --verbose --debug_spikes --debug_plasticity --debug_memory

# Custom configuration
./nlm --config custom_config.json --neuron_count 3000 --simulation_steps 3000
```

### Command Line Arguments

| Argument | Description | Default |
|----------|-------------|---------|
| `--config <file>` | Load configuration from file | `configs/default.cfg` |
| `--neuron_count <n>` | Number of neurons | `1000` |
| `--simulation_steps <n>` | Number of simulation steps | `1000` |
| `--timestep <t>` | Simulation timestep in ms | `0.001` |
| `--random_seed <n>` | Random seed for reproducibility | `42` |
| `--connection_probability <p>` | Probability of connections | `0.1` |
| `--enable_stdp` | Enable STDP plasticity | `true` |
| `--enable_hebbian` | Enable Hebbian plasticity | `true` |
| `--enable_structural` | Enable structural plasticity | `true` |
| `--enable_memory` | Enable memory systems | `true` |
| `--enable_neuromodulation` | Enable neuromodulation | `true` |
| `--enable_cognition` | Enable cognitive systems | `true` |
| `--enable_development` | Enable developmental stages | `true` |
| `--verbose` | Enable verbose output | `false` |
| `--quiet` | Suppress informational messages | `false` |
| `--benchmark` | Run benchmark mode | `false` |
| `--save_state <file>` | Save brain state to file | (none) |
| `--load_state <file>` | Load brain state from file | (none) |
| `--log_file <file>` | Write logs to file | (none) |
| `--debug_spikes` | Log spike events | `false` |
| `--debug_plasticity` | Log plasticity events | `false` |
| `--debug_memory` | Log memory events | `false` |
| `--debug_neuromodulation` | Log neuromodulation events | `false` |

### Command Line Usage Examples

```bash
# Simple simulation with default settings
./nlm

# Custom configuration
./nlm --config my_config.json

# High-performance simulation
./nlm --neuron_count 5000 --simulation_steps 10000 --benchmark --performance_mode

# Research simulation with all features
./nlm --neuron_count 2000 --simulation_steps 50000 \
     --enable_stdp --enable_hebbian --enable_structural \
     --enable_memory --enable_neuromodulation \
     --enable_cognition --enable_development \
     --verbose --save_state research_state.bin

# Debugging and development
./nlm --neuron_count 100 --simulation_steps 1000 \
     --verbose --debug_spikes --debug_plasticity

# Performance testing
./nlm --neuron_count 1000 --simulation_steps 5000 --benchmark

# State management
./nlm --neuron_count 1000 --simulation_steps 2000 --save_state checkpoint1.bin
./nlm --neuron_count 1000 --simulation_steps 2000 --load_state checkpoint1.bin

# Help and information
./nlm --help
./nlm --version
```

### Command Line Script Examples

```bash
#!/bin/bash
# run_simulation.sh - Run a simulation with specified parameters

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --neuron_count)
            NEURON_COUNT="$2"
            shift 2
            ;;
        --simulation_steps)
            SIMULATION_STEPS="$2"
            shift 2
            ;;
        --benchmark)
            BENCHMARK=true
            shift
            ;;
        --save_state)
            SAVE_STATE="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [--neuron_count N] [--simulation_steps N] [--benchmark] [--save_state FILE]"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Set default values
NEURON_COUNT=${NEURON_COUNT:-1000}
SIMULATION_STEPS=${SIMULATION_STEPS:-1000}
BENCHMARK=false
SAVE_STATE=""

# Run simulation
./nlm --neuron_count $NEURON_COUNT --simulation_steps $SIMULATION_STEPS \
     $(if [ "$BENCHMARK" = true ]; then echo "--benchmark"; fi) \
     $(if [ -n "$SAVE_STATE" ]; then echo "--save_state $SAVE_STATE"; fi)

# Check result
if [ $? -eq 0 ]; then
    echo "Simulation completed successfully"
else
    echo "Simulation failed"
    exit 1
fi
```

## Configuration Files

### Configuration File Format

Configuration files should be in JSON format:

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "threshold": 0.1
        },
        "novelty": {
            "enable": true,
            "decay_rate": 0.1
        }
    },
    "memory": {
        "working_memory": {
            "enable": true,
            "capacity": 100,
            "decay_rate": 0.01
        },
        "episodic_memory": {
            "enable": true,
            "max_episodes": 1000,
            "consolidation_threshold": 0.8
        },
        "associative_memory": {
            "enable": true,
            "association_strength": 0.5
        }
    },
    "cognition": {
        "planning": {
            "enable": true,
            "depth": 5
        },
        "concept": {
            "enable": true,
            "learning_rate": 0.01
        },
        "attention": {
            "enable": true,
            "inhibition_strength": 0.5,
            "excitation_strength": 1.5
        }
    },
    "development": {
        "enable": true,
        "stage": "Adult",
        "plasticity_modulation": true,
        "adaptation_rate": 0.01
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000
    }
}
```

### Example Configuration Files

#### Minimal Configuration (`minimal_config.json`)
```json
{
    "brain": {
        "neuron_count": 100,
        "synapse_density": 0.05
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 100
    }
}
```

#### Standard Configuration (`standard_config.json`)
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000
    }
}
```

#### Advanced Configuration (`advanced_config.json`)
```json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.15,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "memory": {
        "working_memory": {
            "enable": true,
            "capacity": 500,
            "decay_rate": 0.01
        },
        "episodic_memory": {
            "enable": true,
            "max_episodes": 5000,
            "consolidation_threshold": 0.8
        },
        "associative_memory": {
            "enable": true,
            "association_strength": 0.5
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "threshold": 0.1
        },
        "novelty": {
            "enable": true,
            "decay_rate": 0.1
        }
    },
    "cognition": {
        "planning": {
            "enable": true,
            "depth": 5
        },
        "concept": {
            "enable": true,
            "learning_rate": 0.01
        },
        "attention": {
            "enable": true,
            "inhibition_strength": 0.5,
            "excitation_strength": 1.5
        }
    },
    "development": {
        "enable": true,
        "stage": "Adult",
        "plasticity_modulation": true,
        "adaptation_rate": 0.01
    },
    "simulation": {
        "timestep": 0.0005,
        "max_steps": 10000
    }
}
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Simulation Fails to Initialize

**Symptoms:**
- Error messages during brain initialization
- Program exits with error code
- No output or incomplete output

**Solutions:**

1. **Check Configuration**
   - Verify configuration file exists
   - Check for valid JSON format
   - Validate parameter values
   ```python
   import pynlm
   config = pynlm.createDefaultConfig()
   if not config.has("brain.neuron_count"):
       config.set("brain.neuron_count", 1000)
   ```

2. **Check Dependencies**
   - Ensure all required libraries are installed
   - Verify system resources (memory, CPU)
   - Check for conflicting processes

3. **Validate Brain Initialization**
   ```python
   config = pynlm.createAdvancedConfig()
   brain = pynlm.createAdvancedBrain(config)
   
   if not brain.initialize():
       print("Brain initialization failed")
       # Check error logs
       brain.logStatus()
   ```

#### Issue: Simulation Runs Too Slowly

**Symptoms:**
- High CPU usage
- Long execution time
- Performance bottlenecks

**Solutions:**

1. **Optimize Configuration**
   ```python
   # Use performance-optimized configuration
   config = pynlm.createAdvancedConfig()
   config.set("brain.neuron_count", 2000)  # Reduce neuron count
   config.set("memory.episodic_memory.enable", False)  # Disable episodic memory
   config.set("cognition.enable", False)  # Disable cognition
   config.set("development.enable", False)  # Disable development
   config.set("simulation.timestep", 0.001)  # Optimal timestep
   ```

2. **Enable Parallel Processing**
   ```python
   config.set("parallel.enable", True)
   config.set("parallel.threads", 8)
   ```

3. **Use Performance Monitoring**
   ```python
   import time
   start_time = time.time()
   # Run simulation
   end_time = time.time()
   
   duration = end_time - start_time
   steps_per_second = steps / duration
   
   print(f"Performance: {steps_per_second:.2f} steps/sec")
   ```

#### Issue: Memory Usage Too High

**Symptoms:**
- High memory consumption
- Memory leaks
- System slowdown

**Solutions:**

1. **Configure Memory Limits**
   ```python
   config.set("memory.working_memory.capacity", 100)
   config.set("memory.episodic_memory.max_episodes", 100)
   config.set("memory.associative_memory.enable", False)
   ```

2. **Enable Memory Monitoring**
   ```python
   def monitor_memory_usage(brain):
       status = brain.getNeuralActivityStats()
       memory_stats = brain.getWorkingMemoryStats()
       
       print(f"Memory usage: {memory_stats.get('total_memory_mb', 0)} MB")
       print(f"Active traces: {memory_stats.get('active_traces', 0)}")
   ```

3. **Enable Memory Cleanup**
   ```python
   brain.getWorkingMemory().clear()
   brain.getEpisodicMemory().clear()
   brain.getAssociativeMemory().clear()
   ```

#### Issue: Configuration File Errors

**Symptoms:**
- Invalid JSON format
- Missing required fields
- Invalid parameter values

**Solutions:**

1. **Validate Configuration**
   ```python
   def validate_config(config):
       errors = []
       warnings = []
       
       # Check required fields
       if not config.has("brain.neuron_count"):
           errors.append("Missing brain.neuron_count")
       
       # Validate field values
       neuron_count = config.get("brain.neuron_count", 0)
       if neuron_count <= 0:
           errors.append("brain.neuron_count must be positive")
       
       return errors, warnings
   
   errors, warnings = validate_config(config)
   if errors:
       print("Configuration errors:")
       for error in errors:
           print(f"  - {error}")
   ```

2. **Use Configuration Templates**
   ```python
   # Get default configuration template
   template = pynlm.createDefaultConfig()
   
   # Create configuration from template
   config = template
   config.set("brain.neuron_count", 1000)
   ```

#### Issue: Command Line Interface Errors

**Symptoms:**
- Unknown options
- Missing required arguments
- Incorrect syntax

**Solutions:**

1. **Check Command Line Syntax**
   ```bash
   # Check available options
   ./nlm --help
   
   # Check version
   ./nlm --version
   ```

2. **Use Correct Syntax**
   ```bash
   # Correct syntax
   ./nlm --neuron_count 1000 --simulation_steps 1000
   
   ./nlm --config my_config.json
   ```

3. **Debug Command Line Arguments**
   ```python
   import sys
   print("Command line arguments:", sys.argv)
   print("Program name:", sys.argv[0])
   
   # Parse arguments
   for i, arg in enumerate(sys.argv):
       print(f"Argument {i}: {arg}")
   ```

#### Issue: Python API Errors

**Symptoms:**
- Import errors
- Attribute errors
- Type errors

**Solutions:**

1. **Check Python Installation**
   ```bash
   # Check Python version
   python --version
   
   # Check pip version
   pip --version
   
   # Check installed packages
   pip list | grep pynlm
   ```

2. **Install or Update NLM**
   ```bash
   # Install in development mode
   pip install -e .
   
   # Update to latest version
   pip install --upgrade pynlm
   ```

3. **Check Python API Usage**
   ```python
   import pynlm
   
   # Check API documentation
   help(pynlm)
   
   # Check available functions
   dir(pynlm)
   ```

### Error Handling Best Practices

1. **Always Validate Input**
   ```python
   def safe_operation(brain, steps):
       if not brain or steps <= 0:
           raise ValueError("Invalid parameters")
       
       # Perform operation
       for step in range(steps):
           brain.step(step)
   ```

2. **Use Try-Catch Blocks**
   ```python
   try:
       result = brain.step(step)
   except Exception as e:
       print(f"Error during simulation: {e}")
       # Handle error or exit gracefully
       sys.exit(1)
   ```

3. **Provide Meaningful Error Messages**
   ```python
   def operation_that_fails():
       try:
           # Some operation
           pass
       except ValueError as e:
           raise ValueError(f"Operation failed: {e}") from e
       except RuntimeError as e:
           raise RuntimeError(f"System error: {e}") from e
   ```

4. **Log Errors**
   ```python
   import logging
   
   # Configure logging
   logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
   
   # Log errors
   logging.error(f"Simulation failed: {error}")
   ```

### Performance Monitoring

1. **Monitor System Resources**
   ```python
   import psutil
   
   def monitor_resources():
       cpu_percent = psutil.cpu_percent()
       memory = psutil.virtual_memory()
       
       print(f"CPU: {cpu_percent}%")
       print(f"Memory: {memory.percent}%")
       
       if cpu_percent > 90:
           print("Warning: High CPU usage")
       
       if memory.percent > 90:
           print("Warning: High memory usage")
   ```

2. **Monitor Simulation Progress**
   ```python
   def monitor_simulation(brain, total_steps):
       import time
       start_time = time.time()
       
       for step in range(total_steps):
           brain.step(step)
           
           if step % 100 == 0:
               elapsed = time.time() - start_time
               steps_per_second = step / elapsed
               
               print(f"Step {step}/{total_steps} ({steps_per_second:.2f} steps/sec)")
               
               # Check for performance issues
               if steps_per_second < 10:
                   print("Warning: Low performance detected")
   ```

3. **Benchmark Performance**
   ```python
   def benchmark_performance(brain, steps):
       import time
       start_time = time.time()
       
       for step in range(steps):
           brain.step(step)
       
       end_time = time.time()
       duration = end_time - start_time
       steps_per_second = steps / duration
       
       return {
           'steps': steps,
           'duration_ms': duration * 1000,
           'steps_per_second': steps_per_second,
           'performance_rating': 'good' if steps_per_second > 100 else 'fair' if steps_per_second > 10 else 'poor'
       }
   ```

## Conclusion

The enhanced NLM framework provides a comprehensive toolkit for neural simulation with advanced features for experienced users. This documentation covers:

1. **Core functionality** - Basic simulation and configuration
2. **Advanced features** - Memory systems, neuromodulation, cognition
3. **Performance optimization** - Parallel processing, memory management
4. **Research tools** - Experiment management, data analysis
5. **Error handling** - Robust error detection and recovery
6. **Command line interface** - Advanced options and scripting
7. **Configuration management** - Templates and validation

The framework is designed to be flexible and extensible, allowing researchers and developers to create sophisticated neural simulations while maintaining performance and reliability.

## References

1. [NLM Documentation](https://nlm.readthedocs.io)
2. [GitHub Repository](https://github.com/nlm-project/nlm)
3. [Python API Reference](https://nlm.readthedocs.io/en/latest/api/python.html)
4. [C++ API Reference](https://nlm.readthedocs.io/en/latest/api/cpp.html)

## Acknowledgements

This enhanced documentation and API improvements were developed as part of the NLM Phase 6 (Final Integration) project, building upon the existing NLM framework with advanced features for research and development.