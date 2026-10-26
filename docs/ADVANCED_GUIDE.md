# NLM - Advanced Usage Guide

This guide covers advanced usage patterns, API reference, and complex scenarios for the NLM (Neural Learning Machine) framework.

## 1. Advanced Agent Architecture

### 1.1 Multi-Modal Agent with Integrated Systems

This example shows how to create an agent that integrates all NLM systems for complex behavior:

```python
import pynlm
import numpy as np

class AdvancedAgent:
    def __init__(self):
        # Create configuration with all systems enabled
        config = pynlm.createDefaultConfig()
        
        # Optimize for complex tasks
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world with complex sensory capabilities
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=50, height=50,
            visionWidth=16, visionHeight=16
        )
        self.world.reset()
        self.world.setAgentStart(25.0, 25.0)
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Enable all subsystems for maximum capability
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Track agent performance
        self.episode_count = 0
        self.total_reward = 0.0
        self.steps_without_learning = 0
        
    def run_episode(self, max_steps=1000):
        """Run a complete episode with the advanced agent."""
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get comprehensive sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input through all brain systems
            self.agent.processSensoryInput(percept)
            
            # Run brain step with prediction and attention
            self.brain.step(step)
            
            # Decode motor command from brain's action selection
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get internal reward signals
            internal_signals = percept.getInternal()
            reward = internal_signals[0] if internal_signals else 0.0
            
            # Apply reward modulation (learning from experience)
            predicted_reward = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Update development (brain maturation)
            self.agent.updateDevelopment(0.1)
            
            # Track performance
            self.total_reward += reward
            
            if self.agent.getCuriosityLevel() < 0.1:
                self.steps_without_learning += 1
            else:
                self.steps_without_learning = 0
            
            # Print progress
            if step % 100 == 0:
                self.print_status(step, reward)
                
                # Save checkpoint periodically
                if step % 500 == 0:
                    self.save_checkpoint(step)
        
        self.episode_count += 1
        return self.total_reward / max_steps
    
    def print_status(self, step, reward):
        """Print detailed agent status."""
        print(f"Step {step}:")
        print(f"  Position: {self.world.getAgentBody().x:.1f}, {self.world.getAgentBody().y:.1f}")
        print(f"  Reward: {reward:.3f}")
        print(f"  Total Reward: {self.total_reward:.3f}")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Prediction Error: {self.agent.getPredictionError():.3f}")
        print(f"  Development Stage: {self.brain.getDevelopmentalStage()}")
        print(f"  Neurons Firing: {self.brain.getFiringNeuronCount()}")
        print(f"  Total Spikes: {self.brain.getTotalSpikeCount()}")
        
    def save_checkpoint(self, step):
        """Save agent brain state for later restoration."""
        filename = f"checkpoint_episode_{self.episode_count}_step_{step}.bin"
        self.brain.save(filename)
        print(f"  Checkpoint saved: {filename}")
        
    def load_checkpoint(self, filename):
        """Load agent brain state from checkpoint."""
        self.brain.load(filename)
        print(f"Checkpoint loaded: {filename}")

# Run advanced agent demonstration
agent = AdvancedAgent()
print("Running advanced agent with integrated NLM systems...")
avg_reward = agent.run_episode(1000)
print(f"Episode completed. Average reward: {avg_reward:.3f}")
```

### 1.2 Learning from Memory Replay

This example demonstrates how to use the episodic memory replay system for accelerated learning:

```python
import pynlm
import time

class MemoryReplayAgent:
    def __init__(self):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.12)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        
        # Enable memory systems
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        
        # Configure replay parameters
        self.replay_interval = 50  # Replay every 50 steps
        self.consolidation_interval = 200  # Consolidate every 200 steps
        
    def run_with_memory_replay(self, max_steps=500):
        """Run simulation with memory replay for accelerated learning."""
        for step in range(max_steps):
            # Normal operation
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            self.agent.applyRewardModulation(reward, self.agent.getPredictionError())
            self.agent.updateDevelopment(0.1)
            
            # Memory replay phase
            if step % self.replay_interval == 0 and step > 0:
                self.replay_recent_memories(step)
            
            # Memory consolidation phase
            if step % self.consolidation_interval == 0 and step > 0:
                self.consolidate_memories(step)
            
            if step % 100 == 0:
                print(f"Step {step}: Replay={self.replay_interval}, Consolidation={self.consolidation_interval}")
                
    def replay_recent_memories(self, step):
        """Replay recent episodic memories to strengthen learning."""
        # Access episodic memory through brain
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            # Get recent episodes for replay
            episodes_to_replay = episodic_memory.getEpisodesForReplay(5)
            
            print(f"  Replaying {len(episodes_to_replay)} episodes...")
            
            # Replay each episode
            for episode in episodes_to_replay:
                episodic_memory.replayEpisode(episode)
                
    def consolidate_memories(self, step):
        """Consolidate important memories and remove weak ones."""
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            # Consolidate with a threshold
            episodic_memory.consolidate(0.3)
            print(f"  Consolidated memories at step {step}")

# Demonstrate memory replay
agent = MemoryReplayAgent()
print("Running memory replay demonstration...")
agent.run_with_memory_replay(500)
```

## 2. Python API Reference

### 2.1 Core Classes

#### Brain Class

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain with configuration | None | bool |
| `step(step)` | Perform simulation step | `step` (SimulationStep) | None |
| `step(step, time)` | Perform step with timestamp | `step` (SimulationStep), `time` (Timestamp) | None |
| `receiveSensoryInput(input)` | Inject sensory input | `input` (SensoryInput) | None |
| `injectCurrent(neuron, current)` | Inject current into neuron | `neuron` (NeuronId), `current` (MembranePotential) | None |
| `injectCurrentToNeurons(type, current)` | Inject current to neurons by type | `type` (NeuronType), `current` (MembranePotential) | None |
| `produceAction()` | Generate motor action | None | Action |
| `reset()` | Reset brain state | None | None |
| `save(filepath)` | Save brain state | `filepath` (str) | bool |
| `load(filepath)` | Load brain state | `filepath` (str) | bool |
| `addRegion(name)` | Add neural region | `name` (str, optional) | RegionId |
| `getRegion(id)` | Get region by ID | `id` (RegionId) | NeuralRegion* |
| `getTotalNeuronCount()` | Get total neuron count | None | size_t |
| `getTotalSynapseCount()` | Get total synapse count | None | size_t |
| `getFiringNeuronCount()` | Get currently firing neurons | None | size_t |
| `getTotalSpikeCount()` | Get total spike count | None | size_t |
| `getAverageFiringRate()` | Get average firing rate | None | float |
| `getExcitationInhibitionRatio()` | Get E/I balance | None | float |

**Advanced Methods:**

```python
# Memory systems
brain.getWorkingMemory()          # NeuralWorkingMemory*
brain.getEpisodicMemory()         # NeuralEpisodicMemory*
brain.getAssociativeMemory()      # NeuralAssociativeMemory*

# Prediction system
brain.getPredictionSystem()       # PredictionSystem*

# Cognitive systems
brain.getPlanner()                # NeuralPlanner*
brain.getConceptFormation()       # ConceptFormation*
brain.getAttention()              # AttentionalSelection*

# Development system
brain.getDevelopmentSystem()      # DevelopmentSystem*

# Neuromodulation systems
brain.getDopamine()              # Dopamine*
brain.getCuriosity()              # Curiosity*
brain.getNovelty()                # Novelty*
brain.getPredictionErrorSignal()  # PredictionError*

# Plasticity systems
brain.getSTDP()                   # STDP*
brain.getHebbian()                # Hebbian*
brain.getStructuralPlasticity()   # StructuralPlasticity*

# Advanced queries
brain.getMostActiveNeurons(10)    # List[NeuronId]
brain.getNeuronsByType(NeuronType.Excitatory)  # List[NeuronId]
brain.getSynapsesBetween(source_id, target_id)  # List[SynapseId]
```

#### Config Class

**Methods:**

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `loadFromFile(filepath)` | Load from JSON/YAML/file | `filepath` (str) | bool |
| `loadFromJSON(str)` | Load from JSON string | `str` (str) | bool |
| `loadFromYAML(filepath)` | Load from YAML file | `filepath` (str) | bool |
| `loadFromEnvironment()` | Load from env vars | None | None |
| `saveToFile(filepath)` | Save to file | `filepath` (str) | bool |
| `validate()` | Validate configuration | None | bool |
| `generateTemplate()` | Generate config template | None | str |
| `has(key)` | Check if key exists | `key` (str) | bool |
| `get(key)` | Get value | `key` (str) | Optional[ConfigValue] |
| `getOr(key, default)` | Get with default | `key` (str), `default` (T) | T |
| `set(key, value, source)` | Set value | `key` (str), `value` (ConfigValue), `source` (ConfigSource) | None |

#### AgentBrain Class

**Methods:**

```python
# Core agent functionality
agent.initialize(world)                    # Initialize with world
agent.processSensoryInput(percept)         # Process sensory input
agent.decodeMotorCommand()                 # Get motor command
agent.applyRewardModulation(reward, predicted)  # Apply reward
agent.updateDevelopment(timestep)          # Update development
agent.reset()                              # Reset for new episode

# Subsystem control
agent.enableRewardModulation(True/False)   # Enable/disable reward
agent.enableStructuralPlasticity(True/False)# Enable/disable structural plasticity
agent.enableDevelopment(True/False)        # Enable/disable development
agent.enableCuriosity(True/False)          # Enable/disable curiosity

# Query methods
agent.getCuriosityLevel()                   # float
agent.getNoveltyLevel()                     # float
agent.getPredictionError()                  # float
agent.getDevelopmentalStage()              # DevelopmentalStage
agent.getNeuromodulationLevel()             # float
agent.isRewardModulationEnabled()           # bool
agent.isDevelopmentEnabled()                # bool
```

#### SimpleWorld Class

```python
world = pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)
world.reset()
world.setAgentStart(x, y)
world.update(timestep)
world.applyMotorCommand(action, time)
percept = world.getSensoryPercept()
body = world.getAgentBody()
world.addObject(obj)
world.removeObject(x, y)
world.isValidPosition(x, y)
```

### 2.2 Enumerations

All enumerations are available as Python classes:

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult

# World object types
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
```

### 2.3 Error Handling

Python bindings include proper error handling:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.ValueError as e:
    print(f"Value error: {e}")
except pynlm.IndexError as e:
    print(f"Index error: {e}")
```

## 3. Configuration Examples

### 3.1 JSON Configuration

```json
{
  "brain": {
    "neuron_count": 2000,
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
      "enable": true
    },
    "structural": {
      "enable": true
    }
  },
  
  "neuromod": {
    "dopamine": {
      "scale": 1.5
    },
    "curiosity": {
      "enable": true
    },
    "novelty": {
      "enable": true
    }
  },
  
  "development": {
    "initial_plasticity_rate": 1.0,
    "maturation_factor": 0.5
  }
}
```

### 3.2 YAML Configuration

```yaml
# NLM Configuration Example
brain:
  neuron_count: 2000
  synapse_density: 0.15
  connection_probability: 0.05
  v_thresh: -50.0
  v_rest: -70.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
  structural:
    enable: true

neuromod:
  dopamine:
    scale: 1.5
  curiosity:
    enable: true
  novelty:
    enable: true

development:
  initial_plasticity_rate: 1.0
  maturation_factor: 0.5
```

### 3.3 Advanced Configuration

```python
import pynlm

# Create and configure advanced brain
config = pynlm.createDefaultConfig()

# Optimize for learning
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)

# Enable all neuromodulation
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Development settings
config.set("development.initial_plasticity_rate", 1.5)
config.set("development.maturation_factor", 0.3)

# Environment variable override
import os
os.environ["NLM_NEURON_COUNT"] = "5000"
os.environ["NLM_LOG_LEVEL"] = "DEBUG"

# Create brain with enhanced configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

## 4. Performance Optimization Tips

### 4.1 Memory Management

```python
# Monitor memory usage
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")

# Periodically save checkpoints for long runs
if step % 1000 == 0:
    brain.save(f"checkpoint_step_{step}.bin")
```

### 4.2 Simulation Efficiency

```python
# Use batch processing for multiple steps
for batch_step in range(0, total_steps, batch_size):
    end_step = min(batch_step + batch_size, total_steps)
    for step in range(batch_step, end_step):
        brain.step(step)
    
    # Print batch statistics
    print(f"Batch {batch_step}-{end_step-1}: {brain.getFiringNeuronCount()} firing neurons")
```

### 4.3 Parallel Processing

```python
# Process multiple agents in parallel (for research)
import threading
import queue

def process_agent(agent_config, results_queue):
    agent = AdvancedAgent(agent_config)
    reward = agent.run_episode(100)
    results_queue.put((agent_config, reward))

# Create multiple agent instances with different configurations
configs = [
    {"neurons": 1000, "plasticity": "stdp"},
    {"neurons": 2000, "plasticity": "hebbian"},
    {"neurons": 1500, "plasticity": "both"}
]

# Process in parallel
results_queue = queue.Queue()
threads = []

for config in configs:
    thread = threading.Thread(target=process_agent, args=(config, results_queue))
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()

# Collect results
while not results_queue.empty():
    config, reward = results_queue.get()
    print(f"Config {config}: Reward = {reward:.3f}")
```

## 5. Research and Development Features

### 5.1 Experiment Control

```python
# Control experiment parameters
class ExperimentController:
    def __init__(self):
        self.config_template = self.load_config_template()
        self.experiment_params = {}
        
    def run_experiment(self, condition_name, params):
        """Run an experiment with specific parameters."""
        config = self.config_template.copy()
        config.update(params)
        
        # Create and run agent
        agent = AdvancedAgent(config)
        reward = agent.run_episode(params.get("duration", 500))
        
        # Record results
        self.record_results(condition_name, params, reward)
        return reward
    
    def compare_conditions(self, conditions):
        """Compare multiple experimental conditions."""
        results = {}
        
        for name, params in conditions.items():
            print(f"\nRunning condition: {name}")
            reward = self.run_experiment(name, params)
            results[name] = reward
            
        return results

# Define experimental conditions
conditions = {
    "baseline": {
        "brain.neuron_count": 1000,
        "plasticity.stdp.enable": True,
        "neuromod.dopamine.scale": 1.0
    },
    "enhanced_plasticity": {
        "brain.neuron_count": 1000,
        "plasticity.stdp.learning_rate": 0.005,
        "plasticity.hebbian.enable": True,
        "neuromod.curiosity.enable": True
    },
    "high_neural_density": {
        "brain.neuron_count": 3000,
        "brain.synapse_density": 0.25,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True
    }
}

# Run comparison
controller = ExperimentController()
results = controller.compare_conditions(conditions)

print("\nExperiment Results:")
for condition, reward in results.items():
    print(f"{condition}: {reward:.3f}")
```

### 5.2 Checkpoint Management

```python
class CheckpointManager:
    def __init__(self, brain, save_dir="./checkpoints"):
        self.brain = brain
        self.save_dir = save_dir
        self.checkpoint_index = 0
        
    def save_checkpoint(self, description=""):
        """Save brain state with timestamp."""
        import datetime
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        
        if description:
            filename = f"{self.save_dir}/checkpoint_{self.checkpoint_index:04d}_{timestamp}_{description}.bin"
        else:
            filename = f"{self.save_dir}/checkpoint_{self.checkpoint_index:04d}_{timestamp}.bin"
            
        if self.brain.save(filename):
            print(f"Checkpoint saved: {filename}")
            self.checkpoint_index += 1
            return True
        else:
            print(f"Failed to save checkpoint: {filename}")
            return False
    
    def load_latest_checkpoint(self):
        """Load the most recent checkpoint."""
        import glob
        checkpoint_files = glob.glob(f"{self.save_dir}/checkpoint_*.bin")
        
        if not checkpoint_files:
            print("No checkpoints found")
            return False
            
        latest_file = sorted(checkpoint_files)[-1]
        if self.brain.load(latest_file):
            print(f"Loaded checkpoint: {latest_file}")
            return True
        else:
            print(f"Failed to load checkpoint: {latest_file}")
            return False
    
    def restore_and_continue(self, target_step):
        """Load checkpoint and continue from specific step."""
        if self.load_latest_checkpoint():
            print(f"Resumed from checkpoint, continuing from step {target_step}")
            return True
        return False
```

## 6. Troubleshooting Guide

### 6.1 Common Issues

**Issue: Brain initialization fails**

```python
# Solution 1: Check configuration
config = pynlm.createDefaultConfig()
if not config.validate():
    print("Invalid configuration!")
    print(config.summary())
    # Fix configuration issues

# Solution 2: Check required parameters
required_keys = ["random_seed", "neuron_count", "region_count"]
for key in required_keys:
    if not config.has(key):
        print(f"Missing required key: {key}")
        config.set(key, 42)  # Set reasonable default
```

**Issue: Agent doesn't move**

```python
# Solution 1: Check world configuration
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5.0, 5.0)

# Solution 2: Check action generation
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
action = agent.decodeMotorCommand()
print(f"Generated action: {action}")
```

**Issue: Memory usage grows too high**

```python
# Solution 1: Monitor memory
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")

# Solution 2: Use smaller configurations for testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller for testing

# Solution 3: Periodic checkpoints
if step % 1000 == 0:
    brain.save(f"checkpoint_step_{step}.bin")
```

### 6.2 Debugging Techniques

```python
# Enable verbose logging
import pynlm
config = pynlm.createDefaultConfig()
# Set log level in config
config.set("log_level", "DEBUG")

# Custom logging function
def custom_log_handler(message):
    print(f"[NLM DEBUG] {message}")

# Run with debug output
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor specific metrics
print("=== Monitoring Start ===")
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Prediction error: {brain.getPredictionSystem().getPredictionError():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
print("=== Monitoring End ===")
```

## 7. Best Practices

### 7.1 Configuration Management

```python
# Always validate configuration
config = pynlm.createDefaultConfig()
if not config.validate():
    print("Configuration validation failed!")
    return False

# Use environment variables for deployment
import os
if "NLM_CONFIG_PATH" in os.environ:
    config.loadFromFile(os.environ["NLM_CONFIG_PATH"])

# Backup important configurations
config.save("backup_config.json")
```

### 7.2 Memory Management

```python
# Properly manage brain lifecycle
brain = pynlm.createBrain(config)
brain.initialize()

# Always save important states
try:
    # Run simulation
    for step in range(1000):
        brain.step(step)
        
        # Save progress periodically
        if step % 100 == 0:
            brain.save(f"progress_step_{step}.bin")
            
finally:
    # Clean up resources
    del brain
```

### 7.3 Experiment Reproducibility

```python
# Set random seed for reproducibility
config = pynlm.createDefaultConfig()
config.set("random_seed", 42)
config.set("log_level", "INFO")

# Record experiment metadata
experiment_metadata = {
    "config": config.summary(),
    "description": "Agent learning with NLM",
    "parameters": {
        "neuron_count": 1000,
        "plasticity_rate": 0.001,
        "duration_steps": 500
    }
}

# Save metadata for reproducibility
import json
with open("experiment_metadata.json", "w") as f:
    json.dump(experiment_metadata, f, indent=2)

# Run experiment
brain = pynlm.createBrain(config)
brain.initialize()

# Collect statistics
results = []
for step in range(500):
    brain.step(step)
    if step % 50 == 0:
        results.append({
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "prediction_error": brain.getPredictionSystem().getPredictionError()
        })

# Save results
with open("experiment_results.json", "w") as f:
    json.dump(results, f, indent=2)
```

## 8. Advanced Topics

### 8.1 Custom Learning Algorithms

```python
# Extend NLM with custom plasticity
class CustomPlasticity:
    def __init__(self):
        self.custom_rules = []
        
    def add_rule(self, condition, effect):
        self.custom_rules.append((condition, effect))
        
    def apply_custom_plasticity(self, synapse, brain_state):
        for condition, effect in self.custom_rules:
            if condition(synapse, brain_state):
                effect(synapse)

# Use custom plasticity with NLM
custom_plasticity = CustomPlasticity()
custom_plasticity.add_rule(is_high_activation, enhance_strength)
custom_plasticity.add_rule(is_predictive, strengthen_reward)

# This would integrate with the existing STDP/Hebbian systems
```

### 8.2 Multi-Agent Systems

```python
# Create multiple interacting agents
class MultiAgentSystem:
    def __init__(self, num_agents):
        self.agents = []
        self.shared_world = pynlm.createSimpleWorld()
        
        for i in range(num_agents):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            config.set("random_seed", i)  # Different seed for each agent
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.shared_world)
            
            self.agents.append(agent)
    
    def run_timestep(self):
        """Run all agents in the same world state."""
        # All agents share the same world
        self.shared_world.update(0.1)
        
        # Each agent processes the same sensory input
        percept = self.shared_world.getSensoryPercept()
        
        # Each agent makes decisions
        actions = []
        for agent in self.agents:
            agent.processSensoryInput(percept)
            action = agent.decodeMotorCommand()
            actions.append(action)
            
            # Apply first agent's action (for demonstration)
            if len(actions) == 1:  # Only first agent's action affects world
                self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
    
    def run_episode(self, steps):
        """Run multi-agent episode."""
        for step in range(steps):
            self.run_timestep()
            
            # Log agent interactions
            if step % 100 == 0:
                self.log_agent_interactions(step)
```

### 8.3 Statistical Analysis

```python
# Statistical analysis of agent performance
import numpy as np
import matplotlib.pyplot as plt

class PerformanceAnalyzer:
    def __init__(self, results):
        self.results = results
        
    def analyze_performance(self):
        """Analyze agent performance statistics."""
        rewards = [r['reward'] for r in self.results]
        firing_rates = [r['firing_rate'] for r in self.results]
        
        stats = {
            'reward_mean': np.mean(rewards),
            'reward_std': np.std(rewards),
            'reward_median': np.median(rewards),
            'firing_rate_mean': np.mean(firing_rates),
            'firing_rate_std': np.std(firing_rates),
            'performance_correlation': self.correlate_metrics()
        }
        
        return stats
    
    def correlate_metrics(self):
        """Correlate different performance metrics."""
        rewards = [r['reward'] for r in self.results]
        curiosity = [r['curiosity'] for r in self.results]
        
        if len(rewards) == len(curiosity):
            correlation = np.corrcoef(rewards, curiosity)[0, 1]
            return correlation
        return 0.0

# Example usage
analyzer = PerformanceAnalyzer(experiment_results)
performance_stats = analyzer.analyze_performance()

print("Performance Statistics:")
for metric, value in performance_stats.items():
    if isinstance(value, float):
        print(f"{metric}: {value:.3f}")
    else:
        print(f"{metric}: {value}")
```

## 9. Future Enhancements

### 9.1 Additional Features Roadmap

- **Distributed Computing**: Multi-GPU training for large-scale brains
- **Web Integration**: REST API for remote brain simulation
- **Visualization**: Interactive brain state visualization
- **Hardware Acceleration**: CUDA support for faster neural computation
- **Cloud Deployment**: Containerized deployment for scalability

### 9.2 Research Applications

- **Cognitive Modeling**: Study human-like cognitive development
- **Reinforcement Learning**: Advanced RL with intrinsic motivation
- ** Neuromodulation Research**: Study dopamine, curiosity, novelty effects
- **Memory Research**: Investigate memory consolidation and replay
- **Development Research**: Study critical periods and maturation

## 10. References

- NLM Documentation: https://nlm.readthedocs.io
- GitHub Repository: https://github.com/nlm-project/nlm
- Scientific Papers: https://arxiv.org/search/all?query=nlm+neural+learning+machine

---

*This guide provides comprehensive examples and reference material for using the NLM framework. For more detailed information, consult the specific documentation files and code comments.*