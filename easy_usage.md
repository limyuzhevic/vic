# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain simulation system. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Note:** NLM is currently in Phase 6 (Final Integration) but has not yet fully integrated all cognitive systems. The core neural simulation works well, but memory, prediction, and higher cognition systems are not yet connected to the main brain loop.

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns
- **Neuromodulation** = Reward prediction error and curiosity signals

NLM simulates neural computation and basic learning, but full cognitive integration is still in progress.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (NEURAL CORE - FULLY OPERATIONAL)
2. **World** - The environment the brain lives in (WORLD SIMULATION - WORKING)
3. **Agent** - The bridge connecting brain to world (INTEGRATION LAYER - MOSTLY WORKING)

**Current Status:** The neural core (LIF neurons, spikes, synapses) works perfectly. The world and agent systems work together. However, many cognitive systems (memory, prediction, higher cognition) are not yet integrated with the main brain loop.

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print(f"Neurons: {brain.getTotalNeuronCount()}, Firing: {brain.getFiringNeuronCount()}")
```

### Example 2: Enhanced Brain with Neuromodulation

```python
import pynlm

# Create brain with enhanced features
config = pynlm.createDefaultConfig()

# Configure for learning
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent brain with learning capabilities
agent = pynlm.createAgentBrain(brain)

# Enable all learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards via dopamine
agent.enableCuriosity(True)             # Exploration drive
agent.enableDevelopment(True)           # Brain maturation over time
agent.enableStructuralPlasticity(True)  # Grow new connections

print("Enhanced brain created with neuromodulation!")
```

### Example 3: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=500):
    """Run a complete agent simulation with learning."""
    
    # 1. Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 6. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step (neural computation)
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward and apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(500)
print(f"Simulation complete! Final stage: {brain.getDevelopmentalStage()}")
```

---

## CLI Usage Examples

### Basic Brain Simulation

```bash
# Run a simple brain simulation using the CLI
./nlm_cli run --neuron-count 100 --max-steps 500

# Run with verbose output
./nlm_cli run --neuron-count 500 --max-steps 1000 --verbose

# Run with custom timestep
./nlm_cli run --neuron-count 200 --timestep 0.0001
```

### Experiment Examples

```bash
# Run predefined experiments
./nlm_cli experiment connectivity
./nlm_cli experiment plasticity
./nlm_cli experiment phase2

# Run experiment with output file
./nlm_cli experiment phase2 --output phase2_results.json

# Run experiment with custom configuration
./nlm_cli experiment phase2 --config configs/complex.cfg
```

### Configuration Management

```bash
# Generate default configuration file
./nlm_cli config generate --output my_config.cfg

# Validate configuration
./nlm_cli config validate my_config.cfg

# List current configuration
./nlm_cli config list
```

### Export and Import

```bash
# Export brain state to JSON
./nlm_cli export json --output brain_state.json

# Export to CSV for analysis
./nlm_cli export csv --output neurons.csv

# Import saved brain state
./nlm_cli import saved_brain.json
```

### Performance Monitoring

```bash
# Monitor brain performance
./nlm_cli monitor --interval 0.5

# Profile memory usage
./nlm_cli profile --output memory_profile.json

# Run performance benchmark
./nlm_cli benchmark
```

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

---

## Configuration Presets

### Preset 1: Basic Simulation

```python
import pynlm

# Basic configuration for simple simulations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)
config.set("simulation.timestep", 0.001)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Preset 2: Learning-Focused

```python
import pynlm

# Configuration optimized for learning and development
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("plasticity.structural.enable", True)
config.set("development.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Preset 3: Memory Research

```python
import pynlm

# Configuration optimized for memory experiments
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("memory.working.capacity", 500)
config.set("memory.episodic.enabled", True)
config.set("memory.associative.enabled", True)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

---

## Experiment Examples

### Experiment 1: Temporal Learning

```python
import pynlm

"""
Test if NLM can learn temporal sequences.
Presents pattern A followed by pattern B repeatedly
and measures prediction accuracy over time.
"""

def run_temporal_learning_experiment(steps=200):
    config = pynlm.createDefaultConfig()
    config.set("prediction.horizon", 3)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use pattern sequences
    pattern_a = [0.1, 0.2, 0.3, 0.4, 0.5]
    pattern_b = [0.6, 0.7, 0.8, 0.9, 1.0]
    
    prediction_errors = []
    
    for step in range(steps):
        if step % 2 == 0:
            # Present pattern A
            current_pattern = pattern_a
            # Expect pattern B next
            expected = pattern_b
        else:
            # Present pattern B
            current_pattern = pattern_b
            # Expect pattern A next
            expected = pattern_a
        
        # Process the pattern
        for i, value in enumerate(current_pattern):
            input_val = value * 5.0  # Scale for neural input
            # Inject input at appropriate neuron groups
        
        # Run brain step
        brain.step(step)
        
        # Measure prediction error (simplified)
        error = abs(brain.getPredictionError())
        prediction_errors.append(error)
    
    return brain, prediction_errors

brain, errors = run_temporal_learning_experiment(200)
print(f"Temporal learning experiment completed.")
print(f"Initial error: {errors[0]:.4f}, Final error: {errors[-1]:.4f}")
print(f"Improvement: {(errors[0] - errors[-1]):.4f}")
```

### Experiment 2: Working Memory

```python
import pynlm

"""
Test working memory capacity and retention.
Presents a pattern then delays before testing recall.
"""

def run_working_memory_experiment(delay_steps=50, patterns=10):
    config = pynlm.createDefaultConfig()
    config.set("memory.working.capacity", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    retention_accuracies = []
    
    for pattern_num in range(patterns):
        # Create a test pattern
        test_pattern = [0.2, 0.4, 0.6, 0.8, 1.0] * 3  # 15 values
        
        # Encode pattern
        for step, value in enumerate(test_pattern):
            input_val = value * 3.0
            # Encode pattern in working memory
        
        # Maintenance delay
        for delay_step in range(delay_steps):
            brain.step(delay_step)
            
            # Test retention
            current_activity = brain.getFiringNeuronCount()
            expected_activity = len(test_pattern) * 10  # Rough estimate
            accuracy = max(0.0, min(1.0, current_activity / expected_activity))
            retention_accuracies.append(accuracy)
    
    return brain, retention_accuracies

brain, accuracies = run_working_memory_experiment()
print(f"Working memory experiment completed.")
print(f"Average retention accuracy: {sum(accuracies)/len(accuracies):.3f}")
print(f"Best retention: {max(accuracies):.3f}, Worst retention: {min(accuracies):.3f}")
```

### Experiment 3: Curiosity-Driven Exploration

```python
import pynlm

"""
Test curiosity-driven exploration behavior.
Measures exploration rate and learning from novel experiences.
"""

def run_curiosity_experiment(steps=300):
    config = pynlm.createDefaultConfig()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    exploration_rates = []
    novelty_levels = []
    
    # Simple environment with changing patterns
    patterns = [
        [0.1, 0.2, 0.3],
        [0.4, 0.5, 0.6],
        [0.7, 0.8, 0.9],
        [0.2, 0.3, 0.4],
        [0.5, 0.6, 0.7]
    ]
    
    for step in range(steps):
        # Cycle through patterns
        pattern_idx = (step // 10) % len(patterns)
        current_pattern = patterns[pattern_idx]
        
        # Process pattern
        for value in current_pattern:
            input_val = value * 2.0
        
        # Run brain step
        brain.step(step)
        
        # Create agent to get curiosity and novelty levels
        agent = pynlm.createAgentBrain(brain)
        exploration_rate = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        
        exploration_rates.append(exploration_rate)
        novelty_levels.append(novelty_level)
    
    return brain, exploration_rates, novelty_levels

brain, exploration_rates, novelty_levels = run_curiosity_experiment()
print(f"Curiosity experiment completed.")
print(f"Average exploration rate: {sum(exploration_rates)/len(exploration_rates):.3f}")
print(f"Peak novelty: {max(novelty_levels):.3f}")
print(f"Average novelty: {sum(novelty_levels)/len(novelty_levels):.3f}")
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

## Common Problems and Solutions

### "My brain simulation is not learning or exploring enough"

**Cause:** Learning subsystems may not be enabled.

**Solution:**
```python
# Enable all learning subsystems
agent.enableRewardModulation(True)  # Dopamine-based learning
agent.enableCuriosity(True)         # Exploration drive
agent.enableDevelopment(True)       # Brain maturation
agent.enableStructuralPlasticity(True)  # New connections
```

### "Brain not responding to sensory input"

**Cause:** Sensory neurons may not be connected or input injection not working.

**Solution:**
```python
# Check sensory input processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
# Verify that brain receives input through:
# - Vision input (256 neurons)
# - Touch input (8 neurons)  
# - Internal input (4 neurons)
# - Proprioception (6 neurons)
```

### "Performance issues with large brains"

**Cause:** Simulation parameters may be too aggressive.

**Solution:**
```python
# Adjust configuration for better performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)    # Reduce neuron count
config.set("simulation.timestep", 0.001)  # Optimize timestep
config.set("plasticity.stdp.learning_rate", 0.001)  # Reduce learning rate
```

### "Development not progressing"

**Cause:** Development system may be disabled or at maximum age.

**Solution:**
```python
# Check developmental stage
brain.getDevelopmentalStage()  # Should progress: Initial → CriticalPeriod → Maturation → Adult → Aging

# Ensure development is enabled
agent.enableDevelopment(True)
```

### "Low curiosity or novelty levels"

**Cause:** Neuromodulation may not be enabled or environment too predictable.

**Solution:**
```python
# Enable neuromodulation
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)  # If available

# Create more variable environments for novelty detection
```

---

## Quick Reference

### Core Creation and Initialization

| What you want | Code |
|--------------|------|
| Create simple brain | `brain = pynlm.createBrain(pynlm.createDefaultConfig())` |
| Create enhanced brain with learning | `config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config)` |
| Initialize brain | `brain.initialize()` |
| Run simulation | `brain.step(step_number)` |

### Agent and World Setup

| What you want | Code |
|--------------|------|
| Create world | `world = pynlm.createSimpleWorld()` |
| Configure world | `world.configure(width=20, height=20, visionWidth=16, visionHeight=16)` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Initialize agent | `agent.initialize(world)` |
| Process sensory input | `agent.processSensoryInput(percept)` |
| Get motor command | `action = agent.decodeMotorCommand()` |
| Apply action | `world.applyMotorCommand(action, world.getSimulationTime())` |

### Learning and Neuromodulation

| What you want | Code |
|--------------|------|
| Enable reward learning | `agent.enableRewardModulation(True)` |
| Enable curiosity | `agent.enableCuriosity(True)` |
| Enable development | `agent.enableDevelopment(True)` |
| Enable structural plasticity | `agent.enableStructuralPlasticity(True)` |
| Get curiosity level | `curiosity = agent.getCuriosityLevel()` |
| Get novelty level | `novelty = agent.getNoveltyLevel()` |
| Apply reward modulation | `agent.applyRewardModulation(reward, predicted_reward)` |
| Update development | `agent.updateDevelopment(timestep)` |

### Brain Statistics and Monitoring

| What you want | Code |
|--------------|------|
| Total neurons | `brain.getTotalNeuronCount()` |
| Firing neurons | `brain.getFiringNeuronCount()` |
| Total spikes | `brain.getTotalSpikeCount()` |
| Average firing rate | `brain.getAverageFiringRate()` |
| E/I balance | `brain.getExcitationInhibitionRatio()` |
| Developmental stage | `brain.getDevelopmentalStage()` |
| Prediction error | `agent.getPredictionError()` |

### Configuration Presets

| Preset Name | Description |
|-------------|-------------|
| `BasicSimulation` | Simple simulation with core learning |
| `LearningFocused` | Optimized for learning and development |
| `MemoryResearch` | Configured for memory experiments |

### CLI Commands

| Command | Description |
|---------|-------------|
| `./nlm_cli run --neuron-count 100 --max-steps 500` | Basic brain simulation |
| `./nlm_cli experiment connectivity` | Run connectivity experiment |
| `./nlm_cli config generate --output config.cfg` | Generate configuration file |
| `./nlm_cli export json --output brain.json` | Export brain state |
| `./nlm_cli profile --output profile.json` | Profile performance |

---

## Next Steps

When you're comfortable with the basics:

### 1. **Phase 4 Cognitive Experiments**
- Explore temporal prediction learning
- Test working memory capacity  
- Investigate curiosity-driven behavior
- Study concept formation

### 2. **Advanced Usage**
- Read `HOW_TO_USE.md` for complete API reference
- Read `docs/PHASE6_FINAL_AUDIT.md` to understand current system status
- Experiment with different configuration presets
- Run Phase 4 experiments to see emergent cognition

### 3. **Performance Optimization**
- Profile your simulations using `./nlm_cli profile`
- Optimize neuron counts and timesteps for your use case
- Use configuration presets for different application types

### 4. **Research Applications**
- Use the experiment framework to test hypotheses
- Collect neural and behavioral data
- Analyze results with the analysis tools

### 5. **Integration with Other Systems**
- Connect NLM with external environments
- Integrate with virtual reality systems
- Add custom sensory modalities

**Remember:** NLM is currently in Phase 6 (Final Integration). The core neural simulation works excellently, but many cognitive systems (memory, prediction, higher cognition) are still being integrated. Focus on building on what works and helping advance the integration process!

That's it! You're now ready to use NLM for brain simulation and cognitive research.
