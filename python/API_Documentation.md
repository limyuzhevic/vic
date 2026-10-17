# NLM Python Bindings - Comprehensive API Documentation

## Overview

NLM (Neural Learning Machine) Python bindings provide a comprehensive interface to an experimental artificial developmental brain system. The bindings enable users to create, simulate, and interact with neural networks using Python while leveraging the high-performance C++ core.

**Version**: 0.1.0  
**Authors**: NLM Authors  
**License**: MIT  
**Documentation**: Generated automatically from source code

## Quick Start

### Installation

```bash
pip install pynlm
# Or install from source
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent Example

```python
import pynlm

def run_complete_simulation(num_steps=1000):
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_simulation(1000)
print("Simulation complete!")
```

## Core API Reference

### Configuration System

#### `pynlm.Config` Class

```python
config = pynlm.Config()

# Set configuration values
config.set_int("brain.neuron_count", 1000)
config.set_float("brain.synapse_density", 0.1)
config.set_bool("learning.enabled", True)
config.set_string("brain.name", "MyBrain")

# Get configuration values
neuron_count = config.get_int("brain.neuron_count", 1000)
learning_rate = config.get_float("learning.rate", 0.001)
enabled = config.get_bool("learning.enabled", False)

# Check if key exists
if config.has("brain.neuron_count"):
    keys = config.getKeys()
    summary = config.summary()

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("config.json")
```

#### Configuration Helper Functions

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Apply presets
config.applyPreset("performance")
config.applyPreset("development")

# Validate configuration
validation = config.validate()
if not validation.valid:
    print(f"Validation error: {validation.errorMessage}")
    for warning in validation.warnings:
        print(f"Warning: {warning}")
```

### Brain Class

#### Basic Operations

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Get developmental stage
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Reset brain
brain.reset()

# Save/load brain state
brain.save("brain_checkpoint.bin")
brain.load("brain_checkpoint.bin")
```

#### Batch Operations

```python
# Run multiple steps efficiently
brain.runSteps(1000)

# Inject current to multiple neurons
neurons = [brain.getRegion(0).getNeurons()[0]]  # Get some neurons
brain.injectCurrentToNeuronsBatch(neurons, 5.0)

# Get regional statistics
region_stats = brain.getRegionStats(brain.getRegion(0))
print(f"Region {region_stats.id}: {region_stats.neuronCount} neurons")

# Get all region statistics
all_stats = brain.getAllRegionStats()
for stats in all_stats:
    print(f"Region {stats.id} ({stats.name}): {stats.neuronCount} neurons, {stats.firingRate:.2f} Hz")
```

#### Memory System Operations

```python
# Working memory operations
brain.updateWorkingMemoryPattern([0.1, 0.5, 0.9, 0.3, 0.7])
activation = brain.getWorkingMemoryActivation()
print(f"Working memory activation: {activation}")

# Episodic memory operations
brain.storeEpisodicMemory([1.0, 0.8, 0.6, 0.4, 0.2], 0.9)
retrieved = brain.retrieveEpisodicMemory(0)
print(f"Retrieved episodic memory: {retrieved}")

# Memory importance
importance = brain.getEpisodicMemoryImportance(0)
print(f"Memory importance: {importance}")
```

#### Prediction System Operations

```python
# Compute prediction
input_data = [0.1, 0.5, 0.9, 0.3, 0.7]
prediction = brain.computePrediction(input_data)
print(f"Prediction: {prediction}")

# Get prediction error
error = brain.getPredictionError()
print(f"Prediction error: {error}")

# Update prediction
brain.updatePrediction(prediction)
```

#### Neuromodulation Operations

```python
# Apply reward modulation
brain.applyRewardModulationBatch(reward=1.0, predictedReward=0.8)

# Apply curiosity modulation
brain.applyCuriosityModulationBatch(novelty=0.7, explorationDrive=0.9)

# Apply dopamine modulation
brain.applyDopamineModulationBatch(predictionError=0.3, learningRate=0.001)

# Get neuromodulation levels
curiosity = brain.getCuriosityLevel()
novelty = brain.getNoveltyLevel()
dopamine = brain.getNeuromodulationLevel()
print(f"Curiosity: {curiosity:.3f}, Novelty: {novelty:.3f}, Dopamine: {dopamine:.3f}")
```

#### Development System Operations

```python
# Advance development
brain.advanceDevelopmentStage()

# Accelerate development
brain.accelerateDevelopmentBatch(factor=1.5)

# Experience-based development
experience = [0.1, 0.5, 0.9, 0.3, 0.7]
brain.applyExperienceBasedDevelopment(experience)

# Get current stage
stage = brain.getDevelopmentalStage()
print(f"Current development stage: {stage}")
```

#### Cognition System Operations

```python
# Planning
actions = brain.planActionsBatch(planningHorizon=5)
print(f"Planned actions: {actions}")

# Evaluate plan
plan_score = brain.evaluateActionPlan(actions)
print(f"Plan score: {plan_score:.3f}")

# Refine plan with feedback
feedback = [0.1, 0.5, 0.9, 0.3, 0.7]
refined_plan = brain.refineActionPlanBatch(actions, feedback)
print(f"Refined plan: {refined_plan}")

# Concept formation
patterns = [[1.0, 0.8], [0.9, 0.7], [0.8, 0.6]]
concepts = brain.formConceptsBatch(patterns)
print(f"Formed concepts: {concepts}")

# Attention selection
attention_weights = [0.1, 0.5, 0.9, 0.3, 0.7]
focused_neurons = brain.selectAttentiveFocusBatch(attention_weights)
print(f"Focused neurons: {len(focused_neurons)}")
```

#### Spike System Operations

```python
# Get neuron lists
active_neurons = brain.getActiveNeuronsBatch()
firing_neurons = brain.getFiringNeuronsBatch()
inactive_neurons = brain.getInactiveNeuronsBatch()
print(f"Active neurons: {len(active_neurons)}, Firing neurons: {len(firing_neurons)}")

# Firing rates by type
firing_rates_by_type = brain.getFiringRateByTypeMap()
for neuron_type, rate in firing_rates_by_type.items():
    print(f"{neuron_type}: {rate:.2f} Hz")

# E/I ratio by type
excitatory_inhibitory_ratio = brain.getExcitatoryInhibitoryRatio()
for neuron_type, ratio in excitatory_inhibitory_ratio.items():
    print(f"{neuron_type} E/I ratio: {ratio:.3f}")
```

#### Plasticity Operations

```python
# Apply STDP
brain.applySTDPBatch(learningRate=0.001, tauPlus=20.0, tauMinus=20.0)

# Apply Hebbian learning
brain.applyHebbianLearningBatch(learningRate=0.001)

# Apply structural plasticity
brain.applyStructuralPlasticityBatch(growthRate=0.0001, pruningRate=0.00001)

# Strengthen synapses
synapse_changes = [(synapse_id_1, 0.1), (synapse_id_2, 0.2)]
brain.strengthenSynapsesBatch(synapse_changes)

# Get strong synapses
strong_synapses = brain.getStrongSynapsesBatch(threshold=0.5)
print(f"Strong synapses: {len(strong_synapses)}")
```

#### Neuron Management

```python
# Add neurons to region
neurons = [neuron_id_1, neuron_id_2, neuron_id_3]
brain.addNeuronsToRegionBatch(neurons, region_id)

# Get neurons in region
region_neurons = brain.getNeuronsInRegion(region_id)
print(f"Region {region_id} has {len(region_neurons)} neurons")

# Set region excitability
brain.setRegionExcitabilityBatch(region_id, excitability=0.8)

# Get region average excitability
avg_excitability = brain.getRegionAverageExcitability(region_id)
print(f"Region {region_id} average excitability: {avg_excitability:.3f}")
```

### AgentBrain Class

#### Basic Operations

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Initialize with world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Decode motor command
motor_cmd = agent.decodeMotorCommand()

# Apply reward modulation
reward = 1.0
predicted_reward = 0.8
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopmentBatch(0.1)

# Get subsystem status
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")

# Check subsystem enable status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
```

### SimpleWorld Class

#### World Management

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world
world.reset()

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Update world
for step in range(100):
    world.update(timestep=0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Get agent body
    body = world.getAgentBody()
    print(f"Agent position: ({body.x}, {body.y}), Orientation: {body.orientation}")

# Apply motor command
motor_cmd = pynlm.MotorCommand.MoveForward
world.applyMotorCommand(motor_cmd, world.getSimulationTime())

# Add/remove world objects
obj = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(obj)
world.removeObject(5.0, 5.0)

# World properties
print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
print(f"Current simulation time: {world.getSimulationTime()}")
```

### Enumerations

#### Neuron Types

```python
import pynlm

# Neuron types
neuron_types = [
    pynlm.NeuronType.Excitatory,
    pynlm.NeuronType.Inhibitory,
    pynlm.NeuronType.Modulatory,
    pynlm.NeuronType.Sensory,
    pynlm.NeuronType.Motor,
    pynlm.NeuronType.Internal
]

for neuron_type in neuron_types:
    print(f"Neuron type: {neuron_type}")
```

#### Action Types

```python
import pynlm

# Action types
action_types = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.MoveBackward,
    pynlm.ActionType.MoveLeft,
    pynlm.ActionType.MoveRight,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.TurnRight,
    pynlm.ActionType.Look,
    pynlm.ActionType.Interact,
    pynlm.ActionType.Eat,
    pynlm.ActionType.Drink,
    pynlm.ActionType.Rest,
    pynlm.ActionType.Wait
]

for action_type in action_types:
    print(f"Action type: {action_type}")
```

#### Developmental Stages

```python
import pynlm

# Developmental stages
stages = [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
    pynlm.DevelopmentalStage.Aging
]

for stage in stages:
    print(f"Development stage: {stage}")
```

### Utility Functions

#### Configuration Helper Functions

```python
import pynlm

# Format spike count
spike_count = 12500
formatted = pynlm.format_spike_count(spike_count)
print(f"Spike count: {formatted}")

# Format neuron count
neuron_count = 2500
formatted = pynlm.format_neuron_count(neuron_count)
print(f"Neuron count: {formatted}")

# Format time
time_seconds = 125.5
formatted = pynlm.format_time(time_seconds)
print(f"Time: {formatted}")
```

#### Validation Functions

```python
import pynlm

# Validate neuron count
brain = pynlm.createBrain(pynlm.createDefaultConfig())
is_valid = pynlm.validate_neuron_count(brain, 10000)
print(f"Neuron count valid: {is_valid}")

# Validate step
is_valid = pynlm.validate_step(brain, 100)
print(f"Step valid: {is_valid}")

# Validate time
is_valid = pynlm.validate_time(brain, 10.0)
print(f"Time valid: {is_valid}")
```

### Error Handling

#### Exception Classes

```python
import pynlm

try:
    config = pynlm.Config()
    config.loadFromFile("nonexistent.json")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidConfigError as e:
    print(f"Invalid config error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

#### Error Checking

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check for errors
errors_detected = brain.detectErrorsBatch()
if errors_detected:
    recent_errors = brain.getRecentErrorsBatch()
    for error in recent_errors:
        print(f"Error: {error}")
    
    # Attempt recovery
    recovery_successful = brain.attemptRecoveryBatch()
    print(f"Recovery successful: {recovery_successful}")
```

### Advanced Usage Patterns

#### State Serialization

```python
import pynlm
import json

# Create brain with state
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for a while
for step in range(100):
    brain.step(step)

# Save brain state
brain.saveToCheckpoint("brain_state.bin")

# Export configuration to JSON
config_json = brain.getConfig().saveToJson()
with open("config.json", "w") as f:
    f.write(config_json)

# Import configuration from JSON
import_config = pynlm.Config()
import_config.loadFromJson(config_json)
brain.updateConfigurationParameterBatch("brain.neuron_count", 2000)
```

#### Performance Monitoring

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Start profiling
brain.startProfilingBatch()

# Run simulation
for step in range(1000):
    brain.step(step)

# Stop profiling
brain.stopProfilingBatch()

# Get performance metrics
metrics = brain.getPerformanceMetricsBatch()
for key, value in metrics.items():
    print(f"{key}: {value}")

# Check for anomalies
anomalies = brain.checkForAnomaliesBatch()
if anomalies:
    alerts = brain.getActiveAlertsBatch()
    for alert in alerts:
        print(f"Alert: {alert}")
```

#### Resource Management

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check resource availability
required_energy = 100.0
if brain.checkResourceAvailabilityBatch(required_energy):
    # Allocate resources
    brain.allocateResourcesBatch(required_energy)
    print("Resources allocated successfully")
else:
    print("Insufficient resources")

# Get resource usage
resource_usage = brain.getResourceUsageBatch()
for resource, usage in resource_usage.items():
    print(f"{resource}: {usage}")
```

#### System Diagnostics

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Validate integrity
integrity_valid = brain.validateBrainIntegrityBatch()
print(f"Brain integrity valid: {integrity_valid}")

# Get diagnostics
diagnostics = brain.getBrainDiagnosticsBatch()
for diagnostic, value in diagnostics.items():
    print(f"{diagnostic}: {value}")

# Check system status
status = brain.getSystemStatusBatch()
print(f"System status: {status}")

# Get health indicators
health = brain.getSystemHealthBatch()
for indicator, status in health.items():
    print(f"{indicator}: {status}")
```

## Configuration Presets

The following configuration presets are available:

### Performance Presets

- **performance**: Optimized for high-speed simulation
- **balanced**: Balanced performance and accuracy
- **detail**: High accuracy, slower simulation

### Development Presets

- **development**: Aggressive developmental plasticity
- **conservative**: Slow, gradual development
- **critical_period**: Focused development during critical period

### Learning Presets

- **curiosity**: Curiosity-driven exploration
- **reward**: Reward-based learning
- **prediction**: Prediction-based learning
- **mixed**: Combination of all learning modes

### Memory Presets

- **working**: Strong working memory
- **episodic**: Rich episodic memory
- **semantic**: Concept-based memory
- **procedural**: Skill-based memory

To apply a preset, use:

```python
config.applyPreset("performance")
```

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"

**Problem**: Brain appears inactive

**Solution**:
```python
# Ensure brain is initialized properly
brain = pynlm.createBrain(config)
brain.initialize()

# Check for neurons
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")

# Add some input
brain.injectCurrentToNeuronsBatch([some_neuron_id], 10.0)

# Try more simulation steps
brain.runSteps(100)
```

#### "The agent isn't moving"

**Problem**: Agent not responding to environment

**Solution**:
```python
# Check that world.update() is being called
world.update(0.1)

# Verify motor command decoding
motor_cmd = agent.decodeMotorCommand()
print(f"Decoded motor command: {motor_cmd}")

# Apply motor command
world.applyMotorCommand(motor_cmd, world.getSimulationTime())

# Check agent subsystems
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
```

#### "Everything is 0"

**Problem**: All values are zero

**Solution**:
```python
# Brains need time to "warm up"
brain.runSteps(100)

# Some neurons need input to fire
brain.injectCurrentToNeuronsBatch([sensory_neuron_id], 5.0)

# Check sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### "ImportError: No module named 'pynlm'"

**Problem**: Python module not found

**Solution**:
```bash
# Install the module
pip install pynlm

# Or install from source
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

#### "Segmentation fault"

**Problem**: Crash during simulation

**Solution**:
```python
# Ensure brain is initialized before using it
brain = pynlm.createBrain(config)
brain.initialize()

# Reset before re-running
brain.reset()

# Check for errors
errors = brain.detectErrorsBatch()
if errors:
    print(f"Errors detected: {errors}")
    # Attempt recovery
    brain.attemptRecoveryBatch()
```

### Advanced Debugging

#### Logging and Monitoring

```python
import pynlm

# Enable verbose logging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable profiling for performance analysis
brain.startProfilingBatch()

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        # Get system status
        status = brain.getSystemStatusBatch()
        print(f"Step {step}: {status}")
        
        # Check for anomalies
        anomalies = brain.checkForAnomaliesBatch()
        if anomalies:
            print(f"Anomalies detected at step {step}")

# Stop profiling and get results
brain.stopProfilingBatch()
metrics = brain.getPerformanceMetricsBatch()
print(f"Performance metrics: {metrics}")
```

#### Configuration Debugging

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Check configuration validity
validation = config.validate()
if not validation.valid:
    print(f"Configuration invalid: {validation.errorMessage}")
    for warning in validation.warnings:
        print(f"Warning: {warning}")

# Check schema
schema_valid = config.validateSchema(predefined_schema)
if schema_valid:
    print("Configuration matches schema")
else:
    print("Configuration does not match schema")

# Debug specific values
debug_value = config.get_int("brain.neuron_count", -1)
print(f"Debug value: {debug_value}")
```

## Migration Guide

### From Phase 3/4 to Phase 6

If migrating from earlier phases, update your code:

#### Phase 3/4 Example

```python
# Old Phase 3/4 style
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### New Phase 6 Style

```python
# New Phase 6 style with enhanced features
brain = pynlm.createBrain(config)
brain.initialize()

# Enable integrated systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with monitoring
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Access integrated systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
planner = brain.getPlanner()
development_system = brain.getDevelopmentSystem()
```

### From Simple to Advanced Usage

#### Simple Usage

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Done! Spikes: {brain.getTotalSpikeCount()}")
```

#### Advanced Usage

```python
import pynlm

# Create complete agent simulation
def run_advanced_simulation():
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run complex simulation
    for step in range(1000):
        # World update
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Motor output
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Development
        agent.updateDevelopment(0.1)
        
        # Monitoring
        if step % 100 == 0:
            print(f"Step {step}: Status check")
    
    return brain, agent, world

brain, agent, world = run_advanced_simulation()
```

## Performance Tips

### Simulation Performance

1. **Batch Operations**: Use `runSteps()` for multiple steps instead of individual `step()` calls
2. **Memory Management**: Call `reset()` between simulation runs to free memory
3. **Configuration**: Use appropriate presets for your use case
4. **Hardware**: Ensure your system has adequate RAM and CPU for large simulations

### Memory Optimization

1. **Checkpointing**: Use `saveToCheckpoint()` to save brain state between runs
2. **Resource Monitoring**: Monitor `getResourceUsageBatch()` for memory usage
3. **Cleanup**: Clear unused references to help garbage collection

### Development Best Practices

1. **Modular Design**: Create separate functions for different parts of your simulation
2. **Error Handling**: Implement comprehensive error checking and recovery
3. **Logging**: Use system status and diagnostics for debugging
4. **Testing**: Test with different configuration presets

## Future Enhancements

The following features are planned for future releases:

1. **Enhanced Visualization**: Integration with plotting libraries
2. **Advanced Learning Algorithms**: More sophisticated learning mechanisms
3. **Multi-Agent Systems**: Support for multiple interacting agents
4. **Custom Environments**: Framework for custom world implementations
5. **Distributed Computing**: Support for parallel simulation across multiple machines
6. **Real-time Simulation**: Integration with real-time sensors and actuators

## Conclusion

NLM Python bindings provide a comprehensive interface to an experimental artificial developmental brain system. The enhanced API includes:

- **Improved organization** with logical grouping of related functions
- **Better error handling** with specific exception classes
- **Batch operations** for improved performance
- **Comprehensive validation** and diagnostics
- **Advanced convenience methods** for common operations
- **Detailed documentation** with examples and troubleshooting guides

The bindings enable users to create sophisticated neural simulations while maintaining high performance and flexibility. Future releases will continue to enhance the functionality and usability of the NLM system.

## References

- NLM Project Documentation
- Python Bindings Technical Specifications
- C++ Core Implementation Details
- Research Papers on Neural Learning Machines

---

*For technical support, please refer to the NLM project documentation or contact the development team.*
