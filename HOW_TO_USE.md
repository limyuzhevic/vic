# NLM Integration and Advanced Usage Guide

**For users who want to go beyond basic examples and build sophisticated artificial brain systems**

This guide provides comprehensive examples of how to integrate and use the advanced NLM systems.

---

## Table of Contents

1. [Advanced Memory System Integration](#advanced-memory-system-integration)
2. [Neuromodulation Control](#neuromodulation-control)
3. [Prediction and Planning Integration](#prediction-and-planning-integration)
4. [Development and Maturation](#development-and-maturation)
5. [Sleep and Memory Consolidation](#sleep-and-memory-consolidation)
6. [Performance Optimization](#performance-optimization)
7. [Multi-Agent Systems](#multi-agent-systems)

---

## Advanced Memory System Integration

This section shows how to use the advanced memory systems (working memory, episodic memory, attention) in sophisticated ways.

### 1. Working Memory with Dynamic Capacity

```python
import pynlm

# Create brain with larger working memory
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # More neurons
brain = pynlm.createBrain(config)
brain.initialize()

# Set working memory capacity
working_memory = brain.getWorkingMemory()
if working_memory:
    working_memory.setCapacity(200)  # Large capacity
    working_memory.setDecayRate(0.02f)  # Faster decay for recent events
    working_memory.setInhibitionStrength(0.8f)  # Stronger competition

# Inject multiple patterns over time
for step in range(100):
    # Different patterns based on step
    pattern = [math.sin(step * 0.1) * 10.0f for _ in range(50)]
    
    # Store in working memory
    working_memory.store(pattern, 1.5f)  # Stronger activation
    
    brain.step(step)
    
    # Check working memory content
    if working_memory.getActiveTraces() > 0:
        print(f"Step {step}: {working_memory.getActiveTraces()} active traces")
        
        # Get the most active neurons
        active_neurons = working_memory.getWinners()
        print(f"  Most active neurons: {len(active_neurons)}")

print("Working memory test complete")
```

### 2. Episodic Memory with Replay and Consolidation

```python
import pynlm
import random

# Create brain with episodic memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

episodic_memory = brain.getEpisodicMemory()
if not episodic_memory:
    print("Episodic memory not available!")
    exit()

# Track memory statistics
memory_stats = []

# Run simulation with experience accumulation
for episode in range(20):
    # Create random sensory experience
    sensory_pattern = [
        random.uniform(0.0f, 1.0f) 
        for _ in range(100)  # 100-dimensional experience
    ]
    
    # Encode episode
    episodic_memory.storeEpisode(
        sensory_pattern=sensory_pattern,
        timestamp=brain.getTotalSpikeCount(),
        action=random.choice([pynlm.MotorCommand.MoveForward, 
                             pynlm.MotorCommand.TurnLeft,
                             pynlm.MotorCommand.Interact]),
        reward=random.uniform(0.0f, 1.0f)
    )
    
    # Store some in working memory for immediate processing
    working_memory = brain.getWorkingMemory()
    if working_memory:
        working_memory.store(sensory_pattern[:50], 2.0f)
    
    # Process steps
    for step in range(20):
        brain.step(step)
        
        # Occasionally trigger replay
        if step % 50 == 0 and step > 0:
            recent_episodes = episodic_memory.getEpisodesForReplay(3)
            if recent_episodes:
                print(f"  Replaying {len(recent_episodes)} episodes...")
    
    # Check memory stats
    stats = {
        'episode': episode + 1,
        'total_episodes': episodic_memory.getEpisodeCount(),
        'avg_reward': episodic_memory.getAverageReward(),
        'working_memory_traces': working_memory.getActiveTraces() if working_memory else 0
    }
    memory_stats.append(stats)
    
    print(f"Episode {episode + 1}: {stats['total_episodes']} episodes, " +
          f"avg reward {stats['avg_reward']:.3f}")

# Run consolidation
episodic_memory.consolidate(0.3f)  # Keep stronger memories

print("\nMemory consolidation complete")
print(f"Total episodes after consolidation: {episodic_memory.getEpisodeCount()}")
```

### 3. Attentional Selection for Focused Processing

```python
import pynlm

# Create brain with attention system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

attention = brain.getAttention()
if not attention:
    print("Attention system not available!")
    exit()

# Configure attention parameters
attention.setInhibitionStrength(0.6f)
attention.setExcitationStrength(1.8f)
attention.setCompetitionThreshold(0.4f)

# Simulate competitive attention
for step in range(100):
    # Create multiple sensory inputs to compete
    sensory_inputs = []
    for i in range(10):
        # Create different sensory patterns
        pattern = [math.sin(i * step * 0.05f) * 10.0f for _ in range(20)]
        sensory_inputs.append(pattern)
    
    # Process each input through attention
    for i, pattern in enumerate(sensory_inputs):
        # Convert pattern to neuron activations
        neuron_activations = []
        for j, value in enumerate(pattern):
            neuron_id = j + i * 1000  # Unique neuron IDs
            activation = value * 2.0f  # Scale activation
            neuron_activations.append((neuron_id, activation))
        
        # Apply bottom-up salience based on pattern strength
        for neuron_id, activation in neuron_activations:
            attention.applyBottomUpSalience(neuron_id, activation)
        
        # Apply top-down bias (simulating goal-directed attention)
        if i == 5:  # Focus on 6th input (e.g., looking at something specific)
            for neuron_id, _ in neuron_activations:
                attention.applyTopDownBias(neuron_id, 2.0f)
    
    brain.step(step)
    
    # Check attention winners
    winners = attention.getWinners()
    if winners:
        print(f"Step {step}: Attention focused on {len(winners)} neurons")
        
        # Use attention winners for action selection
        action_neurons = [n for n in winners if n < 1000]  # Filter motor-related neurons
        if action_neurons:
            print(f"  Suggested action based on attended neurons: {action_neurons[0]}")

print("Attention experiment complete")
```

---

## Neuromodulation Control

This section demonstrates advanced neuromodulation control and integration.

### 1. Curiosity-Driven Exploration

```python
import pynlm
import math

# Create agent with neuromodulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Configure neuromodulation parameters
agent.setCuriosityWeight(1.0f)  # How much novelty affects curiosity
agent.setPredictionErrorWeight(0.5f)  # How much prediction error affects curiosity

# Run curiosity-driven exploration
for step in range(200):
    # Get sensory input
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Record curiosity before processing
    curiosity_before = agent.getCuriosityLevel()
    
    brain.step(step)
    
    # Get action (curiosity will influence exploration)
    action = agent.decodeMotorCommand()
    
    # Check curiosity level after step
    curiosity_after = agent.getCuriosityLevel()
    
    # Apply reward and update development
    reward = world.computeReward(action)
    agent.applyRewardModulation(reward, 0.0f)  # No prediction for now
    
    agent.updateDevelopment(0.1)
    
    # Log curiosity dynamics
    if step % 20 == 0:
        novelty = agent.getNoveltyLevel()
        print(f"Step {step}: Curiosity {curiosity_before:.3f} -> {curiosity_after:.3f}, " +
              f"Novelty {novelty:.3f}, Reward {reward:.3f}")

print("Curiosity experiment complete")
```

### 2. Dopamine and Reward Integration

```python
import pynlm

# Create brain for reward learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Track reward prediction
predicted_rewards = []
actual_rewards = []

# Run reward learning experiment
for episode in range(50):
    # Create a world with predictable rewards
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Start with simple predictable pattern
    pattern_length = 10
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward (create predictable pattern)
        reward = 1.0f if (step + 1) % pattern_length == 0 else 0.0f
        
        # Simple prediction - predict next reward
        predicted_reward = 0.5f if (step + 1) % pattern_length == 0 else 0.0f
        
        predicted_rewards.append(predicted_reward)
        actual_rewards.append(reward)
        
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Log prediction error
        prediction_error = reward - predicted_reward
        if step % 20 == 0:
            dopamine = agent.getNeuromodulationLevel()
            print(f"Step {step}: PE={prediction_error:.3f}, DA={dopamine:.3f}, Reward={reward:.3f}")

# Analyze learning
print("\nReward Learning Analysis:")
print(f"Prediction Error (mean): {sum(predicted_rewards) / len(predicted_rewards):.3f}")
print(f"Prediction Error (actual): {sum(actual_rewards) / len(actual_rewards):.3f}")

# Calculate learning improvement
if len(predicted_rewards) > 10:
    # Compare first half to second half
    midpoint = len(predicted_rewards) // 2
    first_half_pe = abs(sum(predicted_rewards[:midpoint]) / midpoint - sum(actual_rewards[:midpoint]) / midpoint)
    second_half_pe = abs(sum(predicted_rewards[midpoint:]) / (len(predicted_rewards) - midpoint) - 
                         sum(actual_rewards[midpoint:]) / (len(actual_rewards) - midpoint))
    
    print(f"Prediction error improvement: {first_half_pe:.3f} -> {second_half_pe:.3f}")

print("Dopamine experiment complete")
```

---

## Prediction and Planning Integration

This section shows how to use the prediction system for planning and action selection.

### 1. Neural Prediction System

```python
import pynlm
import math

# Create brain with prediction system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

prediction_system = brain.getPredictionSystem()
if not prediction_system:
    print("Prediction system not available!")
    exit()

# Configure prediction system
prediction_system.setPredictionHorizon(10)
prediction_system.setPredictionAccuracy(0.8f)

# Track predictions and outcomes
predictions = []
actuals = []

# Create a simple world with predictable patterns
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run prediction experiment
for step in range(150):
    # Get sensory input
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Update prediction system
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Create simple prediction (based on recent patterns)
    if step >= 20:
        # Look back at last 20 steps to predict next
        recent_values = []
        for i in range(max(0, step - 19), step):
            val = percept[i % len(percept)] if percept else 0.0f
            recent_values.append(val)
        
        if recent_values:
            # Simple average prediction
            predicted_value = sum(recent_values) / len(recent_values)
            predictions.append(predicted_value)
            
            # Create actual outcome based on action
            actual_value = percept[0] if percept else 0.0f
            actuals.append(actual_value)
            
            # Feed into prediction system
            prediction_system.updatePrediction(predicted_value)
            prediction_system.computeError(actual_value, predicted_value)
            
            # Log prediction
            if step % 30 == 0:
                error = prediction_system.getError()
                confidence = prediction_system.getConfidence()
                print(f"Step {step}: Predicted {predicted_value:.3f}, Actual {actual_value:.3f}, " +
                      f"Error {error:.3f}, Confidence {confidence:.3f}")

# Analyze prediction accuracy
if predictions and actuals:
    pred_errors = [abs(p - a) for p, a in zip(predictions, actuals)]
    avg_error = sum(pred_errors) / len(pred_errors)
    max_error = max(pred_errors)
    
    print(f"\nPrediction Analysis:")
    print(f"Average prediction error: {avg_error:.3f}")
    print(f"Maximum prediction error: {max_error:.3f}")
    print(f"Total predictions: {len(predictions)}")

print("Prediction experiment complete")
```

### 2. Neural Planning Integration

```python
import pynlm

# Create brain with planning capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

planner = brain.getPlanner()
if not planner:
    print("Planner not available!")
    exit()

# Configure planner
planner.setPlanningDepth(5)
planner.setEvaluationWindow(20)

# Create world for testing
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run planning experiment
for episode in range(10):
    print(f"\nPlanning Episode {episode + 1}:")
    
    # Reset and reinitialize
    brain.reset()
    brain.initialize()
    
    # Collect some experience
    experience = []
    for step in range(50):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        experience.append({
            'step': step,
            'percept': percept,
            'action': action,
            'reward': world.computeReward(action)
        })
    
    # Use planner to evaluate different action sequences
    if planner and len(experience) > 10:
        # Extract state sequence
        state_sequence = [exp['percept'] for exp in experience[-20:]]
        
        # Generate and evaluate action sequences
        action_sequences = []
        for _ in range(5):  # Generate 5 different action sequences
            sequence = []
            for _ in range(5):  # 5 actions per sequence
                sequence.append(random.choice(list(pynlm.MotorCommand)))
            action_sequences.append(sequence)
        
        # Evaluate each sequence
        best_sequence = None
        best_score = -float('inf')
        
        for i, sequence in enumerate(action_sequences):
            score = planner.evaluateSequence(
                state_sequence,
                sequence,
                brain.getTotalSpikeCount(),
                brain.getAverageFiringRate()
            )
            
            print(f"  Sequence {i + 1}: Score {score:.3f}")
            
            if score > best_score:
                best_score = score
                best_sequence = sequence
        
        if best_sequence:
            print(f"  Best sequence selected with score: {best_score:.3f}")
            # Follow the best sequence
            for action in best_sequence:
                world.applyMotorCommand(action, world.getSimulationTime())

print("Planning experiment complete")
```

---

## Development and Maturation

This section demonstrates the developmental process and how it affects brain performance.

### 1. Developmental Stages Simulation

```python
import pynlm

# Create brain for development study
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Track development metrics
development_metrics = []

# Run development experiment
for time_unit in range(0, 2000, 100):  # 20 time units
    # Get current development stage
    stage = brain.getDevelopmentalStage()
    age = time_unit / 100.0  # Convert to minutes
    
    # Record metrics
    metrics = {
        'time': age,
        'stage': stage,
        'plasticity': brain.getBrain()->getStructuralPlasticity()->getPlasticityFactor(),
        'neuros': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'connections': brain.getTotalSynapseCount()
    }
    development_metrics.append(metrics)
    
    # Simulate time passing
    for step in range(10):
        brain.step(step)
    
    # Print development stage info
    print(f"Time {age:.1f} min: Stage {stage}, " +
          f"Plasticity {metrics['plasticity']:.3f}, " +
          f"Connections {metrics['connections']}")

# Analyze development
print("\nDevelopment Analysis:")
stages = [m['stage'] for m in development_metrics]
print(f"Development stages observed: {set(stages)}")

# Find when stage changed
for i in range(1, len(development_metrics)):
    if development_metrics[i]['stage'] != development_metrics[i-1]['stage']:
        print(f"Stage transition at time: {development_metrics[i]['time']:.1f} min")

print("Development experiment complete")
```

---

## Sleep and Memory Consolidation

This section shows advanced memory consolidation during sleep/rest phases.

### 1. Sleep-Wake Cycle with Memory Consolidation

```python
import pynlm

# Create brain for sleep experiment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

episodic_memory = brain.getEpisodicMemory()
working_memory = brain.getWorkingMemory()

if not episodic_memory or not working_memory:
    print("Memory systems not available!")
    exit()

print("Running sleep-wake memory consolidation experiment")

# Configure sleep parameters
replay_interval = 100  # Steps between replays
consolidation_interval = 500  # Steps between consolidations

# Run sleep cycles
for sleep_cycle in range(5):
    print(f"\nSleep Cycle {sleep_cycle + 1}:")
    
    # WAKE PHASE: Encode new memories
    print("  WAKE: Encoding new experiences")
    for step in range(replay_interval):
        # Inject random patterns to encode
        for i in range(5):
            brain.injectCurrentToNeurons(
                pynlm.NeuronType.Sensory, 
                random.uniform(20.0f, 50.0f)
            )
        
        brain.step(step)
        
        # Occasionally store in working memory
        if step % 10 == 0:
            working_memory.storeToNeuron(
                pynlm.NeuronId(step % 1000),  # Use step as neuron ID
                random.uniform(0.5f, 1.0f)   # Random activation
            )
    
    # SLEEP PHASE: Memory consolidation
    print("  SLEEP: Memory consolidation")
    
    # Reset activity (simulating sleep)
    brain.reset()
    brain.initialize()
    
    # Replay important episodes
    episodes_to_replay = episodic_memory.getEpisodesForReplay(3)
    for episode in episodes_to_replay:
        print(f"    Replaying episode with {len(episode.activeNeurons)} active neurons")
        episodic_memory.replayEpisode(episode)
    
    # Consolidate memories
    if brain.getTotalSpikeCount() % consolidation_interval == 0:
        print("    Consolidating weak memories")
        episodic_memory.consolidate(0.4f)  # Higher threshold during sleep
        
        # Check consolidation results
        episodes_before = episodic_memory.getEpisodeCount()
        print(f"    Episodes after consolidation: {episodes_before}")
    
    # WAKE PHASE 2: Process sleep replay
    print("  WAKE 2: Processing replay")
    for step in range(replay_interval // 2):
        brain.step(step)
        
        # Integrate replayed patterns
        if working_memory.getActiveTraces() > 0:
            print(f"    Working memory has {working_memory.getActiveTraces()} traces")

print("\nSleep experiment complete")
print(f"Total episodes encoded: {episodic_memory.getEpisodeCount()}")
print(f"Average episode reward: {episodic_memory.getAverageReward():.3f}")
```

---

## Performance Optimization

This section shows how to optimize and monitor brain performance.

### 1. Performance Monitoring

```python
import pynlm

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Performance metrics
performance_metrics = {
    'spike_counts': [],
    'memory_usage': [],
    'connection_changes': [],
    'time_stamps': []
}

# Run performance monitoring experiment
for step in range(500):
    # Record current performance
    metrics = {
        'step': step,
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'pending_spikes': brain.getPendingSpikeEventCount(),
        'memory_traces': brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
        'development_stage': brain.getDevelopmentalStage()
    }
    performance_metrics['spike_counts'].append(metrics['spike_count'])
    performance_metrics['memory_usage'].append(metrics['memory_traces'])
    performance_metrics['time_stamps'].append(step)
    
    # Process step
    brain.step(step)
    
    # Check for performance issues
    if metrics['pending_spikes'] > 1000:
        print(f"Step {step}: High spike queue ({metrics['pending_spikes']})")
    
    if metrics['firing_rate'] > 50.0f:
        print(f"Step {step}: High firing rate ({metrics['firing_rate']:.3f})")

# Analyze performance
print("\nPerformance Analysis:")
if performance_metrics['spike_counts']:
    avg_spikes = sum(performance_metrics['spike_counts']) / len(performance_metrics['spike_counts'])
    max_spikes = max(performance_metrics['spike_counts'])
    min_spikes = min(performance_metrics['spike_counts'])
    
    print(f"Spike counts - Average: {avg_spikes:.1f}, Max: {max_spikes}, Min: {min_spikes}")
    
    if max_spikes > avg_spikes * 3:
        print("  Warning: High spike variation detected!")

print("Performance monitoring complete")
```

---

## Multi-Agent Systems

This section shows how to create and manage multiple intelligent agents.

### 1. Competing Agents

```python
import pynlm

# Create multiple competing agents
agents = []
brains = []
worlds = []

print("Creating competing agents...")

for agent_id in range(3):
    # Create unique configuration for each agent
    config = pynlm.createDefaultConfig()
    config.set("random_seed", agent_id * 42)  # Different seeds
    config.set("neuron_count", 500)  # Same size brains
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Give each agent different characteristics
    agent.enableRewardModulation(True)
    agent.enableCuriosity(agent_id % 2 == 0)  # Some agents are curious
    
    agents.append(agent)
    brains.append(brain)
    worlds.append(world)

# Run competition
print("Running agent competition...")
for episode in range(5):
    print(f"\nEpisode {episode + 1}:")
    
    # Each agent runs independently
    for agent_id, (agent, brain, world) in enumerate(zip(agents, brains, worlds)):
        # Reset for fresh episode
        brain.reset()
        brain.initialize()
        
        # Run agent
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record performance
        reward = world.computeReward(action)
        curiosity = agent.getCuriosityLevel()
        novel = agent.getNoveltyLevel()
        
        print(f"  Agent {agent_id + 1}: Reward {reward:.3f}, " +
              f"Curiosity {curiosity:.3f}, Novelty {novel:.3f}")

# Compare agent performance
print("\nCompetition Results:")
total_rewards = []
for agent_id, agent in enumerate(agents):
    world = worlds[agent_id]
    total_reward = sum([world.computeReward(action) for action in action_history])  # Would need to track actions
    total_rewards.append(total_reward)

for agent_id, reward in enumerate(total_rewards):
    print(f"  Agent {agent_id + 1}: Total reward {reward:.3f}")

print("Multi-agent competition complete")
```

---

## Quick Reference

### Memory Systems

| Function | Code Example |
|----------|--------------|
| Get working memory | `brain.getWorkingMemory()` |
| Store pattern | `working_memory.store(pattern, strength)` |
| Get active traces | `working_memory.getActiveTraces()` |
| Get winners | `working_memory.getWinners()` |
| Store episode | `episodic_memory.storeEpisode(episode)` |
| Get episodes | `episodic_memory.getEpisodeCount()` |
| Get recent episodes | `episodic_memory.getRecentEpisodes(count)` |
| Replay episodes | `episodic_memory.replayEpisode(episode)` |

### Neuromodulation

| Function | Code Example |
|----------|--------------|
| Get dopamine | `brain.getDopamine()` |
| Get curiosity | `agent.getCuriosityLevel()` |
| Get novelty | `agent.getNoveltyLevel()` |
| Get prediction error | `agent.getPredictionError()` |
| Apply reward | `agent.applyRewardModulation(reward, predicted)` |

### Planning

| Function | Code Example |
|----------|--------------|
| Get planner | `brain.getPlanner()` |
| Evaluate sequence | `planner.evaluateSequence(sequence, ...)` |
| Set planning depth | `planner.setPlanningDepth(depth)` |

### Development

| Function | Code Example |
|----------|--------------|
| Get stage | `brain.getDevelopmentalStage()` |
| Get plasticity | `brain.getDevelopmentSystem()->getPlasticityFactor()` |
| Update development | `agent.updateDevelopment(timestep)` |

---

## Advanced Configuration

```python
# Advanced configuration examples

# 1. Memory-heavy configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 3000)
config.set("region_count", 3)
config.set("synaptogenesis_rate", 0.00005f)
config.set("pruning_rate", 0.000005f)

# 2. Learning-focused configuration
config2 = pynlm.createDefaultConfig()
config2.set("random_seed", 12345)
config2.set("stdp_ltp_weight", 0.03f)
config2.set("stdp_ltd_weight", 0.025f)
config2.set("curiosity_enabled", True)
config2.set("development_enabled", True)

# 3. Fast simulation configuration
config3 = pynlm.createDefaultConfig()
config3.set("simulation_timestep", 0.0005)  # Faster steps
config3.set("neuron_count", 1000)  # Fewer neurons
config3.set("max_steps", 1000)  # Limited runtime
```

---

## Next Steps

1. **Read Architecture Documentation**: `docs/ARCHITECTURE.md` for detailed system design
2. **Experiment**: Try different configurations and see how they affect behavior
3. **Add Agents**: Create multiple agents with different characteristics
4. **Monitor Performance**: Use the performance monitoring examples to track system health
5. **Study Development**: Run long simulations to observe developmental changes

---

## Troubleshooting Advanced Usage

**"Memory systems not available"**
- Check that you're using Phase 6 or later versions
- Ensure all necessary modules are compiled

**"Neuromodulation not working"**
- Verify that neuromodulator systems are properly initialized
- Check configuration flags for neuromodulation

**"Performance too slow"**
- Reduce neuron count for faster simulations
- Decrease simulation timestep
- Use fewer regions

**"Planning not finding good sequences"**
- Increase planning depth
- Extend evaluation window
- Add more training data

---

## Summary

This guide provides comprehensive examples for using and integrating advanced NLM systems. You now have the tools to:

1. **Use memory systems** for storage and retrieval
2. **Control neuromodulation** for intelligent behavior
3. **Apply prediction** for planning and decision-making
4. **Study development** for understanding learning processes
5. **Optimize performance** for efficient simulations
6. **Create multi-agent systems** for complex scenarios

Start with simple examples and gradually build up to more sophisticated integrations. The key is to understand how the different systems (memory, neuromodulation, prediction, development) interact to create intelligent behavior.

Happy experimenting with NLM!
