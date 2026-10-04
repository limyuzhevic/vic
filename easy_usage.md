# NLM - Enhanced Usage Guide for Beginners

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

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain (with enhanced API)
brain = pynlm.createBrain(pynlm.createDefaultConfig(), 1000, 1, 0.1)

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Enhanced Brain in a Simple World

```python
import pynlm

# Create everything with enhanced API
config = pynlm.create_advanced_config(1500, 0.15, 0.002, 0.8)
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld(20, 20, 8, 8)
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain, world)
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

### Example 3: Complete Agent with Advanced Features

```python
import pynlm

def run_advanced_agent_simulation(num_steps=2000):
    """Run a complete agent simulation with advanced features."""
    
    # Create enhanced configuration for deep learning
    config = pynlm.create_deep_learning_config()
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld(15, 15, 8, 8)
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop with detailed tracking
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (enhanced learning)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development (brain matures)
        agent.updateDevelopment(0.1)
        
        # Print detailed progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Create checkpoint
    brain.save(f"simulation_checkpoint_step_{num_steps}.bin")
    print(f"Simulation complete! Brain state saved to checkpoint.")
    
    return brain, agent, world

# Run the advanced simulation
brain, agent, world = run_advanced_agent_simulation(1000)
```

---

## Enhanced Example Patterns

### Pattern 1: Run Multiple Simulations in Batch

```python
def run_batch_simulation(configs, num_steps_per_config):
    """Run multiple simulations with different configurations."""
    brains = pynlm.run_batch_simulation(configs, num_steps_per_config)
    
    for i, brain in enumerate(brains):
        print(f"Simulation {i+1} complete!")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Final firing rate: {brain.getAverageFiringRate():.3f}")
        
        # Log detailed brain status
        brain.logStatus()
    
    return brains

# Example usage with different configurations
configs = [
    pynlm.create_fast_config(),
    pynlm.create_balanced_config(),
    pynlm.create_deep_learning_config(),
]

brains = run_batch_simulation(configs, 500)
```

### Pattern 2: Monitor and Debug Brain Performance

```python
def monitor_and_optimize_brain(brain, num_steps=1000):
    """Monitor brain performance and provide optimization suggestions."""
    
    # Run benchmark
    benchmark_result = pynlm.benchmark_brain(brain, num_steps)
    
    print(f"Performance Benchmark:")
    print(f"  Total steps: {benchmark_result.total_steps}")
    print(f"  Total time: {benchmark_result.total_time_ms:.2f} ms")
    print(f"  Average speed: {benchmark_result.avg_steps_per_second:.1f} steps/sec")
    print(f"  Max spikes: {benchmark_result.max_spikes}")
    
    # Debug brain state
    debug_info = pynlm.debug_brain_state(brain)
    
    print(f"\nBrain State Analysis:")
    print(f"  Total neurons: {debug_info.total_neurons}")
    print(f"  Total synapses: {debug_info.total_synapses}")
    print(f"  Active regions: {debug_info.active_regions}")
    print(f"  Firing ratio: {debug_info.firing_ratio:.3f}")
    print(f"  E/I ratio: {debug_info.ei_ratio:.3f}")
    
    # Check for potential issues
    if debug_info.firing_ratio > 0.8:
        print("  WARNING: High firing ratio - may indicate instability")
    
    if debug_info.ei_ratio < 0.1:
        print("  WARNING: Low E/I ratio - may indicate network hyperexcitability")
    
    return benchmark_result, debug_info

# Create a brain and monitor its performance
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

benchmark, debug = monitor_and_optimize_brain(brain, 500)
```

### Pattern 3: Advanced Agent with Curiosity-Driven Learning

```python
def run_curiosity_driven_experiment(world_width=20, world_height=20, total_steps=1000):
    """Run an experiment that demonstrates curiosity-driven learning."""
    
    # Create exploration configuration
    config = pynlm.create_exploration_config()
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld(world_width, world_height, 8, 8)
    world.configure(width=world_width, height=world_height, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(world_width/2, world_height/2)
    
    agent = pynlm.createAgentBrain(brain, world)
    agent.initialize(world)
    
    # Enable exploration behaviors
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enableRewardModulation(True)
    
    print("Running Curiosity-Driven Learning Experiment")
    print("=" * 50)
    
    exploration_events = 0
    learning_events = 0
    
    for step in range(total_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Agent decides action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward and update development
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Track exploration and learning
        if agent.getCuriosityLevel() > 0.7:
            exploration_events += 1
            print(f"Step {step}: High curiosity - exploring!")
        
        if agent.getPredictionError() > 0.1:
            learning_events += 1
            print(f"Step {step}: High prediction error - learning!")
        
        if step % 200 == 0:
            print(f"Step {step}: Cur={agent.getCuriosityLevel():.2f}, Novelty={agent.getNoveltyLevel():.2f}, PredErr={agent.getPredictionError():.2f}")
    
    print("\nExperiment Results:")
    print(f"  Total steps: {total_steps}")
    print(f"  Exploration events: {exploration_events}")
    print(f"  Learning events: {learning_events}")
    print(f"  Final development stage: {brain.getDevelopmentalStage()}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

# Run the curiosity-driven experiment
brain, agent, world = run_curiosity_driven_experiment(1000)
```

---

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create brain with custom params | `pynlm.createBrain(config, 1000, 1, 0.1)` |
| Create advanced config | `pynlm.create_advanced_config(1500, 0.15, 0.002, 0.8)` |
| Create deep learning config | `pynlm.create_deep_learning_config()` |
| Create exploration config | `pynlm.create_exploration_config()` |
| Create fast config | `pynlm.create_fast_config()` |
| Get preset configs | `pynlm.get_config_presets()` |
| Run batch simulation | `pynlm.run_batch_simulation(configs, 500)` |
| Benchmark brain | `pynlm.benchmark_brain(brain, 1000)` |
| Debug brain state | `pynlm.debug_brain_state(brain)` |
| Advanced agent sim | `pynlm.run_advanced_agent_simulation(brain, world, 1000, True, True)` |
| Log brain status | `brain.logStatus()` |
| Create checkpoint | `brain.save("checkpoint.bin")` |

---

## Troubleshooting

**"My advanced simulation is too slow"**
- Try using `create_fast_config()` for quicker simulations
- Use `create_balanced_config()` for balanced performance
- Reduce `num_steps` in benchmark functions

**"The agent isn't learning properly"**
- Ensure `enableRewardModulation(True)` is called
- Check that `enableCuriosity(True)` is enabled
- Verify `enableDevelopment(True)` for brain maturation

**"Curiosity is too high"**
- Adjust world complexity and sensory input
- Use `create_balanced_config()` for more controlled learning
- Monitor novelty and prediction error levels

**"Brain is unstable"**
- Use `create_fast_config()` or `create_balanced_config()`
- Check benchmark results for performance issues
- Use `debug_brain_state()` to analyze stability

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more detailed C++ usage information
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations** and presets!
4. **Try advanced patterns** like batch processing and benchmarking
5. **Create your own custom configurations** using `create_advanced_config()`

That's it! You're now ready to use NLM with all its advanced features!

**Remember:** The enhanced API provides powerful tools for both beginners and advanced users. Start with simple examples and gradually explore the advanced capabilities as you become more familiar with the system.
