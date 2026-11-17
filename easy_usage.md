# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. It's a **brain simulator** for computers that lets you create virtual brains which can:

- Learn from experience using multiple learning mechanisms (STDP, Hebbian, structural plasticity)
- Remember things through working memory, episodic memory, and associative memory
- Make decisions using prediction systems and cognitive architectures
- Develop and mature over time through developmental stages
- Use neuromodulation (dopamine, curiosity, novelty detection) to guide behavior

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells that can strengthen or weaken
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns and experiences
- **Development** = The brain maturing and changing over time
- **Motivation** = Drive to seek rewards and explore novel things

## The 3 Core Components

1. **Brain** - The virtual brain that thinks, learns, and makes decisions
2. **World** - The environment the brain lives in (can be a grid world, simulation, etc.)
3. **Agent** - The bridge connecting brain to world (processes sensory input, produces motor output)

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (Neural Computation Only)

```python
import pynlm

# Step 1: Make a brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)  # Smaller brain for faster testing
config.set("connection_probability", 0.1)
config.set("random_seed", 42)
brain = pynlm.createBrain(config)

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of neural computation

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Neurons firing:", brain.getFiringNeuronCount())
print("Development stage:", brain.getDevelopmentalStage())
```

### Example 2: Complete Agent in a World (Full Integration)

```python
import pynlm
import time

# Create everything
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)  # Learn from rewards
action.enableCuriosity(True)       # Explore new things
action.enableDevelopment(True)      # Brain matures over time

# Run for 100 simulation steps
for step in range(100):
    # 1. Update the world (time passes)
    world.update(0.1)  # 100ms per step
    
    # 2. Get what the agent sees (visual input)
    percept = world.getSensoryPercept()
    
    # 3. Tell the brain (sensory processing)
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks (neural computation)
    brain.step(step)
    
    # 5. Get action from brain (decision making)
    action = agent.decodeMotorCommand()
    
    # 6. Do action in world (motor output)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Optional: Apply reward/punishment based on outcome
    # reward = world.getReward()
    # if reward != 0:
    #     agent.applyRewardModulation(reward, 0.0)

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Memory usage:", brain.getWorkingMemorySize(), "working memory items")
print("Development stage:", brain.getDevelopmentalStage())
```

---

## Advanced Examples

### Example 3: Learning Experiment with Plasticity

```python
import pynlm

# Create a brain with plasticity enabled
config = pynlm.createDefaultConfig()
config.set("neuron_count", 200)
config.set("stdp_ltp_weight", 0.02)  # Strong potentiation
config.set("stdp_ltd_weight", 0.015) # Moderate depression
config.set("synaptogenesis_rate", 0.001)  # Allow new connections

brain = pynlm.createBrain(config)
brain.initialize()

# Enable plasticity mechanisms
brain.enablePlasticity(PlasticityFlags.STDP | PlasticityFlags.Hebbadian)

print("=== Plasticity Learning Experiment ===")
print("Initial brain state:")
print("  Neurons:", brain.getTotalNeuronCount())
print("  Synapses:", brain.getTotalSynapseCount())
print("  Firing rate:", brain.getAverageFiringRate())

# Present a learning pattern repeatedly
pattern_neurons = list(range(10))  # First 10 neurons
for epoch in range(5):  # 5 learning epochs
    for step in range(100):  # 100 steps per epoch
        # Present pattern input
        for neuron_id in pattern_neurons:
            brain.injectCurrent(neuron_id, 50.0)
        
        brain.step(epoch * 100 + step)
    
    print(f"Epoch {epoch + 1}:")
    print("  Total spikes:", brain.getTotalSpikeCount())
    print("  Mean weight change:", brain.getMeanSynapticWeightChange())

print("Learning experiment complete!")
```

### Example 4: Memory System Demonstration

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
config.set("neuron_count", 300)
config.set("episodic_memory_capacity", 50)
config.set("working_memory_capacity", 100)

brain = pynlm.createBrain(config)
brain.initialize()

print("=== Memory System Test ===")

# Test working memory
print("Testing working memory...")
working_memory = brain.getWorkingMemory()
print(f"  Working memory capacity: {working_memory.getCapacity()}")
print(f"  Current items: {working_memory.getSize()}")

# Test episodic memory
print("Testing episodic memory...")
episodic_memory = brain.getEpisodicMemory()
print(f"  Episodic memory capacity: {episodic_memory.getCapacity()}")
print(f"  Current episodes: {episodic_memory.getSize()}")

# Simulate some memory formation
for step in range(50):
    brain.step(step)
    
    # Encode some information
    experience = {
        "step": step,
        "spike_count": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "sensory_input": "pattern_" + str(step % 5)
    }
    
    episodic_memory.encode(experience)

print(f"  After encoding: {episodic_memory.getSize()} episodes")

# Test replay
print("Testing memory replay...")
replayed_episodes = episodic_memory.replay(10)  # Replay last 10 episodes
print(f"  Replayed {len(replayed_episodes)} episodes")
print("Memory system test complete!")
```

### Example 5: Neuromodulation and Motivation

```python
import pynlm

# Create brain with neuromodulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize agent with neuromodulation
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulators
agent.enableCuriosity(True)  # Drive to explore novel things
agent.enableNoveltyDetection(True)  # Detect surprises
agent.enablePredictionError(True)   # Learn from prediction errors

print("=== Neuromodulation Test ===")
print("Neuromodulation systems enabled:")
print(f"  Curiosity: {agent.getCuriosityStrength()}")
print(f"  Novelty detection: {agent.getNoveltyDetectionStrength()}")
print(f"  Prediction error learning: {agent.getPredictionErrorStrength()}")

# Run simulation with curiosity-driven exploration
for step in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate curiosity (surprise at sensory input)
    curiosity = agent.getCuriosityStrength() * (1.0 - percept.similarity_to_previous)
    print(f"Step {step}: Action={action}, Curiosity={curiosity:.3f}")

print("Neuromodulation test complete!")
```

---

## Advanced Configuration Options

### Creating Custom Configurations

```python
import pynlm

# Create and customize a configuration
config = pynlm.createDefaultConfig()

# Core neural parameters
config.set("neuron_count", 500)                    # Number of neurons
config.set("region_count", 3)                      # Number of brain regions  
config.set("connection_probability", 0.12)         # How many connections exist
config.set("simulation_timestep", 0.001)           # Time resolution (1ms)
config.set("random_seed", 12345)                   # For reproducible experiments

# Learning parameters
config.set("stdp_ltp_weight", 0.015)               # STDP long-term potentiation
config.set("stdp_ltd_weight", 0.012)               # STDP long-term depression
config.set("stdp_tau", 20.0)                       # STDP time constant (ms)
config.set("plasticity_learning_rate", 0.01)      # Hebbian learning rate

# Structural plasticity
config.set("synaptogenesis_rate", 0.0005)         # New connections per step
config.set("pruning_rate", 0.00005)               # Connection removal per step

# Memory parameters
config.set("working_memory_capacity", 200)         # Working memory size
config.set("episodic_memory_capacity", 100)       # Episodic memory size

# Development parameters
config.set("development_synaptogenesis_rate", 0.001)  # Developmental growth
config.set("development_pruning_rate", 0.0001)         # Developmental pruning

# Neuromodulation
config.set("dopamine_baseline", 0.1)              # Baseline dopamine level
config.set("curiosity_strength", 0.8)             # Curiosity drive
config.set("novelty_threshold", 0.3)              # Novelty detection sensitivity

# Environment parameters
config.set("environment_width", 20)               # World width
config.set("environment_height", 20)              # World height
config.set("vision_width", 8)                      # Visual field width
config.set("vision_height", 8)                     # Visual field height

print(config.summary())
```

---

## Pattern: Complete Agent Architecture

```python
import pynlm
import time
import random

class NLMInteraction:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.setup_brain()
        self.setup_world()
        self.setup_agent()
        self.setup_learning()
        
    def setup_brain(self):
        """Initialize the brain with configuration"""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
    def setup_world(self):
        """Initialize the simulation world"""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=self.config.getOr("environment_width", 20),
            height=self.config.getOr("environment_height", 20),
            visionWidth=self.config.getOr("vision_width", 8),
            visionHeight=self.config.getOr("vision_height", 8)
        )
        self.world.reset()
        
    def setup_agent(self):
        """Create and initialize the agent"""
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
    def setup_learning(self):
        """Enable learning mechanisms"""
        self.agent.enableRewardModulation(
            self.config.getOr("reward_modulation", True)
        )
        self.agent.enableCuriosity(
            self.config.getOr("curiosity_strength", 0.8) > 0
        )
        self.agent.enableStructuralPlasticity(
            self.config.getOr("structural_plasticity", True)
        )
        self.agent.enableDevelopment(
            self.config.getOr("development_enabled", True)
        )
        
    def run_simulation(self, steps, mode="closed_loop"):
        """Run the complete agent simulation"""
        print(f"Running {mode} simulation for {steps} steps...")
        
        for step in range(steps):
            # Update environment
            self.world.update(0.1)  # 100ms time step
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Neural computation
            self.brain.step(step)
            
            # Generate motor output
            action = self.agent.decodeMotorCommand()
            
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Closed-loop reward
            if mode == "closed_loop":
                reward = self.world.getReward()
                if reward != 0:
                    self.agent.applyRewardModulation(reward, 0.0)
            
            # Log progress
            if step % 20 == 0:
                print(f"Step {step}:")
                print(f"  Action: {action}")
                print(f"  Neurons firing: {self.brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
                print(f"  Memory items: {self.brain.getWorkingMemorySize()}")
                print(f"  Development stage: {self.brain.getDevelopmentalStage()}")
                
        print("Simulation complete!")
        return self.get_stats()
        
    def get_stats(self):
        """Get comprehensive simulation statistics"""
        return {
            "total_spikes": self.brain.getTotalSpikeCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "average_firing_rate": self.brain.getAverageFiringRate(),
            "development_stage": self.brain.getDevelopmentalStage(),
            "working_memory_size": self.brain.getWorkingMemorySize(),
            "episodic_memory_size": self.brain.getEpisodicMemorySize(),
            "structural_connections": self.brain.getTotalSynapseCount(),
            "plasticity_enabled": self.agent.getPlasticityEnabled()
        }

# Example usage
if __name__ == "__main__":
    # Create interaction with custom settings
    interaction = NLMInteraction()
    
    # Run a closed-loop simulation
    stats = interaction.run_simulation(200, "closed_loop")
    
    print("\n=== Final Statistics ===")
    for key, value in stats.items():
        print(f"{key}: {value}")
```

---

## Command Line Interface

### Basic Usage

The NLM system provides several command-line executables:

```bash
# Main neural computation demo (Phase 2)
./nlm [options]

# Phase 3 demonstration
./nlm_phase3_demo [options]

# Phase 4 demonstration  
./nlm_phase4_demo [options]

# Phase 5 demo (sandbox learning)
./nlm_phase5_demo [options]

# Phase 6 integration demo
./nlm_phase6_demo [options]

# Performance benchmark
./nlm_benchmark [options]
```

### Available Options

Common command-line options include:

```bash
# Configuration file
--config=filename.cfg

# Override configuration values
--neuron_count=1000
--region_count=3
--connection_probability=0.15

# Run specific tests
--test=basic_connectivity
--test=plasticity
--test=stdp_verification
--test=memory_integration

# Output and logging
--verbose          # Detailed output
--quiet            # Minimal output
--log_level=INFO   # Set log level (DEBUG, INFO, WARNING, ERROR)

# Simulation control
--steps=1000       # Number of simulation steps
--timestep=0.001   # Simulation timestep (seconds)
--duration=10.0    # Simulation duration (seconds)

# Performance
--threads=4        # Number of threads for parallel processing
--device=gpu       # Use GPU acceleration if available
```

### Example Command Line Usage

```bash
# Run basic neural computation test
./nlm --config=custom.cfg --test=plasticity --steps=1000

# Run memory integration demo  
./nlm_phase6_demo --test=memory --verbose --steps=500

# Benchmark with specific configuration
./nlm_benchmark --neuron_count=2000 --region_count=4 --threads=8

# Debug run with detailed logging
./nlm_phase5_demo --verbose --log_level=DEBUG --test=sandbox
```

---

## Troubleshooting

### "My brain isn't doing anything"

**Check:**
- Did you call `brain.initialize()`?
- Are there enough neurons configured?
- Is sensory input properly connected?

**Solutions:**
- Increase `neuron_count` or `steps`
- Check that `world.update()` is being called
- Verify that `agent.processSensoryInput()` is receiving data
- Enable `curiosity` or other neuromodulators to drive activity

### "The agent isn't moving"

**Check:**
- Did you call `world.applyMotorCommand()`?
- Is the world being updated regularly?
- Are motor neurons receiving proper input?

**Solutions:**
- Ensure `world.update()` is called each step
- Check motor neuron configuration
- Verify action selection is working correctly
- Test with simpler actions first

### "Everything is 0" (No activity)

**Check:**
- Are there enough simulation steps for "warming up"?
- Is sensory input being properly injected?
- Are synapses being formed?

**Solutions:**
- Run more steps (100-1000 for initial activity)
- Check sensory neuron connectivity
- Verify that structural plasticity is enabled
- Use larger `connection_probability`
- Manually inject current into specific neurons: `brain.injectCurrent(neuron_id, amount)`

### "Memory isn't working"

**Check:**
- Are memory systems enabled in configuration?
- Is the brain capacity sufficient?
- Are experiences being properly encoded?

**Solutions:**
- Enable `episodic_memory_capacity` and `working_memory_capacity`
- Ensure `agent.enableCuriosity(True)` is called
- Check that `episodic_memory.encode()` is being called
- Use smaller capacities for testing first

### "Performance is slow"

**Check:**
- Are you using too many neurons?
- Is the configuration optimized?
- Are you running unnecessary features?

**Solutions:**
- Reduce `neuron_count` for testing
- Disable unused plasticity mechanisms
- Use fewer regions for faster processing
- Consider using `nlm_benchmark` to measure performance

---

## Next Steps

### For Beginners:

1. **Start with basic examples** - Use `easy_usage.md` and the Python examples
2. **Explore configurations** - Try different `neuron_count`, `connection_probability` values
3. **Run demos** - Execute the various `./nlm_*_demo` executables
4. **Read documentation** - Check `HOW_TO_USE.md` for detailed technical information
5. **Examine architecture** - Review `docs/ARCHITECTURE.md` to understand system design

### For Advanced Users:

1. **Study Phase 6 integration** - The `nlm_phase6_demo` shows all systems working together
2. **Performance optimization** - Use `nlm_benchmark` to tune configurations
3. **Custom experiments** - Create your own configurations in `configs/`
4. **Memory systems** - Explore `docs/MEMORY.md` for memory system details
5. **Neuromodulation** - Study `docs/NEUROMODULATION.md` for motivation systems
6. **Development** - Review `docs/DEVELOPMENT.md` for brain maturation

### For Researchers:

1. **Phase-specific demos** - Each phase has executable demonstrations
2. **Documentation** - Comprehensive documentation in the `docs/` directory
3. **Configuration management** - Custom configs in `configs/` for different experiments
4. **Performance analysis** - Use benchmarking tools for system optimization
5. **Extension development** - Add new neuron types, plasticity rules, or memory systems

---

## Quick Reference

| What you want | Python Code | Command Line |
|--------------|-------------|--------------|
| Create simple brain | `pynlm.createBrain(pynlm.createDefaultConfig())` | `./nlm` |
| Start brain | `brain.initialize()` | (automatic) |
| Make brain think | `brain.step(step_number)` | --steps=N |
| Create simulation world | `pynlm.createSimpleWorld()` | `./nlm_phase3_demo` |
| Create agent | `pynlm.createAgentBrain(brain)` | (automatic) |
| Get brain statistics | `brain.getFiringNeuronCount()` | (automatic) |
| See world | `world.getSensoryPercept()` | (automatic) |
| Make action | `agent.decodeMotorCommand()` | (automatic) |
| Enable learning | `agent.enableCuriosity(True)` | --enable_curiosity |
| Save brain state | `brain.saveToFile("checkpoint.nml")` | (not available) |
| Load brain state | `brain.loadFromFile("checkpoint.nml")` | (not available) |

---

That's it! You're now ready to use NLM with confidence. The system provides flexible options for everything from simple neural computation to complex agent-based learning and decision-making.

For the most up-to-date information, always check the current version of `easy_usage.md` and the detailed technical documentation in `HOW_TO_USE.md`.
