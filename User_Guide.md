# NLM - Neural Learning Machine: User Guide

## Quick Start

This guide provides step-by-step instructions for getting started with the enhanced NLM Phase 6 system. It covers installation, basic usage, and advanced features.

## Section 1: Installation and Setup

### 1.1 System Requirements

**Hardware Requirements:**
- Modern CPU (64-bit architecture)
- At least 8GB RAM (16GB recommended for complex simulations)
- Storage: 100MB for the core system, additional space for simulations and checkpoints

**Software Requirements:**
- C++20 compatible compiler (GCC 11+, Clang 14+, or MSVC 2022+)
- CMake 3.20 or higher
- Python 3.8+ (for Python bindings)
- pip or equivalent package manager

### 1.2 Building the System

**Option A: Build from Source (Recommended)**

```bash
# Clone the repository
cd /path/to/nlm
git clone https://github.com/NeuralLearningMachine/NLM.git

# Create build directory
mkdir build && cd build

# Configure with enhanced CLI features
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_CLI=ON

# Build the system
make -j$(nproc)

# Install (optional)
cmake --install .
```

**Option B: Python Installation (Easier)**

```bash
# Install Python dependencies
pip install pynlm
pip install scikit-build-core pybind11 numpy memory-profiler

# Install NLM Python package
pip install -e .

# Or install with enhanced features
pip install -e . --config-settings=--enable-advanced-cli
```

### 1.3 Verification

**Verify the installation:**

```bash
# Test the executable (if built from source)
./nlm --mode phase6 --help

# Test the Python bindings
python3 -c "import pynlm; print(f'NLM Version: {pynlm.__version__}')"
python3 -c "import pynlm; config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config); print(f'Neurons: {brain.getTotalNeuronCount()}')"
```

## Section 2: Basic Usage

### 2.1 Understanding NLM Components

The NLM system consists of three main components:

1. **Brain**: The neural network with spiking neurons and synaptic connections
2. **World**: The virtual environment where the brain interacts
3. **Agent**: The bridge that connects the brain to the world

### 2.2 Creating a Simple Brain

**Python Example - Basic Configuration:**

```python
import pynlm

# Create a basic configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain development stage: {brain.getDevelopmentalStage()}")
```

**Python Example - Enhanced Configuration:**

```python
import pynlm

# Create enhanced configuration with Phase 6 features
config = pynlm.createDefaultConfig()
config.set("development.enable", True)
config.set("neuromodulation.dopamine.enable", True)
config.set("neuromodulation.curiosity.enable", True)
config.set("memory.working_memory.capacity", 1000)
config.set("memory.episodic_memory.capacity", 5000)
config.set("prediction.enable", True)

# Create and initialize the enhanced brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Enhanced brain created with Phase 6 capabilities")
print(f"Development enabled: {config.get('development.enable')}")
print(f"Memory capacity: {config.get('memory.working_memory.capacity')}")
```

### 2.3 Running a Simulation

**Python Example - Simple Simulation:**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print("Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**Python Example - Enhanced Simulation:**

```python
import pynlm
import time

def run_enhanced_simulation(steps=1000):
    """Run an enhanced simulation with all Phase 6 features."""
    
    # Create enhanced configuration
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("neuromodulation.dopamine.enable", True)
    config.set("neuromodulation.curiosity.enable", True)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable enhanced features
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run enhanced simulation
    start_time = time.time()
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Enhanced learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 100 == 0:
            print(f"Step {step}: Reward={agent.getTotalReward():.4f}, "
                  f"Firing={brain.getAverageFiringRate():.2f}Hz")
    
    elapsed = time.time() - start_time
    print(f"\nSimulation complete in {elapsed:.2f} seconds")
    print(f"Steps per second: {steps/elapsed:.1f}")

# Run enhanced simulation
run_enhanced_simulation(1000)
```

## Section 3: Using Different Modes

The NLM system offers several execution modes for different use cases:

### 3.1 Phase Modes

| Mode | Description | Best Use Case |
|------|-------------|---------------|
| `phase2` | Basic neural computation | Learning basic neural dynamics |
| `phase3` | World interaction and learning | Testing environment interaction |
| `phase4` | Full cognitive development | Exploring cognitive emergence |
| `phase5` | Advanced learning and memory | Studying continual learning |
| `phase6` | Complete integration | Production and research |

### 3.2 Usage Examples

**Command Line - Phase 2:**

```bash
# Basic neural computation
./nlm --mode phase2 --neurons 500 --steps 1000
```

**Command Line - Phase 3:**

```bash
# World interaction with enhanced learning
./nlm --mode phase3 --neurons 1000 --steps 2000 \
     --development --curiosity
```

**Command Line - Phase 4:**

```bash
# Cognitive development with monitoring
./nlm --mode phase4 --neurons 2000 --steps 5000 \
     --enable-system-monitoring --verbose
```

**Command Line - Phase 6:**

```bash
# Complete integration with all features
./nlm --mode phase6 --neurons 2000 --steps 10000 \
     --enable-checkpointing --enable-replay \
     --enable-system-monitoring --enable-statistics-export
```

### 3.3 Integration Mode

**For advanced users, the integration mode combines all systems:**

```bash
# Integration with monitoring and checkpoints
./nlm --mode integration --neurons 2000 --steps 5000 \
     --enable-checkpointing --enable-system-monitoring \
     --enable-statistics-export --verbose
```

## Section 4: Configuring Experiments

### 4.1 Configuration Files

Create configuration files in JSON format:

**Simple Configuration Example (`simple_config.json`):**

```json
{
  "neuron_count": 1000,
  "steps": 5000,
  "regions": 4,
  "development": true,
  "checkpoint": false,
  "replay": false,
  "verbose": false,
  "enable_system_monitoring": true,
  "enable_profiling": false,
  "learning_rate": 0.01,
  "connection_probability": 0.1
}
```

**Advanced Configuration Example (`advanced_config.json`):**

```json
{
  "neuron_count": 5000,
  "steps": 20000,
  "regions": 8,
  "development": true,
  "checkpoint": true,
  "replay": true,
  "verbose": true,
  "enable_system_monitoring": true,
  "enable_profiling": true,
  "enable_statistics_export": true,
  "enable_scripted_ops": true,
  "auto_recover": true,
  "error_recovery_mode": "graceful",
  "learning_rate": 0.005,
  "connection_probability": 0.15,
  "curiosity": true,
  "novelty": true,
  "prediction": true,
  "working_memory_capacity": 5000,
  "episodic_memory_capacity": 20000
}
```

### 4.2 Using Configuration Files

**Load configuration from file:**

```bash
# Using CLI
./nlm --mode phase6 --config-file simple_config.json

# Or with additional options
./nlm --mode phase6 --config-file advanced_config.json --verbose
```

**Python Example:**

```python
import pynlm
import json

# Load configuration from file
with open("simple_config.json", 'r') as f:
    config_data = json.load(f)

# Convert to NLM configuration format
config = pynlm.createDefaultConfig()
config.set("neuron_count", config_data["neuron_count"])
config.set("steps", config_data["steps"])
config.set("development.enable", config_data["development"])
config.set("learning_rate", config_data["learning_rate"])

# Create and use brain
brain = pynlm.createBrain(config)
brain.initialize()
```

## Section 5: Interpreting Results

### 5.1 Key Metrics

The NLM system provides numerous metrics for analysis:

**Brain Metrics:**
- **Total Neurons**: Number of neurons in the brain
- **Firing Neurons**: Neurons currently active
- **Average Firing Rate**: Mean firing frequency (Hz)
- **Total Spikes**: Cumulative spike count
- **Development Stage**: Current developmental phase
- **Working Memory Usage**: Current memory utilization
- **Episodic Memory Count**: Number of stored experiences

**Learning Metrics:**
- **Curiosity Level**: Exploration motivation
- **Novelty Level**: Detection of new experiences
- **Prediction Error**: Learning signal
- **Neuromodulation Level**: Internal state modulation
- **Total Reward**: Accumulated reward

**System Metrics:**
- **CPU Usage**: Processor utilization
- **Memory Usage**: RAM consumption
- **Simulation Time**: Real-time elapsed

### 5.2 Example Results Analysis

**Python Example - Results Visualization:**

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np
import json

# Run simulation and collect data
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

metrics = []
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        metrics.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage()
        })

# Convert to DataFrame for analysis
import pandas as pd
df = pd.DataFrame(metrics)

# Plot results
plt.figure(figsize=(15, 5))

plt.subplot(1, 3, 1)
plt.plot(df['step'], df['firing_rate'])
plt.title('Average Firing Rate')
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')

plt.subplot(1, 3, 2)
plt.plot(df['step'], df['total_spikes'])
plt.title('Total Spike Count')
plt.xlabel('Step')
plt.ylabel('Total Spikes')

plt.subplot(1, 3, 3)
plt.hist(df['development_stage'], bins=len(df['development_stage'].unique()))
plt.title('Development Stage Distribution')
plt.xlabel('Development Stage')
plt.ylabel('Count')

plt.tight_layout()
plt.savefig('enhanced_simulation_results.png', dpi=300)
plt.show()

# Export results to JSON
results_dict = df.to_dict('records')
with open('simulation_results.json', 'w') as f:
    json.dump(results_dict, f, indent=2)

print("Results analysis complete! Saved to enhanced_simulation_results.png")
```

## Section 6: Using Advanced Features

### 6.1 Performance Benchmarking

**Command Line - Benchmarking:**

```bash
# Basic benchmarking
./nlm --mode phase6 --benchmark

# Detailed benchmarking with comparison
./nlm --mode phase6 --detailed-benchmark --benchmark-compare

# Compare different configurations
./nlm --mode phase6 --benchmark \
     --benchmark-neurons "500 1000 2000" \
     --benchmark-steps "1000 5000 10000"
```

**Python Example:**

```python
import pynlm

# Create benchmark configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Run benchmark
brain = pynlm.createBrain(config)
brain.initialize()

import time
start_time = time.time()
for step in range(1000):
    brain.step(step)
elapsed = time.time() - start_time

print(f"Benchmark results:")
print(f"  Time per step: {elapsed/1000:.4f} seconds")
print(f"  Steps per second: {1000/elapsed:.1f}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 6.2 Scripted Operations

**Command Line - Scripted Operations:**

```bash
# Batch mode
./nlm --mode phase6 --enable-scripted-ops --batch-mode

# Parameter sweep
./nlm --mode phase6 --enable-scripted-ops --parameter-sweep

# Random search
./nlm --mode phase6 --enable-scripted-ops --random-search --random-seeds 10

# Multi-run mode
./nlm --mode phase6 --enable-scripted-ops --multi-run-mode --multi-run-count 50
```

**Python Example:**

```python
import pynlm

# Create enhanced agent with all features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run enhanced simulation with scripted operations
for step in range(1000):
    # Enhanced interaction loop
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Enhanced learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print("Enhanced scripted operations complete!")
```

### 6.3 Error Handling and Recovery

**Command Line - Error Recovery:**

```bash
# Graceful error recovery (default)
./nlm --mode phase6 --auto-recover

# Aggressive recovery for critical systems
./nlm --mode integration --error-recovery-mode aggressive

# Strict recovery for research
./nlm --mode phase6 --error-recovery-mode strict
```

**Python Example:**

```python
import pynlm

# Create brain with error handling
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

if brain.initialize():
    try:
        # Run simulation with error handling
        for step in range(1000):
            brain.step(step)
            
            # Simulate potential error
            if step == 500:
                raise Exception("Simulated error for testing")
                
    except Exception as e:
        print(f"Error occurred: {e}")
        # Enhanced error handling logic
        print("Attempting recovery...")
        # Implement recovery strategy
```

## Section 7: Troubleshooting

### 7.1 Common Issues

**Issue: Installation Problems**

**Solution:**
```bash
# Ensure build tools are installed
apt-get update
apt-get install build-essential cmake

# Reinstall Python package
pip uninstall pynlm
pip install -e . --force-reinstall
```

**Issue: Simulation Performance**

**Solution:**
```bash
# Build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PROFILING=ON

# Monitor system resources
./nlm --mode phase6 --enable-system-monitoring
```

**Issue: Memory Usage**

**Solution:**
```bash
# Reduce simulation size
./nlm --mode phase6 --neurons 500 --steps 1000

# Enable memory pooling
./nlm --mode phase6 --enable-memory-pool
```

### 7.2 Debugging Techniques

**Enable Debug Logging:**

```bash
# Enable debug logging
./nlm --mode phase6 --verbose --debug
```

**Python Debugging:**

```python
import pynlm
import logging

# Enable debug logging
logging.basicConfig(level=logging.DEBUG)

# Create enhanced configuration with debug info
config = pynlm.createDefaultConfig()
config.set("development.enable", True)

# Run simulation with debugging
brain = pynlm.createBrain(config)
brain.initialize()

# Add custom error handling
try:
    for step in range(100):
        brain.step(step)
except Exception as e:
    print(f"Error at step {step}: {e}")
    import traceback
    traceback.print_exc()
```

## Section 8: Next Steps

### 8.1 For Beginners

1. **Start with basic examples**: Use the minimal examples provided
2. **Learn configuration options**: Understand the different configuration parameters
3. **Explore Phase modes**: Try different Phase modes to understand capabilities
4. **Analyze results**: Learn to interpret simulation metrics

### 8.2 For Intermediate Users

1. **Experiment with configurations**: Modify configuration parameters
2. **Use advanced features**: Explore benchmarking, scripted operations
3. **Analyze performance**: Monitor system resources and optimize
4. **Create custom examples**: Develop your own simulation scenarios

### 8.3 For Advanced Users

1. **Develop custom recovery strategies**: Implement specialized error handling
2. **Optimize performance**: Use profiling and monitoring tools
3. **Create custom configurations**: Design complex experiment setups
4. **Contribute to the community**: Share your findings and enhancements

## Section 9: Getting Help

### 9.1 Documentation Resources

- **Main Documentation**: This User Guide
- **Advanced CLI Reference**: `Enhanced_CLI_GUIDE.md`
- **Advanced Examples**: `Advanced_Examples_Guide.md`
- **System Architecture**: `docs/PHASE6_ARCHITECTURE.md`
- **Performance Guide**: `docs/PERFORMANCE.md`

### 9.2 Online Resources

- **GitHub Repository**: https://github.com/NeuralLearningMachine/NLM
- **Documentation Website**: https://docs.neurallearningmachine.org
- **Community Forum**: https://forum.neurallearningmachine.org
- **Issue Tracker**: https://github.com/NeuralLearningMachine/NLM/issues

### 9.3 Support Channels

- **Bug Reports**: Report issues on GitHub
- **Feature Requests**: Submit feature requests on GitHub
- **Technical Support**: Contact support@neurallearningmachine.org
- **Community Help**: Join the community forum

## Section 10: Conclusion

### 10.1 Summary

The NLM Phase 6 system with enhanced CLI features provides a comprehensive platform for artificial brain research and development. It integrates all neural systems into a coherent architecture with advanced configuration, monitoring, and error recovery capabilities.

### 10.2 Key Benefits

- **Integrated Architecture**: All neural systems work together seamlessly
- **Advanced Configuration**: Extensive options for customization
- **Performance Monitoring**: Real-time system and learning metrics
- **Error Recovery**: Graceful handling of system failures
- **Research Tools**: Comprehensive benchmarking and analysis tools

### 10.3 Future Directions

- **Machine Learning Integration**: Combine with modern ML techniques
- **Multi-Agent Systems**: Scale to multiple interacting agents
- **Real-time Adaptation**: Dynamic system adaptation
- **Cloud Deployment**: Scale to distributed computing

The NLM system continues to evolve, and this enhanced documentation provides the foundation for using and extending the Phase 6 capabilities.

---

*User Guide for Enhanced NLM Phase 6 System*
*Version 6.0 with Advanced CLI Features*
*Last Updated: 2026-10-21*