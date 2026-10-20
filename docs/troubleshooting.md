# NLM Troubleshooting Guide

Comprehensive troubleshooting guide for NLM (Neural Learning Machine) system. This documentation covers common issues, their causes, and solutions to help users quickly resolve problems and optimize their NLM simulations.

## Table of Contents

1. [Troubleshooting Overview](#troubleshooting-overview)
2. [Common Error Categories](#common-error-categories)
3. [Installation and Setup Issues](#installation-and-setup-issues)
4. [Configuration Issues](#configuration-issues)
5. [Runtime Issues](#runtime-issues)
6. [Performance Issues](#performance-issues)
7. [Debugging Techniques](#debugging-techniques)
8. [Error Message Explanations](#error-message-explanations)
9. [Common Problem Solutions](#common-problem-solutions)
10. [Advanced Troubleshooting](#advanced-troubleshooting)
11. [Community Support](#community-support)

## Troubleshooting Overview

NLM troubleshooting involves identifying and resolving issues that occur during installation, configuration, simulation execution, and performance optimization. This guide provides systematic approaches to:

- **Problem Identification**: Recognize and categorize common issues
- **Root Cause Analysis**: Understand the underlying causes of problems
- **Solution Implementation**: Apply appropriate fixes and workarounds
- **Prevention**: Implement measures to avoid recurring issues
- **Optimization**: Improve system performance and reliability

### Troubleshooting Process

1. **Problem Identification**: Recognize symptoms and error messages
2. **Information Gathering**: Collect relevant information about the issue
3. **Hypothesis Formation**: Form hypotheses about potential causes
4. **Testing**: Test hypotheses systematically
5. **Root Cause Analysis**: Identify the underlying cause
6. **Solution Implementation**: Apply fixes and workarounds
7. **Verification**: Verify the solution resolves the issue
8. **Documentation**: Document the issue and resolution

## Common Error Categories

### Installation and Setup Errors

#### Module Not Found Errors
```
ImportError: No module named 'pynlm'
ModuleNotFoundError: No module named 'nlm'
```

**Causes:**
- NLM not properly installed
- Python path not configured correctly
- Dependencies missing
- Incorrect Python environment

**Solutions:**
```bash
# Install NLM in development mode
pip install -e .

# Or install from source
pip install .

# Check installation
python -c "import pynlm; print(pynlm.__version__)"
```

#### Build Failures
```
Build error: CMake Error at CMakeLists.txt: Cannot find Python development headers
error: command 'gcc' failed with exit status 1
```

**Causes:**
- Missing build dependencies
- Compiler not installed
- CMake version too old
- Insufficient system resources

**Solutions:**
```bash
# Install build dependencies
pip install pybind11 scikit-build-core numpy

# Install build tools
sudo apt-get update && sudo apt-get install build-essential cmake

# Check CMake version
cmake --version

# Build with verbose output
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DPYTHON_EXECUTABLE=$(which python)
```

### Configuration Errors

#### Invalid Configuration Parameters
```
ConfigurationError: Invalid parameter 'brain.invalid_parameter'
ValueError: Invalid value for parameter 'plasticity.stdp.learning_rate'
```

**Causes:**
- Typographical errors in parameter names
- Invalid parameter values
- Missing required parameters
- Parameter type mismatches

**Solutions:**
```python
import pynlm as nlm

# Create configuration
config = nlm.createDefaultConfig()

# Fix invalid parameters
config.set("brain.neuron_count", 1000)  # Correct parameter name
config.set("plasticity.stdp.learning_rate", 0.001)  # Valid value

# Add missing parameters
config.set("brain.synapse_density", 0.1)
config.set("neuromod.curiosity.exploration_weight", 0.5)

# Validate configuration
if not nlm.validateConfig(config):
    errors = nlm.getConfigValidationError(config)
    print(f"Configuration errors: {errors}")
```

#### Configuration File Errors
```
FileNotFoundError: Configuration file not found: config.json
JSONDecodeError: Invalid JSON in configuration file
```

**Causes:**
- Configuration file missing
- File path incorrect
- Invalid JSON syntax
- Corrupted configuration file

**Solutions:**
```python
# Save configuration to file
config.saveToFile("config.json")

# Load configuration from file
new_config = nlm.createDefaultConfig()
new_config.loadFromFile("config.json")

# Check if file exists
import os
if os.path.exists("config.json"):
    print("Configuration file exists")
else:
    print("Configuration file not found")
```

### Runtime Errors

#### Initialization Errors
```
RuntimeError: Brain initialization failed
InitializationError: Invalid brain configuration
```

**Causes:**
- Invalid brain configuration
- Insufficient memory
- Corrupted internal state
- Missing required components

**Solutions:**
```python
import pynlm as nlm

# Create valid configuration
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Initialize brain with error handling
try:
    brain = nlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
except nlm.InitializationError as e:
    print(f"Brain initialization failed: {e}")
    # Try with different configuration
    config.set("brain.neuron_count", 500)
    brain = nlm.createBrain(config)
    brain.initialize()
```

#### Simulation Errors
```
RuntimeError: Simulation step failed
OperationError: Invalid action command
```

**Causes:**
- Invalid neural state
- Memory corruption
- Invalid action parameters
- Simulation configuration errors

**Solutions:**
```python
import pynlm as nlm

# Create brain and world
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with error handling
try:
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Step {step}: Success")
    
    print("Simulation completed successfully")
except nlm.OperationError as e:
    print(f"Simulation error: {e}")
    # Debug the error
    print(f"Brain firing rate: {brain.getAverageFiringRate()}")
    print(f"Agent curiosity: {agent.getCuriosityLevel()}")
```

#### Memory Errors
```
MemoryError: Ran out of memory
RuntimeError: Memory allocation failed
```

**Causes:**
- Insufficient system memory
- Memory fragmentation
- Memory allocation errors
- Large network configuration

**Solutions:**
```python
import pynlm as nlm
import psutil
import os

# Check available memory
process = psutil.Process(os.getpid())
available_memory = process.memory_info().available / 1024 / 1024
print(f"Available memory: {available_memory:.1f} MB")

# Optimize memory usage
config = nlm.createDefaultConfig()

# Reduce network size
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)

# Optimize configuration
config.set("resources.max_memory", 512.0)
config.set("plasticity.structural.enable", False)
config.set("neuromod.curiosity.enable", False)

# Try with optimized configuration
try:
    brain = nlm.createBrain(config)
    brain.initialize()
    print("Brain initialized with optimized memory usage")
except Exception as e:
    print(f"Failed to initialize brain: {e}")
```

### Performance Issues

#### Low Performance
```
PerformanceWarning: Low simulation performance
RuntimeError: Simulation too slow
```

**Causes:**
- Inefficient configuration
- Large network size
- Memory limitations
- CPU bottlenecks

**Solutions:**
```python
import pynlm as nlm

# Create performance-optimized configuration
config = nlm.createDefaultConfig()

# Optimize for speed
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.02)
config.set("plasticity.stdp.learning_rate", 0.0005)
config.set("simulation.timestep", 0.2)
config.set("resources.max_memory", 256.0)

# Initialize brain
brain = nlm.createBrain(config)
brain.initialize()

# Run performance benchmark
import time
start_time = time.time()
for step in range(100):
    brain.step(step)
end_time = time.time()

steps_per_second = 100 / (end_time - start_time)
print(f"Performance: {steps_per_second:.1f} steps/second")

if steps_per_second < 100:
    print("Performance too low - consider optimization")
```

#### Memory Exhaustion
```
MemoryError: Out of memory during simulation
RuntimeError: Memory allocation failed
```

**Causes:**
- Large network configuration
- Memory fragmentation
- Memory leaks
- Insufficient system memory

**Solutions:**
```python
import pynlm as nlm
import psutil
import os

# Check system memory
process = psutil.Process(os.getpid())
total_memory = process.memory_info().total / 1024 / 1024
available_memory = process.memory_info().available / 1024 / 1024

print(f"Total memory: {total_memory:.1f} MB")
print(f"Available memory: {available_memory:.1f} MB")

# Create memory-efficient configuration
config = nlm.createDefaultConfig()

# Reduce memory usage
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.01)
config.set("plasticity.structural.enable", False)
config.set("neuromod.curiosity.enable", False)
config.set("neuromod.novelty.enable", False)
config.set("resources.max_memory", 128.0)

# Monitor memory usage during simulation
try:
    brain = nlm.createBrain(config)
    brain.initialize()
    
    for step in range(50):
        brain.step(step)
        
        # Check memory usage
        current_memory = process.memory_info().rss / 1024 / 1024
        print(f"Step {step}: Memory usage: {current_memory:.1f} MB")
        
        if current_memory > 100.0:
            print("Warning: High memory usage")
except Exception as e:
    print(f"Simulation failed: {e}")
```

## Debugging Techniques

### Systematic Debugging Approach

#### 1. Information Collection
```python
def collect_debug_information(brain, world, agent):
    """Collect comprehensive debug information."""
    
    debug_info = {
        "timestamp": time.time(),
        "brain_state": {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "plasticities": {
                "stdp": brain.getSTDP() is not None,
                "hebbian": brain.getHebbian() is not None,
                "structural": brain.getStructuralPlasticity() is not None
            }
        },
        "agent_state": {
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "development_stage": str(agent.getDevelopmentalStage()),
            "neuromodulation_level": agent.getNeuromodulationLevel()
        },
        "world_state": {
            "position": (world.getAgentBody().x, world.getAgentBody().y),
            "vision": (world.getVisionWidth(), world.getVisionHeight()),
            "simulation_time": world.getSimulationTime()
        },
        "system_resources": {
            "cpu_percent": psutil.cpu_percent(),
            "memory_percent": psutil.virtual_memory().percent,
            "memory_available_mb": psutil.virtual_memory().available / 1024 / 1024
        }
    }
    
    return debug_info
```

#### 2. Error Logging
```python
class DebugLogger:
    def __init__(self, log_file="debug.log"):
        self.log_file = log_file
        self.log_entries = []
    
    def log(self, message, level="INFO"):
        """Log debug message."""
        import time
        entry = {
            "timestamp": time.time(),
            "level": level,
            "message": message
        }
        
        self.log_entries.append(entry)
        
        # Also write to file
        with open(self.log_file, 'a') as f:
            f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} [{level}] {message}\n")
    
    def log_error(self, error, context=None):
        """Log error with context."""
        message = f"ERROR: {error}"
        if context:
            message += f" Context: {context}"
        
        self.log(message, "ERROR")
    
    def log_warning(self, message):
        """Log warning."""
        self.log(message, "WARNING")
    
    def log_debug(self, message):
        """Log debug information."""
        self.log(message, "DEBUG")
    
    def get_logs(self, level=None, since=None):
        """Get logs filtered by level and time."""
        logs = self.log_entries
        
        if level:
            logs = [l for l in logs if l["level"] == level]
        
        if since:
            logs = [l for l in logs if l["timestamp"] >= since]
        
        return logs
    
    def save_logs(self, filepath=None):
        """Save logs to file."""
        if filepath:
            self.log_file = filepath
        
        with open(self.log_file, 'w') as f:
            for entry in self.log_entries:
                f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(entry['timestamp']))} [{entry['level']}] {entry['message']}\n")
```

#### 3. State Inspection
```python
class StateInspector:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
    
    def inspect_brain_state(self):
        """Inspect brain state for debugging."""
        print("=== Brain State ===")
        print(f"Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"Development stage: {self.brain.getDevelopmentalStage()}")
        
        # Check plasticities
        stdp = self.brain.getSTDP()
        hebbian = self.brain.getHebbian()
        structural = self.brain.getStructuralPlasticity()
        
        print(f"\nPlasticity systems:")
        print(f"  STDP: {'Enabled' if stdp else 'Disabled'}")
        print(f"  Hebbian: {'Enabled' if hebbian else 'Disabled'}")
        print(f"  Structural: {'Enabled' if structural else 'Disabled'}")
        
        if stdp:
            print(f"  STDP learning rate: {stdp.getLearningRate():.6f}")
        
        if hebbian:
            print(f"  Hebbian learning rate: {hebbian.getLearningRate():.6f}")
    
    def inspect_agent_state(self):
        """Inspect agent state for debugging."""
        print("\n=== Agent State ===")
        print(f"Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"Novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"Prediction error: {self.agent.getPredictionError():.3f}")
        print(f"Neuromodulation level: {self.agent.getNeuromodulationLevel():.3f}")
        print(f"Development stage: {self.agent.getDevelopmentalStage()}")
    
    def inspect_world_state(self):
        """Inspect world state for debugging."""
        print("\n=== World State ===")
        body = self.world.getAgentBody()
        print(f"Agent position: ({body.x:.1f}, {body.y:.1f})")
        print(f"Vision range: {self.world.getVisionWidth()}x{self.world.getVisionHeight()}")
        print(f"Simulation time: {self.world.getSimulationTime():.3f}")
        print(f"World dimensions: {self.world.getWidth()}x{self.world.getHeight()}")
    
    def perform_system_check(self):
        """Perform comprehensive system check."""
        print("=== System Check ===")
        
        # Check brain health
        self.inspect_brain_state()
        
        # Check agent health
        self.inspect_agent_state()
        
        # Check world health
        self.inspect_world_state()
        
        # Check for inconsistencies
        self.check_consistency()
    
    def check_consistency(self):
        """Check for consistency issues."""
        print("\n=== Consistency Checks ===")
        
        # Check firing rate vs spike count
        firing_rate = self.brain.getAverageFiringRate()
        spike_count = self.brain.getTotalSpikeCount()
        expected_spikes = firing_rate * 100  # Approximate
        
        if spike_count > 0 and abs(firing_rate - expected_spikes / 100) > 0.1:
            print(f"⚠ Inconsistent firing rate: {firing_rate:.3f}, expected ~{expected_spikes/100:.3f}")
        
        # Check developmental stage consistency
        brain_stage = self.brain.getDevelopmentalStage()
        agent_stage = self.agent.getDevelopmentalStage()
        
        if str(brain_stage) != str(agent_stage):
            print(f"⚠ Stage mismatch: brain={brain_stage}, agent={agent_stage}")
        
        # Check memory usage
        import psutil
        process = psutil.Process()
        memory_percent = process.memory_percent()
        
        if memory_percent > 90.0:
            print(f"⚠ High memory usage: {memory_percent:.1f}%")
        
        print("Consistency checks completed")
```

#### 4. Step-by-Step Debugging
```python
class StepByStepDebugger:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.debug_log = []
    
    def debug_step(self, step):
        """Debug a single simulation step."""
        print(f"\n=== Debug Step {step} ===")
        
        # Log state before step
        before_state = {
            "brain_firing": self.brain.getFiringNeuronCount(),
            "brain_firing_rate": self.brain.getAverageFiringRate(),
            "agent_curiosity": self.agent.getCuriosityLevel(),
            "agent_novelty": self.agent.getNoveltyLevel(),
            "world_position": (self.world.getAgentBody().x, self.world.getAgentBody().y),
            "world_time": self.world.getSimulationTime()
        }
        
        print("State before step:")
        for key, value in before_state.items():
            print(f"  {key}: {value}")
        
        try:
            # Execute step
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Log state after step
            after_state = {
                "brain_firing": self.brain.getFiringNeuronCount(),
                "brain_firing_rate": self.brain.getAverageFiringRate(),
                "agent_curiosity": self.agent.getCuriosityLevel(),
                "agent_novelty": self.agent.getNoveltyLevel(),
                "world_position": (self.world.getAgentBody().x, self.world.getAgentBody().y),
                "world_time": self.world.getSimulationTime()
            }
            
            print("State after step:")
            for key, value in after_state.items():
                print(f"  {key}: {value}")
            
            # Check for anomalies
            self.check_for_anomalies(before_state, after_state, step)
            
            print("✓ Step completed successfully")
            
        except Exception as e:
            print(f"✗ Step failed: {e}")
            self.debug_error(e, step)
    
    def check_for_anomalies(self, before_state, after_state, step):
        """Check for anomalies in step execution."""
        anomalies = []
        
        # Check for unrealistic firing rate changes
        firing_rate_change = after_state["brain_firing_rate"] - before_state["brain_firing_rate"]
        if abs(firing_rate_change) > 0.5:
            anomalies.append(f"Large firing rate change: {firing_rate_change:.3f}")
        
        # Check for position changes
        pos_change = ((after_state["world_position"][0] - before_state["world_position"][0]) ** 2 +
                      (after_state["world_position"][1] - before_state["world_position"][1]) ** 2) ** 0.5
        if pos_change > 10.0:
            anomalies.append(f"Large position change: {pos_change:.1f}")
        
        # Check for curiosity anomalies
        curiosity_change = after_state["agent_curiosity"] - before_state["agent_curiosity"]
        if abs(curiosity_change) > 0.3:
            anomalies.append(f"Large curiosity change: {curiosity_change:.3f}")
        
        if anomalies:
            print(f"⚠ Anomalies detected: {', '.join(anomalies)}")
    
    def debug_error(self, error, step):
        """Debug specific error."""
        print(f"Error at step {step}:")
        print(f"  Error type: {type(error).__name__}")
        print(f"  Error message: {error}")
        
        # Check specific error types
        if "memory" in str(error).lower():
            print("  → Memory management issue")
            print("  Suggestions: Check system memory, reduce network size")
        
        elif "initialization" in str(error).lower():
            print("  → Initialization issue")
            print("  Suggestions: Check configuration, verify brain setup")
        
        elif "step" in str(error).lower():
            print("  → Simulation step issue")
            print("  Suggestions: Check neural state, verify connections")
        
        elif "brain" in str(error).lower():
            print("  → Brain-specific issue")
            print("  Suggestions: Check brain configuration, verify neuron counts")
        
        else:
            print("  → Unknown error type")
            print("  Suggestions: Check logs for more details")
```

## Error Message Explanations

### Common Error Messages

#### "No module named 'pynlm'"
**Meaning:** NLM Python bindings not found
**Solution:** Install NLM correctly
```bash
pip install -e .
```

#### "CMakeError: Cannot find Python development headers"
**Meaning:** Python development libraries missing
**Solution:** Install Python development packages
```bash
sudo apt-get install python3-dev python3-pip
pip install pybind11 scikit-build-core
```

#### "Brain initialization failed"
**Meaning:** Brain could not be initialized
**Solution:** Check configuration
```python
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

brain = nlm.createBrain(config)
brain.initialize()  # Might fail if parameters invalid
```

#### "Memory allocation failed"
**Meaning:** Insufficient memory for NLM operations
**Solution:** Optimize memory usage
```python
# Reduce network size
config.set("brain.neuron_count", 500)

# Disable memory-intensive features
config.set("plasticity.structural.enable", False)
```

#### "Simulation too slow"
**Meaning:** Performance below acceptable levels
**Solution:** Optimize performance
```python
# Use performance-optimized configuration
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.02)
config.set("simulation.timestep", 0.2)
```

## Common Problem Solutions

### Problem 1: Brain Won't Initialize

**Symptoms:**
- Brain initialization fails
- Error messages about invalid configuration
- Empty brain state

**Solution:**
```python
# Create minimal valid configuration
config = nlm.createDefaultConfig()

# Set basic parameters
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)

# Enable basic systems
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)

# Initialize brain with error handling
try:
    brain = nlm.createBrain(config)
    brain.initialize()
    print("✓ Brain initialized successfully")
except Exception as e:
    print(f"✗ Brain initialization failed: {e}")
    print("Try simpler configuration...")
    
    # Minimal configuration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    print("✓ Brain initialized with minimal configuration")
```

### Problem 2: Simulation Doesn't Progress

**Symptoms:**
- Simulation runs without visible changes
- No spikes or activity
- Agent doesn't move or act
- No learning observed

**Solution:**
```python
# Debug simulation state
import pynlm as nlm

# Create configuration with basic learning
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.curiosity.exploration_weight", 0.5)

# Create brain and world
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with debug output
for step in range(20):
    print(f"\nStep {step}:")
    
    # Check initial state
    print(f"  Brain firing: {brain.getFiringNeuronCount()}")
    print(f"  Agent curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # Execute step
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check results
    print(f"  Brain firing after: {brain.getFiringNeuronCount()}")
    print(f"  Agent curiosity after: {agent.getCuriosityLevel():.3f}")
    
    # Stop if agent starts moving
    if world.getAgentBody().x != world.getAgentBody().x or world.getAgentBody().y != world.getAgentBody().y:
        print("  Agent is moving - simulation progressing")
        break
```

### Problem 3: Performance Issues

**Symptoms:**
- Simulation too slow
- High memory usage
- Frequent errors or crashes
- Inconsistent behavior

**Solution:**
```python
import pynlm as nlm
import psutil
import time

# Monitor system resources
process = psutil.Process()

def get_system_status():
    return {
        "cpu_percent": process.cpu_percent(),
        "memory_percent": process.virtual_memory().percent,
        "memory_available_mb": process.virtual_memory().available / 1024 / 1024
    }

# Create performance-optimized configuration
config = nlm.createDefaultConfig()

# Optimize for speed over accuracy
config.set("brain.neuron_count", 300)
config.set("brain.synapse_density", 0.01)
config.set("plasticity.stdp.learning_rate", 0.0005)
config.set("neuromod.curiosity.enable", False)
config.set("neuromod.novelty.enable", False)
config.set("simulation.timestep", 0.5)
config.set("resources.max_memory", 128.0)

# Initialize brain
brain = nlm.createBrain(config)
brain.initialize()

# Run performance test
start_time = time.time()
memory_before = process.virtual_memory().used

for step in range(50):
    brain.step(step)

memory_after = process.virtual_memory().used
end_time = time.time()

# Calculate performance metrics
steps_per_second = 50 / (end_time - start_time)
memory_used_mb = (memory_after - memory_before) / 1024 / 1024

print(f"Performance Test Results:")
print(f"  Steps per second: {steps_per_second:.1f}")
print(f"  Memory used: {memory_used_mb:.1f} MB")
print(f"  Average step time: {(end_time - start_time) * 1000 / 50:.1f} ms")

# Check if performance is acceptable
if steps_per_second < 10:
    print("⚠ Performance too low - consider further optimization")
elif memory_used_mb > 50:
    print("⚠ Memory usage high - consider reducing network size")
else:
    print("✓ Performance acceptable")
```

### Problem 4: Memory Issues

**Symptoms:**
- Memory usage increases during simulation
- System becomes slow or unresponsive
- Memory errors or crashes
- "Out of memory" errors

**Solution:**
```python
import pynlm as nlm
import psutil
import os

# Check available memory
process = psutil.Process(os.getpid())
total_memory = process.virtual_memory().total
available_memory = process.virtual_memory().available

print(f"Total system memory: {total_memory / 1024 / 1024:.1f} MB")
print(f"Available system memory: {available_memory / 1024 / 1024:.1f} MB")

# Create memory-efficient configuration
config = nlm.createDefaultConfig()

# Reduce memory usage
config.set("brain.neuron_count", 200)
config.set("brain.synapse_density", 0.005)
config.set("plasticity.structural.enable", False)
config.set("neuromod.curiosity.enable", False)
config.set("neuromod.novelty.enable", False)
config.set("neuromod.dopamine.enable", False)
config.set("resources.max_memory", 64.0)

# Monitor memory usage during simulation
brain = nlm.createBrain(config)
brain.initialize()

print("\nMemory usage during simulation:")
for step in range(20):
    brain.step(step)
    
    current_memory = process.virtual_memory().used / 1024 / 1024
    print(f"  Step {step}: {current_memory:.1f} MB")
    
    if current_memory > 50.0:
        print("  ⚠ High memory usage - consider further optimization")
```

### Problem 5: Configuration Errors

**Symptoms:**
- Invalid parameter values
- Missing required parameters
- Parameter type mismatches
- Configuration validation failures

**Solution:**
```python
import pynlm as nlm

# Create configuration with validation
config = nlm.createDefaultConfig()

# Fix common configuration errors
try:
    # Invalid parameter name
    config.set("brain.neuroncount", 1000)  # Wrong parameter name
    config.set("brain.neuron_count", 1000)  # Correct parameter name
    
    # Invalid parameter value
    config.set("brain.synapse_density", 1.5)  # Invalid value (>1.0)
    config.set("brain.synapse_density", 0.1)  # Valid value
    
    # Missing required parameter
    config.set("neuromod.curiosity.exploration_weight", 0.5)  # Add missing parameter
    
    # Validate configuration
    if not nlm.validateConfig(config):
        errors = nlm.getConfigValidationError(config)
        print(f"Configuration errors: {errors}")
    else:
        print("✓ Configuration is valid")
    
    # Try to use configuration
    brain = nlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created and initialized successfully")
    
except Exception as e:
    print(f"✗ Error: {e}")
    print("Try with a simpler configuration...")
    
    # Minimal configuration
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    print("✓ Brain created with minimal configuration")
```

## Advanced Troubleshooting

### System-Level Troubleshooting

#### Debug System Resources
```python
def debug_system_resources():
    """Debug system resource availability."""
    
    import psutil
    import os
    import platform
    
    print("=== System Resource Debug ===")
    
    # System information
    print(f"Platform: {platform.system()} {platform.release()}")
    print(f"Architecture: {platform.machine()}")
    print(f"Python version: {platform.python_version()}")
    
    # Memory information
    memory = psutil.virtual_memory()
    print(f"\nMemory:")
    print(f"  Total: {memory.total / 1024 / 1024:.1f} MB")
    print(f"  Available: {memory.available / 1024 / 1024:.1f} MB")
    print(f"  Used: {memory.used / 1024 / 1024:.1f} MB ({memory.percent}%)")
    
    # CPU information
    cpu_count = psutil.cpu_count()
    cpu_freq = psutil.cpu_freq()
    print(f"\nCPU:")
    print(f"  Cores: {cpu_count}")
    print(f"  Max frequency: {cpu_freq.max:.0f} MHz")
    print(f"  Current frequency: {cpu_freq.current:.0f} MHz")
    
    # Process information
    process = psutil.Process(os.getpid())
    print(f"\nProcess:")
    print(f"  PID: {process.pid}")
    print(f"  Parent PID: {process.ppid()}")
    print(f"  Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")
    
    # Check for memory issues
    if memory.available / 1024 / 1024 < 100:
        print("\n⚠ Low system memory - consider closing other applications")
    
    if process.memory_info().rss / 1024 / 1024 > 500:
        print("⚠ High process memory usage - consider reducing network size")
    
    print("\nSystem resource check completed")
```

#### Check Installation Health
```python
def check_installation_health():
    """Check NLM installation health."""
    
    print("=== Installation Health Check ===")
    
    # Check Python packages
    try:
        import pynlm
        print(f"✓ NLM package: {pynlm.__version__}")
    except ImportError as e:
        print(f"✗ NLM package import failed: {e}")
        return False
    
    # Check build system
    try:
        import cmake
        print(f"✓ CMake available: {cmake.__version__}")
    except ImportError:
        print("⚠ CMake not available - may need to install")
    
    # Check compilation
    try:
        import platform
        print(f"✓ Platform: {platform.system()} {platform.machine()}")
        
        # Try to create a simple brain
        import pynlm as nlm
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        print("✓ Brain creation test passed")
        
    except Exception as e:
        print(f"✗ Brain creation test failed: {e}")
        return False
    
    print("\nInstallation health check completed")
    return True
```

### Performance Analysis

#### Analyze Performance Patterns
```python
class PerformanceAnalyzer:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.performance_data = []
    
    def collect_performance_data(self, duration, sampling_interval=1.0):
        """Collect performance data over time."""
        
        import time
        import psutil
        
        start_time = time.time()
        end_time = start_time + duration
        
        print(f"Collecting performance data for {duration} seconds...")
        
        while time.time() < end_time:
            # Collect performance metrics
            data_point = {
                "timestamp": time.time(),
                "simulation_time": self.world.getSimulationTime(),
                "brain_firing_rate": self.brain.getAverageFiringRate(),
                "agent_curiosity": self.agent.getCuriosityLevel(),
                "cpu_percent": psutil.cpu_percent(),
                "memory_percent": psutil.virtual_memory().percent,
                "memory_available_mb": psutil.virtual_memory().available / 1024 / 1024
            }
            
            self.performance_data.append(data_point)
            
            if len(self.performance_data) % 100 == 0:
                print(f"  Collected {len(self.performance_data)} data points")
            
            time.sleep(sampling_interval)
    
    def analyze_performance(self):
        """Analyze performance data."""
        
        if not self.performance_data:
            print("No performance data to analyze")
            return
        
        print("\n=== Performance Analysis ===")
        
        # Calculate statistics
        simulation_times = [d["simulation_time"] for d in self.performance_data]
        firing_rates = [d["brain_firing_rate"] for d in self.performance_data]
        curiosity_levels = [d["agent_curiosity"] for d in self.performance_data]
        cpu_usage = [d["cpu_percent"] for d in self.performance_data]
        memory_usage = [d["memory_percent"] for d in self.performance_data]
        
        # Calculate performance metrics
        analysis = {
            "simulation": {
                "total_duration": simulation_times[-1] if simulation_times else 0,
                "average_firing_rate": sum(firing_rates) / len(firing_rates) if firing_rates else 0,
                "max_firing_rate": max(firing_rates) if firing_rates else 0,
                "min_firing_rate": min(firing_rates) if firing_rates else 0,
                "firing_rate_std": self.calculate_std(firing_rates) if firing_rates else 0
            },
            
            "curiosity": {
                "average": sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0,
                "max": max(curiosity_levels) if curiosity_levels else 0,
                "min": min(curiosity_levels) if curiosity_levels else 0,
                "trend": self.calculate_trend(curiosity_levels)
            },
            
            "system": {
                "average_cpu": sum(cpu_usage) / len(cpu_usage) if cpu_usage else 0,
                "max_cpu": max(cpu_usage) if cpu_usage else 0,
                "memory_pressure": max(memory_usage) if memory_usage else 0
            }
        }
        
        # Print analysis results
        print(f"Simulation Performance:")
        print(f"  Total duration: {analysis['simulation']['total_duration']:.1f}")
        print(f"  Average firing rate: {analysis['simulation']['average_firing_rate']:.3f}")
        print(f"  Firing rate range: {analysis['simulation']['min_firing_rate']:.3f} - {analysis['simulation']['max_firing_rate']:.3f}")
        
        print(f"\nCuriosity Dynamics:")
        print(f"  Average curiosity: {analysis['curiosity']['average']:.3f}")
        print(f"  Curiosity trend: {analysis['curiosity']['trend']}")
        
        print(f"\nSystem Load:")
        print(f"  Average CPU: {analysis['system']['average_cpu']:.1f}%")
        print(f"  Max CPU: {analysis['system']['max_cpu']:.1f}%")
        print(f"  Memory pressure: {analysis['system']['memory_pressure']:.1f}%")
        
        return analysis
    
    def calculate_std(self, values):
        """Calculate standard deviation."""
        if not values:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return variance ** 0.5
    
    def calculate_trend(self, values):
        """Calculate trend (increasing, decreasing, stable)."""
        if len(values) < 2:
            return "stable"
        
        first_half = values[:len(values)//2]
        second_half = values[len(values)//2:]
        
        first_avg = sum(first_half) / len(first_half)
        second_avg = sum(second_half) / len(second_half)
        
        if second_avg > first_avg * 1.1:
            return "increasing"
        elif second_avg < first_avg * 0.9:
            return "decreasing"
        else:
            return "stable"
```

## Community Support

### Get Help with NLM

#### Documentation Resources
- **NLM Documentation**: https://docs.nln.org
- **API Reference**: https://api.nln.org/python
- **Tutorials**: https://tutorials.nln.org
- **Examples**: https://examples.nln.org

#### Community Forums
- **NLM Community Forum**: https://community.nln.org
- **GitHub Discussions**: https://github.com/nlm/nlm/discussions
- **Stack Overflow**: https://stackoverflow.com/questions/tagged/nlm

#### Support Channels
- **Email Support**: support@nln.org
- **Bug Reports**: https://github.com/nlm/nlm/issues
- **Feature Requests**: https://github.com/nlm/nlm/issues/new
- **Technical Support**: Join NLM Slack community

#### Getting Help

When seeking help with NLM, provide the following information:

1. **Problem Description**: Clear description of the issue
2. **Error Messages**: Full error messages and stack traces
3. **System Information**: Platform, Python version, NLM version
4. **Configuration**: Configuration file or parameters used
5. **Reproduction Steps**: Steps to reproduce the issue
6. **Logs**: Relevant log files or console output
7. **Expected Behavior**: What should happen vs. what actually happens

Example help request:
```
Subject: NLM Simulation Performance Issues

Problem: NLM simulation is very slow and memory usage is high.

System Information:
- Platform: Linux Ubuntu 20.04
- Python: 3.8.10
- NLM: 1.0.0
- Network size: 5000 neurons

Configuration:
brain.neuron_count: 5000
brain.synapse_density: 0.1
plasticity.stdp.learning_rate: 0.01

Error Messages:
No explicit errors - simulation runs but is very slow.
Performance: 50 steps/second (target: 1000+ steps/second)

Reproduction Steps:
1. Create configuration with 5000 neurons
2. Initialize brain
3. Run 100 simulation steps
4. Measure performance

Expected Behavior:
- Simulation should run at >1000 steps/second
- Memory usage should be reasonable (<1GB)

Actual Behavior:
- Simulation runs at 50 steps/second
- Memory usage is high (>2GB)
```

## Conclusion

The NLM troubleshooting guide provides comprehensive coverage of common issues, debugging techniques, and problem solutions. This guide helps users:

### Key Troubleshooting Capabilities

1. **Problem Identification**: Recognize and categorize common issues
2. **Root Cause Analysis**: Understand underlying causes of problems
3. **Solution Implementation**: Apply appropriate fixes and workarounds
4. **Prevention**: Implement measures to avoid recurring issues
5. **Optimization**: Improve system performance and reliability

### Troubleshooting Framework

1. **Systematic Approach**: Follow structured troubleshooting process
2. **Comprehensive Coverage**: Address installation, configuration, runtime, and performance issues
3. **Debugging Techniques**: Use advanced debugging tools and techniques
4. **Error Analysis**: Understand error messages and their solutions
5. **Performance Optimization**: Optimize system performance and resolve bottlenecks

### Best Practices

1. **Information Collection**: Gather comprehensive information about issues
2. **Systematic Approach**: Follow structured troubleshooting process
3. **Documentation**: Document issues and resolutions
4. **Prevention**: Implement measures to prevent recurring issues
5. **Continuous Improvement**: Learn from issues and improve system

The NLM troubleshooting guide provides users with the knowledge and tools to effectively troubleshoot NLM issues, resolve problems quickly, and optimize their NLM simulations for better performance and reliability.
