# NLM Configuration Guide

## Overview

This guide provides comprehensive instructions for configuring NLM for different experimental scenarios. Configuration is central to NLM's functionality, affecting neural dynamics, learning, memory, and development. Understanding configuration options allows you to tailor NLM to your specific research needs.

## Configuration Philosophy

### 1. Hierarchical Structure

NLM uses a hierarchical configuration system:

```
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.region_count = 2
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0

plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.structural.enable = true

neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

memory.working.enable = true
memory.episodic.enable = true
memory.associative.enable = true

prediction.enable = true
prediction.horizon = 10
prediction.confidence_threshold = 0.5

development.enable = true
development.stage_duration = 100
```

### 2. Configuration Types

#### Core Configuration
Basic settings that define the system:
- Number of neurons and regions
- Connection patterns
- Basic neuron parameters

#### Dynamic Configuration
Runtime parameters that can change:
- Learning rates
- Neuromodulation levels
- Memory capacities

#### Experimental Configuration
Settings specific to experimental paradigms:
- Environment parameters
- Task specifications
- Reward structures

## Configuration Methods

### Method 1: Python Configuration API (Recommended)

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.15f)

# Enable systems
config.set("enable_prediction_system", True)
config.set("enable_working_memory", True)
config.set("enable_episodic_memory", True)
config.set("enable_associative_memory", True)

# Set learning parameters
config.set("plasticity.stdp.learning_rate", 0.005f)
config.set("plasticity.stdp.tau_plus", 15.0f)
config.set("plasticity.stdp.tau_minus", 25.0f)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 2.0f)
config.set("neuromod.curiosity.threshold", 0.3f)
config.set("neuromod.novelty.sensitivity", 0.8f)

# Save configuration
config.saveToFile("my_config.json")
```

### Method 2: JSON Configuration Files

Create `config.json`:

```json
{
    "brain": {
        "neuron_count": 1500,
        "region_count": 2,
        "connection_probability": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0,
            "weight_max": 1.0,
            "weight_min": -0.5
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0001f
        },
        "structural": {
            "enable": true,
            "growth_rate": 0.01,
            "pruning_rate": 0.005
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0,
            "decay_rate": 0.01
        },
        "curiosity": {
            "enable": true,
            "threshold": 0.5,
            "decay": 0.02
        },
        "novelty": {
            "enable": true,
            "sensitivity": 1.0,
            "decay": 0.01
        }
    },
    "memory": {
        "working": {
            "enable": true,
            "capacity": 50,
            "decay_rate": 0.01
        },
        "episodic": {
            "enable": true,
            "max_episodes": 100,
            "replay_interval": 100
        },
        "associative": {
            "enable": true,
            "pattern_threshold": 0.7,
            "interference": "competitive"
        }
    },
    "prediction": {
        "enable": true,
        "horizon": 10,
        "confidence_threshold": 0.5,
        "prediction_decay": 0.05
    },
    "development": {
        "enable": true,
        "stage_duration": 100,
        "plasticity_decay": 0.1,
        "critical_period": 50
    }
}
```

Load it:

```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
```

### Method 3: Programmatic Configuration

```python
import pynlm

def create_research_config(research_type, parameters=None):
    """Create configuration for specific research types"""
    
    if parameters is None:
        parameters = {}
    
    config = pynlm.createDefaultConfig()
    
    if research_type == "basic_learning":
        config.set("brain.neuron_count", 500)
        config.set("brain.connection_probability", 0.05)
        config.set("enable_prediction_system", False)
        
    elif research_type == "memory_research":
        config.set("brain.neuron_count", 2000)
        config.set("brain.region_count", 3)
        config.set("enable_working_memory", True)
        config.set("enable_episodic_memory", True)
        config.set("memory.working.capacity", 100)
        config.set("memory.episodic.max_episodes", 500)
        
    elif research_type == "neuromodulation_research":
        config.set("brain.neuron_count", 1000)
        config.set("enable_curiosity", True)
        config.set("enable_novelty_detection", True)
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("neuromod.curiosity.threshold", 0.3)
        
    elif research_type == "development_research":
        config.set("brain.neuron_count", 800)
        config.set("enable_development", True)
        config.set("development.stage_duration", 200)
        config.set("development.critical_period", 100)
        
    elif research_type == "advanced_research":
        config.set("brain.neuron_count", 5000)
        config.set("brain.region_count", 5)
        config.set("brain.connection_probability", 0.2)
        config.set("enable_prediction_system", True)
        config.set("enable_working_memory", True)
        config.set("enable_episodic_memory", True)
        config.set("enable_associative_memory", True)
        config.set("enable_neuromodulation", True)
        config.set("enable_structural_plasticity", True)
        config.set("enable_development", True)
        
        # Advanced parameters
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.stdp.tau_plus", 10.0)
        config.set("plasticity.stdp.tau_minus", 30.0)
        config.set("memory.working.capacity", 200)
        config.set("memory.episodic.replay_interval", 50)
    
    # Override with custom parameters
    for key, value in parameters.items():
        set_nested_config(config, key, value)
    
    return config

def set_nested_config(config, key, value):
    """Set nested configuration parameter"""
    if "." in key:
        parts = key.split(".")
        current = config
        for part in parts[:-1]:
            if not hasattr(current, part):
                setattr(current, part, {})
            current = getattr(current, part)
        setattr(current, parts[-1], value)
    else:
        config.set(key, value)
```

## Configuration Parameters

### Brain Configuration

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons | 500-10000 |
| `brain.region_count` | int | 2 | Number of brain regions | 1-10 |
| `brain.connection_probability` | float | 0.1 | Probability of connection between neurons | 0.01-0.3 |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) | -60 to -40 |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) | -65 to -75 |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) | -80 to -70 |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) | 10-50 |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) | 1-5 |

### Plasticity Configuration

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP | true/false |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate | 0.0001-0.01 |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (positive) | 10-40 |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (negative) | 10-40 |
| `plasticity.stdp.weight_max` | float | 1.0 | Maximum synaptic weight | 0.5-2.0 |
| `plasticity.stdp.weight_min` | float | -0.5 | Minimum synaptic weight | -1.0-0.0 |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning | true/false |
| `plasticity.hebbian.learning_rate` | float | 0.0001 | Hebbian learning rate | 0.00001-0.001 |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity | true/false |
| `plasticity.structural.growth_rate` | float | 0.01 | Synapse formation rate | 0.001-0.05 |
| `plasticity.structural.pruning_rate` | float | 0.005 | Synapse removal rate | 0.001-0.01 |

### Neuromodulation Configuration

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale | 0.5-2.0 |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration | true/false |
| `neuromod.curiosity.threshold` | float | 0.5 | Curiosity activation threshold | 0.1-0.9 |
| `neuromod.curiosity.decay` | float | 0.02 | Curiosity decay rate | 0.01-0.05 |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection | true/false |
| `neuromod.novelty.sensitivity` | float | 1.0 | Novelty detection sensitivity | 0.5-2.0 |
| `neuromod.novelty.decay` | float | 0.01 | Novelty decay rate | 0.005-0.02 |

### Memory Configuration

#### Working Memory

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `memory.working.enable` | bool | true | Enable working memory | true/false |
| `memory.working.capacity` | int | 50 | Maximum working memory items | 20-200 |
| `memory.working.decay_rate` | float | 0.01 | Working memory decay rate | 0.001-0.05 |

#### Episodic Memory

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `memory.episodic.enable` | bool | true | Enable episodic memory | true/false |
| `memory.episodic.max_episodes` | int | 100 | Maximum stored episodes | 50-1000 |
| `memory.episodic.replay_interval` | int | 100 | Memory replay frequency | 10-500 |
| `memory.episodic.replay_strength` | float | 0.8 | Replay consolidation strength | 0.5-1.0 |

#### Associative Memory

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `memory.associative.enable` | bool | true | Enable associative memory | true/false |
| `memory.associative.pattern_threshold` | float | 0.7 | Pattern matching threshold | 0.5-0.9 |
| `memory.associative.interference` | string | "competitive" | Memory interference type | "competitive", "additive", "none" |

### Prediction Configuration

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `prediction.enable` | bool | true | Enable prediction system | true/false |
| `prediction.horizon` | int | 10 | Prediction time horizon | 5-50 |
| `prediction.confidence_threshold` | float | 0.5 | Action selection threshold | 0.1-0.9 |
| `prediction.prediction_decay` | float | 0.05 | Prediction decay rate | 0.01-0.1 |
| `prediction.error_gain` | float | 1.0 | Prediction error gain | 0.5-2.0 |

### Development Configuration

| Parameter | Type | Default | Description | Recommended Range |
|-----------|------|---------|-------------|-------------------|
| `development.enable` | bool | true | Enable development system | true/false |
| `development.stage_duration` | int | 100 | Default developmental stage duration | 50-500 |
| `development.plasticity_decay` | float | 0.1 | Plasticity decay with development | 0.05-0.2 |
| `development.critical_period` | int | 50 | Critical period length | 20-100 |
| `development.maturation_rate` | float | 0.01 | Rate of capability maturation | 0.001-0.05 |

## Configuration Templates

### Template 1: Basic Cognitive Development

```python
def create_basic_development_config():
    """Template for studying basic cognitive development"""
    
    config = pynlm.createDefaultConfig()
    
    # Core brain parameters
    config.set("brain.neuron_count", 800)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.08)
    
    # Essential learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Memory systems
    config.set("memory.working.enable", True)
    config.set("memory.working.capacity", 30)
    config.set("memory.episodic.enable", True)
    config.set("memory.episodic.max_episodes", 50)
    
    # Neuromodulation for learning
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    
    # Development
    config.set("development.enable", True)
    config.set("development.stage_duration", 150)
    
    return config
```

### Template 2: Advanced Cognitive Research

```python
def create_advanced_research_config():
    """Template for advanced cognitive research"""
    
    config = pynlm.createDefaultConfig()
    
    # Large-scale brain
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.15)
    
    # Comprehensive learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    
    # Full memory systems
    config.set("memory.working.enable", True)
    config.set("memory.working.capacity", 100)
    config.set("memory.episodic.enable", True)
    config.set("memory.episodic.max_episodes", 200)
    config.set("memory.associative.enable", True)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.3)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 1.2)
    
    # Prediction and planning
    config.set("prediction.enable", True)
    config.set("prediction.horizon", 20)
    config.set("prediction.confidence_threshold", 0.6)
    
    # Extended development
    config.set("development.enable", True)
    config.set("development.stage_duration", 300)
    config.set("development.critical_period", 80)
    
    return config
```

### Template 3: Neuromodulation Experiment

```python
def create_neuromodulation_experiment_config(variant="baseline"):
    """Template for neuromodulation experiments"""
    
    config = pynlm.createDefaultConfig()
    
    # Fixed parameters
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    
    # Neuromodulation manipulation
    if variant == "high_curiosity":
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.threshold", 0.2)
        config.set("neuromod.curiosity.decay", 0.03)
    
    elif variant == "high_novelty":
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 2.0)
        config.set("neuromod.novelty.decay", 0.005)
    
    elif variant == "high_dopamine":
        config.set("neuromod.dopamine.scale", 2.0)
    
    elif variant == "no_modulation":
        config.set("neuromod.curiosity.enable", False)
        config.set("neuromod.novelty.enable", False)
    
    # Control for other systems
    config.set("memory.working.enable", True)
    config.set("prediction.enable", True)
    
    return config
```

## Configuration Validation

### 1. Parameter Consistency Check

```python
def validate_configuration(config):
    """Validate configuration for consistency"""
    
    errors = []
    warnings = []
    
    # Check neuron count vs region count
    neuron_count = config.get("brain.neuron_count")
    region_count = config.get("brain.region_count")
    
    if neuron_count < region_count:
        errors.append("Neuron count must be >= region count")
    
    if neuron_count > 10000:
        warnings.append("Large neuron count may impact performance")
    
    # Check memory capacity
    wm_capacity = config.get("memory.working.capacity")
    em_max = config.get("memory.episodic.max_episodes")
    
    if wm_capacity < 10:
        warnings.append("Working memory capacity seems too low")
    
    # Check learning rates
    stdp_rate = config.get("plasticity.stdp.learning_rate")
    hebbian_rate = config.get("plasticity.hebbian.learning_rate")
    
    if stdp_rate > 0.01:
        warnings.append("STDP learning rate is high - may cause instability")
    
    if hebbian_rate > stdp_rate * 10:
        warnings.append("Hebbian rate much higher than STDP rate")
    
    # Check development parameters
    if config.get("development.enable"):
        stage_duration = config.get("development.stage_duration")
        critical_period = config.get("development.critical_period")
        
        if critical_period > stage_duration * 0.8:
            warnings.append("Critical period too long relative to stage duration")
    
    return errors, warnings

# Validate configuration
config = create_advanced_research_config()
errors, warnings = validate_configuration(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
    
if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

### 2. Configuration Testing

```python
def test_configuration(config, test_scenarios):
    """Test configuration with various scenarios"""
    
    results = {}
    
    for scenario_name, scenario_config in test_scenarios.items():
        print(f"Testing scenario: {scenario_name}")
        
        # Apply scenario
        scenario_config.apply_to(config)
        
        # Test initialization
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run quick test
            success = test_brain_initialization(brain, scenario_config)
            
            results[scenario_name] = {
                "success": success,
                "brain": brain if success else None,
                "error": None if success else "Test failed"
            }
            
        except Exception as e:
            results[scenario_name] = {
                "success": False,
                "brain": None,
                "error": str(e)
            }
    
    return results

# Define test scenarios
scenarios = {
    "minimal": {
        "brain.neuron_count": 100,
        "brain.region_count": 1,
        "enable_prediction_system": False,
        "enable_working_memory": False,
        "enable_episodic_memory": False,
        "enable_associative_memory": False,
        "enable_curiosity": False,
        "enable_novelty_detection": False,
        "enable_development": False,
    },
    "balanced": {
        "brain.neuron_count": 1000,
        "brain.connection_probability": 0.1,
        "enable_prediction_system": True,
        "enable_working_memory": True,
        "enable_episodic_memory": True,
        "enable_associative_memory": False,
        "enable_curiosity": True,
        "enable_novelty_detection": True,
        "enable_development": True,
    },
    "comprehensive": {
        "brain.neuron_count": 3000,
        "brain.region_count": 3,
        "brain.connection_probability": 0.15,
        "enable_prediction_system": True,
        "enable_working_memory": True,
        "enable_episodic_memory": True,
        "enable_associative_memory": True,
        "enable_curiosity": True,
        "enable_novelty_detection": True,
        "enable_development": True,
        "plasticity.stdp.learning_rate": 0.005,
        "plasticity.hebbian.learning_rate": 0.001,
    }
}
```

## Configuration Optimization

### 1. Performance Optimization

```python
def optimize_configuration_performance(config):
    """Optimize configuration for performance"""
    
    optimizations = []
    
    # Optimize neuron count for performance
    neuron_count = config.get("brain.neuron_count")
    if neuron_count < 500:
        optimizations.append("Increase neuron count for better cognitive capabilities")
    
    # Optimize connection probability
    conn_prob = config.get("brain.connection_probability")
    if conn_prob < 0.05:
        optimizations.append("Increase connection probability for better integration")
    
    # Memory optimization
    if config.get("memory.episodic.max_episodes") > 500:
        optimizations.append("Consider memory replay optimization")
    
    # Learning rate optimization
    stdp_rate = config.get("plasticity.stdp.learning_rate")
    if stdp_rate > 0.01:
        optimizations.append("Reduce STDP learning rate for stability")
    
    return optimizations

# Get optimizations
optimizations = optimize_configuration_performance(config)
for opt in optimizations:
    print(f"Optimization: {opt}")
```

### 2. Scientific Optimization

```python
def optimize_configuration_science(config, research_hypothesis):
    """Optimize configuration for scientific research"""
    
    # Hypothesis-specific optimizations
    if "learning_rate" in research_hypothesis.lower():
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.learning_rate", 0.0001)
        
    elif "memory" in research_hypothesis.lower():
        config.set("memory.working.capacity", 50)
        config.set("memory.episodic.max_episodes", 100)
        config.set("memory.episodic.replay_interval", 100)
        
    elif "development" in research_hypothesis.lower():
        config.set("development.stage_duration", 150)
        config.set("development.critical_period", 50)
        
    elif "neuromodulation" in research_hypothesis.lower():
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.threshold", 0.5)
        config.set("neuromod.novelty.sensitivity", 1.0)
        
    elif "prediction" in research_hypothesis.lower():
        config.set("prediction.horizon", 10)
        config.set("prediction.confidence_threshold", 0.5)
    
    return config
```

## Configuration Troubleshooting

### 1. "Configuration Takes Too Long to Initialize"

**Symptoms**: Slow brain initialization or memory issues.

**Causes**:
- Too many neurons or regions
- Memory settings too high
- Multiple systems enabled

**Solutions**:
1. Reduce neuron count (1000-2000 for most experiments)
2. Disable unused memory systems
3. Turn off development if not needed
4. Use `enable_advanced_plasticity=false`

### 2. "Brain Doesn't Learn Anything"

**Symptoms**: No changes in behavior or neural activity.

**Causes**:
- Learning rates too low
- No reward signals
- Memory systems disabled
- Too simple environment

**Solutions**:
1. Increase learning rates:
   ```python
   config.set("plasticity.stdp.learning_rate", 0.005)
   config.set("plasticity.hebbian.learning_rate", 0.001)
   ```
2. Enable neuromodulation for learning
3. Add meaningful rewards
4. Create complex environment

### 3. "Configuration Inconsistent"

**Symptoms**: Parameter conflicts or errors during validation.

**Causes**:
- Neuron count < region count
- Memory capacity too low
- Learning rates too high

**Solutions**:
1. Run validation check:
   ```python
   errors, warnings = validate_configuration(config)
   ```
2. Fix reported issues
3. Consider trade-offs between performance and scientific goals

### 4. "Python Configuration Issues"

**Symptoms**: Python errors accessing configuration.

**Causes**:
- Wrong parameter names
- Missing Python bindings
- Configuration not properly loaded

**Solutions**:
1. Check parameter names in `HOW_TO_USE.md`
2. Install Python bindings properly
3. Test configuration with minimal example

## Configuration Management Best Practices

### 1. Backup Configurations

```python
def backup_configuration(config, base_path="configs/"):
    """Save configuration with descriptive name"""
    
    import datetime
    import json
    
    # Generate filename
    timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"{base_path}config_{timestamp}.json"
    
    # Save configuration
    config.saveToFile(filename)
    
    # Also save metadata
    metadata = {
        "name": f"Config_{timestamp}",
        "neuron_count": config.get("brain.neuron_count"),
        "systems_enabled": get_enabled_systems(config),
        "research_purpose": "TODO: add research purpose",
        "date_created": timestamp,
    }
    
    metadata_filename = filename.replace(".json", "_meta.json")
    with open(metadata_filename, 'w') as f:
        json.dump(metadata, f, indent=2)
    
    return filename, metadata
```

### 2. Configuration Repository

```python
# Create configuration management script
configs = {
    "basic": "configs/basic_config.json",
    "development": "configs/development_config.json", 
    "neuromodulation": "configs/neuromod_config.json",
    "prediction": "configs/prediction_config.json",
    "advanced": "configs/advanced_config.json",
    "research_1": "configs/research_1_config.json",
    "research_2": "configs/research_2_config.json",
}

def list_configurations():
    """List available configurations"""
    print("Available NLM configurations:")
    for name, path in configs.items():
        try:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(path)
            print(f"  {name:20} - {config.get('brain.neuron_count'):5d} neurons")
        except:
            print(f"  {name:20} - ERROR loading file")

def load_configuration(name):
    """Load configuration by name"""
    if name in configs:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(configs[name])
        return config
    else:
        print(f"Configuration '{name}' not found")
        return None
```

## Configuration Case Studies

### Case Study 1: Working Memory Capacity Experiment

**Research Question**: How does working memory capacity affect learning?

**Configuration Used**:
```python
config = create_working_memory_experiment_config()

# Vary working memory capacity
capacities = [10, 20, 30, 50, 100]

for capacity in capacities:
    test_config = config.copy()
    test_config.set("memory.working.capacity", capacity)
    
    # Run experiment
    results = run_experiment("WM_capacity", test_config)
    
    # Analyze effect of capacity
    analyze_working_memory_effect(results, capacity)
```

### Case Study 2: Neuromodulation Comparison

**Research Question**: How do different neuromodulators affect exploration?

**Configuration Used**:
```python
# Create neuromodulation variants
variants = [
    "baseline",           # Normal neuromodulation
    "high_curiosity",     # Enhanced curiosity
    "high_novelty",       # Enhanced novelty detection
    "high_dopamine",      # Enhanced dopamine
    "no_modulation",      # Neuromodulation disabled
]

for variant in variants:
    config = create_neuromodulation_experiment_config(variant)
    
    # Run identical experiment across all variants
    results[variant] = run_experiment("neuromod_comparison", config)
```

### Case Study 3: Development Timeline

**Research Question**: How do developmental changes affect learning?

**Configuration Used**:
```python
# Create development configurations
stages = [
    {"name": "juvenile", "duration": 100, "plasticity": 1.0},
    {"name": "adolescent", "duration": 100, "plasticity": 0.7},
    {"name": "adult", "duration": 100, "plasticity": 0.5},
    {"name": "aging", "duration": 100, "plasticity": 0.3},
]

for stage in stages:
    config = create_development_stage_config(stage)
    
    # Run development experiment
    results[stage["name"]] = run_development_experiment(config, stage["duration"])
```

## Conclusion

Configuration is a powerful tool in NLM research. It allows you to:

1. **Control Variables**: Isolate and test specific aspects of neural systems
2. **Scale Experiments**: From simple to highly complex configurations
3. **Reproduce Results**: Use consistent configurations across studies
4. **Optimize Performance**: Balance scientific goals with computational constraints
5. **Test Hypotheses**: Systematically manipulate parameters to test theories

The key is to:

- **Start Simple**: Begin with basic configurations
- **Understand Parameters**: Know what each setting does
- **Validate Rigorously**: Check for consistency and potential issues
- **Document Thoroughly**: Record configuration choices and rationale
- **Iterate Continuously**: Refine based on experimental results

Configuration expertise develops through practice. Don't be afraid to experiment and discover what works best for your research questions!

---

*Configuration Resources*:
- `HOW_TO_USE.md` - Complete Python configuration API
- `examples/` - Working configuration examples
- `configs/` - Predefined configuration files
- Configuration validation scripts
- Performance optimization tools

*Next Configuration Steps*:
1. **Try basic configurations**: Use `create_basic_development_config()`
2. **Test with simple experiments**: Verify configuration works
3. **Customize for your research**: Modify for your specific questions
4. **Document your work**: Save and share configuration files
5. **Contribute to community**: Add configuration templates