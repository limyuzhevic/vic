# Advanced NLM Usage Guide

## Introduction

Welcome to the advanced usage guide for NLM (Neural Learning Machine). This document covers advanced features and techniques for working with the NLM framework, including memory systems, prediction, neuromodulation, and advanced configuration options.

## Advanced Configuration

### 1. Advanced Configuration Setup

The NLM framework supports advanced configuration through the `createAdvancedConfig()` function, which sets up all Phase 6 features by default. Here's how to use it:

```python
import pynlm

# Create advanced configuration with all Phase 6 features enabled
config = pynlm.createAdvancedConfig()

# Override specific settings
config.set("brain.neuron_count", 5000)  # More neurons
config.set("plasticity.stdp.learning_rate", 0.005)  # Adjust learning rate
config.set("neuromod.curiosity.threshold", 0.3)  # Curiosity threshold

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. JSON and YAML Configuration Support

NLM now supports modern configuration formats:

#### JSON Configuration

```python
import pynlm
import json

config = pynlm.createDefaultConfig()
config.loadFromFile("advanced_config.json")
```

```json
{
  "brain": {
    "neuron_count": 2000,
    "synapse_density": 0.15
  },
  "plasticity": {
    "stdp": {"enable": true, "learning_rate": 0.01},
    "hebbian": {"enable": true},
    "structural": {"enable": true}
  },
  "neuromod": {
    "dopamine": {"scale": 1.0},
    "curiosity": {"enable": true, "threshold": 0.2},
    "novelty": {"enable": true}
  },
  "memory": {
    "episodic": {"enabled": true},
    "associative": {"enabled": true}
  },
  "prediction": {
    "enabled": true,
    "confidence_threshold": 0.7
  }
}
```

#### YAML Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("advanced_config.yaml")
```

```yaml
brain:
  neuron_count: 2000
  synapse_density: 0.15
  
plasticity:
  stdp:
    enable: true
    learning_rate: 0.01
  hebbian:
    enable: true
  structural:
    enable: true
    
neuromod:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    threshold: 0.2
  novelty:
    enable: true
    
memory:
  episodic:
    enabled: true
  associative:
    enabled: true
    
prediction:
  enabled: true
  confidence_threshold: 0.7
```

### 3. Memory System Integration

#### Working Memory

```python
import pynlm

config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory (Phase 6 feature)
working_memory = brain.getWorkingMemory()

# Store and retrieve information
working_memory.store("current_state", [0.1, 0.5, 0.9])
state = working_memory.retrieve("current_state")

# Update working memory with decay
working_memory.update(0.01)  # Decay factor
```

#### Episodic Memory

```python
# Access episodic memory for experience storage
episodic_memory = brain.getEpisodicMemory()

# Record an episode
episode_data = {
    "step": 42,
    "action": "MoveForward",
    "reward": 0.8,
    "sensory_input": [0.2, 0.4, 0.6],
    "context": "room1"
}

episodic_memory.recordEpisode(episode_data)

# Replay episodes
recent_episodes = episodic_memory.getRecentEpisodes(10)
```

#### Associative Memory

```python
# Access associative memory for pattern associations
associative_memory = brain.getAssociativeMemory()

# Form associations
associative_memory.associate("food", "reward", 0.9)
associative_memory.associate("hazard", "pain", 0.7)

# Retrieve associations
strength = associative_memory.getAssociation("food", "reward")
```

### 4. Prediction System

#### Basic Prediction

```python
# Access prediction system
prediction_system = brain.getPredictionSystem()

# Create sensory input for prediction
sensory = pynlm.createSensoryInput()
sensory.setData([0.1, 0.2, 0.3, 0.4])

# Make predictions
predicted_state = prediction_system.predictNextState(sensory)

# Update predictions with actual observations
prediction_system.updatePredictions(predicted_state, sensory)

# Get prediction error
error = prediction_system.getPredictionError()
print(f"Prediction error: {error}")
```

#### Prediction with Confidence

```python
# Use prediction with confidence threshold
confidence = prediction_system.getConfidence()
if confidence > 0.7:
    # High confidence prediction - trust it
    action = predicted_state.getAction()
else:
    # Low confidence - be more exploratory
    action = pynlm.MotorCommand.Wait
```

### 5. Neuromodulation Systems

#### Dopamine System

```python
# Access dopamine neuromodulation
dopamine = brain.getDopamine()

# Apply reward-based learning
reward = 1.0
predicted_reward = 0.3

dopamine.applyRewardModulation(reward, predicted_reward)

# Get current neuromodulation level
level = dopamine.getLevel()
```

#### Curiosity System

```python
# Access curiosity system
curiosity = brain.getCuriosity()

# Update curiosity with novelty and prediction error
curiosity.update(novelty_level=0.8, prediction_error=0.2, dt=0.1)

# Get exploration drive
exploration = curiosity.getExplorationDrive()
print(f"Exploration drive: {exploration}")
```

#### Novelty Detection

```python
# Access novelty system
novelty = brain.getNovelty()

# Compute novelty from sensory input
current_percept = world.getSensoryPercept()
novelty_level = novelty.computeNovelty(current_percept)

# Get current novelty level
current = novelty.getLevel()
```

### 6. Development System

#### Developmental Stages

```python
# Access development system
development = brain.getDevelopmentSystem()

# Get current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Current stage: {stage}")

# Update development
development.update(0.1)  # 0.1 seconds

# Set developmental stage (for testing)
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
```

#### Age-based Plasticity

```python
# Development affects plasticity rates
plasticity_modifier = development.getPlasticityModifier()
print(f"Plasticity modifier: {plasticity_modifier}")

# Early development (high plasticity)
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Initial:
    print("High plasticity - rapid learning")
# 
elif brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Adult:
    print("Low plasticity - stable behavior")
```

### 7. Cognition Systems

#### Neural Planner

```python
# Access neural planner
planner = brain.getPlanner()

# Plan sequences of actions
action_sequence = planner.plan(
    goal="find_food",
    context=current_state,
    max_steps=10
)

# Evaluate action sequences
score = planner.evaluate(action_sequence)
```

#### Concept Formation

```python
# Access concept formation system
concept_formation = brain.getConceptFormation()

# Learn concepts from patterns
concept_formation.learnPattern([0.1, 0.2, 0.3, 0.4])
concept_formation.learnPattern([0.5, 0.6, 0.7, 0.8])

# Get concepts
concepts = concept_formation.getCurrentConcepts()
```

#### Attentional Selection

```python
# Access attentional selection
attention = brain.getAttention()

# Focus on relevant features
relevant_features = attention.selectFeatures(
    all_features=all_sensory_data,
    context=current_context
)

# Get attention strength
attention_strength = attention.getStrength()
```

### 8. Plasticity Systems

#### STDP (Spike-Timing-Dependent Plasticity)

```python
# Access STDP system
stdp = brain.getSTDP()

# Configure STDP parameters
stdp.setLTPWeight(0.02)   # Long-term potentiation
stdp.setLTDWeight(0.015)  # Long-term depression
stdp.setTauPlus(20.0)     # LTP time constant
stdp.setTauMinus(20.0)    # LTD time constant

# Enable/disable STDP
stdp.enable(True)
```

#### Hebbian Learning

```python
# Access Hebbian system
hebbian = brain.getHebbian()

# Configure Hebbian parameters
hebbian.setLearningRate(0.01)
hebbian.setThreshold(0.5)  # Co-activity threshold

# Enable/disable Hebbian
hebbian.enable(True)
```

#### Structural Plasticity

```python
# Access structural plasticity
structural = brain.getStructuralPlasticity()

# Configure structural plasticity
structural.setSynaptogenesisRate(0.0001)
structural.setPruningRate(0.00001)

# Enable/disable structural plasticity
structural.enable(True)
```

### 9. Spike System

#### Event-driven Computation

```python
# Access spike system
spike_system = brain.getSpikeSystem()

# Get spike statistics
spike_count = spike_system.getTotalSpikeCount()
pending_events = spike_system.getPendingSpikeEventCount()

# Monitor neural activity
active_neurons = spike_system.getActiveNeuronCount()
```

## Advanced Agent Integration

### 1. Advanced Agent Configuration

```python
import pynlm

config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
world.setAgentStart(15, 15)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
```

### 2. Advanced Sensory Processing

```python
# Create advanced sensory percepts
percept = pynlm.SensoryPercept()

# Set different sensory modalities
vision_data = [0.1, 0.2, 0.3, 0.4] * 64  # 8x8 vision
percept.setVision(vision_data)

touch_data = [0.0, 0.5, 0.0, 0.8]  # Touch sensors
percept.setTouch(touch_data)

internal_data = [0.2, -0.1, 0.3]  # Internal state
percept.setInternal(internal_data)

proprioception_data = [0.0, 0.1, -0.1, 0.0, 0.2, -0.2]  # Body position
percept.setProprioception(proprioception_data)

# Process with agent
agent.processSensoryInput(percept)
```

### 3. Advanced Motor Decoding

```python
# Decode motor commands with advanced features
action = agent.decodeMotorCommand()

# Get detailed motor statistics
motor_stats = agent.getMotorStatistics()
print(f"Motor command: {action}")
print(f"Forward activity: {motor_stats['forward']:.3f}")
print(f"Turn left activity: {motor_stats['left']:.3f}")
print(f"Exploration level: {motor_stats['curiosity']:.3f}")
```

## Command Line Interface

For easier command-line control of NLM, use the `nlm-cli.py` script:

```bash
# Run a simulation
python nlm-cli.py run --steps 1000 --world-width 20 --world-height 20

# Get brain statistics
python nlm-cli.py stats --format json

# Run a specific test scenario
python nlm-cli.py test --scenario integration --steps 500

# Configuration management
python nlm-cli.py config get brain.neuron_count
python nlm-cli.py config set brain.neuron_count 5000

# Save/load brain state
python nlm-cli.py brain save my_brain_state.bin
python nlm-cli.py brain load my_brain_state.bin
```

## Performance Optimization

### 1. Performance Monitoring

```python
import time
import pynlm

config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Time simulation steps
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
end_time = time.time()
print(f"Average time per step: {(end_time - start_time) / 1000:.4f} seconds")

# Get performance statistics
stats = brain.getPerformanceStats()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Memory usage: {stats['memory_usage_mb']:.2f} MB")
```

### 2. Checkpointing and Recovery

```python
import pynlm

config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save checkpoint every 100 steps
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0 and step > 0:
        checkpoint_file = f"checkpoint_step_{step}.bin"
        brain.save(checkpoint_file)
        print(f"Checkpoint saved: {checkpoint_file}")

# Recover from checkpoint if needed
if os.path.exists("checkpoint_step_500.bin"):
    brain.load("checkpoint_step_500.bin")
    print("Recovered from checkpoint")
```

## Error Handling and Recovery

### 1. Robust Simulation Loop

```python
import pynlm

def run_robust_simulation(config_file="configs/default.cfg", steps=1000):
    """Run simulation with error handling and recovery"""
    config = pynlm.createDefaultConfig()
    
    try:
        config.loadFromFile(config_file)
    except Exception as e:
        print(f"Warning: Could not load config file: {e}")
        print("Using default configuration")
    
    brain = pynlm.createBrain(config)
    
    try:
        brain.initialize()
    except Exception as e:
        print(f"Error initializing brain: {e}")
        return False
    
    for step in range(steps):
        try:
            brain.step(step)
        except Exception as e:
            print(f"Error at step {step}: {e}")
            # Attempt recovery
            try:
                brain.reset()
                brain.initialize()
                print("Recovered from error")
            except Exception as recovery_error:
                print(f"Recovery failed: {recovery_error}")
                return False
    
    return True

# Run robust simulation
if run_robust_simulation("configs/default.cfg", 1000):
    print("Simulation completed successfully")
else:
    print("Simulation failed")
```

### 2. Validation and Error Recovery

```python
def validate_and_recover(config):
    """Validate configuration and recover from errors"""
    # Check required parameters
    required_params = ["brain.neuron_count", "simulation_timestep"]
    
    for param in required_params:
        if not config.has(param):
            print(f"Warning: Missing required parameter: {param}")
            # Set sensible defaults
            if param == "brain.neuron_count":
                config.set(param, 1000)
            elif param == "simulation_timestep":
                config.set(param, 0.001)
    
    # Validate parameter ranges
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr<int>("brain.neuron_count", 1000)
        if neuron_count < 10:
            print(f"Warning: Neuron count too low ({neuron_count}), setting to 100")
            config.set("brain.neuron_count", 100)
        elif neuron_count > 100000:
            print(f"Warning: Neuron count too high ({neuron_count}), setting to 10000")
            config.set("brain.neuron_count", 10000)
    
    return config

# Use validation
config = pynlm.createDefaultConfig()
config = validate_and_recover(config)
```

## Debugging and Troubleshooting

### 1. Brain Status and Diagnostics

```python
import pynlm

config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get detailed status information
status = brain.getDebugStatus()
print("=== Brain Debug Status ===")
print(f"Neuron count: {status['neuron_count']}")
print(f"Synapse count: {status['synapse_count']}")
print(f"Active neurons: {status['active_neurons']}")
print(f"Total spikes: {status['total_spikes']}")
print(f"Region count: {status['region_count']}")
print(f"Current stage: {status['developmental_stage']}")
print(f"Memory usage: {status['memory_usage_mb']:.2f} MB")
```

### 2. Performance Profiling

```python
import time
import pynlm

config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Profile simulation performance
steps = 500
times = []

for step in range(steps):
    start = time.perf_counter()
    brain.step(step)
    end = time.perf_counter()
    times.append(end - start)

avg_time = sum(times) / len(times)
min_time = min(times)
max_time = max(times)

print(f"Performance Profile ({steps} steps):")
print(f"  Average time per step: {avg_time:.4f}s")
print(f"  Minimum time per step: {min_time:.4f}s")
print(f"  Maximum time per step: {max_time:.4f}s")
print(f"  Throughput: {steps / sum(times):.1f} steps/second")
```

## Migration Guide

### From Phase 1 to Phase 6

If you're migrating from Phase 1 to Phase 6 features, follow this migration guide:

#### 1. Configuration Changes

**Phase 1 Config:**
```ini
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
```

**Phase 6 Config:**
```python
config = pynlm.createAdvancedConfig()
config.set("brain.neuron_count", 2000)  # Increased
config.set("plasticity.stdp.enable", true)  # Enable STDP
config.set("neuromod.curiosity.enable", true)  # Enable curiosity
```

#### 2. API Changes

**Phase 1:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
```

**Phase 6:**
```python
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)

# Access new systems
working_memory = brain.getWorkingMemory()
prediction_system = brain.getPredictionSystem()
curiosity = brain.getCuriosity()
```

#### 3. Feature Additions

**New Features in Phase 6:**
- Memory systems (working, episodic, associative)
- Prediction system with confidence tracking
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Development system with age-based plasticity
- Cognition systems (planning, concept formation, attention)
- Advanced plasticity rules (STDP, Hebbian, structural)
- Checkpoint system for recovery

## Best Practices

### 1. Configuration Management

```python
def create_production_config():
    """Create configuration suitable for production use"""
    config = pynlm.createAdvancedConfig()
    
    # Performance tuning
    config.set("brain.neuron_count", 5000)
    config.set("plasticity.stdp.learning_rate", 0.005)
    
    # Learning balance
    config.set("neuromod.curiosity.threshold", 0.1)
    config.set("neuromod.dopamine.scale", 0.8)
    
    # Memory settings
    config.set("memory.episodic.capacity", 1000)
    config.set("memory.associative.threshold", 0.3)
    
    return config
```

### 2. Simulation Lifecycle Management

```python
class NLMSimulation:
    def __init__(self, config_file=None):
        self.config = pynlm.createDefaultConfig()
        if config_file and os.path.exists(config_file):
            self.config.loadFromFile(config_file)
        
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self.checkpoints = []
    
    def setup(self, world_config=None):
        """Setup simulation environment"""
        self.brain.initialize()
        
        if world_config:
            self.world.configure(**world_config)
        
        # Configure world defaults
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent.initialize(self.world)
        
        # Enable all learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
    
    def run_episode(self, steps):
        """Run a single simulation episode"""
        for step in range(steps):
            try:
                # Update world
                self.world.update(0.1)
                
                # Process sensory input
                percept = self.world.getSensoryPercept()
                self.agent.processSensoryInput(percept)
                
                # Run brain step
                self.brain.step(step)
                
                # Decode action
                action = self.agent.decodeMotorCommand()
                
                # Apply action
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply learning
                reward = self.get_reward()  # Your reward function
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Save checkpoint periodically
                if step % 100 == 0:
                    self.save_checkpoint(step)
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                self.recover(step)
    
    def save_checkpoint(self, step):
        """Save simulation checkpoint"""
        checkpoint_file = f"checkpoint_step_{step}.bin"
        self.brain.save(checkpoint_file)
        self.checkpoints.append((step, checkpoint_file))
    
    def recover(self, step):
        """Recover from error"""
        # Try to recover from last checkpoint
        if self.checkpoints:
            last_step, last_checkpoint = self.checkpoints[-1]
            print(f"Recovering from checkpoint at step {last_step}")
            self.brain.load(last_checkpoint)
        else:
            # Reset and reinitialize
            print("No checkpoints available, resetting")
            self.brain.reset()
            self.brain.initialize()
```

### 3. Advanced Error Handling Patterns

```python
def robust_simulation_loop(simulation_func, max_retries=3):
    """Run simulation with retry logic"""
    for attempt in range(max_retries):
        try:
            return simulation_func()
        except Exception as e:
            print(f"Attempt {attempt + 1} failed: {e}")
            if attempt == max_retries - 1:
                raise
            print("Retrying...")
            time.sleep(1)  # Brief pause before retry
```

## Troubleshooting Common Issues

### 1. Segmentation Faults

**Symptoms:**
- Crashes during simulation
- Memory access violations
- Infinite loops

**Solutions:**
1. Check configuration parameters
2. Use smaller neuron counts for testing
3. Enable checkpointing for recovery
4. Run with debug mode

**Debug command:**
```bash
python nlm-cli.py run --config configs/default.cfg --steps 100
```

### 2. Slow Performance

**Symptoms:**
- High latency per step
- Memory usage grows indefinitely
- CPU usage spikes

**Solutions:**
1. Reduce neuron count
2. Disable unnecessary plasticity rules
3. Use checkpointing to save memory
4. Optimize configuration

**Performance check:**
```bash
python nlm-cli.py stats --format text
```

### 3. Learning Issues

**Symptoms:**
- Weights don't change
- No learning detected
- Behavior doesn't improve

**Solutions:**
1. Check reward function
2. Enable appropriate plasticity rules
3. Verify neuromodulation settings
4. Check novelty detection

**Test learning:**
```bash
python nlm-cli.py test --scenario plasticity --steps 500
```

## Advanced Examples

### 1. Complete Agent with All Systems

```python
import pynlm
import time

def advanced_agent_simulation(steps=1000):
    """Run simulation with all Phase 6 features enabled"""
    print("Starting advanced agent simulation...")
    
    # Create advanced configuration
    config = pynlm.createAdvancedConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15, 15)
    
    # Create agent with all subsystems
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    print("Agent initialized with all Phase 6 features")
    
    # Run simulation
    start_time = time.time()
    
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = calculate_reward(world, action)  # Your reward function
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}, "
                  f"DevStage: {brain.getDevelopmentalStage()}, "
                  f"Action: {action}")
    
    end_time = time.time()
    
    # Print final statistics
    print("\n=== Final Statistics ===")
    print(f"Simulation time: {end_time - start_time:.2f} seconds")
    print(f"Steps per second: {steps / (end_time - start_time):.1f}")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Get system statistics
    print("\n=== System Statistics ===")
    print(f"Working memory usage: {agent.getWorkingMemoryUsage():.2f} MB")
    print(f"Prediction confidence: {agent.getPredictionConfidence():.3f}")
    print(f"Exploration drive: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

# Run the simulation
if __name__ == "__main__":
    brain, agent, world = advanced_agent_simulation(1000)
```

### 2. Configuration Migration Tool

```python
import pynlm
import json
import os

def migrate_config(old_config_path, new_config_path):
    """Migrate configuration from old format to new Phase 6 format"""
    print(f"Migrating configuration from {old_config_path} to Phase 6 format...")
    
    # Load old configuration
    config = pynlm.createDefaultConfig()
    if os.path.exists(old_config_path):
        config.loadFromFile(old_config_path)
    
    # Create new advanced configuration
    new_config = pynlm.createAdvancedConfig()
    
    # Copy existing settings
    for key in config.getKeys():
        new_config.set(key, config.get(key))
    
    # Set Phase 6 defaults for missing keys
    defaults = {
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": True,
        "neuromod.dopamine.scale": 1.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "memory.episodic.enabled": True,
        "memory.associative.enabled": True,
        "prediction.enabled": True,
        "cognition.planner.enabled": True,
        "cognition.attention.enabled": True
    }
    
    for key, value in defaults.items():
        if not new_config.has(key):
            new_config.set(key, value)
    
    # Save new configuration
    new_config.saveToFile(new_config_path)
    
    print(f"Configuration migrated and saved to {new_config_path}")
    print("New configuration includes all Phase 6 features")

# Use the migration tool
if __name__ == "__main__":
    migrate_config("configs/default.cfg", "configs/default_phase6.cfg")
```

### 3. Performance Benchmark Tool

```python
import pynlm
import time
import json

def benchmark_configuration(config, steps=500):
    """Benchmark performance of a given configuration"""
    print(f"Benchmarking configuration with {config.getOr<int>('brain.neuron_count', 1000)} neurons...")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm-up
    for i in range(100):
        brain.step(i)
    
    # Benchmark
    start_time = time.perf_counter()
    
    for i in range(steps):
        brain.step(i)
    
    end_time = time.perf_counter()
    
    total_time = end_time - start_time
    avg_time = total_time / steps
    throughput = steps / total_time
    
    # Get statistics
    stats = {
        "config": {
            "neuron_count": config.getOr<int>("brain.neuron_count", 1000),
            "synapse_density": config.getOr<double>("brain.synapse_density", 0.1)
        },
        "performance": {
            "total_time_seconds": total_time,
            "average_time_per_step_ms": avg_time * 1000,
            "throughput_steps_per_second": throughput,
            "steps_benchmarked": steps
        },
        "brain_stats": {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio()
        }
    }
    
    # Save benchmark results
    with open("benchmark_results.json", "w") as f:
        json.dump(stats, f, indent=2)
    
    print(f"Benchmark completed in {total_time:.2f} seconds")
    print(f"Average time per step: {avg_time*1000:.2f} ms")
    print(f"Throughput: {throughput:.1f} steps/second")
    
    return stats

# Example usage
if __name__ == "__main__":
    config = pynlm.createAdvancedConfig()
    benchmark_configuration(config, 500)
```

## Summary

This advanced usage guide provides comprehensive information on working with NLM Phase 6 features, including:

- **Advanced Configuration**: JSON/YAML support and Phase 6 defaults
- **Memory Systems**: Working, episodic, and associative memory
- **Prediction System**: Predictive coding with confidence tracking
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Development System**: Age-based plasticity and developmental stages
- **Cognition Systems**: Planning, concept formation, and attention
- **Plasticity Systems**: STDP, Hebbian, and structural plasticity
- **Command Line Interface**: Easy control of simulations
- **Performance Optimization**: Monitoring and benchmarking tools
- **Error Handling**: Recovery mechanisms and troubleshooting

The NLM framework provides a complete platform for creating artificial neural systems with advanced learning capabilities. Whether you're a beginner or an expert, these tools and techniques will help you build sophisticated simulations for research and development.
