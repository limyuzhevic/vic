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

## Advanced Topics for Power Users

### 1. Brain Development and Maturation

The NLM brain develops through several stages, affecting its learning capabilities and behavior:

- **Initial**: High plasticity, rapid learning, exploratory behavior
- **Critical Period**: High plasticity but with guided learning
- **Maturation**: Moderate plasticity, selective learning
- **Adult**: Low plasticity, stable behavior, refined responses

```python
import pynlm

def develop_brain_over_time(brain, stages=3, steps_per_stage=200):
    """
    Demonstrate how a brain develops over time.
    
    This function shows how developmental stages affect learning capacity,
    neural connectivity, and behavioral patterns.
    """
    print("=== Brain Development Demo ===\n")
    
    # Track development metrics
    development_metrics = []
    
    for stage in range(stages):
        stage_start_step = sum(steps_per_stage for _ in range(stage))
        
        print(f"--- Development Stage {stage + 1}: {brain.getDevelopmentalStage()} ---")
        print(f"Plasticity Factor: {brain.getDevelopmentSystem().getPlasticityFactor():.3f}")
        print(f"Neural Excitability: {brain.getExcitabilityLevel():.3f}")
        
        # Run simulation for this developmental stage
        for step in range(steps_per_stage):
            brain.step(step)
            
            if step % 50 == 0:
                print(f"  Step {step + stage_start_step}: {brain.getFiringNeuronCount()} firing neurons")
        
        # Record metrics
        development_metrics.append({
            'stage': stage + 1,
            'stage_name': str(brain.getDevelopmentalStage()),
            'final_neuron_count': brain.getTotalNeuronCount(),
            'final_synapse_count': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'plasticity_factor': brain.getDevelopmentSystem().getPlasticityFactor()
        })
        
        # Progress to next developmental stage
        if stage < stages - 1:
            brain.setDevelopmentalStage(brain.getDevelopmentSystem().getNextStage())
    
    print(f"\n=== Development Summary ===")
    for metric in development_metrics:
        print(f"Stage {metric['stage']} ({metric['stage_name']}):")
        print(f"  Neurons: {metric['final_neuron_count']}")
        print(f"  Synapses: {metric['final_synapse_count']}")
        print(f"  Learning Rate: {metric['plasticity_factor']:.3f}")
    
    return development_metrics

def advanced_development_demo():
    """
    Demonstrate advanced developmental concepts including critical periods,
    synaptic pruning, and neural maturation.
    """
    print("=== Advanced Development Concepts ===\n")
    
    # Create a brain with extended development timeline
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("development_synaptogenesis_rate", 0.003)
    config.set("development_pruning_rate", 0.0008)
    config.set("development_critical_period_duration", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Extended development simulation
    print("Running extended development simulation (1000 steps)...")
    
    critical_period_active = False
    pruning_events = []
    synaptogenesis_events = []
    
    for step in range(1000):
        brain.step(step)
        
        # Detect critical period
        if step == 299:
            critical_period_active = True
            print(f"  -> Critical period initiated at step {step}")
        
        # Detect major structural changes
        if step % 100 == 0:
            synapse_count = brain.getTotalSynapseCount()
            neuron_count = brain.getTotalNeuronCount()
            
            if step % 300 == 0:
                pruning_events.append({
                    'step': step,
                    'synapse_count': synapse_count,
                    'type': 'pruning'
                })
                print(f"  -> Pruning event at step {step}: {synapse_count} synapses")
            
            if step % 100 == 0:
                synaptogenesis_events.append({
                    'step': step,
                    'synapse_count': synapse_count,
                    'type': 'synaptogenesis'
                })
                print(f"  -> Synaptogenesis event at step {step}: {synapse_count} synapses")
        
        # Check developmental stage changes
        if step % 200 == 0:
            print(f"  Development stage at step {step}: {brain.getDevelopmentalStage()}")
            print(f"  Current plasticity factor: {brain.getDevelopmentSystem().getPlasticityFactor():.3f}")
    
    print(f"\n=== Structural Changes Summary ===")
    print(f"Total synaptogenesis events: {len(synaptogenesis_events)}")
    print(f"Total pruning events: {len(pruning_events)}")
    print(f"Final synapse count: {brain.getTotalSynapseCount()}")
    print(f"Final neuron count: {brain.getTotalNeuronCount()}")
    
    return brain, synaptogenesis_events, pruning_events

# Use development demonstrations
print("Running development demonstrations...\n")

# Simple demonstration
development_metrics = develop_brain_over_time(
    pynlm.createBrain(pynlm.createDefaultConfig()),
    stages=3,
    steps_per_stage=200
)

# Advanced demonstration
brain, synaptogenesis, pruning = advanced_development_demo()
```

### 2. Neuromodulation and Learning Control

The brain uses multiple neuromodulators to control learning, behavior, and adaptability:

```python
import pynlm

def neuromodulation_control_demo():
    """
    Demonstrate how different neuromodulators work together to control
    learning, exploration, and behavior adaptation.
    """
    print("=== Neuromodulation Control Demo ===\n")
    
    # Create configuration with balanced neuromodulation
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.threshold", 0.15)
    config.set("neuromod.curiosity.rate", 0.08)
    config.set("neuromod.novelty.adaptation_rate", 0.06)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25)
    world.reset()
    world.setAgentStart(12.0, 12.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    
    # Track neuromodulation state over time
    neuromod_state = {
        'step': [],
        'dopamine': [],
        'curiosity': [],
        'novelty': [],
        'prediction_error': [],
        'behavior_mode': []
    }
    
    print("Running neuromodulation control simulation...")
    print("Behavior modes: EXPLORATION (high curiosity), EXPLOITATION (high dopamine), BALANCED")
    
    for step in range(800):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get current neuromodulation levels
        dopamine_level = brain.getDopamine().getLevel() if brain.getDopamine() else 0.0
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        # Determine behavior mode based on neuromodulation
        behavior_mode = "BALANCED"
        if curiosity_level > 0.4 and novelty_level > 0.3:
            behavior_mode = "EXPLORATION"
        elif dopamine_level > 0.8 and curiosity_level < 0.3:
            behavior_mode = "EXPLOITATION"
        
        # Store state
        neuromod_state['step'].append(step)
        neuromod_state['dopamine'].append(dopamine_level)
        neuromod_state['curiosity'].append(curiosity_level)
        neuromod_state['novelty'].append(novelty_level)
        neuromod_state['prediction_error'].append(prediction_error)
        neuromod_state['behavior_mode'].append(behavior_mode)
        
        # Apply neuromodulation to learning
        # Higher dopamine = stronger reward learning
        # Higher curiosity = more exploration, random actions
        # Higher novelty = surprise detection, behavioral flexibility
        
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Neuromodulation State:")
            print(f"    Dopamine: {dopamine_level:.3f}")
            print(f"    Curiosity: {curiosity_level:.3f}")
            print(f"    Novelty: {novelty_level:.3f}")
            print(f"    Prediction Error: {prediction_error:.3f}")
            print(f"  Behavior Mode: {behavior_mode}")
            print(f"  Action: {agent.decodeMotorCommand().getType()}")
            
            # Apply reward with neuromodulation modulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            
            # Neuromodulation affects reward learning
            if dopamine_level > 0.5:
                # Strong dopamine = high reward sensitivity
                reward *= (1.0 + dopamine_level)
            
            if curiosity_level > 0.3:
                # High curiosity = exploration bonus
                reward += curiosity_level * 0.5
            
            if novelty_level > 0.4:
                # High novelty = learning from surprise
                reward *= (1.0 + novelty_level)
            
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    print(f"\n=== Neuromodulation Analysis ===")
    print(f"Total steps: {len(neuromod_state['step'])}")
    print(f"Average dopamine: {sum(neuromod_state['dopamine']) / len(neuromod_state['dopamine']):.3f}")
    print(f"Average curiosity: {sum(neuromod_state['curiosity']) / len(neuromod_state['curiosity']):.3f}")
    print(f"Average novelty: {sum(neuromod_state['novelty']) / len(neuromod_state['novelty']):.3f}")
    
    mode_counts = {}
    for mode in neuromod_state['behavior_mode']:
        mode_counts[mode] = mode_counts.get(mode, 0) + 1
    
    print(f"\nBehavior mode distribution:")
    for mode, count in mode_counts.items():
        print(f"  {mode}: {count} steps ({count/len(neuromod_state['step'])*100:.1f}%)")
    
    return neuromod_state

# Run neuromodulation demo
neuro_state = neuromodulation_control_demo()
```

### 3. Memory Systems Integration

```python
import pynlm

def memory_integration_demo():
    """
    Demonstrate how working memory, episodic memory, and associative memory
    work together to support complex cognitive tasks.
    """
    print("=== Memory Systems Integration Demo ===\n")
    
    # Create configuration optimized for memory-intensive tasks
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2500)
    config.set("working_memory.capacity", 400)
    config.set("episodic_memory.max_episodes", 3000)
    config.set("associative_memory.similarity_threshold", 0.7)
    config.set("plasticity.stdp.learning_rate", 0.008)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    # Track memory usage
    memory_stats = {
        'working_memory_usage': [],
        'episodic_memory_count': [],
        'associative_memory_clusters': [],
        'memory_retrieval_events': 0
    }
    
    print("Running memory integration simulation...")
    print("Tracking: working memory capacity, episodic memory storage, associative memory formation")
    
    for step in range(1200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record memory statistics periodically
        if step % 150 == 0:
            # Working memory usage
            active_traces = working_memory.getActiveTraces()
            memory_stats['working_memory_usage'].append(active_traces)
            
            # Episodic memory count
            episode_count = episodic_memory.getEpisodeCount()
            memory_stats['episodic_memory_count'].append(episode_count)
            
            # Associative memory clusters (in real implementation, would be more detailed)
            cluster_count = len(associative_memory.getConcepts())
            memory_stats['associative_memory_clusters'].append(cluster_count)
            
            print(f"\nStep {step}:")
            print(f"  Memory Systems Status:")
            print(f"    Working Memory: {active_traces} active traces / {working_memory.getCapacity()} capacity")
            print(f"    Episodic Memory: {episode_count} stored episodes")
            print(f"    Associative Memory: {cluster_count} concept clusters")
            print(f"    Total Brain Spikes: {brain.getTotalSpikeCount()}")
            print(f"    Development Stage: {brain.getDevelopmentalStage()}")
        
        # Simulate memory consolidation periodically
        if step % 300 == 0 and episodic_memory.getEpisodeCount() > 100:
            memory_stats['memory_retrieval_events'] += 1
            print(f"  Memory consolidation triggered at step {step}")
            
            # Consolidate important memories
            episodic_memory.consolidate(0.4)
            print(f"    Consolidated {len(associative_memory.getConsolidatedEpisodes())} episodes")
        
        # Demonstrate memory retrieval
        if step % 400 == 0 and working_memory.getActiveTraces() > 0:
            memory_stats['memory_retrieval_events'] += 1
            print(f"  Memory retrieval at step {step}")
            
            # Get working memory neurons
            memory_neurons = working_memory.getMemoryNeurons()
            print(f"    Retrieved {len(memory_neurons)} memory neurons")
    
    print(f"\n=== Memory Integration Results ===")
    print(f"Final working memory usage: {memory_stats['working_memory_usage'][-1]} / {working_memory.getCapacity()}")
    print(f"Final episodic memory count: {memory_stats['episodic_memory_count'][-1]}")
    print(f"Final associative memory clusters: {memory_stats['associative_memory_clusters'][-1]}")
    print(f"Total memory retrieval events: {memory_stats['memory_retrieval_events']}")
    print(f"Average working memory usage: {sum(memory_stats['working_memory_usage']) / len(memory_stats['working_memory_usage']):.1f}")
    
    return memory_stats

# Run memory integration demo
memory_stats = memory_integration_demo()
```

### 4. Advanced Planning and Prediction

```python
import pynlm

def planning_prediction_demo():
    """
    Demonstrate advanced planning capabilities, prediction systems,
    and cognitive control mechanisms.
    """
    print("=== Planning and Prediction Demo ===\n")
    
    # Create configuration optimized for planning and prediction
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("prediction.planning_depth", 3)
    config.set("prediction.confidence_threshold", 0.75)
    config.set("prediction.error_correction_rate", 0.8)
    config.set("cognition.attention.inhibition_strength", 0.6)
    config.set("cognition.attention.excitation_strength", 1.8)
    config.set("cognition.attention.focus_window", 10)
    config.set("cognition.attentional_selection.adaptive_focus", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=35, height=35)
    world.reset()
    world.setAgentStart(17.5, 17.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get planning and prediction systems
    prediction_system = brain.getPredictionSystem()
    neural_planner = brain.getPlanner()
    attentional_system = brain.getAttention()
    concept_formation = brain.getConceptFormation()
    
    # Track planning and prediction metrics
    planning_stats = {
        'planning_attempts': 0,
        'successful_plans': 0,
        'prediction_accuracy': [],
        'attention_focus_changes': 0,
        'concept_formations': 0,
        'decision_confidence': []
    }
    
    print("Running planning and prediction simulation...")
    print("Tracking: planning success rate, prediction accuracy, attentional focus")
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record planning and prediction statistics
        if step % 100 == 0:
            # Planning statistics
            planning_stats['planning_attempts'] += 1
            
            # Simulate planning success (in real implementation, would query planner)
            plan_success = step % 8 == 0
            if plan_success:
                planning_stats['successful_plans'] += 1
            
            # Prediction accuracy (simulated)
            prediction_accuracy = 0.6 + (step % 40) / 100  # Varying accuracy
            planning_stats['prediction_accuracy'].append(prediction_accuracy)
            
            # Decision confidence (simulated)
            decision_confidence = 0.7 + (step % 30) / 100
            planning_stats['decision_confidence'].append(decision_confidence)
            
            # Attention focus changes (in real implementation, would track attentional shifts)
            if step % 150 == 0:
                planning_stats['attention_focus_changes'] += 1
            
            # Concept formation (simulated)
            if step % 200 == 0:
                planning_stats['concept_formations'] += 1
            
            print(f"\nStep {step}:")
            print(f"  Planning System:")
            print(f"    Planning Success Rate: {planning_stats['successful_plans'] / planning_stats['planning_attempts']:.3f}")
            print(f"    Average Prediction Accuracy: {sum(planning_stats['prediction_accuracy']) / len(planning_stats['prediction_accuracy']):.3f}")
            print(f"    Average Decision Confidence: {sum(planning_stats['decision_confidence']) / len(planning_stats['decision_confidence']):.3f}")
            print(f"  Cognitive Control:")
            print(f"    Attention Focus Changes: {planning_stats['attention_focus_changes']}")
            print(f"    Concepts Formed: {planning_stats['concept_formations']}")
            print(f"  Action: {action.getType() if hasattr(action, 'getType') else 'Unknown'}")
        
        # Demonstrate attentional selection
        if step % 300 == 0 and attentional_system and brain.getWorkingMemory():
            memory_neurons = brain.getWorkingMemory().getMemoryNeurons()
            if memory_neurons:
                attentional_system.processCompetition(memory_neurons)
                print(f"  Attentional selection processed {len(memory_neurons)} memory neurons")
    
    print(f"\n=== Planning and Prediction Results ===")
    print(f"Planning Success Rate: {planning_stats['successful_plans'] / planning_stats['planning_attempts']:.3f}")
    print(f"Average Prediction Accuracy: {sum(planning_stats['prediction_accuracy']) / len(planning_stats['prediction_accuracy']):.3f}")
    print(f"Average Decision Confidence: {sum(planning_stats['decision_confidence']) / len(planning_stats['decision_confidence']):.3f}")
    print(f"Attention Focus Changes: {planning_stats['attention_focus_changes']}")
    print(f"Concept Formations: {planning_stats['concept_formations']}")
    
    return planning_stats

# Run planning and prediction demo
planning_stats = planning_prediction_demo()
```

---

## Advanced Configuration Templates

### 1. High-Performance Learning Configuration

```python
import pynlm

def create_high_performance_config():
    """
    Create a configuration optimized for high-performance learning,
    balancing speed, accuracy, and adaptability.
    """
    config = pynlm.createDefaultConfig()
    
    # Neural architecture
    config.set("neuron_count", 3000)
    config.set("region_count", 4)
    config.set("connection_probability", 0.08)
    
    # Learning mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.015)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 25.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.01)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.002)
    config.set("plasticity.structural.pruning_rate", 0.0002)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.12)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.adaptation_rate", 0.07)
    
    # Memory systems
    config.set("working_memory.capacity", 500)
    config.set("episodic_memory.max_episodes", 5000)
    config.set("associative_memory.similarity_threshold", 0.8)
    
    # Development
    config.set("development_synaptogenesis_rate", 0.003)
    config.set("development_pruning_rate", 0.0005)
    config.set("development_critical_period_duration", 400)
    
    return config

# Use the configuration
performance_config = create_high_performance_config()
brain = pynlm.createBrain(performance_config)
brain.initialize()
```

### 2. Memory-Optimized Configuration

```python
import pynlm

def create_memory_optimized_config():
    """
    Create a configuration optimized for memory-intensive tasks,
    emphasizing storage, retrieval, and associative learning.
    """
    config = pynlm.createDefaultConfig()
    
    # Neural architecture for memory
    config.set("neuron_count", 2500)
    config.set("region_count", 3)
    config.set("connection_probability", 0.12)  // Higher connectivity for memory
    
    # Learning mechanisms focused on memory
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.008)
    config.set("plasticity.stdp.tau_plus", 30.0)  // Longer potentiation
    config.set("plasticity.stdp.tau_minus", 30.0)  // Longer depression
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.015)  // Stronger Hebbian
    config.set("plasticity.structural.enable", False)  // Less structural change
    
    # Neuromodulation for memory consolidation
    config.set("neuromod.dopamine.scale", 0.9)
    config.set("neuromod.curiosity.enable", False)  // Less exploration
    config.set("neuromod.novelty.enable", True)  // More novelty detection for memory
    config.set("neuromod.novelty.adaptation_rate", 0.05)
    
    # Memory systems optimized for storage
    config.set("working_memory.capacity", 800)
    config.set("episodic_memory.max_episodes", 8000)
    config.set("associative_memory.similarity_threshold", 0.85)  // Higher threshold for stronger associations
    
    # Development focused on memory
    config.set("development_synaptogenesis_rate", 0.004)
    config.set("development_pruning_rate", 0.0003)  // Slower pruning
    
    return config

# Use the configuration
memory_config = create_memory_optimized_config()
brain = pynlm.createBrain(memory_config)
brain.initialize()
```

### 3. Developmental Configuration

```python
import pynlm

def create_developmental_config():
    """
    Create a configuration optimized for developmental learning,
    emphasizing plasticity, adaptation, and stage-dependent learning.
    """
    config = pynlm.createDefaultConfig()
    
    # Neural architecture
    config.set("neuron_count", 1800)
    config.set("region_count", 3)
    config.set("connection_probability", 0.06)
    
    # Development-focused learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.02)  // Higher for rapid learning
    config.set("plasticity.stdp.tau_plus", 35.0)  // Extended potentiation
    config.set("plasticity.stdp.tau_minus", 35.0)  // Extended depression
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.025)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.005)
    config.set("plasticity.structural.pruning_rate", 0.001)
    
    # Neuromodulation for development
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.08)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.adaptation_rate", 0.06)
    
    # Development-focused memory
    config.set("working_memory.capacity", 300)
    config.set("episodic_memory.max_episodes", 2000)
    config.set("associative_memory.similarity_threshold", 0.75)
    
    # Extended development timeline
    config.set("development_synaptogenesis_rate", 0.006)
    config.set("development_pruning_rate", 0.0008)
    config.set("development_critical_period_duration", 600)
    
    return config

# Use the configuration
development_config = create_developmental_config()
brain = pynlm.createBrain(development_config)
brain.initialize()
```

### 4. Exploration-Optimized Configuration

```python
import pynlm

def create_exploration_config():
    """
    Create a configuration optimized for exploration and discovery,
    emphasizing curiosity, novelty detection, and behavioral flexibility.
    """
    config = pynlm.createDefaultConfig()
    
    # Neural architecture
    config.set("neuron_count", 2200)
    config.set("region_count", 4)
    config.set("connection_probability", 0.1)
    
    # Learning for exploration
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.03)  // Higher for exploration
    config.set("plasticity.stdp.tau_plus", 40.0)  // Extended for persistent learning
    config.set("plasticity.stdp.tau_minus", 40.0)
    config.set("plasticity.hebbian.enable", False)  // Less structured learning
    
    // Structural plasticity for exploration
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.008)
    config.set("plasticity.structural.pruning_rate", 0.002)
    
    // Neuromodulation for exploration
    config.set("neuromod.dopamine.scale", 0.8)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.05)  // Lower threshold for exploration
    config.set("neuromod.curiosity.rate", 0.12)  // Higher curiosity rate
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.adaptation_rate", 0.08)  // Faster adaptation
    
    // Memory for exploration
    config.set("working_memory.capacity", 200)
    config.set("episodic_memory.max_episodes", 3000)
    config.set("associative_memory.similarity_threshold", 0.7)  // Lower threshold for more associations
    
    return config

// Use the configuration
exploration_config = create_exploration_config()
brain = pynlm.createBrain(exploration_config)
brain.initialize()
```

---

## Performance Optimization Guidelines

### 1. Memory Management

```python
def optimize_memory_usage(brain):
    """
    Optimize memory usage for better performance.
    """
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    # Adjust working memory capacity based on neuron count
    neuron_count = brain.getTotalNeuronCount()
    optimal_capacity = max(100, min(1000, neuron_count // 10))
    working_memory.setCapacity(optimal_capacity)
    
    # Clean up old episodic memories
    if episodic_memory.getEpisodeCount() > 2000:
        episodic_memory.consolidate(0.3)
    
    print(f"Optimized memory: {optimal_capacity} working memory capacity, {episodic_memory.getEpisodeCount()} episodic episodes")
```

### 2. Learning Rate Scheduling

```python
def adaptive_learning_rate_scheduler(brain, step):
    """
    Adapt learning rates based on development stage and performance.
    """
    development_stage = brain.getDevelopmentalStage()
    
    # Schedule learning rates based on developmental stage
    if development_stage == "Initial":
        # High learning rates for rapid initial learning
        stdp_rate = 0.02
        hebbian_rate = 0.03
    elif development_stage == "CriticalPeriod":
        # Moderate learning with guided exploration
        stdp_rate = 0.01
        hebbian_rate = 0.02
    elif development_stage == "Maturation":
        # Lower learning rates for refinement
        stdp_rate = 0.005
        hebbian_rate = 0.01
    else:  // Adult
        // Very low learning rates for stability
        stdp_rate = 0.001
        hebbian_rate = 0.002
    
    // Update learning rates if the systems support it
    if brain.getSTDP():
        brain.getSTDP().setLearningRate(stdp_rate)
    
    if brain.getHebbian():
        brain.getHebbian().setLearningRate(hebbian_rate)
    
    return stdp_rate, hebbian_rate
```

### 3. Checkpoint Management

```python
def efficient_checkpointing(brain, step, checkpoint_dir):
    """
    Manage checkpoints efficiently to balance memory usage and performance.
    """
    checkpoint_manager = brain.getCheckpointManager()
    
    // Determine checkpoint frequency based on step number
    if step % 500 == 0:
        // Regular checkpoints for intermediate states
        checkpoint_file = f"{checkpoint_dir}/checkpoint_step_{step}.bin"
        brain.save(checkpoint_file)
        print(f"Regular checkpoint saved: {checkpoint_file}")
    
    elif step % 1000 == 0:
        // Major checkpoints for significant milestones
        checkpoint_file = f"{checkpoint_dir}/checkpoint_milestone_{step}.bin"
        brain.save(checkpoint_file)
        print(f"Milestone checkpoint saved: {checkpoint_file}")
    
    // Clean up old checkpoints if there are too many
    checkpoints = checkpoint_manager.getRecentCheckpoints(5)
    if len(checkpoints) < checkpoint_manager.getMaxCheckpoints():
        // Keep some history
        checkpoint_manager.cleanupOldCheckpoints(3)
    else:
        // Clean up old checkpoints to save space
        checkpoint_manager.cleanupOldCheckpoints(2)
```

### 4. Visualization and Monitoring

```python
def setup_performance_monitoring(brain, interval=100):
    """
    Set up performance monitoring and visualization.
    """
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    import time
    
    // Data collection
    performance_data = {
        'steps': [],
        'spike_counts': [],
        'firing_neurons': [],
        'curiosity_levels': [],
        'development_stages': [],
        'planning_success': []
    }
    
    // Create performance monitoring figure
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle('NLM Performance Monitoring', fontsize=16)
    
    // Setup plots
    setup_performance_plots(axes, performance_data)
    
    def update_performance_plot(frame):
        // Update plots with latest data
        update_performance_plots(axes, performance_data)
        
        return axes
    
    // Create animation
    animation.FuncAnimation(
        fig, update_performance_plot, frames=500, interval=interval,
        blit=True, cache_frame_data=False
    )
    
    plt.show()
    
    return performance_data
```

---

## Advanced Machine Learning Applications

### 1. Transfer Learning Between Brains

```python
def transfer_learning_system(source_brain, target_brains):
    """
    Implement transfer learning where knowledge from a source brain
    is transferred to target brains for faster adaptation.
    """
    print("=== Transfer Learning System ===\n")
    
    for target_brain in target_brains:
        print(f"Transferring knowledge to target brain...")
        
        // Extract knowledge from source brain
        source_knowledge = extract_knowledge(source_brain)
        
        // Initialize target brain with source knowledge
        initialize_target_with_knowledge(target_brain, source_knowledge)
        
        // Fine-tune target brain
        fine_tune_brain(target_brain, source_brain)
        
        print(f"Target brain adaptation complete")
        print(f"  Initial performance: {get_performance(target_brain):.3f}")
        print(f"  Final performance: {get_performance(source_brain):.3f}")
        print(f"  Transfer efficiency: {calculate_transfer_efficiency(source_brain, target_brain):.3f}")
    
    return target_brains
```

### 2. Multi-Task Learning

```python
def multi_task_learning_system(task_configs):
    """
    Create a system that can learn multiple tasks simultaneously
    while avoiding catastrophic forgetting.
    """
    print("=== Multi-Task Learning System ===\n")
    
    // Create specialized brains for each task
    task_brains = []
    for task_id, config in enumerate(task_configs):
        brain = pynlm.createBrain(config)
        brain.initialize()
        task_brains.append({
            'task_id': task_id,
            'brain': brain,
            'performance_history': []
        })
    
    // Interleave training across tasks
    for epoch in range(100):
        print(f"\nEpoch {epoch + 1}")
        
        for task_info in task_brains:
            task_id = task_info['task_id']
            brain = task_info['brain']
            
            // Train on this task
            train_on_task(brain, task_id, steps_per_epoch=20)
            
            // Record performance
            performance = get_performance(brain)
            task_info['performance_history'].append(performance)
            
            print(f"  Task {task_id}: {performance:.3f}")
        
        // Balance training load
        balance_task_training(task_brains)
    
    return task_brains
```

### 3. Meta-Learning and Adaptation

```python
def meta_learning_system(task_variations):
    """
    Create a meta-learning system that can rapidly adapt to new tasks
    by learning how to learn.
    """
    print("=== Meta-Learning System ===\n")
    
    // Create base meta-learner
    meta_learner_config = create_meta_learner_config()
    meta_learner = pynlm.createBrain(meta_learner_config)
    meta_learner.initialize()
    
    // Create task variations for meta-training
    task_variations = generate_task_variations(task_variations)
    
    // Meta-train the learner
    for meta_epoch in range(50):
        print(f"\nMeta-epoch {meta_epoch + 1}")
        
        for task_id, task_config in enumerate(task_variations):
            // Create temporary task brain
            task_brain = pynlm.createBrain(task_config)
            task_brain.initialize()
            
            // Meta-learn how to adapt this task
            meta_learn_task_adaptation(meta_learner, task_brain, task_id)
            
            // Evaluate meta-learning performance
            adaptation_success = evaluate_meta_learning(meta_learner, task_brain)
            print(f"  Task {task_id}: adaptation success = {adaptation_success:.3f}")
    
    return meta_learner
```

---

## Conclusion

This comprehensive guide has covered advanced topics in the NLM framework, including:

### Core Advanced Topics
1. **Brain Development and Maturation** - Understanding how brains develop through different stages
2. **Neuromodulation and Learning Control** - How neuromodulators regulate behavior and learning
3. **Memory Systems Integration** - Working together with working, episodic, and associative memory
4. **Advanced Planning and Prediction** - Cognitive control, decision-making, and planning
5. **Developmental Trajectories** - How brains change over time

### Configuration and Optimization
1. **Performance-Optimized Configurations** - Balanced settings for different use cases
2. **Memory-Optimized Configurations** - Enhanced for memory-intensive tasks
3. **Developmental Configurations** - Focused on learning and adaptation
4. **Exploration-Optimized Configurations** - Designed for discovery and innovation
5. **Performance Optimization Guidelines** - Strategies for better efficiency

### Advanced Machine Learning Applications
1. **Transfer Learning** - Knowledge transfer between different brains
2. **Multi-Task Learning** - Simultaneous learning across multiple tasks
3. **Meta-Learning** - Learning how to learn more efficiently

### Practical Implementation
1. **Advanced Memory Management** - Optimizing memory usage and cleanup
2. **Learning Rate Scheduling** - Adaptive learning based on development stage
3. **Checkpoint Management** - Efficient state saving and loading
4. **Performance Monitoring** - Real-time visualization and analysis
5. **Visualization and Monitoring** - Tools for understanding brain behavior

These advanced techniques will help you build more sophisticated neural simulations and applications. The key is to understand both the underlying neuroscience principles and the computational implementation, then apply them appropriately to your specific use case.

Remember that the most effective NLM applications combine:
- **Biological plausibility** - Following principles observed in real brains
- **Computational efficiency** - Making the best use of available resources
- **Task-specific optimization** - Tuning parameters for your specific application
- **Robustness and generalizability** - Maintaining performance across different conditions

Happy experimenting! The boundaries of what's possible with NLM are limited only by your imagination and computational resources. 🧠✨

---

## References

- [Neural Engineering Principles](https://doi.org/10.1016/j.neuron.2021.05.012)
- [Computational Neuroscience](https://www.sciencedirect.com/science/article/pii/S0165027120301234)
- [Brain-Machine Interfaces](https://www.nature.com/articles/s41593-020-0621-1)
- [Spiking Neural Networks](https://www.springer.com/gp/book/9780387952008)
- [Learning Systems in Neuroscience](https://www.annualreviews.org/doi/10.1146/annurev-neuro-070918-050019)

---

*This guide is continuously being updated as new features are added to the NLM framework. For the latest documentation and examples, visit the official NLM documentation repository.*
