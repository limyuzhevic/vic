# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

---

## Quick Start (What You Can Do)

This guide shows you how to use NLM to create virtual brains that can learn, remember, and make decisions.

### The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in  
3. **Agent** - The bridge connecting brain to world

### Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

### One Line to Get Started

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## What Makes NLM Special

### Connected Systems Architecture
NLM implements a closed-loop brain architecture following this flow:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE → REWARD/SURPRISE/ERROR → PLASTICITY → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

### Key Features

- **Emerging Cognition**: Learn attention, planning, memory, and self-model from experience
- **Neuromodulation Integration**: Reward, curiosity, novelty, and prediction error signals
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Development**: Stage-dependent plasticity and brain maturation
- **Prediction System**: Temporal sequence learning and action consequence prediction
- **Full Integration**: All systems work together in Phase 6 (Final Integration)

---

## Getting Started

### Method 1: Quick Demo (Fastest)

```bash
# Run the Phase 6 integration demo
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Method 2: Python Quick Start

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 steps of thinking
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Method 3: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")

# Run the simulation
run_agent_simulation(1000)
```

---

## Common Tasks

### Task 1: Run a Silent Brain Test

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Task 2: Watch a Brain in a World

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

### Task 3: Complete Agent with Learning

```python
import pynlm

# Setup with learning
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
agent.enableDevelopment(True)

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

## Key Classes and Methods

### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization  
brain.initialize()

# Simulation
brain.step(step_number)                    # Run one simulation step
brain.reset()                              # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()      # Currently firing
brain.getAverageFiringRate()       # Average firing rate
brain.getDevelopmentalStage()     # Current developmental stage

# State management
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
```

### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableCuriosity(True/False)
agent.enableDevelopment(True/False)
```

### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

---

## Advanced Usage Patterns

### Pattern 1: Run Multiple Episodes

```python
def run_episode(brain, world, agent, num_steps, episode_id):
    """Run a single simulation episode."""
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Episode {episode_id} completed")
    return brain.getAverageFiringRate()

# Run multiple episodes
for episode in range(5):
    rate = run_episode(brain, world, agent, 200, episode)
    print(f"Episode {episode} avg firing rate: {rate:.3f}")
```

### Pattern 2: Brain State Management

```python
def save_and_reload_brain(brain, filepath, steps_per_save):
    """Save brain state periodically and reload it."""
    for step in range(steps_per_save):
        brain.step(step)
        
        if step % 100 == 0:
            brain.save(f"{filepath}_step{step}.bin")
    
    # Load from middle of saved sequence
    brain.load(f"{filepath}_step50.bin")
    print(f"Loaded brain from step 50, firing neurons: {brain.getFiringNeuronCount()}")
```

### Pattern 3: Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify specific parameters
config.set("brain.neuron_count", 5000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### "Memory systems aren't working"
- Check that reward modulation is enabled: `agent.enableRewardModulation(True)`
- Verify that curiosity is enabled for exploration: `agent.enableCuriosity(True)`

### "Development isn't progressing"
- Enable development: `agent.enableDevelopment(True)`
- Run enough steps for development to occur

---

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md` for more details** - Comprehensive technical documentation
2. **Read `docs/ARCHITECTURE.md`** - System architecture overview
3. **Read `docs/SCIENCE.md`** - Scientific background and principles
4. **Experiment with different configurations** - Try different parameters and see what happens
5. **Check `docs/EXPERIMENTS.md`** - Real experiment examples and results

### Learning Resources

- **Beginner**: Start with `easy_usage.md` and the quick start examples
- **Intermediate**: Work through `HOW_TO_USE.md` and experiment with the complete agent example
- **Advanced**: Read `ARCHITECTURE.md` and `SCIENCE.md` for deep understanding

---

## Performance Tips

1. **Increase timestep** for faster simulations (but may affect accuracy)
2. **Use larger brain configurations** for more realistic behavior
3. **Enable learning subsystems** for adaptive behavior
4. **Monitor firing rates** to ensure neurons are active
5. **Use checkpointing** for long-running simulations

## Performance Monitoring

NLM now includes comprehensive performance monitoring capabilities:

### 1. Performance Monitoring
- **Performance Monitor**: Tracks runtime metrics, CPU usage, memory usage, and system performance
- **Real-time Metrics**: Collects and analyzes performance data during simulation
- **Performance Metrics**: Tracks simulation steps, firing rates, spikes, and resource utilization

### 2. Profiling
- **Performance Profiler**: Profiles function execution times and identifies bottlenecks
- **RAII Profiling**: Easy-to-use scope-based profiling with minimal overhead
- **Detailed Timing**: Records timing for specific functions and code sections

### 3. Performance Analysis
- **Performance Analyzer**: Provides statistical analysis of performance data
- **Anomaly Detection**: Identifies performance deviations and outliers
- **Moving Averages**: Smooths performance data for trend analysis

### 4. Performance Alerting
- **Performance Alerting**: Monitors for performance threshold violations
- **Custom Conditions**: Define alerts for CPU, memory, spike rate, and simulation speed
- **Multiple Severities**: Different alert levels for different severity conditions
- **Callback System**: Integrate with external monitoring systems

### 5. Performance Benchmarking
- **Benchmark Suite**: Comprehensive benchmarking of neural network operations
- **Test Categories**: Neural network creation, simulation steps, memory access, spike processing, multithreading
- **Performance Comparison**: Compare baseline vs. optimized implementations
- **Report Generation**: Generate detailed performance reports in multiple formats

### 6. Performance Visualization
- **Export to File**: Save metrics, profiles, and alerts to files for analysis
- **JSON Format**: Export performance data in standard JSON format for external tools
- **Detailed Reports**: Generate comprehensive performance reports

### Example Usage

```python
import pynlm

# Create performance monitor and profiler
monitor = pynlm.create_performance_monitor()
profiler = pynlm.create_performance_profiler()

# Configure monitoring
config = pynlm.create_default_performance_config()
config.enable_monitoring = True
config.enable_profiling = True
config.enable_alerting = True
monitor.configure(config)

# Create performance benchmark
benchmark = pynlm.create_neural_simulation_benchmark()

# Run simulation with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    with profiler.make_scope("brain_step", "main.py", step):
        brain.step(step)
    
    # Performance monitoring will automatically collect metrics
    # and profile data

# Analyze performance results
avg_firing_rate = monitor.get_average_firing_rate()
history = monitor.get_metrics_history()
benchmark_result = benchmark.run_benchmark("NeuralNetworkCreation")

# Save performance data
monitor.save_metrics_to_file("metrics.json")
monitor.save_profile_to_file("profile.json")
```

### Performance Benefits

The performance monitoring system provides several benefits:

1. **Identifies Bottlenecks**: Find slow functions and optimize performance
2. **Resource Management**: Monitor memory usage and prevent resource exhaustion
3. **Performance Validation**: Ensure simulations meet performance targets
4. **Debugging Support**: Profile and debug performance issues in production
5. **Optimization Guidance**: Compare performance across different implementations

### Configuration Options

The performance monitoring system can be configured with various options:

- **Sampling Interval**: How frequently to collect metrics
- **Profile Sample Rate**: How often to sample profile data
- **Maximum History**: How many historical data points to retain
- **Alert Conditions**: Define custom alert thresholds
- **Output Files**: Where to save performance data

The performance monitoring system is designed to be lightweight with minimal overhead, making it suitable for production use without significantly impacting simulation performance.

---

## About NLM

NLM (熙然, "serene flow") is a research project investigating computational brain-like systems. It is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM is a **brain simulator** that creates virtual brains with neurons and synapses that can learn from experience. All learning comes from interaction with the environment, not from pretrained knowledge.

**License**: MIT

**Authors**: Research project — See docs for scientific background.