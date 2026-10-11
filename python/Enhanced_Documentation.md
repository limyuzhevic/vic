# NLM Python Bindings - Enhanced Documentation

## Overview

The NLM (Neural Learning Machine) Python bindings provide a comprehensive interface to the NLM C++ neural simulation framework. This enhanced documentation covers all the features available in the Python API, including the newly added advanced cognitive and memory systems.

## Installation and Setup

### Prerequisites

- Python 3.8+
- pip (Python package installer)
- C++ compiler with C++20 support

### Installation from Source

```bash
# Clone the NLM repository
https://github.com/nlm-project/nlm.git

cd nlm

# Build the Python bindings
pip install -e .

# Or install from the python directory
pip install -e python/
```

### Verification

```python
import pynlm

# Check version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Core API Reference

### Factory Functions

The Python bindings provide factory functions for easy creation of NLM systems:

```python
import pynlm as nlm

# Create core systems
config = nlm.create_default_config()
brain = nlm.create_brain(config)
agent = nlm.create_agent_brain(brain)
world = nlm.create_simple_world()

# Create advanced cognitive systems
planner = nlm.create_neural_planner()
concept_formation = nlm.create_concept_formation()
self_model = nlm.create_self_model()
social_learning = nlm.create_social_learning()
spatial = nlm.create_spatial_representation()
temporal = nlm.create_temporal_relation()
working_memory = nlm.create_neural_working_memory()
episodic_memory = nlm.create_neural_episodic_memory()
associative_memory = nlm.create_neural_associative_memory()
prediction_system = nlm.create_prediction_system()
```

### Brain Class

The Brain class is the central neural simulation engine:

```python
class Brain:
    def __init__(self, config=None):
        # Create brain with optional configuration
        self.brain = pynlm.create_brain(config)
    
    def initialize(self):
        # Initialize brain with configuration
        self.brain.initialize()
    
    def step(self, step_number, time=None):
        # Run one simulation step
        if time is None:
            self.brain.step(step_number)
        else:
            self.brain.step(step_number, time)
    
    def reset(self):
        # Reset brain state
        self.brain.reset()
    
    # Statistics and monitoring
    def get_total_neuron_count(self):
        return self.brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self):
        return self.brain.getTotalSynapseCount()
    
    def get_firing_neuron_count(self):
        return self.brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self):
        return self.brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self):
        return self.brain.getExcitationInhibitionRatio()
    
    # State persistence
    def save(self, filepath):
        self.brain.save(filepath)
    
    def load(self, filepath):
        self.brain.load(filepath)
    
    # Region management
    def add_region(self, name=""):
        return self.brain.addRegion(name)
    
    def get_region(self, region_id):
        return self.brain.getRegion(region_id)
    
    def get_region_count(self):
        return self.brain.getRegionCount()
    
    # Memory systems
    def get_working_memory(self):
        return self.brain.getWorkingMemory()
    
    def get_episodic_memory(self):
        return self.brain.getEpisodicMemory()
    
    def get_associative_memory(self):
        return self.brain.getAssociativeMemory()
    
    def get_prediction_system(self):
        return self.brain.getPredictionSystem()
    
    # Cognition systems
    def get_planner(self):
        return self.brain.getPlanner()
    
    def get_concept_formation(self):
        return self.brain.getConceptFormation()
    
    def get_attention(self):
        return self.brain.getAttention()
    
    # Development system
    def get_development_system(self):
        return self.brain.getDevelopmentSystem()
    
    def get_developmental_stage(self):
        return self.brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage):
        self.brain.setDevelopmentalStage(stage)
    
    # Neuromodulation systems
    def get_dopamine(self):
        return self.brain.getDopamine()
    
    def get_curiosity(self):
        return self.brain.getCuriosity()
    
    def get_novelty(self):
        return self.brain.getNovelty()
    
    def get_prediction_error_signal(self):
        return self.brain.getPredictionErrorSignal()
```

### Config Class

The Config class manages configuration parameters:

```python
class Config:
    def __init__(self):
        self.config = pynlm.create_default_config()
    
    def load_from_file(self, filepath):
        self.config.loadFromFile(filepath)
    
    def save_to_file(self, filepath):
        self.config.saveToFile(filepath)
    
    def has(self, key):
        return self.config.has(key)
    
    def get_keys(self):
        return self.config.getKeys()
    
    def set(self, key, value):
        self.config.set(key, value)
    
    # Common configuration options
    def set_neuron_count(self, count):
        self.config.set("brain.neuron_count", count)
    
    def set_region_count(self, count):
        self.config.set("brain.region_count", count)
    
    def set_connection_probability(self, probability):
        self.config.set("brain.connection_probability", probability)
```

### AgentBrain Class

The AgentBrain class connects the NLM brain to the world:

```python
class AgentBrain:
    def __init__(self, brain):
        self.agent = pynlm.create_agent_brain(brain)
    
    def initialize(self, world):
        self.agent.initialize(world)
    
    def process_sensory_input(self, percept):
        self.agent.processSensoryInput(percept)
    
    def decode_motor_command(self):
        return self.agent.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward, predicted_reward):
        self.agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep):
        self.agent.updateDevelopment(timestep)
    
    # Configuration
    def enable_reward_modulation(self, enable):
        self.agent.enableRewardModulation(enable)
    
    def enable_structural_plasticity(self, enable):
        self.agent.enableStructuralPlasticity(enable)
    
    def enable_development(self, enable):
        self.agent.enableDevelopment(enable)
    
    def enable_curiosity(self, enable):
        self.agent.enableCuriosity(enable)
    
    def enable_neural_planning(self, enable):
        self.agent.enableNeuralPlanning(enable)
    
    # Statistics
    def get_curiosity_level(self):
        return self.agent.getCuriosityLevel()
    
    def get_novelty_level(self):
        return self.agent.getNoveltyLevel()
    
    def get_prediction_error(self):
        return self.agent.getPredictionError()
    
    def get_neuromodulation_level(self):
        return self.agent.getNeuromodulationLevel()
    
    def get_developmental_stage(self):
        return self.agent.getDevelopmentalStage()
```

### SimpleWorld Class

The SimpleWorld class simulates a 2D environment:

```python
class SimpleWorld:
    def __init__(self):
        self.world = pynlm.create_simple_world()
    
    def configure(self, width, height, vision_width, vision_height):
        self.world.configure(width, height, vision_width, vision_height)
    
    def reset(self):
        self.world.reset()
    
    def update(self, timestep):
        self.world.update(timestep)
    
    def set_agent_start(self, x, y):
        self.world.set_agent_start(x, y)
    
    def set_random_seed(self, seed):
        self.world.setRandomSeed(seed)
    
    # Agent interaction
    def apply_motor_command(self, motor_command, current_time):
        self.world.applyMotorCommand(motor_command, current_time)
    
    # State access
    def get_sensory_percept(self):
        return self.world.getSensoryPercept()
    
    def get_agent_body(self):
        return self.world.getAgentBody()
    
    # World objects
    def add_object(self, obj):
        self.world.addObject(obj)
    
    def remove_object(self, x, y):
        self.world.removeObject(x, y)
    
    def is_valid_position(self, x, y):
        return self.world.isValidPosition(x, y)
```

### Enumerations

The Python bindings expose all NLM enumerations:

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

## Advanced System Examples

### Neural Planner Usage

The NeuralPlanner provides action planning through predictive dynamics:

```python
import pynlm as nlm

# Create planner and configure
planner = nlm.create_neural_planner()
planner.set_planning_depth(5)
planner.set_current_goal([0.8, 0.6, 0.0])

# Use current state to plan
state = [0.0, 0.0, 0.0]  # Example state vector
action = planner.plan_action(state)
print(f"Planned action: {action.get_name()}")

# Evaluate action sequences
candidate_actions = [nlm.ActionType.MoveForward, nlm.ActionType.TurnLeft]
candidate = planner.evaluate_sequence(candidate_actions, state)
print(f"Expected reward: {candidate.expected_reward}, confidence: {candidate.confidence}")

# Get planning statistics
print(f"Planning depth: {planner.get_planning_depth()}")
```

### Concept Formation

The ConceptFormation system discovers recurring patterns from experience:

```python
import pynlm as nlm

concept_system = nlm.create_concept_formation()
concept_system.set_formation_threshold(0.8)
concept_system.set_stability_threshold(0.6)

# Learn concepts from experiences
for pattern, features, reward in experiences:
    concept_id = concept_system.present_experience(pattern, features, reward, 0)
    if concept_id > 0:
        print(f"New concept discovered: {concept_id}")
        stability = concept_system.get_concept_stability(concept_id)
        print(f"Concept stability: {stability}")
        
        # Get concept prototype
        prototype = concept_system.get_concept_prototype(concept_id)
        print(f"Concept prototype: {prototype}")

# Get all discovered concepts
for concept_id in range(concept_system.get_concept_count()):
    concept = concept_system.get_concept(concept_id)
    print(f"Concept {concept_id}: {concept.categoryHint}")
```

### Memory Systems

#### Working Memory

Working memory maintains active information:

```python
import pynlm as nlm

working_mem = nlm.create_neural_working_memory()
working_mem.set_capacity(100)  # Maximum number of memory traces
working_mem.set_decay_rate(0.95)  # Decay factor per step

# Store information
working_mem.store([0.1, 0.5, 0.9, 0.3, 0.7])

# Retrieve current content
content = working_mem.retrieve()
print(f"Working memory content: {content}")

# Check memory statistics
print(f"Active traces: {working_mem.get_active_traces()}")
print(f"Capacity: {working_mem.get_capacity()}")
print(f"Activity level: {working_mem.get_memory_activity()}")
```

#### Episodic Memory

Episodic memory stores experience episodes:

```python
import pynlm as nlm

episodic_mem = nlm.create_neural_episodic_memory()
episodic_mem.set_max_episodes(1000)
episodic_mem.set_replay_interval(100)  # Replay every 100 steps

# Store an episode
episode = nlm.EpisodicMemoryItem()
episode.sensory_state = [0.1, 0.5, 0.9]
episode.action = nlm.ActionType.MoveForward
episode.reward = 1.0
episode.timestamp = 50

episodic_mem.store_episode(episode)

# Get recent episodes
recent_episodes = episodic_mem.get_recent_episodes(5)
print(f"Number of recent episodes: {len(recent_episodes)}")

# Trigger memory replay
if episodic_mem.should_replay():
    replay_episodes = episodic_mem.get_episodes_for_replay(3)
    for ep in replay_episodes:
        episodic_mem.replay_episode(ep)
```

### Self-Model

The SelfModel represents the agent's internal model of itself:

```python
import pynlm as nlm

self_model = nlm.create_self_model()

# Record self-actions to build self-model
def record_action_effect(action, before_state, after_state):
    self_model.record_self_action(action, before_state, after_state)

# Predict consequences of actions
predicted = self_model.predict_action_consequence(
    nlm.ActionType.MoveForward, 
    [0.0, 0.0, 0.0]
)
print(f"Predicted state: {predicted}")

# Get self-model confidence
confidence = self_model.get_self_model_confidence(nlm.ActionType.MoveForward)
print(f"Self-model confidence for MoveForward: {confidence}")

# Get preferred actions
state = [0.2, 0.8, 0.1]
preferred_action = self_model.get_preferred_action(state)
print(f"Preferred action in this state: {preferred_action.get_name()}")
```

### Social Learning

SocialLearning enables learning from other agents:

```python
import pynlm as nlm

social = nlm.create_social_learning()

# Record observations of other agents
for action, observer_state, resulting_state in observations:
    social.observe_agent_action(action, observer_state, resulting_state)

# Check if we can imitate actions
if social.can_imitate(nlm.ActionType.Interact):
    print("Can imitate Interact action")

# Get imitation action based on current state
current_state = [0.1, 0.3, 0.5]
imitation_action = social.get_imitation_action(current_state)
print(f"Imitation action: {imitation_action.get_name()}")

# Get social knowledge statistics
print(f"Observation count: {social.get_observation_count()}")
print(f"Has social knowledge: {social.has_social_knowledge()}")
```

### Prediction System

The PredictionSystem predicts future sensory states:

```python
import pynlm as nlm

prediction = nlm.create_prediction_system()

# Make prediction for next state
current_state = nlm.Vision(10, 10, 3)  # Example vision input
predicted_state = prediction.predict_next_state(current_state)
print(f"Predicted state type: {type(predicted_state)}")

# Get prediction error
error = prediction.get_prediction_error()
print(f"Prediction error: {error}")

# Get prediction confidence
confidence = prediction.get_confidence()
print(f"Prediction confidence: {confidence}")

# Train prediction model
prediction.train(current_state)
```

## Complete Integration Example

This example demonstrates how to integrate all NLM systems:

```python
import pynlm as nlm
import time

def create_complete_agent():
    """Create a complete agent with all NLM systems"""
    
    # 1. Create core systems
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    agent = nlm.create_agent_brain(brain)
    world = nlm.create_simple_world()
    
    # 2. Configure world
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    # 3. Initialize agent with world
    agent.initialize(world)
    
    # 4. Configure and initialize all advanced systems
    systems = {
        'NeuralPlanner': nlm.create_neural_planner(),
        'ConceptFormation': nlm.create_concept_formation(),
        'SelfModel': nlm.create_self_model(),
        'SocialLearning': nlm.create_social_learning(),
        'SpatialRepresentation': nlm.create_spatial_representation(),
        'TemporalRelation': nlm.create_temporal_relation(),
        'WorkingMemory': nlm.create_neural_working_memory(),
        'EpisodicMemory': nlm.create_neural_episodic_memory(),
        'AssociativeMemory': nlm.create_neural_associative_memory(),
        'PredictionSystem': nlm.create_prediction_system()
    }
    
    for system_name, system in systems.items():
        try:
            system.initialize(brain)
            print(f"✅ {system_name} initialized")
        except Exception as e:
            print(f"❌ {system_name} failed to initialize: {e}")
    
    # 5. Enable key subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    agent.enable_neural_planning(True)
    
    # Configure planner
    systems['NeuralPlanner'].set_planning_depth(5)
    
    return {
        'systems': systems,
        'brain': brain,
        'agent': agent,
        'world': world
    }

def run_agent_simulation(agent_config, num_steps=1000):
    """Run a complete agent simulation"""
    
    # Create agent
    components = create_complete_agent()
    brain = components['brain']
    agent = components['agent']
    world = components['world']
    systems = components['systems']
    
    # Run simulation
    print("Starting agent simulation...")
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process sensory input in brain
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decode_motor_command()
        
        # Apply motor command to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.get_sensory_percept().getInternal()[0] if world.get_sensory_percept().getInternal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Working memory traces: {systems['WorkingMemory'].get_active_traces()}")
    
    print(f"\nSimulation complete! Ran {num_steps} steps.")

# Run the simulation
if __name__ == "__main__":
    config = {}
    run_agent_simulation(config, 1000)
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working.capacity` | int | neuron_count/10 | Working memory capacity |
| `memory.working.decay` | float | 0.99 | Working memory decay factor |
| `memory.episodic.max_episodes` | int | 1000 | Maximum episodic memory episodes |
| `memory.episodic.replay_interval` | int | 100 | Steps between memory replays |
| `memory.associative.similarity_threshold` | float | 0.7 | Similarity threshold for associations |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.stages.critical_period.end` | float | 300.0 | End of critical period |
| `development.stages.maturation.end` | float | 900.0 | End of maturation phase |
| `development.plasticity.initial` | float | 1.0 | Initial plasticity factor |
| `development.plasticity.critical` | float | 0.8 | Critical period plasticity |
| `development.plasticity.maturation` | float | 0.5 | Maturation plasticity |
| `development.plasticity.adult` | float | 0.2 | Adult plasticity |

## Error Handling

The NLM Python bindings provide custom exception classes for better error handling:

```python
import pynlm as nlm

try:
    # Create and initialize brain
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except nlm.NLMError as e:
    print(f"NLM error: {e}")
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    
except nlm.MemoryError as e:
    print(f"Memory error: {e}")
    
except nlm.PlanningError as e:
    print(f"Planning error: {e}")
```

## Performance Tips

### For Faster Simulations

1. **Reduce simulation timestep**: Smaller timesteps require more steps but can provide smoother dynamics.

2. **Limit visualization**: Disable visualization if not needed for production runs.

3. **Configure appropriate region count**: Too many regions can slow down processing.

4. **Adjust plasticity parameters**: Reduce frequency of plasticity updates if not needed.

### For Memory Efficiency

1. **Configure working memory capacity**: Set appropriate capacity based on expected usage.

2. **Use episodic memory pruning**: Configure episodic memory to remove old episodes.

3. **Limit concept formation**: Set appropriate thresholds for concept stability.

### For Better Planning

1. **Adjust planning depth**: Deeper planning is more accurate but computationally expensive.

2. **Set confidence thresholds**: Higher thresholds reduce unnecessary replanning.

3. **Enable memory replay**: Memory replay can improve learning but adds computational overhead.

## Troubleshooting

### Common Issues

**Issue**: ImportError: No module named 'pynlm'
**Solution**: Install the Python bindings:
```bash
pip install pynlm
```

**Issue**: Segmentation fault when calling brain.step()
**Solution**:
1. Ensure `brain.initialize()` is called before `brain.step()`
2. Check configuration parameters
3. Verify all required systems are properly initialized

**Issue**: MemoryError when running long simulations
**Solution**:
1. Monitor memory usage with `import psutil; psutil.Process().memory_info()`
2. Configure appropriate memory system capacities
3. Consider using checkpointing for very long simulations

**Issue**: Poor performance with many neurons
**Solution**:
1. Reduce neuron count for testing
2. Use appropriate region configuration
3. Disable unnecessary subsystems
4. Optimize computer hardware

### Getting Help

1. **Documentation**: Check the NLM documentation at https://nlm.readthedocs.io
2. **Examples**: Run the example scripts in the repository
3. **Issue Tracker**: Report issues at https://github.com/nlm-project/nlm/issues
4. **Community**: Join the NLM discussion forum at https://discourse.nlm-project.org

## Conclusion

The NLM Python bindings provide a comprehensive interface to the NLM neural simulation framework, including all 12 core brain systems. The enhanced documentation and examples make it easy to:

- Create and configure complete NLM agents
- Integrate with external environments
- Implement sophisticated cognitive architectures
- Perform research in neural computation
- Develop advanced AI applications

The NLM Python bindings enable researchers and developers to build artificial brains that learn, develop, and adapt through experience, providing a platform for exploring the frontiers of artificial intelligence and computational neuroscience.
