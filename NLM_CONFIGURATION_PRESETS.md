# NLM Configuration Presets

## Overview

NLM provides several pre-configured settings to help users get started quickly with different use cases. Configuration presets abstract away the complexity of fine-tuning parameters while providing sensible defaults for various simulation scenarios.

## Available Presets

### Preset: "minimal"

**Use Case:** Quick testing, prototyping, learning the basics
**Best For:** Beginners, educational purposes, rapid development

**Parameters:**
- **Neurons:** 50
- **Regions:** 1
- **Connection Probability:** 0.05
- **Enabled Features:** Basic functionality only

**Example:**
```python
import pynlm

# Create minimal brain - fast and simple
config = pynlm.create_preset_config("minimal")
brain = pynlm.createBrain(config)
brain.initialize()

# Quick simulation
for i in range(50):
    brain.step(i)
    
print(f"Minimal brain test: {brain.getTotalNeuronCount()} neurons")
```

### Preset: "standard"

**Use Case:** Balanced simulation, learning algorithms, basic AI
**Best For:** Standard research, reinforcement learning, agent-based modeling

**Parameters:**
- **Neurons:** 200
- **Regions:** 3
- **Connection Probability:** 0.1
- **Enabled Features:** Reward modulation, curiosity

**Example:**
```python
import pynlm

# Create standard brain - good balance of complexity and performance
config = pynlm.create_preset_config("standard")
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    if action:
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Preset: "advanced"

**Use Case:** Complex cognitive systems, neuroscience research, advanced AI
**Best For:** Scientific research, sophisticated learning, brain modeling

**Parameters:**
- **Neurons:** 500
- **Regions:** 5
- **Connection Probability:** 0.15
- **Enabled Features:** All advanced features including memory systems

**Features Included:**
- Working memory
- Episodic memory
- Curiosity
- Development
- Structural plasticity

**Example:**
```python
import pynlm

# Create advanced brain - full cognitive capabilities
config = pynlm.create_preset_config("advanced")
brain = pynlm.createBrain(config)
brain.initialize()

# Create sophisticated agent
world = pynlm.create_world_with_obstacles(40, 40, 20)
world.configure(40, 40, 16, 16)
agent = pynlm.create_agent_with_monitoring(brain, world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run advanced simulation with monitoring
print("Running advanced simulation...")
for episode in range(50):
    world.reset()
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        if action:
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
            agent.updateDevelopment(0.1)

print(f"Advanced simulation completed. Final stats:")
stats = pynlm.get_detailed_stats(brain, agent, world)
print(f"  Neurons: {stats['brain']['total_neurons']}")
print(f"  Firing rate: {stats['brain']['average_firing_rate']:.2f} Hz")
print(f"  Curiosity level: {stats['agent']['curiosity_level']:.2f}")
```

## Custom Configuration Builder

### Method 1: Direct Configuration

```python
import pynlm

# Create custom configuration from scratch
config = pynlm.createDefaultConfig()
config.set("neuron_count", 300)           # Custom neuron count
config.set("region_count", 4)             # Custom region count
config.set("connection_probability", 0.12) # Custom connectivity
config.set("enable_working_memory", True)   # Enable memory
config.set("enable_curiosity", True)       # Enable curiosity

brain = pynlm.createBrain(config)
brain.initialize()
```

### Method 2: Template-Based Configuration

```python
import pynlm

def create_custom_config(base_preset="standard", **customizations):
    """
    Create configuration using preset as base with customizations.
    
    Args:
        base_preset: "minimal", "standard", or "advanced"
        **customizations: Override or add configuration parameters
    
    Returns:
        Configured Config object
    """
    # Start with preset
    config = pynlm.create_preset_config(base_preset)
    
    # Apply customizations
    for key, value in customizations.items():
        if key == "neuron_count":
            config.set("neuron_count", value)
        elif key == "region_count":
            config.set("region_count", value)
        elif key == "connection_probability":
            config.set("connection_probability", value)
        elif key == "enable_features":
            for feature, enabled in value.items():
                config.set(f"enable_{feature}", enabled)
        elif key == "custom_parameters":
            for param_name, param_value in value.items():
                config.set(param_name, param_value)
    
    return config

# Example: Enhanced standard configuration
enhanced_config = create_custom_config(
    base_preset="standard",
    neuron_count=400,                           # More neurons
    region_count=5,                             # More regions
    enable_features={
        "working_memory": True,
        "episodic_memory": True,
        "curiosity": True,
        "development": False,                   # Disable to save memory
        "structural_plasticity": True          # Enable for growth
    }
)

brain = pynlm.createBrain(enhanced_config)
brain.initialize()
```

### Method 3: Functional Configuration

```python
import pynlm

def create_reinforcement_learning_config(learning_style="td_error"):
    """
    Create configuration optimized for reinforcement learning.
    
    Args:
        learning_style: "td_error", "policy_gradient", or "q_learning"
    
    Returns:
        RL-optimized configuration
    """
    config = pynlm.create_preset_config("standard")
    
    if learning_style == "td_error":
        # Configure for temporal difference learning
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("prediction_horizon", 10)
        config.set("discount_factor", 0.95)
        config.set("learning_rate", 0.01)
        
    elif learning_style == "policy_gradient":
        # Configure for policy gradient methods
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("entropy_regularization", 0.1)
        config.set("policy_learning_rate", 0.001)
        config.set("value_learning_rate", 0.01)
        
    elif learning_style == "q_learning":
        # Configure for Q-learning style
        config.set("enable_reward_modulation", True)
        config.set("action_value_estimation", True)
        config.set("exploration_rate", 0.1)
        config.set("q_learning_rate", 0.1)
    
    return config

def create_neuroscience_research_config(species="human_like"):
    """
    Create configuration based on biological species characteristics.
    
    Args:
        species: "human_like", "rodent_like", "insect_like"
    
    Returns:
        Species-specific configuration
    """
    if species == "human_like":
        config = pynlm.create_preset_config("advanced")
        config.set("neuron_count", 1000)
        config.set("region_count", 10)
        config.set("critical_period_length", 200)
        config.set("maturation_rate", 0.02)
        
    elif species == "rodent_like":
        config = pynlm.create_preset_config("standard")
        config.set("neuron_count", 300)
        config.set("region_count", 4)
        config.set("critical_period_length", 100)
        config.set("maturation_rate", 0.05)
        
    elif species == "insect_like":
        config = pynlm.create_preset_config("minimal")
        config.set("neuron_count", 50)
        config.set("region_count", 2)
        config.set("enable_structural_plasticity", True)
        config.set("plasticity_rate", 0.1)
    
    return config
```

## Configuration Parameter Reference

### Core Parameters

| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `neuron_count` | int64_t | 10-10000 | Total number of neurons in brain |
| `region_count` | int64_t | 1-50 | Number of neural regions |
| `connection_probability` | double | 0.0-1.0 | Probability of connection between neurons |

### Feature Flags

| Feature | Default | Description |
|---------|---------|-------------|
| `enable_working_memory` | false | Neural working memory system |
| `enable_episodic_memory` | false | Episodic memory storage |
| `enable_associative_memory` | false | Pattern association memory |
| `enable_curiosity` | false | Novelty-driven exploration |
| `enable_development` | false | Developmental progression |
| `enable_structural_plasticity` | false | Synapse formation/removal |
| `enable_reward_modulation` | false | Reward-based learning |
| `enable_prediction` | false | Predictive modeling |
| `enable_planning` | false | Action sequence planning |

### Learning Parameters

| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `discount_factor` | double | 0.0-1.0 | Reward discounting factor |
| `learning_rate` | double | 0.0-1.0 | Learning rate for plasticity |
| `plasticity_rate` | double | 0.0-1.0 | Overall plasticity level |
| `critical_period_length` | int64_t | 0-1000 | Duration of critical learning period |
| `maturation_rate` | double | 0.0-1.0 | Rate of developmental maturation |

### Simulation Parameters

| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `simulation_timestep` | double | 0.0-1.0 | Time step duration |
| `prediction_horizon` | int64_t | 1-100 | Number of steps for prediction |
| `max_energy` | double | 0.0-1000 | Maximum energy for agent |
| `energy_decay_rate` | double | 0.0-1.0 | Energy depletion rate |

## Configuration Validation

### Automatic Validation

```python
import pynlm

def validate_and_fix_config(config):
    """
    Validate configuration and automatically fix common issues.
    
    Args:
        config: Configuration to validate
    
    Returns:
        Tuple of (is_valid, fixed_config, warnings)
    """
    warnings = []
    
    # Validate neuron count
    neuron_count = config.get<int64_t>("neuron_count")
    if not neuron_count.has_value() or neuron_count.value() < 10:
        warnings.append("Neuron count too low, setting to minimum (10)")
        config.set("neuron_count", static_cast<int64_t>(10))
    elif neuron_count.value() > 10000:
        warnings.append("Neuron count very high, may impact performance")
    
    # Validate region count
    region_count = config.get<int64_t>("region_count")
    if not region_count.has_value() or region_count.value() < 1:
        warnings.append("Region count must be at least 1")
        config.set("region_count", static_cast<int64_t>(1))
    
    # Validate connection probability
    conn_prob = config.get<double>("connection_probability")
    if not conn_prob.has_value() or conn_prob.value() < 0.0 or conn_prob.value() > 1.0:
        warnings.append("Connection probability must be between 0.0 and 1.0")
        config.set("connection_probability", 0.1)
    
    # Validate feature flag consistency
    enable_features = {
        "working_memory": "enable_working_memory",
        "episodic_memory": "enable_episodic_memory",
        "associative_memory": "enable_associative_memory",
        "curiosity": "enable_curiosity",
        "development": "enable_development",
        "structural_plasticity": "enable_structural_plasticity",
        "reward_modulation": "enable_reward_modulation"
    }
    
    for feature_name, config_key in enable_features.items() {
        if config.get<bool>(config_key).value_or(false) {
            # Enable dependent features
            if feature_name == "working_memory" and !config.get<bool>("enable_associative_memory").value_or(false) {
                warnings.append("Working memory requires associative memory - enabling automatically")
                config.set("enable_associative_memory", True)
            }
        }
    }
    
    # Check for potential conflicts
    if config.get<bool>("enable_curiosity").value_or(false) && 
       !config.get<bool>("enable_reward_modulation").value_or(false) {
        warnings.append("Curiosity works best with reward modulation - enabling automatically")
        config.set("enable_reward_modulation", True)
    }
    
    # Final validation
    is_valid = validate_configuration(config)
    
    return is_valid, config, warnings

# Example usage
config = pynlm.create_preset_config("advanced")
# Add some problematic values
config.set("neuron_count", 5)  # Too low
config.set("region_count", 0)  # Invalid
config.set("connection_probability", 1.5)  # Invalid

# Validate and fix
is_valid, fixed_config, warnings = validate_and_fix_config(config)
if warnings:
    print("Warnings fixed configuration:")
    for warning in warnings:
        print(f"  - {warning}")

brain = pynlm.createBrain(fixed_config)
brain.initialize()
```

### Performance Recommendations

```python
import pynlm

def get_performance_recommendations(config):
    """
    Get performance recommendations based on configuration.
    
    Args:
        config: Configuration to analyze
    
    Returns:
        Dictionary of performance recommendations
    """
    recommendations = {
        "expected_simulation_time": "unknown",
        "memory_usage": "unknown",
        "performance_tips": [],
        "warnings": []
    }
    
    neuron_count = config.get<int64_t>("neuron_count").value_or(100)
    region_count = config.get<int64_t>("region_count").value_or(1)
    
    # Estimate simulation time
    if neuron_count > 1000:
        recommendations["expected_simulation_time"] = "slow (>10 seconds per step)"
        recommendations["performance_tips"].append("Consider reducing neuron_count for faster simulation")
    elif neuron_count > 500:
        recommendations["expected_simulation_time"] = "moderate (2-10 seconds per step)"
        recommendations["performance_tips"].append("Enable structural_plasticity only if needed")
    else:
        recommendations["expected_simulation_time"] = "fast (<2 seconds per step)"
    
    # Memory usage estimates
    base_memory = 10  # MB per 100 neurons
    estimated_memory = neuron_count * base_memory / 100
    if estimated_memory > 500:
        recommendations["memory_usage"] = "high (>500 MB)"
        recommendations["performance_tips"].append("Enable checkpointing for long simulations")
    elif estimated_memory > 200:
        recommendations["memory_usage"] = "moderate (200-500 MB)"
    else:
        recommendations["memory_usage"] = "low (<200 MB)"
    
    # Feature-specific recommendations
    if config.get<bool>("enable_working_memory").value_or(false):
        recommendations["performance_tips"].append("Working memory increases memory usage")
    
    if config.get<bool>("enable_curiosity").value_or(false):
        recommendations["performance_tips"].append("Curiosity adds computational overhead")
    
    if config.get<bool>("enable_structural_plasticity").value_or(false):
        recommendations["performance_tips"].append("Structural plasticity significantly increases computation time")
    
    return recommendations

# Example usage
config = pynlm.create_preset_config("advanced")
recommendations = get_performance_recommendations(config)

print("Performance Recommendations:")
print(f"  Expected simulation time: {recommendations['expected_simulation_time']}")
print(f"  Estimated memory usage: {recommendations['memory_usage']}")
print("  Tips:")
for tip in recommendations['performance_tips']:
    print(f"    - {tip}")
```

## Configuration Best Practices

### 1. Start with Presets
```python
# Always start with a preset for your use case
if use_case == "research":
    config = pynlm.create_preset_config("advanced")
elif use_case == "prototype":
    config = pynlm.create_preset_config("standard")
else:
    config = pynlm.create_preset_config("minimal")
```

### 2. Validate Before Use
```python
# Always validate configuration
is_valid, validation_msg = pynlm.validate_configuration(config)
if not is_valid:
    raise ValueError(f"Invalid configuration: {validation_msg}")
```

### 3. Monitor Performance
```python
# Monitor configuration performance
config_monitor = pynlm.create_config_monitor(config)
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        stats = config_monitor.get_performance_stats(step)
        print(f"Step {step}: {stats['simulation_time']:.2f}s")
```

### 4. Use Appropriate Timesteps
```python
# Choose timestep based on simulation needs
if simulating_rapid_events:
    config.set("simulation_timestep", 0.001)
elif simulating_slow_processes:
    config.set("simulation_timestep", 0.1)
else:
    config.set("simulation_timestep", 0.01)
```

### 5. Balance Complexity and Performance
```python
# Balance computational resources with simulation fidelity
if system_resources_limited:
    # Reduce complexity
    config.set("neuron_count", 100)
    config.set("enable_structural_plasticity", False)
    config.set("enable_development", False)
else:
    # Use full features
    config = pynlm.create_preset_config("advanced")
```

## Migration Between Configurations

### From Minimal to Standard
```python
def migrate_minimal_to_standard(original_config):
    """
    Migrate from minimal to standard configuration while preserving settings.
    """
    new_config = pynlm.create_preset_config("standard")
    
    # Preserve custom settings
    if original_config.get<bool>("enable_working_memory").has_value():
        new_config.set("enable_working_memory", 
                      original_config.get<bool>("enable_working_memory").value())
    
    # Add any custom parameters
    for key in original_config.get_all_keys():
        if key not in ["neuron_count", "region_count", "connection_probability"]:
            if original_config.has_value(key):
                new_config.set(key, original_config.get(key))
    
    return new_config
```

### Scaling Configuration
```python
def scale_configuration(base_config, scale_factor=1.0):
    """
    Scale configuration parameters by a factor.
    
    Args:
        base_config: Base configuration to scale
        scale_factor: Factor to scale parameters by (default 1.0)
    
    Returns:
        Scaled configuration
    """
    scaled_config = pynlm.createDefaultConfig()
    
    # Scale core parameters
    base_neuron_count = base_config.get<int64_t>("neuron_count").value_or(100)
    scaled_config.set("neuron_count", static_cast<int64_t>(base_neuron_count * scale_factor))
    
    base_region_count = base_config.get<int64_t>"region_count").value_or(1)
    scaled_config.set("region_count", static_cast<int64_t>(base_region_count * scale_factor))
    
    # Copy other parameters
    for key in base_config.get_all_keys():
        if key not in ["neuron_count", "region_count"]:
            if base_config.has_value(key):
                scaled_config.set(key, base_config.get(key))
    
    return scaled_config

# Example: Scale up for more complex simulations
large_config = scale_configuration(pynlm.create_preset_config("standard"), 2.0)
```

## Conclusion

Configuration presets in NLM provide a powerful way to:

1. **Get Started Quickly**: Use presets for immediate results
2. **Maintain Consistency**: Presets ensure consistent parameter settings
3. **Scale Systems**: Scale configurations for different requirements
4. **Document Experiments**: Presets provide clear documentation of setup
5. **Share Configurations**: Presets can be easily shared between researchers

The configuration system balances ease of use with flexibility, allowing both beginners and experts to work effectively with NLM's complex brain simulation capabilities.