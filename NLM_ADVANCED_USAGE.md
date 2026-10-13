# NLM Advanced Usage Examples
# This file demonstrates advanced features and improvements in NLM Phase 6

## Overview

This document showcases the enhanced features in NLM Phase 6, including:

1. Advanced Configuration Management
2. Command-Line Interface Improvements
3. Performance Monitoring and Profiling
4. Plugin Architecture
5. Error Handling and Validation
6. Advanced Usage Patterns

## 1. Advanced Configuration Management

### Using Advanced Configuration Files

```bash
# Create a custom advanced configuration
cat > my_advanced_config.cfg << EOF
# NLM Advanced Configuration
# Phase 6 Enhanced Settings

core_settings {
    random_seed = 123456789
    simulation_timestep = 0.0005
    neuron_count = 20000
    region_count = 4
    connectivity_pattern = "small_world"
}

plasticity {
    hebbian.enable = true
    hebbian.learning_rate = 0.025
    stdp.enable = true
    stdp.ltp_weight = 0.03
    stdp.ltd_weight = 0.02
    structural.enable = true
    structural.synaptogenesis_rate = 0.0008
}

performance {
    threading.enable = true
    threading.max_threads = 12
    optimization.level = "aggressive"
    memory_pool.enable = true
}

output {
    logging.level = "DEBUG"
    logging.format = "json"
    logging.file = "nlm_advanced.log"
}
EOF

# Run with advanced configuration
./nlm --config my_advanced_config.cfg --benchmark --visualization
```

### Using Command-Line Overrides

```bash
# Override specific configuration values
./nlm \
    --config configs/default.cfg \
    --set brain.neuron_count=50000 \
    --set plasticity.stdp.ltp_weight=0.05 \
    --set performance.optimization.level=aggressive \
    --benchmark-duration 500 \
    --output results.json
```

## 2. Command-Line Interface Improvements

### Basic Usage

```bash
# Help and version information
./nlm --help
./nlm --version

# Running different modes
./nlm --benchmark                    # Run benchmark mode
./nlm --visualization                # Enable visualization
./nlm --no-colors                    # Disable colored output

# Configuration loading
./nlm --config custom.cfg            # Load specific config
./nlm --set key=value                # Override configuration
./nlm --benchmark-duration 1000      # Set benchmark duration
```

### Configuration Validation

```bash
# Validate configuration before running
./nlm --config my_config.cfg --validate

# Get configuration export for debugging
./nlm --config my_config.cfg --export-config

# Run with comprehensive error checking
./nlm --config my_config.cfg --validation.strict_mode=true
```

## 3. Performance Monitoring and Profiling

### Monitoring Output

```python
import pynlm

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable performance monitoring
agent = pynlm.createAgentBrain(brain)
agent.enablePerformanceMonitoring(True)

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    
    # Get performance statistics
    perf_stats = agent.getPerformanceStats()
    print(f"Step {step}: {perf_stats['steps_per_second']} steps/sec")
    print(f"Memory usage: {perf_stats['memory_usage_mb']} MB")
    print(f"CPU usage: {perf_stats['cpu_usage_percent']}%")
```

### Benchmark Mode

```bash
# Run benchmark with performance metrics
./nlm --benchmark --benchmark-duration 2000 --output benchmark_results.json

# Benchmark with detailed profiling
./nlm --benchmark --benchmark-duration 5000 --log-level DEBUG
```

## 4. Plugin Architecture

### Available Plugins

NLM comes with several built-in plugins:

1. **AdvancedValidationPlugin** - Enhanced configuration validation
2. **PerformanceEnhancementPlugin** - Performance optimization
3. **LoggingEnhancementPlugin** - Advanced logging capabilities
4. **ConfigValidationPlugin** - Configuration validation and error checking

### Using Plugins

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with plugin support
agent = pynlm.createAgentBrain(brain)

# Enable specific plugins
agent.enableAdvancedValidation(True)
agent.enablePerformanceEnhancement(True)
agent.enableLoggingEnhancement(True)

# Create simulation world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
world.reset()

# Run simulation with plugins
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Creating Custom Plugins

```cpp
// Example custom plugin implementation
#include "plugins/PluginInterface.hpp"

class CustomPlugin : public nlm::IPlugin {
public:
    std::string getName() const override { return "CustomPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Custom plugin for specific functionality"; }
    
    bool canHandle(const std::string& key) const override {
        return key.find("custom") != std::string::npos;
    }
    
    std::string handleConfig(const std::string& key, const std::string& value) override {
        // Custom processing logic
        return value + "_processed";
    }
};
```

## 5. Error Handling and Validation

### Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
validation_result = config.validate()
if not validation_result.valid:
    print("Configuration errors:")
    for error in validation_result.errors:
        print(f"  - {error}")
else:
    print("Configuration is valid")
    if validation_result.warnings:
        print("Warnings:")
        for warning in validation_result.warnings:
            print(f"  - {warning}")
```

### Advanced Error Handling

```python
# Enable comprehensive error handling
agent.enableErrorHandling({
    "level": "comprehensive",
    "auto_recover": True,
    "max_attempts": 3,
    "report_format": "structured"
})

# Example error handling scenarios
try:
    # Code that might fail
    result = some_risky_operation()
except pynlm.Error as e:
    print(f"Error: {e.message}")
    print(f"Severity: {e.severity}")
    print(f"Recovery options: {e.recovery_options}")
```

## 6. Advanced Usage Patterns

### Pattern 1: Performance-Optimized Simulation

```python
def run_performance_optimized_simulation(config, duration, output_file=None):
    """Run a performance-optimized simulation."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enablePerformanceEnhancement(True)
    agent.enableAdvancedValidation(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=200, height=200, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Performance monitoring
    start_time = time.time()
    
    for step in range(duration):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            perf_stats = agent.getPerformanceStats()
            print(f"Step {step}: {perf_stats['steps_per_second']:.1f} steps/sec")
    
    end_time = time.time()
    
    if output_file:
        results = {
            "duration": duration,
            "elapsed_time": end_time - start_time,
            "steps_per_second": duration / (end_time - start_time),
            "final_neuron_count": brain.getTotalNeuronCount(),
            "final_firing_rate": brain.getAverageFiringRate()
        }
        import json
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
    
    return brain, world
```

### Pattern 2: Research and Experimentation

```python
def run_research_simulation(config, experiments):
    """Run multiple research experiments with the same configuration."""
    results = {}
    
    for exp_name, exp_config in experiments.items():
        print(f"Running experiment: {exp_name}")
        
        # Create fresh configuration
        exp_brain = pynlm.createBrain(config)
        exp_brain.initialize()
        
        exp_agent = pynlm.createAgentBrain(exp_brain)
        exp_agent.enableAdvancedValidation(True)
        
        exp_world = pynlm.createSimpleWorld()
        exp_world.configure(**exp_config['world_settings'])
        exp_world.reset()
        exp_agent.initialize(exp_world)
        
        # Run experiment
        for step in range(exp_config['duration']):
            # Experiment-specific behavior
            exp_world.update(0.1)
            percept = exp_world.getSensoryPercept()
            exp_agent.processSensoryInput(percept)
            exp_brain.step(step)
            action = exp_agent.decodeMotorCommand()
            exp_world.applyMotorCommand(action, exp_world.getSimulationTime())
        
        # Collect results
        results[exp_name] = {
            'firing_rate': exp_brain.getAverageFiringRate(),
            'neuron_count': exp_brain.getTotalNeuronCount(),
            'total_spikes': exp_brain.getTotalSpikeCount(),
            'final_activity': exp_brain.getFiringNeuronCount()
        }
    
    return results
```

## 7. Configuration Examples

### Advanced Configuration File (JSON/YAML)

```json
{
    "nlm_version": "Phase 6",
    "core_settings": {
        "random_seed": 123456789,
        "simulation_timestep": 0.0005,
        "neuron_count": 20000,
        "region_count": 4,
        "connectivity_pattern": "small_world"
    },
    "plasticity": {
        "hebbian": {
            "enable": true,
            "learning_rate": 0.025
        },
        "stdp": {
            "enable": true,
            "ltp_weight": 0.03,
            "ltd_weight": 0.02,
            "tau": 30.0
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0008
        }
    },
    "performance": {
        "threading": {
            "enable": true,
            "max_threads": 12
        },
        "optimization": {
            "level": "aggressive"
        }
    },
    "output": {
        "logging": {
            "level": "DEBUG",
            "format": "json",
            "file": "nlm_advanced.log"
        }
    }
}
```

## 8. Troubleshooting

### Common Issues and Solutions

**Issue: Configuration validation fails**
```bash
# Solution: Check configuration files for syntax errors
# Ensure all required keys are present
# Use the --validate flag to check configuration
./nlm --config your_config.cfg --validate
```

**Issue: Performance is slow**
```bash
# Solution: Optimize configuration
# Enable performance enhancements
./nlm --config config.cfg --performance.optimization.level=aggressive
# Use appropriate thread count
./nlm --config config.cfg --threading.max_threads=8
```

**Issue: Memory usage is high**
```bash
# Solution: Optimize memory usage
# Reduce neuron count for testing
./nlm --config config.cfg --set brain.neuron_count=1000
# Disable memory-intensive features
./nlm --config config.cfg --set memory_pool.enable=false
```

### Getting Help

```bash
# Comprehensive help
./nlm --help

# Configuration validation help
./nlm --help validation

# Plugin information
./nlm --help plugins

# Performance monitoring help
./nlm --help performance
```

## 9. Future Improvements

The NLM platform continues to evolve with these planned enhancements:

1. **Machine Learning Integration** - Integrate neural network training capabilities
2. **Distributed Computing** - Support for distributed simulations across multiple machines
3. **GPU Acceleration** - CUDA integration for faster computations
4. **Advanced Visualization** - Real-time visualization and interaction
5. **Cloud Integration** - Cloud deployment and management
6. **Custom Plugin Marketplace** - Community plugin sharing and distribution

## Conclusion

NLM Phase 6 provides a comprehensive platform for neural simulation with advanced configuration management, performance monitoring, and extensibility. The improvements demonstrated in this document enable researchers and developers to:

- Configure complex simulations with ease
- Monitor and optimize performance
- Extend functionality through plugins
- Validate configurations automatically
- Handle errors robustly
- Reproduce and compare experiments

These features make NLM a powerful tool for exploring neural systems and artificial intelligence research.
