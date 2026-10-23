# NLM Quick Start Guide: Enhanced Phase 6 Integration

## Welcome to NLM Phase 6

NLM (熙然) is now a fully integrated artificial brain system with enhanced configuration, command-line interface, and Python API. This guide will help you get started quickly with the improved features.

## Quick Installation

```bash
# Standard build (CMake 3.16+)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

```bash
# Python bindings (optional)
cd python
pip install -e .
```

## 1. Configuration Options

### Enhanced Configuration File Format

NLM now supports advanced configuration parsing with multiple value types:

```ini
# configs/enhanced.cfg
# Basic settings
random_seed = 42
simulation_timestep = 0.001s
neuron_count = 2000
region_count = 2

# Connection settings
connection_probability = 0.15

# STDP plasticity
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity  
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Experimental features (advanced)
experimental_features_enabled = true
metrics_enabled = true
checkpoint_enabled = true
checkpoint_interval = 1000

# Integration mode
integration_mode = auto
performance_mode = balanced

# Neural processing settings
plasticity_enabled = true
neuromodulation_enabled = true
memory_systems_enabled = true
prediction_enabled = true
cognition_enabled = true
```

### Configuration Value Types Supported

- **Integer**: `value`, `0xFF` (hex), `0123` (octal), `-100` (negative)
- **Float**: `3.14159`, `1.23e-4` (scientific notation)
- **Boolean**: `true`, `false`, `yes`, `no`, `on`, `off`, `enable`, `disable`
- **String**: `"string value"` (quoted)
- **Vectors**: `[1,2,3,4,5]` (JSON-like arrays)

## 2. Command-Line Interface

### Basic Usage

```bash
# Run with default configuration
./nlm

# Run with custom configuration file
./nlm --config my_config.cfg

# Override specific settings
./nlm \
  --neuron_count 5000 \
  --region_count 3 \
  --connection_probability 0.2 \
  --random_seed 12345
```

### Advanced Options

```bash
# Enable experimental features
./nlm \
  --experimental_features \
  --enable_plasticity \
  --enable_neuromodulation \
  --enable_memory \
  --enable_prediction \
  --enable_cognition

# Performance metrics and checkpointing
./nlm \
  --enable_metrics \
  --enable_checkpoint \
  --checkpoint_interval 500

# Integration mode and performance
./nlm \
  --integration_mode parallel \
  --performance_mode speed \
  --thread_count 4

# Debug logging
./nlm --enable_debug_logging

# Help
./nlm --help
```

### Configuration Override Examples

```bash
# Override single values
./nlm \
  --neuron_count 1000 \
  --stdp_ltp_weight 0.025

# Boolean flags
./nlm \
  --enable_plasticity \
  --disable_neuromodulation

# Performance settings
./nlm \
  --memory_limit 8.0 \
  --thread_count 8
```

## 3. Python API Examples

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()

# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)

print("NLM Phase 6 brain initialized successfully!")
```

### Advanced Python API Features

```python
import pynlm

# Create brain with experimental features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize with world
world = pynlm.createSimpleWorld()
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Configure advanced features
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Set learning parameters
agent_brain.setLearningRate(0.01)
agent_brain.setExplorationFactor(0.8)
agent_brain.setPredictionHorizon(10)

# Configure memory
agent_brain.setMemoryCapacity(10000)

print("Advanced agent configured successfully")

# Get system statistics
stats = agent_brain.getTrainingStats()
print(f"Training statistics: {stats}")

performance = agent_brain.getPerformanceMetrics()
print(f"Performance metrics: {performance}")
```

### Python Integration Examples

```python
# Example: Simulate an episode
import pynlm

agent_brain = pynlm.createAgentBrain(brain)

# Run simulation steps for 1000 steps
for step in range(1000):
    # Get current percept
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Step brain
    brain.step(step, step * 0.001)
    
    # Simulate world update
    world.update(0.001)
    
    # Check for curiosity-driven exploration
    if agent_brain.getCuriosityLevel() > 0.5:
        print(f"Step {step}: Curiosity-driven exploration")

print("Episode completed successfully!")
```

## 4. Running Experiments

### Basic Experiment

```bash
# Run built-in experiments
./nlm --experiment basic --max_steps 1000

# Run connectivity experiment
./nlm --experiment connectivity --max_steps 5000

# Run plasticity experiment  
./nlm --experiment plasticity --max_steps 2000 --enable_metrics

# Run all experiments
./nlm --experiment_all --max_steps 10000
```

### Advanced Experiment Configuration

```bash
# Configure experiment with custom settings
./nlm \
  --experiment plasticity \
  --max_steps 5000 \
  --enable_checkpoint \
  --checkpoint_interval 200 \
  --enable_debug_logging

# Run with performance mode
./nlm \
  --experiment stdp_verification \
  --performance_mode accuracy \
  --memory_limit 16.0
```

## 5. Checkpointing and Persistence

### Save/Load Brain State

```bash
# Save brain state
./nlm --enable_checkpoint --checkpoint_interval 500 --save_checkpoint my_brain_checkpoint

# Load brain state  
./nlm --load_checkpoint my_brain_checkpoint
```

### Python Checkpointing

```python
import pynlm

brain = pynlm.createBrain(config)

# Save brain state
brain.save("my_brain_state.nlm")

# Load brain state (later in the same session)
loaded_brain = pynlm.loadBrain("my_brain_state.nlm")

print("Brain state saved and loaded successfully!")
```

## 6. Performance Optimization

### Performance Modes

The NLM supports different performance optimization modes:

- **speed**: Maximum processing speed
- **accuracy**: Higher numerical precision
- **balanced**: Trade-off between speed and accuracy

```bash
# Optimize for speed
./nlm --performance_mode speed

# Optimize for accuracy  
./nlm --performance_mode accuracy

# Balanced performance (default)
./nlm --performance_mode balanced
```

### Memory Management

```bash
# Configure memory limits
./nlm --memory_limit 8.0  # 8 GB limit
./nlm --memory_limit 16.0 # 16 GB limit

# Optimize thread count
./nlm --thread_count 4  # Use 4 CPU threads
./nlm --thread_count 8  # Use 8 CPU threads (for multi-core systems)
```

## 7. Getting Started Examples

### Example 1: Simple Learning Environment

```bash
# Create a simple learning environment
cat > simple_env.cfg << EOF
# Simple learning environment configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
plasticity_enabled = true
neuromodulation_enabled = true
experimental_features_enabled = true
EOF

# Run the environment
./nlm --config simple_env.cfg
```

### Example 2: Complex Integrated Experiment

```bash
# Create complex experiment configuration
cat > complex_experiment.cfg << EOF
# Complex integrated experiment configuration
neuron_count = 5000
region_count = 4
connection_probability = 0.2
simulation_timestep = 0.001
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.012

# Enable all experimental features
experimental_features_enabled = true
metrics_enabled = true
checkpoint_enabled = true
checkpoint_interval = 1000
integration_mode = parallel
performance_mode = balanced

# Enable all subsystems
plasticity_enabled = true
neuromodulation_enabled = true
memory_systems_enabled = true
prediction_enabled = true
cognition_enabled = true
EOF

# Run complex experiment
./nlm --config complex_experiment.cfg \
  --max_steps 10000 \
  --enable_metrics \
  --enable_checkpoint
```

### Example 3: Python-Based Experiment

```python
#!/usr/bin/env python3
import pynlm
import time

def run_experiment(config_file="default.cfg"):
    """Run a complete NLM experiment"""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, vision_width=5, vision_height=5)
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Enable advanced features
    agent_brain.enableStructuralPlasticity(True)
    agent_brain.enableDevelopment(True)
    
    print("Starting experiment...")
    start_time = time.time()
    
    # Run simulation
    for step in range(config.getOr("max_steps", 1000)):
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process input
        agent_brain.processSensoryInput(percept)
        
        # Step brain
        brain.step(step, step * 0.001)
        
        # Update world
        world.update(0.001)
        
        # Apply neuromodulation based on performance
        if step % 100 == 0:
            reward = world.getReward()
            agent_brain.applyRewardModulation(reward, agent_brain.getPredictionError())
    
    # Save experiment results
    end_time = time.time()
    print(f"Experiment completed in {end_time - start_time:.2f} seconds")
    
    # Save brain state
    brain.save("experiment_results.brain")
    
    return brain

if __name__ == "__main__":
    # Run the experiment
    final_brain = run_experiment()
    print("Experiment completed successfully!")
```

## 8. Advanced Features Reference

### AgentBrain Advanced Methods

```python
# Advanced agent brain methods (expert users)

# Learning control
agent_brain.setLearningRate(0.01)
agent_brain.getLearningRate()

# Exploration behavior  
agent_brain.setExplorationFactor(0.8)
agent_brain.getExplorationFactor()

# Memory management
agent_brain.setMemoryCapacity(10000)
agent_brain.getMemoryStats()

# Prediction system
agent_brain.setPredictionHorizon(10)
agent_brain.getPredictionStats()

# Knowledge management
agent_brain.exportKnowledge("json")
agent_brain.importKnowledge(knowledge_data)

# Goal management
agent_brain.createSubGoal("reach_target")
agent_brain.getSubGoals()

# Learning control
agent_brain.pauseLearning()
agent_brain.resumeLearning()

# State management
agent_brain.saveAgentState("agent_state.json")
agent_brain.loadAgentState("agent_state.json")

# Performance benchmarking
agent_brain.benchmark(100, 1000, "benchmark_results.json")
```

### Configuration Features

- **Enhanced parsing**: Support for JSON-like syntax in config files
- **Type detection**: Automatic type inference for values
- **Inline comments**: Support for `#` comments in config files
- **Multiple formats**: Support for `=`, `:` delimiters
- **Advanced types**: Boolean, integer, float, string, hex, octal, scientific notation

### Command-Line Features

- **Full configuration override**: Override any config value via command line
- **Advanced options**: Experimental features, performance modes, integration modes
- **Help system**: Comprehensive help with examples
- **Boolean flags**: Enable/disable individual features
- **Value types**: Support for different value types in command line

### Python API Features

- **Full integration**: Complete Python bindings for all NLM features
- **Advanced methods**: Expert-level methods for power users
- **Type hints**: Full type support throughout Python API
- **Memory management**: Efficient memory handling for large-scale simulations
- **Performance monitoring**: Built-in performance metrics and benchmarking

## 9. Troubleshooting

### Common Issues

**1. Configuration File Errors**
```bash
# Fix malformed config file
./nlm --config fixed_config.cfg
```

**2. Missing Dependencies**
```bash
# Install dependencies
pip install -r requirements.txt
```

**3. Build Issues**
```bash
# Clean and rebuild
cd build && make clean && cmake .. && make -j4
```

**4. Memory Issues**
```bash
# Increase memory limit
./nlm --memory_limit 16.0
```

### Getting Help

- **Documentation**: Check `docs/` directory for comprehensive guides
- **Examples**: Look in `examples/` directory for practical examples
- **Community**: GitHub repository for issues and discussions
- **Support**: Project documentation for troubleshooting

## 10. Performance Tips

### Optimization Recommendations

1. **Use parallel integration mode** for multi-core systems:
   ```bash
   ./nlm --integration_mode parallel
   ```

2. **Configure memory limits** for large simulations:
   ```bash
   ./nlm --memory_limit 8.0 --thread_count 4
   ```

3. **Enable performance metrics** for optimization:
   ```bash
   ./nlm --enable_metrics --enable_checkpoint
   ```

4. **Use performance modes** based on your needs:
   ```bash
   # Speed (default)
   ./nlm --performance_mode speed
   
   # Accuracy (for precise experiments)
   ./nlm --performance_mode accuracy
   
   # Balanced (recommended for most use cases)
   ./nlm --performance_mode balanced
   ```

5. **Configure checkpointing** for long experiments:
   ```bash
   ./nlm --enable_checkpoint --checkpoint_interval 1000
   ```

## 11. Example Scripts

### Script 1: Automated Experiment Runner

```python
#!/usr/bin/env python3
"""
Automated experiment runner for NLM Phase 6
"""
import pynlm
import json
import time

def run_parameter_search():
    """Run parameter search experiment"""
    
    configurations = [
        {"neurons": 1000, "plasticity": True, "neuromodulation": True},
        {"neurons": 2000, "plasticity": False, "neuromodulation": True},
        {"neurons": 1500, "plasticity": True, "neuromodulation": False},
    ]
    
    results = []
    
    for config in configurations:
        print(f"Running experiment with {config['neurons']} neurons...")
        
        # Create configuration
        nlm_config = pynlm.createDefaultConfig()
        nlm_config.set("neuron_count", config["neurons"])
        nlm_config.set("plasticity_enabled", config["plasticity"])
        nlm_config.set("neuromodulation_enabled", config["neuromodulation"])
        
        # Run experiment
        brain = pynlm.createBrain(nlm_config)
        brain.initialize()
        
        start_time = time.time()
        brain.step(100, 0.1)  # Run 100 steps
        end_time = time.time()
        
        result = {
            "configuration": config,
            "steps_completed": 100,
            "execution_time": end_time - start_time,
            "neuron_count": config["neurons"]
        }
        
        results.append(result)
        print(f"Completed: {result}")
    
    # Save results
    with open("parameter_search_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print("Parameter search completed and saved to results.json")
    return results

if __name__ == "__main__":
    run_parameter_search()
```

### Script 2: Real-Time Monitoring

```python
#!/usr/bin/env python3
"""
Real-time monitoring for NLM experiments
"""
import pynlm
import time
import threading
import sys

def monitor_brain(brain, interval=5.0):
    """Monitor brain activity in real-time"""
    
    def monitor_loop():
        while True:
            try:
                # Get brain statistics
                stats = brain.logStatus()
                print(f"[MONITOR] Step: {brain.getTotalSpikeCount()}, "
                      f"Neurons: {brain.getTotalNeuronCount()}, "
                      f"Synaptoses: {brain.getTotalSynapseCount()}")
                
                time.sleep(interval)
                
            except Exception as e:
                print(f"[MONITOR] Error: {e}")
                break
    
    # Start monitoring thread
    monitor_thread = threading.Thread(target=monitor_loop, daemon=True)
    monitor_thread.start()
    
    return monitor_thread

def run_monitored_experiment():
    """Run experiment with real-time monitoring"""
    
    # Create brain with monitoring enabled
    config = pynlm.createDefaultConfig()
    config.set("metrics_enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Starting monitored experiment...")
    print("Press Ctrl+C to stop monitoring")
    
    # Start monitoring
    monitor_thread = monitor_brain(brain, interval=2.0)
    
    try:
        # Run experiment
        for step in range(100):
            brain.step(step, step * 0.001)
            time.sleep(0.1)  # Simulate real-time
            
            if step % 20 == 0:
                print(f"Progress: {step}/100 steps")
        
        print("Experiment completed!")
        
    except KeyboardInterrupt:
        print("\nExperiment interrupted by user")
    
    return brain

if __name__ == "__main__":
    run_monitored_experiment()
```

## Quick Reference

### Key Commands

**Build:**
```bash
mkdir build && cd build && cmake .. && make -j4
```

**Basic Run:**
```bash
./nlm
```

**With Configuration:**
```bash
./nlm --config my_config.cfg
```

**Advanced Run:**
```bash
./nlm --experimental_features --enable_metrics --enable_checkpoint
```

**Python Usage:**
```python
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### File Locations

- **Build artifacts:** `build/`
- **Source code:** `src/`
- **Python bindings:** `python/`
- **Tests:** `tests/`
- **Configuration:** `configs/`
- **Documentation:** `docs/`
- **Examples:** `examples/`

### Getting Help

- **Documentation:** Check `README.md` and `docs/`
- **API Reference:** `python/bindings.cpp` for Python API
- **Examples:** Look in `examples/` directory
- **Community:** GitHub repository for support

## Conclusion

NLM Phase 6 now provides a comprehensive, feature-rich artificial brain system with:

- ✅ **Enhanced configuration system** with advanced parsing
- ✅ **Power user command-line interface** with extensive options
- ✅ **Advanced Python API** for expert users
- ✅ **Comprehensive test coverage** with improved test organization
- ✅ **Real-time monitoring** and performance metrics
- ✅ **Checkpointing and persistence** for long experiments
- ✅ **Performance optimization** with multiple modes
- ✅ **Practical examples** and documentation

The system is now ready for both beginners and expert users to run experiments, integrate with other systems, and explore advanced neuroscience simulations.

---

*Last updated: Current Phase 6 Integration*
*For issues or questions: GitHub repository*
*Documentation: `docs/` directory*
*API Reference: `python/bindings.cpp`*

This guide will help you make the most of NLM Phase 6's enhanced features and capabilities. Enjoy your exploration of artificial developmental brain systems!