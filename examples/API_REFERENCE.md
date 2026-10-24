# NLM Python API Reference: Convenience Methods

This document provides comprehensive documentation for the new convenience methods added to the NLM Python API, showcasing the improved functionality and ease of use.

## Overview

The NLM Python API has been enhanced with several convenience methods that simplify common operations and provide more intuitive access to brain and configuration data. These methods complement the existing API while providing additional functionality for improved user experience.

## Config Convenience Methods

### getInt(key, default=0)

**Description:** Safely retrieve an integer configuration value with a default fallback.

**Parameters:**
- `key` (str): Configuration key to retrieve
- `default` (int, optional): Default value if key doesn't exist. Defaults to 0.

**Returns:**
- int: Configuration value as integer

**Error Handling:**
- Returns default value if key doesn't exist
- Throws exception for invalid key names

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Basic usage
neuron_count = config.getInt("brain.neuron_count")  # Returns 1000

# With default
max_neurons = config.getInt("brain.max_neurons", 2000)  # Returns 2000

# Non-existent key (returns default)
uninitialized = config.getInt("missing.key", 42)  # Returns 42
```

### getFloat(key, default=0.0)

**Description:** Safely retrieve a float configuration value with a default fallback.

**Parameters:**
- `key` (str): Configuration key to retrieve
- `default` (float, optional): Default value if key doesn't exist. Defaults to 0.0.

**Returns:**
- float: Configuration value as float

**Error Handling:**
- Returns default value if key doesn't exist
- Converts compatible types (int to float)
- Throws exception for invalid key names

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.synapse_density", 0.15)
config.set("neuromod.dopamine.scale", 1.5)

# Basic usage
synapse_density = config.getFloat("brain.synapse_density")  # Returns 0.15
dopamine_scale = config.getFloat("neuromod.dopamine.scale")  # Returns 1.5

# With default
learning_rate = config.getFloat("plasticity.stdp.learning_rate", 0.001)  # Returns 0.001

# Type conversion
config.set("brain.neuron_count", 1000)
converted = config.getFloat("brain.neuron_count", 0.0)  # Returns 1000.0
```

### getBool(key, default=False)

**Description:** Safely retrieve a boolean configuration value with a default fallback.

**Parameters:**
- `key` (str): Configuration key to retrieve
- `default` (bool, optional): Default value if key doesn't exist. Defaults to False.

**Returns:**
- bool: Configuration value as boolean

**Error Handling:**
- Returns default value if key doesn't exist
- Converts string "true"/"false" to boolean
- Converts numeric 0/1 to boolean
- Throws exception for invalid key names

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuromod.curiosity.enable", True)
config.set("plasticity.stdp.enable", False)

# Basic usage
curiosity_enabled = config.getBool("neuromod.curiosity.enable")  # Returns True
stdp_enabled = config.getBool("plasticity.stdp.enable")  # Returns False

# With default
hebbian_enabled = config.getBool("plasticity.hebbian.enable", True)  # Returns True

# Type conversion
config.set("plasticity.enabled", 1)
converted = config.getBool("plasticity.enabled", False)  # Returns True
```

### getString(key, default="")

**Description:** Safely retrieve a string configuration value with a default fallback.

**Parameters:**
- `key` (str): Configuration key to retrieve
- `default` (str, optional): Default value if key doesn't exist. Defaults to "".

**Returns:**
- str: Configuration value as string

**Error Handling:**
- Returns default value if key doesn't exist
- Converts other types to string (int, float, bool)
- Throws exception for invalid key names

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.region_name", "cortex")
config.set("simulation.name", "test_run")

# Basic usage
region_name = config.getString("brain.region_name")  # Returns "cortex"
simulation_name = config.getString("simulation.name")  # Returns "test_run"

# With default
config_name = config.getString("simulation.name", "default_run")  # Returns "test_run"
backup_name = config.getString("nonexistent.name", "backup")  # Returns "backup"

# Type conversion
config.set("neuron.count", 1000)
converted = config.getString("neuron.count", "unknown")  # Returns "1000"
```

## Brain Convenience Methods

### getNeuronCountByType(type)

**Description:** Count the number of neurons of a specific type in the brain.

**Parameters:**
- `type` (NeuronType): Neuron type to count

**Returns:**
- int: Number of neurons of the specified type

**Available Neuron Types:**
- `pynlm.NeuronType.Excitatory`
- `pynlm.NeuronType.Inhibitory`
- `pynlm.NeuronType.Sensory`
- `pynlm.NeuronType.Motor`
- `pynlm.NeuronType.Modulatory`
- `pynlm.NeuronType.Internal`

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Count different neuron types
excitatory = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
inhibitory = brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)
sensory = brain.getNeuronCountByType(pynlm.NeuronType.Sensory)
motor = brain.getNeuronCountByType(pynlm.NeuronType.Motor)

print(f"Excitatory: {excitatory}, Inhibitory: {inhibitory}")
print(f"Sensory: {sensory}, Motor: {motor}")
```

### getBrainStats()

**Description:** Get comprehensive brain statistics as a Python dictionary.

**Parameters:**
- None

**Returns:**
- dict: Dictionary containing comprehensive brain statistics

**Available Statistics:**
- `total_neurons`: Total number of neurons in the brain
- `total_synapses`: Total number of synapses in the brain
- `active_neurons`: Number of currently active neurons
- `firing_neurons`: Number of currently firing neurons
- `total_spikes`: Total number of spikes recorded
- `firing_rate`: Average firing rate across all neurons
- `e_to_i_ratio`: Excitation/inhibition balance ratio
- `developmental_stage`: Current developmental stage (int)
- `region_count`: Number of neural regions
- `populations`: List of region information dictionaries

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive statistics
brain_stats = brain.getBrainStats()

# Access individual statistics
print(f"Total neurons: {brain_stats['total_neurons']}")
print(f"Total spikes: {brain_stats['total_spikes']}")
print(f"Firing rate: {brain_stats['firing_rate']:.4f}")
print(f"E/I ratio: {brain_stats['e_to_i_ratio']:.3f}")
print(f"Development stage: {brain_stats['developmental_stage']}")

# Iterate through all statistics
for key, value in brain_stats.items():
    if isinstance(value, float):
        print(f"{key}: {value:.4f}")
    else:
        print(f"{key}: {value}")
```

## AgentBrain Convenience Methods

### getAgentStats()

**Description:** Get comprehensive agent statistics as a Python dictionary.

**Parameters:**
- None

**Returns:**
- dict: Dictionary containing comprehensive agent statistics

**Available Statistics:**
- `curiosity`: Current curiosity level (0.0 to 1.0)
- `novelty`: Current novelty level (0.0 to 1.0)
- `prediction_error`: Current prediction error (0.0 to 1.0)
- `neuromodulation`: Current neuromodulation level (0.0 to 1.0)
- `development_stage`: Current developmental stage (int)
- `enabled_systems`: Dictionary showing which systems are enabled
- `brain_stats`: Nested dictionary with brain statistics (if brain is available)

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Get comprehensive agent statistics
agent_stats = agent.getAgentStats()

# Access individual statistics
print(f"Curiosity level: {agent_stats['curiosity']:.3f}")
print(f"Novelty level: {agent_stats['novelty']:.3f}")
print(f"Prediction error: {agent_stats['prediction_error']:.3f}")
print(f"Neuromodulation level: {agent_stats['neuromodulation']:.3f}")
print(f"Development stage: {agent_stats['development_stage']}")

# Check enabled systems
enabled = agent_stats['enabled_systems']
print(f"Reward modulation: {enabled['reward_modulation']}")
print(f"Structural plasticity: {enabled['structural_plasticity']}")
print(f"Development: {enabled['development']}")
print(f"Curiosity: {enabled['curiosity']}")
```

### getActionRecommendation(context)

**Description:** Get action recommendation based on current brain state and context.

**Parameters:**
- `context` (dict, optional): Context dictionary for recommendation. Defaults to empty dict.

**Returns:**
- dict: Action recommendation with action type and reasoning

**Recommendation Structure:**
- `action`: Recommended action type (string)
- `reason`: Explanation of the recommendation (string)
- `curiosity`: Current curiosity level (float)
- `prediction_error`: Current prediction error (float)
- `firing_rate`: Current brain firing rate (float)

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Get action recommendation
recommendation = agent.getActionRecommendation()

# Access recommendation details
print(f"Recommended action: {recommendation['action']}")
print(f"Reason: {recommendation['reason']}")
print(f"Curiosity: {recommendation['curiosity']:.3f}")
print(f"Prediction error: {recommendation['prediction_error']:.3f}")
print(f"Firing rate: {recommendation['firing_rate']:.4f}")

# With custom context
custom_context = {"step": 100, "time": 10.5, "environment": "normal"}
recommendation = agent.getActionRecommendation(custom_context)
```

### enableAllSystems()

**Description:** Enable all agent systems at once (reward modulation, structural plasticity, development, and curiosity).

**Parameters:**
- None

**Returns:**
- str: Confirmation message

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable all systems at once
result = agent.enableAllSystems()
print(result)  # "All agent systems enabled"

# Check status
system_status = agent.getSystemStatus()
print(f"Systems enabled: {sum(system_status.values())}/{len(system_status)}")
```

### disableAllSystems()

**Description:** Disable all agent systems at once (reward modulation, structural plasticity, development, and curiosity).

**Parameters:**
- None

**Returns:**
- str: Confirmation message

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable all systems first
agent.enableAllSystems()

# Disable all systems at once
result = agent.disableAllSystems()
print(result)  # "All agent systems disabled"

# Check status
system_status = agent.getSystemStatus()
print(f"Systems enabled: {sum(system_status.values())}/{len(system_status)}")
```

### getSystemStatus()

**Description:** Get current status of all agent systems.

**Parameters:**
- None

**Returns:**
- dict: Dictionary showing the status of each agent system

**System Status Structure:**
- `reward_modulation`: Boolean indicating if reward modulation is enabled
- `structural_plasticity`: Boolean indicating if structural plasticity is enabled
- `development`: Boolean indicating if development is enabled
- `curiosity`: Boolean indicating if curiosity is enabled

**Example:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Get system status
system_status = agent.getSystemStatus()

# Access individual system status
print(f"Reward modulation enabled: {system_status['reward_modulation']}")
print(f"Structural plasticity enabled: {system_status['structural_plasticity']}")
print(f"Development enabled: {system_status['development']}")
print(f"Curiosity enabled: {system_status['curiosity']}")

# Count enabled systems
enabled_count = sum(system_status.values())
total_systems = len(system_status)
print(f"Enabled systems: {enabled_count}/{total_systems}")
```

## Integration Examples

### Complete Example with All Convenience Methods

```python
import pynlm

def advanced_simulation_example():
    """Example demonstrating integration of all convenience methods"""
    
    # 1. Configure using convenience methods
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create world and initialize agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30)
    world.reset()
    agent.initialize(world)
    
    # 5. Enable all systems
    agent.enableAllSystems()
    
    # 6. Run simulation with statistics collection
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print enhanced statistics using convenience methods
        if step % 20 == 0:
            # Get brain statistics
            brain_stats = brain.getBrainStats()
            
            # Get agent statistics
            agent_stats = agent.getAgentStats()
            
            # Get system status
            system_status = agent.getSystemStatus()
            
            print(f"Step {step}:")
            print(f"  Brain stats: {brain_stats['total_spikes']} spikes, {brain_stats['firing_rate']:.4f} firing rate")
            print(f"  Agent stats: curiosity={agent_stats['curiosity']:.3f}, novelty={agent_stats['novelty']:.3f}")
            print(f"  Systems enabled: {sum(system_status.values())}/{len(system_status)}")
            print(f"  Action recommendation: {agent.getActionRecommendation()['action']}")
    
    # 7. Get final statistics
    final_brain_stats = brain.getBrainStats()
    final_agent_stats = agent.getAgentStats()
    
    print("\nFinal Statistics:")
    print(f"Total neurons: {final_brain_stats['total_neurons']}")
    print(f"Total spikes: {final_brain_stats['total_spikes']}")
    print(f"Final curiosity level: {final_agent_stats['curiosity']:.3f}")
    print(f"Final novelty level: {final_agent_stats['novelty']:.3f}")
    
    return brain, agent, world

# Run the example
brain, agent, world = advanced_simulation_example()
```

### Error Handling Best Practices

```python
import pynlm

def safe_config_access_example():
    """Example demonstrating safe configuration access"""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # Safe access with error handling
def safe_get(config, key, default_value, value_type):
        try:
            if value_type == "int":
                return config.getInt(key, default_value)
            elif value_type == "float":
                return config.getFloat(key, default_value)
            elif value_type == "bool":
                return config.getBool(key, default_value)
            elif value_type == "str":
                return config.getString(key, default_value)
        except Exception as e:
            print(f"Error accessing {key}: {e}")
            return default_value
    
    # Usage examples
    neuron_count = safe_get(config, "brain.neuron_count", 1000, "int")
    learning_rate = safe_get(config, "learning.rate", 0.001, "float")
    auto_enabled = safe_get(config, "auto.learn", False, "bool")
    model_name = safe_get(config, "model.name", "default", "str")
    
    return neuron_count, learning_rate, auto_enabled, model_name

# Run safe access example
neuron_count, learning_rate, auto_enabled, model_name = safe_config_access_example()
print(f"Neuron count: {neuron_count}, Learning rate: {learning_rate}")
print(f"Auto enabled: {auto_enabled}, Model name: {model_name}")
```

## Migration Guide

### From Old API to New API

**Before (Old API):**
```python
# Basic statistics
firing_rate = brain.getAverageFiringRate()
neuron_count = brain.getTotalNeuronCount()

# Configuration
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Manual statistics collection
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "e_to_i_ratio": brain.getExcitationInhibitionRatio()
}
```

**After (New API):**
```python
# Enhanced statistics with convenience methods
brain_stats = brain.getBrainStats()
firing_rate = brain_stats['firing_rate']
neuron_count = brain_stats['total_neurons']

# Simplified configuration access
neuron_count = config.getInt("brain.neuron_count", 1000)

# Neuron type analysis
excitatory = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
inhibitory = brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)

# Comprehensive statistics
stats = brain.getBrainStats()  # All statistics in one call
```

### Benefits of New API

1. **Simpler Code**: Less code needed for common operations
2. **Better Error Handling**: Built-in default values for missing keys
3. **Type Safety**: Convenience methods ensure type conversion
4. **Consistency**: Unified interface for statistics collection
5. **Performance**: Reduced method calls (e.g., `getBrainStats()` instead of multiple individual calls)
6. **Readability**: More descriptive method names and easier-to-understand code

## Performance Considerations

### Method Call Optimization

1. **Batch Statistics Collection**: Use `getBrainStats()` instead of individual statistics methods
2. **Cache Frequently Used Values**: Cache configuration values that are used frequently
3. **Conditional Access**: Only call statistics methods when needed

### Memory Usage

1. **Dictionary Overhead**: `getBrainStats()` and `getAgentStats()` create dictionaries
2. **Large Statistics Objects**: For very large brains, consider whether all statistics are needed
3. **Memory Management**: Clear references when not needed to free memory

## Testing and Validation

### Testing Convenience Methods

```python
def test_config_convenience_methods():
    """Test all config convenience methods"""
    
    config = pynlm.createDefaultConfig()
    
    # Test set/get roundtrip
    config.set("brain.neuron_count", 1000)
    assert config.getInt("brain.neuron_count", 0) == 1000
    
    config.set("brain.synapse_density", 0.15)
    assert config.getFloat("brain.synapse_density", 0.0) == 0.15
    
    config.set("neuromod.curiosity.enable", True)
    assert config.getBool("neuromod.curiosity.enable", False) == True
    
    config.set("brain.region_name", "cortex")
    assert config.getString("brain.region_name", "") == "cortex"
    
    # Test default values
    assert config.getInt("missing.key", 42) == 42
    assert config.getFloat("missing.key", 3.14) == 3.14
    assert config.getBool("missing.key", True) == True
    assert config.getString("missing.key", "default") == "default"

def test_brain_convenience_methods():
    """Test all brain convenience methods"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test neuron count by type
    excitatory = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
    assert isinstance(excitatory, int) and excitatory >= 0
    
    inhibitory = brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)
    assert isinstance(inhibitory, int) and inhibitory >= 0
    
    # Test brain stats
    brain_stats = brain.getBrainStats()
    assert isinstance(brain_stats, dict)
    assert "total_neurons" in brain_stats
    assert "firing_rate" in brain_stats
    assert isinstance(brain_stats["total_neurons"], int)
    assert isinstance(brain_stats["firing_rate"], float)

def test_agentbrain_convenience_methods():
    """Test all agent brain convenience methods"""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test enableAllSystems
    result = agent.enableAllSystems()
    assert isinstance(result, str)
    
    # Test getSystemStatus
    system_status = agent.getSystemStatus()
    assert isinstance(system_status, dict)
    assert all(isinstance(v, bool) for v in system_status.values())
    
    # Test getAgentStats
    agent_stats = agent.getAgentStats()
    assert isinstance(agent_stats, dict)
    assert "curiosity" in agent_stats
    assert "novelty" in agent_stats
    assert isinstance(agent_stats["curiosity"], float)
```

## Conclusion

The new convenience methods in the NLM Python API provide a more intuitive, robust, and user-friendly interface for working with brain simulations. These methods simplify common operations, provide better error handling, and make the API more accessible to users of all skill levels.

Key benefits include:

1. **Simplified Code**: Less code needed for common operations
2. **Better Error Handling**: Built-in default values and type conversion
3. **Enhanced Functionality**: Comprehensive statistics and analysis capabilities
4. **Improved Usability**: More descriptive method names and clearer interfaces
5. **Performance Optimization**: Reduced method calls and batch statistics collection

These convenience methods transform the NLM Python API from a collection of low-level functions into a comprehensive, user-friendly toolkit that makes it easy to create, simulate, and analyze brain-like systems.