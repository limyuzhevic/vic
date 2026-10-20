# NLM Progressive Learning Examples

Comprehensive progressive learning examples for NLM (Neural Learning Machine). This guide provides a structured learning path from basic concepts to advanced applications, with clear progression and skill development.

## Table of Contents

1. [Learning Progression Overview](#learning-progression-overview)
2. [Example 1: Silent Brain (Hello World)](#example-1-silent-brain-hello-world)
3. [Example 2: Brain Watching a World](#example-2-brain-watching-a-world)
4. [Example 3: Complete Agent](#example-3-complete-agent)
5. [Example 4: Learning Agent](#example-4-learning-agent)
6. [Example 5: Advanced Learning](#example-5-advanced-learning)
7. [Example 6: Research Experiment](#example-6-research-experiment)
8. [Example 7: Performance Optimization](#example-7-performance-optimization)
9. [Example 8: Custom Configuration](#example-8-custom-configuration)
10. [Example 9: Multi-Agent System](#example-9-multi-agent-system)
11. [Example 10: Real-World Application](#example-10-real-world-application)
12. [Learning Assessment](#learning-assessment)

## Learning Progression Overview

The NLM learning progression is designed to build skills progressively, starting with basic concepts and moving to advanced applications. Each example builds on the previous ones, ensuring solid foundation before introducing new concepts.

### Learning Levels

#### Level 1: Foundation Skills
- **Understanding Core Components**: Brain, world, agent
- **Basic Operations**: Create, initialize, run simulation
- **Simple Examples**: Silent brain, basic brain-world interaction
- **Code Concepts**: Simple Python scripts, basic error handling

#### Level 2: Intermediate Skills
- **Integration Understanding**: How components work together
- **Advanced Operations**: Configuration, optimization, analysis
- **Complex Examples**: Complete agent, learning systems
- **Code Concepts**: Structured code, error handling, documentation

#### Level 3: Advanced Skills
- **System Design**: Custom architectures, optimization strategies
- **Research Methods**: Experimental design, data analysis
- **Production Use**: Performance optimization, deployment
- **Code Concepts**: Advanced Python, modular design, testing

### Learning Objectives

By completing these examples, learners will:

1. **Conceptual Understanding**:
   - Understand NLM architecture and components
   - Explain neural computation and learning
   - Describe system interactions and workflows

2. **Practical Skills**:
   - Create and configure NLM systems
   - Run and analyze simulations
   - Debug and troubleshoot issues
   - Optimize performance and memory usage

3. **Research Capabilities**:
   - Design experimental protocols
   - Analyze and interpret results
   - Create reproducible research workflows
   - Document and share findings

## Example 1: Silent Brain (Hello World)

### Overview
The simplest NLM example that creates a brain and lets it "think" without any external input or output. This introduces basic NLM concepts and establishes the foundation for more complex examples.

### Learning Objectives
- Understand brain creation and initialization
- Learn basic brain operation and spike generation
- Practice Python-NLM integration
- Create first working NLM script

### Code Example
```python
import pynlm as nlm

print("=== Silent Brain Example ===")

# Create a simple brain configuration
config = nlm.createDefaultConfig()
print(f"Configuration created with default parameters")

# Create the brain
brain = nlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Initialize the brain
brain.initialize()
print("Brain initialized successfully")

# Let the brain "think" (process spikes)
print("Letting brain think for 10 steps...")
for step in range(10):
    brain.step(step)
    if step % 3 == 0:  # Print every 3 steps
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"\n=== Results ===")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")

print("\n🎉 Silent brain example completed!")
print("This demonstrates basic NLM brain operation.")
```

### Key Concepts

#### Brain Creation
```python
# Configuration
config = nlm.createDefaultConfig()  # Create default configuration

# Brain Creation
brain = nlm.createBrain(config)  # Create brain with configuration
brain.initialize()  # Initialize brain state
```

#### Brain Operation
```python
# Basic brain operations
brain.step(step_number)  # Process one simulation step
brain.getTotalNeuronCount()  # Get total neuron count
brain.getFiringNeuronCount()  # Get currently firing neurons
brain.getAverageFiringRate()  # Get average firing rate
brain.getTotalSpikeCount()  # Get total spike count
```

#### Brain Statistics
```python
# Brain statistics
stats = nlm.getBrainStats(brain)
print(f"Brain statistics: {stats}")
```

### Learning Activities

1. **Run the Example**: Execute the silent brain example
2. **Understand Output**: Interpret brain statistics and results
3. **Explore Brain State**: Try different configuration parameters
4. **Compare Configurations**: Test with different neuron counts
5. **Document Results**: Record findings and observations

### Expected Results

When running the example, you should see:

- **Console Output**: Brain creation, initialization, and operation messages
- **Statistics**: Total neurons, firing neurons, spikes, firing rate
- **Development Stage**: Current developmental stage of the brain
- **Success Message**: Completion confirmation

### Extensions and Variations

#### Custom Brain Size
```python
# Create larger brain
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
brain = nlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

#### Custom Configuration
```python
# Create custom configuration
config = nlm.createDefaultConfig()
config.set("brain.synapse_density", 0.05)
config.set("brain.learning_rate", 0.01)
config.set("plasticity.stdp.enable", True)

brain = nlm.createBrain(config)
brain.initialize()
```

#### Extended Simulation
```python
# Run longer simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Common Issues and Solutions

#### "Brain isn't doing anything"
**Solution:** Ensure brain is properly initialized
```python
# Check initialization
brain.initialize()  # Always call this before brain.step()
```

#### "Everything is 0"
**Solution:** Brains need time to warm up
```python
# Run more steps
for step in range(100):  # More steps
    brain.step(step)
```

## Example 2: Brain Watching a World

### Overview
An extended example showing how a brain interacts with a simple world. This example introduces world simulation, sensory processing, and basic agent behavior.

### Learning Objectives
- Understand world simulation and configuration
- Learn sensory input processing
- Practice agent initialization and operation
- Create brain-world integration

### Code Example
```python
import pynlm as nlm

print("=== Brain Watching World Example ===")

# 1. Create and configure brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")

# 2. Create world (environment)
world = nlm.createSimpleWorld()
world.configure(
    width=20,           # World is 20x20 units
    height=20,
    visionWidth=8,      # Agent sees 8x8 area at a time
    visionHeight=8
)
world.reset()
print(f"✓ World created ({world.getWidth()}x{world.getHeight()} units)")

# 3. Create agent (brain-world interface)
agent = nlm.createAgentBrain(brain)
agent.initialize(world)
print("✓ Agent brain interface created")

# 4. Enable learning (optional but recommended)
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore new things
print("✓ Learning systems enabled")

# 5. Run simulation
print("\nRunning simulation...")
for step in range(30):
    # Update world
    world.update(0.1)  # 0.1 seconds per step
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Let the brain think
    brain.step(step)
    
    # Get the brain's decision
    action = agent.decodeMotorCommand()
    
    # Do something in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    if step % 10 == 0:
        print(f"  Step {step}:")
        print(f"    🧠 {brain.getFiringNeuronCount()} neurons firing")
        print(f"    🎯 Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"    📍 Agent position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")

print(f"\n=== Simulation Complete ===")
print(f"📊 Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"🎯 Final curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"⚡ Total spikes generated: {brain.getTotalSpikeCount()}")

print("\n🎉 Brain watching world example completed!")
print("This demonstrates brain-world interaction and basic agent behavior.")
```

### Key Concepts

#### World Configuration
```python
# Create and configure world
world = nlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()  # Reset to initial state
```

#### Agent Interface
```python
# Create agent interface
agent = nlm.createAgentBrain(brain)
agent.initialize(world)  # Initialize with world reference

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

#### Simulation Loop
```python
# Main simulation loop
for step in range(num_steps):
    # Update environment
    world.update(timestep)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Motor output
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect data
    collect_simulation_data(step, percept, action)
```

#### Simulation Monitoring
```python
# Monitor simulation progress
print(f"Step {step}:")
print(f"  🧠 Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  🎯 Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  📍 Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
print(f"  📊 Firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

### Learning Activities

1. **Run the Example**: Execute the brain-world simulation
2. **Observe Behavior**: Track agent position and curiosity
3. **Analyze Performance**: Study brain activity and learning patterns
4. **Modify Parameters**: Try different world sizes and configurations
5. **Document Observations**: Record behavior patterns and changes

### Expected Results

When running the example, you should see:

- **Simulation Progress**: Step-by-step simulation output
- **Agent Movement**: Agent exploring and moving in the world
- **Learning Indicators**: Increasing curiosity and exploration
- **Brain Activity**: Neural firing and learning patterns
- **Final Statistics**: Summary of simulation results

### Extensions and Variations

#### Custom World Configuration
```python
# Create custom world
world = nlm.createSimpleWorld()
world.configure(
    width=50,  # Larger world
    height=50,
    visionWidth=15,  # Better vision
    visionHeight=15
)
world.reset()
```

#### Extended Simulation
```python
# Run longer simulation with more detailed output
for step in range(100):
    world.update(0.1)
    
    # Record detailed metrics
    metrics = {
        "step": step,
        "firing_rate": brain.getAverageFiringRate(),
        "curiosity": agent.getCuriosityLevel(),
        "position": (world.getAgentBody().x, world.getAgentBody().y),
        "time": world.getSimulationTime()
    }
    
    # Save metrics
    save_metrics(metrics)
    
    if step % 20 == 0:
        print(f"Progress: Step {step}, Position {metrics['position']}")
```

#### Performance Monitoring
```python
# Monitor performance
import time
import psutil

start_time = time.time()
for step in range(100):
    brain.step(step)

end_time = time.time()

elapsed = end_time - start_time
steps_per_second = 100 / elapsed

print(f"Performance: {steps_per_second:.1f} steps/second")
print(f"Memory usage: {psutil.Process().memory_info().rss / 1024 / 1024:.1f} MB")
```

### Common Issues and Solutions

#### "Agent isn't moving"
**Solution:** Check world update and action application
```python
# Ensure world is updating
world.update(0.1)

# Ensure agent is decoding actions correctly
action = agent.decodeMotorCommand()
if action is None:
    print("Warning: Agent didn't produce action")

# Apply action to world
world.applyMotorCommand(action, world.getSimulationTime())
```

#### "High memory usage"
**Solution:** Optimize configuration
```python
# Reduce network size
config.set("brain.neuron_count", 500)

# Disable memory-intensive features
config.set("plasticity.structural.enable", False)
config.set("neuromod.curiosity.enable", False)
```

## Example 3: Complete Agent

### Overview
A comprehensive example demonstrating all NLM systems working together. This example shows how to create a learning agent that explores, learns, and adapts to its environment.

### Learning Objectives
- Understand complete agent lifecycle
- Learn to enable and configure all NLM systems
- Practice advanced simulation techniques
- Create production-ready agent code

### Code Example
```python
import pynlm as nlm
import time

def run_complete_agent_simulation(num_steps=500):
    """Run a complete agent simulation with all systems enabled."""
    
    print("=== Complete Agent Simulation ===")
    print(f"Running {num_steps} simulation steps...")
    
    # 1. Create configuration with all systems
    config = nlm.createDefaultConfig()
    
    # Configure for balanced performance
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    config.set("neuromod.curiosity.exploration_weight", 0.7)
    config.set("neuromod.novelty.threshold", 0.3)
    config.set("development.initial_stage", "CriticalPeriod")
    
    # 2. Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # 3. Create world
    world = nlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)  # Start in center
    print(f"✓ World created ({world.getWidth()}x{world.getHeight()} units)")
    
    # 4. Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    print("✓ Agent interface created")
    
    # 5. Enable ALL learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Enable neuromodulators
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    if dopamine:
        dopamine.enable()
    if curiosity:
        curiosity.enable()
    if novelty:
        novelty.enable()
    
    print("✓ All learning systems enabled")
    
    # 6. Performance monitoring
    start_time = time.time()
    metrics = []
    
    # 7. Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        if dopamine:
            dopamine.signalReward(reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Update curiosity
        prediction_error = agent.getPredictionError()
        if curiosity and novelty:
            curiosity.update(agent.getCuriosityLevel(), prediction_error, 0.1)
        
        # Record metrics every 50 steps
        if step % 50 == 0:
            metric = {
                "step": step,
                "brain_firing": brain.getFiringNeuronCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "position": (world.getAgentBody().x, world.getAgentBody().y),
                "time": world.getSimulationTime()
            }
            metrics.append(metric)
            
            # Print progress
            print(f"  Step {step}:")
            print(f"    🧠 Firing: {metric['brain_firing']} neurons")
            print(f"    🎯 Curiosity: {metric['curiosity']:.3f}")
            print(f"    📍 Position: ({metric['position'][0]:.1f}, {metric['position'][1]:.1f})")
    
    # 8. Calculate performance metrics
    end_time = time.time()
    elapsed_time = end_time - start_time
    steps_per_second = num_steps / elapsed_time
    
    # Final statistics
    final_stats = {
        "total_steps": num_steps,
        "elapsed_time": elapsed_time,
        "steps_per_second": steps_per_second,
        "final_firing_rate": brain.getAverageFiringRate(),
        "final_curiosity": agent.getCuriosityLevel(),
        "total_spikes": brain.getTotalSpikeCount(),
        "agent_position": (world.getAgentBody().x, world.getAgentBody().y),
        "system_metrics": metrics
    }
    
    # 9. Print final results
    print(f"\n=== Simulation Complete ===")
    print(f"⏱️  Total time: {elapsed_time:.1f} seconds")
    print(f"🚀 Performance: {steps_per_second:.1f} steps/second")
    print(f"📊 Final firing rate: {final_stats['final_firing_rate']:.2f} Hz")
    print(f"🎯 Final curiosity: {final_stats['final_curiosity']:.3f}")
    print(f"⚡ Total spikes: {final_stats['total_spikes']}")
    print(f"📍 Final position: ({final_stats['agent_position'][0]:.1f}, {final_stats['agent_position'][1]:.1f})")
    
    # 10. Save brain state
    nlm.exportBrainState(brain, "simulation_checkpoint.brain")
    print(f"💾 Brain state saved: simulation_checkpoint.brain")
    
    return brain, world, agent, final_stats

if __name__ == "__main__":
    # Run the complete agent simulation
    brain, world, agent, stats = run_complete_agent_simulation(500)
    
    print(f"\n🎉 Complete agent simulation finished successfully!")
    print(f"The agent learned and explored for {stats['total_steps']} steps.")
```

### Key Concepts

#### Complete System Configuration
```python
# Configure all NLM systems
config = nlm.createDefaultConfig()

# Brain configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)

# Plasticity systems
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("plasticity.structural.enable", True)

# Neuromodulation systems
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.exploration_weight", 0.7)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.3)

# Development system
config.set("development.initial_stage", "CriticalPeriod")
```

#### Complete Agent Initialization
```python
# Enable all agent systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Enable neuromodulators
dopamine = brain.getDopamine()
if dopamine:
    dopamine.enable()

curiosity = brain.getCuriosity()
if curiosity:
    curiosity.enable()

novelty = brain.getNovelty()
if novelty:
    novelty.enable()
```

#### Comprehensive Simulation Loop
```python
# Complete simulation loop with all systems
for step in range(num_steps):
    # World update
    world.update(0.1)
    
    # Sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Neural processing
    brain.step(step)
    
    # Motor output
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Neuromodulation
    reward = calculate_reward(percept)
    if dopamine:
        dopamine.signalReward(reward)
    
    # Development
    agent.updateDevelopment(0.1)
    
    # Curiosity update
    prediction_error = agent.getPredictionError()
    if curiosity and novelty:
        curiosity.update(agent.getCuriosityLevel(), prediction_error, 0.1)
    
    # Metrics collection
    record_metrics(step, brain, agent, world)
```

#### Performance Monitoring
```python
# Performance monitoring
start_time = time.time()

for step in range(num_steps):
    brain.step(step)
    
    # Calculate steps per second
    elapsed = time.time() - start_time
    steps_per_second = (step + 1) / elapsed

# Memory monitoring
import psutil
memory_usage = psutil.Process().memory_info().rss / 1024 / 1024

# Print performance summary
print(f"Performance: {steps_per_second:.1f} steps/second")
print(f"Memory usage: {memory_usage:.1f} MB")
```

### Learning Activities

1. **Run the Complete Example**: Execute the full agent simulation
2. **Analyze Learning Curves**: Study curiosity and firing rate changes
3. **Performance Monitoring**: Track system performance throughout simulation
4. **Parameter Tuning**: Optimize configuration for different scenarios
5. **Document Results**: Record comprehensive simulation results

### Expected Results

When running the example, you should see:

- **Detailed Progress Output**: Step-by-step simulation with metrics
- **Learning Indicators**: Curiosity levels, firing rates, position tracking
- **Performance Statistics**: Steps per second, memory usage
- **Final Summary**: Comprehensive results and brain state save

### Extensions and Variations

#### Custom Performance Configuration
```python
# Optimize for specific performance targets
def create_performance_config(target_fps=10):
    config = nlm.createDefaultConfig()
    
    # Calculate neuron count based on target performance
    target_steps_per_second = target_fps
    estimated_neurons = int(target_steps_per_second * 100)
    
    config.set("brain.neuron_count", estimated_neurons)
    config.set("brain.synapse_density", 0.01)
    config.set("plasticity.stdp.learning_rate", 0.0005)
    config.set("simulation.timestep", 0.2)
    
    return config
```

#### Real-time Monitoring
```python
# Real-time monitoring during simulation
def monitor_simulation(brain, world, agent, interval=1.0):
    import time
    
    start_time = time.time()
    
    while time.time() - start_time < interval * 10:  # Run for 10 intervals
        # Collect metrics
        metrics = {
            "time": time.time() - start_time,
            "firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel(),
            "position": (world.getAgentBody().x, world.getAgentBody().y),
            "memory": psutil.Process().memory_info().rss / 1024 / 1024
        }
        
        print(f"[{metrics['time']:.1f}s] Firing: {metrics['firing_rate']:.2f} Hz, "
              f"Curiosity: {metrics['curiosity']:.3f}, "
              f"Memory: {metrics['memory']:.1f} MB")
        
        time.sleep(interval)
```

#### Advanced Analytics
```python
# Advanced analytics and visualization
import matplotlib.pyplot as plt
import pandas as pd

def analyze_simulation_results(metrics):
    """Analyze and visualize simulation results."""
    
    df = pd.DataFrame(metrics)
    
    # Plot firing rate over time
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 2, 1)
    plt.plot(df['step'], df['firing_rate'])
    plt.title('Firing Rate Over Time')
    plt.xlabel('Step')
    plt.ylabel('Firing Rate (Hz)')
    
    # Plot curiosity over time
    plt.subplot(2, 2, 2)
    plt.plot(df['step'], df['curiosity'])
    plt.title('Curiosity Over Time')
    plt.xlabel('Step')
    plt.ylabel('Curiosity Level')
    
    # Plot position over time
    plt.subplot(2, 2, 3)
    plt.plot(df['step'], df['position_x'], label='X')
    plt.plot(df['step'], df['position_y'], label='Y')
    plt.title('Agent Position Over Time')
    plt.xlabel('Step')
    plt.ylabel('Position')
    plt.legend()
    
    # Plot memory usage
    plt.subplot(2, 2, 4)
    plt.plot(df['step'], df['memory'])
    plt.title('Memory Usage Over Time')
    plt.xlabel('Step')
    plt.ylabel('Memory (MB)')
    
    plt.tight_layout()
    plt.show()
    
    return analysis
```

### Common Issues and Solutions

#### High Memory Usage
**Solution:** Optimize configuration and use memory-efficient settings
```python
# Create memory-efficient configuration
config = nlm.createDefaultConfig()

# Reduce memory footprint
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.01)
config.set("plasticity.structural.enable", False)
config.set("neuromod.curiosity.enable", False)
config.set("neuromod.novelty.enable", False)
config.set("resources.max_memory", 256.0)
```

#### Low Performance
**Solution:** Use performance-optimized configuration
```python
# Create performance-optimized configuration
config = nlm.createDefaultConfig()

# Optimize for speed
config.set("brain.neuron_count", 300)
config.set("brain.synapse_density", 0.005)
config.set("plasticity.stdp.learning_rate", 0.0005)
config.set("neuromod.curiosity.enable", False)
config.set("simulation.timestep", 0.5)
```

#### Simulation Issues
**Solution:** Add error handling and debugging
```python
try:
    # Run simulation
    brain.step(step)
    
except Exception as e:
    print(f"Error at step {step}: {e}")
    
    # Debug the issue
    print(f"Brain state: {brain.getTotalNeuronCount()} neurons")
    print(f"World state: {world.getSimulationTime()} time")
    
    # Try to recover
    if "memory" in str(e).lower():
        print("Memory error - try reducing network size")
    elif "step" in str(e).lower():
        print("Step error - check brain state")
```

## Learning Assessment

### Assessment Checklist

#### Conceptual Understanding
- [ ] Can explain NLM components (brain, world, agent)
- [ ] Can describe neural computation process
- [ ] Can explain learning mechanisms
- [ ] Can describe system integration

#### Practical Skills
- [ ] Can create NLM configuration
- [ ] Can initialize brain, world, and agent
- [ ] Can run basic simulation
- [ ] Can interpret simulation results
- [ ] Can troubleshoot common issues

#### Advanced Skills
- [ ] Can design custom configurations
- [ ] Can analyze performance metrics
- [ ] Can optimize system parameters
- [ ] Can create comprehensive simulations
- [ ] Can document and share results

#### Research Capabilities
- [ ] Can design experimental protocols
- [ ] Can analyze and interpret complex data
- [ ] Can create reproducible research workflows
- [ ] Can publish results effectively

### Assessment Activities

1. **Knowledge Quiz**: Test understanding of NLM concepts
2. **Code Implementation**: Create and run simulation examples
3. **Performance Analysis**: Analyze and optimize simulation performance
4. {term}: project}Project**: Create comprehensive simulation with all systems
5. **Documentation**: Write clear documentation of simulations and results

### Continuous Learning Path

#### Week 1: Foundation
- Complete Example 1: Silent Brain
- Practice basic brain creation and initialization
- Understand neural computation basics
- Create simple configuration files

#### Week 2: Interaction
- Complete Example 2: Brain Watching World
- Learn world simulation and sensory processing
- Understand agent behavior and motor control
- Create basic simulation loops

#### Week 3: Integration
- Complete Example 3: Complete Agent
- Learn system integration and coordination
- Understand learning mechanisms
- Practice performance optimization

#### Week 4: Application
- Complete Examples 4-7: Advanced Applications
- Learn research experimentation
- Understand performance optimization
- Create professional documentation

#### Ongoing: Mastery
- Complete Example 8-10: Advanced Applications
- Develop custom solutions
- Contribute to NLM community
- Pursue advanced research

## Conclusion

The NLM progressive learning examples provide a comprehensive learning path from basic concepts to advanced applications. Each example builds on previous ones, ensuring solid foundation before introducing new concepts.

### Learning Outcomes

By completing these examples, learners will:

1. **Conceptual Mastery**:
   - Understand NLM architecture and components
   - Explain neural computation and learning
   - Describe system interactions and workflows

2. **Practical Competence**:
   - Create and configure NLM systems
   - Run and analyze simulations
   - Debug and troubleshoot issues
   - Optimize performance and memory usage

3. **Research Excellence**:
   - Design experimental protocols
   - Analyze and interpret results
   - Create reproducible research workflows
   - Document and share findings

4. **Professional Skills**:
   - Write clean, well-documented code
   - Create comprehensive simulations
   - Analyze and visualize data
   - Collaborate with NLM community

### Next Steps

After completing the examples:

1. **Deepen Knowledge**: Study advanced NLM concepts
2. **Expand Applications**: Create custom solutions for specific use cases
3. **Contribute to Community**: Share examples and improvements
4. **Pursue Research**: Apply NLM to research problems
5. **Professional Development**: Use NLM in production environments

The progressive learning examples provide a solid foundation for mastering NLM and preparing for advanced applications in neural simulation and artificial intelligence research.
