# NLM - Advanced User Guide

This guide covers advanced usage patterns, complex scenarios, and expert-level techniques for the NLM (Neural Learning Machine) framework.

## Advanced Configuration and Customization

### Custom Neuron Populations

```python
import pynlm

# Create custom brain configuration
config = pynlm.createDefaultConfig()

# Add custom neuron types with specific parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.08)

# Configure specific region properties
config.set("region_0.neuron_type", "Excitatory")
config.set("region_0.threshold", -50.0)
config.set("region_0.resting_potential", -65.0)

# Initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Memory System Configuration

```python
import pynlm

# Create complex memory configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Get memory systems for advanced manipulation
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

# Configure working memory capacity
working_memory.setCapacity(2000)

# Configure episodic memory parameters
episodic_memory.setMaxEpisodes(5000)

# Enable consolidation with custom threshold
episodic_memory.setConsolidationThreshold(0.7)

# Setup replay with custom parameters
episodic_memory.setReplayInterval(50)
```

## Complex Brain Dynamics

### Multi-Stage Development Simulation

```python
import pynlm
import time

def run_development_simulation(steps):
    """Run a simulation with multiple developmental stages"""
    
    # Configure for long-term development
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.development_mode", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.001)
    config.set("plasticity.structural.pruning_rate", 0.0001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with advanced neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Create world with complex environment
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    agent.initialize(world)
    
    print("Starting developmental simulation...")
    start_time = time.time()
    
    for step in range(steps):
        # Update environment
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get motor command and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward with prediction
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log developmental stage progression
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            print(f"Step {step}: Developmental stage = {stage}, "
                  f"Working memory traces = {working_memory.getActiveTraces()}")
    
    end_time = time.time()
    print(f"Development simulation completed in {end_time - start_time:.2f} seconds")
    
    return brain, agent, world
```

### Prediction and Planning Integration

```python
import pynlm

# Create brain with prediction capabilities
config = pynlm.createDefaultConfig()
config.set("brain.prediction.enabled", True)
config.set("brain.planning.enabled", True)
config.set("brain.attention.enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Get prediction and planning systems
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
attention = brain.getAttention()

# Configure prediction parameters
if prediction_system:
    prediction_system.setPredictionHorizon(10)
    prediction_system.setPredictionErrorThreshold(0.1)

# Configure planner for complex action sequences
if planner:
    planner.setPlanningDepth(5)
    planner.setActionSequenceLength(8)

# Configure attention system
if attention:
    attention.setInhibitionStrength(0.6)
    attention.setExcitationStrength(1.8)

print("Advanced prediction and planning systems configured")
```

## Neuromodulation and Learning

### Reward-Modulated Plasticity

```python
import pynlm
import numpy as np

def run_reward_modulated_learning(num_episodes):
    """Run learning with advanced reward modulation"""
    
    # Create configuration optimized for reward learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Learning statistics
    episode_rewards = []
    dopamine_levels = []
    curiosity_levels = []
    
    print("Starting reward-modulated learning...")
    
    for episode in range(num_episodes):
        episode_reward = 0.0
        steps_in_episode = 0
        max_steps = 1000
        
        while steps_in_episode < max_steps:
            # Update world
            world.update(0.1)
            
            # Get percept and process it
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(episode * max_steps + steps_in_episode)
            
            # Get action and execute
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate and apply reward
            internal_signals = percept.getInternal()
            reward = internal_signals[0] if internal_signals else 0.0
            
            # Use prediction error for more sophisticated learning
            prediction_error = agent.getPredictionError()
            predicted_reward = 0.1  # Simplified prediction
            
            agent.applyRewardModulation(reward, predicted_reward)
            
            episode_reward += reward
            steps_in_episode += 1
            
            # Break if agent is stuck
            if action.getType() == pynlm.ActionType.Wait and steps_in_episode > 100:
                break
        
        # Record statistics
        episode_rewards.append(episode_reward)
        dopamine_levels.append(agent.getNeuromodulationLevel())
        curiosity_levels.append(agent.getCuriosityLevel())
        
        # Log progress
        if episode % 10 == 0:
            avg_reward = np.mean(episode_rewards[-10:])
            print(f"Episode {episode}: Avg reward = {avg_reward:.2f}, "
                  f"Dopamine = {dopamine_levels[-1]:.3f}, "
                  f"Curiosity = {curiosity_levels[-1]:.3f}")
    
    # Analyze learning progress
    final_avg_reward = np.mean(episode_rewards[-100:])
    reward_improvement = (final_avg_reward - np.mean(episode_rewards[:100])) / np.mean(episode_rewards[:100]) * 100
    
    print(f"\nLearning completed!")
    print(f"Final average reward: {final_avg_reward:.2f}")
    print(f"Reward improvement: {reward_improvement:.1f}%")
    print(f"Dopamine variance: {np.var(dopamine_levels):.3f}")
    print(f"Curiosity variance: {np.var(curiosity_levels):.3f}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'rewards': episode_rewards,
        'dopamine': dopamine_levels,
        'curiosity': curiosity_levels,
        'final_performance': final_avg_reward,
        'improvement': reward_improvement
    }
```

## Advanced Usage Patterns

### Pattern: Hierarchical Memory System

```python
def hierarchical_memory_example():
    """Example of using memory systems in a hierarchical architecture"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access all memory systems
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    associative_mem = brain.getAssociativeMemory()
    
    # Create a hierarchical storage strategy
    for step in range(1000):
        # Short-term: Working memory
        if step % 10 == 0:
            working_mem.storeToNeuron(step, 0.5)  # Store recent patterns
        
        # Medium-term: Episodic memory (episodes every 50 steps)
        if step % 50 == 0 and step > 0:
            # Create episode from working memory contents
            episode = create_episode_from_working_memory(working_mem)
            episodic_mem.storeEpisode(episode)
        
        # Long-term: Associative memory consolidation
        if step % 200 == 0 and step > 0:
            consolidate_to_associative(working_mem, episodic_mem, associative_mem)
        
        # Execute brain step
        brain.step(step)
        
        # Periodically replay important memories
        if step % 100 == 0 and episodic_mem.getEpisodeCount() > 0:
            replay_important_memories(episodic_mem)
    
    return brain

def create_episode_from_working_memory(working_mem):
    """Create an episode from working memory traces"""
    # Implementation details...
    pass

def consolidate_to_associative(working_mem, episodic_mem, associative_mem):
    """Consolidate patterns to associative memory"""
    # Implementation details...
    pass

def replay_important_memories(episodic_mem):
    """Replay important memories for consolidation"""
    # Implementation details...
    pass
```

### Pattern: Developmental Learning

```python
def developmental_learning_example():
    """Example of developmental learning over time"""
    
    # Create brain with developmental parameters
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.development.initial_plasticity", 1.0)
    config.set("brain.development.critical_period_length", 1000)
    config.set("brain.development.adult_plasticity", 0.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for developmental learning
    agent = pynlm.createAgentBrain(brain)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25)
    world.reset()
    
    agent.initialize(world)
    
    # Track development stages
    developmental_stages = []
    plasticities = []
    
    print("Starting developmental learning simulation...")
    
    # Critical period phase (high plasticity)
    print("Phase 1: Critical Period (high plasticity)")
    for step in range(1000):
        brain.step(step)
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            developmental_stages.append(stage)
            plasticities.append(1.0)  # High plasticity
    
    # Maturation phase (decreasing plasticity)
    print("Phase 2: Maturation (decreasing plasticity)")
    for step in range(1000, 2000):
        brain.step(step)
        agent.updateDevelopment(0.1)
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            developmental_stages.append(stage)
            plasticities.append(0.5)  # Medium plasticity
    
    # Adult phase (stable plasticity)
    print("Phase 3: Adult (stable plasticity)")
    for step in range(2000, 3000):
        brain.step(step)
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            developmental_stages.append(stage)
            plasticities.append(0.2)  # Low plasticity
    
    print(f"Development completed. Final stage: {developmental_stages[-1]}")
    
    return brain, developmental_stages, plasticities
```

## Performance Optimization

### Batch Processing

```python
def batch_brain_processing(steps, batch_size=100):
    """Process brain steps in batches for better performance"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Pre-compute batch steps for potential optimization
    num_batches = (steps + batch_size - 1) // batch_size
    batch_metrics = []
    
    print(f"Processing {steps} steps in {num_batches} batches of {batch_size}")
    
    for batch in range(num_batches):
        start_step = batch * batch_size
        end_step = min(start_step + batch_size, steps)
        
        batch_start_time = time.time()
        
        # Process batch
        for step in range(start_step, end_step):
            brain.step(step)
        
        batch_end_time = time.time()
        batch_duration = batch_end_time - batch_start_time
        
        # Collect batch metrics
        metrics = {
            'batch': batch,
            'steps_processed': end_step - start_step,
            'duration': batch_duration,
            'steps_per_second': (end_step - start_step) / batch_duration,
            'avg_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        batch_metrics.append(metrics)
        
        print(f"Batch {batch}: {metrics['steps_processed']} steps "
              f"({metrics['steps_per_second']:.0f} steps/sec)")
    
    # Aggregate results
    total_duration = sum(m['duration'] for m in batch_metrics)
    total_steps = sum(m['steps_processed'] for m in batch_metrics)
    overall_throughput = total_steps / total_duration
    
    print(f"\nBatch processing completed!")
    print(f"Overall throughput: {overall_throughput:.0f} steps/sec")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain, batch_metrics
```

## Error Handling and Recovery

### Robust Simulation Wrapper

```python
def robust_simulation(config, max_steps, checkpoint_path=None):
    """Run simulation with error handling and recovery"""
    
    brain = pynlm.createBrain(config)
    
    try:
        # Initialize brain with error handling
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        print("Brain initialized successfully")
        
        for step in range(max_steps):
            try:
                # Run brain step with error handling
                brain.step(step)
                
                # Checkpoint periodically
                if checkpoint_path and step % 1000 == 0 and step > 0:
                    if not brain.save(checkpoint_path):
                        print(f"Warning: Failed to save checkpoint at step {step}")
                
                # Log progress occasionally
                if step % 500 == 0:
                    print(f"Step {step}/{max_steps} completed")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                
                # Attempt recovery if checkpoint available
                if checkpoint_path and step > 100:
                    print("Attempting recovery from checkpoint...")
                    try:
                        new_brain = pynlm.createBrain(config)
                        if new_brain.load(checkpoint_path):
                            new_brain.initialize()
                            brain = new_brain
                            print("Recovery successful")
                        else:
                            print("Recovery failed - checkpoint corrupted")
                    except Exception as recovery_error:
                        print(f"Recovery failed: {recovery_error}")
                else:
                    print("No recovery attempt - too early in simulation")
                
                # Continue or break based on error severity
                continue
        
        print(f"Simulation completed successfully after {max_steps} steps")
        
        # Final checkpoint
        if checkpoint_path:
            if brain.save(checkpoint_path):
                print(f"Final checkpoint saved to {checkpoint_path}")
            else:
                print("Warning: Failed to save final checkpoint")
        
        return brain
        
    except Exception as e:
        print(f"Fatal error: {e}")
        if checkpoint_path:
            try:
                brain.save(checkpoint_path + ".emergency")
                print(f"Emergency checkpoint saved to {checkpoint_path}.emergency")
            except Exception as save_error:
                print(f"Failed to save emergency checkpoint: {save_error}")
        
        raise
```

## Custom Integration Examples

### Connecting Brain to External Systems

```python
class ExternalSystemInterface:
    """Interface for connecting NLM brain to external systems"""
    
    def __init__(self, brain):
        self.brain = brain
        self.sensor_data = []
        self.control_commands = []
        self.performance_metrics = []
    
    def process_external_input(self, external_data):
        """Process data from external system"""
        # Convert external data to NLM sensory input format
        sensory_input = convert_to_sensory_input(external_data)
        
        # Inject into brain
        self.brain.receiveSensoryInput(sensory_input)
        
        # Record for analysis
        self.sensor_data.append((time.time(), external_data))
    
    def get_brain_output(self):
        """Get brain output for external system control"""
        # Get brain action
        action = self.brain.produceAction()
        
        # Convert to external control commands
        control_cmd = convert_to_external_command(action)
        self.control_commands.append((time.time(), control_cmd))
        
        return control_cmd
    
    def get_system_metrics(self):
        """Get performance metrics for external monitoring"""
        metrics = {
            'firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'working_memory_traces': self.brain.getWorkingMemory().getActiveTraces() if self.brain.getWorkingMemory() else 0,
            'energy': calculate_brain_energy(self.brain),
            'stability': calculate_stability(self.brain)
        }
        
        self.performance_metrics.append((time.time(), metrics))
        return metrics

def convert_to_sensory_input(data):
    """Convert external data to NLM sensory input"""
    # Implementation depends on data format
    pass

def convert_to_external_command(action):
    """Convert NLM action to external control command"""
    # Implementation depends on external system requirements
    pass

def calculate_brain_energy(brain):
    """Calculate brain energy consumption estimate"""
    # Implementation based on neuron activity
    pass

def calculate_stability(brain):
    """Calculate brain stability metrics"""
    # Implementation based on firing patterns and memory states
    pass

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create external interface
external_interface = ExternalSystemInterface(brain)

# Connect to external system (example)
for i in range(1000):
    # Get external sensor data
    external_data = get_sensor_data()  # External system specific
    
    # Process through NLM
    external_interface.process_external_input(external_data)
    brain.step(i)
    
    # Get control commands
    control_cmd = external_interface.get_brain_output()
    apply_external_control(control_cmd)  # External system specific
    
    # Log metrics periodically
    if i % 100 == 0:
        metrics = external_interface.get_system_metrics()
        print(f"Step {i}: Firing rate = {metrics['firing_rate']:.2f}, "
              f"Energy = {metrics['energy']:.2f}")
```

## Best Practices

### 1. Configuration Management

- Use environment variables for configuration where possible
- Implement configuration validation before initialization
- Log configuration settings for reproducibility
- Use checkpointing for long-running simulations

### 2. Memory Management

- Monitor working memory usage regularly
- Implement periodic consolidation to prevent memory overload
- Use episodic memory for experience replay and learning
- Balance between exploration and exploitation

### 3. Error Handling

- Implement graceful degradation when components fail
- Use checkpointing for simulation recovery
- Monitor for instability and implement corrective actions
- Log all errors for debugging

### 4. Performance Optimization

- Profile your simulations to identify bottlenecks
- Use batch processing for large-scale simulations
- Consider hardware acceleration if available
- Optimize memory access patterns

### 5. Experiment Design

- Use proper randomization seeds for reproducibility
- Implement statistical significance testing for results
- Document all experimental parameters
- Use multiple trials for robust conclusions

## Next Steps

For users interested in extending NLM:

1. **Reading**: Consult `docs/ARCHITECTURE.md` for technical implementation details
2. **Experimentation**: Try different configurations in `configs/`
3. **Research**: Explore scientific papers referenced in `docs/SCIENCE.md`
4. **Extension**: Consider adding custom neuron models or plasticity rules
5. **Community**: Join the NLM research community for collaboration

This guide provides a foundation for advanced NLM usage. As the framework evolves, new patterns and capabilities will emerge. Always start with simple examples and gradually increase complexity as you gain confidence with the system.

```