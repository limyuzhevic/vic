# NLM - Advanced Usage Guide

## Command Line Interface for NLM

This guide provides advanced usage patterns for the NLM (Neural Learning Machine) framework, including command-line tools and advanced Python programming techniques.

## 1. Basic Command Line Operations

### Brain Management Commands

```bash
# Create a new brain with custom configuration
nlm create-brain --neurons 2000 --regions 2 --timestep 0.001

# Load a saved brain state
nlm load-brain brain_checkpoint.bin

# Save current brain state
nlm save-brain --output brain_checkpoint.bin --format binary

# Reset brain to initial state
nlm reset-brain

# Get brain statistics
nlm stats --verbose
```

### Experiment Management Commands

```bash
# Run a predefined experiment
nlm run-experiment --type phase6 --neurons 1000 --steps 5000

# Create custom experiment configuration
nlm create-experiment --name my_experiment --config experiment_config.yaml

# List available experiments
nlm list-experiments

# Export experiment results
nlm export-results --format json --output results.json
```

### Configuration Commands

```bash
# Show current configuration
nlm show-config --section brain

# Set configuration values
nlm set-config brain.neuron_count 5000
nlm set-config plasticity.stdp.enable true

# Load configuration from file
nlm load-config configs/production.yaml

# Validate configuration
nlm validate-config
```

## 2. Advanced Brain Configuration

### Custom Brain Templates

```python
import pynlm
from pynlm import createBrain, createDefaultConfig

# Create specialized brain configurations

def create_large_brain():
    """Create a large-scale brain with many regions"""
    config = createDefaultConfig()
    config.set("brain.neuron_count", 10000)
    config.set("brain.region_count", 5)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    brain = createBrain(config)
    brain.initialize()
    return brain

def create_developmental_brain():
    """Create a brain with developmental stages"""
    config = createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("development.enable", True)
    config.set("development.critical_period", 1000)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = createBrain(config)
    brain.initialize()
    return brain

# Usage examples
large_brain = create_large_brain()
developmental_brain = create_developmental_brain()
```

### Advanced Configuration API

```python
import pynlm

class AdvancedBrainManager:
    def __init__(self):
        self.brain_configs = {}
    
    def register_brain_template(self, name, config_dict):
        """Register a custom brain template"""
        self.brain_configs[name] = config_dict
    
    def create_brain_from_template(self, name, overrides=None):
        """Create brain from template with optional overrides"""
        base_config = self.brain_configs[name].copy()
        if overrides:
            base_config.update(overrides)
        
        config = pynlm.createDefaultConfig()
        for key, value in base_config.items():
            config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    def batch_brain_creation(self, count, **kwargs):
        """Create multiple brains with variations"""
        brains = []
        for i in range(count):
            config_overrides = kwargs.copy()
            config_overrides['brain.neuron_count'] = kwargs.get('neuron_count', 1000) + i * 100
            config_overrides['random_seed'] = i  # Different seed for each brain
            
            brain = self.create_brain_from_template('default', config_overrides)
            brains.append(brain)
        return brains

# Register advanced templates
brain_manager = AdvancedBrainManager()
brain_manager.register_brain_template('default', {
    'brain.neuron_count': 1000,
    'brain.region_count': 1,
    'plasticity.stdp.enable': True,
    'neuromod.curiosity.enable': False
})

brain_manager.register_brain_template('research', {
    'brain.neuron_count': 5000,
    'brain.region_count': 3,
    'plasticity.stdp.enable': True,
    'plasticity.hebbian.enable': True,
    'plasticity.structural.enable': True,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True,
    'development.enable': True
})
```

## 3. Advanced World Simulation

### Complex World Environments

```python
import pynlm

class AdvancedWorldSimulator:
    def __init__(self):
        self.world_configs = {}
    
    def create_complex_world(self, width, height, vision_range, agent_start=None):
        """Create a complex 2D world with multiple object types"""
        world = pynlm.createSimpleWorld()
        world.configure(
            width=width,
            height=height,
            visionWidth=vision_range,
            visionHeight=vision_range
        )
        
        if agent_start:
            world.setAgentStart(agent_start[0], agent_start[1])
        
        # Add complex environment objects
        self._populate_environment(world, width, height)
        
        return world
    
    def _populate_environment(self, world, width, height):
        """Populate world with various object types"""
        # Add resources
        world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
        world.addObject(pynlm.WorldObject(15.0, 10.0, pynlm.WorldObjectType.Resource, value=20.0))
        
        # Add hazards
        world.addObject(pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, value=-5.0))
        
        # Add walls
        for x in range(0, width, 2):
            world.addObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall, value=0.0))
        
        # Add markers
        world.addObject(pynlm.WorldObject(3.0, 3.0, pynlm.WorldObjectType.Marker, value=1.0))
    
    def simulate_agent_lifespan(self, brain, world, agent, steps_per_episode, num_episodes):
        """Simulate multiple agent lifespans with memory and learning"""
        episode_rewards = []
        
        for episode in range(num_episodes):
            world.reset()
            agent.initialize(world)
            
            episode_reward = 0.0
            for step in range(steps_per_episode):
                # Update world
                world.update(0.1)
                
                # Get perception
                percept = world.getSensoryPercept()
                
                # Process in brain
                agent.processSensoryInput(percept)
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get reward
                reward = self._calculate_reward(world, agent, brain)
                agent.applyRewardModulation(reward, 0.0)
                
                episode_reward += reward
            
            episode_rewards.append(episode_reward)
            print(f"Episode {episode + 1}: Reward = {episode_reward:.2f}")
        
        return episode_rewards

# Advanced usage example
simulator = AdvancedWorldSimulator()

# Create a complex world
world = simulator.create_complex_world(20, 20, 8, (10.0, 10.0))

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
simulator.simulate_agent_lifespan(brain, world, agent, 100, 10)
```

## 4. Advanced Memory and Learning

### Working Memory Management

```python
import pynlm

class MemoryManager:
    def __init__(self, brain):
        self.brain = brain
        self.working_memory = brain.getWorkingMemory()
        self.episodic_memory = brain.getEpisodicMemory()
    
    def store_and_retrieve(self, neuron_id, activation_strength):
        """Store neural activation in working memory"""
        self.working_memory.storeToNeuron(neuron_id, activation_strength)
        # Retrieve most important traces
        return self.working_memory.getMemoryNeurons()
    
    def create_memory_episode(self, current_step, reward):
        """Create an episodic memory from current brain state"""
        episode = pynlm.EpisodicMemoryItem()
        episode.timestamp = current_step
        episode.reward = reward
        
        # Store active neurons
        active_neurons = self.brain.getActiveNeuronCount()
        episode.activeNeurons = list(range(active_neurons))
        
        # Store neuron activations
        for neuron in self.working_memory.getMemoryNeurons():
            episode.neuronActivations.append(neuron.getActivation())
        
        self.episodic_memory.storeEpisode(episode)
        return episode
    
    def replay_important_memories(self, count=5):
        """Replay important memories to reinforce learning"""
        episodes = self.episodic_memory.getEpisodesForReplay(count)
        for episode in episodes:
            self.episodic_memory.replayEpisode(episode)
        return len(episodes)

# Memory management example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

memory_manager = MemoryManager(brain)

# Store and retrieve working memory
stored_neuron_id = memory_manager.store_and_retrieve(1, 0.8)

# Create episodic memory
episode = memory_manager.create_memory_episode(100, 5.0)

# Replay memories
replayed_count = memory_manager.replay_important_memories()
```

### Neuromodulation Control

```python
import pynlm

class NeuromodulationController:
    def __init__(self, brain):
        self.brain = brain
        self.dopamine = brain.getDopamine()
        self.curiosity = brain.getCuriosity()
        self.novelty = brain.getNovelty()
        self.prediction_error = brain.getPredictionErrorSignal()
    
    def adjust_dopamine_levels(self, reward_prediction_error):
        """Adjust dopamine based on reward prediction error"""
        # Positive RPE increases dopamine (reinforcement)
        # Negative RPE decreases dopamine (punishment)
        self.dopamine.setLevel(abs(reward_prediction_error))
        self.dopamine.setPlasticityFactor(1.0 + reward_prediction_error * 0.1)
        
        return self.dopamine.getLevel()
    
    def drive_curiosity_exploration(self, current_state, novel_state):
        """Drive exploration based on novelty detection"""
        novelty_score = self.novelty.calculateNovelty(current_state, novel_state)
        
        # High novelty increases curiosity
        if novelty_score > 0.7:
            curiosity_level = self.curiosity.getLevel() + 0.1
            self.curiosity.setLevel(min(curiosity_level, 1.0))
        
        return novelty_score
    
    def modulate_plasticity(self, neuromodulation_level):
        """Modulate synaptic plasticity based on neuromodulation"""
        # Higher neuromodulation increases plasticity
        plasticity_factor = 1.0 + neuromodulation_level * 0.5
        
        # Apply to STDP
        stdp = self.brain.getSTDP()
        stdp.setLearningRate(stdp.getLearningRate() * plasticity_factor)
        
        return plasticity_factor

# Neuromodulation controller example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

controller = NeuromodulationController(brain)

# Adjust dopamine levels
reward_error = 2.5
dopamine_level = controller.adjust_dopamine_levels(reward_error)

# Drive curiosity
novelty_score = controller.drive_curiosity_exploration([1.0, 2.0], [3.0, 4.0])

# Modulate plasticity
plasticity_factor = controller.modulate_plasticity(dopamine_level)
```

## 5. Performance Optimization and Monitoring

### Brain Performance Monitoring

```python
import pynlm
import time

class BrainPerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.performance_data = []
        self.monitoring = False
    
    def start_monitoring(self, interval=1.0, duration=None):
        """Start performance monitoring"""
        self.monitoring = True
        start_time = time.time()
        
        while self.monitoring:
            self._collect_metrics()
            time.sleep(interval)
            
            if duration and (time.time() - start_time) > duration:
                break
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.monitoring = False
    
    def _collect_metrics(self):
        """Collect current brain performance metrics"""
        metrics = {
            'timestamp': time.time(),
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'ei_ratio': self.brain.getExcitationInhibitionRatio(),
            'memory_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'episodes': self.brain.getEpisodicMemory().getEpisodeCount() if self.brain.getEpisodicMemory() else 0
        }
        
        self.performance_data.append(metrics)
        return metrics
    
    def analyze_performance(self):
        """Analyze collected performance data"""
        if not self.performance_data:
            return None
        
        analysis = {
            'total_samples': len(self.performance_data),
            'average_firing_rate': sum(m['average_firing_rate'] for m in self.performance_data) / len(self.performance_data),
            'max_active_neurons': max(m['active_neurons'] for m in self.performance_data),
            'avg_memory_usage': sum(m['memory_traces'] for m in self.performance_data) / len(self.performance_data),
            'performance_trends': self._calculate_trends()
        }
        
        return analysis
    
    def _calculate_trends(self):
        """Calculate performance trends over time"""
        if len(self.performance_data) < 2:
            return {}
        
        rates = [m['average_firing_rate'] for m in self.performance_data]
        
        # Simple linear trend calculation
        n = len(rates)
        x = list(range(n))
        avg_x = sum(x) / n
        avg_y = sum(rates) / n
        
        numerator = sum((xi - avg_x) * (yi - avg_y) for xi, yi in zip(x, rates))
        denominator = sum((xi - avg_x) ** 2 for xi in x)
        
        slope = numerator / denominator if denominator != 0 else 0
        
        return {
            'firing_rate_trend': 'increasing' if slope > 0.01 else 'decreasing' if slope < -0.01 else 'stable',
            'trend_slope': slope
        }

# Performance monitoring example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

monitor = BrainPerformanceMonitor(brain)

# Start monitoring
monitor.start_monitoring(interval=0.5, duration=10.0)

# Simulate some work
for i in range(100):
    brain.step(i)

# Stop monitoring
monitor.stop_monitoring()

# Analyze performance
performance_analysis = monitor.analyze_performance()
print(f"Performance Analysis: {performance_analysis}")
```

### Checkpoint Management

```python
import pynlm
import time

class CheckpointManager:
    def __init__(self, brain, checkpoint_dir="./checkpoints"):
        self.brain = brain
        self.checkpoint_dir = checkpoint_dir
        self.checkpoint_interval = 1000
        self.last_checkpoint = 0
    
    def maybe_checkpoint(self, current_step):
        """Create checkpoint at specified intervals"""
        if current_step % self.checkpoint_interval == 0 and current_step > 0:
            filename = f"{self.checkpoint_dir}/checkpoint_step_{current_step}.bin"
            if self.brain.save(filename):
                self.last_checkpoint = current_step
                print(f"Checkpoint saved: {filename}")
                return True
        return False
    
    def load_latest_checkpoint(self):
        """Load the most recent checkpoint"""
        # Implementation would find and load the latest checkpoint
        pass
    
    def cleanup_old_checkpoints(self, keep_last=10):
        """Remove old checkpoints, keeping only the last N"""
        # Implementation would clean up old checkpoints
        pass

# Checkpoint manager example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

checkpoint_manager = CheckpointManager(brain, "./my_checkpoints")

# Run simulation with checkpointing
for step in range(5000):
    brain.step(step)
    checkpoint_manager.maybe_checkpoint(step)
```

## 6. Advanced Experiment Framework

### Custom Experiment Definitions

```python
import pynlm
import json

class AdvancedExperimentFramework:
    def __init__(self):
        self.experiment_templates = {}
    
    def define_experiment(self, name, config, results_processor=None):
        """Define a custom experiment"""
        self.experiment_templates[name] = {
            'config': config,
            'results_processor': results_processor
        }
    
    def run_experiment(self, name, brain=None):
        """Run a defined experiment"""
        if name not in self.experiment_templates:
            raise ValueError(f"Experiment '{name}' not defined")
        
        template = self.experiment_templates[name]
        config = template['config']
        processor = template['results_processor']
        
        # Create brain if not provided
        if brain is None:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
        
        # Modify config if needed
        for key, value in config.items():
            if '.' in key:
                # Nested key handling
                parts = key.split('.')
                # Apply config modifications based on experiment requirements
        
        # Run experiment
        return self._execute_experiment(brain, config, processor)
    
    def _execute_experiment(self, brain, config, processor):
        """Execute the actual experiment"""
        results = {
            'total_reward': 0.0,
            'avg_firing_rate': 0.0,
            'memory_episodes_stored': 0,
            'dopamine_level': 0.0,
            'working_memory_integrated': False,
            'episodic_memory_integrated': False,
            'neuromodulation_integrated': False,
            'prediction_integrated': False,
            'development_integrated': False,
            'checkpointing_works': False,
            'total_wall_clock_time': 0.0
        }
        
        # Implementation would run the experiment with the provided brain
        # and return results
        
        return results

# Advanced experiment framework usage
framework = AdvancedExperimentFramework()

# Define Phase 6 integration experiment
phase6_config = {
    'neuron_count': 1000,
    'max_steps': 2000,
    'enable_checkpointing': True,
    'enable_replay': True,
    'enable_development': True,
    'memory_capacity': 500,
    'novelty_threshold': 0.5
}

framework.define_experiment('phase6_integration', phase6_config)

# Define developmental experiment
development_config = {
    'neuron_count': 500,
    'max_steps': 5000,
    'enable_development': True,
    'developmental_stages': ['initial', 'critical_period', 'maturation', 'adult'],
    'plasticity_decay_rate': 0.1
}

framework.define_experiment('developmental_learning', development_config)

# Run experiments
results1 = framework.run_experiment('phase6_integration')
results2 = framework.run_experiment('developmental_learning')
```

## 7. Batch Processing and Parallel Execution

### Parallel Brain Simulation

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import time

class ParallelBrainSimulator:
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or 1
        self.executor = None
    
    def __enter__(self):
        if self.num_workers > 1:
            self.executor = ProcessPoolExecutor(max_workers=self.num_workers)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.executor:
            self.executor.shutdown(wait=True)
    
    def create_brain_configuration(self, base_config, variations):
        """Create multiple brain configurations with variations"""
        configs = []
        
        for variation in variations:
            config = pynlm.createDefaultConfig()
            
            # Apply base config
            for key, value in base_config.items():
                config.set(key, value)
            
            # Apply variation
            for key, value in variation.items():
                config.set(key, value)
            
            configs.append(config)
        
        return configs
    
    def simulate_single_brain(self, config_index, config_file, experiment_name):
        """Simulate a single brain (worker function)"""
        import pynlm
        
        # Load configuration from file
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
        
        # Apply experiment-specific overrides
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run experiment
        start_time = time.time()
        
        for step in range(config.getOr<size_t>('max_steps', 1000)):
            brain.step(step)
        
        end_time = time.time()
        
        return {
            'config_index': config_index,
            'experiment_name': experiment_name,
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'execution_time': end_time - start_time,
            'memory_integrated': brain.getWorkingMemory() is not None,
            'final_reward': brain.getDopamine().getLevel() if brain.getDopamine() else 0.0
        }
    
    def batch_simulation(self, config_files, experiment_name, use_processes=False):
        """Run batch simulation across multiple configurations"""
        if use_processes and self.num_workers > 1:
            return self._parallel_batch_simulation(config_files, experiment_name)
        else:
            return self._sequential_batch_simulation(config_files, experiment_name)
    
    def _parallel_batch_simulation(self, config_files, experiment_name):
        """Run batch simulation in parallel using processes"""
        futures = []
        
        for i, config_file in enumerate(config_files):
            future = self.executor.submit(
                self.simulate_single_brain, i, config_file, experiment_name
            )
            futures.append(future)
        
        # Collect results
        results = []
        for future in futures:
            results.append(future.result())
        
        return results
    
    def _sequential_batch_simulation(self, config_files, experiment_name):
        """Run batch simulation sequentially"""
        results = []
        
        for i, config_file in enumerate(config_files):
            result = self.simulate_single_brain(i, config_file, experiment_name)
            results.append(result)
        
        return results

# Parallel simulation example
with ParallelBrainSimulator(num_workers=4) as simulator:
    # Create configuration files
    config_files = []
    for i in range(10):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000 + i * 100)
        config.set("max_steps", 1000)
        filename = f"config_{i}.yaml"
        config.saveToFile(filename)
        config_files.append(filename)
    
    # Run batch simulation
    results = simulator.batch_simulation(config_files, 'neurally_complex', use_processes=True)
    
    # Analyze results
    total_spikes = sum(r['total_spikes'] for r in results)
    avg_firing_rate = sum(r['average_firing_rate'] for r in results) / len(results)
    total_time = sum(r['execution_time'] for r in results)
    
    print(f"Batch simulation results:")
    print(f"  Total spikes across all brains: {total_spikes}")
    print(f"  Average firing rate: {avg_firing_rate:.2f}")
    print(f"  Total execution time: {total_time:.2f}s")
    print(f"  Average time per brain: {total_time/len(results):.2f}s")
```

## 8. Command Line Advanced Usage

### Complex Command Examples

```bash
# Run multiple experiments in parallel with different configurations
nlm batch-run \
  --experiments phase6,developmental,curiosity \
  --config templates/*.yaml \
  --workers 4 \
  --output-dir results/parallel

# Create a custom simulation pipeline
nlm pipeline \
  --create brain_init \
  --create world_config \
  --create agent_setup \
  --create simulation_loop \
  --save pipeline.json

# Export brain for analysis
nlm export-brain \
  --format neuromorpho \
  --coordinates --connectivity \
  --input brain.bin \
  --output brain_neuromorpho.swc

# Performance profiling
nlm profile \
  --target brain \
  --measurements spikes,firing_rate,memory \
  --duration 60s \
  --output profiling_data.csv

# Configuration comparison
nlm compare-configs \
  --configs baseline.yaml,optimized.yaml,test.yaml \
  --metrics performance,memory,accuracy \
  --output comparison_report.html
```

## 9. Extending the Framework

### Adding Custom Components

```python
import pynlm

# Example of extending the framework with custom components

class CustomPlasticityRule:
    def __init__(self, params):
        self.params = params
    
    def apply(self, synapse, pre_spike_time, post_spike_time):
        # Custom plasticity rule implementation
        time_diff = post_spike_time - pre_spike_time
        
        if abs(time_diff) < 20:  # Within 20ms window
            if time_diff > 0:  # Post after pre -> LTP
                weight_change = self.params['LTP_rate'] * time_diff
            else:  # Post before pre -> LTD
                weight_change = -self.params['LTD_rate'] * abs(time_diff)
            
            return weight_change
        
        return 0.0

class CustomWorld:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.objects = []
    
    def add_custom_object(self, obj_type, position, properties):
        """Add custom world object"""
        custom_obj = {
            'type': obj_type,
            'position': position,
            'properties': properties,
            'last_update': time.time()
        }
        self.objects.append(custom_obj)
        return len(self.objects) - 1
    
    def update(self, timestep):
        """Update custom world dynamics"""
        # Custom world dynamics implementation
        for obj in self.objects:
            self._update_object(obj, timestep)
    
    def _update_object(self, obj, timestep):
        """Update individual object state"""
        # Custom object update logic
        pass

# Usage example
custom_plasticity = CustomPlasticityRule({
    'LTP_rate': 0.01,
    'LTD_rate': 0.012
})

custom_world = CustomWorld(20, 20)
custom_world.add_custom_object('food', (10.0, 10.0), {'nutritional_value': 5.0})
```

## 10. Integration with External Tools

### Data Analysis and Visualization

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

class AdvancedAnalysisTools:
    def __init__(self, brain):
        self.brain = brain
    
    def spike_analysis(self, duration_ms=1000):
        """Analyze spike patterns and statistics"""
        spike_history = []
        
        for neuron in self.brain.getAllNeurons():
            history = neuron.getSpikeHistory()
            spike_history.extend(history)
        
        if not spike_history:
            return None
        
        # Calculate statistics
        spike_times = sorted(spike_history)
        inter_spike_intervals = np.diff(spike_times)
        
        analysis = {
            'total_spikes': len(spike_times),
            'mean_isi': np.mean(inter_spike_intervals) if len(inter_spike_intervals) > 0 else 0,
            'std_isi': np.std(inter_spike_intervals) if len(inter_spike_intervals) > 0 else 0,
            'firing_rate_hz': len(spike_times) / (duration_ms / 1000),
            'spike_time_histogram': self._create_spike_histogram(spike_times)
        }
        
        return analysis
    
    def _create_spike_histogram(self, spike_times, bin_width=10):
        """Create histogram of spike times"""
        if not spike_times:
            return []
        
        max_time = max(spike_times)
        bins = np.arange(0, max_time + bin_width, bin_width)
        hist, _ = np.histogram(spike_times, bins=bins)
        
        return list(zip(bins[:-1], bins[1:], hist))
    
    def connectivity_analysis(self):
        """Analyze network connectivity patterns"""
        total_synapses = self.brain.getTotalSynapseCount()
        
        if total_synapses == 0:
            return None
        
        # Analyze by region
        region_stats = {}
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                neurons = region.getAllNeurons()
                synapses = region.getAllSynapses()
                
                region_stats[region_id.index()] = {
                    'neuron_count': len(neurons),
                    'synapse_count': len(synapses),
                    'synapse_density': len(synapses) / max(1, len(neurons)),
                    'average_weight': self._average_synapse_weight(synapses)
                }
        
        return region_stats
    
    def _average_synapse_weight(self, synapses):
        """Calculate average synaptic weight"""
        if not synapses:
            return 0.0
        
        total_weight = sum(syn.getWeight() for syn in synapses)
        return total_weight / len(synapses)

# Advanced analysis example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate some activity
for step in range(1000):
    brain.step(step)

analyzer = AdvancedAnalysisTools(brain)

# Perform spike analysis
spike_analysis = analyzer.spike_analysis()
if spike_analysis:
    print(f"Spike Analysis:")
    print(f"  Total spikes: {spike_analysis['total_spikes']}")
    print(f"  Mean ISI: {spike_analysis['mean_isi']:.2f}ms")
    print(f"  Firing rate: {spike_analysis['firing_rate_hz']:.2f} Hz")

# Perform connectivity analysis
connectivity = analyzer.connectivity_analysis()
if connectivity:
    print(f"\nConnectivity Analysis:")
    for region_id, stats in connectivity.items():
        print(f"  Region {region_id}: {stats['neuron_count']} neurons, "
              f"{stats['synapse_count']} synapses, "
              f"density: {stats['synapse_density']:.3f}")
```

## 11. Error Handling and Debugging

### Advanced Error Recovery

```python
import pynlm
import traceback

class AdvancedErrorHandler:
    def __init__(self):
        self.error_history = []
        self.recovery_strategies = {}
    
    def register_recovery_strategy(self, error_type, strategy):
        """Register a recovery strategy for specific error types"""
        self.recovery_strategies[error_type] = strategy
    
    def handle_simulation_error(self, brain, error, step):
        """Handle errors during simulation with recovery"""
        error_info = {
            'type': type(error).__name__,
            'message': str(error),
            'step': step,
            'timestamp': time.time(),
            'stack_trace': traceback.format_exc()
        }
        
        self.error_history.append(error_info)
        
        # Apply recovery strategy if available
        if type(error).__name__ in self.recovery_strategies:
            strategy = self.recovery_strategies[type(error).__name__]
            return strategy(brain, error, step)
        
        # Default recovery: log and continue
        print(f"Error at step {step}: {error}")
        return False
    
    def validate_brain_state(self, brain):
        """Validate brain state and detect potential issues"""
        issues = []
        
        # Check for abnormal values
        if brain.getTotalNeuronCount() == 0:
            issues.append("No neurons in brain")
        
        if brain.getTotalSynapseCount() == 0 and brain.getTotalNeuronCount() > 0:
            issues.append("No synapses but neurons present")
        
        # Check for NaN or infinite values
        firing_rate = brain.getAverageFiringRate()
        if np.isnan(firing_rate) or np.isinf(firing_rate):
            issues.append("Abnormal firing rate detected")
        
        return issues

# Advanced error handling example
error_handler = AdvancedErrorHandler()

# Register recovery strategies
error_handler.register_recovery_strategy('ValueError', lambda brain, error, step: brain.reset())
error_handler.register_recovery_strategy('RuntimeError', lambda brain, error, step: print(f"Critical error, saving checkpoint: {step}"))

# Example usage with error handling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

try:
    for step in range(1000):
        # Validate state before each step
        issues = error_handler.validate_brain_state(brain)
        if issues:
            print(f"Validation issues: {issues}")
        
        # Perform simulation step with error handling
        try:
            brain.step(step)
        except Exception as e:
            if not error_handler.handle_simulation_error(brain, e, step):
                print(f"Unrecoverable error at step {step}: {e}")
                break
                
except KeyboardInterrupt:
    print("Simulation interrupted by user")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## 12. Performance Profiling Tools

### Detailed Performance Analysis

```python
import pynlm
import time
import cProfile
import pstats
from io import StringIO

class PerformanceProfiler:
    def __init__(self, brain):
        self.brain = brain
        self.profiling_data = {}
    
    def profile_neuron_operations(self, num_steps=100):
        """Profile individual neuron operations"""
        profiler = cProfile.Profile()
        
        profiler.enable()
        
        for step in range(num_steps):
            # Profile neuron stepping
            for region in self.brain.getRegions():
                for population in region.getPopulations():
                    for neuron in population.getNeurons():
                        neuron.step(step)
        
        profiler.disable()
        
        # Get profiling statistics
        stream = StringIO()
        stats = pstats.Stats(profiler, stream=stream)
        stats.sort_stats('cumulative')
        stats.print_stats()
        
        return stream.getvalue()
    
    def profile_spike_system(self, num_steps=1000):
        """Profile spike system performance"""
        start_time = time.time()
        
        for step in range(num_steps):
            # Trigger spike system operations
            self.brain.getSpikeSystem().processSpikes(step)
            self.brain.getSpikeSystem().processDelayedSpikes(step, step * 0.001)
        
        end_time = time.time()
        
        return {
            'total_time': end_time - start_time,
            'steps_per_second': num_steps / (end_time - start_time),
            'avg_step_time': (end_time - start_time) / num_steps
        }
    
    def profile_memory_systems(self):
        """Profile memory system performance"""
        working_memory = self.brain.getWorkingMemory()
        episodic_memory = self.brain.getEpisodicMemory()
        
        # Profile working memory operations
        if working_memory:
            wm_start = time.time()
            for i in range(1000):
                working_memory.storeToNeuron(i, 0.5)
                working_memory.getMemoryNeurons()
            wm_time = time.time() - wm_start
        
        # Profile episodic memory operations
        if episodic_memory:
            em_start = time.time()
            for i in range(100):
                episode = pynlm.EpisodicMemoryItem()
                episode.timestamp = i
                episode.reward = float(i)
                episodic_memory.storeEpisode(episode)
                episodic_memory.getEpisodesForReplay(5)
            em_time = time.time() - em_start
        
        return {
            'working_memory_time': wm_time if working_memory else 0,
            'episodic_memory_time': em_time if episodic_memory else 0,
            'total_memory_time': (wm_time + em_time) if working_memory and episodic_memory else 0
        }
    
    def generate_performance_report(self):
        """Generate comprehensive performance report"""
        report = {
            'timestamp': time.time(),
            'brain_configuration': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'region_count': self.brain.getRegionCount()
            }
        }
        
        # Add profiling results
        neuron_profile = self.profile_neuron_operations()
        spike_profile = self.profile_spike_system()
        memory_profile = self.profile_memory_systems()
        
        report['performance'] = {
            'neuron_operations': neuron_profile,
            'spike_system': spike_profile,
            'memory_systems': memory_profile
        }
        
        return report

# Performance profiling example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

profiler = PerformanceProfiler(brain)

# Simulate some activity
for step in range(100):
    brain.step(step)

# Generate performance report
performance_report = profiler.generate_performance_report()
print(f"Performance Report generated at {performance_report['timestamp']}")
```

## 13. Advanced Python API Extensions

### Custom Brain Subclasses

```python
import pynlm

class CustomBrain(pynlm.Brain):
    """Custom brain subclass with additional functionality"""
    
    def __init__(self, config):
        super().__init__(config)
        self.custom_properties = {}
    
    def add_custom_property(self, name, value):
        """Add custom property to brain"""
        self.custom_properties[name] = value
    
    def get_custom_property(self, name, default=None):
        """Get custom property by name"""
        return self.custom_properties.get(name, default)
    
    def apply_custom_stimulus(self, stimulus_type, parameters):
        """Apply custom stimulus to brain"""
        if stimulus_type == 'burst':
            self._apply_burst_stimulus(parameters)
        elif stimulus_type == 'pattern':
            self._apply_pattern_stimulus(parameters)
        elif stimulus_type == 'targeted':
            self._apply_targeted_stimulus(parameters)
    
    def _apply_burst_stimulus(self, params):
        """Apply burst stimulus to neurons"""
        # Implementation for burst stimulation
        intensity = params.get('intensity', 1.0)
        duration = params.get('duration', 10)
        
        for region in self.getRegions():
            for population in region.getPopulations():
                for neuron in population.getNeurons():
                    # Apply burst stimulation
                    for t in range(duration):
                        neuron.injectCurrent(intensity * (1.0 - t/duration))
    
    def _apply_pattern_stimulus(self, params):
        """Apply patterned stimulus"""
        # Implementation for patterned stimulation
        pattern = params.get('pattern', [])
        neuron_ids = params.get('neuron_ids', [])
        
        for i, neuron_id in enumerate(neuron_ids):
            if i < len(pattern):
                self.injectCurrent(neuron_id, pattern[i])
    
    def _apply_targeted_stimulus(self, params):
        """Apply targeted stimulus to specific regions"""
        # Implementation for targeted stimulation
        target_region = params.get('target_region')
        stimulus_value = params.get('stimulus_value', 1.0)
        
        for region_id in self.getRegionIds():
            if region_id == target_region:
                region = self.getRegion(region_id)
                for neuron in region.getAllNeurons():
                    neuron.injectCurrent(stimulus_value)

# Custom brain usage
config = pynlm.createDefaultConfig()
custom_brain = CustomBrain(config)
custom_brain.initialize()

# Add custom properties
custom_brain.add_custom_property('creation_date', time.time())
custom_brain.add_custom_property('researcher', 'Dr. Smith')

# Apply custom stimuli
custom_brain.apply_custom_stimulus('burst', {'intensity': 2.0, 'duration': 20})
custom_brain.apply_custom_stimulus('pattern', {'pattern': [1.0, 0.5, 1.0, 0.5], 'neuron_ids': [1, 2, 3, 4]})
```

## 14. Command Line Script Examples

### Python Script with Command Line Interface

```python
#!/usr/bin/env python3
"""Advanced NLM analysis script with command line interface"""

import pynlm
import argparse
import json
import time
from pathlib import Path

def create_brain_from_args(args):
    """Create brain based on command line arguments"""
    config = pynlm.createDefaultConfig()
    
    # Set basic configuration
    if args.neurons:
        config.set("brain.neuron_count", args.neurons)
    
    if args.regions:
        config.set("brain.region_count", args.regions)
    
    if args.timestep:
        config.set("simulation_timestep", args.timestep)
    
    # Set advanced configuration
    if args.plasticity:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", args.plasticity_rate)
    
    if args.novelty:
        config.set("neuromod.novelty.enable", True)
    
    if args.curiosity:
        config.set("neuromod.curiosity.enable", True)
    
    if args.development:
        config.set("development.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def run_simulation(brain, args):
    """Run simulation with specified parameters"""
    start_time = time.time()
    results = []
    
    for step in range(args.steps):
        # Record metrics
        if args.metrics:
            metrics = {
                'step': step,
                'time': time.time() - start_time,
                'neurons_firing': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'ei_ratio': brain.getExcitationInhibitionRatio()
            }
            
            if args.memory:
                working_memory = brain.getWorkingMemory()
                if working_memory:
                    metrics['working_memory_traces'] = working_memory.getActiveTraces()
                
                episodic_memory = brain.getEpisodicMemory()
                if episodic_memory:
                    metrics['episodic_memory_episodes'] = episodic_memory.getEpisodeCount()
            
            results.append(metrics)
        
        # Perform simulation step
        brain.step(step)
        
        # Create checkpoint
        if args.checkpoint and step % args.checkpoint_interval == 0:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            brain.save(checkpoint_file)
    
    end_time = time.time()
    
    return {
        'total_time': end_time - start_time,
        'steps_per_second': args.steps / (end_time - start_time),
        'final_results': results[-1] if results else None,
        'all_results': results,
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'ei_ratio': brain.getExcitationInhibitionRatio()
    }

def analyze_results(results, args):
    """Analyze and export simulation results"""
    if not results:
        print("No results to analyze")
        return
    
    # Create analysis
    analysis = {
        'simulation_parameters': {
            'neurons': args.neurons if args.neurons else 'default',
            'regions': args.regions if args.regions else 'default',
            'steps': args.steps,
            'timestep': args.timestep if args.timestep else 0.001
        },
        'performance_metrics': {
            'total_time': results['total_time'],
            'steps_per_second': results['steps_per_second'],
            'total_spikes': results['total_spikes'],
            'average_firing_rate': results['average_firing_rate'],
            'ei_ratio': results['ei_ratio']
        }
    }
    
    # Add time series data if requested
    if args.time_series and 'all_results' in results:
        analysis['time_series'] = results['all_results']
    
    # Add memory metrics if requested
    if args.memory and 'final_results' in results and results['final_results']:
        analysis['memory_metrics'] = {
            'working_memory_traces': results['final_results'].get('working_memory_traces', 0),
            'episodic_memory_episodes': results['final_results'].get('episodic_memory_episodes', 0)
        }
    
    # Export based on format
    if args.output:
        if args.output.endswith('.json'):
            with open(args.output, 'w') as f:
                json.dump(analysis, f, indent=2)
        elif args.output.endswith('.csv'):
            df = pd.DataFrame(analysis['time_series'])
            df.to_csv(args.output, index=False)
        elif args.output.endswith('.html'):
            # Generate HTML report
            html_report = generate_html_report(analysis)
            with open(args.output, 'w') as f:
                f.write(html_report)
    
    return analysis

def generate_html_report(analysis):
    """Generate HTML report from analysis"""
    html = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>NLM Simulation Results</title>
        <style>
            body {{ font-family: Arial, sans-serif; margin: 40px; }}
            .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
            .metrics {{ display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; }}
            .metric-card {{ background-color: #e9e9e9; padding: 15px; border-radius: 5px; }}
            .time-series {{ margin-top: 30px; }}
            table {{ width: 100%; border-collapse: collapse; }}
            th, td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
            th {{ background-color: #f2f2f2; }}
        </style>
    </head>
    <body>
        <div class="header">
            <h1>NLM Simulation Results</h1>
            <p>Generated on {time.ctime()}</p>
        </div>
        
        <h2>Performance Metrics</h2>
        <div class="metrics">
            <div class="metric-card">
                <h3>Total Time</h3>
                <p>{analysis['performance_metrics']['total_time']:.2f} seconds</p>
            </div>
            <div class="metric-card">
                <h3>Steps per Second</h3>
                <p>{analysis['performance_metrics']['steps_per_second']:.2f}</p>
            </div>
            <div class="metric-card">
                <h3>Total Spikes</h3>
                <p>{analysis['performance_metrics']['total_spikes']}</p>
            </div>
            <div class="metric-card">
                <h3>Average Firing Rate</h3>
                <p>{analysis['performance_metrics']['average_firing_rate']:.2f}</p>
            </div>
        </div>
        
        <h2>Time Series Data</h2>
        <div class="time-series">
            <table>
                <tr>
                    <th>Step</th>
                    <th>Time (s)</th>
                    <th>Neurons Firing</th>
                    <th>Total Spikes</th>
                    <th>Average Firing Rate</th>
                    <th>E/I Ratio</th>
                </tr>
    """
    
    if 'time_series' in analysis:
        for point in analysis['time_series'][:100]:  # Show first 100 points
            html += f"""
                <tr>
                    <td>{point['step']}</td>
                    <td>{point['time']:.3f}</td>
                    <td>{point['neurons_firing']}</td>
                    <td>{point['total_spikes']}</td>
                    <td>{point['average_firing_rate']:.2f}</td>
                    <td>{point['ei_ratio']:.2f}</td>
                </tr>
            """
    
    html += """
            </table>
        </div>
    </body>
    </html>
    """
    
    return html

def main():
    parser = argparse.ArgumentParser(description="Advanced NLM simulation and analysis")
    
    # Basic simulation parameters
    parser.add_argument('--neurons', type=int, help='Number of neurons')
    parser.add_argument('--regions', type=int, help='Number of regions')
    parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
    
    # Advanced configuration
    parser.add_argument('--plasticity', action='store_true', help='Enable plasticity')
    parser.add_argument('--plasticity-rate', type=float, default=0.001, help='Plasticity learning rate')
    parser.add_argument('--novelty', action='store_true', help='Enable novelty detection')
    parser.add_argument('--curiosity', action='store_true', help='Enable curiosity')
    parser.add_argument('--development', action='store_true', help='Enable development')
    
    # Output and analysis options
    parser.add_argument('--metrics', action='store_true', help='Record performance metrics')
    parser.add_argument('--memory', action='store_true', help='Record memory metrics')
    parser.add_argument('--time-series', action='store_true', help='Include time series data')
    parser.add_argument('--output', help='Output file for results')
    
    # Checkpoint and simulation options
    parser.add_argument('--checkpoint', action='store_true', help='Enable checkpointing')
    parser.add_argument('--checkpoint-interval', type=int, default=100, help='Checkpoint interval')
    
    args = parser.parse_args()
    
    # Create brain
    brain = create_brain_from_args(args)
    
    # Run simulation
    print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons...")
    results = run_simulation(brain, args)
    
    # Analyze results
    print("Analyzing results...")
    analysis = analyze_results(results, args)
    
    # Save brain state
    if args.output and not args.output.endswith('.csv') and not args.output.endswith('.html'):
        brain.save(args.output.replace('.json', '_brain.bin'))
    
    print(f"Simulation completed in {results['total_time']:.2f} seconds")
    print(f"Average firing rate: {results['average_firing_rate']:.2f}")
    print(f"Total spikes: {results['total_spikes']}")

if __name__ == '__main__':
    main()
"""

# Write the script to file
with open('advanced_nlm_analysis.py', 'w') as f:
    f.write(script_content)

print("Advanced NLM analysis script created: advanced_nlm_analysis.py")
print("Usage: python advanced_nlm_analysis.py --neurons 1000 --steps 5000 --output results.json")
```

This comprehensive guide provides advanced usage patterns, command-line tools, and programming techniques for the NLM framework. It covers:

1. **Command Line Interface**: Advanced CLI commands for brain management, experiments, and configuration
2. **Complex Brain Configurations**: Custom brain templates and specialized configurations
3. **Advanced World Simulation**: Complex world environments with multiple object types
4. **Memory and Learning**: Advanced memory management and neuromodulation control
5. **Performance Optimization**: Performance monitoring, checkpoint management, and profiling tools
6. **Advanced Experiment Framework**: Custom experiment definitions and batch processing
7. **Parallel Processing**: Parallel brain simulation and batch processing
8. **Error Handling**: Advanced error recovery and debugging tools
9. **Performance Profiling**: Detailed performance analysis and reporting
10. **Framework Extensions**: Custom brain subclasses and component extensions
11. **External Tool Integration**: Integration with analysis and visualization tools
12. **Command Line Scripts**: Advanced Python scripts with command-line interfaces

This guide enables users to work with the NLM framework at an advanced level, leveraging all available features and capabilities for complex neural simulations and research tasks.