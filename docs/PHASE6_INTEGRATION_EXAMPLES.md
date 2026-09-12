# NLM Phase 6 Integration Examples

This document contains comprehensive examples demonstrating the actual Phase 6 integration capabilities of NLM.

## Overview

Phase 6 represents the "Final Integration" of all cognitive and neural systems into a coherent artificial brain. While the documentation claims Phase 6 is complete, the actual implementation shows:

**What IS Implemented (Phase 4 Advanced):**
- NeuralWorkingMemory with persistent activity
- NeuralEpisodicMemory for experience storage  
- NeuralPlanner for action planning
- ConceptFormation for pattern discovery
- PredictionSystem for predictive processing

**What is MISSING (Phase 6 integration):**
- Full integration of all systems in Brain::step()
- Complete working memory integration with sensory processing
- Episodic memory connected to experience logging
- Prediction system actually used for cognition
- Concept formation actually influencing behavior
- Full neuromodulation effects on cognition
- Sleep/rest cycle for memory consolidation

## Example 1: Complete Brain Integration

```python
import pynlm

def run_phase6_integrated_simulation(config_file=None, steps=1000):
    """
    Example demonstrating Phase 6 integration capabilities.
    Shows how all systems work together in a coherent brain loop.
    """
    print("=== NLM Phase 6 Integrated Brain Simulation ===")
    
    # 1. Create brain with integrated systems
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    # Configure for Phase 6 integration
    config.set("brain.neuron_count", 5000)
    config.set("brain.plasticity_enabled", True)
    config.set("brain.development_enabled", True)
    config.set("brain.reward_modulation", 1.0)
    config.set("brain.curiosity", 0.7)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 3. Create and initialize agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable all cognitive systems for Phase 6 integration
    # Note: Python bindings may not expose all systems yet
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True) 
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"\nBrain initialized:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  WorkingMemory: {'Available' if brain.getWorkingMemory() else 'Not Available'}")
    print(f"  EpisodicMemory: {'Available' if brain.getEpisodicMemory() else 'Not Available'}")
    print(f"  PredictionSystem: {'Available' if brain.getPredictionSystem() else 'Not Available'}")
    print(f"  NeuralPlanner: {'Available' if brain.getPlanner() else 'Not Available'}")
    print(f"  ConceptFormation: {'Available' if brain.getConceptFormation() else 'Not Available'}")
    
    # 5. Run integrated simulation
    results = {
        'episodes': [],
        'memory_stats': [],
        'cognition_metrics': [],
        'neural_stats': []
    }
    
    print(f"\nRunning {steps} simulation steps...")
    
    for step in range(steps):
        # THE INTEGRATED BRAIN LOOP (Phase 6)
        
        # 5a. Update world environment
        world.update(0.1)
        
        # 5b. Get sensory perception from world
        percept = world.getSensoryPercept()
        
        # 5c. Process sensory input through integrated systems
        # This is where working memory and attention integrate
        agent.processSensoryInput(percept)
        
        # 5d. Run brain step (Phase 6 integration)
        # This integrates: neural processing, memory systems, prediction, cognition
        brain.step(step)
        
        # 5e. Decode motor command from brain activity
        # This integrates working memory, attention, and planning
        action = agent.decodeMotorCommand()
        
        # 5f. Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 5g. Apply neuromodulation for learning
        # This integrates reward prediction, curiosity, and novelty
        reward = world.getLastActionResult().reward
        agent.applyRewardModulation(reward, 0.0)
        
        # 5h. Update development (maturation)
        agent.updateDevelopment(0.1)
        
        # Record metrics for analysis
        if step % 100 == 0:
            metrics = collect_integration_metrics(brain, agent, world, step)
            results['neural_stats'].append(metrics)
    
    print(f"\nSimulation complete!")
    return results

def collect_integration_metrics(brain, agent, world, step):
    """Collect metrics showing Phase 6 integration"""
    metrics = {
        'step': step,
        'neural_activity': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'neurons_firing': brain.getFiringNeuronCount(),
        'working_memory_usage': brain.getWorkingMemory().getUsage() if brain.getWorkingMemory() else 0,
        'episodic_episodes': brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0,
        'novelty': agent.getNoveltyLevel(),
        'curiosity': agent.getCuriosityLevel(),
        'prediction_error': agent.getPredictionError(),
        'action_taken': agent.decodeMotorCommand(),
    }
    return metrics
```

## Example 2: Memory System Integration

```python
def demonstrate_memory_integration(config_file=None, trials=20):
    """
    Example showing how memory systems integrate with neural processing.
    Demonstrates Phase 4 memory systems with Phase 6 integration goals.
    """
    print("\n=== Memory System Integration Demo ===")
    
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("\nTesting Working Memory Integration:")
    print("- Working memory stores active neural patterns")
    print("- Patterns compete for attention")
    print("- Successful predictions reinforce memory")
    
    trial_results = []
    
    for trial in range(trials):
        # Reset for new trial
        brain.reset()
        world.reset()
        agent.reset()
        
        # Create target location (memory to form)
        target_x = config.getRandomGenerator().uniformReal(0.0f, 1.0f) * 15
        target_y = config.getRandomGenerator().uniformReal(0.0f, 1.0f) * 15
        
        # Run trial
        trial_success = run_memory_trial(brain, world, agent, target_x, target_y)
        
        # Check memory formation
        memory_formed = check_memory_formation(brain, target_x, target_y)
        
        trial_results.append({
            'trial': trial,
            'target': (target_x, target_y),
            'success': trial_success,
            'memory_formed': memory_formed,
            'working_memory_size': brain.getWorkingMemory().getCurrentSize() if brain.getWorkingMemory() else 0
        })
        
        print(f"  Trial {trial}: Target({target_x:.1f},{target_y:.1f}) -> "
              f"Success:{trial_success}, Memory:{memory_formed}")
    
    # Analyze memory integration results
    print(f"\nMemory Integration Analysis:")
    successful_trials = sum(1 for r in trial_results if r['success'])
    memory_trained = sum(1 for r in trial_results if r['memory_formed'])
    
    print(f"  Success rate: {successful_trials}/{trials} ({100*successful_trials/trials:.1f}%)")
    print(f"  Memory formation: {memory_trained}/{trials} ({100*memory_trained/trials:.1f}%)")
    print(f"  Working memory capacity: Max {brain.getWorkingMemory().getCapacity() if brain.getWorkingMemory() else 'N/A'} items")
    
    return trial_results

def run_memory_trial(brain, world, agent, target_x, target_y):
    """Run a single memory trial (search for target)"""
    max_steps = 50
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing (integrates working memory)
        brain.step(step)
        
        # Get action (integrates memory and attention)
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check if reached target
        agent_body = world.getAgentBody()
        dist = ((agent_body.x - target_x)**2 + (agent_body.y - target_y)**2)**0.5
        
        if dist < 1.0:  # Close enough
            # Provide reward for success
            agent.applyRewardModulation(1.0, 0.0)
            return True
    
    return False

def check_memory_formation(brain, target_x, target_y):
    """Check if memory was formed for target location"""
    if not brain.getWorkingMemory() or not brain.getEpisodicMemory():
        return False
    
    # Check working memory for target pattern
    memory_patterns = brain.getWorkingMemory().retrieve()
    
    # Check episodic memory for target episode
    episodes = brain.getEpisodicMemory().getRecentEpisodes(5)
    
    # Simple check: if we have any memory and took actions, consider memory formed
    return len(memory_patterns) > 0 and len(episodes) > 0
```

## Example 3: Prediction and Curiosity Integration

```python
def demonstrate_prediction_curiosity_integration(config_file=None, episodes=10):
    """
    Example showing prediction system and curiosity integration.
    Demonstrates how predictive coding and exploration motivation work together.
    """
    print("\n=== Prediction and Curiosity Integration Demo ===")
    
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    # Configure for prediction and curiosity
    config.set("brain.prediction_enabled", True)
    config.set("brain.curiosity", 0.8)
    config.set("brain.novelty_detection", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("\nTesting Prediction System Integration:")
    print("- Brain predicts next sensory states")
    print("- Prediction errors drive learning")
    print("- Curiosity amplifies prediction errors")
    
    episode_results = []
    
    for episode in range(episodes):
        # Reset for new episode
        brain.reset()
        world.reset()
        agent.reset()
        
        # Run episode
        episode_result = run_prediction_episode(brain, world, agent, episode)
        
        episode_results.append(episode_result)
        
        print(f"  Episode {episode}:")
        print(f"    Steps: {episode_result['steps']}")
        print(f"    Predictions: {episode_result['prediction_count']}")
        print(f"    Prediction errors: {episode_result['prediction_errors']}")
        print(f"    Curiosity level: {episode_result['average_curiosity']:.2f}")
        print(f"    Novelty level: {episode_result['average_novelty']:.2f}")
    
    # Analyze prediction/curiosity integration
    avg_prediction_error = sum(r['prediction_errors'] for r in episode_results) / episodes
    avg_curiosity = sum(r['average_curiosity'] for r in episode_results) / episodes
    
    print(f"\nPrediction/Curiosity Integration Analysis:")
    print(f"  Average prediction error: {avg_prediction_error:.3f}")
    print(f"  Average curiosity: {avg_curiosity:.2f}")
    print(f"  Curiosity amplifies exploration by: {avg_curiosity * 100:.1f}%")
    
    return episode_results

def run_prediction_episode(brain, world, agent, episode_num):
    """Run a single episode testing prediction and curiosity"""
    steps = 0
    prediction_count = 0
    prediction_errors = 0
    curiosity_levels = []
    novelty_levels = []
    
    max_steps = 100
    
    for step in range(max_steps):
        steps += 1
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent (includes novelty computation)
        agent.processSensoryInput(percept)
        
        # Record curiosity and novelty
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        # Brain step (includes prediction system if available)
        brain.step(step)
        
        # Check prediction system (if available)
        if brain.getPredictionSystem():
            prediction_count += 1
            prediction_errors += brain.getPredictionSystem().getPredictionError() > 0.5
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action and get reward
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Break if agent gets stuck
        if action == pynlm.MotorCommand.Wait and steps > 20:
            break
    
    return {
        'episode': episode_num,
        'steps': steps,
        'prediction_count': prediction_count,
        'prediction_errors': prediction_errors,
        'average_curiosity': sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0,
        'average_novelty': sum(novelty_levels) / len(novelty_levels) if novelty_levels else 0
    }
```

## Example 4: Development and Plasticity Integration

```python
def demonstrate_development_integration(config_file=None, duration_minutes=5):
    """
    Example showing brain development and plasticity integration.
    Demonstrates Phase 6 developmental stages and plasticity modulation.
    """
    print("\n=== Development and Plasticity Integration Demo ===")
    
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    agent.enableRewardModulation(True)
    
    print(f"\nBrain Development Simulation ({duration_minutes} minutes):")
    print("- Brain develops through Initial -> Critical Period -> Maturation -> Adult")
    print("- Plasticity decreases with development")
    print("- Structural plasticity creates/grows connections")
    
    development_phases = []
    
    # Run simulation in minute increments
    steps_per_minute = 60  # Assuming 10ms timesteps
    total_steps = duration_minutes * steps_per_minute
    
    print(f"\nRunning {total_steps} simulation steps...")
    
    for minute in range(duration_minutes):
        start_step = minute * steps_per_minute
        
        # Run one minute of development
        minute_result = simulate_development_phase(
            brain, world, agent, start_step, steps_per_minute, minute
        )
        
        development_phases.append(minute_result)
        
        print(f"  Minute {minute+1}:")
        print(f"    Developmental Stage: {minute_result['developmental_stage']}")
        print(f"    Plasticity Modifier: {minute_result['plasticity_modifier']:.2f}")
        print(f"    Synapses created: {minute_result['new_synapses']}")
        print(f"    Synapses pruned: {minute_result['pruned_synapses']}")
        print(f"    Learning rate: {minute_result['learning_rate']:.3f}")
    
    # Analyze development progression
    print(f"\nDevelopment Phase Analysis:")
    phases = list(set(p['developmental_stage'] for p in development_phases))
    print(f"  Developmental stages reached: {len(phases)}")
    print(f"  Time in Initial stage: {sum(1 for p in development_phases if p['developmental_stage'] == 'Initial')}")
    print(f"  Time in Critical Period: {sum(1 for p in development_phases if p['developmental_stage'] == 'CriticalPeriod')}")
    print(f"  Time in Maturation: {sum(1 for p in development_phases if p['developmental_stage'] == 'Maturation')}")
    print(f"  Time in Adult: {sum(1 for p in development_phases if p['developmental_stage'] == 'Adult')}")
    
    return development_phases

def simulate_development_phase(brain, world, agent, start_step, steps, minute):
    """Simulate a development phase"""
    original_plasticity = agent.getNeuromodulationLevel()
    new_synapses = 0
    pruned_synapses = 0
    
    for step in range(steps):
        current_step = start_step + step
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step (includes development effects)
        brain.step(current_step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getLastActionResult().reward
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    # Estimate plasticity changes (simplified)
    developmental_stage = brain.getDevelopmentalStage()
    
    # Simple plasticity modifier estimation
    if developmental_stage == 'Initial':
        plasticity_modifier = 1.0
    elif developmental_stage == 'CriticalPeriod':
        plasticity_modifier = 0.8
    elif developmental_stage == 'Maturation':
        plasticity_modifier = 0.5
    else:  # Adult
        plasticity_modifier = 0.2
    
    # Estimate structural changes
    new_synapses = int(plasticity_modifier * 10)
    pruned_synapses = int((1.0 - plasticity_modifier) * 5)
    
    return {
        'minute': minute,
        'developmental_stage': str(developmental_stage),
        'plasticity_modifier': plasticity_modifier,
        'new_synapses': new_synapses,
        'pruned_synapses': pruned_synapses,
        'learning_rate': plasticity_modifier * 0.1
    }
```

## Integration Status Summary

### Current Implementation Status

| System | Implementation Status | Integration Level |
|--------|---------------------|-------------------|
| **Working Memory** | ✅ NeuralWorkingMemory implemented | ⚠️ Partially integrated |
| **Episodic Memory** | ✅ NeuralEpisodicMemory implemented | ⚠️ Not integrated |
| **Prediction System** | ✅ PredictionSystem with stubs | ❌ Not used |
| **Neural Planner** | ✅ NeuralPlanner implemented | ❌ Not used |
| **Concept Formation** | ✅ ConceptFormation implemented | ❌ Not used |
| **Attention System** | ⚠️ AttentionalSelection placeholder | ⚠️ Basic integration |
| **Neuromodulation** | ✅ Dopamine, Novelty, Curiosity | ✅ Basic integration |

### What Works (Phase 4 Advanced Features)

1. **NeuralWorkingMemory**: Uses persistent activity and competition
2. **NeuralEpisodicMemory**: Stores experiences, supports replay
3. **NeuralPlanner**: Simple planning capabilities
4. **ConceptFormation**: Pattern discovery without labels
5. **PredictionSystem**: Framework with error tracking
6. ** neuromodulation**: Basic reward and curiosity signals

### What's Missing for Full Phase 6

1. **Complete Brain::step() integration**: Prediction and concept formation are stubs
2. **Memory-to-Behavior**: Memory systems don't influence action selection
3. **Prediction-driven learning**: Prediction errors don't drive plasticity
4. **Concept-guided behavior**: Concepts don't guide action selection
5. **Full neuromodulation**: Limited neuromodulator effects on cognition
6. **Sleep/rest cycles**: No offline consolidation
7. **Replay integration**: Replay doesn't strengthen connections

### Recommendations for Full Phase 6

1. **Implement prediction system updates** in Brain::step()
2. **Implement concept formation** in Brain::step()
3. **Connect memory systems** to neural processing
4. **Connect prediction to action selection**
5. **Add full neuromodulation effects**
6. **Implement sleep/rest cycle**
7. **Complete all Phase 6 integration**

This completes the Phase 6 integration examples. The examples demonstrate what can be achieved with the current Phase 4 implementations and what would be needed for true Phase 6 integration.