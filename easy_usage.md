# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, "serene flow") is an experimental artificial brain project. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

## What Makes NLM Special (Phase 6 Integration)

Phase 6 brings all the systems together into a coherent artificial brain:

- **Memory Integration**: Working memory stores active information, episodic memory stores experiences, and associative memory makes connections
- **Neuromodulation**: Dopamine affects learning, curiosity drives exploration, novelty detects change
- **Prediction**: The brain predicts what will happen next
- **Cognition**: Planning, concept formation, attention, and self-model
- **Development**: The brain matures over time with changing plasticity
- **Replay and Consolidation**: Memory maintenance during rest

That's the new integrated brain we're simulating!

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
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Working memory traces:", brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0)
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Memory features
agent.enableEpisodicMemory(True)        # Store experiences
agent.enablePrediction(True)           # Predict outcomes
agent.enableConceptFormation(True)     # Discover patterns
agent.enablePlanning(True)             # Plan actions
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

### `agent.enableEpisodicMemory(True)`

Stores experiences in the brain's episodic memory system, allowing the brain to remember and replay past events.

### `agent.enablePrediction(True)`

Activates the prediction system that anticipates future states and actions, enabling proactive behavior planning.

### `agent.enableConceptFormation(True)`

Enables concept formation to discover abstract patterns and categories from experiences without requiring labeled data.

### `agent.enablePlanning(True)`

Turns on the planning system that evaluates potential action sequences and selects optimal futures based on predictions.

---

## What Each Part Does (Enhanced with Phase 6)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses. In Phase 6, this includes integrated memory, prediction, cognition, and neuromodulation systems.

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron (LIF dynamics)
- Sends signals between connected neurons (spike propagation)
- Updates connections based on learning rules (plasticity)
- Updates working memory (neural maintenance)
- Applies neuromodulation (dopamine, curiosity)
- Stores episodic memory (experience encoding)
- Updates prediction system
- Updates attention
- Updates concept formation
- Applies structural plasticity (growth/pruning)
- Updates development
- Consolidates memories
- Manages replay

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain. In Phase 6, this includes attention selection and prediction error computation.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take. In Phase 6, this includes planning and curiosity-driven exploration.

---

## Phase 6 Integration Features

Phase 6 brings all the neural systems together into a coherent artificial brain. Here are the key integrated features:

### Memory Systems

**Working Memory** (`brain.getWorkingMemory()`)
- Stores active neural patterns for immediate processing
- Uses persistent activity with recurrent connections
- Automatically maintained and updated each simulation step
- Provides short-term memory for current processing

**Episodic Memory** (`brain.getEpisodicMemory()`)
- Stores complete experiences with sensory input, actions, and rewards
- Includes temporal and spatial indexing for efficient retrieval
- Supports replay during consolidation for memory strengthening
- Automatically updated every 10 steps in Phase 6

**Associative Memory** (`brain.getAssociativeMemory()`)
- Creates connections between related experiences and patterns
- Enables pattern completion from partial cues
- Supports concept formation through pattern discovery

### Neuromodulation Systems

**Dopamine** (`brain.getDopamine()`)
- Modulates synaptic plasticity based on reward prediction error
- Affects neural excitability and learning rates
- Enables reward-modulated STDP for credit assignment
- Integrates with eligibility traces for learning

**Curiosity** (`agent.getCuriosityLevel()`)
- Drives exploration of novel stimuli and situations
- Combines prediction error with novelty detection
- Influences action selection for exploration vs. exploitation

**Novelty** (`agent.getNoveltyLevel()`)
- Detects changes in sensory input patterns
- Signals unexpected events for learning
- Affects attention and memory encoding

### Prediction System

**Neural Prediction** (`brain.getPredictionSystem()`)
- Anticipates future sensory inputs and action consequences
- Provides prediction error signals for learning
- Supports multi-step planning and goal evaluation
- Continuously updated from experiences

### Cognitive Systems

**Planning** (`brain.getPlanner()`)
- Evaluates multiple action sequences
- Selects optimal futures based on predictions
- Supports hierarchical and sequential planning
- Integrates with attention and self-model

**Concept Formation** (`brain.getConceptFormation()`)
- Discovers abstract patterns from raw data
- Forms categories and prototypes without supervision
- Enables generalization from specific experiences
- Supports transfer learning across tasks

**Attention** (`brain.getAttention()`)
- Selects relevant information for processing
- Uses competitive dynamics for focus selection
- Integrates top-down and bottom-up signals
- Modulates neural processing based on salience

### Development System

**Development** (`brain.getDevelopmentSystem()`)
- Progresses through developmental stages
- Modulates plasticity rates over time
- Affects structural and functional maturation
- Enables critical period learning

## Complete Integration Example

Here's a comprehensive example showing all Phase 6 features working together:

```python
import pynlm
import time

def run_phase6_agent_simulation(num_steps=1000):
    """Run a complete Phase 6 agent simulation with all integrated systems."""
    
    # 1. Create Phase 6 optimized configuration
    config = pynlm.createDefaultConfig()
    
    # Phase 6 specific configurations
    config.set("replay_interval", 100)        # Replay every 100 steps
    config.set("consolidation_interval", 1000) # Consolidate every 1000 steps
    config.set("synaptogenesis_rate", 0.0001)  # Growth rate
    config.set("pruning_rate", 0.00001)       # Pruning rate
    
    # 2. Create brain with integrated systems
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
    
    # 6. Enable ALL Phase 6 features
    agent.enableRewardModulation(True)        # Reward-modulated learning
    agent.enableStructuralPlasticity(True)     # Synaptogenesis and pruning
    agent.enableDevelopment(True)              # Developmental stages
    agent.enableCuriosity(True)               # Exploration drive
    agent.enableEpisodicMemory(True)          # Experience storage
    agent.enablePrediction(True)             # Forward models
    agent.enableConceptFormation(True)       # Pattern abstraction
    agent.enablePlanning(True)               # Action selection
    
    # 7. Main simulation loop
    for step in range(num_steps):
        start_time = time.time()
        
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain (includes attention)
        agent.processSensoryInput(percept)
        
        # Run brain step (integrates ALL systems)
        brain.step(step)
        
        # Decode motor command (uses planning and curiosity)
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (dopamine system)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print comprehensive status every 100 steps
        if step % 100 == 0:
            status = {
                'step': step,
                'time': world.getSimulationTime(),
                'firing_neuron': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'dev_stage': brain.getDevelopmentalStage(),
                'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                'episodic_memories': len(brain.getEpisodicMemory().getEpisodes()) if brain.getEpisodicMemory() else 0,
                'concepts': len(brain.getConceptFormation().getConcepts()) if brain.getConceptFormation() else 0,
                'plan_quality': brain.getPlanner().getLastPlanQuality() if brain.getPlanner() else 0,
            }
            print(f"Step {status['step']}: "
                  f"{status['firing_neuron']} firing, "
                  f"curiosity: {status['curiosity']:.3f}, "
                  f"novelty: {status['novelty']:.3f}, "
                  f"dev: {status['dev_stage'].name}, "
                  f"mem: {status['working_memory_traces']} working, "
                  f"{status['episodic_memories']} episodic")
            
            # Save checkpoint every 500 steps
            if step % 500 == 0:
                brain.save(f"checkpoint_phase6_{step}.bin")
                print(f"Checkpoint saved: checkpoint_phase6_{step}.bin")
    
    # 8. Print final statistics
    print("\n=== Phase 6 Simulation Complete ===")
    final_stats = {
        'total_time': num_steps * 0.1,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'development_progress': brain.getDevelopmentalStage().name,
        'working_memory_utilization': brain.getWorkingMemory().getMemoryUtilization() if brain.getWorkingMemory() else 0,
        'episodic_memory_count': len(brain.getEpisodicMemory().getEpisodes()) if brain.getEpisodicMemory() else 0,
        'concept_count': len(brain.getConceptFormation().getConcepts()) if brain.getConceptFormation() else 0,
        'prediction_accuracy': 1.0 - abs(agent.getPredictionError()),
    }
    
    for key, value in final_stats.items():
        print(f"{key.replace('_', ' ').title()}: {value}")
    
    # Save final checkpoint
    brain.save("checkpoint_final_phase6.bin")
    print("\nFinal checkpoint saved: checkpoint_final_phase6.bin")
    
    return final_stats

# Run the simulation
if __name__ == "__main__":
    print("Starting Phase 6 NLM Agent Simulation...")
    print("This simulation integrates all neural systems:")
    print("  - Memory Systems (Working, Episodic, associative)")
    print("  - Neuromodulation (Dopamine, curiosity, novelty)")
    print("  - Prediction System")
    print("  - Cognitive Systems (Planning, concept formation, attention)")
    print("  - Development System")
    print("  - Replay and Consolidation")
    print()
    
    stats = run_phase6_agent_simulation(1000)
    
    print("\nSimulation completed successfully!")
    print("The integrated brain demonstrated coherent cognitive behavior.")
    print("Key achievements:")
    print(f"  - {stats['episodic_memory_count']} episodic memories stored")
    print(f"  - {stats['concept_count']} concepts formed")
    print(f"  - {stats['working_memory_utilization']:.1%} working memory utilization")
    print(f"  - {stats['prediction_accuracy']:.1%} prediction accuracy")
```

## Quick Reference - Phase 6 Features

| Feature | Method | Description |
|---------|--------|-------------|
| **Working Memory** | `brain.getWorkingMemory()` | Active pattern storage and maintenance |
| **Episodic Memory** | `brain.getEpisodicMemory()` | Experience recording and replay |
| **Associative Memory** | `brain.getAssociativeMemory()` | Pattern associations and connections |
| **Dopamine** | `brain.getDopamine()` | Reward-modulated learning |
| **Curiosity** | `agent.getCuriosityLevel()` | Exploration drive |
| **Novelty** | `agent.getNoveltyLevel()` | Change detection |
| **Prediction** | `brain.getPredictionSystem()` | Forward models and planning |
| **Planning** | `brain.getPlanner()` | Action sequence evaluation |
| **Concept Formation** | `brain.getConceptFormation()` | Pattern abstraction |
| **Attention** | `brain.getAttention()` | Selective processing |
| **Development** | `brain.getDevelopmentSystem()` | Maturation and plasticity |

## Troubleshooting (Updated for Phase 6)

**"My integrated brain isn't working"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps to allow systems to integrate
- Check that `agent.enableEpisodicMemory(True)` is called if you want memory

**"The agent's behavior is too simple"**
- Enable curiosity and planning for more complex behavior: `agent.enableCuriosity(True)`, `agent.enablePlanning(True)`
- Enable concept formation for abstraction: `agent.enableConceptFormation(True)`
- The brain needs time to learn through memory and prediction

**"Memory systems aren't updating"**
- Ensure `agent.enableEpisodicMemory(True)` and `agent.enablePrediction(True)` are called
- Check that replay intervals are configured: `config.set("replay_interval", 100)`
- Memory systems need sufficient recurrent connectivity

**"Performance is slow"**
- Use Phase 6 optimizations: `config.set("synaptogenesis_rate", 0.0001)`
- Consider reducing the number of neurons for faster simulation
- Memory systems use significant resources but provide rich behavior

## Next Steps

When you're comfortable with Phase 6 integration:

1. **Read `HOW_TO_USE.md`** for detailed C++ integration patterns
2. **Read `docs/PHASE6_FINAL_RESULTS.md`** to understand Phase 6 achievements and limitations
3. **Experiment with different configurations** to optimize for specific tasks
4. **Read `docs/SCIENCE.md`** for biological background and theory
5. **Create your own experiments** to test specific neural mechanisms

That's it! You're now ready to use the fully integrated Phase 6 NLM brain with all its cognitive capabilities.

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

## Phase 6 Integration Features

Phase 6 brings all the neural systems together into a coherent artificial brain. Here are the key integrated features:

### Memory Systems

**Working Memory** (`brain.getWorkingMemory()`)
- Stores active neural patterns for immediate processing
- Uses persistent activity with recurrent connections
- Automatically maintained and updated each simulation step
- Provides short-term memory for current processing

**Episodic Memory** (`brain.getEpisodicMemory()`)
- Stores complete experiences with sensory input, actions, and rewards
- Includes temporal and spatial indexing for efficient retrieval
- Supports replay during consolidation for memory strengthening
- Automatically updated every 10 steps in Phase 6

**Associative Memory** (`brain.getAssociativeMemory()`)
- Creates connections between related experiences and patterns
- Enables pattern completion from partial cues
- Supports concept formation through pattern discovery

### Neuromodulation Systems

**Dopamine** (`brain.getDopamine()`)
- Modulates synaptic plasticity based on reward prediction error
- Affects neural excitability and learning rates
- Enables reward-modulated STDP for credit assignment
- Integrates with eligibility traces for learning

**Curiosity** (`agent.getCuriosityLevel()`)
- Drives exploration of novel stimuli and situations
- Combines prediction error with novelty detection
- Influences action selection for exploration vs. exploitation

**Novelty** (`agent.getNoveltyLevel()`)
- Detects changes in sensory input patterns
- Signals unexpected events for learning
- Affects attention and memory encoding

### Prediction System

**Neural Prediction** (`brain.getPredictionSystem()`)
- Anticipates future sensory inputs and action consequences
- Provides prediction error signals for learning
- Supports multi-step planning and goal evaluation
- Continuously updated from experiences

### Cognitive Systems

**Planning** (`brain.getPlanner()`)
- Evaluates multiple action sequences
- Selects optimal futures based on predictions
- Supports hierarchical and sequential planning
- Integrates with attention and self-model

**Concept Formation** (`brain.getConceptFormation()`)
- Discovers abstract patterns from raw data
- Forms categories and prototypes without supervision
- Enables generalization from specific experiences
- Supports transfer learning across tasks

**Attention** (`brain.getAttention()`)
- Selects relevant information for processing
- Uses competitive dynamics for focus selection
- Integrates top-down and bottom-up signals
- Modulates neural processing based on salience

### Development System

**Development** (`brain.getDevelopmentSystem()`)
- Progresses through developmental stages
- Modulates plasticity rates over time
- Affects structural and functional maturation
- Enables critical period learning

## Complete Integration Example

Here's a comprehensive example showing all Phase 6 features working together:

```python
import pynlm
import time

def run_phase6_agent_simulation(num_steps=1000):
    """Run a complete Phase 6 agent simulation with all integrated systems."""
    
    # 1. Create configuration with Phase 6 optimizations
    config = pynlm.createDefaultConfig()
    config.set("replay_interval", 100)        # Replay every 100 steps
    config.set("consolidation_interval", 1000) # Consolidate every 1000 steps
    config.set("synaptogenesis_rate", 0.0001)  # Growth rate
    config.set("pruning_rate", 0.00001)       # Pruning rate
    
    # 2. Create brain with integrated systems
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
    
    # 6. Enable ALL Phase 6 features
    agent.enableRewardModulation(True)        # Reward-modulated learning
    agent.enableStructuralPlasticity(True)     # Synaptogenesis and pruning
    agent.enableDevelopment(True)              # Developmental stages
    agent.enableCuriosity(True)               # Exploration drive
    agent.enableEpisodicMemory(True)          # Experience storage
    agent.enablePrediction(True)             # Forward models
    agent.enableConceptFormation(True)       # Pattern abstraction
    agent.enablePlanning(True)               # Action selection
    
    # 7. Main simulation loop
    for step in range(num_steps):
        start_time = time.time()
        
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain (includes attention)
        agent.processSensoryInput(percept)
        
        # Run brain step (integrates ALL systems)
        brain.step(step)
        
        # Decode motor command (uses planning and curiosity)
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (dopamine system)
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print comprehensive status every 100 steps
        if step % 100 == 0:
            status = {
                'step': step,
                'time': world.getSimulationTime(),
                'firing_neuron': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'dev_stage': brain.getDevelopmentalStage(),
                'working_memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                'episodic_memories': len(brain.getEpisodicMemory().getEpisodes()) if brain.getEpisodicMemory() else 0,
                'concepts': len(brain.getConceptFormation().getConcepts()) if brain.getConceptFormation() else 0,
                'plan_quality': brain.getPlanner().getLastPlanQuality() if brain.getPlanner() else 0,
            }
            print(f"Step {status['step']}: "
                  f"{status['firing_neuron']} firing, "
                  f"curiosity: {status['curiosity']:.3f}, "
                  f"novelty: {status['novelty']:.3f}, "
                  f"dev: {status['dev_stage'].name}, "
                  f"mem: {status['working_memory_traces']} working, "
                  f"{status['episodic_memories']} episodic")
            
            # Save checkpoint every 500 steps
            if step % 500 == 0:
                brain.save(f"checkpoint_phase6_{step}.bin")
                print(f"Checkpoint saved: checkpoint_phase6_{step}.bin")
    
    # 8. Print final statistics
    print("\n=== Phase 6 Simulation Complete ===")
    final_stats = {
        'total_time': num_steps * 0.1,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'development_progress': brain.getDevelopmentalStage().name,
        'working_memory_utilization': brain.getWorkingMemory().getMemoryUtilization() if brain.getWorkingMemory() else 0,
        'episodic_memory_count': len(brain.getEpisodicMemory().getEpisodes()) if brain.getEpisodicMemory() else 0,
        'concept_count': len(brain.getConceptFormation().getConcepts()) if brain.getConceptFormation() else 0,
        'prediction_accuracy': 1.0 - abs(agent.getPredictionError()),
    }
    
    for key, value in final_stats.items():
        print(f"{key.replace('_', ' ').title()}: {value}")
    
    # Save final checkpoint
    brain.save("checkpoint_final_phase6.bin")
    print("\nFinal checkpoint saved: checkpoint_final_phase6.bin")
    
    return final_stats

# Run the simulation
if __name__ == "__main__":
    print("Starting Phase 6 NLM Agent Simulation...")
    print("This simulation integrates all neural systems:")
    print("  - Memory Systems (Working, Episodic, associative)")
    print("  - Neuromodulation (Dopamine, curiosity, novelty)")
    print("  - Prediction System")
    print("  - Cognitive Systems (Planning, concept formation, attention)")
    print("  - Development System")
    print("  - Replay and Consolidation")
    print()
    
    stats = run_phase6_agent_simulation(1000)
    
    print("\nSimulation completed successfully!")
    print("The integrated brain demonstrated coherent cognitive behavior.")
    print("Key achievements:")
    print(f"  - {stats['episodic_memory_count']} episodic memories stored")
    print(f"  - {stats['concept_count']} concepts formed")
    print(f"  - {stats['working_memory_utilization']:.1%} working memory utilization")
    print(f"  - {stats['prediction_accuracy']:.1%} prediction accuracy")
```

## Quick Reference - Phase 6 Features

| Feature | Method | Description |
|---------|--------|-------------|
| **Working Memory** | `brain.getWorkingMemory()` | Active pattern storage and maintenance |
| **Episodic Memory** | `brain.getEpisodicMemory()` | Experience recording and replay |
| **Associative Memory** | `brain.getAssociativeMemory()` | Pattern associations and connections |
| **Dopamine** | `brain.getDopamine()` | Reward-modulated learning |
| **Curiosity** | `agent.getCuriosityLevel()` | Exploration drive |
| **Novelty** | `agent.getNoveltyLevel()` | Change detection |
| **Prediction** | `brain.getPredictionSystem()` | Forward models and planning |
| **Planning** | `brain.getPlanner()` | Action sequence evaluation |
| **Concept Formation** | `brain.getConceptFormation()` | Pattern abstraction |
| **Attention** | `brain.getAttention()` | Selective processing |
| **Development** | `brain.getDevelopmentSystem()` | Maturation and plasticity |

## Troubleshooting (Updated for Phase 6)

**"My integrated brain isn't working"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps to allow systems to integrate
- Check that `agent.enableEpisodicMemory(True)` is called if you want memory

**"The agent's behavior is too simple"**
- Enable curiosity and planning for more complex behavior: `agent.enableCuriosity(True)`, `agent.enablePlanning(True)`
- Enable concept formation for abstraction: `agent.enableConceptFormation(True)`
- The brain needs time to learn through memory and prediction

**"Memory systems aren't updating"**
- Ensure `agent.enableEpisodicMemory(True)` and `agent.enablePrediction(True)` are called
- Check that replay intervals are configured: `config.set("replay_interval", 100)`
- Memory systems need sufficient recurrent connectivity

**"Performance is slow"**
- Use Phase 6 optimizations: `config.set("synaptogenesis_rate", 0.0001)`
- Consider reducing the number of neurons for faster simulation
- Memory systems use significant resources but provide rich behavior

## Next Steps

When you're comfortable with Phase 6 integration:

1. **Read `HOW_TO_USE.md`** for detailed C++ integration patterns
2. **Read `docs/PHASE6_FINAL_RESULTS.md`** to understand Phase 6 achievements and limitations
3. **Experiment with different configurations** to optimize for specific tasks
4. **Read `docs/SCIENCE.md`** for biological background and theory
5. **Create your own experiments** to test specific neural mechanisms

That's it! You're now ready to use the fully integrated Phase 6 NLM brain with all its cognitive capabilities.

