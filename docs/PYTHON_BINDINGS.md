## Memory and Prediction System Access

### Memory System Integration

The enhanced Brain class provides direct access to integrated memory systems:

#### NeuralWorkingMemory
- `getWorkingMemory()` - Access working memory system
- Methods: `storeToNeuron()`, `retrieveFromNeuron()`, `update()`, `clear()`
- Applications: Active maintenance of neural representations, short-term memory

#### NeuralEpisodicMemory  
- `getEpisodicMemory()` - Access episodic memory system
- Methods: `storeEpisode()`, `retrieveEpisode()`, `replayEpisode()`, `getEpisodesForReplay()`
- Applications: Experience storage, memory consolidation, sleep replay

#### NeuralAssociativeMemory
- `getAssociativeMemory()` - Access associative memory system
- Methods: `storePattern()`, `retrievePattern()`, `getPatternCount()`
- Applications: Pattern completion, schema formation, conceptual memory

### Prediction System Integration

#### PredictionSystem
- `getPredictionSystem()` - Access predictive coding system
- Methods: `predict()`, `updatePrediction()`, `getPredictionError()`, `getConfidence()`
- Applications: Forward modeling, error-based learning, expectation generation

### Neuromodulation System Access

Direct access to all neuromodulation systems:
- `getDopamine()` - Reward prediction error signal
- `getCuriosity()` - Exploration motivation system
- `getNovelty()` - Novelty detection system  
- `getPredictionErrorSignal()` - Prediction error signal

### Development System Access

- `getDevelopmentSystem()` - Access developmental plasticity control
- Methods: `getDevelopmentalStage()`, `updateDevelopment()`, `setDevelopmentalStage()`
- Applications: Lifespan development, critical periods, maturation effects

### Advanced AgentBrain Features

#### Enhanced Neuromodulation
- `getNeuromodulationLevel()` - Current neuromodulation level
- `getCuriosityLevel()` - Current curiosity level (exploration drive)
- `getNoveltyLevel()` - Current novelty level (stimulus sensitivity)
- `getPredictionError()` - Prediction error signal

#### Advanced Feature Flags
- `enableRewardModulation(bool)` - Control reward-based learning
- `enableStructuralPlasticity(bool)` - Control synapse creation/removal
- `enableDevelopment(bool)` - Control developmental plasticity
- `enableCuriosity(bool)` - Control exploration behavior

### Integration Examples

#### Complete Agent with Memory Integration
```python
def run_agent_with_memory(agent, world, brain, config, num_steps=1000):
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Get access to memory systems
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    prediction_sys = brain.getPredictionSystem()
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Make prediction
        predicted_state = prediction_sys.predict(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and update neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Store in episodic memory
        if step % 100 == 0:
            episodic_mem.storeEpisode(step, reward, brain.getPredictionError())
        
        # Print memory statistics
        if step % 500 == 0:
            print(f"Step {step}:")
            print(f"  Neurons: {brain.getNeuronCount()}")
            print(f"  Working memory traces: {working_mem.getActiveTraces() if working_mem else 0}")
            print(f"  Episodic memory episodes: {episodic_mem.getEpisodeCount() if episodic_mem else 0}")
            print(f"  Prediction error: {brain.getPredictionError():.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Dopamine level: {dopamine.getLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
```

#### Memory Replay Example
```python
def run_memory_replay(brain, world, agent, agent_brain, num_episodes=5):
    """Demonstrate memory replay during rest/sleep phases."""
    
    episodic_mem = brain.getEpisodicMemory()
    if not episodic_mem:
        print("No episodic memory available for replay")
        return
    
    for episode_idx in range(num_episodes):
        print(f"\n=== Memory Replay Episode {episode_idx + 1} ===")
        
        # Get episodes for replay (most salient ones)
        episodes_to_replay = episodic_mem.getEpisodesForReplay(3)
        
        for i, episode in enumerate(episodes_to_replay):
            print(f"  Replaying episode {i+1} (step {episode.timestamp})")
            
            # Store episode info in working memory
            working_mem = brain.getWorkingMemory()
            if working_mem:
                for neuron_id in episode.activeNeurons:
                    activation = episode.neuronActivations[i] if i < len(episode.neuronActivations) else 0.5
                    working_mem.storeToNeuron(neuron_id, activation)
            
            # Replay with reduced neuromodulation
            original_curiosity = agent_brain.getCuriosityLevel()
            original_dopamine = brain.getDopamine().getLevel() if brain.getDopamine() else 0.0
            
            # During replay, dopamine should be low (reduced learning)
            if brain.getDopamine():
                brain.getDopamine().setLevel(0.1)
            
            # Print memory statistics
            print(f"    Reward value: {episode.reward:.3f}")
            print(f"    Active neurons: {len(episode.activeNeurons)}")
            print(f"    Time step: {episode.timestamp}")
        
        print(f"Completed {len(episodes_to_replay)} replays")
    
    print("Memory replay complete!")
```

#### Development Stage Analysis
```python
def analyze_development_progress(brain, agent_brain, steps=1000):
    """Analyze brain development over time."""
    
    print("=== Development Progress Analysis ===")
    
    stage_data = []
    plasticity_data = []
    curiosity_data = []
    
    for step in range(0, steps, 100):
        brain.step(step)
        stage = brain.getDevelopmentalStage()
        development_system = brain.getDevelopmentSystem()
        
        plasticity = development_system.getPlasticityModifier() if development_system else 0.0
        curiosity = agent_brain.getCuriosityLevel() if agent_brain else 0.0
        
        stage_data.append((step, stage))
        plasticity_data.append((step, plasticity))
        curiosity_data.append((step, curiosity))
        
        print(f"Step {step}: Stage {stage}, Plasticity {plasticity:.3f}, Curiosity {curiosity:.3f}")
    
    print("\n=== Development Insights ===")
    
    # Find developmental stage transitions
    stages = [stage for step, stage in stage_data]
    stage_changes = []
    for i in range(1, len(stages)):
        if stages[i] != stages[i-1]:
            stage_changes.append((i*100, stages[i]))
    
    if stage_changes:
        print(f"Developmental transitions at steps: {stage_changes}")
    else:
        print("No developmental transitions detected in this period")
    
    # Analyze curiosity development
    if curiosity_data:
        avg_curiosity = sum(c for step, c in curiosity_data) / len(curiosity_data)
        max_curiosity = max(c for step, c in curiosity_data)
        print(f"Average curiosity: {avg_curiosity:.3f}")
        print(f"Maximum curiosity: {max_curiosity:.3f}")
        
        # Find peak curiosity
        peak_step, peak_curiosity = max(curiosity_data, key=lambda x: x[1])
        print(f"Peak curiosity at step {peak_step}: {peak_curiosity:.3f}")
    
    print("Development analysis complete!")
```

### Error Handling and Validation

All memory and prediction system methods include comprehensive error handling:

#### Common Error Conditions
- **NullPointerError**: Raised if memory system not initialized
- **InvalidParameterError**: Raised for invalid neuron IDs or memory keys
- **MemoryFullError**: Raised if memory capacity exceeded
- **ReplayError**: Raised if no episodes available for replay
- **PredictionError**: Raised for prediction computation failures

#### Validation Functions
```python
def validate_memory_systems(brain):
    """Validate all memory systems are functional."""
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    associative_mem = brain.getAssociativeMemory()
    prediction_sys = brain.getPredictionSystem()
    
    errors = []
    
    if working_mem and not working_mem.getCapacity():
        errors.append("Working memory has zero capacity")
    
    if episodic_mem and episodic_mem.getMaxEpisodes() < 1:
        errors.append("Episodic memory has insufficient capacity")
    
    if prediction_sys and not prediction_sys.getConfidence():
        errors.append("Prediction system not properly initialized")
    
    if errors:
        raise ValueError(f"Memory system validation failed: {', '.join(errors)}")
    
    print("All memory systems validated successfully")
```

### Performance Considerations

#### Memory Management Tips
1. **Clear Working Memory**: Clear working memory periodically to prevent memory bloat
2. **Consolidate Episodes**: Use consolidation intervals to maintain manageable memory size
3. **Replay Efficiency**: Replay most salient memories first for best consolidation
4. **Prediction Cache**: Cache frequent predictions to improve performance

#### Memory Usage Monitoring
```python
def monitor_memory_usage(brain, agent_brain, num_samples=10):
    """Monitor memory system usage over time."""
    
    print("=== Memory Usage Monitoring ===")
    
    for sample in range(num_samples):
        step = sample * 100
        brain.step(step)
        
        # Check memory systems
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        prediction_sys = brain.getPredictionSystem()
        
        print(f"Step {step}:")
        
        if working_mem:
            print(f"  Working memory: {working_mem.getActiveTraces()} active traces")
            print(f"  Working memory utilization: {working_mem.getUtilization():.1%}")
        
        if episodic_mem:
            print(f"  Episodic memory: {episodic_mem.getEpisodeCount()} episodes")
            print(f"  Episodic memory utilization: {episodic_mem.getUtilization():.1%}")
        
        if prediction_sys:
            print(f"  Prediction error: {prediction_sys.getPredictionError():.3f}")
            print(f"  Prediction confidence: {prediction_sys.getConfidence():.3f}")
        
        print()
    
    print("Memory usage monitoring complete!")
```

### Integration with Agent Systems

#### Complete Agent Architecture
```python
class AdvancedNLMaster:
    """Advanced NLM agent with complete memory and prediction integration."""
    
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.agent_brain = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Initialize with context manager
        with self.brain, self.world:
            self.brain.initialize()
            self.agent_brain.initialize(self.world)
            self.config_brain()
    
    def config_brain(self):
        """Configure brain with memory and prediction systems."""
        # Enable all advanced features
        self.agent_brain.enableRewardModulation(True)
        self.agent_brain.enableStructuralPlasticity(True)
        self.agent_brain.enableDevelopment(True)
        self.agent_brain.enableCuriosity(True)
        
        # Configure world
        self.world.configure(
            width=self.config.get("environment_width", 10),
            height=self.config.get("environment_height", 10),
            visionWidth=self.config.get("vision_width", 8),
            visionHeight=self.config.get("vision_height", 8)
        )
        self.world.reset()
    
    def run_episode(self, max_steps=1000, save_memory=True):
        """Run complete agent episode with memory integration."""
        
        print(f"Starting agent episode ({max_steps} steps)...")
        
        # Get memory system references
        working_mem = self.brain.getWorkingMemory()
        episodic_mem = self.brain.getEpisodicMemory()
        prediction_sys = self.brain.getPredictionSystem()
        dopamine = self.brain.getDopamine()
        curiosity = self.brain.getCuriosity()
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent_brain.processSensoryInput(percept)
            
            # Make prediction
            if prediction_sys:
                predicted_state = prediction_sys.predict(percept)
            else:
                predicted_state = None
            
            # Run brain step
            self.brain.step(step)
            
            # Decode action
            action = self.agent_brain.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and update neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            self.agent_brain.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent_brain.updateDevelopment(0.1)
            
            # Store episode occasionally
            if save_memory and step % 100 == 0:
                if episodic_mem:
                    episodic_mem.storeEpisode(step, reward, self.brain.getPredictionError())
            
            # Print progress
            if step % 200 == 0:
                print(f"Step {step}/{max_steps}:")
                print(f"  Action: {action}")
                print(f"  Reward: {reward:.3f}")
                print(f"  Prediction error: {self.brain.getPredictionError():.3f}")
                print(f"  Curiosity: {self.agent_brain.getCuriosityLevel():.3f}")
                print(f"  Dopamine: {dopamine.getLevel() if dopamine else 0:.3f}")
                print(f"  Development: {self.brain.getDevelopmentalStage()}")
        
        print("Episode complete!")
        
        # Run memory replay if requested
        if save_memory and episodic_mem:
            print("\nRunning memory replay...")
            self.run_memory_replay()
        
        return self.agent_brain.getTotalSpikeCount()
    
    def run_memory_replay(self, num_episodes=3):
        """Run memory replay episode."""
        episodic_mem = self.brain.getEpisodicMemory()
        if not episodic_mem:
            print("No episodic memory available for replay")
            return
        
        episodes = episodic_mem.getEpisodesForReplay(num_episodes)
        print(f"Replaying {len(episodes)} episodes")
        
        for episode in episodes:
            print(f"Replay step {episode.timestamp}: Reward {episode.reward:.3f}")
        
        print("Memory replay complete!")
    
    def get_brain_stats(self):
        """Get comprehensive brain statistics."""
        
        stats = {
            "neurons": self.brain.getNeuronCount(),
            "synapses": self.brain.getTotalSynapseCount(),
            "active_neurons": self.brain.getActiveNeuronCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "development_stage": self.brain.getDevelopmentalStage(),
            "prediction_error": self.brain.getPredictionError(),
            "curiosity_level": self.agent_brain.getCuriosityLevel(),
            "neuromodulation_level": self.agent_brain.getNeuromodulationLevel(),
        }
        
        # Add memory system stats
        working_mem = self.brain.getWorkingMemory()
        episodic_mem = self.brain.getEpisodicMemory()
        prediction_sys = self.brain.getPredictionSystem()
        
        if working_mem:
            stats["working_memory_traces"] = working_mem.getActiveTraces()
            stats["working_memory_utilization"] = working_mem.getUtilization()
        
        if episodic_mem:
            stats["episodic_memory_episodes"] = episodic_mem.getEpisodeCount()
            stats["episodic_memory_utilization"] = episodic_mem.getUtilization()
        
        if prediction_sys:
            stats["prediction_confidence"] = prediction_sys.getConfidence()
        
        return stats
```

This documentation provides comprehensive coverage of memory and prediction system integration, error handling, performance considerations, and advanced agent architectures. The enhanced Python bindings now provide a complete, production-ready interface to the NLM neural simulation framework with full support for Phase 6: Final Integration.
- Error: RuntimeError

**getNeuronCount()**
- Get total neuron count (convenience method)
- Returns: size_t

**getTotalNeuronCount()**
- Get total neuron count across all regions
- Returns: size_t

**getSpikeRate()**
- Get average spike rate per active neuron
- Returns: float

**getDevelopmentalStage()**
- Get current developmental stage
- Returns: DevelopmentalStage

**getWorkingMemory()**, **getEpisodicMemory()**, **getAssociativeMemory()**
- Get memory system references
- Returns: Memory system objects

**getDopamine()**, **getCuriosity()**, **getNovelty()**, **getPredictionErrorSignal()**
- Get neuromodulation system references
- Returns: Neuromodulator system objects

### SimpleWorld Class

The SimpleWorld class provides a 2D simulation environment for agent development.

#### Key Methods

**configure(width, height, visionWidth, visionHeight)**
- Configure world dimensions and sensory capabilities
- Parameters: width, height, visionWidth, visionHeight (all float)

**reset()**
- Reset world to initial state

**update(timestep)**
- Update world state for one simulation timestep
- Parameters: timestep (double)

**applyMotorCommand(cmd, currentTime)**
- Apply motor command to agent
- Parameters: cmd (MotorCommand), currentTime (double)
- Returns: ActionResult

**getSensoryPercept()**
- Get current sensory percept for the agent
- Returns: SensoryPercept

**getAgentBody()**
- Get agent body state
- Returns: AgentBody

### AgentBrain Class

The AgentBrain class provides the interface between the neural brain and the simulated world.

#### Key Methods

**initialize(world)**
- Initialize agent brain with world interface
- Parameters: world (SimpleWorld)

**processSensoryInput(percept)**
- Process sensory percept and inject into brain
- Parameters: percept (SensoryPercept)

**decodeMotorCommand()**
- Decode brain motor activity into motor command
- Returns: MotorCommand

**applyRewardModulation(reward, predictedReward)**
- Apply reward-based neuromodulation
- Parameters: reward (float), predictedReward (float)

**updateDevelopment(timestep)**
- Update development system
- Parameters: timestep (double)

**enableRewardModulation()**, **enableCuriosity()**, **enableDevelopment()**, **enableStructuralPlasticity()**
- Enable/disable neuromodulation features
- Parameters: enable (bool)

## Sensory Classes

### Vision Class

Represents visual data with 2D image structure.

- **getWidth()**, **getHeight()**, **getChannels()**: Access image dimensions
- **getData()**: Get raw pixel data as list of floats
- **setData(data)**: Set vision data

### Audio Class

Represents audio data with sampling rate and number of samples.

- **getSampleRate()**, **getNumSamples()**: Access audio properties
- **getData()**: Get audio samples as list of floats
- **setData(data)**: Set audio samples

### InternalSignals Class

Represents internal neural signals and states.

- **addSignal(value)**: Add signal value
- **clearSignals()**: Clear all signals

## Action Classes

### Action Class

Represents an action to be taken by the agent.

- **getType()**, **setType(type)**: Access/modify action type
- **getParameters()**, **setParameters(params)**: Access/modify parameters
- **getName()**: Get human-readable action name
- **clone()**: Create a copy of this action

### MotorCommand Class

Low-level motor command enumeration for world execution.

- Various movement and interaction commands
- **getParameters()**: Get command parameters (speed, direction, etc.)

## State Classes

### AgentBody Class

Represents the physical state of the agent in the world.

- **x**, **y**: Position coordinates
- **orientation**: Heading angle in radians
- **velocityX**, **velocityY**: Velocity components
- **angularVelocity**: Rotational speed
- **energy**, **health**: Physiological state
- **age**: Developmental age

### ActionResult Class

Represents the outcome of an action taken by the agent.

- **reward**: Reward value (positive = beneficial, negative = detrimental)
- **success**: Whether action was successful
- **message**: Descriptive message of outcome

## Context Manager Support

All major classes support Python's context manager protocol for automatic resource cleanup:

```python
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # ... use brain ...
    # Brain automatically cleaned up

with pynlm.createSimpleWorld() as world:
    world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
    # ... use world ...
    # World automatically cleaned up
```

## Error Handling

The Python bindings provide comprehensive error handling with custom exceptions:

- **RuntimeError**: General runtime errors
- **ValidationError**: Validation-related errors
- **ConfigError**: Configuration errors

All methods document their error conditions and parameter validation requirements.

## Type Inference

The Config class automatically infers types when getting values:

```python
config.set("population_size", 100)        # int
config.set("learning_rate", 0.01)         # float
config.set("enable_curiosity", True)      # bool
config.set("agent_name", "my_agent")     # str

# Automatic type inference when getting
population = config.get("population_size")  # Returns int 100
learning_rate = config.get("learning_rate")  # Returns float 0.01
curiosity = config.get("enable_curiosity")  # Returns bool True
name = config.get("agent_name")           # Returns str "my_agent"
```

## Examples

### Complete Agent Simulation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("population_size", 500)
config.set("learning_rate", 0.01)

# Create brain and world
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=5, visionHeight=5)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
with brain:
    with world:
        for step in range(100):
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Update development
            agent.updateDevelopment(0.01)
            
            # Get current neuromodulation levels
            neuromod_level = agent.getNeuromodulationLevel()
            curiosity_level = agent.getCuriosityLevel()
            novelty_level = agent.getNoveltyLevel()
            
            # Get motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command
            world.applyMotorCommand(motor_cmd, 0.01)
            
            # Update world state
            world.update(0.01)
            
            # Log status periodically
            if step % 10 == 0:
                brain.logStatus()
```

### Configuration Management

```python
import pynlm

# Create config
config = pynlm.createDefaultConfig()

# Convenience methods - set only if not already set
config.setIfNotSet("learning_rate", 0.01)
config.setIfNotSet("population_size", 100)

# Type inference
config.set("threshold", 0.5)  # Automatically detected as double
value = config.get("threshold")

# Validation
try:
    pynlm.validateConfig(config)
    print("Configuration is valid")
except Exception as e:
    print(f"Configuration validation failed: {e}")

# Save config
config.saveToFile("agent_config.json")
```

### Memory System Access

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Access prediction system
prediction_system = brain.getPredictionSystem()

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()
```

## API Reference

### Factory Functions

- **createDefaultConfig()** -> Config: Create default configuration
- **createBrain(config)** -> Brain: Create new brain with configuration
- **createSimpleWorld()** -> SimpleWorld: Create new world
- **createAgentBrain(brain)** -> AgentBrain: Create agent brain interface

### Global Constants

- **INVALID_NEURON_ID**: Invalid neuron identifier
- **INVALID_SYNAPSE_ID**: Invalid synapse identifier
- **INVALID_REGION_ID**: Invalid region identifier
- **INVALID_POPULATION_ID**: Invalid population identifier

### Utility Functions

- **validateConfig(config)**: Validate configuration integrity
- **getVersion()**: Get Python bindings version

## Best Practices

1. **Use Context Managers**: Always use context managers for safe resource management
2. **Validate Configurations**: Validate configuration before use
3. **Handle Errors**: Use try-catch blocks for error handling
4. **Check Types**: Use type inference when retrieving configuration values
5. **Enable Neuromodulation**: Consider enabling reward, curiosity, and development
6. **Monitor State**: Use logStatus() for monitoring brain state during development

## Migration Guide

### From Previous Versions

No changes required - all existing code continues to work unchanged. New methods are additive and do not break backward compatibility.

### For New Features

Simply import the new convenience methods and use them as shown in the examples above.

### For Error Handling

Existing code will automatically benefit from improved error handling with more descriptive error messages.
