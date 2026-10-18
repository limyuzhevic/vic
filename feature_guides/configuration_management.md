# NLM Feature Guide: Configuration Management

**Configuration Management Guide**

This guide covers advanced configuration techniques for NLM, from basic setup to complex parameter optimization.

---

## Overview

NLM's configuration system is hierarchical and highly flexible, allowing precise control over every aspect of the brain simulation. Configuration is managed through nested key-value pairs stored in a Config object.

### Configuration Structure

```
NLM Configuration Hierarchy:
├── brain                    # Core brain parameters
│   ├── neuron_count        # Number of neurons
│   ├── synapse_density     # Connection probability
│   ├── v_thresh           # Membrane threshold
│   ├── tau_mem            # Membrane time constant
│   └── tau_ref            # Refractory period
├── plasticity              # Learning rules
│   ├── stdp.enable        # STDP enabled?
│   ├── stdp.learning_rate # STDP learning rate
│   ├── hebbian.enable     # Hebbian enabled?
│   └── structural.enable  # Structural plasticity
├── neuromodulation         # Modulators
│   ├── dopamine.scale     # Dopamine effect
│   ├── curiosity.enabled # Curiosity enabled?
│   └── novelty.enabled    # Novelty detection?
└── development            # Growth stages
    ├── enable           # Development enabled?
    └── plasticity_rates # Age-dependent learning
```

---

## Configuration Types

### 1. Brain Configuration

#### Core Neural Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `brain.neuron_count` | int | 1000 | 10-100000 | Total neurons in brain |
| `brain.synapse_density` | float | 0.1 | 0.01-1.0 | Probability of synapse formation |
| `brain.connection_probability` | float | 0.05 | 0.001-0.5 | Specific connection probability |
| `brain.initial_weight_mean` | float | 0.5 | -1.0-1.0 | Initial synaptic weight mean |
| `brain.initial_weight_std` | float | 0.1 | 0.0-1.0 | Initial synaptic weight std dev |

#### Neural Dynamics Parameters

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `brain.v_thresh` | float | -50.0 | -100.0 to -10.0 | Membrane potential threshold (mV) |
| `brain.v_rest` | float | -70.0 | -90.0 to -50.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | -80.0 to -60.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | 5.0-100.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | 0.5-10.0 | Refractory period (ms) |

### 2. Plasticity Configuration

#### STDP (Spike-Timing-Dependent Plasticity)

```python
# STDP Configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.stdp.max_weight", 1.0)
config.set("plasticity.stdp.min_weight", -1.0)
```

**STDP Parameters:**

| Parameter | Type | Default | Range | Effect |
|-----------|------|---------|-------|--------|
| `plasticity.stdp.learning_rate` | float | 0.001 | 0.0001-0.01 | Overall learning strength |
| `plasticity.stdp.tau_plus` | float | 20.0 | 10.0-100.0 | LTP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | 10.0-100.0 | LTD time constant |

#### Hebbian Learning

```python
# Hebbian Configuration
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.0005)
config.set("plasticity.hebbian.threshold", 0.5)
config.set("plasticity.hebbian.normalization", True)
```

**Hebbian Parameters:**

| Parameter | Type | Default | Range | Effect |
|-----------|------|---------|-------|--------|
| `plasticity.hebbian.learning_rate` | float | 0.0005 | 0.0001-0.01 | Learning strength |
| `plasticity.hebbian.threshold` | float | 0.5 | 0.0-1.0 | Co-activation threshold |

#### Structural Plasticity

```python
# Structural Plasticity Configuration
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.01)
config.set("plasticity.structural.pruning_rate", 0.005)
config.set("plasticity.structural.min_synapses", 100)
config.set("plasticity.structural.max_synapses", 10000)
```

**Structural Parameters:**

| Parameter | Type | Default | Range | Effect |
|-----------|------|---------|-------|--------|
| `plasticity.structural.synaptogenesis_rate` | float | 0.01 | 0.001-0.1 | New synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.005 | 0.001-0.05 | Synapse removal |

### 3. Neuromodulation Configuration

#### Dopamine System

```python
# Dopamine Configuration
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.dopamine.baseline", 0.1)
config.set("neuromod.dopamine.learning_impact", 0.8)
```

#### Curiosity System

```python
# Curiosity Configuration
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.sensitivity", 0.5)
config.set("neuromod.curiosity.energy_cost", 0.01)
config.set("neuromod.curiosity.exploration_bias", 0.3)
```

#### Novelty Detection

```python
# Novelty Configuration
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.3)
config.set("neuromod.novelty.memory_depth", 10)
config.set("neuromod.novelty.decay_rate", 0.1)
```

### 4. Development Configuration

#### Developmental Stages

```python
# Development Configuration
config.set("development.enable", True)
config.set("development.initial_stage", "Initial")
config.set("development.stage_duration", 100)
config.set("development.plasticity_decay_rate", 0.001)
config.set("development.critical_period_start", 10)
config.set("development.critical_period_end", 50)
```

## Advanced Configuration Techniques

### 1. Dynamic Configuration

```python
import pynlm

# Create brain with dynamic configuration
config = pynlm.createDefaultConfig()

# Initial configuration
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Runtime modification (affects next simulation steps)
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Environment-Based Configuration

```python
# Configure based on environmental factors
def configure_for_environment(env_type):
    config = pynlm.createDefaultConfig()
    
    if env_type == "simple":
        config.set("brain.neuron_count", 500)
        config.set("neuromod.curiosity.sensitivity", 0.3)
        config.set("neuromod.novelty.threshold", 0.5)
    
    elif env_type == "complex":
        config.set("brain.neuron_count", 5000)
        config.set("neuromod.curiosity.sensitivity", 0.8)
        config.set("neuromod.novelty.threshold", 0.2)
        config.set("plasticity.stdp.learning_rate", 0.002)
    
    elif env_type == "dangerous":
        config.set("brain.neuron_count", 1000)
        config.set("neuromod.curiosity.sensitivity", 0.2)
        config.set("neuromod.novelty.threshold", 0.7)
        config.set("plasticity.structural.pruning_rate", 0.02)
    
    return config
```

### 3. Performance-Optimized Configuration

```python
# Configuration for different performance targets

def optimize_for_performance(target_performance):
    config = pynlm.createDefaultConfig()
    
    if target_performance == "speed":
        # Fast, less detailed simulation
        config.set("brain.neuron_count", 100)
        config.set("plasticity.structural.enable", False)
        config.set("neuromod.curiosity.enable", False)
        config.set("neuromod.novelty.enable", False)
        config.set("development.enable", False)
        
    elif target_performance == "accuracy":
        # High accuracy, slower simulation
        config.set("brain.neuron_count", 10000)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.hebbian.learning_rate", 0.005)
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("neuromod.curiosity.sensitivity", 1.0)
        config.set("development.enable", True)
        
    elif target_performance == "balanced":
        # Balanced configuration
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.learning_rate", 0.0005)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.sensitivity", 0.5)
        config.set("development.enable", True)
    
    return config
```

### 4. Configuration Validation

```python
# Validate configuration settings
def validate_configuration(config):
    """Validate configuration for consistency"""
    
    errors = []
    warnings = []
    
    # Check neuron count reasonable
    neuron_count = config.get("brain.neuron_count", 1000)
    if neuron_count < 10:
        warnings.append("Very low neuron count may produce uninteresting dynamics")
    elif neuron_count > 100000:
        warnings.append("Very high neuron count may impact performance")
    
    # Check plasticity settings
    if config.get("plasticity.stdp.enable", True):
        stdp_rate = config.get("plasticity.stdp.learning_rate", 0.001)
        if stdp_rate > 0.1:
            warnings.append("STDP learning rate very high - may cause instability")
        elif stdp_rate < 0.00001:
            warnings.append("STDP learning rate very low - may cause slow learning")
    
    # Check development settings
    if config.get("development.enable", True):
        decay_rate = config.get("development.plasticity_decay_rate", 0.001)
        if decay_rate > 0.01:
            warnings.append("Development plasticity decay rate high - rapid maturation")
        elif decay_rate < 0.000001:
            warnings.append("Development plasticity decay rate low - prolonged critical periods")
    
    return errors, warnings
```

### 5. Configuration Profiles

```python
# Predefined configuration profiles

CONFIG_PROFILES = {
    "beginner": {
        "brain.neuron_count": 100,
        "plasticity.stdp.enable": False,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.enable": False
    },
    
    "research": {
        "brain.neuron_count": 5000,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": True,
        "neuromod.dopamine.scale": 1.5,
        "neuromod.curiosity.sensitivity": 1.0,
        "neuromod.novelty.threshold": 0.1,
        "development.enable": True
    },
    
    "production": {
        "brain.neuron_count": 1000,
        "plasticity.stdp.learning_rate": 0.001,
        "plasticity.hebbian.learning_rate": 0.0005,
        "neuromod.curiosity.sensitivity": 0.5,
        "neuromod.novelty.threshold": 0.3,
        "development.enable": True,
        "development.plasticity_decay_rate": 0.001
    }
}

def apply_config_profile(config, profile_name):
    """Apply a predefined configuration profile"""
    if profile_name not in CONFIG_PROFILES:
        raise ValueError(f"Unknown profile: {profile_name}")
    
    profile = CONFIG_PROFILES[profile_name]
    for key, value in profile.items():
        config.set(key, value)
    
    print(f"Applied configuration profile: {profile_name}")
    return config
```

## Configuration Best Practices

### 1. Start Simple, Then Optimize

```python
# Good practice: incremental configuration
sim = BasicBrainSimulation()

# Step 1: Basic setup
sim.config.set("brain.neuron_count", 100)
sim.config.set("neuromod.curiosity.enable", True)

# Step 2: Add complexity
sim.config.set("plasticity.stdp.enable", True)
sim.config.set("development.enable", True)

# Step 3: Fine-tune parameters
sim.config.set("plasticity.stdp.learning_rate", 0.001)
sim.config.set("neuromod.curiosity.sensitivity", 0.5)
```

### 2. Document Your Configuration

```python
# Document your experimental setup
experiment_config = {
    "description": "Testing novelty-driven learning",
    "parameters": {
        "brain.neuron_count": 2000,
        "neuromod.curiosity.sensitivity": 0.8,
        "neuromod.novelty.threshold": 0.2
    },
    "expected_outcomes": ["faster_convergence", "better_generalization"],
    "date": "2026-10-18",
    "researcher": "NLM Documentation System"
}
```

### 3. Use Configuration Templates

```python
# Configuration template for different experiments
EXPERIMENT_TEMPLATES = {
    "novelty_experiment": {
        "description": "Study effect of novelty on learning",
        "template": {
            "brain.neuron_count": 1000,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.threshold": 0.3,
            "neuromod.novelty.memory_depth": 20,
            "plasticity.stdp.enable": True,
            "development.enable": True
        }
    },
    
    "development_experiment": {
        "description": "Study developmental effects on learning",
        "template": {
            "brain.neuron_count": 500,
            "development.enable": True,
            "development.critical_period_start": 10,
            "development.critical_period_end": 50,
            "neuromod.curiosity.sensitivity": 0.7
        }
    }
}

def get_experiment_template(template_name):
    """Get a configuration template for an experiment"""
    if template_name not in EXPERIMENT_TEMPLATES:
        raise ValueError(f"Unknown template: {template_name}")
    
    return EXPERIMENT_TEMPLATES[template_name]["template"].copy()
```

## Configuration File Management

### 1. Save and Load Configurations

```python
import json
import pynlm

# Save configuration to file
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Convert to dictionary for file storage
config_dict = {}
keys = config.getKeys()
for key in keys:
    config_dict[key] = config.get(key)

# Save to JSON file
with open("experiment_config.json", "w") as f:
    json.dump(config_dict, f, indent=2)

# Load configuration from file
with open("experiment_config.json", "r") as f:
    loaded_config_dict = json.load(f)

loaded_config = pynlm.createDefaultConfig()
for key, value in loaded_config_dict.items():
    loaded_config.set(key, value)

print("Configuration saved and loaded successfully")
```

### 2. Configuration Comparison

```python
# Compare two configurations
def compare_configurations(config1, config2, config1_name="Config 1", config2_name="Config 2"):
    """Compare two configurations and show differences"""
    
    keys1 = config1.getKeys()
    keys2 = config2.getKeys()
    
    all_keys = set(keys1) | set(keys2)
    
    print(f"Configuration Comparison: {config1_name} vs {config2_name}")
    print("=" * 60)
    
    differences = []
    
    for key in sorted(all_keys):
        val1 = config1.get(key) if key in keys1 else "NOT SET"
        val2 = config2.get(key) if key in keys2 else "NOT SET"
        
        if val1 != val2:
            differences.append((key, val1, val2))
            print(f"DIFFERENCE: {key}")
            print(f"  {config1_name}: {val1}")
            print(f"  {config2_name}: {val2}")
            print()
    
    if not differences:
        print("No differences found - configurations are identical")
    
    return differences
```

### 3. Configuration Templates

```python
# Template for configuration documentation
CONFIG_TEMPLATE = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.0005
        }
    }
}

# Generate configuration documentation from template
def generate_configuration_documentation(config, output_file="config_docs.md"):
    """Generate documentation for a configuration"""
    
    with open(output_file, "w") as f:
        f.write("# Configuration Documentation\n\n")
        
        for section in ["brain", "plasticity", "neuromodulation", "development"]:
            f.write(f"## {section.title()}\n\n")
            
            if section == "brain":
                f.write("| Parameter | Value | Default | Description |\n")
                f.write("|-----------|-------|---------|-------------|\n")
                for key, value in config.get(f"{section}.neuron_count", 1000):
                    f.write(f"| `{key}` | {value} | 1000 | Number of neurons |\n")
            
            f.write("\n")
```

## Conclusion

Configuration management is a fundamental skill for NLM experimentation. By mastering configuration techniques, you can:

1. **Control simulation parameters** precisely
2. **Reproduce experiments** consistently
3. **Optimize performance** for different use cases
4. **Design systematic studies** with controlled variables
5. **Document experimental setups** for scientific rigor

**Next Steps:**

1. Practice basic configuration techniques
2. Create custom configuration profiles for your research questions
3. Learn to validate and compare configurations
4. Implement configuration templates for reproducible research

The configuration system in NLM provides the foundation for all advanced simulations and experiments. Master it, and you'll have complete control over your artificial brain simulations.🧠

---

## Interactive Configuration Playground

Try the configuration playground below (simulated):

```python
# Example configuration playground
print("=== NLM Configuration Playground ===")

# Try different configurations
configs_to_test = [
    {"name": "Minimal", "neurons": 100, "plasticity": False},
    {"name": "Basic", "neurons": 1000, "plasticity": True},
    {"name": "Advanced", "neurons": 5000, "plasticity": True, "development": True},
    {"name": "Research", "neurons": 10000, "plasticity": True, "development": True, "neuromodulation": True}
]

for config in configs_to_test:
    print(f"\nTesting {config['name']} Configuration:")
    print(f"  Neurons: {config.get('neurons', 'N/A')}")
    print(f"  Plasticity: {config.get('plasticity', 'N/A')}")
    print(f"  Development: {config.get('development', 'N/A')}")
    print(f"  Neuromodulation: {config.get('neuromodulation', 'N/A')}")

print("\n=== Configuration Analysis Complete ===")
```

**Key Takeaways:**

1. **Start simple** and incrementally add complexity
2. **Document your configurations** for reproducibility
3. **Use profiles** for common experimental setups
4. **Validate configurations** before running simulations
5. **Optimize parameters** for your specific research questions

The configuration system in NLM is designed to be flexible yet structured, allowing both simple experimentation and complex research studies. Use it effectively to control every aspect of your brain simulations! 🎯

---

*This feature guide provides comprehensive coverage of NLM configuration management. Practice these techniques to gain full control over your simulations.*

**Next Steps:**
1. **Explore Memory Systems Guide** - Learn about working, episodic, and associative memory
2. **Try Advanced Examples** - See these concepts in action
3. **Check API Reference** - Technical details for all configuration functions
4. **Read Best Practices** - Proven approaches for successful experimentation