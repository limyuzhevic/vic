# Enhanced NLM Documentation
# Updated for Phase 6 Integration Focus

## Overview

This document provides comprehensive guidance for using NLM (Neural Learning Machine) after the Phase 6 integration improvements. NLM now provides a coherent artificial brain architecture with properly integrated memory, prediction, development, and cognition systems.

## Key Improvements in Phase 6

### 1. System Integration
- **Memory Systems**: Fully integrated working, episodic, and semantic memory
- **Prediction System**: Connected to sensory processing and action selection
- **Development System**: Comprehensive developmental stages affecting all brain functions
- **Neuromodulation**: Full integration of dopamine, curiosity, novelty, and prediction error
- **Cognition Systems**: Neural planner, concept formation, attention, and social learning properly connected

### 2. Enhanced Python API
- Advanced configuration options with `AdvancedConfig` class
- Comprehensive brain management with `AdvancedBrain` class
- Factory functions for different use cases
- Performance monitoring and metrics collection
- Experiment management and checkpointing

### 3. Configuration Improvements
- Fixed configuration bugs (duplicate dopamine_baseline)
- Enhanced pyproject.toml with advanced Python binding options
- New configuration files for memory, prediction, and development integration
- Flexible configuration presets for different use cases

## Getting Started with Enhanced NLM

### Quick Start

```python
# Import the enhanced NLM API
import pynlm
from src.agent.AdvancedBrainAPI import AdvancedBrain, AdvancedConfig

# Create an advanced brain with default configuration
config = AdvancedConfig()
brain = AdvancedBrain(config)

# Initialize the brain
brain.initialize()

# Run an enhanced simulation
results = brain.run_advanced_simulation(1000)

# Get comprehensive statistics
stats = brain.get_brain_stats()

# Export results
brain.export_metrics("simulation_results.json")

# Cleanup
brain.cleanup()
```

### Configuration Presets

The enhanced NLM provides several configuration presets for different use cases:

```python
from src.agent.AdvancedBrainAPI import get_preset_config

# Minimal configuration (fast, lightweight)
minimal_config = get_preset_config("minimal")

# Balanced configuration (good balance of features)
balanced_config = get_preset_config("balanced")

# Advanced configuration (all features enabled)
advanced_config = get_preset_config("advanced")

# Developmental configuration (focus on developmental processes)
developmental_config = get_preset_config("developmental")

# Social configuration (multiple interacting agents)
social_config = get_preset_config("social")

# Prediction configuration (focus on prediction and planning)
prediction_config = get_preset_config("prediction")
```

### Advanced Features

#### Memory Management
- **Working Memory**: Real-time sensory processing and temporary storage
- **Episodic Memory**: Experience recording and replay mechanisms
- **Semantic Memory**: Pattern discovery and concept formation
- **Memory Consolidation**: Sleep/rest cycle for memory strengthening

#### Prediction Systems
- **Forward Models**: Predictive sensory processing
- **Inverse Models**: Action effect prediction
- **Temporal Processing**: Multi-step sequence prediction
- **Active Inference**: Belief updating and action selection

#### Development Systems
- **Developmental Stages**: Initial, critical period, maturation, adult
- **Structural Development**: Synaptogenesis and pruning
- **Functional Development**: Cognitive and neural maturation
- **Cross-System Integration**: Development affecting all brain functions

#### Neuromodulation
- **Dopamine**: Reward prediction and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: New experience detection
- **Prediction Error**: Learning from prediction failures

## API Reference

### AdvancedConfig Class

```python
@dataclass
class AdvancedConfig:
    # Core brain parameters
    neuron_count: int = 1000
    region_count: int = 2
    connection_probability: float = 0.1
    simulation_timestep: float = 0.001
    random_seed: int = 42
    
    # Memory system parameters
    enable_working_memory: bool = True
    enable_episodic_memory: bool = True
    enable_semantic_memory: bool = True
    working_memory_capacity: int = 1000
    episodic_memory_limit: int = 100
    
    # Prediction system parameters
    enable_prediction: bool = True
    prediction_horizon: int = 10
    prediction_error_weight: float = 0.5
    confidence_threshold: float = 0.7
    
    # Neuromodulation parameters
    dopamine_baseline: float = 0.1
    curiosity_threshold: float = 0.3
    novelty_decay: float = 0.99
    
    # Development parameters
    enable_development: bool = True
    critical_period_duration: float = 60.0
    maturation_age: float = 300.0
    structural_plasticity_enabled: bool = True
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # And many more...
```

### AdvancedBrain Class

```python
class AdvancedBrain:
    def __init__(self, config: Optional[AdvancedConfig] = None):
        # Initialize with advanced configuration
        
    def initialize(self) -> bool:
        # Initialize the brain with all integrated systems
        
    def run_advanced_simulation(self, num_steps: int = 1000) -> Dict[str, Any]:
        # Run enhanced simulation with comprehensive monitoring
        
    def get_brain_stats(self) -> Dict[str, Any]:
        # Get comprehensive brain statistics
        
    def enable_feature(self, feature_name: str, parameters: Optional[Dict[str, Any]] = None):
        # Enable an advanced feature dynamically
        
    def get_feature_status(self) -> Dict[str, bool]:
        # Get status of all advanced features
        
    # Additional methods for checkpointing, experiment running, etc.
```

## Integration Architecture

The enhanced NLM follows a comprehensive integration architecture:

```
WORLD
  ↓
SENSORY INPUT (Vision, Touch, Internal, Proprioception)
  ↓
MEMORY SYSTEMS
  ├── Working Memory (real-time processing)
  ├── Episodic Memory (experience storage)
  ├── Semantic Memory (pattern discovery)
  │
PREDICTION SYSTEMS
  ├── Forward Models (sensory prediction)
  ├── Inverse Models (action prediction)
  ├── Temporal Processing (sequence learning)
  │
COGNITION SYSTEMS
  ├── Neural Planner (action planning)
  ├── Concept Formation (pattern discovery)
  ├── Attention (competitive selection)
  │
NEUROMODULATION
  ├── Dopamine (reward processing)
  ├── Curiosity (exploration motivation)
  ├── Novelty (new experience detection)
  ├── Prediction Error (learning signals)
  │
DEVELOPMENT SYSTEMS
  ├── Structural Development (synaptogenesis/pruning)
  ├── Functional Development (cognitive maturation)
  ├── Neuromodulation Development
  │
BRAIN CORE
  ├── LIF Neurons (spiking dynamics)
  ├── Synapses (connectivity)
  ├── Spike Processing (event-driven)
  └── Plasticity (STDP, Hebbian)
```

## Configuration Files

The enhanced NLM includes several configuration files:

### Default Configuration
- `configs/default.cfg`: Basic NLM configuration
- `configs/prediction_integration.cfg`: Prediction system integration settings
- `configs/development_integration.cfg`: Development system integration settings

### Advanced Configuration
The `AdvancedConfig` class provides comprehensive configuration options through a Python interface rather than file-based configuration.

## Experiment Management

The enhanced NLM provides comprehensive experiment management:

```python
# Run an experiment with specific configuration
experiment_config = {
    "num_steps": 2000,
    "enable_prediction": True,
    "enable_curiosity": True,
    "enable_development": True,
    "experiment_tracking": True,
    "auto_save_metrics": True,
}

results = brain.run_experiment(experiment_config)
```

## Performance Monitoring

The enhanced NLM includes comprehensive performance monitoring:

```python
# Performance metrics
stats = brain.get_brain_stats()
print(f"Neurons: {stats['basic_info']['neuron_count']}")
print(f"Firing Rate: {stats['basic_info']['average_firing_rate']:.3f} Hz")
print(f"Prediction Accuracy: {stats['advanced_stats'].get('prediction_accuracy', 0):.3f}")
print(f"Memory Usage: {stats['advanced_stats'].get('memory_usage', 0):.1f} MB")
```

## Troubleshooting

### Common Issues and Solutions

**Issue**: "Brain not initialized"
**Solution**: Always call `brain.initialize()` before running simulations.

**Issue**: "Memory not available"
**Solution**: Check feature status with `brain.get_feature_status()` and enable required features.

**Issue**: "Performance issues"
**Solution**: Use performance profiling with `brain.config.enable_performance_profiling = True`.

**Issue**: "Checkpoint failed"
**Solution**: Ensure write permissions and check disk space.

### Debugging Tools

```python
# Enable debugging
brain.config.log_level = "DEBUG"
brain.config.enable_performance_profiling = True
brain.config.enable_memory_monitoring = True

# Run simulation with debugging
results = brain.run_advanced_simulation(100)

# Check for errors
if "errors" in results:
    print("Errors found:", results["errors"])
```

## Best Practices

### 1. Configuration Management
```python
# Use configuration presets for different use cases
if use_case == "research":
    config = get_preset_config("advanced")
elif use_case == "production":
    config = get_preset_config("balanced")
elif use_case == "testing":
    config = get_preset_config("minimal")
```

### 2. Resource Management
```python
# Always clean up resources
brain = AdvancedBrain(config)
try:
    brain.initialize()
    results = brain.run_advanced_simulation(1000)
    brain.export_metrics("results.json")
finally:
    brain.cleanup()
```

### 3. Error Handling
```python
brain = AdvancedBrain(config)
try:
    brain.initialize()
    results = brain.run_advanced_simulation(1000)
except Exception as e:
    print(f"Simulation failed: {e}")
    # Attempt recovery
    brain = AdvancedBrain(config)
    brain.initialize()
finally:
    brain.cleanup()
```

### 4. Performance Optimization
```python
# Configure for performance
config = AdvancedConfig()
config.enable_memory_pools = True
config.enable_event_queues = True
config.enable_simd_optimization = True
config.enable_parallel_processing = False  # Set to True if multi-core available

brain = AdvancedBrain(config)
brain.initialize()
```

## Future Enhancements

The enhanced NLM provides a foundation for future improvements:

### 1. Multi-Agent Systems
```python
from src.agent.AdvancedBrainAPI import create_social_brain

# Create multiple interacting agents
brains = create_social_brain(num_agents=5)
```

### 2. Specialized Brains
```python
from src.agent.AdvancedBrainAPI import (
    create_experimental_brain,
    create_developmental_brain,
    create_prediction_brain
)

# Create specialized brains for different research areas
experimental_brain = create_experimental_brain()
developmental_brain = create_developmental_brain()
prediction_brain = create_prediction_brain()
```

### 3. Advanced Benchmarking
```python
from src.agent.AdvancedBrainAPI import benchmark_brain, compare_configs

# Benchmark different configurations
configs = [get_preset_config("minimal"), get_preset_config("balanced"), get_preset_config("advanced")]
results = compare_configs(configs, num_steps=1000)
```

## Conclusion

The enhanced NLM provides a comprehensive artificial brain architecture with properly integrated systems for memory, prediction, development, and cognition. The advanced Python API makes it easy to configure, run, and monitor complex neural simulations while maintaining backward compatibility with existing code.

The Phase 6 integration successfully addresses the key findings from the Phase 6 Final Audit by:

1. **Connecting memory systems** to the main brain loop
2. **Integrating prediction systems** with sensory processing and action selection
3. **Enhancing development systems** with comprehensive cross-system integration
4. **Providing advanced tools** for experimentation and analysis
5. **Maintaining backward compatibility** with existing NLM code

This foundation enables researchers and developers to create sophisticated artificial brain simulations with realistic learning, development, and cognitive capabilities.
