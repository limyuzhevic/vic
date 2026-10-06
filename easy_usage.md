# NLM - Enhanced Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental artificial developmental brain** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells  
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## What Makes NLM Different

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM **IS** intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## New Enhanced Features

### 1. **Robust Error Handling**
- All Python methods now validate input parameters
- Clear error messages for invalid inputs
- Graceful handling of missing files and invalid configurations

### 2. **Batch Processing**
- Process multiple sensory inputs at once
- Batch motor command decoding
- Efficient parallel processing

### 3. **Advanced Configuration**
- Detailed configuration options for all subsystems
- Fine-tune brain development, learning, and memory parameters
- Runtime configuration validation

### 4. **Performance Optimizations**
- Memory pooling for efficient allocation
- Optimized region lookup using hash maps
- Pre-allocated memory for better performance

### 5. **Enhanced Validation**
- Comprehensive input validation for all operations
- Parameter bounds checking
- Type safety for configuration values

### 6. **Detailed Statistics and Monitoring**
- Real-time brain state monitoring
- Performance profiling tools
- Neural activity analysis

### 7. **Advanced Learning Features**
- Customizable development stages
- Fine-grained control over plasticity
- Novelty-based exploration

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

# Quick Start (Enhanced)

### Example 1: Simplest Brain with Error Handling

```python
import pynlm

# Step 1: Make a brain with default config
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Initialize with error checking
if not brain.initialize():
    print("Error: Brain initialization failed")
    exit(1)

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Brain status:", brain.getStatus())
```

### Example 2: Brain in a Simple World with Advanced Features

```python
import pynlm

# Create everything with enhanced configuration
config = pynlm.createDefaultConfig()

# Configure advanced parameters
config.set("vision_width", 32)           # Custom vision dimensions
config.set("vision_height", 32)
config.set("neuron_count", 2000)         # More neurons for complex behavior
config.set("development_enabled", True)  # Enable brain development
config.set("curiosity_enabled", True)    # Enable exploration
config.set("reward_modulation", True)    # Enable learning

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableAdvancedLearning(True)

# Run simulation with enhanced monitoring
for step in range(200):
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Process sensory input with validation
    if not agent.processSensoryInput(percept):
        print(f"Warning: Invalid sensory input at step {step}")
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log advanced statistics every 50 steps
    if step % 50 == 0:
        stats = brain.getStatistics()
        print(f"Step {step}: Firing {stats.firingNeurons} neurons, "
              f"Firing rate: {stats.averageFiringRate:.2f}, "
              f"Curiosity: {stats.curiosityLevel:.2f}")

print("Enhanced simulation finished!")
print("Final brain statistics:", brain.getStatistics())
```

### Example 3: Batch Processing for High Performance

```python
import pynlm

# Create brain with performance optimizations
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("enable_performance_optimizations", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create multiple worlds for batch processing
worlds = []
for i in range(5):
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    worlds.append(world)

agents = [pynlm.createAgentBrain(brain) for _ in range(5)]
for i, agent in enumerate(agents):
    agent.initialize(worlds[i])
    agent.enableAdvancedLearning(True)

# Process all worlds in parallel
print("Processing multiple worlds in parallel...")
for step in range(100):
    # Collect all sensory inputs
    all_percepts = []
    for world in worlds:
        all_percepts.append(world.getSensoryPercept())
    
    # Batch process all sensory inputs
    if agent.batchProcessSensoryInputs(all_percepts):
        print(f"Successfully processed {len(all_percepts)} worlds at step {step}")
    
    # Process all brains
    for i, agent in enumerate(agents):
        worlds[i].update(0.1)
        action = agent.decodeMotorCommand()
        worlds[i].applyMotorCommand(action, worlds[i].getSimulationTime())

print("Parallel processing completed!")
```

### Example 4: Memory Management and Checkpointing

```python
import pynlm
import os

# Create brain with checkpointing enabled
config = pynlm.createDefaultConfig()
config.set("checkpoint_dir", "./nlm_checkpoints")
config.set("max_checkpoints", 5)
config.set("enable_compression", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(50):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}")

# Save brain state for later recovery
checkpoint_file = "./nlm_checkpoints/brain_checkpoint.nlm"
if brain.save(checkpoint_file):
    print(f"Brain state saved to {checkpoint_file}")
else:
    print("Error saving brain state")

# Create a new brain and load the saved state
brain2 = pynlm.createBrain(config)
brain2.initialize()

if brain2.load(checkpoint_file):
    print("Successfully loaded saved brain state")
    print("Loaded brain statistics:", brain2.getStatistics())
else:
    print("Error loading brain state")

# Cleanup
if os.path.exists(checkpoint_file):
    os.remove(checkpoint_file)
    os.rmdir("./nlm_checkpoints")
```

### Example 5: Development and Learning Optimization

```python
import pynlm

# Create brain with developmental stages
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("enable_development", True)
config.set("enable_curiosity", True)
config.set("enable_reward_modulation", True)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world=None)  # Simplified for example

# Enable all learning systems
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableAdvancedLearning(True)

print("Running developmental learning simulation...")
print("This will show how the brain develops over time:")

for step in range(300):
    brain.step(step)
    
    # Simulate development updates
    if step % 50 == 0:
        agent.updateDevelopment(50.0)
        developmental_stage = agent.getDevelopmentalStage()
        curiosity = agent.getCuriosityLevel()
        plasticity = agent.getPlasticityModifier()
        
        print(f"Step {step}: Stage={developmental_stage}, "
              f"Curiosity={curiosity:.2f}, Plasticity={plasticity:.2f}")

print("Developmental learning simulation completed!")
print("Final developmental stage:", agent.getDevelopmentalStage())
print("Final plasticity modifier:", agent.getPlasticityModifier())
```

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation with Error Handling and Monitoring

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    """Enhanced simulation with comprehensive monitoring and error handling"""
    errors = 0
    successful_steps = 0
    
    for step in range(num_steps):
        try:
            # 1. Update world
            world.update(0.1)
            
            # 2. Get what the agent sees
            percept = world.getSensoryPercept()
            
            # 3. Tell the brain (with validation)
            if not agent.processSensoryInput(percept):
                errors += 1
                continue
            
            successful_steps += 1
            
            # 4. Brain thinks
            brain.step(step)
            
            # 5. Get action from brain
            action = agent.decodeMotorCommand()
            
            # 6. Do action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # 7. Log statistics every 100 steps
            if step % 100 == 0:
                stats = brain.getStatistics()
                print(f"Step {step}: Firing {stats.firingNeurons} neurons, "
                      f"Firing rate: {stats.averageFiringRate:.2f}")
                      
        except Exception as e:
            errors += 1
            print(f"Error at step {step}: {e}")
    
    print(f"\nSimulation Summary:")
    print(f"  Total steps: {num_steps}")
    print(f"  Successful steps: {successful_steps}")
    print(f"  Errors: {errors}")
    print(f"  Success rate: {successful_steps/num_steps*100:.1f}%")
    
    return errors == 0

# Use it like this:
# run_enhanced_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Comprehensive Brain Stats with Advanced Metrics

```python
# Get detailed brain statistics
stats = brain.getStatistics()

print("=== Comprehensive Brain Statistics ===")
print(f"Neural Statistics:")
print(f"  Total neurons: {stats.totalNeurons}")
print(f"  Active neurons: {stats.activeNeurons}")
print(f"  Firing neurons: {stats.firingNeurons}")
print(f"  Average firing rate: {stats.averageFiringRate:.3f}")
print(f"  Excitation/inhibition ratio: {stats.excitationInhibitionRatio:.3f}")

print(f"\nNeuromodulation State:")
print(f"  Dopamine level: {stats.dopamineLevel:.3f}")
print(f"  Curiositity level: {stats.curiosityLevel:.3f}")
print(f"  Novelty level: {stats.noveltyLevel:.3f}")
print(f"  Prediction error: {stats.predictionError:.3f}")

print(f"\nDevelopmental State:")
print(f"  Developmental age: {stats.developmentalAge:.1f}")
print(f"  Plasticity modifier: {stats.plasticityModifier:.3f}")
print(f"  Current stage: {stats.developmentalStage}")
```

### Pattern 3: Enable Advanced Learning Features

```python
# Enable comprehensive learning
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enableAdvancedLearning(True)      # Use advanced learning algorithms
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections

print("Enabled advanced learning features:")
print(f"  Development: {'enabled' if agent.isDevelopmentEnabled() else 'disabled'}")
print(f"  Curiosity: {'enabled' if agent.isCuriosityEnabled() else 'disabled'}")
print(f"  Advanced Learning: {'enabled' if agent.isAdvancedLearningEnabled() else 'disabled'}")
print(f"  Reward Modulation: {'enabled' if agent.isRewardModulationEnabled() else 'disabled'}")
print(f"  Structural Plasticity: {'enabled' if agent.isStructuralPlasticityEnabled() else 'disabled'}")
```

### Pattern 4: Performance Monitoring and Optimization

```python
def monitorPerformance(brain, agent, duration):
    """Monitor brain performance over time"""
    print(f"Monitoring performance for {duration} steps...")
    
    for step in range(duration):
        brain.step(step)
        
        if step % 50 == 0:
            stats = brain.getStatistics()
            agent.analyzeNeuralActivity()
            
            print(f"Step {step}:")
            print(f"  Firing neurons: {stats.firingNeurons}")
            print(f"  Average firing rate: {stats.averageFiringRate:.3f}")
            print(f"  Processing calls: {agent.getProcessCalls()}")
            print(f"  Decode calls: {agent.getDecodeCalls()}")
    
    agent.profilePerformance()

# Use it like this:
# monitorPerformance(brain, agent, 500)
```

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Initialize brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getStatistics()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Validate input | `agent.validate()` |
| Batch process | `agent.batchProcessSensoryInputs(percepts)` |
| Get performance | `agent.getStatistics()` |

## Troubleshooting (Enhanced)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if `agent.enableDevelopment(True)` is needed
- Validate input with `agent.validate()`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure `agent.decodeMotorCommand()` returns valid action
- Verify `agent.enableCuriosity(True)` for exploration

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input is connected
- Check if `agent.enableRewardModulation(True)` is needed
- Validate configuration parameters

**"Batch processing failed"**
- Check input sizes with `agent.getSensoryInputSize()`
- Ensure all percepts have valid data
- Use `agent.batchProcessSensoryInputs(percepts)` for error details

**"Performance is slow"**
- Use `agent.optimizeNeuralGroups()` for better performance
- Enable performance optimizations in config
- Monitor with `agent.profilePerformance()`
- Check memory usage with `brain.getMemoryUsage()`

## Next Steps

When you're comfortable with the enhanced features:

1. **Read `HOW_TO_USE.md` for more details**
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**
4. **Try the advanced features like batch processing and performance monitoring**
5. **Use the test suite to validate your implementations**

That's it! You're now ready to use the **enhanced NLM** with all its new features!

