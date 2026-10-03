# NLM - Comprehensive User Guide

## Overview

NLM (Neural Learning Machine) is an advanced artificial developmental brain simulation framework that implements real spiking neural computation with biologically-inspired learning mechanisms.

## Architecture Overview

### Core Components

1. **Brain** - The virtual brain that processes information
2. **World** - The environment the brain operates in  
3. **Agent** - The bridge connecting brain to world

### Key Features

- **Leaky Integrate-and-Fire (LIF) Neurons**: Biologically realistic neuron dynamics
- **Event-driven Spike Propagation**: Asynchronous signal transmission with delays
- **STDP Plasticity**: Spike-Timing Dependent Plasticity for learning
- **Hebbian Plasticity**: Activity-dependent synaptic strengthening
- **Structural Plasticity**: Dynamic synaptic growth and pruning
- **Neuromodulation**: Dopamine-based reward processing
- **Curiosity-driven Exploration**: Intrinsic motivation system
- **Development**: Stage-based brain maturation

## Getting Started

### Quick Start (Copy & Paste)

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
for i in range(100):
    brain.step(i)

print(f"Brain thought {brain.getTotalSpikeCount()} times")
```

### Step-by-Step Guide

#### 1. Create a Brain
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

# Create brain with configuration
brain = pynlm.createBrain(config)
```

#### 2. Initialize and Run
```python
# Initialize the brain
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Optional: Get statistics every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### 3. Create Agent and World
```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
```

#### 4. Run Complete Simulation
```python
# Main simulation loop
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get what agent sees
    percept = world.getSensoryPercept()
    
    # Tell brain what it sees
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: Provide reward based on outcome
    # reward = calculate_reward()
    # agent.applyRewardModulation(reward, expected_reward)
```

## Command Line Interface

### Basic Usage

```bash
# Show help
nlm --help

# Run basic connectivity test
nlm --test basic --steps 100

# Run plasticity experiment
nlm --test plasticity --random-seed 123 --progress

# Run STDP verification with verbose output
nlm --test stdp --quiet=false

# Custom configuration
nlm --test basic --neuron-count 500 --connection-prob 0.2

# All tests
nlm --test all --steps 1000
```

### Available Tests

#### Basic Test
Tests basic neural connectivity by injecting current and measuring spike propagation.

**Parameters:**
- `--steps`: Number of simulation steps (default: unlimited)

#### Plasticity Test  
Tests learning through STDP and Hebbian plasticity rules.

**Parameters:**
- `--steps`: Number of simulation steps (default: 1000)
- `--random-seed`: Random seed for reproducibility
- `--progress`: Show progress updates

#### STDP Test
Verifies Spike-Timing Dependent Plasticity mechanisms.

**Parameters:**
- `--verbose`: Detailed output of synaptic changes

### Configuration Options

#### Command Line Arguments

```bash
# General options
--help, -h              Show help message
--quiet, -q             Suppress output
--progress, -p          Show progress updates
--test <name>, -t <name> Run specific test
--steps <n>, -s <n>     Maximum simulation steps

# Configuration files and overrides
--config <file>         Load configuration from file
--random-seed <n>       Set random seed
--neuron-count <n>      Set number of neurons
--region-count <n>      Set number of regions
--connection-prob <p>   Set connection probability (0.0-1.0)

# Advanced options
--log-level <level>     Set logging level (DEBUG, INFO, WARN, ERROR, CRIT)
--log-file <path>       Log to file
--use-colors <bool>      Enable/disable colored output
```

#### Configuration File Format

**Default Configuration File** (`configs/default.cfg`):

```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

**JSON Configuration File Format**:

```json
{
    "nlm_configuration": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "plasticity_learning_rate": 0.01,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_time_constant": 20.0,
        "development_synaptogenesis_rate": 0.001,
        "development_pruning_rate": 0.0001,
        "dopamine_baseline": 0.1,
        "reward_discount_factor": 0.99,
        "environment_name": "GridWorld",
        "environment_width": 10,
        "environment_height": 10,
        "log_level": "INFO",
        "log_to_file": false,
        "log_filename": "nlm.log",
        "max_simulation_steps": 10000,
        "simulation_time_limit": 0.0,
        "visualization_enabled": false,
        "visualization_update_rate": 30.0
    }
}
```

## Advanced Usage

### Custom World Types

Create custom environments with specific characteristics:

```python
import pynlm

# Create a maze world
world = pynlm.createWorld("MazeWorld")
world.configure(
    width=30, 
    height=30,
    mazeComplexity=0.7,
    goalPositions=[(25, 25), (5, 5)]
)

# Create a puzzle world
world = pynlm.createWorld("PuzzleWorld")
world.configure(
    width=20,
    height=20,
    puzzleDifficulty=0.8,
    numberOfObjects=15,
    objectTypes=["resource", "hazard", "tool"]
)
```

### Agent Configuration

Configure advanced agent features:

```python
agent = pynlm.createAgentBrain(brain)

# Fine-tune learning parameters
agent.enableRewardModulation(True, learningRate=0.05, discountFactor=0.95)
agent.enableCuriosity(True, explorationWeight=0.3, noveltyThreshold=0.2)
agent.enableStructuralPlasticity(True, synaptogenesisRate=0.002, pruningRate=0.0005)
agent.enableDevelopment(True, developmentSpeed=1.5, criticalPeriodMultiplier=2.0)

# Set sensory processing parameters
agent.setVisionParameters(fov=120, resolution=64)
agent.setMotorOutputParameters(actionSpace="continuous", outputRange=(-1.0, 1.0))
```

### Performance Optimization

#### Multi-threaded Simulation
```python
import pynlm
import threading

def runSimulationStep(brain, step):
    brain.step(step)

# Create brain with optimization settings
config = pynlm.createDefaultConfig()
config.set("use_multithreading", True)
config.set("num_threads", 4)
config.set("thread_pool_size", 8)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with thread pool
threads = []
for step in range(1000):
    thread = threading.Thread(target=runSimulationStep, args=(brain, step))
    threads.append(thread)
    thread.start()
    
    # Limit active threads
    if len(threads) >= 8:
        for t in threads:
            t.join()
        threads.clear()

# Wait for remaining threads
for t in threads:
    t.join()
```

#### GPU Acceleration (if available)
```python
import pynlm

# Create brain with GPU acceleration
config = pynlm.createDefaultConfig()
config.set("use_gpu", True)
config.set("gpu_device_id", 0)
config.set("tensor_cores_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Use GPU-accelerated operations
brain.step_gpu(0, 0.001)
```

### Machine Learning Integration

#### Transfer Learning
```python
import pynlm
import numpy as np

# Create source brain (pretrained)
source_brain = pynlm.createBrain()
source_brain.load("/path/to/pretrained/model.nnet")

# Create target brain (for fine-tuning)
target_brain = pynlm.createBrain()
target_brain.initialize()

# Transfer knowledge
target_brain.transferKnowledge(source_brain, transferRatio=0.7)

# Fine-tune with new data
for episode in range(100):
    state = environment.reset()
    done = False
    
    while not done:
        action = target_brain.act(state, epsilon=0.1)
        next_state, reward, done = environment.step(action)
        
        # Learn from experience
        target_brain.learn(state, action, reward, next_state, done)
        
        state = next_state
```

#### Reinforcement Learning
```python
import pynlm
import torch

# Create actor-critic agent
class ActorCriticAgent:
    def __init__(self, brain, learning_rate=0.001):
        self.brain = brain
        self.criterion = torch.nn.MSELoss()
        self.optimizer = torch.optim.Adam(brain.parameters(), lr=learning_rate)
    
    def select_action(self, state):
        # Use brain's motor system
        return self.brain.act(state)
    
    def update(self, state, action, reward, next_state, done):
        # Compute advantage using brain's internal reward system
        advantage = self.brain.computeAdvantage(state, action, reward, next_state)
        
        # Update using gradient descent
        loss = self.criterion(advantage, torch.tensor([reward]))
        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()

# Create agent and train
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = ActorCriticAgent(brain)

for episode in range(1000):
    state = environment.reset()
    done = False
    total_reward = 0
    
    while not done:
        action = agent.select_action(state)
        next_state, reward, done = environment.step(action)
        
        agent.update(state, action, reward, next_state, done)
        
        state = next_state
        total_reward += reward
    
    if episode % 100 == 0:
        print(f"Episode {episode}, Total Reward: {total_reward}")
```

### Monitoring and Debugging

#### Real-time Monitoring
```python
import pynlm
import time
import matplotlib.pyplot as plt

# Create brain with monitoring enabled
config = pynlm.createDefaultConfig()
config.set("enable_monitoring", True)
config.set("monitoring_interval", 0.1)  # seconds

brain = pynlm.createBrain(config)
brain.initialize()

# Start monitoring
monitor = brain.getMonitor()

# Record metrics
metrics = {
    'spike_count': [],
    'firing_rate': [],
    'synaptic_weight': [],
    'plasticity_factor': []
}

start_time = time.time()
for step in range(10000):
    brain.step(step, step * 0.001)
    
    if time.time() - start_time >= config.getOr<double>("monitoring_interval", 0.1):
        # Record current metrics
        metrics['spike_count'].append(brain.getTotalSpikeCount())
        metrics['firing_rate'].append(brain.getAverageFiringRate())
        metrics['synaptic_weight'].append(brain.getAverageSynapticWeight())
        metrics['plasticity_factor'].append(brain.getPlasticityFactor())
        
        start_time = time.time()
        
        # Optional: Plot metrics
        if step % 1000 == 0:
            plt.figure(figsize=(12, 8))
            plt.subplot(2, 2, 1)
            plt.plot(metrics['spike_count'])
            plt.title('Spike Count')
            
            plt.subplot(2, 2, 2)
            plt.plot(metrics['firing_rate'])
            plt.title('Firing Rate')
            
            plt.subplot(2, 2, 3)
            plt.plot(metrics['synaptic_weight'])
            plt.title('Synaptic Weight')
            
            plt.subplot(2, 2, 4)
            plt.plot(metrics['plasticity_factor'])
            plt.title('Plasticity Factor')
            
            plt.tight_layout()
            plt.show()
```

#### Debugging Tools
```python
import pynlm

# Create brain with debugging enabled
config = pynlm.createDefaultConfig()
config.set("debug_mode", True)
config.set("trace_level", "detailed")  # "basic", "detailed", "full"

brain = pynlm.createBrain(config)
brain.initialize()

# Debug specific neurons
brain.debugNeuron(42)  # Debug neuron with ID 42
brain.traceSynapse(100, 200)  # Trace synapse between neurons 100 and 200

# Check for issues
if brain.hasMemoryLeak():
    print("WARNING: Memory leak detected!")
    
if brain.hasPlasticityError():
    print("WARNING: Plasticity calculation error!")

# Validate network properties
brain.validateConnectivity()
brain.validateBalance()
```

## API Reference

### Core Brain API

#### Brain Class
```python
class Brain:
    def __init__(self, config):
        """Initialize brain with configuration"""
        
    def initialize(self):
        """Initialize brain with random parameters"""
        
    def step(self, step, time):
        """Process one simulation step"""
        
    def getTotalNeuronCount(self):
        """Get total number of neurons"""
        
    def getFiringNeuronCount(self):
        """Get number of firing neurons"""
        
    def getTotalSpikeCount(self):
        """Get total number of spikes"""
        
    def getAverageFiringRate(self):
        """Get average firing rate (Hz)"""
        
    def getAverageSynapticWeight(self):
        """Get average synaptic weight"""
        
    def getPlasticityFactor(self):
        """Get current plasticity factor"""
        
    def injectCurrent(self, neuron_id, current):
        """Inject current into specific neuron"""
        
    def injectCurrentToNeurons(self, neuron_type, current):
        """Inject current to all neurons of specific type"""
        
    def reset(self):
        """Reset brain to initial state"""
        
    def logStatus(self):
        """Log current brain status"""
```

#### Configuration API
```python
class Configuration:
    def __init__(self):
        """Create empty configuration"""
        
    def loadFromFile(self, filepath):
        """Load configuration from file"""
        
    def loadFromArgs(self, argv):
        """Load configuration from command line arguments"""
        
    def saveToFile(self, filepath):
        """Save configuration to file"""
        
    def set(self, key, value, source=None):
        """Set configuration value"""
        
    def get(self, key):
        """Get configuration value"""
        
    def getOr(self, key, default_value):
        """Get configuration value or default"""
        
    def has(self, key):
        """Check if configuration has key"""
        
    def remove(self, key):
        """Remove configuration key"""
        
    def getKeys(self):
        """Get all configuration keys"""
        
    def clear(self):
        """Clear all configuration"""
        
    def summary(self):
        """Get configuration summary"""
```

#### AgentBrain API
```python
class AgentBrain:
    def __init__(self, brain):
        """Connect brain to world"""
        
    def initialize(self, world):
        """Initialize agent with world"""
        
    def processSensoryInput(self, percept):
        """Process sensory input from world"""
        
    def decodeMotorCommand(self):
        """Decode motor command from brain"""
        
    def applyRewardModulation(self, reward, predicted_reward):
        """Apply reward-based learning"""
        
    def updateDevelopment(self, timestep):
        """Update developmental state"""
        
    # Configuration methods
    def enableRewardModulation(self, enable, learning_rate=1.0, discount_factor=0.95):
        """Enable/disable reward modulation"""
        
    def enableCuriosity(self, enable, exploration_weight=1.0, novelty_threshold=0.5):
        """Enable/disable curiosity system"""
        
    def enableStructuralPlasticity(self, enable, synaptogenesis_rate=0.001, pruning_rate=0.0001):
        """Enable/disable structural plasticity"""
        
    def enableDevelopment(self, enable, development_speed=1.0, critical_period_multiplier=1.0):
        """Enable/disable development system"""
```

#### Environment API
```python
class World:
    def __init__(self, world_type="SimpleWorld"):
        """Create world with specific type"""
        
    def configure(self, **kwargs):
        """Configure world parameters"""
        
    def reset(self):
        """Reset world to initial state"""
        
    def update(self, dt):
        """Update world state by dt seconds"""
        
    def getSensoryPercept(self):
        """Get sensory percept from world"""
        
    def applyMotorCommand(self, action, time):
        """Apply motor command to world"""
        
    def getSimulationTime(self):
        """Get current simulation time"""
```

### Monitoring and Debugging API

#### Logger API
```python
class Logger:
    def __init__(self):
        """Create logger instance"""
        
    def addLogger(self, logger):
        """Add logging sink"""
        
    def setLevel(self, level):
        """Set minimum log level"""
        
    def info(self, message, file="", line=0, function=""):
        """Log info message"""
        
    def debug(self, message, file="", line=0, function=""):
        """Log debug message"""
        
    def warning(self, message, file="", line=0, function=""):
        """Log warning message"""
        
    def error(self, message, file="", line=0, function=""):
        """Log error message"""
        
    def critical(self, message, file="", line=0, function=""):
        """Log critical message"""
        
    @staticmethod
    def getGlobal():
        """Get global logger instance"""
```

#### Progress Tracking
```python
# Progress tracking functions
NLM_PROGRESS_START("TaskName")  # Start progress tracking
NLM_PROGRESS_UPDATE("TaskName", 0.5)  # Update progress to 50%
NLM_PROGRESS_COMPLETE("TaskName")  # Complete progress
NLM_PROGRESS_FAIL("TaskName", "Error message")  # Fail progress with error
```

#### Performance Monitoring
```python
# Performance tracking
NLM_PERF_OP("operation_name", elapsed_time)  # Log performance
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Brain initialization fails
**Cause**: Invalid configuration or missing dependencies
**Solution**: 
- Check configuration file syntax
- Ensure required dependencies are installed
- Verify system resources (memory, CPU)

#### Issue: Simulation runs too slowly
**Cause**: Too many neurons or inefficient implementation
**Solution**:
- Reduce neuron count for testing
- Enable GPU acceleration if available
- Use multi-threading for simulation steps
- Profile and optimize code

#### Issue: Learning doesn't occur
**Cause**: Plasticity disabled or incorrect parameters
**Solution**:
- Ensure plasticity is enabled
- Check plasticity parameter values
- Verify spike activity is occurring
- Review STDP timing parameters

#### Issue: Memory usage too high
**Cause**: Memory leak or inefficient data structures
**Solution**:
- Run memory profiler
- Check for circular references
- Use smart pointers appropriately
- Implement object pooling for frequently allocated objects

### Debug Commands

#### Command Line Debugging
```bash
# Enable verbose debugging
nlm --verbose --debug-mode

# Log level debugging
nlm --log-level DEBUG --log-file debug.log

# Performance profiling
nlm --profile --profile-output profile.json

# Network connectivity testing
nlm --test-connectivity
```

#### Runtime Debugging
```python
import pynlm

# Enable runtime debugging
brain = pynlm.createBrain()
brain.enableDebugging(True)

# Monitor specific components
brain.monitorNeurons([1, 5, 10, 15])
brain.monitorSynapses([(1, 2), (2, 3), (3, 4)])

# Check for anomalies
if brain.detectAnomalies():
    print("Anomalies detected!")
    brain.reportAnomalies()
```

## Performance Optimization Tips

### Simulation Performance
1. **Batch Operations**: Group related operations together
2. **Memory Pooling**: Reuse memory allocations
3. **Vectorization**: Use SIMD instructions where possible
4. **Multi-threading**: Distribute computation across cores
5. **GPU Acceleration**: Leverage GPU for large-scale simulations

### Code Optimization
1. **Use References**: Avoid unnecessary copies
2. **Reserve Memory**: Pre-allocate containers when possible
3. **Cache Frequently Used Data**: Store in registers or cache-friendly structures
4. **Use Move Semantics**: Efficiently transfer ownership
5. **Template Specialization**: Optimize for common cases

## Migration Guide

### From Phase 1 to Phase 2

#### Breaking Changes
1. **Configuration System**: JSON/YAML support added
2. **Neuron Model**: More accurate LIF dynamics
3. **Plasticity Rules**: Enhanced STDP and Hebbian implementations
4. **Progress Tracking**: New progress monitoring system
5. **Logging**: Improved logging with progress tracking

#### Migration Steps
1. **Update Configuration**: Convert old config format to JSON/YAML
2. **Review Neuron Parameters**: Adjust for new dynamics
3. **Update Plasticity Parameters**: Fine-tune new rules
4. **Add Progress Tracking**: Implement in simulation loop
5. **Enhance Logging**: Add progress and performance metrics

### Example Migration

#### Old Configuration
```ini
[config]
neuron_count = 100
simulation_timestep = 0.001
plasticity_learning_rate = 0.01
```

#### New Configuration
```json
{
    "nlm_configuration": {
        "neuron_count": 100,
        "simulation_timestep": 0.001,
        "plasticity_learning_rate": 0.01,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "development_synaptogenesis_rate": 0.001
    }
}
```

## Future Enhancements

### Planned Features
1. **Advanced Learning Algorithms**: Meta-learning, reinforcement learning
2. **Multi-agent Systems**: Multiple interacting brains
3. **Hierarchical Architectures**: Deep neural architectures
4. **Real-time Interaction**: Web-based visualization and control
5. **Cloud Computing**: Distributed simulation capabilities

### Research Directions
1. **Neuromorphic Computing**: Hardware-software co-design
2. **Cognitive Architectures**: Integrated perception and reasoning
3. **Developmental Psychology**: Models of cognitive development
4. **Social Learning**: Multi-agent learning and coordination
5. **Transfer Learning**: Cross-domain knowledge transfer

## References

1. Maass, W., Natschläger, T., & Markram, H. (2002). Real-time computing without fixed point arithmetic. Modern Hardware Paradigms for Neural Simulation.
2. Gerstner, W., Kistler, W., Naud, R., & Paninski, L. (2014). Neural coding, computation, and dynamics in the brain.
3. Bengio, Y., et al. (2013). Deep learning for computer vision: A review.
4. Lillicrap, T. P., et al. (2016). Random synaptic feedback weights support learning in deep neural networks.
5. Hinton, G. E., et al. (2015). Reducing the dimensionality of data with neural networks.

## Acknowledgments

The NLM project builds on decades of research in computational neuroscience, neural networks, and artificial intelligence. Special thanks to:

- Computational neuroscience researchers for biological insights
- Open source community for foundational libraries
- Early contributors to the NLM framework
- Users who provide feedback and report issues

## License

This software is licensed under the MIT License. See LICENSE file for details.

---

*Last updated: October 2024*
*NLM Version 2.0.0*