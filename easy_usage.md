# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

## Advanced Usage for Power Users

This section covers advanced usage patterns and techniques for creating sophisticated brain simulations and experiments.

### Python API Reference

#### Core Functions

```python
# Configuration management
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.saveToFile("my_config.cfg")

# Create specialized configurations
def createSmallConfig():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.005)
    return config

def createLargeConfig():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 10000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.learning_rate", 0.02)
    config.set("plasticity.structural.enable", True)
    return config
```

#### Configuration Types

```python
# Configuration validation
config = pynlm.createBrainConfig()
config.validate()  # Throws exception if invalid

# Configuration comparison
config1 = pynlm.createDefaultConfig()
config2 = pynlm.createBrainConfig()
diff = config1.compare(config2)
```

### Advanced Agent Behaviors

#### Custom Agent with Sensory Processing

```python
import pynlm
import numpy as np

class AdvancedAgent:
    def __init__(self, brain, world):
        self.brain = pynlm.createAgentBrain(brain)
        self.world = world
        self.agent = self.brain
        self.memory_buffer = []
        self.reward_history = []
        
    def initialize_with_advanced_setup(self):
        # Custom configuration
        config = self.brain.getConfig()
        config.set("neuromodulation.curiosity.enable", True)
        config.set("neuromodulation.novelty.enable", True)
        config.set("neuromodulation.prediction_error.enable", True)
        
        # Initialize agent
        self.brain.initialize(self.world)
        
        # Enable advanced features
        self.agent.enableCuriosity(True)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
    def process_sensory_advanced(self, percept):
        # Enhance sensory input with feature extraction
        enhanced_percept = self._extract_features(percept)
        
        # Store in memory
        self.memory_buffer.append({
            'percept': percept,
            'enhanced': enhanced_percept,
            'timestamp': self.world.getSimulationTime()
        })
        
        # Limit memory size
        if len(self.memory_buffer) > 1000:
            self.memory_buffer.pop(0)
            
        # Process with brain
        self.agent.processSensoryInput(enhanced_percept)
        
    def _extract_features(self, percept):
        # Custom feature extraction logic
        vision_data = percept.getVision()
        if vision_data:
            # Extract spatial features
            center_intensity = np.mean(vision_data[vision_data.shape[0]//2, vision_data.shape[1]//2])
            edges = self._detect_edges(vision_data)
            
            # Create enhanced percept
            enhanced = pynlm.createSensoryInput()
            enhanced.setVision(vision_data)
            enhanced.addInternalFeature('center_intensity', center_intensity)
            enhanced.addInternalFeature('edge_density', np.mean(edges))
            
        return enhanced
    
    def learn_from_experience(self, reward, predicted_reward):
        # Advanced reward modulation
        self.agent.applyRewardModulation(reward, predicted_reward)
        
        # Store reward history
        self.reward_history.append({
            'reward': reward,
            'prediction_error': abs(reward - predicted_reward),
            'timestamp': self.world.getSimulationTime()
        })
        
        # Trim history
        if len(self.reward_history) > 500:
            self.reward_history.pop(0)
            
    def get_advanced_stats(self):
        """Get comprehensive brain statistics"""
        brain = self.brain
        stats = {
            'basic': {
                'neurons': brain.getTotalNeuronCount(),
                'synapses': brain.getTotalSynapseCount(),
                'spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio()
            },
            'neuromodulation': {
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'dopamine': self.agent.getDopamineLevel(),
                'prediction_error': self.agent.getPredictionError()
            },
            'memory': {
                'working_memory': brain.getWorkingMemory().getActiveMemoryCount(),
                'episodic_memory': brain.getEpisodicMemory().getMemoryCount()
            },
            'development': {
                'stage': brain.getDevelopmentalStage(),
                'age': brain.getDevelopmentSystem().getDevelopmentalAge()
            }
        }
        return stats
```

#### Batch Processing for Multiple Brains

```python
import pynlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor

def run_single_simulation(config_id):
    """Run a single simulation - worker function"""
    config = pynlm.loadConfigFromFile(f"config_{config_id}.cfg")
    
    # Create and run simulation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run with progress tracking
    for step in range(config.getOr("simulation.max_steps", 1000)):
        brain.step(step)
        
        if step % 100 == 0:
            print(f"Config {config_id}, Step {step}: {brain.getFiringNeuronCount()} firing")
    
    return {
        'config_id': config_id,
        'final_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'learning_efficiency': brain.getExcitationInhibitionRatio()
    }

def batch_run_simulations(config_files):
    """Run multiple simulations in parallel"""
    with ProcessPoolExecutor(max_workers=mp.cpu_count()) as executor:
        futures = [executor.submit(run_single_simulation, i) for i in range(len(config_files))]
        
        results = []
        for future in futures:
            results.append(future.result())
            
        return results

def create_experiment_suite(base_config, num_variants=5):
    """Create a suite of experiment variants"""
    experiments = []
    
    for i in range(num_variants):
        config = pynlm.createBrainConfig(base_config)
        
        # Create variant with different parameters
        if i == 0:
            config.set("brain.neuron_count", 500)
            config.set("plasticity.stdp.learning_rate", 0.001)
        elif i == 1:
            config.set("brain.neuron_count", 1000)
            config.set("plasticity.stdp.learning_rate", 0.01)
            config.set("plasticity.structural.enable", True)
        elif i == 2:
            config.set("brain.neuron_count", 2000)
            config.set("plasticity.stdp.learning_rate", 0.005)
            config.set("neuromodulation.curiosity.enable", True)
        elif i == 3:
            config.set("brain.neuron_count", 1500)
            config.set("plasticity.stdp.learning_rate", 0.02)
            config.set("plasticity.hebbian.enable", True)
        else:
            config.set("brain.neuron_count", 3000)
            config.set("plasticity.stdp.learning_rate", 0.01)
            config.set("plasticity.structural.enable", True)
            config.set("plasticity.hebbian.enable", True)
        
        config.saveToFile(f"experiment_config_{i}.cfg")
        experiments.append(config)
        
    return experiments
```

### Performance Optimization

#### Memory-Efficient Simulation

```python
import pynlm
import gc
import psutil
import os

class MemoryEfficientSimulation:
    def __init__(self, max_memory_gb=8):
        self.max_memory = max_memory_gb * 1024 * 1024 * 1024
        self.process = psutil.Process(os.getpid())
        
    def run_efficiently(self, config, num_steps):
        """Run simulation with memory monitoring"""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(num_steps):
            # Monitor memory
            memory_info = self.process.memory_info()
            if memory_info.rss > self.max_memory:
                self._cleanup_memory()
                
            # Run step
            brain.step(step)
            
            # Optional: periodically save and reload to free memory
            if step % 1000 == 0:
                self._checkpoint_and_reload(brain)
                
        return brain
        
    def _cleanup_memory(self):
        """Force garbage collection"""
        gc.collect()
        
    def _checkpoint_and_reload(self, brain):
        """Save and reload to free memory"""
        checkpoint_file = f"checkpoint_{int(brain.getTotalSpikeCount())}.bin"
        brain.save(checkpoint_file)
        
        # Clear references
        del brain
        gc.collect()
        
        # Reload
        brain = pynlm.createBrain(brain.getConfig())
        brain.load(checkpoint_file)
        return brain
```

#### Real-Time Monitoring and Control

```python
import pynlm
import time
import threading

class RealTimeMonitor:
    def __init__(self, brain, world, agent, update_interval=1.0):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.update_interval = update_interval
        self.monitoring = False
        self.stats_history = []
        self.thread = None
        
    def start_monitoring(self):
        """Start real-time monitoring in background thread"""
        self.monitoring = True
        self.thread = threading.Thread(target=self._monitor_loop)
        self.thread.daemon = True
        self.thread.start()
        
    def stop_monitoring(self):
        """Stop monitoring"""
        self.monitoring = False
        if self.thread:
            self.thread.join(timeout=1.0)
            
    def _monitor_loop(self):
        """Main monitoring loop"""
        while self.monitoring:
            start_time = time.time()
            
            # Collect stats
            stats = {
                'timestamp': time.time(),
                'simulation_time': self.world.getSimulationTime(),
                'neurons_firing': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'average_firing_rate': self.brain.getAverageFiringRate(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'development_stage': self.brain.getDevelopmentalStage()
            }
            
            self.stats_history.append(stats)
            
            # Keep only recent history
            if len(self.stats_history) > 1000:
                self.stats_history.pop(0)
                
            # Sleep for update interval
            elapsed = time.time() - start_time
            time.sleep(max(0, self.update_interval - elapsed))
            
    def analyze_performance(self):
        """Analyze collected performance data"""
        if not self.stats_history:
            return None
            
        spikes_per_second = []
        firing_rates = []
        
        for i in range(1, len(self.stats_history)):
            prev = self.stats_history[i-1]
            curr = self.stats_history[i]
            
            time_diff = curr['timestamp'] - prev['timestamp']
            if time_diff > 0:
                spike_increase = curr['total_spikes'] - prev['total_spikes']
                spikes_per_second.append(spike_increase / time_diff)
                firing_rates.append(curr['average_firing_rate'])
                
        return {
            'avg_spikes_per_second': np.mean(spikes_per_second) if spikes_per_second else 0,
            'avg_firing_rate': np.mean(firing_rates) if firing_rates else 0,
            'max_firing_rate': np.max(firing_rates) if firing_rates else 0,
            'monitoring_duration': self.stats_history[-1]['timestamp'] - self.stats_history[0]['timestamp']
        }
```

### Advanced Configuration Management

#### Configuration Factory Pattern

```python
import pynlm
from abc import ABC, abstractmethod
import json

class ConfigFactory(ABC):
    @abstractmethod
    def create_config(self, params):
        pass

class DevelopmentalConfigFactory(ConfigFactory):
    def create_config(self, params):
        config = pynlm.createDefaultConfig()
        
        # Base configuration
        config.set("brain.neuron_count", params.get("neuron_count", 1000))
        config.set("brain.synapse_density", params.get("synapse_density", 0.1))
        
        # Development-specific settings
        config.set("development.system.enabled", True)
        config.set("development.critical_period.active", params.get("critical_period", True))
        config.set("development.plastic_window", params.get("plastic_window", 100.0))
        
        # Neuromodulation for development
        config.set("neuromodulation.curiosity.enable", True)
        config.set("neuromodulation.novelty.enable", True)
        
        return config

class LearningConfigFactory(ConfigFactory):
    def create_config(self, params):
        config = pynlm.createDefaultConfig()
        
        # Learning-focused configuration
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", params.get("stdp_rate", 0.01))
        config.set("plasticity.stdp.tau_plus", params.get("tau_plus", 20.0))
        config.set("plasticity.stdp.tau_minus", params.get("tau_minus", 20.0))
        
        config.set("plasticity.hebbian.enable", params.get("hebbian_enabled", True))
        config.set("plasticity.hebbian.learning_rate", params.get("hebbian_rate", 0.005))
        
        # Memory systems for learning
        config.set("memory.working_memory.capacity", params.get("working_memory", 100))
        config.set("memory.episodic_memory.capacity", params.get("episodic_memory", 1000))
        
        return config

class ConfigManager:
    def __init__(self):
        self.factories = {
            'development': DevelopmentalConfigFactory(),
            'learning': LearningConfigFactory(),
            'balanced': lambda params: self._merge_configs(
                DevelopmentalConfigFactory().create_config(params),
                LearningConfigFactory().create_config(params)
            )
        }
        
    def create_config(self, config_type, params=None):
        if config_type not in self.factories:
            raise ValueError(f"Unknown config type: {config_type}")
            
        if params is None:
            params = {}
            
        return self.factories[config_type].create_config(params)
        
    def _merge_configs(self, config1, config2):
        """Merge two configurations"""
        merged = pynlm.createBrainConfig()
        
        # Merge all settings from both configs
        for key in config1.getKeys():
            merged.set(key, config1.get(key))
            
        for key in config2.getKeys():
            merged.set(key, config2.get(key))
            
        return merged
```

### Advanced Experimentation

#### Systematic Experimentation Framework

```python
import pynlm
import json
import os
from pathlib import Path

class ExperimentSystem:
    def __init__(self, base_dir="experiments"):
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
        self.results_dir = self.base_dir / "results"
        self.configs_dir = self.base_dir / "configs"
        self.logs_dir = self.base_dir / "logs"
        
        for directory in [self.results_dir, self.configs_dir, self.logs_dir]:
            directory.mkdir(exist_ok=True)
            
    def run_parameter_sweep(self, base_config, parameters, experiment_name):
        """Run parameter sweep experiment"""
        experiment_dir = self.base_dir / experiment_name
        experiment_dir.mkdir(exist_ok=True)
        
        results = []
        
        # Generate parameter combinations
        param_combinations = self._generate_parameter_combinations(parameters)
        
        for i, params in enumerate(param_combinations):
            print(f"Running experiment {i+1}/{len(param_combinations)}")
            
            # Create experiment configuration
            config = self._apply_parameters(base_config, params)
            config_file = experiment_dir / f"config_{i}.cfg"
            config.saveToFile(str(config_file))
            
            # Run experiment
            result = self._run_single_experiment(config, params, experiment_dir)
            results.append(result)
            
            # Save result
            self._save_experiment_result(result, experiment_dir / f"result_{i}.json")
            
        # Generate summary
        self._generate_experiment_summary(results, experiment_dir / "summary.json")
        
        return results
        
    def _generate_parameter_combinations(self, parameters):
        """Generate all combinations of parameters"""
        # Simple implementation - expand to include nested parameters
        param_names = list(parameters.keys())
        param_values = list(parameters.values())
        
        # Generate combinations
        from itertools import product
        combinations = []
        
        for combination in product(*param_values):
            params = dict(zip(param_names, combination))
            combinations.append(params)
            
        return combinations
        
    def _apply_parameters(self, base_config, params):
        """Apply parameters to configuration"""
        config = pynlm.createBrainConfig(base_config)
        
        for key, value in params.items():
            # Convert string keys to config format
            config_key = self._string_to_config_key(key)
            config.set(config_key, value)
            
        return config
        
    def _string_to_config_key(self, key):
        """Convert string parameter name to config key"""
        # Convert snake_case to config format
        return key.replace('_', '.')
        
    def _run_single_experiment(self, config, params, experiment_dir):
        """Run a single experiment"""
        # Setup logging
        log_file = experiment_dir / f"log_{len(list(experiment_dir.glob('log_*.txt')))}.txt"
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        max_steps = config.getOr("simulation.max_steps", 1000)
        
        for step in range(max_steps):
            brain.step(step)
            
            # Log progress periodically
            if step % 100 == 0:
                self._log_experiment_progress(
                    log_file, step, brain, params
                )
                
        # Collect results
        result = {
            'parameters': params,
            'final_state': {
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'neuron_count': brain.getTotalNeuronCount()
            },
            'learning_metrics': self._calculate_learning_metrics(brain),
            'config_used': config.getAllKeys()
        }
        
        return result
        
    def _log_experiment_progress(self, log_file, step, brain, params):
        """Log experiment progress"""
        with open(log_file, 'a') as f:
            f.write(f"Step {step}: ")
            f.write(f"Spikes={brain.getTotalSpikeCount()}, ")
            f.write(f"Firing={brain.getFiringNeuronCount()}, ")
            f.write(f"Rate={brain.getAverageFiringRate():.3f}, ")
            f.write(f"Params={params}\n")
            
    def _calculate_learning_metrics(self, brain):
        """Calculate learning-related metrics"""
        return {
            'synaptic_plasticity': brain.getStructuralPlasticity().getPlasticityLevel(),
            'learning_efficiency': brain.getExcitationInhibitionRatio(),
            'memory_utilization': brain.getWorkingMemory().getMemoryUtilization(),
            'development_progress': brain.getDevelopmentalStage()
        }
        
    def _save_experiment_result(self, result, filepath):
        """Save experiment result to JSON file"""
        with open(filepath, 'w') as f:
            json.dump(result, f, indent=2)
            
    def _generate_experiment_summary(self, results, filepath):
        """Generate experiment summary"""
        summary = {
            'total_experiments': len(results),
            'parameter_space_size': len(results[0]['parameters']) if results else 0,
            'performance_summary': self._calculate_performance_summary(results),
            'config_analysis': self._analyze_configurations(results),
            'best_performers': self._identify_best_performers(results)
        }
        
        with open(filepath, 'w') as f:
            json.dump(summary, f, indent=2)
            
    def _calculate_performance_summary(self, results):
        """Calculate performance summary"""
        if not results:
            return {}
            
        firing_rates = [r['final_state']['firing_rate'] for r in results]
        spikes = [r['final_state']['total_spikes'] for r in results]
        
        return {
            'avg_firing_rate': sum(firing_rates) / len(firing_rates),
            'max_firing_rate': max(firing_rates),
            'min_firing_rate': min(firing_rates),
            'avg_spikes': sum(spikes) / len(spikes),
            'successful_experiments': sum(1 for r in results if r['final_state']['firing_rate'] > 0)
        }
        
    def _analyze_configurations(self, results):
        """Analyze used configurations"""
        all_configs = []
        for result in results:
            all_configs.extend(result['config_used'])
            
        config_counts = {}
        for config in all_configs:
            config_counts[config] = config_counts.get(config, 0) + 1
            
        return {
            'most_common_configs': sorted(config_counts.items(), key=lambda x: x[1], reverse=True)[:10],
            'unique_configs': len(set(all_configs))
        }
        
    def _identify_best_performers(self, results, top_n=5):
        """Identify best performing experiments"""
        # Sort by firing rate (as a proxy for performance)
        sorted_results = sorted(results, key=lambda x: x['final_state']['firing_rate'], reverse=True)
        
        return [
            {
                'rank': i+1,
                'parameters': r['parameters'],
                'firing_rate': r['final_state']['firing_rate'],
                'total_spikes': r['final_state']['total_spikes']
            }
            for i, r in enumerate(sorted_results[:top_n])
        ]
```

### Configuration Schema Documentation

#### Brain Configuration Schema

```python
# Brain Configuration Parameters
brain_config_schema = {
    'neuron_count': {
        'type': 'integer',
        'min': 100,
        'max': 100000,
        'default': 1000,
        'description': 'Total number of neurons in the brain'
    },
    'synapse_density': {
        'type': 'float',
        'min': 0.0,
        'max': 1.0,
        'default': 0.1,
        'description': 'Probability of synapse formation between neurons'
    },
    'connection_probability': {
        'type': 'float',
        'min': 0.0,
        'max': 1.0,
        'default': 0.05,
        'description': 'Base probability for random connections'
    },
    'initial_weight_mean': {
        'type': 'float',
        'min': 0.0,
        'max': 10.0,
        'default': 0.5,
        'description': 'Mean value for initial synaptic weights'
    },
    'v_thresh': {
        'type': 'float',
        'default': -50.0,
        'description': 'Neutron threshold potential (mV)'
    },
    'v_rest': {
        'type': 'float',
        'default': -70.0,
        'description': 'Resting potential (mV)'
    },
    'tau_mem': {
        'type': 'float',
        'default': 20.0,
        'description': 'Membrane time constant (ms)'
    },
    'tau_ref': {
        'type': 'float',
        'default': 2.0,
        'description': 'Refractory period (ms)'
    }
}
```

#### Plasticity Configuration Schema

```python
plasticity_config_schema = {
    'stdp.enable': {
        'type': 'boolean',
        'default': True,
        'description': 'Enable Spike-Timing-Dependent Plasticity'
    },
    'stdp.learning_rate': {
        'type': 'float',
        'min': 0.0,
        'max': 1.0,
        'default': 0.001,
        'description': 'STDP learning rate'
    },
    'stdp.tau_plus': {
        'type': 'float',
        'default': 20.0,
        'description': 'STDP time constant for potentiation (ms)'
    },
    'stdp.tau_minus': {
        'type': 'float',
        'default': 20.0,
        'description': 'STDP time constant for depression (ms)'
    },
    'hebbian.enable': {
        'type': 'boolean',
        'default': True,
        'description': 'Enable Hebbian learning'
    },
    'hebbian.learning_rate': {
        'type': 'float',
        'min': 0.0,
        'max': 1.0,
        'default': 0.005,
        'description': 'Hebbian learning rate'
    },
    'structural.enable': {
        'type': 'boolean',
        'default': True,
        'description': 'Enable structural plasticity (synaptogenesis/pruning)'
    },
    'structural.synaptogenesis_rate': {
        'type': 'float',
        'min': 0.0,
        'max': 0.01,
        'default': 0.001,
        'description': 'Rate of new synapse formation'
    },
    'structural.pruning_rate': {
        'type': 'float',
        'min': 0.0,
        'max': 0.001,
        'default': 0.0001,
        'description': 'Rate of synapse elimination'
    }
}
```

### Debugging and Advanced Troubleshooting

#### Advanced Debugging Tools

```python
import pynlm
import traceback
import sys

class AdvancedDebugger:
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.breakpoints = []
        self.watches = []
        self.snapshot_count = 0
        
    def add_breakpoint(self, condition_func, description):
        """Add a conditional breakpoint"""
        self.breakpoints.append({
            'condition': condition_func,
            'description': description,
            'triggered': False
        })
        
    def add_watch(self, expression_func, name):
        """Add an expression to monitor"""
        self.watches.append({
            'expression': expression_func,
            'name': name,
            'values': []
        })
        
    def take_snapshot(self):
        """Take a snapshot of current brain state"""
        self.snapshot_count += 1
        snapshot_file = f"debug_snapshot_{self.snapshot_count}.bin"
        self.brain.save(snapshot_file)
        
        snapshot_data = {
            'step': self.brain.getTotalSpikeCount(),
            'timestamp': time.time(),
            'snapshot_file': snapshot_file,
            'brain_stats': {
                'neurons': self.brain.getTotalNeuronCount(),
                'synapses': self.brain.getTotalSynapseCount(),
                'spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate()
            }
        }
        
        return snapshot_data
        
    def compare_snapshots(self, snapshot1_file, snapshot2_file):
        """Compare two snapshots"""
        # Load and compare snapshots
        snap1 = self._load_snapshot(snapshot1_file)
        snap2 = self._load_snapshot(snapshot2_file)
        
        comparison = {
            'spike_count_change': snap2['spikes'] - snap1['spikes'],
            'synapse_count_change': snap2['synapses'] - snap1['synapses'],
            'firing_rate_change': snap2['firing_rate'] - snap1['firing_rate'],
            'time_elapsed': snap2['timestamp'] - snap1['timestamp']
        }
        
        return comparison
        
    def _load_snapshot(self, snapshot_file):
        """Load snapshot data"""
        # Implementation would load the binary snapshot and extract key metrics
        pass
        
    def debug_step(self, step_number):
        """Debug a specific step"""
        try:
            # Execute step with debugging
            self.brain.step(step_number)
            
            # Check breakpoints
            for breakpoint in self.breakpoints:
                if not breakpoint['triggered'] and breakpoint['condition'](self.brain):
                    breakpoint['triggered'] = True
                    print(f"BREAKPOINT TRIGGERED: {breakpoint['description']}")
                    self._enter_debug_mode()
                    
            # Update watches
            for watch in self.watches:
                watch['values'].append(watch['expression'](self.brain))
                if len(watch['values']) > 100:
                    watch['values'].pop(0)
                    
        except Exception as e:
            print(f"Error during debug step {step_number}:")
            traceback.print_exc()
            raise
            
    def _enter_debug_mode(self):
        """Enter interactive debug mode"""
        print("Debug mode - available commands:")
        print("  s - step forward")
        print("  n - continue normal execution")
        print("  p - print watch values")
        print("  w <expr> - add watch expression")
        print("  b <desc> - add breakpoint")
        print("  snap - take snapshot")
        print("  comp - compare snapshots")
        print("  exit - return to normal execution")
        
        # Interactive debug loop would go here
```

#### Advanced Error Handling

```python
import pynlm
from enum import Enum

class ErrorSeverity(Enum):
    INFO = 1
    WARNING = 2
    ERROR = 3
    CRITICAL = 4

class AdvancedErrorHandler:
    def __init__(self):
        self.error_log = []
        self.error_handlers = {
            ErrorSeverity.INFO: self._handle_info,
            ErrorSeverity.WARNING: self._handle_warning,
            ErrorSeverity.ERROR: self._handle_error,
            ErrorSeverity.CRITICAL: self._handle_critical
        }
        
    def log_error(self, message, severity=ErrorSeverity.ERROR, 
                  context=None, recoverable=True):
        """Log an error with context"""
        error_entry = {
            'timestamp': time.time(),
            'message': message,
            'severity': severity,
            'context': context,
            'recoverable': recoverable,
            'stack_trace': traceback.format_stack()
        }
        
        self.error_log.append(error_entry)
        
        # Process error
        if severity in self.error_handlers:
            self.error_handlers[severity](error_entry)
            
        # Attempt recovery if possible
        if recoverable and severity in [ErrorSeverity.WARNING, ErrorSeverity.ERROR]:
            self._attempt_recovery(error_entry)
            
    def _handle_info(self, error_entry):
        print(f"[INFO] {error_entry['message']}")
        
    def _handle_warning(self, error_entry):
        print(f"[WARNING] {error_entry['message']}")
        
    def _handle_error(self, error_entry):
        print(f"[ERROR] {error_entry['message']}")
        
    def _handle_critical(self, error_entry):
        print(f"[CRITICAL] {error_entry['message']}")
        print("System may be unstable - consider checkpointing")
        
    def _attempt_recovery(self, error_entry):
        """Attempt to recover from error"""
        # Implement recovery logic based on error type
        if "memory" in error_entry['message'].lower():
            self._recover_memory_issue()
        elif "plasticity" in error_entry['message'].lower():
            self._recover_plasticity_issue()
        elif "synapse" in error_entry['message'].lower():
            self._recover_synapse_issue()
            
    def _recover_memory_issue(self):
        """Recover from memory-related errors"""
        # Force garbage collection
        import gc
        gc.collect()
        
        # Clear unnecessary data
        if hasattr(self, 'large_data_buffer'):
            del self.large_data_buffer
            
    def _recover_plasticity_issue(self):
        """Recover from plasticity-related errors"""
        # Reset plasticity system if possible
        if hasattr(self, 'brain') and self.brain:
            self.brain.getStructuralPlasticity().reset()
            
    def _recover_synapse_issue(self):
        """Recover from synapse-related errors"""
        # Check synapse integrity and repair if needed
        pass
        
    def get_error_summary(self):
        """Get summary of errors"""
        if not self.error_log:
            return {'total_errors': 0}
            
        severity_counts = {}
        for error in self.error_log:
            severity = error['severity']
            severity_counts[severity.value] = severity_counts.get(severity.value, 0) + 1
            
        return {
            'total_errors': len(self.error_log),
            'severity_counts': severity_counts,
            'recent_errors': self.error_log[-10:] if len(self.error_log) > 10 else self.error_log,
            'most_common_messages': self._get_most_common_messages()
        }
        
    def _get_most_common_messages(self):
        """Get most common error messages"""
        message_counts = {}
        for error in self.error_log:
            message = error['message']
            message_counts[message] = message_counts.get(message, 0) + 1
            
        return sorted(message_counts.items(), key=lambda x: x[1], reverse=True)[:5]
```

### Summary

These advanced features provide power users with:

1. **Powerful configuration management** with factory patterns
2. **Batch processing capabilities** for large-scale experiments
3. **Real-time monitoring and performance analysis**
4. **Memory-efficient simulation techniques**
5. **Advanced debugging tools with snapshot comparison**
6. **Robust error handling and recovery systems**
7. **Comprehensive configuration schema documentation**
8. **Systematic experimentation frameworks**

The advanced examples demonstrate how to extend NLM for complex research scenarios, large-scale parameter sweeps, and production-level simulation management. They build upon the basic concepts introduced in the beginner guide while providing the tools needed for sophisticated neural network experimentation.

By mastering these advanced techniques, users can create more complex and realistic brain simulations, conduct systematic scientific experiments, and integrate NLM into larger research workflows or production environments.

Remember: Advanced features should be used once you have a solid understanding of the basic NLM concepts and operations.

### Next Steps

After mastering the advanced features:

1. Explore the C++ API for even deeper control
2. Read `docs/ARCHITECTURE.md` for system design insights
3. Review `docs/SCIENCE.md` for scientific background
4. Experiment with the advanced agent behaviors and configurations
5. Set up your own research experiments using the systematic frameworks provided

The NLM system is now equipped with powerful tools for both beginners and advanced users!

---
