# NLM Advanced Usage Guide

**Version: 0.1.0 - Phase 6 Integration**

This guide provides advanced usage patterns, API reference, and examples for experienced NLM users who want to leverage the full power of the neural learning machine system.

## Table of Contents

1. [Quick Start for Advanced Users](#quick-start-for-advanced-users)
2. [Configuration Deep Dive](#configuration-deep-dive)
3. [Advanced Brain Management](#advanced-brain-management)
4. [Batch Processing & Experiment Management](#batch-processing--experiment-management)
5. [Performance Monitoring & Optimization](#performance-monitoring--optimization)
6. [Memory System Integration](#memory-system-integration)
7. [Neuromodulation Control](#neuromodulation-control)
8. [Development & Evolution](#development--evolution)
9. [Checkpointing & Persistence](#checkpointing--persistence)
10. [Python API Reference](#python-api-reference)
11. [Command Line Interface](#command-line-interface)
12. [Troubleshooting Advanced Issues](#troubleshooting-advanced-issues)

---

## Quick Start for Advanced Users

If you're already familiar with basic NLM usage, here are the essential commands for advanced workflows:

### 1. Create an experimental configuration

```python
import pynlm

# Create advanced configuration with all systems enabled
config = pynlm.createDefaultConfig()

# Enable experimental settings
config.set("brain.neuron_count", 2000)  # Larger brain
config.set("brain.region_count", 4)     # Multiple regions
config.set("brain.connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.018)

# Enable all plasticity types
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Enable neuromodulation with custom settings
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction_error.enable", True)

# Development settings
config.set("development.enable", True)
config.set("development.initial_plasticity", 0.8)

# Memory settings
config.set("memory.working.capacity", 500)
config.set("memory.episodic.max_episodes", 5000)
config.set("memory.associative.patterns", 1000)

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Advanced brain setup with agent integration

```python
import pynlm

# Create world with custom parameters
world = pynlm.createSimpleWorld()
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16
)
world.setAgentStart(25.0, 25.0)
world.setMaxEnergy(1000.0)
world.setEnergyDecayRate(0.001)

# Create brain with advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 4)
config.set("random_seed", 12345)  # Reproducible experiments

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with all systems enabled
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableSocialLearning(True)

# Configure development stages
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Configure neuromodulation
brain.getDopamine().setScale(2.0)
brain.getCuriosity().setExplorationRate(0.8)
brain.getNovelty().setDetectionThreshold(0.3)
```

### 3. Run advanced simulation with monitoring

```python
import pynlm
import time
import json
from datetime import datetime

# Create experimental setup (as above)
# ... (setup code)

# Performance monitoring callback
def performance_monitor(report):
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] {report}")

# Set up monitoring
brain.getSpikeSystem().registerHandler(performance_monitor)

# Run advanced simulation
start_time = time.time()
total_reward = 0.0
episode_count = 0

for episode in range(100):
    world.reset()
    agent.reset()
    world.setAgentStart(25.0, 25.0)
    agent.initialize(world)
    
    step = 0
    while step < 5000 and agent.getCuriosityLevel() < 0.9:
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent (includes prediction and planning)
        agent.processSensoryInput(percept)
        
        # Neural computation
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward modulation
        reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, 0.0)
        total_reward += reward
        
        step += 1
    
    episode_count += 1
    
    # Save episode data
    episode_data = {
        'episode': episode,
        'steps': step,
        'reward': reward,
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage(),
        'memory_traces': agent.getWorkingMemory().getActiveTraces() if agent.getWorkingMemory() else 0
    }
    
    with open(f"episode_{episode:04d}.json", 'w') as f:
        json.dump(episode_data, f, indent=2)

elapsed_time = time.time() - start_time
print(f"Episode {episode_count} completed in {elapsed_time:.1f}s")
print(f"Average reward: {total_reward/episode_count:.3f}")
```

---

## Configuration Deep Dive

### Advanced Configuration Parameters

#### Brain Configuration

| Parameter | Type | Default | Description | Advanced Use |
|-----------|------|---------|-------------|--------------|
| `brain.neuron_count` | int | 1000 | Total neurons | Scale up to 10k+ for large networks |
| `brain.region_count` | int | 1 | Number of brain regions | 4-16 for modular organization |
| `brain.connection_probability` | float | 0.1 | Synapse connectivity | 0.05-0.2 for controlled connectivity |
| `brain.synapse_density` | float | 0.1 | Synaptic density factor | Adjust for network sparsity |
| `random_seed` | int64 | 42 | Random seed | Fixed for reproducible experiments |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable spike-timing dependent plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP LTP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP LTD time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.stdp.coupling` | float | 1.0 | STDP-LTD/LTP coupling ratio |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.dopamine.plasticity_factor` | float | 1.0 | Dopamine effect on plasticity |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.exploration_rate` | float | 0.1 | Baseline exploration rate |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.5 | Novelty detection threshold |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error signals |

### Configuration Factory Functions

```python
# Advanced configuration presets

def create_reproduction_config(neuron_count=1000, seed=42):
    """Create configuration optimized for reproducible research"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", 2)
    config.set("random_seed", seed)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    return config

def create_exploration_config(neuron_count=500, exploration_intensity=0.5):
    """Create configuration optimized for exploration and novelty"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", 1)
    config.set("neuromod.curiosity.exploration_rate", exploration_intensity)
    config.set("neuromod.novelty.threshold", 0.3)
    config.set("plasticity.structural.enable", True)
    return config

def create_learning_config(neuron_count=2000, plasticity_rate=0.8):
    """Create configuration optimized for fast learning"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", 4)
    config.set("plasticity.stdp.learning_rate", 0.01 * plasticity_rate)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    return config
```

---

## Advanced Brain Management

### Brain State Management

#### Checkpoint Management

```python
# Advanced checkpoint operations
def save_with_metadata(brain, filepath, metadata=None):
    """Save brain with custom metadata"""
    # Custom implementation
    pass

def load_with_validation(brain, filepath, required_regions=None):
    """Load brain with validation"""
    # Custom implementation
    pass

def compare_brains(brain1, brain2):
    """Compare two brains for similarity"""
    # Returns similarity metrics
    pass
```

#### Brain Modification

```python
# Advanced brain manipulation

def inject_stimulus(brain, neuron_id, current, duration=100):
    """Inject controlled stimulus into specific neuron"""
    for step in range(duration):
        brain.injectCurrent(neuron_id, current)
        brain.step(step)

def create_connectivity_pattern(brain, pattern_type='random'):
    """Create specific connectivity patterns"""
    if pattern_type == 'random':
        # Random connectivity
        pass
    elif pattern_type == 'modular':
        # Modular connectivity between regions
        pass
    elif pattern_type == 'hub':
        # Hub-and-spoke connectivity
        pass

def analyze_network_topology(brain):
    """Analyze network topology properties"""
    # Returns graph metrics
    pass
```

### Brain Statistics & Analysis

```python
# Comprehensive brain analysis

def get_brain_statistics(brain):
    """Get comprehensive brain statistics"""
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'active_neurons': brain.getActiveNeuronCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'e_i_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'development_stage': brain.getDevelopmentalStage(),
        'learned_preference': brain.getLearnedPreference(),
        'pending_spikes': brain.getPendingSpikeEventCount(),
    }
    
    # Add regional statistics
    regional_stats = []
    for region in brain.getRegions():
        regional_stats.append({
            'id': region.getId().index(),
            'name': region.getName(),
            'neuron_count': region.getTotalNeuronCount(),
            'synapse_count': region.getSynapseCount(),
            'avg_weight': region.getAverageSynapticWeight(),
            'neuron_types': region.getNeuronTypeCount()
        })
    
    stats['regional'] = regional_stats
    return stats

def get_plasticity_summary(brain):
    """Get plasticity system status"""
    summary = {
        'stdp_enabled': brain.getSTDP().getLearningRate() > 0,
        'hebbian_enabled': brain.getHebbian().getLearningRate() > 0,
        'structural_enabled': brain.getStructuralPlasticity().getSynaptogenesisRate() > 0,
        'plasticity_modifiers': brain.getDopamine().getPlasticityFactor() if brain.getDopamine() else 1.0
    }
    return summary
```

---

## Batch Processing & Experiment Management

### Batch Configuration Management

```python
# Batch processing utilities

def load_configs_from_directory(config_dir):
    """Load all configuration files from directory"""
    configs = {}
    for file in Path(config_dir).glob("*.json"):
        config = pynlm.createDefaultConfig()
        config.loadFromFile(str(file))
        configs[file.stem] = config
    return configs

def run_parameter_sweep(base_config, parameters):
    """Run parameter sweep over configuration space"""
    results = {}
    
    for param_name, param_values in parameters.items():
        for value in param_values:
            # Create modified config
            config = pynlm.createDefaultConfig()
            config.loadFromArgs([])  # Load defaults
            
            # Apply parameter
            config.set(f"brain.{param_name}", value, pynlm.ConfigSource.Runtime)
            
            # Run experiment
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation and collect results
            result = run_experiment(brain, duration=1000)
            
            key = f"{param_name}_{value}"
            results[key] = result
    
    return results

def run_optimization_experiment(base_config, objective_function, search_space):
    """Run optimization experiment"""
    # Custom implementation using evolutionary algorithms
    pass
```

### Experiment Management

```python
# Experiment management utilities

class ExperimentRunner:
    """Manage complex experiments"""
    
    def __init__(self):
        self.results = {}
        self.checkpoints = []
    
    def run_multiple_episodes(self, brain, world, agent, num_episodes, steps_per_episode):
        """Run multiple episodes with episode-based analysis"""
        all_episodes = []
        
        for episode in range(num_episodes):
            # Reset environment
            world.reset()
            agent.reset()
            
            # Setup episode
            episode_data = self.setup_episode(episode, brain, world, agent)
            
            # Run episode
            episode_result = self.run_episode(episode_data, steps_per_episode)
            all_episodes.append(episode_result)
            
            # Save checkpoint
            if episode % 10 == 0:
                checkpoint_file = f"checkpoints/episode_{episode:04d}.bin"
                brain.save(checkpoint_file)
                self.checkpoints.append(checkpoint_file)
            
            # Analyze and store
            analysis = self.analyze_episode(episode_result)
            self.results[f"episode_{episode:04d}"] = analysis
        
        return all_episodes
    
    def analyze_experiment(self, episodes):
        """Analyze multiple episodes"""
        analysis = {
            'total_episodes': len(episodes),
            'total_steps': sum(ep['steps'] for ep in episodes),
            'mean_reward': sum(ep['reward'] for ep in episodes) / len(episodes),
            'reward_variance': self.calculate_variance(ep['reward'] for ep in episodes),
            'learning_curve': self.extract_learning_curve(episodes),
            'development_trajectory': self.extract_development_trajectory(episodes)
        }
        
        return analysis
```

---

## Performance Monitoring & Optimization

### Real-time Performance Monitoring

```python
# Performance monitoring utilities

class PerformanceMonitor:
    """Monitor NLM performance in real-time"""
    
    def __init__(self, brain, world, agent, interval=100):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.interval = interval
        self.metrics = {
            'step_count': 0,
            'total_time': 0.0,
            'spike_rate': 0.0,
            'memory_usage': 0.0,
            'cpu_usage': 0.0
        }
    
    def start_monitoring(self, duration_seconds, callback=None):
        """Start performance monitoring for specified duration"""
        start_time = time.time()
        last_report = start_time
        
        while time.time() - start_time < duration_seconds:
            # Update metrics
            self.update_metrics()
            
            # Report periodically
            if self.metrics['step_count'] % self.interval == 0:
                self.report_performance(callback)
            
            # Small delay to prevent busy waiting
            time.sleep(0.001)
        
        return self.get_final_metrics()
    
    def update_metrics(self):
        """Update performance metrics"""
        self.metrics['step_count'] += 1
        
        # Calculate metrics
        self.metrics['spike_rate'] = self.brain.getTotalSpikeCount() / max(1, self.metrics['step_count'])
        self.metrics['memory_usage'] = self.get_memory_usage()
        self.metrics['cpu_usage'] = self.get_cpu_usage()
        
        # Update total time
        self.metrics['total_time'] = time.time() - self.metrics['step_count'] * 0.1
    
    def report_performance(self, callback=None):
        """Generate performance report"""
        report = {
            'timestamp': time.time(),
            'steps_completed': self.metrics['step_count'],
            'elapsed_time': self.metrics['total_time'],
            'steps_per_second': self.metrics['step_count'] / max(0.001, self.metrics['total_time']),
            'spike_rate': self.metrics['spike_rate'],
            'memory_usage': self.metrics['memory_usage'],
            'cpu_usage': self.metrics['cpu_usage'],
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'ei_ratio': self.brain.getExcitationInhibitionRatio()
        }
        
        if callback:
            callback(report)
        
        print(f"Performance Report: Steps={report['steps_completed']}, "
              f"SPS={report['steps_per_second']:.1f}, "
              f"Rate={report['firing_rate']:.2f}Hz, "
              f"Memory={report['memory_usage']:.1f}MB")
        
        return report
```

### Performance Optimization

```python
# Performance optimization utilities

def optimize_brain_configuration(brain, target_performance):
    """Optimize brain configuration for target performance"""
    # Custom optimization algorithm
    pass

def profile_simulation(brain, world, agent, steps):
    """Profile simulation performance"""
    # Custom profiling
    pass

def get_recommended_configuration(neuron_count, target_performance):
    """Get recommended configuration for target performance"""
    # Configuration recommendations
    pass
```

---

## Memory System Integration

### Working Memory Management

```python
# Advanced working memory operations

def manage_working_memory(brain, agent):
    """Advanced working memory management"""
    if not brain.getWorkingMemory():
        return
    
    # Get current working memory state
    active_traces = agent.getWorkingMemory().getActiveTraces()
    memory_neurons = agent.getWorkingMemory().getMemoryNeurons()
    
    # Analyze memory stability
    memory_stability = calculate_memory_stability(active_traces)
    
    # Check for memory interference
    interference = detect_memory_interference(memory_neurons)
    
    # Consolidate important memories
    consolidate_important_memories(brain, agent)
    
    return {
        'active_traces': active_traces,
        'memory_stability': memory_stability,
        'interference_level': interference,
        'memory_capacity': agent.getWorkingMemory().getCapacity()
    }

def calculate_memory_stability(active_traces):
    """Calculate working memory stability metric"""
    if not active_traces:
        return 0.0
    
    # Custom stability calculation
    stability = sum(trace.activation for trace in active_traces) / len(active_traces)
    return stability
```

### Episodic Memory Analysis

```python
# Advanced episodic memory operations

def analyze_episodic_memory(brain, agent):
    """Analyze episodic memory content and quality"""
    if not brain.getEpisodicMemory():
        return None
    
    episodes = brain.getEpisodicMemory().getEpisodes()
    
    analysis = {
        'total_episodes': len(episodes),
        'episode_length_distribution': analyze_episode_lengths(episodes),
        'reward_distribution': analyze_reward_distribution(episodes),
        'novelty_tracking': track_novelty_over_time(episodes),
        'memory_consolidation_efficiency': calculate_consolidation_efficiency(episodes),
        'replay_frequency': calculate_replay_frequency(episodes)
    }
    
    return analysis

def track_novelty_over_time(episodes):
    """Track novelty detection over time"""
    novelty_timeline = []
    
    for episode in episodes:
        avg_novelty = sum(ep.novelty for ep in episode) / len(episode) if episode else 0
        novelty_timeline.append({
            'episode': episode.timestamp,
            'novelty': avg_novelty,
            'reward': episode.reward
        })
    
    return novelty_timeline
```

---

## Neuromodulation Control

### Dopamine System Control

```python
# Advanced dopamine system control

def modulate_dopamine(brain, reward, prediction_error):
    """Advanced dopamine modulation with prediction error"""
    if not brain.getDopamine():
        return
    
    # Calculate prediction error
    pe = reward - brain.getPredictionError()
    
    # Modulate dopamine level
    current_level = brain.getDopamine().getLevel()
    target_level = calculate_target_dopamine_level(current_level, pe)
    
    brain.getDopamine().setLevel(target_level)
    
    # Record modulation effect
    record_dopamine_modulation(current_level, target_level, pe)

def calculate_target_dopamine_level(current_level, prediction_error):
    """Calculate target dopamine level based on prediction error"""
    if prediction_error > 0.1:
        return min(2.0, current_level * 1.2 + 0.3)  # Reward prediction
    elif prediction_error < -0.1:
        return max(0.1, current_level * 0.8 - 0.2)  # Punishment prediction
    else:
        return current_level * 0.95  # No prediction error
```

### Curiosity System Control

```python
# Advanced curiosity system control

def optimize_curiosity_driven_exploration(brain, agent, environment):
    """Optimize curiosity-driven exploration behavior"""
    if not brain.getCuriosity():
        return
    
    current_curiosity = agent.getCuriosityLevel()
    current_novelty = agent.getNoveltyLevel()
    
    # Calculate optimal exploration rate
    optimal_exploration = calculate_optimal_exploration(current_curiosity, current_novelty)
    
    # Adjust curiosity system
    brain.getCuriosity().setExplorationRate(optimal_exploration)
    
    # Record exploration effectiveness
    record_exploration_effectiveness(optimal_exploration, current_novelty)
```

---

## Development & Evolution

### Developmental Stage Management

```python
# Advanced development stage management

def advanced_development_control(brain, agent, world):
    """Advanced developmental control with timing optimization"""
    current_stage = brain.getDevelopmentalStage()
    current_age = agent.getDevelopmentalAge()
    
    # Calculate optimal developmental stage based on age and performance
    optimal_stage = calculate_optimal_developmental_stage(current_age, agent.getPerformanceMetric())
    
    # Transition to optimal stage if appropriate
    if should_transition_stage(current_stage, optimal_stage, current_age):
        brain.setDevelopmentalStage(optimal_stage)
        record_developmental_transition(current_stage, optimal_stage, current_age)
    
    # Apply stage-specific effects
    apply_developmental_stage_effects(brain, optimal_stage)

def calculate_optimal_developmental_stage(age, performance_metric):
    """Calculate optimal developmental stage based on age and performance"""
    if age < 100:  # Early development
        return pynlm.DevelopmentalStage.Initial
    elif age < 1000:  # Critical period
        return pynlm.DevelopmentalStage.CriticalPeriod
    elif age < 5000:  # Maturation
        return pynlm.DevelopmentalStage.Maturation
    else:  # Adult
        return pynlm.DevelopmentalStage.Adult
```

### Evolution Simulation

```python
# Evolutionary simulation utilities

def run_evolution_experiment(brain_template, generations, population_size):
    """Run evolutionary experiment with neural networks"""
    population = create_initial_population(brain_template, population_size)
    
    for generation in range(generations):
        # Evaluate each individual
        fitness_scores = evaluate_population(population)
        
        # Select parents
        parents = select_parents(population, fitness_scores)
        
        # Create offspring through genetic operations
        offspring = genetic_operations(parents)
        
        # Replace population
        population = offspring
        
        # Record generation statistics
        record_generation_statistics(generation, fitness_scores, population)
    
    return get_best_individual(population)

def genetic_operations(parents):
    """Perform genetic operations on parent brains"""
    offspring = []
    
    for parent in parents:
        # Crossover
        child = crossover(parents[0], parents[1])
        
        # Mutation
        mutate_brain(child)
        
        # Epigenetic effects
        apply_epigenetic_modifications(child)
        
        offspring.append(child)
    
    return offspring
```

---

## Checkpointing & Persistence

### Advanced Checkpoint Management

```python
# Advanced checkpointing utilities

class CheckpointManager:
    """Advanced checkpoint management system"""
    
    def __init__(self, brain, max_checkpoints=10, checkpoint_interval=1000):
        self.brain = brain
        self.max_checkpoints = max_checkpoints
        self.checkpoint_interval = checkpoint_interval
        self.checkpoints = []
        self.checkpoint_dir = "checkpoints"
        
        os.makedirs(self.checkpoint_dir, exist_ok=True)
    
    def save_checkpoint(self, description="", metadata=None):
        """Save checkpoint with description and metadata"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"{self.checkpoint_dir}/checkpoint_{timestamp}.bin"
        
        if self.brain.save(filename):
            checkpoint_info = {
                'filename': filename,
                'timestamp': timestamp,
                'description': description,
                'metadata': metadata or {},
                'step': self.brain.getTotalSpikeCount(),
                'neuron_count': self.brain.getTotalNeuronCount()
            }
            
            self.checkpoints.append(checkpoint_info)
            
            # Clean up old checkpoints
            self.cleanup_old_checkpoints()
            
            return checkpoint_info
        
        return None
    
    def load_checkpoint(self, filename):
        """Load checkpoint and return checkpoint info"""
        if self.brain.load(filename):
            info = {
                'filename': filename,
                'loaded': True,
                'neuron_count': self.brain.getTotalNeuronCount()
            }
            return info
        
        return {'loaded': False}
    
    def cleanup_old_checkpoints(self):
        """Clean up old checkpoints if exceeding limit"""
        if len(self.checkpoints) > self.max_checkpoints:
            # Sort by timestamp (oldest first)
            self.checkpoints.sort(key=lambda x: x['timestamp'])
            
            # Remove oldest checkpoints
            for checkpoint in self.checkpoints[:len(self.checkpoints) - self.max_checkpoints]:
                if os.path.exists(checkpoint['filename']):
                    os.remove(checkpoint['filename'])
                self.checkpoints.remove(checkpoint)
    
    def get_checkpoint_summary(self):
        """Get checkpoint management summary"""
        return {
            'total_checkpoints': len(self.checkpoints),
            'max_checkpoints': self.max_checkpoints,
            'checkpoint_interval': self.checkpoint_interval,
            'checkpoint_dir': self.checkpoint_dir,
            'latest_checkpoint': self.checkpoints[-1] if self.checkpoints else None,
            'disk_usage': self.calculate_disk_usage()
        }
```

### Checkpoint Validation

```python
# Checkpoint validation utilities

def validate_checkpoint_integrity(checkpoint_file):
    """Validate checkpoint file integrity"""
    # Custom validation logic
    pass

def validate_checkpoint_compatibility(checkpoint_file, target_brain):
    """Validate checkpoint compatibility with target brain"""
    # Custom compatibility check
    pass

def repair_checkpoint(checkpoint_file):
    """Attempt to repair corrupted checkpoint"""
    # Custom repair logic
    pass
```

---

## Python API Reference

### Core Classes

#### Brain Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain with configuration | None | bool |
| `step(step, time=None)` | Perform simulation step | step, time | None |
| `receiveSensoryInput(input)` | Inject sensory input | input | None |
| `injectCurrent(neuron, current)` | Inject current to neuron | neuron, current | None |
| `produceAction()` | Produce motor action | None | Action |
| `reset()` | Reset brain state | None | None |
| `save(filepath)` | Save brain checkpoint | filepath | bool |
| `load(filepath)` | Load brain checkpoint | filepath | bool |
| `addRegion(name)` | Add neural region | name | RegionId |
| `getRegion(id)` | Get region by ID | id | NeuralRegion* |
| `getTotalNeuronCount()` | Get total neurons | None | size_t |
| `getTotalSynapseCount()` | Get total synapses | None | size_t |
| `getAverageFiringRate()` | Get average firing rate | None | float |
| `getExcitationInhibitionRatio()` | Get E/I ratio | None | float |
| `getTotalSpikeCount()` | Get total spike count | None | size_t |
| `getDevelopmentalStage()` | Get developmental stage | None | DevelopmentalStage |
| `setDevelopmentalStage(stage)` | Set developmental stage | stage | None |

#### AgentBrain Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize with world | world | None |
| `processSensoryInput(percept)` | Process sensory input | percept | None |
| `decodeMotorCommand()` | Decode motor command | None | MotorCommand |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward | reward, predicted_reward | None |
| `updateDevelopment(timestep)` | Update development | timestep | None |
| `enableRewardModulation(enable)` | Enable/disable reward | enable | None |
| `enableStructuralPlasticity(enable)` | Enable/disable structural plasticity | enable | None |
| `enableDevelopment(enable)` | Enable/disable development | enable | None |
| `enableCuriosity(enable)` | Enable/disable curiosity | enable | None |
| `getCuriosityLevel()` | Get curiosity level | None | float |
| `getNoveltyLevel()` | Get novelty level | None | float |
| `getPredictionError()` | Get prediction error | None | float |
| `reset()` | Reset agent | None | None |

#### Config Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load from JSON file | filepath | bool |
| `loadFromArgs(args)` | Load from arguments | args | bool |
| `saveToFile(filepath)` | Save to JSON file | filepath | bool |
| `has(key)` | Check if key exists | key | bool |
| `get(key)` | Get value by key | key | optional<T> |
| `getOr(key, default)` | Get value with default | key, default | T |
| `set(key, value, source)` | Set value | key, value, source | None |
| `getKeys()` | Get all keys | None | vector<string> |
| `clear()` | Clear all entries | None | None |
| `summary()` | Get configuration summary | None | string |

#### SimpleWorld Class

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, vision_width, vision_height)` | Configure world | width, height, vision_width, vision_height | None |
| `reset()` | Reset world | None | None |
| `setAgentStart(x, y)` | Set agent start position | x, y | None |
| `update(timestep)` | Update world | timestep | None |
| `applyMotorCommand(cmd, current_time)` | Apply motor command | cmd, current_time | None |
| `getSensoryPercept()` | Get sensory percept | None | SensoryPercept* |
| `getAgentBody()` | Get agent body | None | AgentBody* |
| `addObject(obj)` | Add world object | obj | None |
| `removeObject(x, y)` | Remove object | x, y | None |
| `isValidPosition(x, y)` | Check valid position | x, y | bool |

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

#### SynapseType
- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction synapse

#### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period stage
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

#### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

#### MotorCommand
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

#### WorldObjectType
- `Empty` - Empty object
- `Resource` - Resource object
- `Hazard` - Hazard object
- `Wall` - Wall object
- `Marker` - Marker object

### Advanced Functions

#### Factory Functions

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `createDefaultConfig()` | Create default configuration | None | Config* |
| `createBrain(config)` | Create brain with config | config | Brain* |
| `createSimpleWorld()` | Create simple world | None | SimpleWorld* |
| `createAgentBrain(brain)` | Create agent brain interface | brain | AgentBrain* |

#### Convenience Functions

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `run_simulation_batch()` | Run batch simulation | brain, world, agent, inputs, steps | float |
| `save_checkpoint()` | Save brain checkpoint | brain, filepath, include_memory, include_random_state | None |
| `load_checkpoint()` | Load brain checkpoint | brain, filepath | None |
| `create_experimental_config()` | Create experimental configuration | neuron_count, region_count, connection_probability, stdp_ltp_weight, stdp_ltd_weight | Config* |
| `monitor_performance()` | Set up performance monitoring | brain, callback | bool |
| `run_development_experiment()` | Run development experiment | brain, world, agent, development_rate, total_steps | tuple |

---

## Command Line Interface

The NLM includes a comprehensive command-line interface (`nlm-cli`) for advanced usage:

### Basic Commands

```bash
# Create brain with default configuration
nlm-cli create-brain

# Create brain with custom configuration
nlm-cli create-brain --config my_config.json

# Create world with custom dimensions
nlm-cli create-world --width 50 --height 50 --vision-width 16 --vision-height 16

# Create agent brain interface
nlm-cli create-agent
```

### Simulation Commands

```bash
# Run simulation
nlm-cli run-simulation --steps 1000 --output results.json

# Run simulation with specific features enabled
nlm-cli run-simulation --steps 1000 --enable-reward --enable-plasticity --enable-development

# Run with custom timestep
nlm-cli run-simulation --steps 500 --timestep 0.05
```

### Checkpoint Commands

```bash
# Save brain checkpoint
nlm-cli save-brain brain_checkpoint.bin

# Load brain checkpoint
nlm-cli load-brain brain_checkpoint.bin

# Export configuration
nlm-cli export-config config.json
```

### Advanced Commands

```bash
# Run batch simulation
nlm-cli batch config1.json config2.json config3.json \
    --output-dir batch_results --steps-per-config 2000

# Monitor performance
nlm-cli monitor --duration 60 --interval 50

# List configuration options
nlm-cli list-config-options
```

### Usage Examples

```bash
#!/bin/bash
# Advanced simulation script

# Create experimental configuration
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
config.set('brain.neuron_count', 2000)
config.set('brain.region_count', 4)
config.set('random_seed', 12345)
config.saveToFile('exp_config.json')
"

# Run simulation with configuration
./nlm-cli create-brain --config exp_config.json
./nlm-cli create-world --width 30 --height 30 --vision-width 12 --vision-height 12
./nlm-cli create-agent
./nlm-cli run-simulation --steps 2000 --output exp_results.json

# Save checkpoint
./nlm-cli save-brain exp_checkpoint.bin

# Run batch experiment
./nlm-cli batch config1.json config2.json --output-dir results --steps-per-config 1000

# Monitor performance during long simulation
./nlm-cli monitor --duration 300 --interval 100 &
./nlm-cli run-simulation --steps 5000
./nlm-cli stop-monitoring
```

---

## Troubleshooting Advanced Issues

### Common Advanced Issues

#### Issue: High memory usage
**Symptoms**: Memory usage grows uncontrollably during long simulations

**Solutions**:

1. **Reduce working memory capacity**:
   ```python
   config.set("memory.working.capacity", 100)
   ```

2. **Enable memory consolidation**:
   ```python
   config.set("memory.episodic.consolidation_interval", 500)
   ```

3. **Limit checkpoint frequency**:
   ```python
   # Use checkpoint management with limited checkpoints
   ```

#### Issue: Slow simulation performance
**Symptoms**: Simulation runs much slower than expected

**Solutions**:

1. **Reduce neuron count**:
   ```python
   config.set("brain.neuron_count", 500)
   ```

2. **Increase connection probability**:
   ```python
   config.set("brain.connection_probability", 0.2)
   ```

3. **Optimize world size**:
   ```python
   # Use smaller world dimensions for faster processing
   world.configure(10, 10, 4, 4)
   ```

#### Issue: Unstable learning
**Symptoms**: Learning metrics fluctuate wildly or diverge

**Solutions**:

1. **Reduce plasticity rates**:
   ```python
   config.set("plasticity.stdp.learning_rate", 0.001)
   ```

2. **Increase dopamine scale**:
   ```python
   config.set("neuromod.dopamine.scale", 0.5)
   ```

3. **Enable novelty detection**:
   ```python
   config.set("neuromod.novelty.enable", True)
   ```

### Advanced Debugging

```python
# Advanced debugging utilities

def debug_simulation(brain, world, agent):
    """Debug simulation with detailed logging"""
    print("=== Advanced Debugging Information ===")
    
    # Brain state debug
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio()}")
    
    # Neuromodulation debug
    if brain.getDopamine():
        print(f"Dopamine level: {brain.getDopamine().getLevel():.3f}")
    
    if brain.getCuriosity():
        print(f"Curiosity level: {brain.getCuriosity().getLevel():.3f}")
    
    if brain.getNovelty():
        print(f"Novelty level: {brain.getNovelty().getLevel():.3f}")
    
    # Agent debug
    print(f"Agent curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Agent novelty: {agent.getNoveltyLevel():.3f}")
    print(f"Agent prediction error: {agent.getPredictionError():.3f}")
    
    # Memory debug
    if brain.getWorkingMemory():
        print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    if brain.getEpisodicMemory():
        print(f"Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    # Development debug
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Developmental age: {agent.getDevelopmentalAge():.1f}")
    
    # World debug
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"Simulation time: {world.getSimulationTime():.3f}")

def performance_profile(brain, world, agent, steps):
    """Profile simulation performance"""
    import time
    
    start_time = time.time()
    spike_counts = []
    memory_usage = []
    
    for step in range(steps):
        # Record metrics
        spike_counts.append(brain.getTotalSpikeCount())
        
        # Run simulation
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get performance metrics
        memory_usage.append(get_memory_usage())
    
    elapsed_time = time.time() - start_time
    
    # Calculate statistics
    avg_spike_rate = (spike_counts[-1] - spike_counts[0]) / max(1, steps) / 0.1
    avg_memory_usage = sum(memory_usage) / len(memory_usage)
    
    print(f"Performance Profile:")
    print(f"  Duration: {elapsed_time:.2f}s")
    print(f"  Steps per second: {steps / elapsed_time:.1f}")
    print(f"  Average spike rate: {avg_spike_rate:.1f} Hz")
    print(f"  Average memory usage: {avg_memory_usage:.1f} MB")
    print(f"  Total spikes: {spike_counts[-1]}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Error Codes

| Error Code | Description | Solution |
|------------|-------------|----------|
| 1 | Configuration error | Check configuration file format |
| 2 | Brain initialization failed | Check system resources |
| 3 | Memory allocation failed | Increase system memory |
| 4 | Invalid parameters | Check parameter ranges |
| 5 | File I/O error | Check file permissions |
| 6 | Simulation timeout | Reduce simulation steps |
| 7 | Network error | Check network connectivity |

---

## Conclusion

This advanced usage guide provides comprehensive coverage of NLM's advanced features, including:

1. **Advanced configuration management** for research experiments
2. **High-performance simulation** with batch processing capabilities
3. **Memory system integration** for long-term learning
4. **Neuromodulation control** for adaptive behavior
5. **Development and evolution** simulations
6. **Comprehensive checkpointing** for reproducibility
7. **Advanced debugging** and troubleshooting tools
8. **Command-line interface** for automated workflows

The NLM framework is now equipped with advanced features that support cutting-edge research in artificial neural systems, making it suitable for:

- **Research experiments** requiring controlled, reproducible conditions
- **Production systems** needing high-performance, long-running simulations
- **Machine learning applications** leveraging neuromorphic principles
- **Educational tools** for teaching advanced neural computation concepts
- **Scientific simulations** studying biological brain dynamics

These improvements transform NLM from a basic neural simulator into a comprehensive platform for advanced neural computation research and development.

---

*Version 0.1.0 - Phase 6 Integration*
*For support, please refer to the NLM documentation or contact the development team.*