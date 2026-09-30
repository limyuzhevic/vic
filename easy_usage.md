# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is a **brain simulator** for computers that creates virtual brains capable of learning, memory, and decision-making through neural dynamics.

Think of it like this:
- **Neurons** = Brain cells that send electrical signals to each other
- **Synapses** = Connections between neurons that transmit signals
- **Learning** = When synaptic connections strengthen or weaken based on neural activity
- **Memory** = The brain storing patterns through synaptic changes
- **Development** = The brain's maturation and adaptation over time

NLM simulates this biological inspiration using computational models of neural computation.

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks and processes information
2. **World** - The environment the brain lives in and interacts with
3. **Agent** - The bridge connecting brain to world, handling sensory processing and motor output

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

### Example 3: Complete Learning Agent

```python
import pynlm

def run_learning_agent():
    """Run a complete learning agent with reward-based learning."""
    # 1. Setup the brain with optimal configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.08)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.initialize(world)
    
    # 3. Track learning progress
    learning_log = {
        'steps': [],
        'rewards': [],
        'curiosity': [],
        'firing_rates': []
    }
    
    # 4. Run simulation loop
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input and get reward
        agent.processSensoryInput(percept)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        
        # Brain computation
        brain.step(step)
        
        # Get action from agent
        action = agent.decodeMotorCommand()
        
        # Apply action and get feedback
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log data
        learning_log['steps'].append(step)
        learning_log['rewards'].append(reward)
        learning_log['curiosity'].append(agent.getCuriosityLevel())
        learning_log['firing_rates'].append(brain.getAverageFiringRate())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: Reward={reward:.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Firing Rate={brain.getAverageFiringRate():.3f}")
    
    return learning_log

# Run the learning agent
learning_results = run_learning_agent()

print(f"\nLearning Complete!")
print(f"Total steps: {len(learning_results['steps'])}")
print(f"Average reward per step: {sum(learning_results['rewards'])/len(learning_results['rewards']):.3f}")
print(f"Final curiosity level: {learning_results['curiosity'][-1]:.3f}")
print(f"Final firing rate: {learning_results['firing_rates'][-1]:.3f}")
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

### Pattern 1: Run a Complete Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    """Run a complete simulation with all core components."""
    for step in range(num_steps):
        # 1. Update world environment
        world.update(0.1)
        
        # 2. Get what the agent sees from world
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain what it perceives
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks and processes information
        brain.step(step)
        
        # 5. Get action from brain activity
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. (Optional) Apply reward modulation for learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development stage
        agent.updateDevelopment(0.1)

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Comprehensive Brain Stats

```python
def monitor_brain_health(brain, step):
    """Monitor brain health and performance metrics."""
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'exciation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'developmental_stage': brain.getDevelopmentalStage(),
        'active_synapse_count': brain.getActiveSynapseCount()
    }
    
    print(f"\nBrain Stats at Step {step}:")
    print(f"  Neural Network:")
    print(f"    - Total neurons: {stats['total_neurons']}")
    print(f"    - Total synapses: {stats['total_synapses']}")
    print(f"    - Connection density: {stats['total_synapses']/max(1, stats['total_neurons']**2):.4f}")
    
    print(f"  Activity:")
    print(f"    - Currently firing: {stats['firing_neurons']}")
    print(f"    - Average firing rate: {stats['average_firing_rate']:.3f}")
    print(f"    - Total spikes: {stats['total_spikes']}")
    
    print(f"  Network state:")
    print(f"    - E/I ratio: {stats['exciation_inhibition_ratio']:.3f}")
    print(f"    - Developmental stage: {stats['developmental_stage']}")
    
    return stats

# Usage in your simulation loop:
# for step in range(num_steps):
#     # ... simulation steps ...
#     monitor_brain_health(brain, step)
```

### Pattern 3: Enable Brain Features

```python
def setup_advanced_features(brain, agent, world):
    """Setup advanced brain features for enhanced learning."""
    # These are all optional features that can be enabled:
    
    # 1. Reward-based learning (dopamine system)
    agent.enableRewardModulation(True)
    print("✓ Reward modulation enabled - agent learns from rewards")
    
    # 2. Structural plasticity (synaptogenesis)
    agent.enableStructuralPlasticity(True)
    print("✓ Structural plasticity enabled - brain grows new connections")
    
    # 3. Developmental maturation
    agent.enableDevelopment(True)
    print("✓ Development enabled - brain matures over time")
    
    # 4. Curiosity-driven exploration
    agent.enableCuriosity(True)
    print("✓ Curiosity enabled - agent explores novel stimuli")
    
    # 5. Additional world features
    world.setRandomSeed(42)  # For reproducible experiments
    world.setMaxEnergy(100.0)  # Energy conservation
    
    return {
        'reward_modulation': True,
        'structural_plasticity': True,
        'development': True,
        'curiosity': True,
        'reproducibility': True
    }

# Usage example:
features = setup_advanced_features(brain, agent, world)
print(f"\nAdvanced features configured: {sum(features.values())}/{len(features)} enabled")
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- Returns: Brain object ready for initialization and simulation

### `brain.initialize()`
Starts up the brain with default configuration. Always call this before using the brain!
- Initializes neural populations
- Sets up synaptic connections
- Prepares for simulation steps
- Returns: None (modifies brain in-place)

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron for potential firing
- Sends signals between connected neurons
- Updates connections based on learning rules (STDP, Hebbian)
- Processes neuromodulation signals
- Returns: None (modifies brain state)

### `world.update(time)`
Updates the virtual world by `time` seconds.
- Advances time in the simulation
- Updates agent position and physics
- Handles environmental changes
- Returns: None (modifies world state)

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, internal signals) to the brain.
- Transduces sensory input to neural signals
- Injects sensory information into sensory neurons
- Prepares brain for processing
- Returns: None (modifies brain state)

### `agent.decodeMotorCommand()`
Reads the brain's motor activity to decide what action to take.
- Analyzes motor neuron activity
- Selects appropriate motor command
- Returns: Action object

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

print("Project 1: Silent Brain")
print("=" * 40)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons created: {brain.getTotalNeuronCount()}")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
```

### Project 2: Brain Watching a World

```python
import pynlm

print("\nProject 2: Brain Watching a World")
print("=" * 40)

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
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Neurons active: {brain.getFiringNeuronCount()}/{brain.getTotalNeuronCount()}")
```

### Project 3: Complete Learning Agent

```python
import pynlm
import matplotlib.pyplot as plt

print("\nProject 3: Complete Learning Agent")
print("=" * 40)

# Setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
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
agent.enableDevelopment(True)

# Run agent with comprehensive monitoring
rewards = []
curiosity_levels = []
firing_rates = []

for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record metrics
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    rewards.append(reward)
    curiosity_levels.append(agent.getCuriosityLevel())
    firing_rates.append(brain.getAverageFiringRate())

# Create visualization
fig, axes = plt.subplots(3, 1, figsize=(10, 8))

axes[0].plot(rewards)
axes[0].set_title('Reward Over Time')
axes[0].set_ylabel('Reward')

axes[1].plot(curiosity_levels)
axes[1].set_title('Curiosity Level Over Time')
axes[1].set_ylabel('Curiosity')

axes[2].plot(firing_rates)
axes[2].set_title('Firing Rate Over Time')
axes[2].set_ylabel('Firing Rate')
axes[2].set_xlabel('Step')

plt.tight_layout()
plt.savefig('learning_agent_results.png')

print("Agent simulation complete!")
print(f"Average reward: {sum(rewards)/len(rewards):.3f}")
print(f"Final curiosity: {curiosity_levels[-1]:.3f}")
print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.3f}")
```

---

## Quick Reference

### Quick Start Commands

| What you want | Code | Description |
|--------------|------|-------------|
| Create brain | `pynlm.createBrain(config)` | Creates neural brain object |
| Start brain | `brain.initialize()` | Initializes brain for simulation |
| Make brain think | `brain.step(step)` | Runs one simulation step |
| Create world | `pynlm.createSimpleWorld()` | Creates environment |
| Create agent | `pynlm.createAgentBrain(brain)` | Creates agent interface |
| Get brain stats | `brain.getFiringNeuronCount()` | Gets number of firing neurons |
| See world | `world.getSensoryPercept()` | Gets sensory input from world |
| Make action | `agent.decodeMotorCommand()` | Gets action from brain |

### Configuration Examples

#### Minimal Configuration

```python
import pynlm

# Quick start with minimal setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### Enhanced Configuration

```python
import pynlm

# Advanced setup with learning capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.08)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.initialize(world)
```

---

## Error Handling and Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
- **Solution:** Did you call `brain.initialize()`? Always initialize before using the brain!

**"The agent isn't moving"**
- **Solution:** Did you call `world.applyMotorCommand()`? Check that `world.update()` is being called before actions.

**"Everything is 0"**
- **Solution:** Brains need time to "warm up" - try more steps (usually 100+ steps)
- **Tip:** Some neurons need input to fire - make sure sensory input is connected properly

**"The simulation is crashing"**
- **Solution:** Check your configuration parameters:
  - `brain.neuron_count` should be positive
  - `brain.synapse_density` should be between 0 and 1
  - `world.width` and `world.height` should be positive

**"Memory issues"**
- **Solution:** Use checkpointing for long simulations:
  ```python
  # Save checkpoints periodically
  if step % 100 == 0:
      brain.save(f"checkpoint_{step}.bin")
  ```

### Debug Checklist

1. **Initialization:** Did you call `brain.initialize()`?
2. **Configuration:** Is your configuration valid?
3. **Simulation loop:** Are you calling `world.update()` before actions?
4. **Sensory input:** Is `world.getSensoryPercept()` being processed?
5. **Memory:** Are you monitoring memory usage?

---

## Performance Tips

### Tips for Better Performance

1. **Use appropriate brain size:**
   - For testing: 100-500 neurons
   - For research: 1000-5000 neurons
   - For production: 5000+ neurons

2. **Enable efficient learning:**
   - Enable reward modulation for goal-directed learning
   - Use curiosity for exploration behavior
   - Balance development for stable learning

3. **Monitor performance regularly:**
   ```python
   def monitor_performance(brain, agent, step):
       print(f"Step {step}:")
       print(f"  Neurons: {brain.getTotalNeuronCount()}")
       print(f"  Firing: {brain.getFiringNeuronCount()}")
       print(f"  Curios: {agent.getCuriosityLevel():.3f}")
       print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
   ```

4. **Use efficient data structures:**
   - For large networks: Consider sparse connectivity
   - For fast simulations: Use optimized step loops

5. **Parallel processing:**
   - Enable multi-threading for CPU-bound tasks
   - Use efficient data access patterns

---

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md`** for more detailed technical documentation
2. **Read `docs/ARCHITECTURE.md`** to understand the complete system architecture
3. **Read `docs/python_api.md`** for comprehensive API documentation
4. **Read `docs/python_examples.md`** for advanced usage examples
5. **Read `docs/advanced_examples.md`** for comprehensive advanced examples
6. **Read `docs/advanced_tutorials.md`** for detailed step-by-step tutorials
7. **Experiment with different configurations** to understand parameter effects
8. **Explore the documentation** (`docs/` directory) for scientific background and research guidance

## Advanced Usage for Experienced Users

### Working with Complex Configurations

```python
import pynlm
import numpy as np

# Advanced brain configuration with learning systems
def create_advanced_config():
    config = pynlm.createDefaultConfig()
    
    # High-performance brain settings
    config.set("brain.neuron_count", 10000)
    config.set("brain.synapse_density", 0.02)
    config.set("brain.v_thresh", -50.0)
    config.set("brain.tau_mem", 15.0)
    
    # Enable all learning systems
    config.set("learning.stdp.enable", True)
    config.set("learning.stdp.a_plus", 0.02)
    config.set("learning.stdp.a_minus", 0.024)
    
    config.set("learning.hebbian.enable", True)
    config.set("learning.hebbian.learning_rate", 0.001)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.baseline", 0.15)
    config.set("neuromod.dopamine.learning_rate", 0.1)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.sensitivity", 0.9)
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.25)
    
    # Memory systems
    config.set("memory.working_memory.enabled", True)
    config.set("memory.working_memory.capacity", 5000)
    
    config.set("memory.episodic.enabled", True)
    config.set("memory.episodic.capacity", 20000)
    
    config.set("memory.associative.enabled", True)
    config.set("memory.associative.similarity_threshold", 0.8)
    
    # Prediction and cognitive systems
    config.set("prediction.enabled", True)
    config.set("prediction.horizon", 20)
    
    config.set("cognition.attention.enabled", True)
    config.set("cognition.attention.competition_strength", 1.5)
    
    return config

# Create advanced brain
advanced_config = create_advanced_config()
brain = pynlm.createBrain(advanced_config)
brain.initialize()
```

### Performance Optimization Tips

```python
import pynlm
import time

class PerformanceMonitor:
    """Monitor and optimize NLM performance."""
    
    def __init__(self, brain, agent):
        self.brain = brain
        self.agent = agent
        self.metrics = {
            'step_times': [],
            'firing_rates': [],
            'memory_usage': [],
            'processing_efficiency': []
        }
        
    def monitor_performance(self, step):
        """Monitor performance at current step."""
        # Record brain metrics
        self.metrics['firing_rates'].append(self.brain.getAverageFiringRate())
        self.metrics['memory_usage'].append(self.brain.getMemoryUsage())
        
        # Calculate processing efficiency
        efficiency = self.calculate_efficiency(step)
        self.metrics['processing_efficiency'].append(efficiency)
        
        return self.get_performance_summary()
    
    def calculate_efficiency(self, step):
        """Calculate processing efficiency."""
        # Simple efficiency metric: firing rate / step time
        if step == 0:
            return 1.0
        
        recent_firing_rate = self.metrics['firing_rates'][-1]
        return recent_firing_rate / max(1.0, step / 100.0)
    
    def get_performance_summary(self):
        """Get performance summary."""
        if not self.metrics['step_times']:
            return {
                'avg_step_time': 0.0,
                'avg_firing_rate': 0.0,
                'avg_memory_usage': 0.0,
                'avg_efficiency': 0.0
            }
        
        return {
            'avg_step_time': np.mean(self.metrics['step_times']),
            'avg_firing_rate': np.mean(self.metrics['firing_rates']),
            'avg_memory_usage': np.mean(self.metrics['memory_usage']),
            'avg_efficiency': np.mean(self.metrics['processing_efficiency'])
        }
    
    def optimize_memory_usage(self):
        """Optimize memory usage."""
        # Implement memory management strategies
        if hasattr(self.brain, 'compress_memory'):
            self.brain.compress_memory()
        
        # Clear unused episodic memories
        if hasattr(self.agent, 'cleanup_episodic_memory'):
            self.agent.cleanup_episodic_memory()
    
    def get_recommendations(self):
        """Get performance optimization recommendations."""
        recommendations = []
        
        # Check firing rate
        avg_firing_rate = np.mean(self.metrics['firing_rates'])
        if avg_firing_rate < 1.0:
            recommendations.append(
                "Low firing rate detected. Consider increasing brain excitability or reducing inhibition."
            )
        elif avg_firing_rate > 20.0:
            recommendations.append(
                "High firing rate detected. Consider reducing brain excitability or increasing inhibition."
            )
        
        # Check memory usage
        avg_memory = np.mean(self.metrics['memory_usage'])
        if avg_memory > 0.9:
            recommendations.append(
                "High memory usage detected. Consider implementing memory compression or reducing memory capacity."
            )
        
        # Check efficiency
        avg_efficiency = np.mean(self.metrics['processing_efficiency'])
        if avg_efficiency < 0.5:
            recommendations.append(
                "Low processing efficiency detected. Consider optimizing brain configuration or reducing simulation complexity."
            )
        
        return recommendations

# Usage example
monitor = PerformanceMonitor(brain, agent)

# Run simulation with monitoring
for step in range(1000):
    # Run simulation step
    self.world.update(0.1)
    percept = self.world.getSensoryPercept()
    self.agent.processSensoryInput(percept)
    self.brain.step(step)
    action = self.agent.decodeMotorCommand()
    self.world.applyMotorCommand(action, self.world.getSimulationTime())
    
    # Monitor performance
    perf = monitor.monitor_performance(step)
    
    # Optimize if needed
    if step % 100 == 0:
        monitor.optimize_memory_usage()
        
        if perf['avg_efficiency'] < 0.7:
            print(f"Performance recommendation: {monitor.get_recommendations()}")
```

### Error Handling Patterns

```python
import pynlm
import traceback

class ErrorHandler:
    """Handle errors in NLM applications."""
    
    def __init__(self):
        self.error_history = []
        self.error_recovery_strategies = {
            'configuration_error': self.recover_from_configuration_error,
            'initialization_error': self.recover_from_initialization_error,
            'simulation_error': self.recover_from_simulation_error,
            'memory_error': self.recover_from_memory_error,
            'unknown_error': self.recover_from_unknown_error
        }
        
    def handle_error(self, error, context=None):
        """Handle an error with appropriate recovery strategy."""
        error_record = {
            'timestamp': time.time(),
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context or {},
            'stack_trace': traceback.format_exc()
        }
        
        self.error_history.append(error_record)
        
        # Determine error type and apply appropriate recovery
        error_type = self.categorize_error(error)
        
        try:
            recovery_result = self.error_recovery_strategies.get(
                error_type, self.error_recovery_strategies['unknown_error']
            )(error, context)
            
            error_record['recovery_applied'] = error_type
            error_record['recovery_successful'] = recovery_result
            
            return recovery_result
            
        except Exception as recovery_error:
            error_record['recovery_error'] = str(recovery_error)
            return False
    
    def categorize_error(self, error):
        """Categorize error for appropriate recovery."""
        error_msg = str(error).lower()
        
        if 'neuron_count' in error_msg or 'synapse' in error_msg:
            return 'configuration_error'
        elif 'initialize' in error_msg:
            return 'initialization_error'
        elif 'step' in error_msg or 'simulate' in error_msg:
            return 'simulation_error'
        elif 'memory' in error_msg or 'out of memory' in error_msg:
            return 'memory_error'
        else:
            return 'unknown_error'
    
    def recover_from_configuration_error(self, error, context):
        """Recover from configuration error."""
        print("Attempting to recover from configuration error...")
        
        # Reset to default configuration
        config = pynlm.createDefaultConfig()
        
        # Try with conservative settings
        config.set("brain.neuron_count", min(1000, config.get("brain.neuron_count", 1000)))
        config.set("brain.synapse_density", max(0.01, config.get("brain.synapse_density", 0.05)))
        
        # If we have a brain object, try to reconfigure it
        if 'brain' in context:
            try:
                context['brain'].configure(config)
                return True
            except:
                pass
        
        return False
    
    def recover_from_initialization_error(self, error, context):
        """Recover from initialization error."""
        print("Attempting to recover from initialization error...")
        
        # Check if brain was properly initialized
        if 'brain' in context and hasattr(context['brain'], 'is_initialized'):
            if not context['brain'].is_initialized():
                # Re-initialize brain
                try:
                    context['brain'].initialize()
                    return True
                except:
                    pass
        
        return False
    
    def recover_from_simulation_error(self, error, context):
        """Recover from simulation error."""
        print("Attempting to recover from simulation error...")
        
        # Check simulation state
        if 'step_count' in context and context['step_count'] > 0:
            # Reset simulation to known good state
            try:
                if 'world' in context:
                    context['world'].reset()
                if 'agent' in context:
                    context['agent'].reset()
                if 'brain' in context:
                    context['brain'].reset()
                
                return True
            except:
                pass
        
        return False
    
    def recover_from_memory_error(self, error, context):
        """Recover from memory error."""
        print("Attempting to recover from memory error...")
        
        # Try memory management strategies
        if 'brain' in context and hasattr(context['brain'], 'cleanup_memory'):
            try:
                context['brain'].cleanup_memory()
                return True
            except:
                pass
        
        return False
    
    def recover_from_unknown_error(self, error, context):
        """Recover from unknown error."""
        print(f"Attempting to recover from unknown error: {error}")
        
        # Try basic recovery steps
        recovery_steps = [
            self.try_basic_recovery(context),
            self.try_restart_simulation(context),
            self.try_configuration_reset(context)
        ]
        
        for step in recovery_steps:
            if step:
                return True
        
        return False
    
    def try_basic_recovery(self, context):
        """Try basic recovery steps."""
        # Reset all components to safe state
        if 'brain' in context and hasattr(context['brain'], 'reset'):
            context['brain'].reset()
        
        return True
    
    def try_restart_simulation(self, context):
        """Try restarting simulation."""
        # This would restart the simulation if possible
        if 'brain' in context and hasattr(context['brain'], 'initialize'):
            try:
                context['brain'].initialize()
                return True
            except:
                pass
        
        return False
    
    def try_configuration_reset(self, context):
        """Try configuration reset."""
        # Reset to minimal working configuration
        if 'brain' in context and hasattr(context['brain'], 'getConfig'):
            config = context['brain'].getConfig()
            # Reset to safe defaults
            return True
        
        return False
    
    def get_error_statistics(self):
        """Get error statistics."""
        if not self.error_history:
            return {
                'total_errors': 0,
                'error_types': {},
                'recovery_success_rate': 0.0
            }
        
        error_types = {}
        successful_recoveries = 0
        
        for error in self.error_history:
            error_type = error['error_type']
            error_types[error_type] = error_types.get(error_type, 0) + 1
            
            if error.get('recovery_successful', False):
                successful_recoveries += 1
        
        return {
            'total_errors': len(self.error_history),
            'error_types': error_types,
            'recovery_success_rate': successful_recoveries / len(self.error_history) if self.error_history else 0.0
        }

# Usage example
error_handler = ErrorHandler()

try:
    # Run complex simulation
    config = advanced_config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
except Exception as e:
    # Handle error
    recovery_result = error_handler.handle_error(
        e, {'brain': brain, 'agent': agent, 'world': world, 'step_count': step}
    )
    
    if recovery_result:
        print("Error recovery successful. Continuing simulation...")
    else:
        print("Error recovery failed. Please check logs for details.")
        
    # Get error statistics
    stats = error_handler.get_error_statistics()
    print(f"Error statistics: {stats}")
```

### Debugging Techniques

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

class Debugger:
    """Debug NLM applications and models."""
    
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.debug_log = []
        
    def debug_step(self, step, percept, action, reward):
        """Debug current simulation step."""
        debug_info = {
            'step': step,
            'timestamp': time.time(),
            'percept_analysis': self.analyze_percept(percept),
            'action_analysis': self.analyze_action(action),
            'reward_analysis': self.analyze_reward(reward),
            'brain_state': self.capture_brain_state(),
            'agent_state': self.capture_agent_state(),
            'world_state': self.capture_world_state()
        }
        
        self.debug_log.append(debug_info)
        
        # Check for potential issues
        issues = self.detect_issues(debug_info)
        
        if issues:
            self.print_debug_report(debug_info, issues)
        
        return issues
    
    def analyze_percept(self, percept):
        """Analyze sensory percept for debugging."""
        if percept is None:
            return {'valid': False, 'error': 'No percept available'}
        
        analysis = {
            'valid': True,
            'vision_available': percept.getVision() is not None,
            'touch_available': percept.getTouch() is not None,
            'internal_available': percept.getInternal() is not None,
            'audio_available': percept.getAudio() is not None,
            'proprioception_available': percept.getProprioception() is not None
        }
        
        # Check for NaN or infinite values
        if hasattr(percept, 'getAllSignals'):
            all_signals = percept.getAllSignals()
            for key, value in all_signals.items():
                if isinstance(value, np.ndarray):
                    if np.any(np.isnan(value)):
                        analysis[f'{key}_has_nan'] = True
                    if np.any(np.isinf(value)):
                        analysis[f'{key}_has_inf'] = True
        
        return analysis
    
    def analyze_action(self, action):
        """Analyze action for debugging."""
        if action is None:
            return {'valid': False, 'error': 'No action available'}
        
        analysis = {
            'valid': True,
            'action_type': action.getType() if hasattr(action, 'getType') else str(action),
            'has_parameters': hasattr(action, 'getParameters'),n            'parameter_count': len(action.getParameters()) if hasattr(action, 'getParameters') else 0
        }
        
        # Check for invalid action parameters
        if hasattr(action, 'getParameters'):
            params = action.getParameters()
            for i, param in enumerate(params):
                if isinstance(param, (int, float)):
                    if np.isnan(param) or np.isinf(param):
                        analysis[f'param_{i}_invalid'] = True
        
        return analysis
    
    def analyze_reward(self, reward):
        """Analyze reward for debugging."""
        analysis = {
            'valid': True,
            'reward_value': reward,
            'is_nan': np.isnan(reward) if isinstance(reward, (int, float)) else False,
            'is_inf': np.isinf(reward) if isinstance(reward, (int, float)) else False,
            'within_bounds': -1.0 <= reward <= 1.0 if isinstance(reward, (int, float)) else False
        }
        
        return analysis
    
    def capture_brain_state(self):
        """Capture brain state for debugging."""
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_neuron_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.brain.getDevelopmentalStage().value if hasattr(self.brain.getDevelopmentalStage(), 'value') else str(self.brain.getDevelopmentalStage())
        }
    
    def capture_agent_state(self):
        """Capture agent state for debugging."""
        return {
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            'developmental_stage': self.agent.getDevelopmentalStage().value if hasattr(self.agent.getDevelopmentalStage(), 'value') else str(self.agent.getDevelopmentalStage()),
            'reward_modulation_enabled': self.agent.isRewardModulationEnabled(),
            'curiosity_enabled': self.agent.isCuriosityEnabled(),
            'development_enabled': self.agent.isDevelopmentEnabled()
        }
    
    def capture_world_state(self):
        """Capture world state for debugging."""
        agent_body = self.world.getAgentBody()
        return {
            'simulation_time': self.world.getSimulationTime(),
            'agent_position': [agent_body.x, agent_body.y] if agent_body else [0, 0],
            'agent_orientation': agent_body.orientation if agent_body else 0.0,
            'world_dimensions': [self.world.getWidth(), self.world.getHeight()]
        }
    
    def detect_issues(self, debug_info):
        """Detect potential issues in debug information."""
        issues = []
        
        # Check brain state
        brain_state = debug_info['brain_state']
        if brain_state['neuron_count'] <= 0:
            issues.append('Invalid neuron count')
        
        if brain_state['firing_neuron_count'] > brain_state['neuron_count']:
            issues.append('Firing neuron count exceeds total neuron count')
        
        if brain_state['average_firing_rate'] > 100.0:
            issues.append('Extremely high firing rate')
        
        # Check agent state
        agent_state = debug_info['agent_state']
        if agent_state['curiosity_level'] > 2.0:
            issues.append('Curiosity level exceeds normal range')
        
        if agent_state['prediction_error'] > 10.0:
            issues.append('Prediction error exceeds normal range')
        
        # Check percept analysis
        percept_analysis = debug_info['percept_analysis']
        if not percept_analysis.get('valid', True):
            issues.append(f"Percept issue: {percept_analysis.get('error', 'Unknown')}")
        
        # Check reward
        reward_analysis = debug_info['reward_analysis']
        if reward_analysis['is_nan']:
            issues.append('Reward is NaN')
        
        if reward_analysis['is_inf']:
            issues.append('Reward is infinite')
        
        return issues
    
    def print_debug_report(self, debug_info, issues):
        """Print debug report with issues."""
        step = debug_info['step']
        
        print(f"\n{'='*60}")
        print(f"DEBUG REPORT - Step {step}")
        print(f"{'='*60}")
        
        print(f"\nIssues Detected ({len(issues)}):")
        for i, issue in enumerate(issues, 1):
            print(f"  {i}. {issue}")
        
        print(f"\nBrain State:")
        brain = debug_info['brain_state']
        print(f"  Neurons: {brain['neuron_count']}, Firing: {brain['firing_neuron_count']}")
        print(f"  Firing Rate: {brain['average_firing_rate']:.2f}")
        print(f"  Excitatory/Inhibitory Ratio: {brain['excitation_inhibition_ratio']:.3f}")
        
        print(f"\nAgent State:")
        agent = debug_info['agent_state']
        print(f"  Curiosity: {agent['curiosity_level']:.3f}")
        print(f"  Prediction Error: {agent['prediction_error']:.3f}")
        print(f"  Neuromodulation: {agent['neuromodulation_level']:.3f}")
        
        print(f"\nPercept Analysis:")
        percept = debug_info['percept_analysis']
        for key, value in percept.items():
            print(f"  {key}: {value}")
        
        if issues:
            print(f"\nRECOMMENDATIONS:")
            for issue in issues:
                if 'neuron count' in issue.lower():
                    print(f"  - Check brain configuration parameters")
                elif 'firing rate' in issue.lower():
                    print(f"  - Reduce neuron excitability or increase inhibition")
                elif 'curiosity' in issue.lower():
                    print(f"  - Adjust curiosity system parameters")
                elif 'prediction error' in issue.lower():
                    print(f"  - Check prediction system configuration")
                elif 'reward' in issue.lower():
                    print(f"  - Review reward calculation and scaling")
                elif 'percept' in issue.lower():
                    print(f"  - Check sensor input and preprocessing")
    
    def visualize_debug_data(self, save_path=None):
        """Visualize debug data."""
        if not self.debug_log:
            print("No debug data to visualize")
            return
        
        # Create visualization plots
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Reward over time
        steps = [d['step'] for d in self.debug_log]
        rewards = [d['reward_analysis']['reward_value'] for d in self.debug_log]
        
        axes[0, 0].plot(steps, rewards)
        axes[0, 0].set_title('Reward Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Reward')
        
        # Plot 2: Brain firing rate over time
        firing_rates = [d['brain_state']['average_firing_rate'] for d in self.debug_log]
        
        axes[0, 1].plot(steps, firing_rates)
        axes[0, 1].set_title('Brain Firing Rate Over Time')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Rate (Hz)')
        
        # Plot 3: Agent curiosity over time
        curiosities = [d['agent_state']['curiosity_level'] for d in self.debug_log]
        
        axes[1, 0].plot(steps, curiosities)
        axes[1, 0].set_title('Agent Curiosity Over Time')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Curiosity Level')
        
        # Plot 4: Simulation time
        sim_times = [d['world_state']['simulation_time'] for d in self.debug_log]
        
        axes[1, 1].plot(steps, sim_times)
        axes[1, 1].set_title('Simulation Time Over Time')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Simulation Time')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path)
            print(f"Debug visualization saved to {save_path}")
        else:
            plt.show()
        
        plt.close()
    
    def export_debug_log(self, filename):
        """Export debug log to file."""
        import json
        
        # Convert numpy values to Python types
        export_data = []
        for entry in self.debug_log:
            export_entry = entry.copy()
            
            # Convert numpy values
            for key, value in export_entry.items():
                if isinstance(value, np.ndarray):
                    export_entry[key] = value.tolist()
                elif isinstance(value, (np.integer, np.floating)):
                    export_entry[key] = float(value)
            
            export_data.append(export_entry)
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"Debug log exported to {filename}")

# Usage example
debugger = Debugger(brain, agent, world)

try:
    # Run simulation with debugging
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        reward = calculate_reward(percept, action, step)  # Define reward function
        
        # Debug current step
        issues = debugger.debug_step(step, percept, action, reward)
        
        if issues:
            print(f"Issues detected at step {step}: {issues}")
finally:
    # Visualize and export debug data
    debugger.visualize_debug_data('debug_visualization.png')
    debugger.export_debug_log('debug_log.json')
```

## Integration Tips for Advanced Users

### 1. Memory Management Best Practices

```python
# Memory-efficient brain configuration
memory_efficient_config = pynlm.createDefaultConfig()
memory_efficient_config.set("brain.neuron_count", 2000)  # Smaller network
memory_efficient_config.set("brain.synapse_density", 0.02)  # Sparse connectivity
memory_efficient_config.set("memory.working_memory.enabled", True)
memory_efficient_config.set("memory.working_memory.capacity", 500)
memory_efficient_config.set("memory.episodic.enabled", False)  # Disable for memory efficiency
memory_efficient_config.set("memory.associative.enabled", False)  # Disable for memory efficiency
```

### 2. Real-time Processing Considerations

```python
# Real-time optimized configuration
realtime_config = pynlm.createDefaultConfig()
realtime_config.set("brain.neuron_count", 1000)  # Smaller for speed
realtime_config.set("brain.synapse_density", 0.01)  # Very sparse
realtime_config.set("brain.v_thresh", -45.0)  # Faster threshold
realtime_config.set("learning.stdp.enable", False)  # Disable learning for real-time
realtime_config.set("learning.hebbian.enable", False)
realtime_config.set("neuromod.dopamine.enable", True)  # Basic neuromodulation
realtime_config.set("neuromod.curiosity.enable", False)  # Disable curiosity for speed
realtime_config.set("neuromod.novelty.enable", False)  # Disable novelty for speed
```

### 3. Batch Processing for Large-Scale Experiments

```python
# Batch processing setup
from batch_simulation_manager import BatchSimulationManager

batch_manager = BatchSimulationManager(
    config_template=advanced_config,
    num_workers=4  # Use 4 CPU cores
)

# Define batch of simulations
simulations = [
    {
        'id': 'experiment_1',
        'parameters': {'neuron_count': 5000, 'synapse_density': 0.05}
    },
    {
        'id': 'experiment_2',
        'parameters': {'neuron_count': 10000, 'synapse_density': 0.03}
    }
]

# Run batch
results = batch_manager.run_batch_sims(simulations, use_parallel=True)
```

### 4. Plugin System for Custom Extensions

```python
# Plugin system setup
from plugin_system import NLMPluginSystem

plugin_system = NLMPluginSystem()

# Create and register custom plugins
custom_vision_plugin = ComputerVisionPlugin({
    'resolution': (128, 128),  # Lower resolution for speed
    'fps': 10
})

custom_curiosity_plugin = CuriosityModulatorPlugin({
    'baseline_curiosity': 0.3,
    'novelty_sensitivity': 0.7
})

# Register plugins
plugin_system.register_sensory_plugin(custom_vision_plugin)
plugin_system.register_neuromodulation_plugin(custom_curiosity_plugin)

# Use in simulation
for step in range(100):
    # Update plugins with global inputs
    plugin_system.update_plugins(0.1, {
        'reward': calculate_reward(),
        'prediction_error': calculate_prediction_error(),
        'novelty': calculate_novelty()
    })
```

## Quick Start Summary

1. **Install NLM:** `pip install -e .`
2. **Import:** `import pynlm`
3. **Create brain:** `brain = pynlm.createBrain(pynlm.createDefaultConfig())`
4. **Initialize:** `brain.initialize()`
5. **Simulate:** `brain.step(step)` in a loop
6. **Get stats:** Use methods like `brain.getTotalSpikeCount()`
7. **Enable advanced features:** Use methods like `agent.enableRewardModulation(True)`

The advanced documentation and resources provide comprehensive guidance for experienced users to build sophisticated NLM applications. Happy experimenting with your virtual brain! 🚀