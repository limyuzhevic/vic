# NLM Phase 6 - Advanced Usage Guide

**Phase 6: Final Integration** - The comprehensive artificial brain integration

## Overview

Phase 6 represents the complete integration of all NLM brain systems into a coherent, functioning artificial intelligence architecture. This guide covers advanced usage patterns, API references, and practical examples for working with the fully integrated brain.

## Phase 6 Integration Architecture

Phase 6 brings together all previous phases into a unified system:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
    ↓                       ↓                    ↓
MEMORY / PREDICTION        MOTIVATION / NEUROMODULATION → ACTION SELECTION
    ↓                        ↓                    ↓
PLASTICITY / DEVELOPMENT → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

### Integrated Systems

1. **Memory Systems** - Working, Episodic, Semantic, Procedural, Associative Memory
2. **Neuromodulation** - Dopamine, Acetylcholine, Norepinephrine, Serotonin
3. **Development** - Synaptogenesis, Pruning, Maturation, Developmental System
4. **Cognition** - Concept Formation, Neural Planning, Attention (AttentionalSelection)
5. **Prediction** - Prediction System, Neural Prediction
6. **Core** - LIF neurons, spike propagation, plasticity, structural changes

## Advanced Python API

### Creating a Complete Phase 6 Brain

```python
import pynlm

# Create advanced configuration with all Phase 6 features
config = pynlm.createDefaultConfig()

# Enable all advanced features
config.set("neuron_count", 2000)           # Larger brain
config.set("region_count", 4)              # Multiple brain regions
config.set("connection_probability", 0.12) # Rich connectivity
config.set("enable_checkpointing", True)
config.set("enable_replay", True)
config.set("enable_development", True)
config.set("enable_plasticity", True)
config.set("enable_neuromodulation", True)

# Create the integrated brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent with full capabilities
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.setRandomSeed(12345)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulation systems
agent.enableRewardModulation(True)      # Dopamine system
agent.enableStructuralPlasticity(True)  # Synaptogenesis
agent.enableDevelopment(True)           # Brain development
agent.enableCuriosity(True)             # Exploration
agent.enableNoveltyDetection(True)      # New experience detection

print("Phase 6 integrated brain created!")
print(f"  - Neurons: {brain.getTotalNeuronCount()}")
print(f"  - Regions: {brain.getRegionCount()}")
print(f"  - Sensory input size: {agent.getSensoryInputSize()}")
print(f"  - Motor output size: {agent.getMotorOutputSize()}")
```

### Memory System Integration

```python
# Working Memory - temporary active information
wm = brain.getWorkingMemory() if hasattr(brain, 'getWorkingMemory') else None
if wm:
    # Store sensory patterns
    percept = world.getSensoryPercept()
    vision_data = percept.getVision()
    if vision_data:
        wm.store(vision_data, strength=0.8)  # Store visual pattern
    
    # Retrieve and use stored information
    retrieval = wm.retrieve(vision_data) if vision_data else 0.0
    print(f"Working memory retrieval strength: {retrieval}")

# Episodic Memory - experience recording
em = brain.getEpisodicMemory() if hasattr(brain, 'getEpisodicMemory') else None
if em:
    # After each episode, store the experience
    em.storeEpisode(percept)  # Store percept as episode
    
    # Later, retrieve recent episodes for replay
    recent = em.getRecentEpisodes(5)  # Get last 5 episodes
    print(f"Stored {len(recent)} episodes in episodic memory")

# Semantic Memory - learned knowledge
if hasattr(brain, 'getSemanticMemory'):
    sm = brain.getSemanticMemory()
    sm.storeFact("object_type_0", "resource")  # Learn object categories
    sm.storeFact("object_type_1", "hazard")
    
    # Retrieve learned knowledge
    obj_type = sm.retrieveFact("object_type_0")
    print(f"Known object type: {obj_type}")

# Associative Memory - relationships
if hasattr(brain, 'getAssociativeMemory'):
    am = brain.getAssociativeMemory()
    # Learn that resource objects are beneficial
    am.associate(object_type_0, resource_value, 0.9)
    
    # Retrieve associations
    related = am.getAssociations(object_type_0)
    print(f"Related concepts: {len(related)}")
```

### Neuromodulation Integration

```python
# Advanced neuromodulation usage
if hasattr(agent, 'getDopamine'):
    dopamine = agent.getDopamine()
    dopamine.signal_reward(1.0)  # Strong positive reinforcement
    dopamine.signal_reward_prediction_error(0.1)  # Small prediction error
    
    print(f"Dopamine level: {dopamine.getLevel()}")
    print(f"Plasticity factor: {dopamine.getPlasticityFactor()}")

# Curiosity-driven exploration
if hasattr(agent, 'getCuriosity'):
    curiosity = agent.getCuriosity()
    curiosity_level = curiosity.getLevel()
    print(f"Curiosity driving exploration: {curiosity_level}")
    
    # Use curiosity to explore novel areas
    novelty = agent.getNoveltyLevel() if hasattr(agent, 'getNoveltyLevel') else 0.0
    if novelty > 0.5:
        print("High novelty detected - triggering exploration")

# Attention system (neural attention)
if hasattr(brain, 'getAttention'):
    attention = brain.getAttention()
    # Focus on important sensory inputs
    attention.setInhibitionStrength(0.3)  # Lower inhibition = broader focus
    attention.setCompetitionThreshold(0.6)  # Higher threshold = more inclusive selection
    
    print(f"Attention focus regions: {len(attention.getAttendedRegions())}")
```

### Development and Plasticity

```python
# Brain development over time
if hasattr(agent, 'updateDevelopment'):
    # Simulate brain development over multiple episodes
    for episode in range(10):
        # Apply development updates
        agent.updateDevelopment(0.1)  # 100ms timestep
        
        # Get current developmental stage
        stage = agent.getDevelopmentalStage()
        stage_name = "Initial"
        if stage == 1:
            stage_name = "CriticalPeriod"
        elif stage == 2:
            stage_name = "Maturation"
        elif stage == 3:
            stage_name = "Adult"
        elif stage == 4:
            stage_name = "Aging"
            
        print(f"Episode {episode}: Developmental stage = {stage_name}")

# Plasticity learning
if hasattr(brain, 'enablePlasticity'):
    brain.enablePlasticity(True)
    # Brain will learn and adapt through experience
    # Synaptic connections will strengthen/weaken based on usage

print("Phase 6 brain development simulation complete")
```

### Prediction and Planning

```python
# Predictive capabilities
if hasattr(brain, 'getPredictionSystem'):
    prediction = brain.getPredictionSystem()
    # Get prediction error (how well predictions match reality)
    error = prediction.getPredictionError()
    print(f"Prediction error: {error}")
    
    # Get prediction confidence
    confidence = prediction.getConfidence()
    print(f"Prediction confidence: {confidence}")
    
    # Plan actions based on predictions
    if confidence > 0.7:
        print("High confidence - planning next actions")

# Neural planning
if hasattr(brain, 'getNeuralPlanner'):
    planner = brain.getNeuralPlanner()
    # Plan actions for reaching goals
    current_state = percept.getVision() if hasattr(percept, 'getVision') else []
    goal_state = [1.0, 1.0]  # Target position (x, y)
    
    planned_action = planner.planAction(current_state, goal_state)
    print(f"Planned action: {planned_action}")
```

### Advanced Simulation Loop

```python
def run_phase6_simulation(brain, world, agent, num_episodes, steps_per_episode):
    """Advanced Phase 6 simulation with all integrated systems"""
    
    total_reward = 0.0
    total_steps = 0
    
    for episode in range(num_episodes):
        print(f"\n=== Phase 6 Episode {episode} ===")
        
        # Reset environment
        world.reset()
        agent.reset()
        brain.reset()
        agent.updateDevelopment(0.0)  # Reset development
        
        # Enable curiosity for exploration
        agent.enableCuriosity(True)
        
        for step in range(steps_per_episode):
            # 1. Update world
            world.update(0.1)  # 100ms timestep
            
            # 2. Get sensory percept
            percept = world.getSensoryPercept()
            
            # 3. Process sensory input
            agent.processSensoryInput(percept)
            
            # 4. Brain thinks (multiple neural steps)
            for neural_step in range(10):
                brain.step(neural_step)
            
            # 5. Get motor command
            action = agent.decodeMotorCommand()
            
            # 6. Apply action to world
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # 7. Apply reward modulation
            prediction_error = agent.getPredictionError() if hasattr(agent, 'getPredictionError') else 0.0
            agent.applyRewardModulation(result.reward, prediction_error)
            
            # 8. Update development
            agent.updateDevelopment(0.1)
            
            # 9. Collect metrics
            total_reward += result.reward
            total_steps += 1
            
            # Print progress
            if step % 50 == 0:
                print(f"  Step {step}: Reward={result.reward:.2f}, "
                      f"Action={action}, Neuromod={agent.getNeuromodulationLevel()}")
        
        # Episode summary
        firing_rate = brain.getAverageFiringRate()
        memory_active = brain.getWorkingMemory().getActiveTraces() if hasattr(brain.getWorkingMemory(), 'getActiveTraces') else 0
        
        print(f"  Episode {episode} complete:")
        print(f"    Total reward: {total_reward/episode:.3f}")
        print(f"    Average firing rate: {firing_rate:.3f}")
        print(f"    Working memory traces: {memory_active}")
        print(f"    Developmental stage: {agent.getDevelopmentalStage()}")
    
    print(f"\n=== Phase 6 Simulation Complete ===")
    print(f"Total reward per step: {total_reward/total_steps:.5f}")
    print(f"Total simulation time: {total_steps} steps")
    
    return {
        'total_reward': total_reward,
        'total_steps': total_steps,
        'avg_reward_per_step': total_reward/total_steps
    }

# Run the advanced simulation
result = run_phase6_simulation(brain, world, agent, 10, 500)
print(f"Final result: {result}")
```

## Command Line Interface

### Enhanced nlm (Phase 6) Command Line

The Phase 6 integration demo (`./nlm`) includes advanced command line options:

```bash
# Basic usage
./nlm

# With configuration file
./nlm --config my_config.json

# Custom parameters
./nlm --neurons 5000 --steps 10000 --demo-only

# Help
./nlm --help
```

### Phase 6 Config Options

```bash
# Create custom config file (my_config.json):
{
    "neuron_count": 3000,
    "region_count": 4,
    "connection_probability": 0.15,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015
}
```

### Command Line Arguments for nlm_phase6_demo

```bash
# Integration verification only
./nlm_phase6_demo --verify-only

# Custom configuration
./nlm_phase6_demo --config custom_phase6.cfg

# Limited test mode
./nlm_phase6_demo --quick-test

# Full experiment with custom parameters
./nlm_phase6_demo --neurons 2000 --max-steps 5000
```

## Configuration Examples

### Advanced Configuration File Example

```json
{
    "simulation_timestep": 0.001,
    "random_seed": 12345,
    
    "neuron_config": {
        "neuron_count": 3000,
        "region_count": 4,
        "connection_probability": 0.15,
        "max_in-degree": 100
    },
    
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    
    "development": {
        "enable_development": true,
        "plasticity_modifier": 1.0,
        "critical_period_ratio": 0.3
    },
    
    "memory": {
        "working_memory_capacity": 100,
        "working_memory_decay": 0.95,
        "episodic_storage_limit": 1000,
        "semantic_recall_threshold": 0.7
    },
    
    "neuromodulation": {
        "dopamine_sensitivity": 1.0,
        "curiosity_drive": 0.5,
        "novelty_threshold": 0.3
    },
    
    "prediction": {
        "prediction_horizon": 5,
        "prediction_confidence_threshold": 0.6
    },
    
    "experiment": {
        "enable_checkpointing": true,
        "checkpoint_path": "./phase6_checkpoint.bin",
        "enable_replay": true,
        "replay_episodes": 10
    }
}
```

## Performance Tips

### Optimizing Phase 6 Simulation

```python
# Performance optimization tips for Phase 6

# 1. Memory management
brain.getWorkingMemory().setCapacity(200)  # Optimal capacity
brain.getWorkingMemory().setDecayRate(0.99)  # Appropriate decay

# 2. Development efficiency
agent.enableDevelopment(True)  # Enable developmental plasticity
agent.updateDevelopment(0.5)  # Use larger timesteps for efficiency

# 3. Neuromodulation tuning
agent.getDopamine().setLevel(0.5)  # Moderate dopamine for balanced learning

# 4. Prediction system optimization
if hasattr(brain, 'getPredictionSystem'):
    prediction = brain.getPredictionSystem()
    prediction.clearHistory()  # Clear old predictions to save memory
    prediction.train(percept)  # Train with new data

# 5. Attention management
if hasattr(brain, 'getAttention'):
    attention = brain.getAttention()
    attention.setInhibitionStrength(0.2)  # Moderate inhibition
    attention.setCompetitionThreshold(0.7)  # Selective attention
```

## Error Handling and Debugging

### Common Phase 6 Issues and Solutions

```python
# Error handling for Phase 6 systems
try:
    # Check if systems are properly initialized
    if brain.getWorkingMemory() is None:
        print("WARNING: Working memory not initialized")
        # Initialize with default configuration
        brain.getWorkingMemory().initialize(brain)
    
    if brain.getEpisodicMemory() is None:
        print("WARNING: Episodic memory not initialized")
        brain.getEpisodicMemory().initialize(brain)
    
    # Check neuromodulation systems
    if hasattr(agent, 'getDopamine') and agent.getDopamine() is None:
        print("WARNING: Dopamine system not available")
        # System may not be enabled - check configuration
        if not agent.isRewardModulationEnabled():
            agent.enableRewardModulation(True)
    
    # Validate simulation parameters
    if world.getWidth() <= 0 or world.getHeight() <= 0:
        raise ValueError("Invalid world dimensions")
        
except Exception as e:
    print(f"Phase 6 error: {e}")
    # Fallback to basic configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)  # Smaller for debugging
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Reverted to basic configuration for debugging")
```

## Integration Examples

### Example 1: Cognitive Development Simulation

```python
"""
Simulates how concepts emerge and develop through experience
"""
def simulate_concept_formation():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("enable_development", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Track concept formation
    concept_counts = {}
    
    for episode in range(20):
        world.reset()
        agent.reset()
        agent.enableCuriosity(True)
        
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes and may form concepts
            for neural_step in range(5):
                brain.step(neural_step)
            
            # Check concept formation (if available)
            if hasattr(brain, 'getConceptFormation'):
                concepts = brain.getConceptFormation().getConceptCount()
                concept_counts[episode] = concepts
                print(f"Episode {episode}: {concepts} concepts formed")
            
            # Take action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from experience
            reward = world.computeReward(brain) if hasattr(world, 'computeReward') else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    # Analyze concept development
    if concept_counts:
        initial = list(concept_counts.values())[0]
        final = list(concept_counts.values())[-1]
        print(f"Concept formation: {initial} -> {final} concepts")
        print(f"Growth: {((final-initial)/initial)*100:.1f}%")
    
    return concept_counts

# Run concept formation simulation
concept_development = simulate_concept_formation()
```

### Example 2: Memory Replay and Consolidation

```python
"""
Demonstrates how episodic memory replay reinforces learning
"""def simulate_memory_replay():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("enable_replay", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    episode_data = []
    
    for episode in range(10):
        # Collect episode data
        episode_start = world.getSimulationTime()
        episode_percepts = []
        episode_actions = []
        
        world.reset()
        agent.reset()
        
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            episode_percepts.append(percept)
            
            agent.processSensoryInput(percept)
            
            for neural_step in range(5):
                brain.step(neural_step)
            
            action = agent.decodeMotorCommand()
            episode_actions.append(action)
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 50 == 0:
                agent.updateDevelopment(0.1)
        
        episode_data.append({
            'episode': episode,
            'start_time': episode_start,
            'percepts': episode_percepts,
            'actions': episode_actions,
            'final_reward': world.computeReward(brain) if hasattr(world, 'computeReward') else 0.0
        })
        
        print(f"Completed episode {episode}")
    
    # Simulate memory consolidation and replay
    if hasattr(brain, 'getEpisodicMemory'):
        em = brain.getEpisodicMemory()
        episode_count = em.getEpisodeCount()
        print(f"Episodic memory contains {episode_count} episodes")
        
        # Get recent episodes for replay
        recent_episodes = em.getRecentEpisodes(3)
        print(f"Replaying {len(recent_episodes)} recent episodes")
        
        # Replay strengthens neural connections
        for episode_idx, episode in enumerate(recent_episodes):
            print(f"Replaying episode {episode_idx + 1}: {len(episode.neurons)} neural patterns")
            
            # Consolidation effect
            consolidation_strength = 0.8 ** (episode_idx + 1)  # Decreasing strength
            print(f"  Consolidation strength: {consolidation_strength:.2f}")
    
    return episode_data

# Run memory replay simulation
replay_data = simulate_memory_replay()
```

## Next Steps

### Advanced Phase 6 Features

1. **Multi-Agent Systems**: Implement multiple interacting agents
2. **Social Learning**: Add observation and imitation of other agents
3. **Communication**: Implement agent-to-agent communication
4. **Complex Environments**: Create richer, more complex world models
5. **Real-time Visualization**: Add real-time neural activity visualization
6. **Performance Monitoring**: Track and optimize simulation performance

### Research Applications

Phase 6 is particularly suited for:

- **Cognitive development studies**: How concepts emerge from neural dynamics
- **Memory research**: Episodic memory formation and replay mechanisms
- **Decision making**: How neuromodulation affects choice behavior
- **Learning optimization**: Finding optimal plasticity parameters
- **Artificial intelligence**: Creating adaptive, self-organizing systems

### Future Enhancements

- **Genetic algorithms**: Evolve brain architectures
- **Transfer learning**: Apply learned skills to new tasks
- **Meta-learning**: Learn how to learn more efficiently
- **Hierarchical processing**: Multiple levels of abstraction

Phase 6 provides a comprehensive framework for studying artificial developmental cognitive systems and implementing advanced neural architectures.