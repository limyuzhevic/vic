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
3. Read `docs/PYTHON_ADVANCED.md` for advanced usage patterns
4. Experiment with different configurations!

That's it! You're now ready to use NLM.

## Power User Features

Once you master the basics, explore these advanced capabilities:

### 1. Context Manager for Automatic Reset
```python
import pynlm

# Use brain with automatic reset
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    brain.step(0)
    print("Brain active within context")
# Brain automatically resets when exiting context
```

### 2. Advanced Configuration Presets
```python
import pynlm

# Create specialized configurations
def create_fast_config():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    return config

def create_research_config():
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.2)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    return config

# Use specialized configs
brain = pynlm.createBrain(create_fast_config())
```

### 3. Performance Monitoring
```python
import pynlm
import time
import statistics

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Track performance
firing_rates = []
memory_usage = []

for step in range(1000):
    brain.step(step)
    
    # Collect statistics
    firing_rates.append(brain.getFiringNeuronCount())
    memory_usage.append(brain.getTotalNeuronCount())
    
    # Log performance every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

# Analyze performance
print(f"Performance summary:")
print(f"  Max firing rate: {max(firing_rates)}")
print(f"  Average firing rate: {statistics.mean(firing_rates):.2f}")
print(f"  Firing rate std: {statistics.stdev(firing_rates):.2f}")
```

### 4. Advanced World Interaction
```python
import pynlm

# Create complex world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

# Add various world objects
resources = []
for i in range(20):
    x = random.uniform(5, 25)
    y = random.uniform(5, 25)
    obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=random.uniform(1, 10))
    world.addObject(obj)
    resources.append(obj)

# Add hazards
hazards = []
for i in range(5):
    x = random.uniform(10, 20)
    y = random.uniform(10, 20)
    obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=1.0)
    world.addObject(obj)
    hazards.append(obj)

# Setup agent with world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world.reset()
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Complex simulation loop
for step in range(500):
    world.update(0.1)
    
    # Get full percept
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Get brain action
    action = agent.decodeMotorCommand()
    
    # Check for special interactions
    if action.getType() == pynlm.ActionType.Interact:
        # Check nearby objects
        x, y = agent.getBrain().getAgentBody().x, agent.getBrain().getAgentBody().y
        nearby_objects = [obj for obj in resources 
                         if abs(obj.x - x) < 1.0 and abs(obj.y - y) < 1.0]
        if nearby_objects:
            # Collect resource
            obj = nearby_objects[0]
            world.removeObject(obj.x, obj.y)
            resources.remove(obj)
            
            # Apply positive reward
            agent.applyRewardModulation(1.0, 0.5)
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 5. Brain State Persistence and Recovery
```python
import pynlm
import time
import json
import os

class BrainSession:
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.session_file = "nlm_session.json"
    
    def create_session(self):
        """Create new brain session"""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        return self
    
    def save_session(self, filename=None):
        """Save current session state"""
        filename = filename or self.session_file
        
        session_data = {
            "step": self.step,
            "brain_stats": {
                "neuron_count": self.brain.getTotalNeuronCount(),
                "firing_count": self.brain.getFiringNeuronCount(),
                "avg_firing_rate": self.brain.getAverageFiringRate(),
                "development_stage": self.brain.getDevelopmentalStage(),
            },
            "agent_stats": {
                "curiosity": self.agent.getCuriosityLevel(),
                "novelty": self.agent.getNoveltyLevel(),
                "neuromodulation": self.agent.getNeuromodulationLevel(),
            }
        }
        
        with open(filename, 'w') as f:
            json.dump(session_data, f, indent=2)
        
        # Also save brain state
        self.brain.save(filename.replace('.json', '.bin'))
        return session_data
    
    def load_session(self, filename=None):
        """Load saved session state"""
        filename = filename or self.session_file
        
        if not os.path.exists(filename):
            raise FileNotFoundError(f"Session file {filename} not found")
        
        with open(filename, 'r') as f:
            session_data = json.load(f)
        
        # Load brain state
        self.brain.load(filename.replace('.json', '.bin'))
        
        # Update session info
        self.step = session_data["step"]
        
        return session_data

# Usage example
session = BrainSession()
session.create_session()

# Run some simulation
for i in range(100):
    session.brain.step(i)

# Save progress
session.save_session("mid_simulation.json")

# Load later
saved_data = session.load_session("mid_simulation.json")
print(f"Loaded session from step {saved_data['step']}")
```

### 6. Advanced Python Features

#### Using Properties and Getters
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use properties with computed values
class BrainAnalyzer:
    def __init__(self, brain):
        self.brain = brain
    
    @property
    def firing_efficiency(self):
        """Calculate firing efficiency (firing rate / total neurons)"""
        return self.brain.getFiringNeuronCount() / self.brain.getTotalNeuronCount()
    
    @property
    def energy_usage(self):
        """Estimate energy usage based on firing activity"""
        return self.brain.getFiringNeuronCount() * 0.1  # Simplified model
    
    @property
    def plasticity_index(self):
        """Calculate plasticity index based on developmental stage"""
        stage = self.brain.getDevelopmentalStage()
        return {"Initial": 0.2, "CriticalPeriod": 1.0, "Maturation": 0.7, 
                "Adult": 0.3, "Aging": 0.1}[stage]
    
    def get_comprehensive_stats(self):
        """Get all statistics in one call"""
        return {
            "total_neurons": self.brain.getTotalNeuronCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "avg_firing_rate": self.brain.getAverageFiringRate(),
            "efficiency": self.firing_efficiency,
            "energy_usage": self.energy_usage,
            "plasticity": self.plasticity_index,
            "development_stage": self.brain.getDevelopmentalStage(),
        }

analyzer = BrainAnalyzer(brain)
stats = analyzer.get_comprehensive_stats()
print(f"Brain efficiency: {stats['efficiency']:.3f}")
```

#### Event-Driven Programming
```python
import pynlm

class BrainMonitor:
    def __init__(self, brain, callback):
        self.brain = brain
        self.callback = callback
        self.threshold_firing = brain.getTotalNeuronCount() * 0.1
        self.threshold_spikes = brain.getTotalSpikeCount() * 1.5
        self.last_check = 0
    
    def check_conditions(self, step):
        """Check for interesting conditions and trigger callbacks"""
        firing_count = self.brain.getFiringNeuronCount()
        total_spikes = self.brain.getTotalSpikeCount()
        
        # Check for high firing activity
        if firing_count > self.threshold_firing:
            self.callback("high_activity", {
                "step": step,
                "firing_count": firing_count,
                "firing_rate": firing_count / self.brain.getTotalNeuronCount()
            })
        
        # Check for milestone spike count
        if total_spikes > self.threshold_spikes:
            self.callback("milestone_spikes", {
                "step": step,
                "total_spikes": total_spikes,
                "milestone": int(total_spikes / 1000000) * 1000000
            })
    
    def run_monitor(self, num_steps):
        """Run the brain with monitoring"""
        for step in range(num_steps):
            self.brain.step(step)
            self.check_conditions(step)

# Event callback
last_stats = {}

def on_event(event_type, data):
    last_stats[event_type] = data
    print(f"Event: {event_type} at step {data['step']}")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
monitor = BrainMonitor(brain, on_event)
monitor.run_monitor(1000)
```

## Advanced API Reference

### Module-level Utilities

```python
# System information
pynlm.getVersion()                    # Return version string
pynlm.getSystemInfo()                # Return system info dict

# Configuration presets  
pynlm.createFastConfig()             # Quick simulation config
pynlm.createResearchConfig()         # High-fidelity config
pynlm.createDefaultConfig()          # Standard config

# Factory functions
pynlm.createBrain(config)            # Create brain with config
pynlm.createAgentBrain(brain)        # Create agent interface
pynlm.createSimpleWorld()            # Create world environment

# Constants
pynlm.VERSION                        # Python bindings version
pynlm.PROJECT_NAME                   # Project name
pynlm.INVALID_NEURON_ID              # Invalid neuron ID constant
pynlm.NeuronType.Excitatory          # Neuron type constants
# ... and many more
```

### Brain Context Management

```python
# Automatic reset context
brain = pynlm.createBrain(config)
brain.initialize()

with brain:  # Use as context manager
    brain.step(0)  # Automatic reset after
    print("Brain was active")
# Brain automatically reset
```

### Advanced Configuration Access

```python
config = pynlm.createDefaultConfig()

# Safe access with type checking
def get_config_value(key, default=None, type=float):
    try:
        value = config.get(key)
        return value if value is not None else default
    except:
        return default

# Batch configuration
def apply_config_preset(name):
    presets = {
        "fast": {"brain.neuron_count": 100, "brain.synapse_density": 0.05},
        "balanced": {"brain.neuron_count": 1000, "brain.synapse_density": 0.1},
        "detailed": {"brain.neuron_count": 5000, "brain.synapse_density": 0.2}
    }
    
    if name in presets:
        for key, value in presets[name].items():
            config.set(key, value)
    
    return config
```

## Quick Reference for Advanced Users

| Feature | Function | Example |
|---------|----------|---------|
| **Brain Context** | Automatic reset | `with brain:` |
| **Performance** | Statistics collection | `brain.getTotalSpikeCount()` |
| **Configuration** | Presets and templates | `createResearchConfig()` |
| **World Objects** | Complex environments | `addObject(world_obj)` |
| **Agent Learning** | Advanced neuromodulation | `applyRewardModulation()` |
| **State Saving** | Session persistence | `save_session()` |
| **Event Monitoring** | Callback system | `BrainMonitor()` |

---

## Final Tips

1. **Start simple**: Use `easy_usage.md` for getting started
2. **Build gradually**: Add one advanced feature at a time
3. **Profile first**: Use the monitoring tools to identify bottlenecks
4. **Save progress**: Use session management for long simulations
5. **Document everything**: Keep notes on configuration experiments

NLM's Python bindings provide a powerful interface for neural simulation research. With the advanced features above, you can build complex, interactive brain models that learn and adapt in real-time.

**Happy simulating! 🤖**
