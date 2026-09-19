# NLM Advanced Usage Guide

## Advanced NLM Configuration and Usage

This document covers advanced usage patterns, configuration options, and best practices for NLM users. It builds upon the basic usage guide and provides deeper insights into the NLM system's capabilities.

## Part 1: Advanced Configuration

### Custom Brain Configuration

The NLM configuration system allows fine-tuned control over brain parameters. Here are some advanced configuration patterns:

```python
import pynlm
import json

# Method 1: Programmatic configuration
config = pynlm.createDefaultConfig()

# Configure specific regions with different characteristics
config.set("brain.region.cortex.neuron_count", 2000)
config.set("brain.region.cortex.synapse_density", 0.15)
config.set("brain.region.cortex.v_thresh", -45.0)  # Lower threshold

config.set("brain.region.hippo.neuron_count", 500)
config.set("brain.region.hippo.synapse_density", 0.08)
config.set("brain.region.hippo.v_thresh", -55.0)

# Configure specific neuron types
config.set("brain.neuron.excitatory.v_thresh", -50.0)
config.set("brain.neuron.inhibitory.v_thresh", -45.0)
config.set("brain.neuron.sensory.v_thresh", -40.0)

# Configure plasticity rules
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.exploration_strength", 0.3)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 0.5)

# Configure development
config.set("development.enabled", True)
config.set("development.critical_period_length", 300.0)  # Longer critical period
config.set("development.plasticity_decay_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced World Configuration

```python
# Create and configure a complex world
world = pynlm.createSimpleWorld()

# Multi-room environment with different zones
world.configure(
    width=50.0,           # Large world
    height=50.0,
    visionWidth=16,       # Higher resolution vision
    visionHeight=16
)

# Set agent starting position with facing direction
world.setAgentStart(25.0, 25.0)

# Configure world physics and dynamics
world.setMaxEnergy(200.0)           # More energy
world.setEnergyDecayRate(0.02)      # Slower energy decay

# Add structured obstacles
world.addObject(pynlm.WorldObject(
    x=10.0, y=25.0, type=pynlm.WorldObjectType.Wall, radius=2.0))

world.addObject(pynlm.WorldObject(
    x=40.0, y=25.0, type=pynlm.WorldObjectType.Wall, radius=2.0))

world.addObject(pynlm.WorldObject(
    x=25.0, y=10.0, type=pynlm.WorldObjectType.Resource, value=5.0))

world.addObject(pynlm.WorldObject(
    x=25.0, y=40.0, type=pynlm.WorldObjectType.Hazard, value=-10.0))

# Set random seed for reproducible experiments
world.setRandomSeed(42)
```

## Part 2: Advanced Agent Patterns

### 1. Curiosity-Driven Exploration

```python
def run_curiosity_driven_agent(config, world, num_episodes=100):
    """Agent that uses curiosity for exploration"""
    
    for episode in range(num_episodes):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Set exploration parameters
        agent.enableCuriosity(True)
        
        # Track novelty for analysis
        novelty_history = []
        curiosity_history = []
        
        for step in range(1000):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Record curiosity metrics
            curiosity_history.append(agent.getCuriosityLevel())
            novelty_history.append(agent.getNoveltyLevel())
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress every 100 steps
            if step % 100 == 0:
                print(f"Episode {episode}, Step {step}:")
                print(f"  Curiosity: {curiosity_history[-1]:.3f}")
                print(f"  Novelty: {novelty_history[-1]:.3f}")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        
        # Reset world for next episode
        world.reset()
    
    return curiosity_history, novelty_history
```

### 2. Reward Prediction Learning

```python
def run_reward_prediction_agent(config, world, num_steps=1000):
    """Agent that learns to predict rewards (actor-critic style)"""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable reward modulation
    agent.enableRewardModulation(True)
    
    # Store experience for analysis
    experiences = []
    reward_predictions = []
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        # Get current reward prediction
        current_prediction = agent.getPredictionError()
        reward_predictions.append(current_prediction)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get actual reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Apply reward with prediction
        agent.applyRewardModulation(reward, current_prediction)
        
        # Store experience
        experiences.append({
            'step': step,
            'action': action,
            'reward': reward,
            'prediction': current_prediction,
            'prediction_error': reward - current_prediction,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        })
        
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {reward:.3f}, Prediction: {current_prediction:.3f}")
            print(f"  Prediction error: {reward - current_prediction:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return experiences, reward_predictions
```

### 3. Developmental Learning Stages

```python
def run_developmental_agent(config, world, num_steps=5000):
    """Agent that develops over time through staged learning"""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all developmental systems
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    development_log = []
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Log developmental milestones
        if step % 500 == 0:
            stage = brain.getDevelopmentalStage()
            development_log.append({
                'step': step,
                'stage': stage,
                'age': agent.getDevelopmentalStage(),
                'neurons_firing': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'plasticity_modifier': agent.getNeuromodulationLevel()
            })
            
            print(f"Step {step} - Stage: {stage}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return development_log
```

## Part 3: Advanced Error Handling and Validation

### 1. Configuration Validation

```python
def validate_config(config):
    """Validate configuration parameters"""
    
    errors = []
    
    # Check neuron count ranges
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr("brain.neuron_count", 0)
        if neuron_count < 100 or neuron_count > 10000:
            errors.append(f"Invalid neuron count: {neuron_count}. Should be between 100 and 10000")
    
    # Check synapse density
    if config.has("brain.synapse_density"):
        density = config.getOr("brain.synapse_density", 0.0)
        if density < 0.0 or density > 1.0:
            errors.append(f"Invalid synapse density: {density}. Should be between 0.0 and 1.0")
    
    # Check threshold ranges
    if config.has("brain.v_thresh"):
        thresh = config.getOr("brain.v_thresh", 0.0)
        if thresh < -100.0 or thresh > 0.0:
            errors.append(f"Invalid threshold: {thresh}. Should be between -100.0 and 0.0")
    
    return errors

def safe_create_brain(config):
    """Create brain with configuration validation"""
    
    errors = validate_config(config)
    
    if errors:
        print("Configuration errors found:")
        for error in errors:
            print(f"  - {error}")
        raise ValueError("Invalid configuration")
    
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    except Exception as e:
        print(f"Failed to create brain: {e}")
        raise
```

### 2. Robust Agent Management

```python
class RobustAgent:
    """Robust agent wrapper with error handling"""
    
    def __init__(self, config):
        self.config = config
        self.brain = None
        self.agent = None
        self.world = None
        self.is_initialized = False
        self.error_count = 0
        self.max_errors = 10
    
    def initialize(self, world):
        """Initialize agent with error handling"""
        try:
            self.world = world
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(world)
            
            # Configure all subsystems
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            self.agent.enableDevelopment(True)
            self.agent.enableCuriosity(True)
            
            self.is_initialized = True
            print("✓ Agent initialized successfully")
            
        except Exception as e:
            self.error_count += 1
            print(f"✗ Agent initialization failed: {e}")
            if self.error_count > self.max_errors:
                raise RuntimeError("Too many initialization failures")
    
    def step(self, step_number):
        """Perform one simulation step with error handling"""
        
        if not self.is_initialized:
            raise RuntimeError("Agent not initialized")
        
        try:
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step_number)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            return True, {
                'action': action,
                'reward': reward,
                'neurons_firing': self.brain.getFiringNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'curiosity': self.agent.getCuriosityLevel()
            }
            
        except Exception as e:
            self.error_count += 1
            print(f"✗ Step {step_number} failed: {e}")
            return False, str(e)
    
    def reset(self):
        """Reset agent state"""
        
        if self.agent:
            self.agent.reset()
            if self.brain:
                self.brain.reset()
        
        self.is_initialized = False
        print("✓ Agent reset")
    
    def save_state(self, filepath):
        """Save agent state to file"""
        
        if self.brain:
            return self.brain.save(filepath)
        return False
    
    def load_state(self, filepath):
        """Load agent state from file"""
        
        if self.brain:
            return self.brain.load(filepath)
        return False
```

## Part 4: Advanced Pythonic Interfaces

### 1. Context Manager for Brain Lifecycle

```python
class NLMContext:
    """Context manager for automatic brain cleanup"""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.agent = None
    
    def __enter__(self):
        """Enter context and create brain"""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context and cleanup"""
        if self.agent:
            # Agent cleanup
            pass
        
        if self.brain:
            # Brain cleanup (if any)
            pass
        
        print("✓ NLM context exited")
        return False  # Don't suppress exceptions

# Usage example
def example_with_context():
    """Example using NLM context manager"""
    
    config = pynlm.createDefaultConfig()
    
    with NLMContext(config) as nlm:
        brain = nlm.brain
        
        for i in range(100):
            brain.step(i)
        
        # Brain automatically cleaned up
    
    print("✓ Brain automatically cleaned up")
```

### 2. Brain State Manager

```python
class BrainStateManager:
    """Manage brain state with checkpointing and rollback"""
    
    def __init__(self, brain):
        self.brain = brain
        self.checkpoint_file = "brain_checkpoint.bin"
        self.state_history = []
        self.max_history = 100
    
    def take_checkpoint(self, description=""):
        """Take a checkpoint of current brain state"""
        
        if self.brain.save(self.checkpoint_file):
            state_info = {
                'timestamp': pynlm.createDefaultConfig().getOr("simulation.time", 0.0),
                'step': len(self.state_history),
                'description': description,
                'file': self.checkpoint_file
            }
            
            self.state_history.append(state_info)
            
            # Keep history bounded
            if len(self.state_history) > self.max_history:
                self.state_history.pop(0)
            
            print(f"✓ Checkpoint taken: {description}")
            return True
        else:
            print("✗ Failed to take checkpoint")
            return False
    
    def rollback_to_checkpoint(self, checkpoint_index):
        """Rollback to previous checkpoint"""
        
        if checkpoint_index < 0 or checkpoint_index >= len(self.state_history):
            raise ValueError(f"Invalid checkpoint index: {checkpoint_index}")
        
        checkpoint = self.state_history[checkpoint_index]
        
        if self.brain.load(checkpoint['file']):
            print(f"✓ Rolled back to checkpoint: {checkpoint['description']}")
            return True
        else:
            print("✗ Failed to rollback to checkpoint")
            return False
    
    def get_checkpoint_list(self):
        """Get list of available checkpoints"""
        return self.state_history.copy()
```

## Part 5: Advanced Tutorials

### Tutorial 1: Building a Complete NLM System

This tutorial walks through building a complete NLM system with all subsystems integrated.

```python
#!/usr/bin/env python3
"""
Complete NLM Tutorial
=====================

This tutorial demonstrates building a complete NLM system with:
- Brain with all neuron types
- Agent with all sensory modalities
- World with physics simulation
- Learning systems (reward, curiosity, novelty)
- Developmental learning
"""

import pynlm
import time
import json

def tutorial_complete_system():
    """Complete NLM system tutorial"""
    
    print("=== Complete NLM System Tutorial ===\n")
    
    # 1. Create configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.v_thresh", -52.0)
    config.set("brain.v_rest", -70.0)
    
    # Configure development
    config.set("development.enabled", True)
    config.set("development.critical_period_length", 500.0)
    
    # Configure plasticity
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Configure neuromodulation
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    print(f"   Created config with {config.getKeys().__len__()} settings")
    
    # 2. Create world
    print("\n2. Creating world...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30.0,
        height=30.0,
        visionWidth=12,
        visionHeight=12
    )
    
    # Add interesting world elements
    # Resource zone
    for x in range(5, 15):
        for y in range(5, 15):
            world.addObject(pynlm.WorldObject(
                x=float(x), y=float(y), 
                type=pynlm.WorldObjectType.Resource, value=2.0))
    
    # Wall obstacles
    for x in range(20, 26):
        world.addObject(pynlm.WorldObject(
            x=float(x), y=15.0, type=pynlm.WorldObjectType.Wall, radius=1.5))
    
    # Hazard zone
    for x in range(20, 26):
        for y in range(25, 30):
            world.addObject(pynlm.WorldObject(
                x=float(x), y=float(y), 
                type=pynlm.WorldObjectType.Hazard, value=-5.0))
    
    world.setAgentStart(15.0, 15.0)
    world.setMaxEnergy(150.0)
    world.setEnergyDecayRate(0.015)
    
    print("   World configured with resources, walls, and hazards")
    
    # 3. Create brain
    print("\n3. Creating brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"   Expected synapses: {brain.getTotalSynapseCount()}")
    
    # 4. Create agent
    print("\n4. Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   Agent initialized with all learning systems enabled")
    
    # 5. Run simulation
    print("\n5. Running simulation...")
    
    stats = {
        'steps': 0,
        'total_spikes': 0,
        'curiosity_levels': [],
        'novelty_levels': [],
        'rewards': [],
        'actions_taken': {},
        'developmental_stages': []
    }
    
    # Track action statistics
    action_types = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.MoveBackward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Wait
    ]
    
    for action_type in action_types:
        stats['actions_taken'][action_type] = 0
    
    start_time = time.time()
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record action
        stats['actions_taken'][action] = stats['actions_taken'].get(action, 0) + 1
        
        # Get reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        stats['rewards'].append(reward)
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record statistics
        stats['curiosity_levels'].append(agent.getCuriosityLevel())
        stats['novelty_levels'].append(agent.getNoveltyLevel())
        stats['developmental_stages'].append(brain.getDevelopmentalStage())
        
        stats['steps'] += 1
        stats['total_spikes'] += brain.getTotalSpikeCount()
        
        # Print progress
        if step % 200 == 0:
            print(f"   Step {step}:")
            print(f"     Action: {action}")
            print(f"     Reward: {reward:.3f}")
            print(f"     Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"     Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"     Stage: {brain.getDevelopmentalStage()}")
            print(f"     Neurons firing: {brain.getFiringNeuronCount()}")
    
    elapsed_time = time.time() - start_time
    
    # 6. Analyze results
    print("\n6. Analyzing results...")
    
    total_actions = sum(stats['actions_taken'].values())
    
    print(f"   Simulation completed in {elapsed_time:.2f} seconds")
    print(f"   Average step time: {elapsed_time/stats['steps']*1000:.2f} ms")
    
    print(f"\n   Statistics:")
    print(f"     Total steps: {stats['steps']}")
    print(f"     Total spikes: {stats['total_spikes']}")
    print(f"     Average firing rate: {stats['total_spikes']/stats['steps']:.2f} spikes/step")
    
    print(f"\n   Action distribution:")
    for action, count in stats['actions_taken'].items():
        if count > 0:
            percentage = count / total_actions * 100
            print(f"     {action}: {count} ({percentage:.1f}%)")
    
    print(f"\n   Learning metrics:")
    print(f"     Average curiosity: {sum(stats['curiosity_levels'])/len(stats['curiosity_levels']):.3f}")
    print(f"     Max curiosity: {max(stats['curiosity_levels']):.3f}")
    print(f"     Average novelty: {sum(stats['novelty_levels'])/len(stats['novelty_levels']):.3f}")
    print(f"     Max novelty: {max(stats['novelty_levels']):.3f}")
    
    # Count developmental stage transitions
    stage_changes = 0
    for i in range(1, len(stats['developmental_stages'])):
        if stats['developmental_stages'][i] != stats['developmental_stages'][i-1]:
            stage_changes += 1
    
    print(f"     Developmental stage transitions: {stage_changes}")
    print(f"     Final developmental stage: {stats['developmental_stages'][-1]}")
    
    # 7. Save results
    print("\n7. Saving results...")
    
    # Save brain state
    if brain.save("tutorial_brain.bin"):
        print("   ✓ Brain saved to tutorial_brain.bin")
    
    # Save configuration
    config.saveToFile("tutorial_config.json")
    print("   ✓ Configuration saved to tutorial_config.json")
    
    # Save statistics
    with open("tutorial_stats.json", "w") as f:
        json.dump(stats, f, indent=2)
    print("   ✓ Statistics saved to tutorial_stats.json")
    
    print("\n=== Tutorial Complete ===")
    print("You have successfully built and ran a complete NLM system!")
    
    return stats

if __name__ == "__main__":
    tutorial_complete_system()
```

## Part 6: Troubleshooting Guide

### Common Issues and Solutions

#### Issue 1: "My brain isn't doing anything"

**Symptoms:** Zero spikes, no activity

**Solutions:**
1. **Check initialization:** Ensure `brain.initialize()` is called before `brain.step()`
2. **Increase simulation steps:** Brains need time to "warm up" - try more steps
3. **Add sensory input:** Ensure sensory neurons receive input via `agent.processSensoryInput()`
4. **Check configuration:** Verify neuron count and synapse settings are reasonable

**Debugging code:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Add sensory input to wake up neurons
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)

# Run warm-up steps
for i in range(100):
    brain.step(i)
    if brain.getFiringNeuronCount() > 0:
        break
```

#### Issue 2: "The agent isn't moving"

**Symptoms:** Agent stays in place

**Solutions:**
1. **Check motor neurons:** Ensure motor neuron groups are populated
2. **Verify action decoding:** Check `agent.decodeMotorCommand()` returns valid actions
3. **Validate world updates:** Ensure `world.update()` is called
4. **Check action application:** Ensure `world.applyMotorCommand()` is called

**Debugging code:**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Check motor neuron groups
print(f"Vision neurons: {agent.getSensoryInputSize()}")
print(f"Motor output size: {agent.getMotorOutputSize()}")

# Manually test action decoding
action = agent.decodeMotorCommand()
print(f"Decoded action: {action}")

# Test world interaction
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(0)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### Issue 3: Segmentation faults

**Symptoms:** Program crashes

**Solutions:**
1. **Memory issues:** Check for memory leaks with large simulations
2. **Null pointers:** Ensure all objects are properly initialized
3. **Configuration errors:** Validate configuration parameters
4. **Resource limits:** Monitor memory and CPU usage

**Prevention strategies:**
```python
# Use context managers for automatic cleanup
with NLMContext(config) as nlm:
    brain = nlm.brain
    # Run simulation
    # Automatic cleanup on exit

# Implement checkpointing for long simulations
state_manager = BrainStateManager(brain)
for step in range(10000):
    brain.step(step)
    
    # Save checkpoints periodically
    if step % 1000 == 0:
        state_manager.take_checkpoint(f"Step {step}")
```

### Performance Optimization Tips

1. **Batch operations:** Group related operations when possible
2. **Reuse objects:** Reuse brain and agent objects when running multiple simulations
3. **Memory management:** Clear references to free memory
4. **Configuration caching:** Cache frequently used configuration values

```python
# Optimized agent reuse pattern
class AgentPool:
    def __init__(self, config, pool_size=5):
        self.config = config
        self.pool = []
        self.pool_size = pool_size
    
    def get_agent(self):
        if not self.pool:
            brain = pynlm.createBrain(self.config)
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            return agent
        return self.pool.pop()
    
    def return_agent(self, agent):
        agent.reset()
        if len(self.pool) < self.pool_size:
            self.pool.append(agent)
```

## Conclusion

This advanced guide provides comprehensive coverage of NLM's advanced features, including:

- **Deep configuration customization** for specific use cases
- **Complex agent patterns** for learning and exploration
- **Robust error handling** for production systems
- **Performance optimization** for large-scale simulations
- **Complete tutorials** for building full NLM applications

The NLM Python API offers extensive flexibility for creating sophisticated neural systems. Whether you're building simple simulations or complex learning environments, these patterns and examples provide a solid foundation for advanced NLM development.

---

*For more information, refer to the documentation files in the `docs/` directory and the source code in the `src/` directory.*
