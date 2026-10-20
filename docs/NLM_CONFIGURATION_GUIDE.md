# NLM Configuration and Troubleshooting Guide

## Configuration Options

This section provides detailed information about all available configuration options for the NLM (Neural Learning Machine) system, along with their default values, descriptions, and usage examples.

## Core Simulation Parameters

| Parameter | Type | Default | Description | Usage Example |
|-----------|------|---------|-------------|---------------|
| `neuron_count` | integer | 1000 | Total number of neurons in the brain | `config.set("neuron_count", 500)` |
| `region_count` | integer | 1 | Number of brain regions | `config.set("region_count", 4)` |
| `connection_probability` | float | 0.1 | Probability of synaptic connections (0.0-1.0) | `config.set("connection_probability", 0.15)` |
| `simulation_timestep` | float | 0.001 | Time step for simulation in seconds | `config.set("simulation_timestep", 0.002)` |

## Memory System Parameters

| Parameter | Type | Default | Description | Usage Example |
|-----------|------|---------|-------------|---------------|
| `working_memory_capacity_ratio` | float | 0.1 | Fraction of neurons for working memory capacity | `config.set("working_memory_capacity_ratio", 0.15)` |
| `max_episodes` | integer | 1000 | Maximum number of episodic memory episodes | `config.set("max_episodes", 2000)` |
| `replay_interval` | integer | 100 | Steps between memory replay operations | `config.set("replay_interval", 50)` |
| `consolidation_interval` | integer | 1000 | Steps between memory consolidation | `config.set("consolidation_interval", 500)` |

## Plasticity System Parameters

| Parameter | Type | Default | Description | Usage Example |
|-----------|------|---------|-------------|---------------|
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight | `config.set("stdp_ltp_weight", 0.015)` |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight | `config.set("stdp_ltd_weight", 0.018)` |
| `stdp_tau` | float | 20.0 | STDP time constant | `config.set("stdp_tau", 25.0)` |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation | `config.set("synaptogenesis_rate", 0.00015)` |
| `pruning_rate` | float | 0.00001 | Rate of synapse elimination | `config.set("pruning_rate", 0.000015)` |

## Neuromodulation Parameters

| Parameter | Type | Default | Description | Usage Example |
|-----------|------|---------|-------------|---------------|
| `neuromodulation_excitability_factor` | float | 0.5 | Neuromodulator effect on excitability | `config.set("neuromodulation_excitability_factor", 0.7)` |
| `neuromodulation_plasticity_factor` | float | 0.5 | Neuromodulator effect on plasticity | `config.set("neuromodulation_plasticity_factor", 0.8)` |

## Development System Parameters

| Parameter | Type | Default | Description | Usage Example |
|-----------|------|---------|-------------|---------------|
| `development_stages` | string | "Initial,CriticalPeriod,Maturation,Adult,Aging" | Developmental stages order | `config.set("development_stages", "Initial,CriticalPeriod,Maturation,Adult")` |
| `plasticity_decay_rate` | float | 0.1 | Rate of plasticity decay with age | `config.set("plasticity_decay_rate", 0.15)` |

## Sensory System Parameters

| Parameter | Type | Default | Description | Usage Example |
|-----------|------|---------|-------------|---------------|
| `vision_width` | integer | 640 | Default vision width | `config.set("vision_width", 320)` |
| `vision_height` | integer | 480 | Default vision height | `config.set("vision_height", 240)` |
| `vision_channels` | integer | 3 | Default vision channels | `config.set("vision_channels", 1)` |
| `audio_sample_rate` | integer | 44100 | Default audio sample rate | `config.set("audio_sample_rate", 22050)` |

## Advanced Configuration

### Random Seed Configuration

```python
# Set deterministic random seed for reproducibility
config.set("random_seed", 12345)

# Use system time for random seed (default)
# config.set("random_seed", 0)  # 0 uses system time
```

### Neuron Pool Distribution

```python
# Configure neuron type distribution
config.set("neuron_pools", {
    "sensory": 0.25,    # 25% sensory neurons
    "internal": 0.5,   # 50% internal/computational neurons
    "motor": 0.25      # 25% motor neurons
})
```

### Synaptic Properties

```python
# Configure synaptic properties
config.set("synaptic_weights", {
    "excitatory_mean": 0.5,
    "excitatory_std": 0.1,
    "inhibitory_mean": -0.5,
    "inhibitory_std": 0.1
})
```

## Configuration Validation

### Required Parameters

The following parameters are required for basic operation:

- `neuron_count`: Total number of neurons (> 0)
- `region_count`: Number of brain regions (> 0)
- `connection_probability`: Connection probability (0.0-1.0)

### Optional Parameters

The following parameters have sensible defaults:

- `simulation_timestep`: Simulation time step
- `replay_interval`: Memory replay interval
- `consolidation_interval`: Memory consolidation interval
- All system-specific parameters

### Validation Rules

| Parameter | Valid Range | Default on Invalid | Warning Message |
|-----------|-------------|-------------------|-----------------|
| `neuron_count` | 1-1,000,000 | 1000 | "neuron_count is 0, using default value" |
| `region_count` | 1-1000 | 1 | "region_count is 0, using default value" |
| `connection_probability` | 0.0-1.0 | 0.1 | "connection_probability out of range [0,1], using default value" |
| `simulation_timestep` | 0.0001-1.0 | 0.001 | "simulation_timestep out of range [0.0001,1.0], using default value" |
| `replay_interval` | 1-10000 | 100 | "replay_interval out of range [1,10000], using default value" |

## Configuration Templates

### Template 1: High-Performance Configuration

```python
# For systems with limited memory
high_perf_config = createDefaultConfig()
high_perf_config.set("neuron_count", 100)
high_perf_config.set("region_count", 2)
high_perf_config.set("connection_probability", 0.05)
high_perf_config.set("max_episodes", 100)
high_perf_config.set("simulation_timestep", 0.002)
```

### Template 2: High-Fidelity Configuration

```python
# For detailed simulation with high fidelity
high_fidelity_config = createDefaultConfig()
high_fidelity_config.set("neuron_count", 2000)
high_fidelity_config.set("region_count", 5)
high_fidelity_config.set("connection_probability", 0.2)
high_fidelity_config.set("max_episodes", 5000)
high_fidelity_config.set("simulation_timestep", 0.001)
high_fidelity_config.set("stdp_ltp_weight", 0.02)
high_fidelity_config.set("stdp_ltd_weight", 0.024)
```

### Template 3: Development Phase Configuration

```python
# For testing and development
dev_config = createDefaultConfig()
dev_config.set("neuron_count", 500)
dev_config.set("region_count", 3)
dev_config.set("connection_probability", 0.1)
dev_config.set("replay_interval", 50)
dev_config.set("consolidation_interval", 200)
```

### Template 4: Production Configuration

```python
# For production use with optimized settings
prod_config = createDefaultConfig()
prod_config.set("neuron_count", 1000)
prod_config.set("region_count", 4)
prod_config.set("connection_probability", 0.12)
prod_config.set("max_episodes", 2000)
prod_config.set("simulation_timestep", 0.001)
prod_config.set("random_seed", 42)  # Reproducible results
```

## Configuration File Formats

### JSON Configuration File

```json
{
    "neuron_count": 1000,
    "region_count": 4,
    "connection_probability": 0.12,
    "simulation_timestep": 0.001,
    "working_memory_capacity_ratio": 0.1,
    "max_episodes": 1000,
    "replay_interval": 100,
    "consolidation_interval": 1000,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "neuromodulation_excitability_factor": 0.5,
    "neuromodulation_plasticity_factor": 0.5,
    "random_seed": 12345
}
```

### Command Line Arguments

The following command line arguments are supported:

```bash
# Basic usage
./nlm --neuron_count 500 --region_count 2 --connection_probability 0.1

# Full configuration
./nlm --neuron_count 1000 \
     --region_count 4 \
     --connection_probability 0.12 \
     --simulation_timestep 0.001 \
     --max_episodes 2000 \
     --replay_interval 100 \
     --consolidation_interval 1000 \
     --stdp_ltp_weight 0.01 \
     --stdp_ltd_weight 0.012 \
     --random_seed 12345
```

## Configuration Best Practices

### 1. Memory vs. Performance Trade-off

```python
# For memory-constrained systems
config.set("neuron_count", 100)  # Fewer neurons
config.set("region_count", 2)    # Fewer regions

# For high-performance systems
config.set("neuron_count", 2000)  # More neurons
config.set("region_count", 8)    # More regions
```

### 2. Stability vs. Exploration

```python
# For stable, predictable behavior
config.set("connection_probability", 0.05)  # Lower connectivity
config.set("stdp_ltp_weight", 0.005)        # Lower learning rate

# For exploratory, adaptive behavior
config.set("connection_probability", 0.2)   # Higher connectivity
config.set("stdp_ltp_weight", 0.02)        # Higher learning rate
```

### 3. Development vs. Production

```python
# Development configuration (easy debugging)
config.set("neuron_count", 100)  # Small for fast iteration
config.set("random_seed", 42)    # Reproducible results

# Production configuration (optimized for performance)
config.set("neuron_count", 1000) # Larger for better behavior
# No random_seed for natural variation
```

### 4. Simulation Length Considerations

```python
# For short-term simulations
config.set("replay_interval", 10)   # Frequent replay
config.set("consolidation_interval", 100)  # Frequent consolidation

# For long-term simulations
config.set("replay_interval", 500)  # Less frequent replay
config.set("consolidation_interval", 5000)  # Less frequent consolidation
```

## Configuration Monitoring

### Track Configuration Usage

```python
# Monitor configuration usage
class ConfigurationMonitor:
    def __init__(self):
        self.config_changes = []
        self.validation_stats = []
    
    def track_change(self, key, value, timestamp):
        self.config_changes.append({
            'key': key,
            'value': value,
            'timestamp': timestamp,
            'user': 'system'  # or 'user'
        })
    
    def get_change_summary(self):
        summary = {}
        for change in self.config_changes:
            key = change['key']
            if key not in summary:
                summary[key] = {'changes': 0, 'values': set()}
            summary[key]['changes'] += 1
            summary[key]['values'].add(change['value'])
        return summary
```

### Validate Configuration

```python
# Validate configuration after changes
class ConfigurationValidator:
    def validate_config(self, config):
        errors = []
        warnings = []
        
        # Check required parameters
        if not config.has("neuron_count"):
            errors.append("neuron_count is required")
        
        # Check value ranges
        neuron_count = config.getOr("neuron_count", 0)
        if neuron_count < 1 or neuron_count > 1000000:
            warnings.append(f"neuron_count {neuron_count} out of recommended range [1, 1000000]")
        
        # Check consistency
        if config.has("region_count") and config.has("neuron_count"):
            region_count = config.getOr("region_count", 1)
            neuron_count = config.getOr("neuron_count", 1000)
            if neuron_count < region_count:
                warnings.append(f"neuron_count ({neuron_count}) < region_count ({region_count}) may cause issues")
        
        return errors, warnings
```

## Configuration Migration

### Migrate from Old Version

```python
# Migration from NLM v1.x to v2.0
old_config = {
    "num_neurons": 500,      # Old parameter name
    "num_regions": 2,        # Old parameter name
    "connectivity": 0.1,     # Old parameter name
    "memory_size": 100,      # Old memory parameter
    "plasticity_rate": 0.01  # Old plasticity parameter
}

new_config = createDefaultConfig()
new_config.set("neuron_count", old_config["num_neurons"])
new_config.set("region_count", old_config["num_regions"])
new_config.set("connection_probability", old_config["connectivity"])
new_config.set("max_episodes", old_config["memory_size"])
new_config.set("stdp_ltp_weight", old_config["plasticity_rate"])
```

### Configuration Backup and Restore

```python
# Create configuration backup
def backup_config(config, backup_path):
    config_dict = {}
    for key in config.getKeys():
        value = config.get(key)
        if value.has_value():
            config_dict[key] = value.value()
    
    with open(backup_path, 'w') as f:
        import json
        json.dump(config_dict, f, indent=2)

# Restore configuration from backup
def restore_config(config, backup_path):
    with open(backup_path, 'r') as f:
        import json
        config_dict = json.load(f)
    
    for key, value in config_dict.items():
        config.set(key, value)
```

## Advanced Configuration Features

### Conditional Configuration

```python
# Configure based on available memory
def adaptive_configuration(available_memory_mb):
    config = createDefaultConfig()
    
    if available_memory_mb < 512:
        # Low memory configuration
        config.set("neuron_count", 50)
        config.set("region_count", 2)
        config.set("connection_probability", 0.05)
    elif available_memory_mb < 1024:
        # Medium memory configuration
        config.set("neuron_count", 200)
        config.set("region_count", 4)
        config.set("connection_probability", 0.1)
    else:
        # High memory configuration
        config.set("neuron_count", 1000)
        config.set("region_count", 8)
        config.set("connection_probability", 0.15)
    
    return config
```

### Environment-based Configuration

```python
# Configure based on environment variables
import os

config = createDefaultConfig()

# Override with environment variables
if 'NLM_NEURON_COUNT' in os.environ:
    config.set("neuron_count", int(os.environ['NLM_NEURON_COUNT']))

if 'NLM_REGION_COUNT' in os.environ:
    config.set("region_count", int(os.environ['NLM_REGION_COUNT']))

if 'NLM_CONNECTION_PROBABILITY' in os.environ:
    config.set("connection_probability", float(os.environ['NLM_CONNECTION_PROBABILITY']))
```

### Configuration Templates Manager

```python
class ConfigurationTemplates:
    TEMPLATES = {
        'minimal': {
            'neuron_count': 10,
            'region_count': 1,
            'connection_probability': 0.05
        },
        'standard': {
            'neuron_count': 200,
            'region_count': 4,
            'connection_probability': 0.12
        },
        'high_performance': {
            'neuron_count': 500,
            'region_count': 6,
            'connection_probability': 0.15,
            'max_episodes': 2000
        },
        'research': {
            'neuron_count': 1000,
            'region_count': 8,
            'connection_probability': 0.2,
            'max_episodes': 5000,
            'stdp_ltp_weight': 0.02,
            'stdp_ltd_weight': 0.024
        }
    }
    
    @classmethod
    def get_template(cls, template_name):
        if template_name not in cls.TEMPLATES:
            raise ValueError(f"Template '{template_name}' not found")
        
        config = createDefaultConfig()
        for key, value in cls.TEMPLATES[template_name].items():
            config.set(key, value)
        
        return config
    
    @classmethod
    def list_templates(cls):
        return list(cls.TEMPLATES.keys())
```

## Configuration Documentation

### Generated Documentation

```python
# Generate configuration documentation
def generate_config_doc(config, output_file):
    doc = f"# NLM Configuration Documentation\n\n"
    doc += "Generated from current configuration\n\n"
    
    doc += "## Current Configuration\n"
    for key in sorted(config.getKeys()):
        if config.has(key):
            value = config.get(key)
            if value.has_value():
                doc += f"- **{key}**: {value.value()}\n"
    
    doc += "\n## Configuration Recommendations\n"
    doc += "- neuron_count: 100-1000 for basic functionality\n"
    doc += "- region_count: 2-8 for distributed processing\n"
    doc += "- connection_probability: 0.05-0.2 for balanced connectivity\n"
    doc += "- max_episodes: 100-5000 for memory capacity\n"
    
    with open(output_file, 'w') as f:
        f.write(doc)
```

## Configuration Validation and Testing

### Automated Configuration Testing

```python
# Automated configuration validation
def validate_configuration(config):
    test_results = []
    
    # Test 1: Required parameters present
    required_params = ['neuron_count', 'region_count', 'connection_probability']
    for param in required_params:
        if not config.has(param):
            test_results.append(('missing_parameter', param, False))
        else:
            test_results.append(('missing_parameter', param, True))
    
    # Test 2: Parameter value ranges
    if config.has('neuron_count'):
        neuron_count = config.getOr("neuron_count", 0)
        if 1 <= neuron_count <= 1000000:
            test_results.append(('neuron_count_range', neuron_count, True))
        else:
            test_results.append(('neuron_count_range', neuron_count, False))
    
    # Test 3: Parameter consistency
    if config.has('region_count') and config.has('neuron_count'):
        region_count = config.getOr("region_count", 1)
        neuron_count = config.getOr("neuron_count", 1)
        if neuron_count >= region_count:
            test_results.append(('neuron_region_consistency', f"{neuron_count}/{region_count}", True))
        else:
            test_results.append(('neuron_region_consistency', f"{neuron_count}/{region_count}", False))
    
    # Summary
    passed = sum(1 for _, _, success in test_results if success)
    total = len(test_results)
    
    return {
        'passed_tests': passed,
        'total_tests': total,
        'success_rate': passed / total if total > 0 else 0,
        'details': test_results
    }
```

## Configuration Optimization

### Performance Optimization

```python
# Optimize configuration for performance
def optimize_configuration(config):
    optimized = config.copy()
    
    # Reduce memory footprint
    if optimized.getOr("neuron_count", 1000) > 500:
        optimized.set("neuron_count", 500)
        optimized.set("region_count", min(optimized.getOr("region_count", 4), 4))
    
    # Optimize memory usage
    if optimized.getOr("max_episodes", 1000) > 2000:
        optimized.set("max_episodes", 2000)
    
    # Optimize connectivity
    if optimized.getOr("connection_probability", 0.1) > 0.15:
        optimized.set("connection_probability", 0.15)
    
    return optimized
```

### Memory Efficiency Optimization

```python
# Optimize for memory-constrained environments
def memory_efficient_config(base_config):
    efficient_config = base_config.copy()
    
    # Calculate optimal neuron count based on memory
    # Formula: neurons * memory_per_neuron <= available_memory
    memory_per_neuron_mb = 0.1  # Approximate memory per neuron in MB
    available_memory_mb = 512  # Available memory
    
    max_neurons = int(available_memory_mb / memory_per_neuron_mb)
    efficient_config.set("neuron_count", min(efficient_config.getOr("neuron_count", 1000), max_neurons))
    
    # Calculate optimal region count
    efficient_config.set("region_count", min(efficient_config.getOr("region_count", 4), max_neurons // 50))
    
    # Reduce memory system requirements
    efficient_config.set("max_episodes", min(efficient_config.getOr("max_episodes", 1000), 500))
    
    return efficient_config
```

## Configuration Examples

### Complete Configuration Examples

```python
# Example 1: Small Robot Brain
robot_brain_config = createDefaultConfig()
robot_brain_config.set("neuron_count", 50)
robot_brain_config.set("region_count", 2)
robot_brain_config.set("connection_probability", 0.08)
robot_brain_config.set("simulation_timestep", 0.002)

# Example 2: Research Platform
research_config = createDefaultConfig()
research_config.set("neuron_count", 2000)
research_config.set("region_count", 6)
research_config.set("connection_probability", 0.15)
research_config.set("max_episodes", 5000)
research_config.set("stdp_ltp_weight", 0.02)
research_config.set("stdp_ltd_weight", 0.024)

# Example 3: Production System
production_config = createDefaultConfig()
production_config.set("neuron_count", 1000)
production_config.set("region_count", 4)
production_config.set("connection_probability", 0.12)
production_config.set("max_episodes", 2000)
production_config.set("random_seed", 12345)
```

## Configuration Troubleshooting

### Common Configuration Issues

| Issue | Symptom | Solution |
|-------|---------|----------|
| Too many neurons | Memory error or slow performance | Reduce neuron_count and region_count |
| Invalid connection probability | Unexpected behavior or crashes | Set connection_probability between 0.0 and 1.0 |
| Memory system disabled | getWorkingMemory() returns None | Check memory system configuration |
| Simulation too slow | Long simulation times | Increase simulation_timestep or reduce neuron_count |
| No memory growth | Max_episodes reached quickly | Increase max_episodes or adjust replay/consolidation intervals |

### Configuration Debugging

```python
# Debug configuration issues
def debug_configuration(config):
    print("=== Configuration Debug Report ===\n")
    
    print("1. Parameter Validation:")
    for key in sorted(config.getKeys()):
        if config.has(key):
            value = config.get(key)
            if value.has_value():
                status = "✓" if is_valid_value(key, value.value()) else "✗"
                print(f"   {status} {key}: {value.value()}")
    
    print("\n2. System Resource Assessment:")
    neuron_count = config.getOr("neuron_count", 1)
    region_count = config.getOr("region_count", 1)
    print(f"   - Neurons: {neuron_count}")
    print(f"   - Regions: {region_count}")
    print(f"   - Memory per neuron (approx): {neuron_count * 0.1:.1f} MB")
    
    print("\n3. Recommended Adjustments:")
    if neuron_count > 500:
        print("   - Consider reducing neuron_count for better performance")
    if region_count > 8:
        print("   - Consider reducing region_count for stability")
    
    suggested_timestep = config.getOr("simulation_timestep", 0.001)
    if suggested_timestep < 0.0005:
        print("   - Consider increasing simulation_timestep for faster simulation")
```

## Configuration Validation Scripts

### Configuration Validation Script

```python
#!/usr/bin/env python3
"""
Configuration validation script for NLM.
Validates configuration parameters and provides recommendations.
"""

import sys
import json
from pathlib import Path

def load_config_from_file(config_file):
    """Load configuration from JSON file."""
    try:
        with open(config_file, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: Configuration file '{config_file}' not found")
        return None
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in configuration file: {e}")
        return None

def validate_config(config_dict):
    """Validate configuration dictionary."""
    errors = []
    warnings = []
    
    # Required parameters
    required_params = ['neuron_count', 'region_count', 'connection_probability']
    for param in required_params:
        if param not in config_dict:
            errors.append(f"Missing required parameter: {param}")
    
    # Validate parameter ranges
    if 'neuron_count' in config_dict:
        neuron_count = config_dict['neuron_count']
        if not isinstance(neuron_count, (int, float)) or neuron_count < 1 or neuron_count > 1000000:
            errors.append(f"Invalid neuron_count: {neuron_count}. Must be 1-1,000,000")
        elif neuron_count > 500:
            warnings.append(f"High neuron_count ({neuron_count}) may impact performance")
    
    if 'region_count' in config_dict:
        region_count = config_dict['region_count']
        if not isinstance(region_count, (int, float)) or region_count < 1 or region_count > 1000:
            errors.append(f"Invalid region_count: {region_count}. Must be 1-1000")
    
    if 'connection_probability' in config_dict:
        prob = config_dict['connection_probability']
        if not isinstance(prob, (int, float)) or prob < 0.0 or prob > 1.0:
            errors.append(f"Invalid connection_probability: {prob}. Must be 0.0-1.0")
    
    # Check consistency
    if 'neuron_count' in config_dict and 'region_count' in config_dict:
        neuron_count = config_dict['neuron_count']
        region_count = config_dict['region_count']
        if neuron_count < region_count:
            warnings.append(f"neuron_count ({neuron_count}) < region_count ({region_count})")
    
    return errors, warnings

def generate_recommendations(config_dict):
    """Generate configuration recommendations."""
    recommendations = []
    
    # Performance recommendations
    if config_dict.get('neuron_count', 1000) > 500:
        recommendations.append("Consider reducing neuron_count to improve performance")
    
    if config_dict.get('connection_probability', 0.1) > 0.15:
        recommendations.append("Consider reducing connection_probability for stability")
    
    # Memory recommendations
    if config_dict.get('max_episodes', 1000) > 2000:
        recommendations.append("Consider reducing max_episodes to save memory")
    
    # Configuration completeness
    essential_params = ['neuron_count', 'region_count', 'connection_probability']
    missing = [p for p in essential_params if p not in config_dict]
    if missing:
        recommendations.append(f"Add missing parameters: {', '.join(missing)}")
    
    return recommendations

def main():
    if len(sys.argv) != 2:
        print("Usage: python config_validator.py <config_file.json>")
        sys.exit(1)
    
    config_file = sys.argv[1]
    
    print("NLM Configuration Validator")
    print("=" * 40)
    
    # Load configuration
    config_dict = load_config_from_file(config_file)
    if not config_dict:
        sys.exit(1)
    
    # Validate configuration
    errors, warnings = validate_config(config_dict)
    
    # Print results
    if errors:
        print("\n❌ VALIDATION ERRORS:")
        for error in errors:
            print(f"  - {error}")
    
    if warnings:
        print("\n⚠️  WARNINGS:")
        for warning in warnings:
            print(f"  - {warning}")
    
    # Generate recommendations
    recommendations = generate_recommendations(config_dict)
    if recommendations:
        print("\n💡 RECOMMENDATIONS:")
        for rec in recommendations:
            print(f"  - {rec}")
    
    # Summary
    total_issues = len(errors) + len(warnings)
    if total_issues == 0:
        print("\n✅ Configuration is valid and complete!")
    else:
        print(f"\n❌ Found {total_issues} issues that need attention")
        sys.exit(1)

if __name__ == "__main__":
    main()
```

## Configuration Best Practices Summary

### 1. Start Simple
- Begin with default configuration
- Gradually increase complexity based on needs

### 2. Profile Before Optimizing
- Measure memory usage and performance
- Make targeted optimizations based on bottlenecks

### 3. Test Configuration Changes
- Use configuration validation scripts
- Test with small neuron counts first
- Gradually scale up based on results

### 4. Document Configuration Choices
- Record configuration parameters used
- Document performance characteristics
- Track configuration evolution over time

### 5. Maintain Configuration Repository
- Keep versions of successful configurations
- Share configurations across team members
- Use configuration templates for common scenarios

## Configuration References

1. NLM Core Documentation: `/workspace/app/README.md`
2. Phase 6 Architecture: `/workspace/app/docs/PHASE6_ARCHITECTURE.md`
3. Configuration Validation Tools: Custom validation scripts
4. Performance Guidelines: `/workspace/app/docs/PERFORMANCE.md`

## Configuration Support

### Getting Help
- **Issue Tracker**: Report configuration problems
- **Documentation**: Refer to this guide for common issues
- **Configuration Validation**: Use validation scripts for automated checking

### Configuration Support Resources
- Configuration validation scripts
- Configuration templates
- Performance optimization guidelines
- Troubleshooting guides

---

*This configuration and troubleshooting guide provides comprehensive information for configuring and troubleshooting the NLM (熙然) neural simulation system. For detailed API documentation, refer to the NLM user guide and reference materials.*

*This document is continuously updated to reflect new configuration options and best practices.*

*Version: 1.0*
*Last Updated: 2026-10-20*

---

**For Questions and Support:**
- Check NLM documentation for detailed configuration examples
- Use configuration validation scripts for automated checking
- Refer to troubleshooting guides for common issues
- Contact NLM support team for specific configuration problems
