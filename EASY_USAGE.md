# NLM (Neural Learning Machine) — Comprehensive Documentation

**Version:** 0.1.0
**Phase:** 6: Final Integration
**Author:** NLM Project Team

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Features

- **Phase 6: Final Integration** - All systems integrated into a coherent artificial brain
- **Memory Systems** - Working memory, episodic memory, associative memory
- **Neuromodulation Integration** - Dopamine, curiosity, novelty detection
- **Prediction System** - Integrated prediction with learning
- **Cognition Systems** - Neural planning, concept formation, attention
- **Development Integration** - Developmental stages affecting plasticity rates
- **Persistence** - Checkpoint save/load, brain state serialization
- **Replay and Consolidation** - Episodic memory replay during simulation

## System Architecture

### Core Components

1. **Brain** - Central neural simulation engine
2. **AgentBrain** - Interface connecting NLM brain to world
3. **SimpleWorld** - 2D environment for NLM simulation
4. **Configuration** - Advanced configuration system
5. **Memory Systems** - Working, episodic, associative memory
6. **Neuromodulation Systems** - Dopamine, curiosity, prediction error
7. **Plasticity Systems** - STDP, Hebbian, structural plasticity
8. **Development System** - Developmental stages and maturation

### Integration Architecture

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Configuration System

### Overview

The NLM configuration system provides comprehensive control over all aspects of the neural simulation, including brain parameters, learning rates, neuromodulation, and environmental settings.

### Configuration Presets

The configuration system includes several preset configurations optimized for different use cases:

#### 1. Development Configuration (`createDevelopmentConfig()`)

Optimized for balanced learning and exploration:

```python
config = nlm.createDevelopmentConfig()
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("stdp_ltp_weight", 0.02f)
config.set("stdp_ltd_weight", 0.015f)
```

**Key Features:**
- High plasticity rates for rapid learning
- Balanced neuromodulation
- Multiple brain regions for diverse processing
- Developmental stages for maturation

#### 2. Exploration Configuration (`createExplorationConfig()`)

Optimized for discovery and novelty-seeking behavior:

```python
config = nlm.createExplorationConfig()
config.set("curiosity_weight_novelty", 2.0f)
config.set("max_exploration_chance", 0.3f)
config.set("social_learning_enabled", True)
```

**Key Features:**
- High curiosity and exploration
- Social learning capabilities
- Increased plasticity for adaptation
- Novelty detection emphasis

#### 3. Efficiency Configuration (`createEfficiencyConfig()`)

Optimized for fast and stable performance:

```python
config = nlm.createEfficiencyConfig()
config.set("parallel_processing_enabled", True)
config.set("simd_optimization_enabled", True)
config.set("memory_pooling_enabled", True)
```

**Key Features:**
- Performance optimization
- SIMD vectorization
- Memory pooling
- Reduced computational overhead

#### 4. Social Configuration (`createSocialConfig()`)

Optimized for cooperative and group learning:

```python
config = nlm.createSocialConfig()
config.set("social_tie_strength", 0.8f)
config.set("cooperation_probability", 0.7f)
config.set("theory_of_mind_enabled", True)
```

**Key Features:**
- Social connection strengths
- Cooperative behavior
- Theory of mind
- Perspective taking

#### 5. Adaptive Configuration (`createAdaptiveConfig()`)

Optimized for self-adjusting based on performance:

```python
config = nlm.createAdaptiveConfig()
config.set("meta_learning_enabled", True)
config.set("learning_rate_adaptation_enabled", True)
config.set("performance_monitoring_enabled", True)
```

**Key Features:**
- Meta-learning capabilities
- Performance monitoring
- Adaptive learning rates
- Dynamic plasticity adjustment

### Configuration Reference

#### Basic Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducible simulations |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int | 1000 | Number of neurons in the network |
| `region_count` | int | 3 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |

#### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant |
| `curiosity_weight_novelty` | float | 2.0 | Weight of novelty in curiosity |
| `curiosity_weight_prediction_error` | float | 0.5 | Weight of prediction error in curiosity |

#### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse pruning |
| `learning_rate` | float | 0.01 | Learning rate for plasticity |

#### Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory_capacity` | int | 10000 | Capacity of memory systems |
| `max_episodes` | int | 1000 | Maximum number of episodic memory episodes |

#### Developmental Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_stage_initial` | int | 0 | Initial developmental stage |
| `development_stage_critical_period` | float | 300.0 | Critical period duration |
| `development_stage_maturation` | float | 900.0 | Maturation period duration |
| `development_stage_adult` | float | 3000.0 | Adult period duration |

### Configuration Validation

The configuration system includes comprehensive validation to ensure parameters are within valid ranges:

```python
# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration errors:", config.getValidationErrors())
```

### Configuration File I/O

The configuration system supports loading from and saving to files:

```python
# Save configuration
config.saveToFile("my_config.txt")

# Load configuration
config2 = nlm.createDefaultConfig()
config2.loadFromFile("my_config.txt")
```

### Configuration Operations

#### Copy and Merge

```python
# Copy configuration
config_copy = config.copy()

# Merge configurations
config1.merge(config2)
```

### Python Binding API

#### Brain Class

```python
# Create brain with default configuration
brain = nlm.createBrain()
brain.initialize()

# Create brain with custom configuration
config = nlm.createDevelopmentConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Simulation step
brain.step(1000)  # Step 1000
brain.step(1000, 0.5)  # Step 1000 with timestamp 0.5

# Sensory input
vision = nlm.Vision(16, 16, 3)
brain.receiveSensoryInput(vision)

# Produce action
action = brain.produceAction()
```

#### AgentBrain Class

```python
# Create agent brain
brain = nlm.createBrain()
agent = nlm.createAgentBrain(brain)

# Initialize with world
world = nlm.createSimpleWorld()
world.configure(100, 100, 16, 16)
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Decode motor command
command = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(0.5, 0.3)

# Update development
agent.updateDevelopment(0.001)
```

#### World Class

```python
# Create world
world = nlm.createSimpleWorld()

# Configure world
world.configure(100, 100, 16, 16)  # width, height, vision_width, vision_height

# Set agent start position
world.setAgentStart(50, 50)

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")

# Add world object
obj = nlm.WorldObject(75, 75, nlm.WorldObjectType.Resource, 0.5, 0.3)
world.addObject(obj)
```

#### Action Class

```python
# Create simple action
action = nlm.Action(nlm.ActionType.MoveForward)

# Create action with parameters
action = nlm.Action(nlm.ActionType.Custom, [0.5, 0.8, 0.3])

# Set action properties
action.setName("move_forward")
action.setType(nlm.ActionType.MoveForward)
action.setParameter(0, 0.5)  # parameter 0

# Get action properties
print(f"Action name: {action.getName()}")
print(f"Action type: {action.getType()}")
print(f"Parameters: {action.getParameters()}")
```

#### Configuration Class

```python
# Create default configuration
config = nlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 1500)
config.set("region_count", 5)
config.set("learning_rate", 0.02f)
config.set("connection_probability", 0.15f)

# Get configuration values
neuron_count = config.getInt("neuron_count")
learning_rate = config.getFloat("learning_rate")
connection_probability = config.getFloat("connection_probability")

# Check if configuration key exists
if config.has("neuron_count"):
    print("Neuron count is set")

# Get all configuration keys
keys = config.getKeys()

# Configuration presets
config = nlm.createDevelopmentConfig()
config = nlm.createExplorationConfig()
config = nlm.createEfficiencyConfig()
config = nlm.createSocialConfig()
config = nlm.createAdaptiveConfig()

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Validation errors:", config.getValidationErrors())
```

#### Exception Handling

```python
import pynlm as nlm

try:
    # Create brain with invalid configuration
    config = nlm.createDefaultConfig()
    config.set("neuron_count", -100)  # Invalid: must be positive
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.InitializationError as e:
    print(f"Initialization error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.AgentError as e:
    print(f"Agent error: {e}")
except nlm.NLMException as e:
    print(f"NLM error: {e}")
```

### Best Practices

#### 1. Configuration Management

```python
def createOptimizedConfig():
    """Create an optimized configuration for a specific task"""
    config = nlm.createDevelopmentConfig()
    
    # Adjust based on specific requirements
    config.set("neuron_count", 2000)  # More neurons for complex tasks
    config.set("learning_rate", 0.01)  # Slower learning for stability
    
    return config
```

#### 2. Error Handling

```python
def safeSimulationStep(brain, step):
    """Perform simulation step with comprehensive error handling"""
    try:
        brain.step(step)
        return True
    except nlm.SimulationError:
        print(f"Simulation error at step {step}")
        return False
    except nlm.NLMException as e:
        print(f"NLM error: {e}")
        return False
```

#### 3. Configuration Validation

```python
def validateAndFixConfig(config):
    """Validate configuration and fix common issues"""
    if not config.validate():
        print("Fixing configuration issues:", config.getValidationErrors())
        
        # Auto-fix common issues
        config.set("neuron_count", max(100, config.getOrInt("neuron_count", 1000)))
        config.set("learning_rate", max(0.001, min(0.1, config.getOrFloat("learning_rate", 0.01))))
        
        # Re-validate
        if not config.validate():
            raise nlm.ConfigurationError("Configuration validation failed after fixes")
    
    return config
```

#### 4. Simulation Loop Pattern

```python
def runSimulation(world, brain, agent, steps):
    """Complete simulation loop with comprehensive error handling"""
    print(f"Starting simulation: {steps} steps")
    
    for step in range(steps):
        try:
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Decode motor command
            command = agent.decodeMotorCommand()
            
            # Apply command to world (implementation depends on world type)
            # world.applyMotorCommand(command, step * world.getTimestep())
            
            # Update world state
            # world.update(world.getTimestep())
            
            # Log progress periodically
            if step % 100 == 0:
                print(f"Step {step}/{steps}")
                
        except (nlm.SimulationError, nlm.AgentError) as e:
            print(f"Error at step {step}: {e}")
            break
        except Exception as e:
            print(f"Unexpected error at step {step}: {e}")
            break
    
    print("Simulation completed")
```

#### 5. Experiment Configuration

```python
def createExperimentConfig(experiment_type="standard"):
    """Create configuration for different experiment types"""
    if experiment_type == "standard":
        return nlm.createDevelopmentConfig()
    elif experiment_type == "exploration":
        return nlm.createExplorationConfig()
    elif experiment_type == "social":
        return nlm.createSocialConfig()
    elif experiment_type == "efficient":
        return nlm.createEfficiencyConfig()
    else:
        return nlm.createAdaptiveConfig()
```

### Advanced Usage

#### 1. Custom Configuration

```python
# Create custom configuration with all parameters
config = nlm.createDefaultConfig()

# Set all parameters manually
config.set("random_seed", 12345)
config.set("simulation_timestep", 0.001)
config.set("neuron_count", 500)
config.set("region_count", 1)
config.set("connection_probability", 0.05)
config.set("stdp_ltp_weight", 0.01)
config.set("stdp_ltd_weight", 0.012)
config.set("stdp_tau", 10.0)
config.set("synaptogenesis_rate", 0.00005)
config.set("pruning_rate", 0.000005)
config.set("learning_rate", 0.005)
config.set("memory_capacity", 3000)
```

#### 2. Configuration Templates

```python
# Configuration templates for different use cases
CONFIG_TEMPLATES = {
    "minimal": {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.02,
    },
    "standard": {
        "neuron_count": 1000,
        "region_count": 3,
        "connection_probability": 0.1,
    },
    "complex": {
        "neuron_count": 5000,
        "region_count": 5,
        "connection_probability": 0.15,
    },
    "debug": {
        "neuron_count": 10,
        "region_count": 1,
        "connection_probability": 0.5,
        "simulation_timestep": 0.1,
    }
}

def getConfigTemplate(template_name):
    """Get a pre-defined configuration template"""
    if template_name not in CONFIG_TEMPLATES:
        raise ValueError(f"Unknown template: {template_name}")
    
    template = CONFIG_TEMPLATES[template_name]
    config = nlm.createDefaultConfig()
    
    for key, value in template.items():
        config.set(key, value)
    
    return config
```

#### 3. Configuration Comparison

```python
def compareConfigs(config1, config2):
    """Compare two configurations"""
    differences = []
    
    keys1 = set(config1.getKeys())
    keys2 = set(config2.getKeys())
    
    # Find differences in keys
    only_in_config1 = keys1 - keys2
    only_in_config2 = keys2 - keys1
    
    if only_in_config1:
        differences.append(f"Only in config1: {only_in_config1}")
    
    if only_in_config2:
        differences.append(f"Only in config2: {only_in_config2}")
    
    # Find differences in values
    common_keys = keys1 & keys2
    for key in common_keys:
        if config1.getValue(key) != config2.getValue(key):
            differences.append(f"Difference in {key}: {config1.getValue(key)} != {config2.getValue(key)}")
    
    return differences
```

### Performance Tips

1. **Reuse Configurations**: Create configurations once and reuse them instead of creating new ones for each simulation.

2. **Validate Early**: Validate configurations early in your code to catch issues before starting simulations.

3. **Use Appropriate Presets**: Use the built-in presets (`createDevelopmentConfig`, `createExplorationConfig`, etc.) instead of creating configurations from scratch.

4. **Monitor Performance**: Use performance monitoring configurations when developing new behaviors.

5. **Validate After Changes**: Always validate configurations after making changes.

### Troubleshooting

#### Common Issues and Solutions

1. **Configuration Validation Errors**
   - Error: "neuron_count must be positive"
   - Solution: Set a positive value for neuron_count

2. **File I/O Errors**
   - Error: "Failed to open file"
   - Solution: Ensure the file path is valid and write permissions are available

3. **Simulation Errors**
   - Error: "Invalid sensory input"
   - Solution: Ensure sensory input data is valid before passing to brain

4. **Memory Issues**
   - Error: "Memory capacity exceeded"
   - Solution: Increase memory_capacity in configuration

#### Error Messages

| Error Type | Description | Solution |
|------------|-------------|----------|
| `ConfigurationError` | Invalid configuration parameters | Validate configuration before use |
| `InitializationError` | Failed to initialize brain | Check configuration values |
| `SimulationError` | Simulation failed | Check environment and parameters |
| `AgentError` | Agent-specific error | Check agent state and behavior |
| `NLMException` | Generic NLM error | Check error message for details |

## Conclusion

The NLM Python bindings provide a comprehensive interface for creating, configuring, and simulating neural agents with advanced learning capabilities. This documentation covers the basic usage as well as advanced features for experienced users.

For the most up-to-date information and additional examples, visit the NLM project documentation and GitHub repository.

**Happy simulating! 🤖✨**