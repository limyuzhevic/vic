# USER_GUIDE.md

# NLM User Guide

## Overview

This comprehensive user guide provides detailed instructions for using the NLM (熙然) system. It covers everything from basic setup to advanced usage, including troubleshooting, performance optimization, and best practices.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Basic Operations](#basic-operations)
3. [Advanced Features](#advanced-features)
4. [Troubleshooting](#troubleshooting)
5. [Performance Optimization](#performance-optimization)
6. [Configuration Management](#configuration-management)
7. [Monitoring and Analytics](#monitoring-and-analytics)
8. [System Maintenance](#system-maintenance)
9. [Advanced Topics](#advanced-topics)
10. [API Reference](#api-reference)

---

## Quick Start

### 1. Installation

#### System Requirements

- **Operating System**: Linux (recommended), macOS (supported), Windows (experimental)
- **Processor**: 64-bit CPU with at least 2 cores
- **Memory**: 512 MB RAM minimum, 1 GB+ recommended
- **Storage**: 100 MB free space
- **Python**: 3.8+ (for Python bindings)

#### Installation Methods

**Option 1: Using Package Manager (Linux)
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install nlm

# CentOS/RHEL
sudo yum install nlm
```

**Option 2: From Source (Recommended for Development)
```bash
# Clone repository
cd /path/to/
git clone https://github.com/nlm/nlm.git
cd nlm

# Build and install
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install system-wide (optional)
sudo make install
```

**Option 3: Python Bindings Only
```bash
# Install Python bindings only
pip install pynlm
```

### 2. First Run

#### Running Phase 6 Demo
```bash
# Navigate to build directory
cd build

# Run Phase 6 integration demo
./nlm_phase6_demo
```

**Expected Output:**
```
Phase 6 Integration Demo Starting...
Phase 6 Brain Components:
  ✓ Memory Systems
  ✓ Neuromodulation
  ✓ Prediction System
  ✓ Cognition Systems
  ✓ Development Integration
  ✓ Replay and Consolidation
  ✓ Reward System
Phase 6 Demo Complete
```

#### Running Python Examples
```bash
# Navigate to examples directory
cd examples

# Run Python introduction example
python python_intro.py

# Run advanced integration example
python advanced_integration.py

# Run neuromodulation demo
python neuromodulation_demo.py

# Run memory system demo
python memory_system_demo.py

# Run development demo
python development_demo.py
```

### 3. Basic System Check

```bash
# Check NLM installation
nlm --version

# Check build information
cat build_info.txt  # If file exists

# Test Python bindings
python -c "import pynlm; print('NLM Python module version:', pynlm.__version__)"

# Test C++ bindings
python -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print('Neurons:', brain.getTotalNeuronCount())
print('Memory usage:', brain.getMemoryUsage() / 1024 / 1024, 'MB')
"
```

---

## Basic Operations

### 1. Creating and Configuring a Brain

#### Creating a Basic Brain
```python
import pynlm

# Create default brain
brain = pynlm.createBrain()
brain.initialize()

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Brain Configuration Options

**Neuron Configuration**
```python
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.08)
config.set("brain.connection_probability", 0.04)
config.set("brain.v_thresh", -50.0)  # Threshold
config.set("brain.v_rest", -70.0)    # Resting potential
config.set("brain.v_reset", -75.0)   # Reset potential
config.set("brain.tau_mem", 20.0)    # Membrane time constant
config.set("brain.tau_ref", 2.0)     # Refractory period
```

**Plasticity Configuration**
```python
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
```

**Neuromodulation Configuration**
```python
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.sensitivity", 2.0)
config.set("neuromod.novelty.enable", True)
```

### 2. Running Simulations

#### Basic Simulation Loop
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Simulation with Agent and World
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Enable key subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)
agent.enableDevelopment(True)

# Run complete simulation loop
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain computation
    brain.step(step)
    
    # Decode motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply reward
    reward = calculate_reward(percept)  # User-defined function
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

### 3. Working with Memory Systems

#### Working Memory
```python
# Access working memory
working_mem = brain.getWorkingMemory()

# Get current active entries
active_entries = working_mem.getActiveEntries()
for entry in active_entries:
    print(f"  Memory entry: {entry}")

# Clear working memory
working_mem.clear()

# Set new working memory
working_mem.set(key="sensor_data", value=perception_data, strength=0.8)
```

#### Episodic Memory
```python
# Access episodic memory
episodic_mem = brain.getEpisodicMemory()

# Get current experience sequence
sequence = episodic_mem.getCurrentSequence()
for experience in sequence:
    print(f"  Experience: {experience}")

# Store new experience
episodic_mem.store(experience=percept, reward=reward, importance=0.9)

# Replay experiences
episodic_mem.replay()
```

#### Associative Memory
```python
# Access associative memory
assoc_mem = brain.getAssociativeMemory()

# Learn new associations
assoc_mem.learn(pattern1=pattern_a, pattern2=pattern_b, strength=0.7)

# Retrieve associations
retrieved = assoc_mem.retrieve(pattern=pattern_a)
for association in retrieved:
    print(f"  Association: {association}")

# Get concept count
concept_count = assoc_mem.getConceptCount()
print(f"Total concepts: {concept_count}")
```

### 4. Neuromodulation

#### Dopamine System
```python
# Monitor dopamine levels
dopamine = brain.getNeuromodulators().dopamine
print(f"Dopamine level: {dopamine:.3f}")

# Apply reward modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.8)
```

#### Curiosity System
```python
# Monitor curiosity
curiosity = agent.getCuriosityLevel()
print(f"Curiosity level: {curiosity:.3f}")

# Enable curiosity-driven exploration
agent.enableCuriosity(True)
```

#### Novelty Detection
```python
# Monitor novelty
novelty = agent.getNoveltyLevel()
print(f"Novelty level: {novelty:.3f}")

# Enable novelty detection
agent.enableNovelty(True)
```

### 5. Development System

#### Developmental Stages
```python
# Get current developmental stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage.name}")

# Check if in critical period
in_critical_period = brain.isInCriticalPeriod()
print(f"In critical period: {in_critical_period}")

# Get development progress
progress = brain.getDevelopmentProgress()
print(f"Development progress: {progress:.1%}")

# Update development
brain.updateDevelopment(0.1)
```

#### Plasticity Management
```python
# Get current plasticity rate
plasticity_rate = agent.getPlasticityRate()
print(f"Plasticity rate: {plasticity_rate:.3f}")

# Get structural plasticity statistics
struct_plasticity = brain.getStructuralPlasticityStats()
print(f"Structural connections: {struct_plasticity['structural_connections']}")
print(f"Growth rate: {struct_plasticity['growth_rate']:.3f}")
print(f"Pruning rate: {struct_plasticity['pruning_rate']:.3f}")
```

### 6. Prediction System
```python
# Access prediction system
prediction_system = brain.getPredictionSystem()

# Get prediction error
prediction_error = prediction_system.getPredictionError()
print(f"Prediction error: {prediction_error:.3f}")

# Get confidence levels
confidence = prediction_system.getConfidence()
print(f"Prediction confidence: {confidence:.3f}")

# Update prediction system
prediction_system.update(predictions, targets)
```

### 7. Cognition Systems
```python
# Neural planning
planner = brain.getNeuralPlanner()
action_sequence = planner.generateActionSequence(goal_state)
print(f"Generated action sequence: {action_sequence}")

# Concept formation
concept_formation = brain.getConceptFormation()
concept_id = concept_formation.identifyConcept(neural_pattern)
print(f"Identified concept: {concept_id}")

# Attention system
attention = brain.getAttentionSystem()
focused_region = attention.getFocusedRegion()
print(f"Focused region: {focused_region}")
```

---

## Advanced Features

### 1. Checkpoint Management
```python
import pynlm

# Create and save checkpoint
brain = pynlm.createBrain()
brain.initialize()

# Save brain state
brain.save("checkpoint1.bin")

# Run some simulation steps
for step in range(100):
    brain.step(step)

# Save second checkpoint
brain.save("checkpoint2.bin")

# Load specific checkpoint
brain.load("checkpoint1.bin")
print("Loaded checkpoint 1")

# Load latest checkpoint
brain.load("latest_checkpoint.bin")
print("Loaded latest checkpoint")
```

### 2. Advanced Configuration
```python
import pynlm

# Create complex configuration
config = pynlm.createConfig()

# Configure brain parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.06)
config.set("brain.v_thresh", -45.0)

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.003)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction.error_sensitivity", 1.5)

# Configure memory
config.set("memory.working_memory.capacity", 200)
config.set("memory.episodic_memory.encoding_strength", 0.9)
config.set("memory.associative_memory.pattern_threshold", 0.7)

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. Multi-Agent Systems
```python
import pynlm

# Create multiple agents
agents = []
brains = []
world = pynlm.createSimpleWorld()

for i in range(3):
    # Each agent gets unique configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500 + i * 500)
    config.set("neuromod.curiosity.scale", 1.0 + i * 0.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    brains.append(brain)
    agents.append(agent)

# Run multi-agent simulation
for step in range(500):
    world.update(0.1)
    
    for i, agent in enumerate(agents):
        # Each agent processes input independently
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brains[i].step(step)
        
        # Each agent produces its own action
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
```

### 4. Performance Monitoring
```python
import pynlm
import time
import psutil
import os

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.process = psutil.Process(os.getpid())
        self.metrics = {
            'steps': 0,
            'start_time': time.time(),
            'memory_samples': [],
            'cpu_samples': [],
            'activity_samples': [],
            'neuromod_samples': []
        }
    
    def collect_metrics(self, step):
        """Collect performance metrics for current step."""
        self.metrics['steps'] = step + 1
        
        # Memory usage
        memory_info = self.process.memory_info()
        self.metrics['memory_samples'].append(memory_info.rss)
        
        # CPU usage
        cpu_percent = self.process.cpu_percent()
        self.metrics['cpu_samples'].append(cpu_percent)
        
        # Brain activity
        activity = self.brain.getAverageActivity()
        self.metrics['activity_samples'].append(activity)
        
        # Neuromodulator levels
        neuromod = self.brain.getNeuromodulators()
        neuromod_stats = {
            'curiosity': neuromod.curiosity,
            'novelty': neuromod.novelty,
            'dopamine': neuromod.dopamine,
            'prediction_error': neuromod.prediction_error
        }
        self.metrics['neuromod_samples'].append(neuromod_stats)
    
    def print_summary(self):
        """Print performance summary."""
        duration = time.time() - self.metrics['start_time']
        steps_per_sec = self.metrics['steps'] / duration
        
        print(f"Performance Summary:")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Steps per second: {steps_per_sec:.1f}")
        print(f"  Total steps: {self.metrics['steps']}")
        print(f"  Memory usage: {max(self.metrics['memory_samples']) / 1024 / 1024:.1f} MB (max)")
        print(f"  Average activity: {np.mean([s['activity'] for s in self.metrics['activity_samples']]):.3f}")
        print(f"  Average curiosity: {np.mean([s['curiosity'] for s in self.metrics['neuromod_samples']]):.3f}")
        print(f"  Average novelty: {np.mean([s['novelty'] for s in self.metrics['neuromod_samples']]):.3f}")
        print(f"  Average prediction error: {np.mean([s['prediction_error'] for s in self.metrics['neuromod_samples']]):.3f}")

# Use performance monitor
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

monitor = PerformanceMonitor(brain)

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        monitor.collect_metrics(step)
        monitor.print_summary()

monitor.print_summary()  # Final summary
```

---

## Troubleshooting

### Common Issues and Solutions

#### 1. Import Errors

**Problem:** `ImportError: No module named 'pynlm'`

**Solution:**
```bash
# Reinstall Python bindings
pip uninstall pynlm
pip install -e .

# Check installation
python -c "import pynlm; print('Success')"
```

#### 2. Memory Errors

**Problem:** `MemoryError` or excessive memory usage

**Solution:**
```python
# Reduce memory usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Smaller brain
config.set("brain.synapse_density", 0.01)  # Lower connectivity

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")
```

#### 3. Performance Issues

**Problem:** Simulation too slow

**Solution:**
```bash
# Build with optimizations
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Use smaller brain for testing
config.set("brain.neuron_count", 500)

# Enable optimizations
config.set("brain.enable_vectorization", True)
config.set("brain.enable_parallel", True)
```

#### 4. Segmentation Faults

**Problem:** Program crashes with segmentation fault

**Solution:**
```bash
# Build with debugging
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run with valgrind (Linux)
valgrind --tool=memcheck ./nlm_simulation
```

### Troubleshooting Checklist

1. **Installation Issues**
   - ✓ Python bindings installed correctly?
   - ✓ All dependencies present?
   - ✓ Environment variables set?

2. **Memory Issues**
   - ✓ Configuration optimized for memory?
   - ✓ Brain size appropriate?
   - ✓ Memory leaks present?

3. **Performance Issues**
   - ✓ Build with optimizations?
   - ✓ Simulation parameters appropriate?
   - ✓ Hardware resources sufficient?

4. **Configuration Issues**
   - ✓ Configuration files valid?
   - ✓ Parameters within valid ranges?
   - ✓ Dependencies satisfied?

5. **Simulation Issues**
   - ✓ Brain initialized before stepping?
   - ✓ Configuration valid?
   - ✓ Input data properly formatted?

### Debugging Tools

#### Python Debugging
```python
import pynlm
import traceback

# Enable Python debugging
import sys
sys.stderr = sys.stdout

# Use debugger
import pdb

# Create brain with debugging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step through with debugger
for step in range(10):
    print(f"Step {step}")
    pdb.set_trace()  # Debug here
    brain.step(step)
```

#### C++ Debugging
```bash
# Build with debugging flags
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run with valgrind
valgrind --tool=memcheck --leak-check=full ./nlm_simulation

# Use GDB
gdb ./nlm_simulation
# (gdb commands)
```

---

## Performance Optimization

### 1. Configuration Optimization

#### Optimal Configuration for Different Use Cases

**For Research and Development**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.08)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
```

**For Production**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", False)  # Disable for performance
config.set("neuromod.curiosity.enable", False)  # Disable for stability
config.set("neuromod.novelty.enable", False)    # Disable for stability
```

**For Resource-Constrained Environments**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.02)
config.set("plasticity.stdp.enable", False)   # Disable for performance
config.set("plasticity.hebbian.enable", False) # Disable for performance
config.set("neuromod.curiosity.enable", False) # Disable for performance
config.set("neuromod.novelty.enable", False)   # Disable for performance
```

### 2. Build Optimization

#### Recommended Build Options

```bash
# Release build (production)
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Debug build (development)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
make -j$(nproc)

# Custom optimization flags
cmake .. -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native"
make -j$(nproc)
```

### 3. Runtime Optimization

#### CPU Optimization
```python
# Use SIMD instructions where available
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.enable_vectorization", True)
config.set("brain.neuron_count", 1024)  # Power of 2 for better SIMD

brain = pynlm.createBrain(config)
brain.initialize()

# Optimize memory access patterns
for step in range(1000):
    # Continuous access to same data
    activity = brain.getAverageActivity()
    firing = brain.getFiringNeuronCount()
    brain.step(step)
```

#### Memory Optimization
```python
# Use memory-efficient configurations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)
config.set("brain.synapse_density", 0.04)
config.set("memory.working_memory.capacity", 50)

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory usage
import psutil
process = psutil.Process()
memory_mb = process.memory_info().rss / 1024 / 1024
print(f"Current memory usage: {memory_mb:.1f} MB")
```

### 4. Parallel Processing

#### Multi-Core Optimization
```python
import pynlm
import multiprocessing as mp

class ParallelNLM:
    def __init__(self, num_workers=None):
        self.num_workers = num_workers or mp.cpu_count()
    
    def run_parallel_simulations(self, num_simulations, steps_per_simulation):
        """Run multiple simulations in parallel."""
        pool = mp.Pool(processes=self.num_workers)
        
        # Prepare arguments
        args = [(i, steps_per_simulation) for i in range(num_simulations)]
        
        # Run simulations
        results = pool.starmap(self._run_single_simulation, args)
        
        pool.close()
        pool.join()
        
        return results
    
    def _run_single_simulation(self, simulation_id, steps):
        """Run a single simulation."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        return {
            'simulation_id': simulation_id,
            'steps_completed': steps,
            'final_activity': brain.getAverageActivity(),
            'firing_neurons': brain.getFiringNeuronCount()
        }

# Use parallel processing
parallel = ParallelNLM()
results = parallel.run_parallel_simulations(10, 200)
```

---

## Configuration Management

### 1. Configuration Files

#### Configuration File Format

**JSON Configuration**
```json
{
    "brain": {
        "neuron_count": 3000,
        "synapse_density": 0.08,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.5
        },
        "curiosity": {
            "enable": true,
            "sensitivity": 2.0
        }
    }
}
```

#### Saving and Loading Configurations
```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

# Save to JSON file
config.saveToFile("my_config.json")

# Load from JSON file
loaded_config = pynlm.createConfig()
loaded_config.loadFromFile("my_config.json")

# Convert to dictionary
config_dict = config.exportToDict()
print(json.dumps(config_dict, indent=2))

# Import from dictionary
config2 = pynlm.createConfig()
config2.importFromDict(config_dict)
```

### 2. Preset Configurations

#### Built-in Presets
```python
import pynlm

# Small configuration (for testing)
small_config = pynlm.createSmallConfig()

# Standard configuration (for general use)
standard_config = pynlm.createStandardConfig()

# Large configuration (for research)
large_config = pynlm.createLargeConfig()

# Performance configuration
performance_config = pynlm.createPerformanceConfig()

# Research configuration
research_config = pynlm.createResearchConfig()

# Use configurations
brain_small = pynlm.createBrain(small_config)
brain_standard = pynlm.createBrain(standard_config)
brain_large = pynlm.createBrain(large_config)
brain_performance = pynlm.createBrain(performance_config)
brain_research = pynlm.createBrain(research_config)

# Initialize brains
brain_small.initialize()
brain_standard.initialize()
brain_large.initialize()
brain_performance.initialize()
brain_research.initialize()
```

### 3. Configuration Validation
```python
import pynlm

# Validate configuration
config = pynlm.createDefaultConfig()

# Check for required parameters
required_keys = ["brain.neuron_count", "brain.synapse_density", "brain.v_thresh"]
for key in required_keys:
    if not config.has(key):
        print(f"Missing required parameter: {key}")
    else:
        value = config.getValue(key)
        print(f"{key}: {value}")

# Validate parameter ranges
if config.getValue("brain.neuron_count") < 100:
    print("Warning: Small brain size may affect performance")

if config.getValue("brain.neuron_count") > 20000:
    print("Warning: Large brain size may require significant memory")

# Validate configuration
if config.isValid():
    print("Configuration is valid")
else:
    print("Configuration is invalid")
```

---

## Monitoring and Analytics

### 1. System Monitoring

#### Basic Monitoring
```python
import pynlm
import time
import psutil
import matplotlib.pyplot as plt

class SystemMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.process = psutil.Process()
        self.metrics = {
            'time_steps': [],
            'cpu_usage': [],
            'memory_usage': [],
            'activity_levels': [],
            'curiosity_levels': []
        }
    
    def collect_metrics(self, step):
        """Collect system metrics for current step."""
        self.metrics['time_steps'].append(step)
        self.metrics['cpu_usage'].append(self.process.cpu_percent())
        self.metrics['memory_usage'].append(self.process.memory_info().rss)
        self.metrics['activity_levels'].append(self.brain.getAverageActivity())
        self.metrics['curiosity_levels'].append(self.brain.getCuriosityLevel())
    
    def print_real_time_stats(self, step):
        """Print real-time statistics."""
        print(f"Step {step}:")
        print(f"  CPU Usage: {self.metrics['cpu_usage'][-1]:.1f}%")
        print(f"  Memory Usage: {self.metrics['memory_usage'][-1] / 1024 / 1024:.1f} MB")
        print(f"  Activity Level: {self.metrics['activity_levels'][-1]:.3f}")
        print(f"  Curiosity Level: {self.metrics['curiosity_levels'][-1]:.3f}")
    
    def plot_metrics(self):
        """Plot collected metrics."""
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        
        # CPU usage
        ax1.plot(self.metrics['time_steps'], self.metrics['cpu_usage'])
        ax1.set_title('CPU Usage')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('CPU (%)')
        
        # Memory usage
        ax2.plot(self.metrics['time_steps'], 
                [m / 1024 / 1024 for m in self.metrics['memory_usage']])
        ax2.set_title('Memory Usage')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Memory (MB)')
        
        # Activity levels
        ax3.plot(self.metrics['time_steps'], self.metrics['activity_levels'])
        ax3.set_title('Activity Level')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Activity')
        
        # Curiosity levels
        ax4.plot(self.metrics['time_steps'], self.metrics['curiosity_levels'])
        ax4.set_title('Curiosity Level')
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Curiosity')
        
        plt.tight_layout()
        plt.show()

# Use system monitor
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

monitor = SystemMonitor(brain)

for step in range(500):
    brain.step(step)
    
    if step % 50 == 0:
        monitor.collect_metrics(step)
        monitor.print_real_time_stats(step)

monitor.plot_metrics()
```

### 2. Performance Analytics

#### Detailed Performance Analysis
```python
import pynlm
import numpy as np
from scipy import stats
import pandas as pd

class PerformanceAnalyzer:
    def __init__(self, brain):
        self.brain = brain
        self.performance_data = []
    
    def collect_performance_data(self, step):
        """Collect performance data for analysis."""
        data = {
            'step': step,
            'activity': self.brain.getAverageActivity(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_neurons': self.brain.getTotalNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'curiosity': self.brain.getCuriosityLevel(),
            'novelty': self.brain.getNoveltyLevel(),
            'prediction_error': self.brain.getPredictionError(),
            'development_stage': self.brain.getDevelopmentalStage().name,
            'memory_usage': self.brain.getMemoryUsage()
        }
        
        self.performance_data.append(data)
    
    def analyze_stability(self):
        """Analyze system stability."""
        df = pd.DataFrame(self.performance_data)
        
        # Calculate statistics
        stability_metrics = {
            'activity_stability': df['activity'].std(),
            'firing_stability': df['firing_rate'].std(),
            'curiosity_stability': df['curiosity'].std(),
            'novelty_stability': df['novelty'].std(),
            'prediction_stability': df['prediction_error'].std()
        }
        
        # Determine stability (lower std = more stable)
        stable_metrics = {
            key: value < 0.1 for key, value in stability_metrics.items()
        }
        
        return stability_metrics, stable_metrics
    
    def analyze_efficiency(self):
        """Analyze system efficiency."""
        df = pd.DataFrame(self.performance_data)
        
        # Calculate efficiency metrics
        efficiency_metrics = {
            'average_activity': df['activity'].mean(),
            'activity_range': df['activity'].max() - df['activity'].min(),
            'average_firing_rate': df['firing_rate'].mean(),
            'firing_efficiency': df['firing_rate'].mean() / df['activity'].mean(),
            'memory_efficiency': df['memory_usage'].mean() / df['firing_rate'].mean()
        }
        
        # Calculate resource efficiency
        efficiency_ratios = {
            'computational_efficiency': efficiency_metrics['average_activity'] / 
                                       efficiency_metrics['firing_rate'],
            'memory_efficiency': efficiency_metrics['firing_rate'] / 
                               efficiency_metrics['memory_usage']
        }
        
        return efficiency_metrics, efficiency_ratios
    
    def analyze_learning(self):
        """Analyze learning dynamics."""
        df = pd.DataFrame(self.performance_data)
        
        # Track changes over time
        mid_point = len(df) // 2
        early_data = df.iloc[:mid_point]
        late_data = df.iloc[mid_point:]
        
        learning_metrics = {
            'activity_change': late_data['activity'].mean() - early_data['activity'].mean(),
            'curiosity_change': late_data['curiosity'].mean() - early_data['curiosity'].mean(),
            'novelty_change': late_data['novelty'].mean() - early_data['novelty'].mean(),
            'prediction_change': late_data['prediction_error'].mean() - early_data['prediction_error'].mean(),
            'development_progress': df['development_stage'].value_counts().index[0]
        }
        
        # Determine if learning is occurring
        learning_occurring = {
            'activity_increasing': learning_metrics['activity_change'] > 0.01,
            'curiosity_increasing': learning_metrics['curiosity_change'] > 0.01,
            'novelty_increasing': learning_metrics['novelty_change'] > 0.01,
            'development_advanced': learning_metrics['development_progress'] != 'Initial'
        }
        
        return learning_metrics, learning_occurring
    
    def run_comprehensive_analysis(self):
        """Run comprehensive performance analysis."""
        print("Running comprehensive performance analysis...")
        
        stability_metrics, stable = self.analyze_stability()
        efficiency_metrics, efficiency_ratios = self.analyze_efficiency()
        learning_metrics, learning = self.analyze_learning()
        
        print("\nStability Analysis:")
        for metric, value in stability_metrics.items():
            print(f"  {metric}: {value:.4f} ({'Stable' if stable[metric] else 'Unstable'})")
        
        print("\nEfficiency Analysis:")
        for metric, value in efficiency_metrics.items():
            print(f"  {metric}: {value:.4f}")
        
        print("\nLearning Analysis:")
        for metric, value in learning_metrics.items():
            print(f"  {metric}: {value:.4f} ({'Yes' if learning[metric] else 'No'})")
        
        # Overall system assessment
        overall_stable = all(stable.values())
        overall_efficient = all(v > 0.1 for v in efficiency_metrics.values())
        overall_learning = any(learning.values())
        
        system_assessment = {
            'stable': overall_stable,
            'efficient': overall_efficient,
            'learning': overall_learning,
            'recommendations': self._generate_recommendations(stability_metrics, efficiency_metrics, learning)
        }
        
        return system_assessment
    
    def _generate_recommendations(self, stability, efficiency, learning):
        """Generate recommendations based on analysis."""
        recommendations = []
        
        if not learning['activity_increasing']:
            recommendations.append("Consider increasing brain complexity or plasticity")
        
        if not learning['curiosity_increasing']:
            recommendations.append("Enable curiosity-driven exploration")
        
        if not learning['novelty_increasing']:
            recommendations.append("Enable novelty detection")
        
        if not stable['activity_stability']:
            recommendations.append("System activity is unstable, consider tuning parameters")
        
        if not stable['curiosity_stability']:
            recommendations.append("Curiosity system is unstable, check neuromodulation settings")
        
        if not stable['prediction_stability']:
            recommendations.append("Prediction system is unstable, check prediction parameters")
        
        if not efficiency['computational_efficiency']:
            recommendations.append("System efficiency is low, consider optimization")
        
        if len(recommendations) == 0:
            recommendations.append("System is operating optimally")
        
        return recommendations

# Use performance analyzer
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

analyzer = PerformanceAnalyzer(brain)

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        analyzer.collect_performance_data(step)

results = analyzer.run_comprehensive_analysis()
print(f"\nSystem Assessment: {results}")
```

---

## System Maintenance

### 1. Backup and Recovery

#### System Backup
```python
import pynlm
import shutil
import os
from datetime import datetime

class SystemBackup:
    def __init__(self, backup_directory="backups"):
        self.backup_directory = backup_directory
        os.makedirs(backup_directory, exist_ok=True)
    
    def create_backup(self, brain, description="Manual backup"):
        """Create system backup."""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        backup_name = f"nlm_backup_{timestamp}_{description}"
        backup_path = os.path.join(self.backup_directory, backup_name)
        
        try:
            # Save brain state
            brain.save(backup_path + ".bin")
            
            # Create metadata file
            metadata = {
                'timestamp': datetime.now().isoformat(),
                'description': description,
                'neuron_count': brain.getTotalNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'activity_level': brain.getAverageActivity(),
                'development_stage': brain.getDevelopmentalStage().name,
                'curiosity_level': brain.getCuriosityLevel()
            }
            
            import json
            with open(backup_path + ".meta", 'w') as f:
                json.dump(metadata, f, indent=2)
            
            print(f"Backup created: {backup_path}")
            return backup_path
            
        except Exception as e:
            print(f"Backup failed: {e}")
            return None
    
    def list_backups(self):
        """List available backups."""
        if not os.path.exists(self.backup_directory):
            return []
        
        backups = []
        for filename in os.listdir(self.backup_directory):
            if filename.startswith("nlm_backup_") and filename.endswith(".bin"):
                metadata_file = filename.replace(".bin", ".meta")
                if os.path.exists(os.path.join(self.backup_directory, metadata_file)):
                    with open(os.path.join(self.backup_directory, metadata_file), 'r') as f:
                        metadata = json.load(f)
                    backups.append({
                        'path': os.path.join(self.backup_directory, filename),
                        'metadata': metadata
                    })
        
        return sorted(backups, key=lambda x: x['metadata']['timestamp'], reverse=True)
    
    def restore_backup(self, backup_path):
        """Restore system from backup."""
        try:
            brain.load(backup_path)
            print(f"System restored from: {backup_path}")
            return True
            
        except Exception as e:
            print(f"Restore failed: {e}")
            return False
    
    def cleanup_old_backups(self, days_to_keep=30):
        """Clean up old backups."""
        import time
        
        current_time = time.time()
        backups = self.list_backups()
        
        for backup in backups:
            backup_time = datetime.fromisoformat(backup['metadata']['timestamp']).timestamp()
            age = current_time - backup_time
            
            if age > days_to_keep * 24 * 60 * 60:  # Older than specified days
                # Delete backup files
                os.remove(backup['path'] + ".bin")
                os.remove(backup['path'] + ".meta")
                print(f"Deleted old backup: {backup['path']}")

# Use backup system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

backup_system = SystemBackup()

# Create backup
backup_path = backup_system.create_backup(brain, "daily_run")

# List backups
backups = backup_system.list_backups()
print(f"Available backups: {len(backups)}")
for backup in backups[:5]:  # Show last 5 backups
    print(f"  {backup['metadata']['timestamp']}: {backup['metadata']['description']}")

# Restore backup (if needed)
# backup_system.restore_backup(backup_path)

# Cleanup old backups
backup_system.cleanup_old_backups(days_to_keep=7)
```

### 2. Log Management

#### Logging System
```python
import pynlm
import logging
from datetime import datetime
import json

class SystemLogger:
    def __init__(self, log_file="nlm_system.log"):
        self.log_file = log_file
        self.logger = logging.getLogger('nlm_system')
        self.logger.setLevel(logging.INFO)
        
        # Create console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        
        # Create file handler
        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(logging.DEBUG)
        
        # Create formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        
        # Set formatters
        console_handler.setFormatter(formatter)
        file_handler.setFormatter(formatter)
        
        # Add handlers
        self.logger.addHandler(console_handler)
        self.logger.addHandler(file_handler)
    
    def log_simulation_step(self, step, brain, action):
        """Log simulation step details."""
        log_data = {
            'timestamp': datetime.now().isoformat(),
            'step': step,
            'brain_state': {
                'activity': brain.getAverageActivity(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_neurons': brain.getTotalNeuronCount(),
                'curiosity': brain.getCuriosityLevel(),
                'novelty': brain.getNoveltyLevel(),
                'development_stage': brain.getDevelopmentalStage().name
            },
            'action': action.__dict__ if hasattr(action, '__dict__') else str(action),
            'system_resources': self._get_system_resources()
        }
        
        self.logger.info(f"Step {step}: {json.dumps(log_data)}")
    
    def _get_system_resources(self):
        """Get current system resource usage."""
        import psutil
        process = psutil.Process()
        
        return {
            'cpu_percent': process.cpu_percent(),
            'memory_mb': process.memory_info().rss / 1024 / 1024,
            'memory_percent': process.memory_percent()
        }
    
    def log_error(self, error, context=None):
        """Log error with context."""
        log_data = {
            'timestamp': datetime.now().isoformat(),
            'error_type': type(error).__name__,
            'error_message': str(error),
            'context': context,
            'system_resources': self._get_system_resources()
        }
        
        self.logger.error(f"Error: {json.dumps(log_data)}")
    
    def generate_log_report(self):
        """Generate log analysis report."""
        import pandas as pd
        
        try:
            # Parse log file
            data = []
            with open(self.log_file, 'r') as f:
                for line in f:
                    if 'Step' in line and 'INFO' in line:
                        try:
                            log_data = json.loads(line.split('INFO - ')[1])
                            data.append(log_data)
                        except:
                            continue
            
            if not data:
                return "No log data available"
            
            # Convert to DataFrame
            df = pd.DataFrame(data)
            
            # Generate statistics
            report = f"""
Log Analysis Report
===================
Generated: {datetime.now().isoformat()}

Simulation Statistics:
- Total steps recorded: {len(df)}
- Average activity: {df['brain_state'].apply(lambda x: x['activity']).mean():.3f}
- Average curiosity: {df['brain_state'].apply(lambda x: x['curiosity']).mean():.3f}
- Average novelty: {df['brain_state'].apply(lambda x: x['novelty']).mean():.3f}

Performance Metrics:
- CPU usage: {df['system_resources'].apply(lambda x: x['cpu_percent']).mean():.1f}%
- Memory usage: {df['system_resources'].apply(lambda x: x['memory_mb']).mean():.1f} MB

Development Progress:
- Development stages: {df['brain_state'].apply(lambda x: x['development_stage']).value_counts().to_dict()}

Recommendations:
- Check for performance bottlenecks
- Monitor memory usage trends
- Adjust simulation parameters if needed
"""
            
            return report
            
        except Exception as e:
            return f"Error generating log report: {e}"

# Use logging system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

logger = SystemLogger()

# Run simulation with logging
for step in range(100):
    brain.step(step)
    logger.log_simulation_step(step, brain, action=None)

# Generate report
report = logger.generate_log_report()
print(report)
```

### 3. System Health Monitoring

#### Health Check
```python
import pynlm
import time
import psutil
from datetime import datetime

class SystemHealthMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.process = psutil.Process()
        self.health_checks = []
    
    def perform_health_check(self):
        """Perform comprehensive health check."""
        timestamp = datetime.now()
        health_status = {
            'timestamp': timestamp.isoformat(),
            'overall_health': 'healthy',
            'checks': {}
        }
        
        # Check memory usage
        memory_info = self.process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        
        health_status['checks']['memory_usage'] = {
            'status': 'healthy' if memory_mb < 1000 else 'warning',
            'value': memory_mb,
            'threshold': 1000
        }
        
        # Check CPU usage
        cpu_percent = self.process.cpu_percent()
        
        health_status['checks']['cpu_usage'] = {
            'status': 'healthy' if cpu_percent < 80 else 'warning',
            'value': cpu_percent,
            'threshold': 80
        }
        
        # Check brain activity
        activity = self.brain.getAverageActivity()
        
        health_status['checks']['brain_activity'] = {
            'status': 'healthy' if 0.1 <= activity <= 0.9 else 'warning',
            'value': activity,
            'threshold_min': 0.1,
            'threshold_max': 0.9
        }
        
        # Check curiosity level
        curiosity = self.brain.getCuriosityLevel()
        
        health_status['checks']['curiosity_level'] = {
            'status': 'healthy' if 0.1 <= curiosity <= 0.9 else 'warning',
            'value': curiosity,
            'threshold_min': 0.1,
            'threshold_max': 0.9
        }
        
        # Check development progress
        development_stage = self.brain.getDevelopmentalStage()
        dev_progress = self.brain.getDevelopmentProgress()
        
        health_status['checks']['development'] = {
            'status': 'healthy' if dev_progress > 0.1 else 'warning',
            'stage': development_stage.name,
            'progress': dev_progress
        }
        
        # Check system resources
        health_status['checks']['system_resources'] = {
            'status': 'healthy' if self._check_system_resources() else 'warning',
            'cpu_count': self.process.cpu_count(),
            'memory_available': self.process.memory_info().vms / 1024 / 1024
        }
        
        # Determine overall health
        health_status['overall_health'] = self._determine_overall_health(health_status['checks'])
        
        self.health_checks.append(health_status)
        
        return health_status
    
    def _check_system_resources(self):
        """Check if system resources are adequate."""
        try:
            # Check available memory (at least 512 MB)
            available_memory = self.process.memory_info().vms / 1024 / 1024
            return available_memory > 512
        except:
            return False
    
    def _determine_overall_health(self, checks):
        """Determine overall system health."""
        if any(check['status'] == 'warning' for check in checks.values()):
            return 'warning'
        else:
            return 'healthy'
    
    def run_health_monitoring(self, duration_seconds, check_interval=10):
        """Run continuous health monitoring."""
        print(f"Starting health monitoring for {duration_seconds} seconds...")
        print(f"Check interval: {check_interval} seconds")
        
        start_time = time.time()
        end_time = start_time + duration_seconds
        
        while time.time() < end_time:
            health_status = self.perform_health_check()
            
            if health_status['overall_health'] == 'warning':
                print(f"⚠️  Health warning: {health_status}")
            else:
                print(f"✓ Health check passed: {health_status['timestamp']}")
            
            # Sleep until next check
            time.sleep(check_interval)
        
        print("\nHealth monitoring completed!")
        self._generate_health_report()
    
    def _generate_health_report(self):
        """Generate health monitoring report."""
        if not self.health_checks:
            print("No health check data available")
            return
        
        report = f"""
Health Monitoring Report
=======================
Generated: {datetime.now().isoformat()}

Total checks performed: {len(self.health_checks)}

Health Status Summary:
- Healthy checks: {len([h for h in self.health_checks if h['overall_health'] == 'healthy'])}
- Warning checks: {len([h for h in self.health_checks if h['overall_health'] == 'warning'])}

Resource Usage Trends:
"""
        
        # Analyze trends
        memory_values = [h['checks']['memory_usage']['value'] for h in self.health_checks]
        cpu_values = [h['checks']['cpu_usage']['value'] for h in self.health_checks]
        
        report += f"- Memory usage: {min(memory_values):.1f} - {max(memory_values):.1f} MB\n"
        report += f"- CPU usage: {min(cpu_values):.1f} - {max(cpu_values):.1f}%\n"
        
        print(report)

# Use health monitor
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

monitor = SystemHealthMonitor(brain)

# Run health monitoring
monitor.run_health_monitoring(duration_seconds=60, check_interval=10)
```

---

## Advanced Topics

### 1. Custom Development

#### Extending NLM with Custom Classes
```python
import pynlm

class CustomBrain(pynlm.Brain):
    """Custom brain class with additional functionality."""
    
    def __init__(self, config):
        super().__init__(config)
        self.custom_metrics = {}
    
    def track_custom_metric(self, name, value):
        """Track custom metric."""
        self.custom_metrics[name] = {
            'value': value,
            'timestamp': datetime.now(),
            'step': self.getCurrentStep()
        }
    
    def get_custom_metric_history(self, name):
        """Get history of custom metric."""
        return self.custom_metrics.get(name, [])
    
    def enhanced_step(self, step):
        """Enhanced step with custom logic."""
        # Pre-step processing
        self.pre_step_processing(step)
        
        # Run parent step
        super().step(step)
        
        # Post-step processing
        self.post_step_processing(step)
    
    def pre_step_processing(self, step):
        """Custom pre-step processing."""
        # Add custom logic here
        pass
    
    def post_step_processing(self, step):
        """Custom post-step processing."""
        # Add custom logic here
        activity = self.getAverageActivity()
        self.track_custom_metric('activity', activity)

# Use custom brain
config = pynlm.createDefaultConfig()
custom_brain = CustomBrain(config)
custom_brain.initialize()

# Run enhanced simulation
for step in range(100):
    custom_brain.enhanced_step(step)
```

### 2. Integration with External Libraries

#### Integration with TensorFlow
```python
import pynlm
import tensorflow as tf
import numpy as np

class NLMTensorFlowIntegration:
    """Integration of NLM with TensorFlow."""
    
    def __init__(self):
        # Create NLM brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create TensorFlow model
        self.tf_model = self._create_tensorflow_model()
        
        # Create data pipeline
        self.data_pipeline = self._create_data_pipeline()
    
    def _create_tensorflow_model(self):
        """Create TensorFlow model for NLM integration."""
        model = tf.keras.Sequential([
            tf.keras.layers.Dense(512, activation='relu', input_shape=(100,)),
            tf.keras.layers.Dense(256, activation='relu'),
            tf.keras.layers.Dense(128, activation='relu'),
            tf.keras.layers.Dense(64, activation='relu'),
            tf.keras.layers.Dense(1)
        ])
        
        model.compile(optimizer='adam', loss='mse')
        return model
    
    def _create_data_pipeline(self):
        """Create data pipeline for training."""
        # This would integrate with NLM simulation data
        # For now, it's a placeholder
        return None
    
    def extract_features(self, brain):
        """Extract features from NLM brain state."""
        features = {
            'neural_activity': brain.getAverageActivity(),
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': brain.getCuriosityLevel(),
            'novelty': brain.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage().name,
            'memory_usage': brain.getMemoryUsage()
        }
        
        return features
    
    def train_with_nlm_data(self, epochs=100):
        """Train TensorFlow model with NLM data."""
        print("Training TensorFlow model with NLM data...")
        
        for epoch in range(epochs):
            epoch_loss = 0
            
            # Collect NLM data
            for step in range(100):
                self.brain.step(step)
                
                # Extract features
                features = self.extract_features(self.brain)
                
                # Create target (simulated prediction)
                target = np.random.rand()  # Placeholder for actual target
                
                # Train model
                loss = self.tf_model.train_on_batch(
                    np.array([list(features.values())]),
                    np.array([target])
                )
                
                epoch_loss += loss
            
            avg_loss = epoch_loss / 100
            print(f"Epoch {epoch + 1}/{epochs}, Loss: {avg_loss:.4f}")
    
    def predict_with_nlm(self, brain_features):
        """Make predictions using trained TensorFlow model."""
        features_array = np.array([list(brain_features.values())])
        prediction = self.tf_model.predict(features_array, verbose=0)
        return prediction[0][0]

# Use NLM-TensorFlow integration
nlm_tf = NLMTensorFlowIntegration()

# Run training
nlm_tf.train_with_nlm_data(epochs=10)

# Make predictions
brain_features = nlm_tf.extract_features(nlm_tf.brain)
prediction = nlm_tf.predict_with_nlm(brain_features)
print(f"Prediction: {prediction:.4f}")
```

### 3. Research and Experimental Features

#### Advanced Research Features
```python
import pynlm
import numpy as np
from scipy import stats
import matplotlib.pyplot as plt

class NLMResearchToolkit:
    """Research toolkit for NLM experiments."""
    
    def __init__(self, brain):
        self.brain = brain
        self.experiment_data = {}
    
    def run_control_experiment(self, steps=100):
        """Run control experiment (no neuromodulation)."""
        print("Running control experiment...")
        
        # Disable all neuromodulation
        self.brain.disableCuriosity()
        self.brain.disableNovelty()
        self.brain.disableDevelopment()
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
        
        # Collect data
        self.experiment_data['control'] = {
            'steps': steps,
            'final_activity': self.brain.getAverageActivity(),
            'final_curiosity': self.brain.getCuriosityLevel(),
            'final_novelty': self.brain.getNoveltyLevel(),
            'final_development_stage': self.brain.getDevelopmentalStage().name
        }
        
        return self.experiment_data['control']
    
    def run_novuality_experiment(self, steps=100):
        """Run novelty detection experiment."""
        print("Running novelty detection experiment...")
        
        # Enable only novelty detection
        self.brain.disableCuriosity()
        self.brain.enableNovelty()
        self.brain.disableDevelopment()
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
        
        # Collect data
        self.experiment_data['novelty_only'] = {
            'steps': steps,
            'final_activity': self.brain.getAverageActivity(),
            'final_curiosity': self.brain.getCuriosityLevel(),
            'final_novelty': self.brain.getNoveltyLevel(),
            'final_development_stage': self.brain.getDevelopmentalStage().name
        }
        
        return self.experiment_data['novelty_only']
    
    def run_curiosity_experiment(self, steps=100):
        """Run curiosity experiment."""
        print("Running curiosity experiment...")
        
        # Enable only curiosity
        self.brain.enableCuriosity()
        self.brain.disableNovelty()
        self.brain.disableDevelopment()
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
        
        # Collect data
        self.experiment_data['curiosity_only'] = {
            'steps': steps,
            'final_activity': self.brain.getAverageActivity(),
            'final_curiosity': self.brain.getCuriosityLevel(),
            'final_novelty': self.brain.getNoveltyLevel(),
            'final_development_stage': self.brain.getDevelopmentalStage().name
        }
        
        return self.experiment_data['curiosity_only']
    
    def run_development_experiment(self, steps=100):
        """Run development experiment."""
        print("Running development experiment...")
        
        # Enable only development
        self.brain.disableCuriosity()
        self.brain.disableNovelty()
        self.brain.enableDevelopment()
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
        
        # Collect data
        self.experiment_data['development_only'] = {
            'steps': steps,
            'final_activity': self.brain.getAverageActivity(),
            'final_curiosity': self.brain.getCuriosityLevel(),
            'final_novelty': self.brain.getNoveltyLevel(),
            'final_development_stage': self.brain.getDevelopmentalStage().name
        }
        
        return self.experiment_data['development_only']
    
    def run_full_integration_experiment(self, steps=100):
        """Run full integration experiment."""
        print("Running full integration experiment...")
        
        # Enable all systems
        self.brain.enableCuriosity()
        self.brain.enableNovelty()
        self.brain.enableDevelopment()
        
        # Run simulation
        for step in range(steps):
            self.brain.step(step)
        
        # Collect data
        self.experiment_data['full_integration'] = {
            'steps': steps,
            'final_activity': self.brain.getAverageActivity(),
            'final_curiosity': self.brain.getCuriosityLevel(),
            'final_novelty': self.brain.getNoveltyLevel(),
            'final_development_stage': self.brain.getDevelopmentalStage().name
        }
        
        return self.experiment_data['full_integration']
    
    def run_comparative_analysis(self):
        """Run comparative analysis of all experiments."""
        print("Running comparative analysis...")
        
        # Run all experiments
        self.run_control_experiment()
        self.run_novuality_experiment()
        self.run_curiosity_experiment()
        self.run_development_experiment()
        self.run_full_integration_experiment()
        
        # Analyze results
        analysis = self._analyze_experiments()
        
        return analysis
    
    def _analyze_experiments(self):
        """Analyze experiment results."""
        data = self.experiment_data
        
        analysis = {
            'control_vs_novelty': {
                'activity_difference': data['novelty_only']['final_activity'] - data['control']['final_activity'],
                'curiosity_difference': data['novelty_only']['final_curiosity'] - data['control']['final_curiosity'],
                'novelty_difference': data['novelty_only']['final_novelty'] - data['control']['final_novelty']
            },
            'control_vs_curiosity': {
                'activity_difference': data['curiosity_only']['final_activity'] - data['control']['final_activity'],
                'curiosity_difference': data['curiosity_only']['final_curiosity'] - data['control']['final_curiosity'],
                'novelty_difference': data['curiosity_only']['final_novelty'] - data['control']['final_novelty']
            },
            'control_vs_development': {
                'activity_difference': data['development_only']['final_activity'] - data['control']['final_activity'],
                'curiosity_difference': data['development_only']['final_curiosity'] - data['control']['final_curiosity'],
                'novelty_difference': data['development_only']['final_novelty'] - data['control']['final_novelty']
            },
            'full_integration_vs_control': {
                'activity_difference': data['full_integration']['final_activity'] - data['control']['final_activity'],
                'curiosity_difference': data['full_integration']['final_curiosity'] - data['control']['final_curiosity'],
                'novelty_difference': data['full_integration']['final_novelty'] - data['control']['final_novelty']
            }
        }
        
        # Statistical significance (placeholder)
        for key, comparison in analysis.items():
            comparison['statistically_significant'] = False  # Placeholder
        
        return analysis
    
    def plot_experiment_results(self):
        """Plot experiment results."""
        data = self.experiment_data
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Activity comparison
        activities = [
            data['control']['final_activity'],
            data['novelty_only']['final_activity'],
            data['curiosity_only']['final_activity'],
            data['development_only']['final_activity'],
            data['full_integration']['final_activity']
        ]
        labels = ['Control', 'Novelty Only', 'Curiosity Only', 'Development Only', 'Full Integration']
        
        axes[0, 0].bar(labels, activities)
        axes[0, 0].set_title('Final Activity Level')
        axes[0, 0].set_ylabel('Activity')
        axes[0, 0].tick_params(axis='x', rotation=45)
        
        # Curiosity comparison
        curiosities = [
            data['control']['final_curiosity'],
            data['novelty_only']['final_curiosity'],
            data['curiosity_only']['final_curiosity'],
            data['development_only']['final_curiosity'],
            data['full_integration']['final_curiosity']
        ]
        
        axes[0, 1].bar(labels, curiosities)
        axes[0, 1].set_title('Final Curiosity Level')
        axes[0, 1].set_ylabel('Curiosity')
        axes[0, 1].tick_params(axis='x', rotation=45)
        
        # Novelty comparison
        novelties = [
            data['control']['final_novelty'],
            data['novelty_only']['final_novelty'],
            data['curiosity_only']['final_novelty'],
            data['development_only']['final_novelty'],
            data['full_integration']['final_novelty']
        ]
        
        axes[1, 0].bar(labels, novelties)
        axes[1, 0].set_title('Final Novelty Level')
        axes[1, 0].set_ylabel('Novelty')
        axes[1, 0].tick_params(axis='x', rotation=45)
        
        # Development stage comparison
        stages = [
            data['control']['final_development_stage'],
            data['novelty_only']['final_development_stage'],
            data['curiosity_only']['final_development_stage'],
            data['development_only']['final_development_stage'],
            data['full_integration']['final_development_stage']
        ]
        
        unique_stages = list(set(stages))
        stage_counts = [stages.count(stage) for stage in unique_stages]
        
        axes[1, 1].bar(unique_stages, stage_counts)
        axes[1, 1].set_title('Development Stage Distribution')
        axes[1, 1].set_ylabel('Count')
        
        plt.tight_layout()
        plt.show()
        
        return fig

# Use NLM research toolkit
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

researcher = NLMResearchToolkit(brain)

# Run comparative analysis
results = researcher.run_comparative_analysis()

print("\nComparative Analysis Results:")
for key, comparison in results.items():
    print(f"\n{key}:")
    for metric, value in comparison.items():
        if metric != 'statistically_significant':
            print(f"  {metric}: {value:.4f}")

# Plot results
researcher.plot_experiment_results()
```

---

## API Reference

### Brain Class

#### Constructor
```python
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Core Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain | None | None |
| `step(step)` | Run simulation step | `step: int` | None |
| `reset()` | Reset brain state | None | None |
| `save(filename)` | Save brain state | `filename: str` | None |
| `load(filename)` | Load brain state | `filename: str` | None |
| `receiveSensoryInput(data)` | Inject sensory data | `data: dict` | None |
| `produceAction()` | Get motor action | None | dict |

#### Getter Methods

| Method | Description |
|--------|-------------|
| `getTotalNeuronCount()` | Get total neuron count |
| `getTotalSynapseCount()` | Get total synapse count |
| `getFiringNeuronCount()` | Get currently firing neurons |
| `getAverageFiringRate()` | Get average firing rate |
| `getExcitationInhibitionRatio()` | Get E/I balance |
| `getMemoryUsage()` | Get memory usage |
| `getAverageActivity()` | Get average neural activity |
| `getCurrentStep()` | Get current simulation step |
| `getSimulationTime()` | Get current simulation time |
| `getDevelopmentalStage()` | Get current developmental stage |
| `getDevelopmentProgress()` | Get development progress (0-1) |
| `getCuriosityLevel()` | Get curiosity level |
| `getNoveltyLevel()` | Get novelty level |
| `getPredictionError()` | Get prediction error |
| `isInCriticalPeriod()` | Check if in critical period |
| `getPlasticityRate()` | Get current plasticity rate |

#### Subsystems

| Method | Description |
|--------|-------------|
| `getWorkingMemory()` | Get working memory |
| `getEpisodicMemory()` | Get episodic memory |
| `getAssociativeMemory()` | Get associative memory |
| `getNeuromodulators()` | Get neuromodulator levels |
| `getPredictionSystem()` | Get prediction system |
| `getDevelopmentSystem()` | Get development system |
| `getNeuralPlanner()` | Get neural planner |
| `getConceptFormation()` | Get concept formation |
| `getAttentionSystem()` | Get attention system |

### Config Class

#### Constructor
```python
config = pynlm.createDefaultConfig()
config = pynlm.createConfig()
config = pynlm.createSmallConfig()
config = pynlm.createStandardConfig()
config = pynlm.createLargeConfig()
```

#### Core Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `set(key, value)` | Set configuration value | `key: str`, `value: any` | None |
| `getValue(key)` | Get configuration value | `key: str` | any |
| `has(key)` | Check if key exists | `key: str` | bool |
| `getKeys()` | Get all keys | None | list |
| `loadFromFile(filename)` | Load from file | `filename: str` | None |
| `saveToFile(filename)` | Save to file | `filename: str` | None |
| `exportToDict()` | Export to dictionary | None | dict |
| `importFromDict(d)` | Import from dictionary | `d: dict` | None |
| `isValid()` | Validate configuration | None | bool |

### AgentBrain Class

#### Constructor
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Core Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize agent | `world: SimpleWorld` | None |
| `processSensoryInput(percept)` | Process sensory input | `percept: dict` | None |
| `decodeMotorCommand()` | Decode motor command | None | dict |
| `applyRewardModulation(reward, predicted)` | Apply reward | `reward: float`, `predicted: float` | None |
| `updateDevelopment(timestep)` | Update development | `timestep: float` | None |
| `enableRewardModulation(enable)` | Enable/disable reward | `enable: bool` | None |
| `enableCuriosity(enable)` | Enable/disable curiosity | `enable: bool` | None |
| `enableNovelty(enable)` | Enable/disable novelty | `enable: bool` | None |
| `enableStructuralPlasticity(enable)` | Enable/disable structural plasticity | `enable: bool` | None |
| `enableDevelopment(enable)` | Enable/disable development | `enable: bool` | None |
| `enablePredictionSystem(enable)` | Enable/disable prediction system | `enable: bool` | None |
| `enableConceptFormation(enable)` | Enable/disable concept formation | `enable: bool` | None |
| `enableAttentionSystem(enable)` | Enable/disable attention system | `enable: bool` | None |
| `enableNeuralPlanning(enable)` | Enable/disable neural planning | `enable: bool` | None |

#### Getter Methods

| Method | Description |
|--------|-------------|
| `getCuriosityLevel()` | Get curiosity level |
| `getNoveltyLevel()` | Get novelty level |
| `getPredictionError()` | Get prediction error |
| `getNeuromodulationLevel()` | Get neuromodulation level |
| `getMemoryUsage()` | Get memory usage |
| `getActiveSubsystems()` | Get active subsystems |
| `getDevelopmentalStage()` | Get developmental stage |

### SimpleWorld Class

#### Constructor
```python
world = pynlm.createSimpleWorld()
world = pynlm.createOpenWorld()
world = pynlm.createGridWorld(width, height)
```

#### Core Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, visionWidth, visionHeight)` | Configure world | `width: int`, `height: int`, `visionWidth: int`, `visionHeight: int` | None |
| `reset()` | Reset world | None | None |
| `update(timestep)` | Update physics | `timestep: float` | None |
| `step(action, timestep)` | Take action | `action: dict`, `timestep: float` | None |
| `setAgentStart(x, y)` | Set agent start position | `x: float`, `y: float` | None |
| `applyMotorCommand(action, time)` | Apply motor command | `action: dict`, `time: float` | None |
| `getSensoryPercept()` | Get sensory percept | None | dict |
| `getAgentBody()` | Get agent body | None | dict |
| `getAgentX()`, `getAgentY()` | Get agent position | None | float |
| `getWidth()`, `getHeight()` | Get world dimensions | None | int |
| `getSimulationTime()` | Get simulation time | None | float |
| `getMaxEnergy()` | Get maximum energy | None | float |
| `setMaxEnergy(value)` | Set maximum energy | `value: float` | None |
| `addObject(object)` | Add object | `object: WorldObject` | None |
| `removeObject(x, y)` | Remove object | `x: float`, `y: float` | None |
| `getObject(x, y)` | Get object | `x: float`, `y: float` | WorldObject |
| `isValidPosition(x, y)` | Check position validity | `x: float`, `y: float` | bool |

### Enumerations

#### NeuronType
- `pynlm.NeuronType.Excitatory`
- `pynlm.NeuronType.Inhibitory`
- `pynlm.NeuronType.Sensory`
- `pynlm.NeuronType.Motor`
- `pynlm.NeuronType.Modulatory`
- `pynlm.NeuronType.Internal`

#### DevelopmentalStage
- `pynlm.DevelopmentalStage.Initial`
- `pynlm.DevelopmentalStage.CriticalPeriod`
- `pynlm.DevelopmentalStage.Maturation`
- `pynlm.DevelopmentalStage.Adult`
- `pynlm.DevelopmentalStage.Aging`

#### ActionType
- `pynlm.ActionType.MoveForward`
- `pynlm.ActionType.TurnLeft`
- `pynlm.ActionType.TurnRight`
- `pynlm.ActionType.MoveBackward`
- `pynlm.ActionType.Interact`
- `pynlm.ActionType.Eat`
- `pynlm.ActionType.Sleep`
- `pynlm.ActionType.Grab`

#### WorldObjectType
- `pynlm.WorldObjectType.Empty`
- `pynlm.WorldObjectType.Resource`
- `pynlm.WorldObjectType.Hazard`
- `pynlm.WorldObjectType.Wall`
- `pynlm.WorldObjectType.Marker`

---

## Index

This document provides comprehensive guidance for using the NLM system. For additional information, refer to:

- [QUICK_START.md](#quick-start) - Quick start guide
- [BUILDING.md](#building) - Build instructions
- [PYTHON_BINDING_GUIDE.md](#python-binding-guide) - Python API guide
- [PHASE6_INTEGRATION_EXAMPLES.md](#phase6-integration-examples) - Phase 6 integration examples
- [ADVANCED_TOPICS.md](#advanced-topics) - Advanced topics
- [USER_GUIDE.md](#user-guide) - This user guide

---

*Last updated: $(date +%Y-%m-%d)*