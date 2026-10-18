# NLM (Neural Learning Machine) - Improved Documentation

## Overview

NLM is an advanced experimental artificial brain simulator that implements spiking neural networks with biologically realistic dynamics. This document provides comprehensive guidance for both beginners and expert users to effectively use and extend the NLM system.

## Key Improvements in This Release

### Enhanced Learning Mechanisms
- **STDP**: Real spike-timing dependent plasticity with exponential learning windows
- **Hebbian Learning**: Correlation-based learning "neurons that fire together, wire together"
- **Reward-Modulated STDP (R-STDP)**: Learning driven by reward signals with eligibility traces
- **Triplet STDP**: Advanced learning with pre-post spike triplet interactions

### Advanced Neural Dynamics
- **Biologically Realistic LIF Neurons**: Exponential integration, spike-frequency adaptation
- **Short-Term Plasticity**: Dynamic synaptic efficacy modulation (facilitation/depression)
- **Eligibility Traces**: Delayed reward-modulated learning capabilities
- **Synaptic Dynamics**: Real synaptic dynamics with proper conductance-based modeling

### System Integration
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
- **Development**: Neural development and maturation over time
- **Prediction**: Predictive coding and error computation

## Quick Start Guide

### 1. Basic Usage (Beginner Level)

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()

# Optionally customize configuration
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate activity for 100 steps
for step in range(100):
    brain.step(step)
    
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 2. Advanced Agent Simulation (Expert Level)

```python
import pynlm

def run_advanced_simulation(config_path, num_steps=1000):
    """Advanced agent simulation with all features enabled"""
    
    # Load custom configuration
    config = pynlm.createDefaultConfig()
    if config.loadFromFile(config_path):
        print(f"Loaded configuration from {config_path}")
    
    # Create brain and agent systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, height=30,
        visionWidth=10, visionHeight=10
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    
    # Run simulation with detailed monitoring
    print("Starting advanced simulation...")
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics periodically
        if step % 100 == 0:
            metrics = collect_simulation_metrics(brain, agent, world, step)
            print(f"Step {step}: {metrics}")
    
    return brain, world, agent

# Enable advanced subsystems
def enable_advanced_features(agent):
    """Enable all advanced learning and development features"""
    # Learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    
    # Memory systems
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    
    # Advanced plasticity
    agent.enableSTDP(True)
    agent.enableHebbianLearning(True)
    agent.enableRewardModulatedLearning(True)
```

### 3. Configuration Guide

#### Brain Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Connection probability between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable spike-timing dependent plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | LTP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | LTD time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.01 | Hebbian learning rate |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Advanced Features Guide

### 1. Custom Learning Rules

Create custom plasticity rules by extending the base PlasticityRule class:

```python
class CustomPlasticityRule(pynlm.api.PlasticityRule):
    def update(self, synapse, pre_spikes, post_spikes, dt):
        # Custom learning logic based on spike timing
        delta = self.compute_weight_change(pre_spikes, post_spikes, dt)
        self.apply_weight_change(synapse, delta)
    
    def apply_weight_change(self, synapse, delta):
        # Apply weight change with custom constraints
        new_weight = synapse.get_weight() + delta
        new_weight = max(-1.0, min(1.0, new_weight))
        synapse.set_weight(new_weight)
    
    def getName(self):
        return "CustomPlasticity"
```

### 2. Multi-Region Brain Architecture

```python
import pynlm

# Create a multi-region brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add different regions with specialized functions
region_cortex = brain.addRegion("cortex")
region_hippo = brain.addRegion("hippocampus")
region_amygdala = brain.addRegion("amygdala")

# Configure regional properties
brain.configureRegion(region_cortex, neuron_type=pynlm.NeuronType.Excitatory,
                     density=0.15, connection_probability=0.12)
brain.configureRegion(region_hippo, neuron_type=pynlm.NeuronType.Inhibitory,
                     density=0.05, connection_probability=0.08)
```

### 3. Advanced Sensory Processing

```python
import pynlm
import numpy as np

# Create custom sensory input with temporal dynamics
class DynamicVision:
    def __init__(self, width, height, channels=3):
        self.width = width
        self.height = height
        self.channels = channels
        self.frame_history = []
        self.noise_level = 0.01
        self.motion_sensitivity = 0.5
    
    def create_motion_contrast(self, current_frame, previous_frame):
        """Create motion contrast for obstacle detection"""
        if previous_frame is None:
            return current_frame.copy()
        
        # Compute temporal difference for motion detection
        motion = np.abs(current_frame.astype(float) - previous_frame.astype(float))
        enhanced_motion = np.clip(motion * self.motion_sensitivity, 0, 1)
        
        # Combine with current frame
        result = (current_frame * 0.7) + (enhanced_motion * 0.3)
        return result
    
    def add_gaussian_noise(self, data):
        """Add realistic sensor noise"""
        noise = np.random.normal(0, self.noise_level, data.shape)
        return np.clip(data + noise, 0, 255)

# Use dynamic vision in simulation
vision_system = DynamicVision(10, 10)
```

### 4. Performance Optimization

```python
import pynlm

# Performance optimization configuration
def optimize_brain_performance(brain, optimization_level="high"):
    """Apply performance optimizations based on required quality/speed trade-off"""
    
    if optimization_level == "high":
        # Enable sparse connectivity for faster computation
        brain.config.set("brain.sparsity_threshold", 0.05)
        brain.config.set("brain.enable_vectorization", True)
        brain.config.set("brain.enable_simd", True)
        
        # Reduce memory usage
        brain.config.set("memory.working_memory.capacity", 100)
        brain.config.set("memory.episodic_memory.max_episodes", 1000)
        
    elif optimization_level == "balanced":
        # Balance performance and accuracy
        brain.config.set("brain.sparsity_threshold", 0.08)
        brain.config.set("brain.enable_vectorization", True)
        
    elif optimization_level == "accuracy":
        # Maximum accuracy (slower but more precise)
        brain.config.set("brain.sparsity_threshold", 0.02)
        brain.config.set("brain.enable_vectorization", True)
        brain.config.set("brain.enable_simd", True)
        brain.config.set("brain.use_precise_integration", True)
    
    # Apply optimizations
    brain.optimize(optimization_level)
    return brain

# Memory management and checkpointing
class MemoryManager:
    def __init__(self, brain, checkpoint_dir="."):
        self.brain = brain
        self.checkpoint_dir = checkpoint_dir
        self.checkpoint_interval = 1000
        self.memory_usage = []
    
    def create_checkpoint(self, step):
        """Create a checkpoint for memory management"""
        checkpoint_path = f"{self.checkpoint_dir}/checkpoint_step_{step}.bin"
        if self.brain.save(checkpoint_path):
            self.log_memory_usage(step)
            return checkpoint_path
        return None
    
    def load_checkpoint(self, checkpoint_path):
        """Load from checkpoint"""
        return self.brain.load(checkpoint_path)
    
    def manage_memory(self, step):
        """Manage memory based on usage patterns"""
        if step % self.checkpoint_interval == 0:
            self.create_checkpoint(step)
        
        # Monitor and optimize memory usage
        current_memory = self.brain.getMemoryUsage()
        self.memory_usage.append((step, current_memory))
        
        if current_memory > self.memory_usage[-1][1] * 1.5:  # 50% growth
            self.optimize_memory_usage()
    
    def optimize_memory_usage(self):
        """Optimize memory usage by reducing complexity"""
        # Reduce working memory traces
        self.brain.config.set("memory.working_memory.capacity", 
                            max(50, self.brain.config.getInt("memory.working_memory.capacity") // 2))
        
        # Clean up old episodic memory
        current_episodes = self.brain.config.getInt("memory.episodic_memory.episode_count")
        if current_episodes > 1000:
            self.brain.config.set("memory.episodic_memory.episode_count", 1000)
```

## Troubleshooting Guide

### Common Issues and Solutions

#### Issue 1: "Brain not firing any neurons"

**Causes:**
- Brain not initialized
- Neuron thresholds too high
- No input current

**Solutions:**
```python
# Check initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!

# Check neuron parameters
brain.config.set("brain.v_thresh", -55.0)  # Lower threshold
brain.config.set("brain.v_rest", -70.0)   # Normal resting potential

# Add input current
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
```

#### Issue 2: "Simulation running slowly"

**Causes:**
- Too many neurons/synapses
- No performance optimizations
- Memory usage too high

**Solutions:**
```python
# Enable performance optimizations
brain.optimize("balanced")

# Reduce simulation complexity
brain.config.set("brain.neuron_count", 500)  # Reduce neuron count
brain.config.set("brain.synapse_density", 0.05)  # Reduce connectivity

# Enable memory management
memory_manager = MemoryManager(brain)
```

#### Issue 3: "Memory usage growing uncontrollably"

**Causes:**
- Working memory not being cleaned
- Episodic memory not being pruned
- No checkpoint system

**Solutions:**
```python
# Enable memory management
brain.enableMemoryManagement(True)
brain.setMemoryLimit(1000)  # Maximum memory usage

# Enable checkpointing
brain.enableCheckpointing(True, "checkpoints/")
```

### Debugging Tools

```python
import pynlm

# Debug brain state
class BrainDebugger:
    def __init__(self, brain):
        self.brain = brain
    
    def check_neuron_health(self):
        """Check neuron state and health"""
        total_neurons = self.brain.getTotalNeuronCount()
        firing_neurons = self.brain.getFiringNeuronCount()
        
        print(f"Neuron Health:")
        print(f"  Total neurons: {total_neurons}")
        print(f"  Firing neurons: {firing_neurons}")
        print(f"  Firing rate: {firing_neurons/total_neurons*100:.1f}%")
        
        # Check for potential issues
        if firing_neurons == 0:
            print("  WARNING: No neurons firing - check input currents")
        elif firing_neurons == total_neurons:
            print("  WARNING: All neurons firing - check thresholds")
        
        return firing_neurons > 0 and firing_neurons < total_neurons
    
    def check_plasticity_health(self):
        """Check plasticity system health"""
        # Check if plasticity rules are enabled
        stdp_enabled = self.brain.isSTDPSEnabled()
        hebbian_enabled = self.brain.isHebbianLearningEnabled()
        
        print(f"Plasticity Health:")
        print(f"  STDP enabled: {stdp_enabled}")
        print(f"  Hebbian enabled: {hebbian_enabled}")
        
        if not (stdp_enabled or hebbian_enabled):
            print("  WARNING: No plasticity rules enabled - weights won't change")
        
        return stdp_enabled or hebbian_enabled
    
    def check_memory_health(self):
        """Check memory system health"""
        working_memory = self.brain.getWorkingMemory()
        episodic_memory = self.brain.getEpisodicMemory()
        
        if working_memory:
            active_traces = working_memory.getActiveTraces()
            print(f"Working Memory: {active_traces} active traces")
        
        if episodic_memory:
            episode_count = episodic_memory.getEpisodeCount()
            print(f"Episodic Memory: {episode_count} episodes stored")
        
        return True  # Assume healthy if systems exist
    
    def run_full_debug_check(self):
        """Run complete health check"""
        print("=== NLM Brain Debug Report ===")
        
        health_status = []
        health_status.append(self.check_neuron_health())
        health_status.append(self.check_plasticity_health())
        health_status.append(self.check_memory_health())
        
        print(f"\nOverall Health: {'GOOD' if all(health_status) else 'ISSUES DETECTED'}")
        return all(health_status)

# Use the debugger
debugger = BrainDebugger(brain)
debugger.run_full_debug_check()
```

## Advanced Configuration Examples

### 1. Learning-Focused Configuration

```python
def create_learning_focused_config():
    """Configuration optimized for rapid learning"""
    config = pynlm.createDefaultConfig()
    
    # High learning rates
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.learning_rate", 0.02)
    
    # Enhanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Memory focused configuration
    config.set("memory.working_memory.capacity", 500)
    config.set("memory.episodic_memory.max_episodes", 5000)
    config.set("memory.associative_memory.pattern_count", 1000)
    
    # Brain configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.v_thresh", -55.0)
    
    return config
```

### 2. Real-Time Interactive Configuration

```python
class RealTimeConfigManager:
    """Manage configuration in real-time during simulation"""
    
    def __init__(self, brain):
        self.brain = brain
        self.original_config = brain.getConfig()
        self.config_snapshot = []
        self.auto_save_enabled = True
    
    def adapt_to_behavior(self, behavioral_metrics):
        """Adapt configuration based on agent behavior"""
        
        # Adjust learning rates based on learning speed
        learning_speed = behavioral_metrics.get('learning_speed', 0.0)
        
        if learning_speed > 0.8:
            # Fast learning - reduce learning rates
            self.brain.config.set("plasticity.stdp.learning_rate", 0.001)
            self.brain.config.set("plasticity.hebbian.learning_rate", 0.005)
        elif learning_speed < 0.2:
            # Slow learning - increase learning rates
            self.brain.config.set("plasticity.stdp.learning_rate", 0.01)
            self.brain.config.set("plasticity.hebbian.learning_rate", 0.05)
        
        # Adjust exploration based on behavior entropy
        behavior_entropy = behavioral_metrics.get('behavior_entropy', 0.0)
        
        if behavior_entropy < 0.1:
            # Low exploration - increase curiosity
            self.brain.config.set("neuromod.curiosity.scale", 2.0)
        else:
            # High exploration - reduce curiosity
            self.brain.config.set("neuromod.curiosity.scale", 0.5)
        
        # Save configuration snapshot
        if self.auto_save_enabled:
            self.save_current_config(f"snapshot_{len(self.config_snapshot)}.json")
    
    def save_current_config(self, filename):
        """Save current configuration"""
        self.brain.saveConfig(filename)
        self.config_snapshot.append(filename)
    
    def load_config_snapshot(self, filename):
        """Load configuration from snapshot"""
        self.brain.loadConfig(filename)
    
    def revert_to_original(self):
        """Revert to original configuration"""
        self.brain.loadConfig(self.original_config)
```

### 3. Performance Profiling

```python
import pynlm
import time

class PerformanceProfiler:
    """Profile and optimize NLM performance"""
    
    def __init__(self, brain):
        self.brain = brain
        self.performance_data = []
    
    def profile_step(self, step):
        """Profile a single simulation step"""
        start_time = time.time()
        
        # Run one simulation step
        self.brain.step(step)
        
        end_time = time.time()
        step_time = end_time - start_time
        
        # Collect performance metrics
        metrics = {
            'step': step,
            'step_time': step_time,
            'neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'memory_usage': self.brain.getMemoryUsage(),
            'cpu_usage': self.brain.getCPUUsage(),
            'gpu_usage': self.brain.getGPUUsage() if hasattr(self.brain, 'getGPUUsage') else 0.0
        }
        
        self.performance_data.append(metrics)
        return metrics
    
    def analyze_performance(self):
        """Analyze performance data and provide recommendations"""
        if not self.performance_data:
            return
        
        print("=== Performance Analysis ===")
        
        # Calculate statistics
        step_times = [m['step_time'] for m in self.performance_data]
        avg_step_time = sum(step_times) / len(step_times)
        min_step_time = min(step_times)
        max_step_time = max(step_times)
        
        print(f"Step Time:")
        print(f"  Average: {avg_step_time:.4f}s")
        print(f"  Min: {min_step_time:.4f}s")
        print(f"  Max: {max_step_time:.4f}s")
        
        # Performance bottlenecks
        slow_steps = [m for m in self.performance_data if m['step_time'] > avg_step_time * 1.5]
        if slow_steps:
            print(f"\nPerformance Bottlenecks (>{avg_step_time*1.5:.4f}s): {len(slow_steps)} steps")
            
            # Identify patterns in slow steps
            slow_step_indices = [m['step'] for m in slow_steps]
            print(f"  Slow steps at: {slow_step_indices[:5]}...")
        
        # Memory usage trends
        memory_usages = [m['memory_usage'] for m in self.performance_data]
        avg_memory = sum(memory_usages) / len(memory_usages)
        print(f"\nMemory Usage:")
        print(f"  Average: {avg_memory:.2f} MB")
        print(f"  Min: {min(memory_usages):.2f} MB")
        print(f"  Max: {max(memory_usages):.2f} MB")
        
        # Recommendations
        if avg_step_time > 0.1:
            print(f"\nRecommendations:")
            print(f"  - Reduce neuron count or synapse density")
            print(f"  - Enable performance optimizations")
            print(f"  - Consider using specialized hardware")
        
        if avg_memory > 500:
            print(f"  - Enable memory management and checkpointing")
            print(f"  - Reduce working memory capacity")
            print(f"  - Implement episodic memory pruning")
    
    def profile_configuration(self, config_name, num_steps=100):
        """Profile a specific configuration"""
        print(f"\nProfiling configuration: {config_name}")
        print(f"Running {num_steps} simulation steps...")
        
        for step in range(num_steps):
            self.profile_step(step)
        
        self.analyze_performance()
        
        # Save profile results
        self.save_profile(f"{config_name}_profile.json")
    
    def save_profile(self, filename):
        """Save performance profile to file"""
        import json
        with open(filename, 'w') as f:
            json.dump(self.performance_data, f, indent=2)
        print(f"Profile saved to: {filename}")
```

## Appendix: API Reference

### Core Classes

#### Brain
- `createBrain(config)` - Create a new brain
- `initialize()` - Initialize the brain
- `step(step_number)` - Run one simulation step
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

#### Config
- `createDefaultConfig()` - Create default configuration
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value
- `loadFromFile(filepath)` - Load from file
- `saveToFile(filepath)` - Save to file

#### AgentBrain
- `createAgentBrain(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted)` - Apply reward

#### SimpleWorld
- `createSimpleWorld()` - Create simple world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world
- `update(timestep)` - Update world
- `applyMotorCommand(action, time)` - Apply motor command

### Enumerations

#### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Modulatory` - Modulatory neurons
- `Internal` - Internal neurons

#### ActionType
- `MoveForward`, `MoveBackward` - Movement actions
- `TurnLeft`, `TurnRight` - Rotation actions
- `Look`, `LookUp`, `LookDown` - Looking actions
- `Interact`, `Eat`, `Drink` - Interaction actions
- `Rest`, `Wait` - Rest actions
- `Custom` - Custom actions

#### DevelopmentalStage
- `Initial` - Initial stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### System Status Commands

```python
# Check system status
brain.debug_check()  # Run full health check
world.status()       # Check world status
agent.status()       # Check agent status

# Performance monitoring
profiler = PerformanceProfiler(brain)
profiler.profile_configuration("test_config", 1000)

# Memory management
memory_manager = MemoryManager(brain)
memory_manager.create_checkpoint(1000)
memory_manager.manage_memory(step)
```

## Conclusion

The NLM system provides a comprehensive framework for implementing advanced neural computation with biologically realistic dynamics. This improved version includes:

1. **Enhanced Learning Mechanisms** - Real STDP, Hebbian, and reward-modulated learning
2. **Advanced Neural Dynamics** - Realistic neuron dynamics with short-term plasticity
3. **System Integration** - Complete integration of memory, neuromodulation, and development
4. **Performance Optimizations** - Configurable performance trade-offs
5. **Comprehensive Documentation** - Detailed guides for all usage levels
6. **Troubleshooting Support** - Extensive debugging and troubleshooting tools

The system is now suitable for:
- **Beginners**: Simple neural simulation with easy-to-use APIs
- **Researchers**: Advanced customization and experimentation
- **Developers**: Extension and integration into larger systems
- **Performance-Critical Applications**: Configurable optimizations for different use cases

For further assistance or custom implementations, refer to the full documentation or contact the development team.