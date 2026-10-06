# NLM (Neural Learning Machine) - Documentation Enhancement

## Overview
This document provides comprehensive documentation for the NLM (Neural Learning Machine) project, an experimental artificial developmental brain system implemented in C++20 with Python bindings.

## Contents

### 1. Quick Start Guide
#### Installation

```bash
# Clone the repository
cd /path/to/nlm

# Build the C++ library and Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install scikit-build-core pybind11 pytest numpy
pip install -e .  # Install in development mode
```

#### Basic Usage Example

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run basic simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes recorded")
```

### 2. Core Concepts

#### Brain Architecture
NLM implements a brain-inspired architecture with multiple integrated systems:

1. **Neurons**: Leaky Integrate-and-Fire (LIF) neurons with biological plausibility
2. **Synaptic Plasticity**: STDP, Hebbian, and reward-modulated learning
3. **Memory Systems**: Working memory, episodic memory, associative memory
4. **Neuromodulation**: Dopamine, curiosity, novelty detection
5. **Cognition**: Neural planning, concept formation, attention
6. **Development**: Age-dependent plasticity, structural maturation

#### Key Components
- **Brain**: Central neural processing unit with integrated systems
- **Agent**: Interface between brain and environment
- **World**: Simulated environment with sensory and motor interaction
- **Config**: Configuration management (JSON/YAML support)

### 3. Configuration Management

#### Configuration File Formats

##### JSON Format (Recommended)

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity": {
      "learning_rate": 0.01,
      "stdp_ltp_weight": 0.02,
      "stdp_ltd_weight": 0.018
    }
  },
  "environment": {
    "type": "GridWorld",
    "width": 20,
    "height": 20
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000
  }
}
```

##### Key-Value Format (Backward Compatible)

```
# Configuration file
brain.neuron_count = 1000
brain.region_count = 2
brain.connection_probability = 0.1
brain.plasticity.stdp_ltp_weight = 0.02
brain.plasticity.stdp_ltd_weight = 0.018
```

#### Configuration Classes

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Set values
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp_learning_rate", 0.01)

# Get values
neuron_count = config.getOr("brain.neuron_count", 1000)
if config.has("brain.custom_parameter"):
    value = config.get("brain.custom_parameter")

# Save configuration
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")
```

### 4. Advanced Brain Control

#### Brain Initialization and Setup

```python
def create_advanced_brain(neuron_count=2000, development_enabled=True):
    """Create a brain with advanced configuration"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.12)
    config.set("plasticity.stdp_ltp_weight", 0.025)
    config.set("plasticity.stdp_ltd_weight", 0.023)
    config.set("plasticity.hebbian_learning_rate", 0.015)
    config.set("development.synaptogenesis_rate", 0.0002)
    config.set("development.pruning_rate", 0.00002)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable all advanced plasticity mechanisms
    brain.getSTDP().configure(0.025f, 0.023f, 25.0f)
    brain.getHebbian().setLearningRate(0.015f)
    
    return brain
```

#### Simulation Control

```python
def run_advanced_simulation(brain, num_steps=1000, enable_logging=True, 
                           checkpoint_interval=100):
    """Run advanced simulation with comprehensive monitoring"""
    
    # Enable advanced features
    brain.getSTDP().configure(0.02f, 0.018f, 30.0f)
    brain.getHebbian().setLearningRate(0.01f)
    
    if enable_logging:
        brain.logStatus()
    
    # Get initial snapshot
    initial_snapshot = get_brain_snapshot(brain)
    
    # Run simulation with progress monitoring
    for step in range(num_steps):
        brain.step(step, step * 0.001)
        
        # Periodic checkpointing
        if checkpoint_interval > 0 and step % checkpoint_interval == 0:
            checkpoint_path = f"checkpoints/step_{step:06d}.bin"
            if brain.save(checkpoint_path):
                print(f"Checkpoint saved: {checkpoint_path}")
        
        # Advanced monitoring
        if enable_logging and step % 100 == 0:
            monitor_brain_activity(brain, step)
    
    # Get final snapshot
    final_snapshot = get_brain_snapshot(brain)
    
    # Calculate learning metrics
    learning_metrics = calculate_learning_metrics(initial_snapshot, final_snapshot)
    
    return learning_metrics
```

#### Network Topology Analysis

```python
def analyze_network_topology(brain, output_file="network_topology.txt"):
    """Analyze and export brain network structure"""
    
    regions = brain.getRegions()
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    
    with open(output_file, 'w') as f:
        f.write("NLM Network Topology Analysis\n")
        f.write("=" * 40 + "\n\n")
        
        f.write(f"Total Neurons: {total_neurons}\n")
        f.write(f"Total Synapses: {total_synapses}\n")
        f.write(f"Average Connectivity: {total_synapses/total_neurons:.2f} synapses/neuron\n\n")
        
        f.write("Region Analysis:\n")
        for i, region in enumerate(regions):
            f.write(f"  Region {i}: {region.getName() or 'Unnamed'}\n")
            f.write(f"    Neurons: {region.getNeuronCount()}\n")
            f.write(f"    Synapses: {region.getSynapseCount()}\n")
            f.write(f"    Connectivity Density: {region.getSynapseCount()/region.getNeuronCount():.3f}\n\n")
        
        # Analyze plasticity distribution
        f.write("Plasticity Distribution:\n")
        if brain.getSTDP():
            f.write(f"  STDP: LTP weight = {brain.getSTDP().getLTPWeight()}, "
                   f"LTD weight = {brain.getSTDP().getLTDWeight()}\n")
        if brain.getHebbian():
            f.write(f"  Hebbian: Learning rate = {brain.getHebbian().getLearningRate()}\n")
        if brain.getStructuralPlasticity():
            f.write(f"  Structural: Synaptogenesis = {brain.getStructuralPlasticity().getSynaptogenesisRate()}, "
                   f"Pruning = {brain.getStructuralPlasticity().getPruningRate()}\n")
```

### 5. Agent Environment Integration

#### Complete Agent Example

```python
def run_agent_simulation(num_steps=1000, enable_advance_features=True):
    """Run a complete agent simulation with world interaction"""
    
    # Setup brain with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.region_count", 2)
    config.set("environment.width", 30)
    config.set("environment.height", 30)
    config.set("environment.visionWidth", 10)
    config.set("environment.visionHeight", 10)
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # Enable advanced agent features
    if enable_advance_features:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward and apply modulation
        reward = 0.0
        if agent.getRewardModulationEnabled():
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print()
    
    return brain, agent, world
```

### 6. Performance Monitoring and Analysis

#### Performance Metrics

```python
def get_comprehensive_performance_metrics(brain):
    """Get detailed performance metrics for the brain"""
    
    metrics = {}
    
    # Basic metrics
    metrics["neuron_count"] = brain.getTotalNeuronCount()
    metrics["synapse_count"] = brain.getTotalSynapseCount()
    metrics["firing_rate"] = brain.getAverageFiringRate()
    metrics["total_spikes"] = brain.getTotalSpikeCount()
    metrics["excitatory_inhibitory_ratio"] = brain.getExcitationInhibitionRatio()
    
    # Efficiency metrics
    total_possible = brain.getTotalNeuronCount() * 10  # Approximate maximum connectivity
    metrics["connectivity_efficiency"] = brain.getTotalSynapseCount() / total_possible
    
    theoretical_max_rate = 100.0  # Hz
    metrics["network_utilization"] = brain.getAverageFiringRate() / theoretical_max_rate
    
    # Plasticity metrics
    if brain.getSTDP():
        metrics["stdp_ltp_weight"] = brain.getSTDP().getLTPWeight()
        metrics["stdp_ltd_weight"] = brain.getSTDP().getLTDWeight()
        metrics["stdp_time_constant"] = brain.getSTDP().getTimeConstant()
    
    if brain.getHebbian():
        metrics["hebbian_learning_rate"] = brain.getHebbian().getLearningRate()
    
    if brain.getStructuralPlasticity():
        metrics["synaptogenesis_rate"] = brain.getStructuralPlasticity().getSynaptogenesisRate()
        metrics["pruning_rate"] = brain.getStructuralPlasticity().getPruningRate()
    
    # Development metrics
    metrics["developmental_stage"] = brain.getDevelopmentalStage()
    
    return metrics
```

#### Brain Comparison

```python
def compare_two_brains(brain1, brain2, comparison_name="Comparison"):
    """Compare two brains and return detailed comparison metrics"""
    
    comparison = {}
    comparison["name"] = comparison_name
    comparison["timestamp"] = time.time()
    
    # Structural comparison
    comparison["neuron_count_match"] = brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount()
    comparison["synapse_count_match"] = brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount()
    comparison["region_count_match"] = brain1.getRegionCount() == brain2.getRegionCount()
    
    # Functional comparison
    comparison["firing_rate_match"] = brain1.getAverageFiringRate() == brain2.getAverageFiringRate()
    comparison["spike_count_match"] = brain1.getTotalSpikeCount() == brain2.getTotalSpikeCount()
    
    # Difference metrics
    comparison["firing_rate_difference"] = abs(brain1.getAverageFiringRate() - brain2.getAverageFiringRate())
    comparison["spike_count_difference"] = abs(brain1.getTotalSpikeCount() - brain2.getTotalSpikeCount())
    
    # Plasticity comparison
    if brain1.getSTDP() and brain2.getSTDP():
        comparison["stdp_match"] = (brain1.getSTDP().getLTPWeight() == brain2.getSTDP().getLTPWeight() and
                                   brain1.getSTDP().getLTDWeight() == brain2.getSTDP().getLTDWeight())
        comparison["stdp_ltp_diff"] = abs(brain1.getSTDP().getLTPWeight() - brain2.getSTDP().getLTPWeight())
    
    return comparison
```

### 7. Troubleshooting and Common Issues

#### Common Problems and Solutions

##### "My brain isn't doing anything"
**Issue**: Brain appears inactive with no spikes or firing neurons.

**Possible causes**:
1. Brain not initialized (forgot to call `brain.initialize()`)
2. Insufficient current injection into neurons
3. Incorrect configuration parameters
4. Threshold too high or membrane potential too low

**Solutions**:
```python
# Solution 1: Ensure brain is initialized
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!

# Solution 2: Inject current to activate neurons
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0)

# Solution 3: Check configuration
config.set("brain.v_thresh", -50.0)  # Lower threshold
config.set("brain.v_rest", -70.0)    # Reset resting potential
```

##### "The agent isn't moving"
**Issue**: Agent produces no motor actions.

**Possible causes**:
1. No motor neurons activated
2. Motor command decoding fails
3. World doesn't accept actions

**Solutions**:
```python
# Solution 1: Check motor neuron activity
print(f"Motor neurons: {brain.getTotalNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")

# Solution 2: Debug motor command
motor_cmd = agent.decodeMotorCommand()
print(f"Motor command type: {motor_cmd.getType()}")
print(f"Motor command parameters: {motor_cmd.getParameters()}")

# Solution 3: Check world state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent is moving: {body.isMoving}")
```

##### "Everything is 0"
**Issue**: All metrics show zero values.

**Solution**: Brains need time to "warm up". Increase simulation steps.

```python
# Increase simulation time for warm-up
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### Debug Scripts

```python
def debug_brain(brain, world=None, agent=None):
    """Debug brain state and print diagnostic information"""
    
    print("=== Brain Debug Information ===\n")
    
    # Basic statistics
    print("Basic Statistics:")
    print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing Neurons: {brain.getFiringNeuronCount()}")
    print(f"  Active Neurons: {brain.getActiveNeuronCount()}")
    print(f"  Average Firing Rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Developmental Stage: {brain.getDevelopmentalStage()}\n")
    
    # Connectivity analysis
    total_possible = brain.getTotalNeuronCount() * 10
    connectivity = brain.getTotalSynapseCount() / total_possible
    print(f"Connectivity Analysis:")
    print(f"  Theoretical Max Synapses: {total_possible}")
    print(f"  Actual Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Connectivity Density: {connectivity:.3f}\n")
    
    # Plasticity analysis
    print("Plasticity Systems:")
    if brain.getSTDP():
        print(f"  STDP - LTP: {brain.getSTDP().getLTPWeight()}, LTD: {brain.getSTDP().getLTDWeight()}")
    if brain.getHebbian():
        print(f"  Hebbian - Learning Rate: {brain.getHebbian().getLearningRate()}")
    if brain.getStructuralPlasticity():
        print(f"  Structural - Synaptogenesis: {brain.getStructuralPlasticity().getSynaptogenesisRate()}, "
              f"Pruning: {brain.getStructuralPlasticity().getPruningRate()}\n")
    
    # Regional analysis
    print("Regional Analysis:")
    regions = brain.getRegions()
    for i, region in enumerate(regions):
        region_id = brain.getRegionIds()[i]
        region_name = region.getName() or f"Region_{i}"
        print(f"  {region_name}:")
        print(f"    Neurons: {region.getNeuronCount()}")
        print(f"    Synapses: {region.getSynapseCount()}")
        if region.getNeuronCount() > 0:
            density = region.getSynapseCount() / region.getNeuronCount()
            print(f"    Density: {density:.3f}\n")
    
    # World/Agent analysis if available
    if world:
        print("World State:")
        body = world.getAgentBody()
        print(f"  Position: ({body.x:.1f}, {body.y:.1f})")
        print(f"  Orientation: {body.orientation:.1f}°")
        print(f"  Velocity: ({body.velocityX:.2f}, {body.velocityY:.2f})")
        print(f"  Energy: {body.energy:.1f}/{body.getMaxEnergy():.1f}")
        print(f"  Health: {body.health:.1f}\n")
    
    if agent:
        print("Agent State:")
        print(f"  Reward Modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
        print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
        print(f"  Development Enabled: {'Yes' if agent.isDevelopmentEnabled() else 'No'}")
```

### 8. Advanced Configuration Examples

#### High-Performance Brain Configuration

```python
def create_high_performance_brain():
    """Create a brain optimized for high performance and learning"""
    config = pynlm.createDefaultConfig()
    
    # Optimize for performance
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.15)
    config.set("simulation_timestep", 0.0005)  # Smaller timestep for accuracy
    
    # Advanced plasticity settings
    config.set("plasticity.stdp_ltp_weight", 0.03)
    config.set("plasticity.stdp_ltd_weight", 0.028)
    config.set("plasticity.stdp_time_constant", 30.0)
    config.set("plasticity.hebbian_learning_rate", 0.02)
    config.set("plasticity.structural.enable", true)
    
    # Development settings
    config.set("development.synaptogenesis_rate", 0.0003)
    config.set("development.pruning_rate", 0.00003)
    config.set("development.developmental_period", 0.5)
    
    # Neuromodulation settings
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", true)
    config.set("neuromod.curiosity.intensity", 0.8)
    
    # Memory settings
    config.set("memory.working_memory.capacity", 50)
    config.set("memory.episodic.capacity", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Apply optimal plasticity parameters
    if brain.getSTDP():
        brain.getSTDP().configure(0.03f, 0.028f, 30.0f)
    
    return brain
```

#### Learning-Focused Configuration

```python
def create_learning_focused_brain():
    """Create a brain optimized for rapid learning and adaptation"""
    config = pynlm.createDefaultConfig()
    
    # Moderate size for efficient learning
    config.set("brain.neuron_count", 1500)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.12)
    
    # Enhanced plasticity for learning
    config.set("plasticity.stdp_ltp_weight", 0.025)
    config.set("plasticity.stdp_ltd_weight", 0.023)
    config.set("plasticity.hebbian_learning_rate", 0.015)
    config.set("plasticity.reward_modulated.enable", true)
    
    # Strong developmental plasticity
    config.set("development.synaptogenesis_rate", 0.0005)
    config.set("development.pruning_rate", 0.00005)
    config.set("development.stage_transition_speed", 0.8)
    
    # Exploration-focused neuromodulation
    config.set("neuromod.curiosity.enable", true)
    config.set("neuromod.curiosity.intensity", 1.0)
    config.set("neuromod.novelty.enable", true)
    config.set("neuromod.prediction_error.sensitivity", 1.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable all learning mechanisms
    if brain.getSTDP():
        brain.getSTDP().configure(0.025f, 0.023f, 25.0f)
    
    if brain.getHebbian():
        brain.getHebbian().setLearningRate(0.015f)
    
    return brain
```

### 9. API Reference

#### Core Classes

##### Config Class
| Method | Description | Parameters | Return Type |
|--------|-------------|------------|-------------|
| `loadFromFile(filepath)` | Load configuration from JSON/YAML file | `filepath: str` | `bool` |
| `saveToFile(filepath)` | Save configuration to JSON file | `filepath: str` | `bool` |
| `getOr(key, default)` | Get value or return default | `key: str, default: Any` | `Any` |
| `has(key)` | Check if key exists | `key: str` | `bool` |
| `set(key, value, source)` | Set configuration value | `key: str, value: Any, source: ConfigSource` | `void` |

##### Brain Class
| Method | Description | Parameters | Return Type |
|--------|-------------|------------|-------------|
| `step(step)` | Run simulation step | `step: SimulationStep` | `void` |
| `receiveSensoryInput(input)` | Inject sensory input | `input: SensoryInput` | `void` |
| `produceAction()` | Produce motor action | - | `Action` |
| `save(filepath)` | Save brain state | `filepath: str` | `bool` |
| `load(filepath)` | Load brain state | `filepath: str` | `bool` |
| `getTotalNeuronCount()` | Get total neuron count | - | `size_t` |
| `getFiringNeuronCount()` | Get firing neuron count | - | `size_t` |
| `getAverageFiringRate()` | Get average firing rate | - | `float` |
| `getTotalSpikeCount()` | Get total spike count | - | `size_t` |

##### AgentBrain Class
| Method | Description | Parameters | Return Type |
|--------|-------------|------------|-------------|
| `initialize(world)` | Initialize with world | `world: SimpleWorld` | `void` |
| `processSensoryInput(percept)` | Process sensory percept | `percept: SensoryPercept` | `void` |
| `decodeMotorCommand()` | Decode motor command | - | `MotorCommand` |
| `applyRewardModulation(reward, predicted)` | Apply reward modulation | `reward: float, predicted: float` | `void` |
| `updateDevelopment(timestep)` | Update development | `timestep: float` | `void` |

#### Helper Functions

```python
# Configuration helpers
pynlm.createDefaultConfig()
pynlm.createBrain(config)
pynlm.createSimpleWorld()
pynlm.createAgentBrain(brain)

# Constants
pynlm.INVALID_NEURON_ID
pynlm.INVALID_SYNAPSE_ID
pynlm.INVALID_REGION_ID

# Enumerations
pynlm.NeuronType
pynlm.SynapseType
pynlm.DevelopmentalStage
pynlm.ActionType
pynlm.MotorCommand
```

### 10. Performance Tips

#### Memory Management

1. **Reuse objects**: Create brain/world objects and reuse them
2. **Checkpoint strategically**: Save progress at meaningful intervals
3. **Monitor memory usage**: Use `getTotalNeuronCount()` to track memory

#### Simulation Optimization

1. **Batch processing**: Use `runSimulationSteps()` for multiple steps
2. **Adjust timestep**: Smaller timesteps for accuracy, larger for speed
3. **Enable pruning**: Structural plasticity can reduce network size
4. **Use development stages**: Later stages can have faster computation

#### Hardware Considerations

1. **CPU vs GPU**: NLM runs on CPU, optimize for cache efficiency
2. **Memory bandwidth**: Large networks benefit from efficient data structures
3. **Parallelization**: Use multiple simulation steps concurrently when possible

### 11. Future Directions

#### Planned Enhancements

1. **PyTorch Integration**: GPU acceleration for neural computations
2. **Distributed Computing**: Multi-machine brain simulation
3. **Advanced Visualization**: Real-time 3D brain visualization
4. **Cloud Deployment**: AWS/GCP integration for scalable simulation
5. **Mobile Support**: Optimized for embedded systems and mobile devices

#### Research Opportunities

1. **Neuromorphic Computing**: Hardware-software co-design
2. **Developmental Robotics**: Learning through interaction
3. **Social Learning**: Multi-agent collaboration
4. **Transfer Learning**: Brain state transfer between simulations

## Appendices

### A. Configuration File Examples

### B. Error Codes

### C. Version History

### D. Acknowledgements

---

**NLM Documentation**
Maintained by the NLM Project Team
Last Updated: 2026-10-06
License: MIT

*This documentation is continuously updated. Please report any issues or suggestions.*