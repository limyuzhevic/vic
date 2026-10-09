# NLM - Advanced Usage Guide

This guide extends the basic usage with advanced examples that leverage the enhanced Python bindings.

## Advanced Memory System Usage

### Example: Working Memory for Goal Pursuit

```python
import pynlm

def goal_pursuit_agent(config, num_steps):
    """Agent that uses working memory to maintain goals."""
    
    # Create brain with all systems enabled
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with learning systems
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Enable memory access
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    
    # Main simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Store goal in working memory (maintains through simulation)
        if working_mem:
            working_mem.store("current_goal", [0.8, 0.2, 0.1])  # Example goal vector
            working_mem.store("step_count", float(step))
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Record experience in episodic memory
        if episodic_mem:
            episodic_mem.record({
                'step': step,
                'percept': percept,
                'action': action,
                'reward': reward,
                'goal': working_mem.retrieve("current_goal") if working_mem else None
            })
    
    return brain, agent, world, working_mem, episodic_mem
```

### Example: Associative Memory for Pattern Completion

```python
import pynlm

def pattern_completion_agent(config, num_steps):
    """Agent that uses associative memory for pattern completion."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(pynlm.createSimpleWorld()))
    
    # Enable associative memory access
    assoc_mem = brain.getAssociativeMemory()
    
    if assoc_mem:
        # Create associations between patterns
        # Example: Red apple -> ['red', 'round', 'sweet', 'fruit']
        red_pattern = [1.0, 0.0, 0.0, 0.0]  # Red
        apple_pattern = [0.0, 1.0, 0.0, 0.0]  # Apple
        sweet_pattern = [0.0, 0.0, 1.0, 0.0]  # Sweet
        
        assoc_mem.associate(red_pattern, sweet_pattern, 0.8)
        assoc_mem.associate(apple_pattern, sweet_pattern, 0.9)
        assoc_mem.associate(red_pattern, apple_pattern, 0.7)
        
        # Test pattern completion
        # Given "red_"
        red_partial = [0.8, 0.2, 0.0, 0.0]  # Partial red cue
        completed = assoc_mem.spreadActivation(red_partial, steps=3)
        print(f"Pattern completion: red -> {[completed]}")
    
    return brain, agent, assoc_mem
```

## Advanced Neuromodulation Control

### Example: Custom Reward System

```python
import pynlm

def custom_reward_agent(config, num_steps):
    """Agent with custom reward scaling and prediction errors."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.initialize(pynlm.createSimpleWorld())))
    
    # Access neuromodulators for fine control
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    pred_error = brain.getPredictionErrorSignal()
    
    # Configure neuromodulators for custom behavior
    if dopamine:
        dopamine.setLevel(2.0)  # Amplify reward signals
        
    if curiosity:
        curiosity.setLevel(0.8)  # High exploration drive
        curiosity.setNoveltyWeight(0.7)  # Weight novelty heavily
        curiosity.setPredictionErrorWeight(0.3)
        
    if novelty:
        novelty.setSensitivity(0.6)  # Medium novelty detection
        
    # Simulation loop with custom reward processing
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Custom reward with prediction error
        reward = 1.0 if action.getType() == pynlm.ActionType.Interact else 0.0
        predicted_reward = 0.5  # Could be based on internal model
        
        # Signal reward prediction error through dopamine
        if dopamine:
            error = reward - predicted_reward
            dopamine.signalRewardPredictionError(abs(error))
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return brain, agent, dopamine, curiosity, novelty, pred_error
```

### Example: Novelty-Driven Exploration

```python
import pynlm

def novelty_driven_exploration(config, num_steps):
    """Agent driven by novelty detection for exploration."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Setup novelty detection
    novelty = brain.getNovelty()
    curiosity = brain.getCuriosity()
    
    if novelty and curiosity:
        novelty.setSensitivity(0.8)
        curiosity.setLevel(0.9)
    
    # Track previous observations for novelty detection
    previous_observation = None
    exploration_episodes = []
    
    for step in range(num_steps):
        world.update(0.1)
        
        # Get current observation
        percept = world.getSensoryPercept()
        current_observation = percept.getVision() if percept else None
        
        # Detect novelty
        if novelty and previous_observation and current_observation:
            novelty.detectNovelty(current_observation, previous_observation)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Action based on curiosity level
        action = agent.decodeMotorCommand()
        
        # Increase novelty if action is exploratory
        if action.getType() == pynlm.ActionType.Look:
            if novelty:
                novelty.setSensitivity(min(1.0, novelty.getSensitivity() + 0.05))
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record exploration episode
        if novelty.getLevel() > 0.7:
            exploration_episodes.append({
                'step': step,
                'novelty': novelty.getLevel(),
                'action': action,
                'curiosity': curiosity.getLevel() if curiosity else 0.0
            })
        
        previous_observation = current_observation
    
    return brain, agent, novelty, curiosity, exploration_episodes
```

## Advanced Plasticity Control

### Example: Custom Plasticity Rule Configuration

```python
import pynlm

def plasticity_controlled_agent(config, num_steps):
    """Agent with direct control over plasticity rules."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableStructuralPlasticity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Get direct plasticity control
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    
    # Configure plasticity based on task requirements
    if stdp:
        # For temporally precise tasks, increase LTP weight
        stdp.setLTPWeight(0.02)
        stdp.setLTDWeight(0.018)
        stdp.setTimeConstant(20.0)  # Longer time window
        
    if hebbian:
        # For spatial tasks, increase Hebbian learning rate
        hebbian.setLearningRate(0.005)
        hebbian.setMaxWeight(2.0)
        
    if structural:
        # For dynamic environments, increase synaptogenesis
        structural.setSynaptogenesisRate(0.001)
        structural.setPruningRate(0.0001)
    
    # Simulation with adaptive plasticity
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Periodically adjust plasticity based on performance
        if step % 500 == 0 and step > 0:
            # Fine-tuning phase - reduce learning rates
            if stdp:
                stdp.setLearningRate(0.0001)
            if hebbian:
                hebbian.setLearningRate(0.001)
            
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record successful learning events
        if brain.getTotalSpikeCount() > 100:
            if structural:
                structural.setSynaptogenesisRate(0.002)  # Boost connection formation
    
    return brain, agent, stdp, hebbian, structural
```

### Example: Structural Plasticity for Brain Growth

```python
import pynlm

def growing_brain_simulation(config, num_steps):
    """Brain that grows new neurons and connections over time."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # Setup structural plasticity
    structural = brain.getStructuralPlasticity()
    
    # Track brain growth metrics
    growth_metrics = {
        'neurons_created': 0,
        'synapses_created': 0,
        'neurons_removed': 0,
        'synapses_removed': 0,
        'capacity_utilization': []
    }
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Record pre-operation metrics
        initial_neurons = brain.getTotalNeuronCount()
        initial_synapses = brain.getTotalSynapseCount()
        
        # Occasionally trigger structural changes
        if step % 100 == 0:
            if structural:
                # Create new neuron based on developmental stage
                new_neuron = structural.createNeuron(brain, pynlm.NeuronType.Excitatory)
                if new_neuron != pynlm.INVALID_NEURON_ID:
                    growth_metrics['neurons_created'] += 1
                    
                # Create new synapse between existing neurons
                if brain.getTotalNeuronCount() >= 2:
                    # Get random neurons for connection
                    neurons = brain.getRegionIds()  # Simplified - get all regions
                    if len(neurons) >= 2:
                        source_neuron = neurons[0].getNeuronCount() > 0 and neurons[0] or neurons[1]
                        dest_neuron = neurons[1] if neurons[0] != neurons[1] else neurons[2] if len(neurons) > 1 else neurons[0]
                        
                        new_synapse = structural.createSynapse(brain, source_neuron, dest_neuron, 0.5)
                        if new_synapse != pynlm.INVALID_SYNAPSE_ID:
                            growth_metrics['synapses_created'] += 1
        
        # Occasionally prune unused connections
        if step % 200 == 50 and step > 0:
            if structural:
                # Get current connectivity stats
                synapse_count = brain.getTotalSynapseCount()
                if synapse_count > 10:
                    # Simple pruning: remove weak connections
                    pruned = structural.setPruningRate(0.01)  # 1% pruning rate
                    if pruned:
                        growth_metrics['synapses_removed'] += 1
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record post-operation metrics
        growth_metrics['capacity_utilization'].append({
            'step': step,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'growth_rate': (brain.getTotalNeuronCount() - initial_neurons) / 100.0
        })
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return brain, agent, structural, growth_metrics
```

## Development System Control

### Example: Manual Development Phases

```python
import pynlm

def manual_development_agent(config, num_steps):
    """Agent with manual control over developmental stages."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Get development system for manual control
    dev = brain.getDevelopmentSystem()
    
    # Simulation with manual developmental control
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Manual developmental control based on task phase
        if step < 100:
            # Initial learning phase - high plasticity
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
            if dev:
                dev.setMaturationRate(0.05)
                dev.setCriticalPeriodDuration(80.0)
                
        elif step < 300:
            # Maturation phase - moderate plasticity
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
            if dev:
                dev.setMaturationRate(0.02)
                
        elif step < 500:
            # Adult phase - fine-tuning
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
            if dev:
                dev.setMaturationRate(0.01)
                
        else:
            # Aging phase - reduced plasticity
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Aging)
            if dev:
                dev.setMaturationRate(0.005)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development (even with manual stages)
        agent.updateDevelopment(0.1)
        
        # Trigger specific developmental events at certain stages
        if step % 150 == 0 and dev:
            dev.triggerPhaseTransition(brain.getDevelopmentalStage())
    
    return brain, agent, dev
```

### Example: Auto-Development with Human Oversight

```python
import pynlm

def auto_development_agent(config, num_steps):
    """Agent that auto-develops but allows human intervention."""
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=35, height=35, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # Get development system
    dev = brain.getDevelopmentSystem()
    curiosity = brain.getCuriosity()
    
    # Override counter for manual interventions
    human_override_count = 0
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Check for human intervention via curiosity level
        if curiosity and curiosity.getLevel() > 0.9:
            # High curiosity suggests exploration - allow manual intervention
            human_override_count += 1
            if human_override_count % 20 == 0:  # Every 20 curiosity spikes
                print(f"Step {step}: High curiosity detected - manual intervention possible")
                
                # Manual development adjustment
                if dev:
                    # Reduce critical period to speed up maturation
                    dev.setCriticalPeriodDuration(60.0)
                    
                # Manually trigger development
                dev.triggerPhaseTransition(brain.getDevelopmentalStage())
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 1.0 if action.getType() == pynlm.ActionType.Interact else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development with curiosity modulation
        if curiosity:
            # Curiosity affects developmental rate
            dev_rate = 0.1 * (1.0 + curiosity.getLevel() * 0.5)
            agent.updateDevelopment(dev_rate)
        else:
            agent.updateDevelopment(0.1)
    
    return brain, agent, dev, curiosity, human_override_count
```

## Integration Examples

### Complete Advanced Agent Example

```python
import pynlm

def complete_advanced_agent(config, num_steps):
    """Complete agent using all advanced Python bindings."""
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all systems
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Get all system access
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    assoc_mem = brain.getAssociativeMemory()
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    dev = brain.getDevelopmentSystem()
    
    # Advanced configuration
    if dopamine:
        dopamine.setLevel(1.5)  # Custom reward scaling
    if curiosity:
        curiosity.setLevel(0.8)  # High exploration
    if stdp:
        stdp.setLTPWeight(0.015)
        stdp.setLTDWeight(0.014)
    
    # Main simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get percept and store in working memory
        percept = world.getSensoryPercept()
        if working_mem:
            working_mem.store("current_state", percept.getVision())
            
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Store episode if memory systems available
        if episodic_mem:
            episodic_mem.record({
                'step': step,
                'percept': percept,
                'action': action,
                'reward': reward,
                'brain_state': brain.getAverageFiringRate()
            })
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: Firing rate {brain.getAverageFiringRate():.2f}, "
                  f"Curiosity {curiosity.getLevel() if curiosity else 0:.2f}, "
                  f"Novelty {dopamine.getLevel() if dopamine else 0:.2f}")
    
    # Replay important episodes
    if episodic_mem:
        important_episodes = episodic_mem.getRecentEpisodes(5)
        print(f"Replayed {len(important_episodes)} recent episodes")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'working_memory': working_mem,
        'episodic_memory': episodic_mem,
        'associative_memory': assoc_mem,
        'dopamine': dopamine,
        'curiosity': curiosity,
        'stdp': stdp,
        'hebbian': hebbian,
        'development': dev
    }
```

## Advanced API Usage Tips

### 1. Error Handling with Advanced Systems
```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Safely access memory systems
    working_mem = brain.getWorkingMemory()
    if working_mem:
        working_mem.store("key", [1.0, 0.5, 0.0])
        retrieved = working_mem.retrieve()
        
    # Safely access neuromodulators
    dopamine = brain.getDopamine()
    if dopamine:
        dopamine.setLevel(2.0)
        
except Exception as e:
    print(f"Error in advanced agent: {e}")
    # Graceful degradation or recovery strategy
```

### 2. Performance Optimization
```python
# Batch processing for efficiency
percepts = [world.getSensoryPercept() for _ in range(10)]
for percept in percepts:
    agent.processSensoryInput(percept)

# Cache frequently accessed values
brain_state = {
    'neuron_count': brain.getTotalNeuronCount(),
    'synapse_count': brain.getTotalSynapseCount(),
    'firing_rate': brain.getAverageFiringRate()
}

# Reuse neuromodulator instances for consistent behavior
if dopamine:
    dopamine.setLevel(1.5)  # Set once, use multiple times
```

### 3. Custom Integration Patterns
```python
def custom_integration(brain, agent, world):
    # Custom sensory preprocessing
    percept = world.getSensoryPercept()
    processed = [x * 0.5 for x in percept.getVision()]  # Normalize
    
    # Custom action post-processing
    action = agent.decodeMotorCommand()
    action_params = action.getParameters()
    action_params = [min(max(p, 0.0), 1.0) for p in action_params]  # Clamp
    
    return processed, action
```

---

## Next Steps for Advanced Users

When you're comfortable with these advanced examples:

1. **Read HOW_TO_USE.md** for detailed C++ build instructions and Python API reference
2. **Read docs/ARCHITECTURE.md** to understand system integration
3. **Experiment with different configurations** - the new bindings enable many possibilities
4. **Combine multiple advanced features** - e.g., use episodic memory + neuromodulation + plasticity control
5. **Create custom wrapper functions** - build your own high-level APIs for specific tasks

The advanced Python bindings now provide comprehensive access to NLM's memory, neuromodulation, plasticity, and development systems, enabling research-grade neural network simulations with user-friendly Python interfaces.