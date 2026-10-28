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

## Advanced Usage Examples

### Example 1: Custom Brain Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("brain.v_thresh", -55.0)
config.set("brain.v_rest", -65.0)
config.set("brain.v_reset", -70.0)

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Custom brain created: {brain.getTotalNeuronCount()} neurons")
print(f"STDP learning rate: {config.getOr('plasticity.stdp.learning_rate', 0.001)}")
```

### Example 2: Advanced Agent with Multiple Sensory Modalities

```python
import pynlm
import time

def run_advanced_agent_simulation(num_steps=2000):
    """Run a sophisticated agent with multiple learning systems."""
    
    # 1. Create optimized configuration
    config = pynlm.createDefaultConfig()
    
    # Performance optimizations
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.08)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, 
        height=30, 
        visionWidth=12, 
        visionHeight=12,
        visionConeAngle=90
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure development schedule
    agent.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
    
    # 2. Main simulation loop with logging
    for step in range(num_steps):
        # Update world with variable timestep
        timestep = 0.1 + (step % 10) * 0.01  # Variable timestep
        world.update(timestep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = calculate_reward(percept)  # Your reward function
        prediction_error = agent.getPredictionError()
        agent.applyRewardModulation(reward, prediction_error)
        
        # Update development
        agent.updateDevelopment(timestep)
        
        # Advanced logging
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {prediction_error:.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage()}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
            print(f"  Agent position: {world.getAgentBody().getPosition()}")
    
    return brain, agent, world

def calculate_reward(percept):
    """Calculate reward based on agent's goals."""
    # Example reward function
    internal = percept.getInternal()
    if internal and len(internal) > 0:
        return internal[0]  # First internal signal as reward
    return 0.0
```

### Example 3: Batch Processing Multiple Brains

```python
import pynlm
from concurrent.futures import ThreadPoolExecutor
import threading

def create_and_test_brain(brain_id, config_params):
    """Create and test a single brain configuration."""
    print(f"Creating brain #{brain_id}")
    
    config = pynlm.createDefaultConfig()
    
    # Apply configuration parameters
    for key, value in config_params.items():
        config.set(key, value)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Return results
    return {
        'brain_id': brain_id,
        'neuron_count': brain.getTotalNeuronCount(),
        'firing_count': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'spike_count': brain.getTotalSpikeCount()
    }

def batch_process_brains(brain_configs):
    """Process multiple brain configurations in parallel."""
    results = []
    
    # Use thread pool for parallel processing
    with ThreadPoolExecutor(max_workers=4) as executor:
        # Submit all brain creation tasks
        futures = []
        for i, config_params in enumerate(brain_configs):
            future = executor.submit(create_and_test_brain, i, config_params)
            futures.append(future)
        
        # Collect results
        for future in futures:
            try:
                result = future.result(timeout=30)
                results.append(result)
            except Exception as e:
                print(f"Error processing brain: {e}")
                results.append({'error': str(e)})
    
    return results

# Example batch processing
if __name__ == "__main__":
    # Define different brain configurations to test
    brain_configurations = [
        # Configuration 1: Small, fast brain
        {
            'brain.neuron_count': 500,
            'brain.synapse_density': 0.05,
            'plasticity.stdp.learning_rate': 0.001
        },
        # Configuration 2: Medium, balanced brain
        {
            'brain.neuron_count': 1000,
            'brain.synapse_density': 0.08,
            'plasticity.stdp.learning_rate': 0.005
        },
        # Configuration 3: Large, complex brain
        {
            'brain.neuron_count': 2000,
            'brain.synapse_density': 0.1,
            'plasticity.stdp.learning_rate': 0.01
        }
    ]
    
    # Run batch processing
    results = batch_process_brains(brain_configurations)
    
    # Display results
    print("\nBatch Processing Results:")
    print("=" * 50)
    for result in results:
        if 'error' in result:
            print(f"Brain #{result['brain_id']}: ERROR - {result['error']}")
        else:
            print(f"Brain #{result['brain_id']}:")
            print(f"  Neurons: {result['neuron_count']}")
            print(f"  Firing: {result['firing_count']}")
            print(f"  Avg firing rate: {result['average_firing_rate']:.3f}")
            print(f"  Total spikes: {result['spike_count']}")
```

### Example 4: Error Handling and Robust Simulation

```python
import pynlm
import logging
import traceback

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_simulation.log'),
        logging.StreamHandler()
    ]
)

def safe_simulation_run(config, max_steps=1000, checkpoint_file=None):
    """Run simulation with error handling and checkpointing."""
    
    brain = None
    try:
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Simulation started with {brain.getTotalNeuronCount()} neurons")
        
        # Run simulation steps
        for step in range(max_steps):
            try:
                # Run brain step
                brain.step(step)
                
                # Periodic checkpoint
                if step % 100 == 0 and checkpoint_file:
                    if brain.save(checkpoint_file + f"_step{step}.bin"):
                        print(f"Checkpoint saved at step {step}")
                    else:
                        print(f"Warning: Failed to save checkpoint at step {step}")
                
                # Log progress
                if step % 200 == 0:
                    logging.info(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
                
            except Exception as e:
                logging.error(f"Error at step {step}: {e}")
                break
        
        # Final checkpoint
        if checkpoint_file and brain:
            if brain.save(checkpoint_file + "_final.bin"):
                print("Final checkpoint saved")
        
        print("Simulation completed successfully")
        return brain
        
    except Exception as e:
        logging.error(f"Simulation failed: {e}")
        logging.error(traceback.format_exc())
        
        # Try to load checkpoint if available
        if checkpoint_file and brain:
            try:
                if brain.load(checkpoint_file + "_final.bin"):
                    print("Successfully loaded from checkpoint")
                    return brain
            except Exception as load_error:
                logging.error(f"Failed to load checkpoint: {load_error}")
        
        return None

# Example usage with error handling
if __name__ == "__main__":
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # Run simulation with checkpointing
    brain = safe_simulation_run(
        config,
        max_steps=1000,
        checkpoint_file="simulation_checkpoint"
    )
    
    if brain:
        print(f"Simulation successful! Final firing rate: {brain.getAverageFiringRate():.3f}")
    else:
        print("Simulation failed, try again with different configuration")
```

### Example 5: Performance Monitoring and Optimization

```python
import pynlm
import time
import threading
from collections import deque

class SimulationMonitor:
    """Monitor simulation performance and memory usage."""
    
    def __init__(self):
        self.step_times = deque(maxlen=100)
        self.memory_usage = deque(maxlen=100)
        self.start_time = time.time()
        self.is_running = False
        self.monitor_thread = None
    
    def start_monitoring(self):
        """Start monitoring thread."""
        self.is_running = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop monitoring thread."""
        self.is_running = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _monitor_loop(self):
        """Monitoring loop running in separate thread."""
        while self.is_running:
            current_step_time = time.time()
            # Get approximate memory usage (platform-specific)
            # This is a placeholder - implement platform-specific memory tracking
            self.memory_usage.append(0)  # Placeholder
            self.step_times.append(current_step_time - self.start_time)
            time.sleep(0.1)
    
    def get_performance_stats(self):
        """Get performance statistics."""
        if not self.step_times:
            return {}
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        steps_per_second = len(self.step_times) / (self.step_times[-1] - self.step_times[0] if len(self.step_times) > 1 else 1)
        
        return {
            'total_steps': len(self.step_times),
            'average_step_time': avg_step_time,
            'steps_per_second': steps_per_second,
            'total_time': time.time() - self.start_time
        }

def run_monitored_simulation(brain, num_steps=1000):
    """Run simulation with performance monitoring."""
    
    monitor = SimulationMonitor()
    monitor.start_monitoring()
    
    start_time = time.time()
    
    try:
        for step in range(num_steps):
            step_start = time.time()
            
            # Run brain step
            brain.step(step)
            
            step_time = time.time() - step_start
            if step % 100 == 0:
                print(f"Step {step}: {step_time:.4f}s")
        
        end_time = time.time()
        
        # Get monitoring statistics
        monitor.stop_monitoring()
        stats = monitor.get_performance_stats()
        
        print(f"\nSimulation Performance:")
        print(f"  Total time: {stats['total_time']:.2f}s")
        print(f"  Average step time: {stats['average_step_time']:.4f}s")
        print(f"  Steps per second: {stats['steps_per_second']:.1f}")
        print(f"  Actual steps completed: {stats['total_steps']}")
        
        return brain, stats
        
    except Exception as e:
        monitor.stop_monitoring()
        print(f"Simulation failed: {e}")
        return None, None

# Example usage
if __name__ == "__main__":
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run monitored simulation
    result_brain, performance_stats = run_monitored_simulation(brain, num_steps=500)
    
    if result_brain and performance_stats:
        print(f"\nFinal brain statistics:")
        print(f"  Total spikes: {result_brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {result_brain.getAverageFiringRate():.3f}")
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

That's it! You're now ready to use NLM.
