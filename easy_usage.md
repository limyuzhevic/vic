# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**NEW: Enhanced AgentBrain Interface**

This guide has been updated with NLM's latest improvements:
- **AgentBrain Performance Optimization**: Built-in speed vs. accuracy trade-offs
- **Advanced Configuration Presets**: Pre-configured behavior modes
- **Detailed Performance Monitoring**: Track every aspect of agent performance
- **Improved Error Handling**: Better debugging and error messages

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

---

## Advanced Agent Configuration

### Configuration Presets

NLM's AgentBrain now offers pre-configured behavior modes:

```python
import pynlm

# Create agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Goal-oriented behavior (balanced learning)
agent.configureForGoalOrientedBehavior()

# Exploration mode (curiosity-focused)
# agent.configureForExploration()

# Exploitation mode (stable performance)
# agent.configureForExploitation()
```

### Performance Optimization

Choose the right optimization strategy for your use case:

```python
# Optimize for speed (faster simulation)
agent.optimizeForSpeed()

# Optimize for accuracy (precise results)
# agent.optimizeForAccuracy()
```

### Performance Monitoring

Track your agent's performance in real-time:

```python
# Get basic statistics
print("Steps processed:", agent.getStepCount())
print("Sensory inputs:", agent.getInputCount())
print("Motor commands:", agent.getOutputCount())

# Get detailed metrics
metrics = agent.getPerformanceMetrics()
print("Dopamine level:", metrics["dopamine_level"])
print("Curiosity level:", metrics["curiosity_level"])
print("Average sensory response:", metrics["average_sensory_response"])
print("Development stage:", metrics["developmental_stage"])

# Log all metrics
agent.logStatus()
```

---

## Advanced Usage Patterns

### Pattern 4: Monitoring and Debugging

```python
def run_simulation_with_monitoring(brain, world, agent, num_steps):
    print("Starting simulation with monitoring...")
    
    # Configure agent for accuracy
    agent.optimizeForAccuracy()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation (learning)
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Log progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Dopamine level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Action confidence: {agent.getMotorConfidence(action):.3f}")
    
    # Log final performance metrics
    print("\n=== Final Performance Metrics ===")
    metrics = agent.getPerformanceMetrics()
    for key, value in metrics.items():
        if key.startswith("motor_confidence_"):
            print(f"  {key}: {value:.3f}")
        elif key in ["dopamine_level", "curiosity_level", "average_sensory_response"]:
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
```

### Pattern 5: Development Tracking

```python
def track_development_over_time(brain, world, agent, total_steps=1000):
    print("Tracking agent development over time...")
    
    # Enable development system
    agent.enableDevelopment(True)
    
    # Store developmental milestones
    developmental_stages = []
    curiosity_levels = []
    firing_rates = []
    
    for step in range(total_steps):
        # Update world and agent
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Record developmental stage
        developmental_stages.append(agent.getDevelopmentalStage())
        curiosity_levels.append(agent.getCuriosityLevel())
        firing_rates.append(brain.getFiringNeuronCount())
        
        # Log every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Stage: {agent.getDevelopmentalStage()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Analyze development progression
    print("\n=== Development Analysis ===")
    print(f"Initial development stage: {developmental_stages[0]}")
    print(f"Final development stage: {developmental_stages[-1]}")
    print(f"Peak curiosity level: {max(curiosity_levels):.3f}")
    print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.1f}")
```

### Pattern 6: Custom Configuration

```python
def create_custom_agent(config_file=None, enable_features=None):
    import pynlm
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    if config_file:
        # Load custom configuration
        config.loadFromFile(config_file)
    else:
        # Set default values
        config.set("neuron_count", 1000)
        config.set("region_count", 4)
        config.set("connection_probability", 0.1)
        config.set("stdp_ltp_weight", 0.01)
        config.set("stdp_ltd_weight", 0.012)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable requested features
    if enable_features is None:
        enable_features = []
    
    if "reward_modulation" in enable_features:
        agent.enableRewardModulation(True)
    
    if "structural_plasticity" in enable_features:
        agent.enableStructuralPlasticity(True)
    
    if "development" in enable_features:
        agent.enableDevelopment(True)
    
    if "curiosity" in enable_features:
        agent.enableCuriosity(True)
    
    # Configure for specific behavior
    if "exploration" in enable_features:
        agent.configureForExploration()
    elif "exploitation" in enable_features:
        agent.configureForExploitation()
    else:
        agent.configureForGoalOrientedBehavior()
    
    return brain, agent
```

---

## Power User Examples

### Example 4: Multi-Agent Simulation

```python
import pynlm
import threading
import time

def run_agent_simulation(agent_id, brain_config, shared_world):
    print(f"Starting Agent {agent_id}")
    
    # Create agent
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(shared_world)
    
    # Configure for exploration (agents compete)
    agent.configureForExploration()
    agent.optimizeForSpeed()
    
    # Run agent loop
    for step in range(500):
        shared_world.update(0.1)
        
        percept = shared_world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        result = shared_world.applyMotorCommand(action, shared_world.getSimulationTime())
        
        agent.applyRewardModulation(result.reward, 0.0)
        
        if step % 100 == 0:
            print(f"Agent {agent_id} Step {step}: {agent.getCuriosityLevel():.3f} curiosity")
    
    return agent.getPerformanceMetrics()

# Create shared world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
world.reset()

# Create different configurations for different agents
configs = []
for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500 + i * 100)
    configs.append(config)

# Run multiple agents concurrently
agents = []
threads = []

for i, config in enumerate(configs):
    thread = threading.Thread(
        target=lambda i=i, config=config: agents.append(run_agent_simulation(i, config, world))
    )
    threads.append(thread)
    thread.start()
    time.sleep(0.1)  # Stagger agent starts

# Wait for all agents to complete
for thread in threads:
    thread.join()

print("All agent simulations completed!")
for i, agent in enumerate(agents):
    print(f"Agent {i} total steps: {agent['steps_processed']}")
```

### Example 5: Real-time Performance Monitoring

```python
import pynlm
import time

class PerformanceMonitor:
    def __init__(self, agent):
        self.agent = agent
        self.metrics = {
            'start_time': time.time(),
            'steps': [],
            'curiosity': [],
            'firing_rates': [],
            'dopamine_levels': []
        }
    
    def record_step(self, step, brain):
        self.metrics['steps'].append(step)
        self.metrics['curiosity'].append(self.agent.getCuriosityLevel())
        self.metrics['firing_rates'].append(brain.getAverageFiringRate())
        self.metrics['dopamine_levels'].append(self.agent.getNeuromodulationLevel())
    
    def generate_report(self):
        import statistics
        
        report = {
            'total_steps': len(self.metrics['steps']),
            'elapsed_time': time.time() - self.metrics['start_time'],
            'steps_per_second': len(self.metrics['steps']) / 
                             (time.time() - self.metrics['start_time']),
            'curiosity_stats': {
                'mean': statistics.mean(self.metrics['curiosity']),
                'min': min(self.metrics['curiosity']),
                'max': max(self.metrics['curiosity']),
                'std': statistics.stdev(self.metrics['curiosity']) if len(self.metrics['curiosity']) > 1 else 0
            },
            'firing_rate_stats': {
                'mean': statistics.mean(self.metrics['firing_rates']),
                'min': min(self.metrics['firing_rates']),
                'max': max(self.metrics['firing_rates'])
            },
            'dopamine_stats': {
                'mean': statistics.mean(self.metrics['dopamine_levels']),
                'min': min(self.metrics['dopamine_levels']),
                'max': max(self.metrics['dopamine_levels'])
            }
        }
        
        return report

def run_monitored_simulation(brain, world, agent, num_steps):
    monitor = PerformanceMonitor(agent)
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Record metrics
        monitor.record_step(step, brain)
        
        # Log every 100 steps
        if step % 100 == 0:
            report = monitor.generate_report()
            print(f"Step {step} - Performance: {report['steps_per_second']:.1f} steps/sec")
    
    # Generate final report
    final_report = monitor.generate_report()
    print("\n=== Final Performance Report ===")
    print(f"Total simulation time: {final_report['elapsed_time']:.1f} seconds")
    print(f"Performance: {final_report['steps_per_second']:.1f} steps/second")
    print(f"Average curiosity level: {final_report['curiosity_stats']['mean']:.3f}")
    print(f"Curiosity range: {final_report['curiosity_stats']['min']:.3f} - {final_report['curiosity_stats']['max']:.3f}")
    print(f"Average firing rate: {final_report['firing_rate_stats']['mean']:.1f} Hz")
    
    return final_report
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

**NEW: Advanced Agent Methods**

| Advanced Feature | Method |
|-----------------|--------|
| Configure for behavior | `agent.configureForGoalOrientedBehavior()` |
| Optimize performance | `agent.optimizeForSpeed()` / `agent.optimizeForAccuracy()` |
| Get performance metrics | `agent.getPerformanceMetrics()` |
| Monitor progress | `agent.logStatus()` |
| Track statistics | `agent.getStepCount()`, `agent.getInputCount()`, `agent.getOutputCount()` |

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

**"Performance is slow"**
- Try `agent.optimizeForSpeed()`
- Check if you need all features enabled

**"Metrics are not updating"**
- Make sure you're calling `agent.initialize()` before recording metrics
- Verify that `agent.processSensoryInput()` is being called

---

## Next Steps

When you're comfortable:

1. **Explore HOW_TO_USE.md** for more detailed technical information
2. **Read docs/ARCHITECTURE.md** to understand how all the systems integrate
3. **Experiment with different configurations** and optimization strategies
4. **Try the power user examples** to see what's possible with the advanced features
5. **Create your own performance monitoring systems** using the new metrics API

**NEW: What's Possible with NLM v2**

- **Multi-agent simulations** with shared environments
- **Real-time performance monitoring** and analysis
- **Custom behavior configurations** for specific use cases
- **Advanced optimization strategies** for speed vs. accuracy trade-offs
- **Detailed statistical analysis** of agent behavior over time

That's it! You're now ready to use NLM with all its advanced features and optimizations. The AgentBrain class provides unprecedented control over neural simulation performance and behavior, making it suitable for everything from simple demonstrations to complex research applications.
