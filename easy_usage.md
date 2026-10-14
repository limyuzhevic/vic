# NLM - Enhanced Easy Usage Guide for Beginners

**Updated Version** - Building on the original easy_usage.md with improved structure and examples

## What is NLM?

NLM (Neural Learning Machine, 熙然) is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

### The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (Enhanced)

```python
import pynlm

# Step 1: Make a brain with optimized Phase 6 settings
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps with monitoring
for i in range(100):
    brain.step(i)  # One step of thinking
    if i % 25 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

### Example 2: Complete Agent with Monitoring (Enhanced)

```python
import pynlm

# Setup with monitoring and progress tracking
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run with comprehensive monitoring
stats = []
for step in range(50):
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
    
    # 7. Record statistics
    if step % 10 == 0:
        stat = {
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'dev_stage': brain.getDevelopmentalStage()
        }
        stats.append(stat)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.2f}")

print("Simulation finished!")
print(f"Final statistics: {stats[-1]}")
```

### Example 3: Benchmarking and Performance Testing (New)

```python
import pynlm
import time

# Create different configuration types for benchmarking
configs = {
    'simple': pynlm.createDefaultConfig(),
    'high_performance': pynlm.createHighPerformanceConfig(),
    'development': pynlm.createDevelopmentConfig(),
    'exploration': pynlm.createExplorationConfig()
}

benchmarks = {}

for name, config in configs.items():
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Benchmark the brain
    start_time = time.time()
    for step in range(1000):
        brain.step(step)
    elapsed = time.time() - start_time
    
    benchmarks[name] = {
        'time': elapsed,
        'steps_per_second': 1000 / elapsed,
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount()
    }
    
    print(f"{name:20s}: {elapsed:8.3f}s, {1000/elapsed:8.0f} steps/s")

print("\nBenchmark Summary:")
for name, results in benchmarks.items():
    print(f"{name:20s}: {results['steps_per_second']:8.0f} steps/s, "
          f"{results['total_spikes']:8d} spikes")
```

### Example 4: Batch Processing (New)

```python
import pynlm

# Create a batch of brains for parallel processing
batch = pynlm.SimulationBatch("ExperimentBatch")

# Add multiple brains with different configurations
for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200 + i * 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    batch.addBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    batch.addWorld(world)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    batch.addAgent(agent)

# Run batch simulation
print("Running batch simulation...")
batch.runBatch(200, timestep=0.1)

print("Batch simulation complete!")
```

### Example 5: Data Export and Analysis (New)

```python
import pynlm

# Create and run a brain
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(500):
    brain.step(step)

# Extract neural features for analysis
features = pynlm.extractFeatures(brain)
print("Extracted features:", features)

# Generate report
from pynlm.utils import PerformanceMetrics
metrics = PerformanceMetrics()
metrics.stepsCompleted = 500
metrics.totalSpikes = brain.getTotalSpikeCount()
metrics.avgFiringRate = brain.getAverageFiringRate()

# Generate detailed report
report = pynlm.generateReport(brain, metrics, format="text")
print("\n" + report)

# Export network data
network_data = pynlm.exportNetwork(brain, format="json")
print("\nNetwork data exported (first 500 chars):")
print(network_data[:500] + "...")
```

---

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation with Statistics

```python
def run_monitored_simulation(brain, world, agent, num_steps):
    """Run a complete simulation with comprehensive monitoring."""
    stats = []
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect statistics
        if step % 50 == 0:
            stat = {
                'step': step,
                'firing': brain.getFiringNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dev_stage': brain.getDevelopmentalStage()
            }
            stats.append(stat)
            
            # Print progress
            print(f"Step {step}: {stat['firing']} neurons firing, "
                  f"rate: {stat['rate']:.2f} Hz, "
                  f"curiosity: {stat['curiosity']:.2f}")
    
    return stats

# Use it like this:
# stats = run_monitored_simulation(brain, world, agent, 1000)
# print(f"Simulation completed. Final stats: {stats[-1]}")
```

### Pattern 2: Configuration Management

```python
def create_experiment_config(experiment_name, **kwargs):
    """Create a configuration for a specific experiment."""
    # Start with base config
    config = pynlm.createDefaultConfig()
    
    # Apply experiment-specific settings
    if experiment_name == "fast_learning":
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("neuromod.curiosity.enable", True)
        
    elif experiment_name == "slow_development":
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.05)
        config.set("development.enable", True)
        config.set("neuromod.novelty.enable", True)
        
    elif experiment_name == "high_stability":
        config.set("brain.neuron_count", 300)
        config.set("brain.synapse_density", 0.01)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", False)
    
    # Override with user-provided values
    for key, value in kwargs.items():
        config.set(key, value)
    
    return config

# Use it like this:
# fast_config = create_experiment_config("fast_learning", learning_rate=0.01)
# dev_config = create_experiment_config("slow_development", neuron_count=1000)
```

### Pattern 3: Error Handling and Recovery

```python
class NLMExperiment:
    """Wrapper class for managing NLM experiments with error handling."""
    
    def __init__(self, config=None, world=None, agent=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = world or pynlm.createSimpleWorld()
        self.agent = agent or pynlm.createAgentBrain(None)  # Will set later
        
    def run_episode(self, max_steps=1000, checkpoint_file=None):
        """Run a complete episode with error handling."""
        try:
            # Initialize
            if not self.brain:
                self.brain = pynlm.createBrain(self.config)
                self.agent = pynlm.createAgentBrain(self.brain)
                self.brain.initialize()
            else:
                self.brain.reset()
                self.brain.initialize()
            
            self.world.configure(10, 10, 8, 8)
            self.world.reset()
            self.agent.initialize(self.world)
            
            # Run simulation
            for step in range(max_steps):
                try:
                    self.world.update(0.1)
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    self.brain.step(step)
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                    
                except Exception as e:
                    print(f"Error at step {step}: {e}")
                    break
                    
            # Checkpoint if requested
            if checkpoint_file and self.brain:
                if not self.brain.save(checkpoint_file):
                    print(f"Warning: Could not save checkpoint to {checkpoint_file}")
                    
            return True
            
        except Exception as e:
            print(f"Experiment failed: {e}")
            return False
    
    def load_checkpoint(self, checkpoint_file):
        """Load a saved brain state."""
        if self.brain and self.brain.load(checkpoint_file):
            print(f"Successfully loaded checkpoint from {checkpoint_file}")
            return True
        else:
            print(f"Failed to load checkpoint from {checkpoint_file}")
            return False

# Use it like this:
# experiment = NLMExperiment()
# success = experiment.run_episode(max_steps=500, checkpoint_file="checkpoint.bin")
# if success:
#     print("Experiment completed successfully!")
```

---

## The Simplest Possible Example (Updated)

```python
import pynlm

# One line to create a virtual brain with optimized Phase 6 settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("This is Phase 6: The Final Integration")
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup, `createDevelopmentConfig()` for learning experiments, `createExplorationConfig()` for curious agents)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- Records statistics and performance metrics

### `brain.benchmark(steps)`
Benchmark brain performance and return (time, steps_per_sec, total_spikes)

### `brain.extractFeatures()`
Extract neural features for analysis (returns dictionary)

### `brain.exportNetwork(format)`
Export brain network in JSON or CSV format (returns string)

### `brain.runExperiment(world, agent, steps, timestep)`
Run a complete experiment with world interaction (returns performance metrics)

### `brain.generateReport(metrics, format)`
Generate detailed analysis report in text or JSON format (returns string)

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `agent.enableRewardModulation(True/False)`
Enable or disable reward-based learning (advanced feature)

### `agent.enableCuriosity(True/False)`
Enable or disable curiosity-driven exploration (advanced feature)

### `agent.enableDevelopment(True/False)`
Enable or disable developmental changes (advanced feature)

---

## Mini Projects (Expanded)

### Project 1: Silent Brain with Analysis

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Created {brain.getTotalNeuronCount()} neurons with {brain.getTotalSynapseCount()} synapses")
print(f"Average firing rate: {brain.getAverageFiringRate():.4f} Hz")
```

### Project 2: Brain Watching a World with Monitoring

```python
import pynlm

# Setup with enhanced monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world with curiosity enabled
agent.enableCuriosity(True)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    if i % 10 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.2f}")

print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.4f}")
```

### Project 3: Complete Agent with Learning and Development (Enhanced)

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent with comprehensive monitoring
print("Starting agent simulation...")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.4f} Hz")
print(f"Total learning: {agent.getPredictionError():.4f} prediction error")
```

### Project 4: Performance Benchmarking

```python
import pynlm
import time

# Test different configurations
configs = [
    ("Basic", pynlm.createDefaultConfig()),
    ("Optimized", pynlm.createHighPerformanceConfig()),
    ("Learning", pynlm.createDevelopmentConfig()),
    ("Exploratory", pynlm.createExplorationConfig())
]

print("Performance Comparison:")
print("=" * 50)

for name, config in configs:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Time the simulation
    start = time.time()
    for i in range(500):
        brain.step(i)
    elapsed = time.time() - start
    
    print(f"{name:12s}: {elapsed:6.3f}s, {500/elapsed:8.0f} steps/s, "
          f"{brain.getTotalSpikeCount():8d} spikes")
```

---

## Quick Reference (Enhanced)

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
| Benchmark brain | `brain.benchmark(steps)` |
| Extract features | `pynlm.extractFeatures(brain)` |
| Export network | `brain.exportNetwork(format)` |
| Run batch | `pynlm.SimulationBatch()` |

---

## Troubleshooting (Enhanced)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Try enabling curiosity for exploration

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"Benchmark is too slow"**
- Try creating a high-performance configuration
- Reduce the number of steps
- Check if `enableCuriosity` is draining resources

**"Agent learning is stuck"**
- Check if `enableRewardModulation` is True
- Verify `enableDevelopment` is enabled
- Monitor curiosity levels - too high curiosity can prevent learning

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - Comprehensive C++ setup guide
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Technical architecture
3. **Experiment with different configurations** - Try the specialized configs:
   - `createDevelopmentConfig()` - For rapid learning experiments
   - `createExplorationConfig()` - For curious, exploratory agents
   - `createHighPerformanceConfig()` - For benchmarking and performance
4. **Try the advanced examples** in the `examples/` directory
5. **Use batch processing** for running multiple experiments efficiently
6. **Export and analyze** your brain data with the enhanced utilities

That's it! You're now ready to use NLM with the enhanced Phase 6 functionality. Remember, this is Phase 6 - the final integration of all neural systems working together as a complete artificial brain!

Happy experimenting! 🚀