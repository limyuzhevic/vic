# NLM Python API Reference - AgentBrain Class

## Overview

The AgentBrain class provides the interface between the NLM neural system and its environment. It connects brain computation with sensory input and motor output, enabling intelligent behavior in simulated worlds.

## Key Features

- **Sensory Processing**: Integrates environmental data into neural activity
- **Motor Control**: Decodes neural activity into executable actions
- **Learning Systems**: Reward modulation, structural plasticity, development, curiosity
- **Neuromodulation**: Integrates multiple neuromodulators for adaptive behavior
- **World Interface**: Manages interaction with simulation environments
- **State Management**: Tracks agent state and development progress
- **Action Planning**: Plans and executes sequences of actions

## Class Definition

```python
agent = pynlm.createAgentBrain(brain)
```

## Core Methods

### Initialization and Setup

#### `agent.initialize(world)`
```python
agent.initialize(world)
```

**Description**: Initialize the agent brain with a world instance. Establishes the connection between neural computation and environmental interaction.

**Parameters**:
- `world` (SimpleWorld): The simulation world instance

**Returns**:
- None

**Example**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)  # Essential initialization
```

#### `agent.reset()`
```python
agent.reset()
```

**Description**: Reset the agent to its initial state. Clears sensory buffers, action history, and developmental progress.

**Parameters**:
- None

**Returns**:
- None

**Example**:
```python
agent.initialize(world)
# Run episode
agent.reset()  # Reset for new episode
```

### Sensory Processing

#### `agent.processSensoryInput(percept)`
```python
agent.processSensoryInput(percept)
```

**Description**: Process sensory input from the world and inject it into the brain's neural network. Converts environmental data into neural signals.

**Parameters**:
- `percept` (SensoryPercept): Sensory input containing vision, touch, internal, and proprioception data

**Returns**:
- None

**Example**:
```python
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### `agent.getSensoryInputSize()`
```python
input_size = agent.getSensoryInputSize()
```

**Description**: Get the size/dimension of expected sensory input.

**Parameters**:
- None

**Returns**:
- `input_size` (int): Number of sensory input dimensions

#### `agent.getMotorOutputSize()`
```python
output_size = agent.getMotorOutputSize()
```

**Description**: Get the size/dimension of motor output expected by the brain.

**Parameters**:
- None

**Returns**:
- `output_size` (int): Number of motor output dimensions

### Motor Control and Action Selection

#### `agent.decodeMotorCommand()`
```python
action = agent.decodeMotorCommand()
```

**Description**: Decode brain motor activity into executable motor command. Reads neural output and converts to action type and parameters.

**Parameters**:
- None

**Returns**:
- `action` (Action): Motor action with type and parameters

**Example**:
```python
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### `agent.getActionStats()`
```python
stats = agent.getActionStats()
```

**Description**: Get statistics about actions taken by the agent.

**Parameters**:
- None

**Returns**:
- `stats` (dict): Dictionary containing action statistics (type counts, timing, etc.)

### Learning Systems

#### `agent.enableRewardModulation(enable)`
```python
agent.enableRewardModulation(enable)
```

**Description**: Enable or disable reward modulation learning system.

**Parameters**:
- `enable` (bool): True to enable, False to disable

**Returns**:
- None

**Example**:
```python
agent.enableRewardModulation(True)  # Enable reward learning
agent.enableRewardModulation(False) # Disable reward learning
```

#### `agent.applyRewardModulation(reward, predicted_reward)`
```python
agent.applyRewardModulation(reward, predicted_reward)
```

**Description**: Apply reward-based neuromodulation to synaptic weights. Implements prediction error learning.

**Parameters**:
- `reward` (float): Actual reward received
- `predicted_reward` (float): Expected/predicted reward

**Returns**:
- None

**Example**:
```python
reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
agent.applyRewardModulation(reward, predicted_reward)
```

#### `agent.enableStructuralPlasticity(enable)`
```python
agent.enableStructuralPlasticity(enable)
```

**Description**: Enable or disable structural plasticity (synaptogenesis and pruning).

**Parameters**:
- `enable` (bool): True to enable, False to disable

**Returns**:
- None

**Example**:
```python
agent.enableStructuralPlasticity(True)  # Enable connection growth
agent.enableStructuralPlasticity(False) # Disable connection growth
```

#### `agent.enableDevelopment(enable)`
```python
agent.enableDevelopment(enable)
```

**Description**: Enable or disable developmental changes (age-dependent plasticity).

**Parameters**:
- `enable` (bool): True to enable, False to disable

**Returns**:
- None

**Example**:
```python
agent.enableDevelopment(True)  # Enable brain maturation
agent.enableDevelopment(False) # Freeze development
```

#### `agent.enableCuriosity(enable)`
```python
agent.enableCuriosity(enable)
```

**Description**: Enable or disable curiosity-driven exploration.

**Parameters**:
- `enable` (bool): True to enable, False to disable

**Returns**:
- None

**Example**:
```python
agent.enableCuriosity(True)  # Enable curiosity-driven exploration
agent.enableCuriosity(False) # Disable curiosity-driven exploration
```

#### `agent.updateDevelopment(timestep)`
```python
agent.updateDevelopment(timestep)
```

**Description**: Update developmental state based on elapsed time. Advances developmental stage and affects plasticity.

**Parameters**:
- `timestep` (float): Time elapsed since last update in milliseconds

**Returns**:
- None

**Example**:
```python
agent.updateDevelopment(100.0)  # Advance development by 100ms
```

### Neuromodulation and Cognitive Systems

#### `agent.getNeuromodulationLevel()`
```python
level = agent.getNeuromodulationLevel()
```

**Description**: Get current overall neuromodulation level.

**Parameters**:
- None

**Returns**:
- `level` (float): Neuromodulation level (0.0 to 1.0+)

#### `agent.getCuriosityLevel()`
```python
level = agent.getCuriosityLevel()
```

**Description**: Get current curiosity level.

**Parameters**:
- None

**Returns**:
- `level` (float): Curiosity level (0.0 to 1.0+)

#### `agent.getNoveltyLevel()`
```python
level = agent.getNoveltyLevel()
```

**Description**: Get current novelty detection level.

**Parameters**:
- None

**Returns**:
- `level` (float): Novelty level (0.0 to 1.0+)

#### `agent.getPredictionError()`
```python
error = agent.getPredictionError()
```

**Description**: Get current prediction error from forward model.

**Parameters**:
- None

**Returns**:
- `error` (float): Prediction error magnitude

#### `agent.getDevelopmentalStage()`
```python
stage = agent.getDevelopmentalStage()
```

**Description**: Get current developmental stage from agent's perspective.

**Parameters**:
- None

**Returns**:
- `stage` (DevelopmentalStage): Current developmental stage

### State Management

#### `agent.getAgentBody()`
```python
body = agent.getAgentBody()
```

**Description**: Get the agent's body state (position, velocity, etc.).

**Parameters**:
- None

**Returns**:
- `body` (AgentBody): Agent body state object

#### `agent.getBrain()`
```python
brain = agent.getBrain()
```

**Description**: Get the brain object associated with this agent.

**Parameters**:
- None

**Returns**:
- `brain` (Brain): Associated brain object

#### `agent.getWorld()`
```python
world = agent.getWorld()
```

**Description**: Get the world instance associated with this agent.

**Parameters**:
- None

**Returns**:
- `world` (SimpleWorld): Associated world object

### Action Planning and Control

#### `agent.planAction(action_type, parameters)`
```python
planned_action = agent.planAction(action_type, parameters)
```

**Description**: Plan a sequence of actions for goal-directed behavior.

**Parameters**:
- `action_type` (ActionType): Type of action to plan
- `parameters` (dict): Parameters for action planning

**Returns**:
- `planned_action` (list[Action]): Sequence of planned actions

#### `agent.executePlannedAction(planned_action)`
```python
agent.executePlannedAction(planned_action)
```

**Description**: Execute a previously planned action sequence.

**Parameters**:
- `planned_action` (list[Action]): Sequence of actions to execute

**Returns**:
- None

### Learning Statistics

#### `agent.isRewardModulationEnabled()`
```python
enabled = agent.isRewardModulationEnabled()
```

**Description**: Check if reward modulation is currently enabled.

**Parameters**:
- None

**Returns**:
- `enabled` (bool): True if reward modulation enabled, False otherwise

#### `agent.isStructuralPlasticityEnabled()`
```python
enabled = agent.isStructuralPlasticityEnabled()
```

**Description**: Check if structural plasticity is currently enabled.

**Parameters**:
- None

**Returns**:
- `enabled` (bool): True if structural plasticity enabled, False otherwise

#### `agent.isDevelopmentEnabled()`
```python
enabled = agent.isDevelopmentEnabled()
```

**Description**: Check if development is currently enabled.

**Parameters**:
- None

**Returns**:
- `enabled` (bool): True if development enabled, False otherwise

#### `agent.isCuriosityEnabled()`
```python
enabled = agent.isCuriosityEnabled()
```

**Description**: Check if curiosity is currently enabled.

**Parameters**:
- None

**Returns**:
- `enabled` (bool): True if curiosity enabled, False otherwise

#### `agent.getLearningStats()`
```python
stats = agent.getLearningStats()
```

**Description**: Get comprehensive learning statistics.

**Parameters**:
- None

**Returns**:
- `stats` (dict): Dictionary containing learning statistics

### Advanced Agent Capabilities

#### `agent.enableAllLearning(enable=True)`
```python
agent.enableAllLearning(enable=True)
```

**Description**: Enable or disable all learning systems at once.

**Parameters**:
- `enable` (bool): True to enable all learning, False to disable (default: True)

**Returns**:
- None

**Example**:
```python
agent.enableAllLearning(True)  # Enable all learning systems
agent.enableAllLearning(False) # Disable all learning systems
```

#### `agent.setLearningStrategy(strategy)`
```python
agent.setLearningStrategy(strategy)
```

**Description**: Set learning strategy for the agent.

**Parameters**:
- `strategy` (str): Learning strategy name (e.g., "curiosity", "reinforcement", "developmental")

**Returns**:
- None

**Example**:
```python
agent.setLearningStrategy("balanced")  # Use balanced learning approach
```

#### `agent.exportAgentState()`
```python
state = agent.exportAgentState()
```

**Description**: Export current agent state for saving or analysis.

**Parameters**:
- None

**Returns**:
- `state` (dict): Dictionary containing agent state

#### `agent.importAgentState(state)`
```python
agent.importAgentState(state)
```

**Description**: Import agent state from saved data.

**Parameters**:
- `state` (dict): Dictionary containing agent state

**Returns**:
- None

## Learning System Details

### Reward Modulation
Reward modulation in NLM implements prediction error learning similar to dopamine signaling in biological brains. Key features:

- **Temporal difference learning**: Updates based on reward prediction errors
- **Eligibility traces**: Maintains memory of recent synaptic modifications
- **Synaptic scaling**: Prevents runaway excitation/inhibition
- **Metaplasticity**: Adjusts learning rates based on recent rewards

### Structural Plasticity
Structural plasticity enables growth and elimination of synaptic connections:

- **Synaptogenesis**: Formation of new connections based on activity and curiosity
- **Pruning**: Elimination of unused or weak connections
- **Reorganization**: Dynamic rewiring based on learning experience

### Developmental Changes
Development in NLM affects multiple aspects of neural function:

- **Plasticity rate changes**: Critical period with high plasticity
- **Neuron properties**: Maturation of neural characteristics
- **Network architecture**: Self-organizing network structure
- **Behavioral strategies**: Age-appropriate behaviors

### Curiosity and Novelty
Curiosity-driven exploration balances exploitation and exploration:

- **Information gain**: Measures novelty of sensory input
- **Prediction error**: Drives exploration of unpredictable environments
- **Intrinsic motivation**: Self-generated exploration behavior
- **Adaptive exploration**: Adjusts based on learning progress

## Interaction Patterns

### Basic Agent Loop
```python
def basic_agent_loop(agent, world, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain computation
        # (Handled internally by agent)
        
        # 5. Get action from agent
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning-Agent Loop
```python
def learning_agent_loop(agent, world, num_steps):
    for step in range(num_steps):
        # Environment interaction
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Get action (brain computation happens internally)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning from experience
        reward = extract_reward_from_world(world)
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Update development
        agent.updateDevelopment(0.1)
```

### Exploration-Agent Loop
```python
def curiosity_agent_loop(agent, world, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Occasionally explore based on curiosity
        if agent.getCuriosityLevel() > 0.7 and random.random() < 0.1:
            # Take random action for exploration
            action = agent.get_random_safe_action()
        else:
            action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from experience
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
```

## Error Handling

### AgentBrain Errors

The AgentBrain class raises the following exceptions:

#### InitializationError
```python
raise InitializationError("AgentBrain requires a valid brain object")
```

**Description**: AgentBrain initialization failed due to invalid brain object.

#### WorldError
```python
raise WorldError("AgentBrain requires a valid world object")
```

**Description**: AgentBrain operation failed due to invalid world object.

#### LearningError
```python
raise LearningError("Cannot enable learning before agent is initialized")
```

**Description**: Learning operation failed due to initialization state.

#### ActionError
```python
raise ActionError("Cannot decode motor command before agent is initialized")
```

**Description**: Action decoding failed due to initialization state.

## Performance Considerations

### Memory Usage
- **AgentBrain overhead**: ~100-200 bytes
- **Action history**: Scales with simulation length
- **Learning buffers**: O(steps) for eligibility traces

### Computational Complexity
- **Sensory processing**: O(sensory_input_size)
- **Motor decoding**: O(brain_output_size)
- **Reward modulation**: O(synaptic_connections)
- **Development updates**: O(1) per timestep

### Optimization Strategies
1. **Batch processing**: Process multiple sensory inputs efficiently
2. **Action caching**: Cache frequently used actions
3. **Learning rate adjustment**: Dynamically adjust learning rates
4. **Memory management**: Clear unused learning buffers

## Compatibility Notes

### Forward Compatibility
- New learning systems added as optional parameters
- Default behavior preserved for existing methods
- New action types added as extensions

### Backward Compatibility
- Method signatures stable across minor versions
- Learning system enable/disable methods unchanged
- State export/import format standardized

### Platform Considerations
- Cross-platform compatible
- No platform-specific dependencies
- File I/O compatible with all platforms

## Example Usage Patterns

### Basic Agent
```python
import pynlm

# Create and initialize components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run basic agent loop
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning Agent
```python
import pynlm

def run_learning_agent(num_steps=1000):
    """Run agent with learning enabled"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning
    agent.enableAllLearning(True)
    
    # Run learning episode
    for step in range(num_steps):
        # Environment interaction
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning (simplified reward extraction)
        reward = 0.0  # Would come from world state
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Monitor progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return agent
```

### Exploration Agent
```python
import pynlm
import random

class CuriosityDrivenAgent(pynlm.AgentBrain):
    def __init__(self, brain):
        super().__init__(brain)
        self.exploration_history = []
        self.learned_preferences = {}
    
    def get_exploration_action(self):
        """Get action based on curiosity level"""
        curiosity = self.getCuriosityLevel()
        
        if curiosity < 0.3:
            # Low curiosity: exploit known behaviors
            return self.decodeMotorCommand()
        
        elif curiosity < 0.7:
            # Medium curiosity: explore nearby options
            return self.get_slightly_random_action()
        
        else:
            # High curiosity: explore randomly
            return self.get_random_safe_action()
    
    def get_slightly_random_action(self):
        """Get action with small amount of randomness"""
        # Get current action
        current_action = self.decodeMotorCommand()
        
        # Sometimes vary action slightly
        if random.random() < 0.2:
            # Switch to different action type
            action_types = list(pynlm.ActionType)
            different_type = random.choice([a for a in action_types if a != current_action.getType()])
            return pynlm.Action(different_type)
        
        return current_action

# Use custom curiosity-driven agent
custom_agent = CuriosityDrivenAgent(brain)
custom_agent.initialize(world)
```

### State Management Agent
```python
import pynlm

class StatefulAgent(pynlm.AgentBrain):
    def __init__(self, brain):
        super().__init__(brain)
        self.episode_count = 0
        self.max_episodes = 10
    
    def run_episodes(self):
        """Run multiple episodes with state management"""
        for episode in range(self.max_episodes):
            print(f"Episode {episode + 1}/{self.max_episodes}")
            
            # Run episode
            episode_stats = self.run_episode()
            
            # Save episode state
            self.saveEpisodeState(episode)
            
            # Adapt based on performance
            self.adaptBasedOnPerformance(episode_stats)
            
            self.episode_count += 1
    
    def run_episode(self):
        """Run a single episode"""
        # Reset for new episode
        self.reset()
        
        # Run simulation
        for step in range(100):
            # Simulate environment step
            pass  # Environment handling
            
            # Get action
            action = self.decodeMotorCommand()
            
            # Record episode statistics
            if step % 10 == 0:
                print(f"  Step {step}")
        
        return self.getEpisodeStats()
    
    def saveEpisodeState(self, episode):
        """Save episode state for later analysis"""
        state = self.exportAgentState()
        state['episode'] = episode
        state['timestamp'] = time.time()
        
        # Save to file or database
        import json
        with open(f"episode_{episode}.json", 'w') as f:
            json.dump(state, f, indent=2)
    
    def adaptBasedOnPerformance(self, episode_stats):
        """Adapt agent based on episode performance"""
        # Analyze performance metrics
        fitness = self.calculateFitness(episode_stats)
        
        # Adapt learning parameters based on fitness
        if fitness < 0.5:
            # Poor performance: increase exploration
            self.enableCuriosity(True)
        else:
            # Good performance: increase exploitation
            self.enableCuriosity(False)

# Use stateful agent
stateful_agent = StatefulAgent(brain)
stateful_agent.initialize(world)
stateful_agent.run_episodes()
```

## Future Extensions

### Planned Enhancements
1. **Advanced Action Planning**: Hierarchical action selection and planning
2. **Social Learning**: Agent-to-agent knowledge transfer
3. **Transfer Learning**: Generalization across tasks
4. **Meta-Learning**: Learning how to learn
5. **Self-Monitoring**: Real-time monitoring of agent health and performance

### Research Applications
- **Behavior analysis**: Studying action patterns and decision-making
- **Learning dynamics**: Understanding learning trajectories and strategies
- **Adaptive systems**: Building systems that adapt to changing environments
- **Human-agent interaction**: Developing interfaces for human-AI collaboration

## Conclusion

The AgentBrain class provides the essential bridge between neural computation and intelligent behavior in simulated environments. Its learning systems, neuromodulation, and action selection capabilities make it suitable for a wide range of applications from basic autonomous agents to complex learning systems.

For more information, see:
- `docs/USAGE/BASIC.md` - Basic usage examples
- `docs/USAGE/INTERMEDIATE.md` - Intermediate usage patterns
- `docs/USAGE/ADVANCED.md` - Advanced features and customization
- `examples/` - Complete working examples
- `docs/API_REFERENCE/BRAIN.md` - Brain class API reference
- `docs/API_REFERENCE/CONFIG.md` - Configuration class API reference

Welcome to the world of intelligent agent simulation!
