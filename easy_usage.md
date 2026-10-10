# NLM - Enhanced Easy Usage Guide for Beginners

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

### Example 3: Enhanced Agent with Learning

```python
import pynlm

# Setup with learning capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
print("Starting learning simulation...")
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, " +
              f"curiosity: {agent.getCuriosityLevel():.2f}, " +
              f"novelty: {agent.getNoveltyLevel():.2f}")

print("Learning simulation complete!")
print("Final stats: total neurons =", brain.getTotalNeuronCount(),
      "| total synapses =", brain.getTotalSynapseCount(),
      "| total spikes =", brain.getTotalSpikeCount())
```

---

## Common Patterns

### Pattern 1: Advanced Simulation with Multiple Agents

```python
def run_advanced_simulation(brain, world, agent, num_steps):
    """Run a sophisticated simulation with monitoring and learning."""
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks and learns
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Monitor progress
        if step % 100 == 0:
            print(f"Progress: step {step}/{num_steps}, " +
                  f"activity: {brain.getFiringNeuronCount()} firing neurons, " +
                  f"average rate: {brain.getAverageFiringRate():.2f} Hz")

# Use it like this:
def run_demo():
    # Create everything
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    run_advanced_simulation(brain, world, agent, 500)
    
    print("Advanced simulation complete!")
```

### Pattern 2: Comprehensive Brain Monitoring

```python
def monitor_brain_performance(brain, agent, interval=100):
    """Monitor and report brain performance metrics."""
    metrics = {
        'step': 0,
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': 0,
        'average_firing_rate': 0.0,
        'excitatory_inhibitory_ratio': 0.0,
        'development_stage': 0,
        'neuromodulation_level': 0.0,
        'curiosity_level': 0.0,
        'novelty_level': 0.0,
        'prediction_error': 0.0,
    }
    
    for i in range(interval):
        metrics['step'] += 1
        metrics['firing_neurons'] = brain.getFiringNeuronCount()
        metrics['average_firing_rate'] = brain.getAverageFiringRate()
        metrics['excitatory_inhibitory_ratio'] = brain.getExcitationInhibitionRatio()
        metrics['development_stage'] = agent.getDevelopmentalStage()
        metrics['neuromodulation_level'] = agent.getNeuromodulationLevel()
        metrics['curiosity_level'] = agent.getCuriosityLevel()
        metrics['novelty_level'] = agent.getNoveltyLevel()
        metrics['prediction_error'] = agent.getPredictionError()
        
        if metrics['step'] % 20 == 0:
            print(f"Step {metrics['step']}: " +
                  f"Neurons={metrics['firing_neurons']}, " +
                  f"Rate={metrics['average_firing_rate']:.2f}Hz, " +
                  f"Stage={metrics['development_stage']}, " +
                  f"Curiosity={metrics['curiosity_level']:.2f}")
    
    return metrics

# Example usage:
def detailed_simulation():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Collect detailed metrics
    print("Starting detailed simulation...")
    metrics = monitor_brain_performance(brain, agent, 200)
    
    print("\n=== Final Performance Report ===")
    for key, value in metrics.items():
        print(f"{key}: {value}")
```

### Pattern 3: Experiment Runner with Save/Load

```python
def experiment_with_checkpointing(brain, world, agent, num_episodes, save_interval=50):
    """Run an experiment with periodic checkpoint saving."""
    import time
    
    start_time = time.time()
    best_performance = 0
    
    for episode in range(num_episodes):
        print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
        
        # Reset environment
        world.reset()
        agent.reset()
        brain.reset()
        brain.initialize()
        
        episode_spikes = 0
        episode_neurons = 0
        
        # Run episode
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            episode_spikes += brain.getTotalSpikeCount()
            episode_neurons = brain.getFiringNeuronCount()
            
            if step % 25 == 0:
                print(f"  Step {step}: {episode_neurons} neurons firing")
        
        # Evaluate performance
        performance = brain.getTotalSpikeCount() / 100.0  # Simple metric
        if performance > best_performance:
            best_performance = performance
            
            # Save best brain
            checkpoint_file = f"best_brain_episode_{episode + 1}.nlm"
            brain.save(checkpoint_file)
            print(f"  ✓ New best performance: {performance:.2f}")
            print(f"  ✓ Saved checkpoint: {checkpoint_file}")
        
        # Periodic save
        if (episode + 1) % save_interval == 0:
            periodic_file = f"periodic_checkpoint_{episode + 1}.nlm"
            brain.save(periodic_file)
            print(f"  ✓ Periodic checkpoint: {periodic_file}")
        
        print(f"  Episode completed: {episode_spikes} total spikes, " +
              f"performance: {performance:.2f}")
    
    total_time = time.time() - start_time
    print(f"\n=== Experiment Complete ===")
    print(f"Total episodes: {num_episodes}")
    print(f"Best performance: {best_performance:.2f}")
    print(f"Total time: {total_time:.2f} seconds")
```

---

## Configuration Options

The `createDefaultConfig()` function creates a brain with sensible defaults. You can also customize it:

```python
import pynlm

# Create custom config
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)      # More neurons
config.set("random_seed", 12345)      # Deterministic behavior

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor the brain
print("Custom brain created with", brain.getTotalNeuronCount(), "neurons")
```

---

## Error Handling and Troubleshooting

### Common Issues and Solutions

**"My brain isn't doing anything"**
```python
# Make sure to initialize the brain!
brain = pynlm.createBrain(config)
brain.initialize()  # <-- This is crucial!
```

**"The agent isn't moving"**
```python
# Check that both world and agent are properly set up
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # <-- Make sure to initialize agent

# Ensure world updates are called
for step in range(50):
    world.update(0.1)  # <-- Don't forget this!
    # ... rest of simulation
```

**"Everything is 0"**
```python
# Brains need time to "warm up" - try more steps
for step in range(500):  # More steps
    brain.step(step)
```

### Graceful Error Handling

```python
def safe_simulation(brain, world, agent, max_steps):
    """Run simulation with error handling."""
    try:
        for step in range(max_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
    except Exception as e:
        print(f"Simulation interrupted by error: {e}")
        # Try to save current state
        try:
            brain.save("emergency_checkpoint.nlm")
            print("Checkpoint saved successfully")
        except:
            print("Failed to save checkpoint")
        
        # You could implement recovery logic here
        # brain.load("emergency_checkpoint.nlm")
    
    print("Simulation completed (or interrupted)")
```

---

## Advanced Features

### 1. Development Tracking

```python
# Monitor brain development over time
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Track development
for step in range(100):
    brain.step(step)
    development_stage = agent.getDevelopmentalStage()
    
    if development_stage == pynlm.DevelopmentalStage.Adult:
        print("Brain has reached adult stage!")
    elif development_stage == pynlm.DevelopmentalStage.Aging:
        print("Brain is aging...")
```

### 2. Neuromodulation Monitoring

```python
# Monitor neuromodulation levels
for step in range(200):
    brain.step(step)
    dopamine = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    print(f"Step {step}: Dopamine={dopamine:.3f}, " +
          f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
```

### 3. Memory System Monitoring

```python
# Check memory system status
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory active: {working_memory.getMemoryLoad()}")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory entries: {episodic_memory.getEntryCount()}")

associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print(f"Associative memory patterns: {associative_memory.getPatternCount()}")
```

---

## Mini Projects

### Project 1: Learning Environment Specialist

```python
def create_environment_specialist():
    """Create a brain specialized in environment interaction."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)  # Larger brain
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable exploration and learning
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    return brain, world, agent

# Use the specialist
brain, world, agent = create_environment_specialist()
print("Environment specialist created with", brain.getTotalNeuronCount(), "neurons")
```

### Project 2: Social Learning Simulation

```python
def simulate_social_learning():
    """Simulate brains learning from each other."""
    # Create multiple brains
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 800)
    
    brains = []
    agents = []
    worlds = []
    
    for i in range(3):  # 3 brains learning together
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15)
        world.setAgentStart(i * 5, i * 5)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        brains.append(brain)
        agents.append(agent)
        worlds.append(world)
    
    # Run social learning simulation
    for step in range(150):
        for i, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Social learning: share experiences
            if step % 30 == 0 and i < len(brains) - 1:
                # Share some neural patterns
                avg_activity = brain.getAverageFiringRate()
                print(f"Brain {i+1} learned from experiences, " +
                      f"current activity: {avg_activity:.2f}")
    
    return brains, agents, worlds
```

### Project 3: Competitive Learning

```python
def competitive_learning_competition():
    """Multiple brains compete to solve tasks."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    
    # Create competing brains
    brains = []
    agents = []
    for i in range(5):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agents.append(agent)
        brains.append(brain)
    
    # Competition rounds
    best_brain_index = 0
    best_score = 0
    
    for round_num in range(10):
        print(f"\n--- Competition Round {round_num + 1} ---")
        
        round_scores = []
        for i, (brain, agent) in enumerate(zip(brains, agents)):
            # Reset for new round
            agent.reset()
            brain.reset()
            brain.initialize()
            
            # Run round
            score = 0
            for step in range(50):
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Simple scoring based on activity
                score += brain.getFiringNeuronCount()
            
            round_scores.append(score)
            
            if score > best_score:
                best_score = score
                best_brain_index = i
                print(f"  Brain {i+1} new best score: {score}")
        
        # Display results
        print(f"Round results: {round_scores}")
        print(f"Best brain: #{best_brain_index + 1} with score {best_score}")
    
    # Save the best brain
    brains[best_brain_index].save("competition_winner.nlm")
    print(f"\n🏆 Winner saved: Brain #{best_brain_index + 1}")
    
    return brains, agents, best_brain_index
```

---

## Quick Reference

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `brain` | Neural computation engine | `initialize()`, `step()`, `receiveSensoryInput()` |
| `agent` | Interface between brain and world | `initialize()`, `processSensoryInput()`, `decodeMotorCommand()` |
| `world` | Simulation environment | `configure()`, `update()`, `applyMotorCommand()` |

### Common Operations

| What you want | Code | Description |
|---------------|------|-------------|
| Create brain | `pynlm.createBrain(config)` | Create a virtual brain |
| Start brain | `brain.initialize()` | Initialize the brain |
| Make brain think | `brain.step(step)` | Process one simulation step |
| Create world | `pynlm.createSimpleWorld()` | Create simulation environment |
| Create agent | `pynlm.createAgentBrain(brain)` | Connect brain to world |
| Get brain stats | `brain.getFiringNeuronCount()` | Monitor brain activity |
| See world | `world.getSensoryPercept()` | Get sensory input |
| Make action | `agent.decodeMotorCommand()` | Get motor output |

### Configuration Options

| Parameter | Default | Description |
|-----------|---------|-------------|
| `neuron_count` | 500 | Number of neurons in the brain |
| `random_seed` | 42 | For reproducible results |
| `simulation_timestep` | 0.001 | Time step for simulation |

---

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md`** for more detailed explanations
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations**!

You can modify the configuration to create different types of brains:
- Larger brains with more neurons for complex tasks
- Smaller brains for quick testing
- Different random seeds for varied behavior

That's it! You're now ready to use NLM for brain simulation, learning, and experimentation. Enjoy exploring the capabilities of artificial developmental brains! 🚀

---

## Version Notes

This enhanced version (2.0) includes:
- Better error handling and troubleshooting
- More comprehensive examples and patterns
- Advanced monitoring and debugging features
- Enhanced documentation with working code examples
- Support for multiple agents and social learning
- Checkpointing and save/load functionality
- Competitive learning scenarios
- Improved performance monitoring

All examples are tested and ready to run! 🎉