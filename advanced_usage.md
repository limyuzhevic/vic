# NLM - Advanced Usage Guide

## Overview

This guide covers advanced features, command-line options, and power user functionality for the NLM (Neural Learning Machine) framework. This complements the easy usage guide and provides comprehensive coverage for experienced users.

---

## Command-Line Interface

NLM supports extensive command-line configuration for experimentation and debugging.

### Basic Help

```bash
# Show help information
nlm --help

# Show version
nlm --version
```

### Core Options

```bash
# Load configuration from file
nlm --config=my_config.cfg

# Basic simulation configuration
nlm --neuron-count=1000 --simulation-timestep=0.0001

# Enable visualization
nlm --enable-visualization --visualization-width=1200 --visualization-height=800
```

### Advanced Configuration

```bash
# STDP parameters
nlm --stdp-ltp-weight=0.025 --stdp-ltd-weight=0.018 --stdp-tau=25.0

# Neuromodulation settings
nlm --dopamine-sensitivity=0.7 --curiosity-threshold=0.2 --novelty-decay-rate=0.98

# Development parameters
nlm --development-enabled --plasticity-decay-rate=0.9
      --critical-period-start=60.0 --critical-period-end=300.0

# Memory system configuration
nlm --working-memory-size=512 --episodic-memory-capacity=2000
      --semantic-memory-capacity=200

# Performance optimization
nlm --use-simd-optimization --parallel-threads=4 --enable-memory-pools
```

### Experiment Options

```bash
# Phase 3 demonstration
nlm --phase3-demo

# Phase 4 demonstration
nlm --phase4-demo

# Phase 6 integration demo
nlm --phase6-demo

# Stress testing
nlm --enable-stress-test --max-memory-usage=2048

# Benchmarking
nlm --enable-benchmark --stats-file=/tmp/experiment_stats.json

# Debug mode
nlm --enable-debug --verbose
```

---

## Python API - Advanced Features

The Python bindings provide extensive functionality for power users.

### Configuration Management

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Set custom parameters
config.set("random_seed", 12345)
config.set("neuron_count", 2000)
config.set("simulation_timestep", 0.0001)
config.set("connection_probability", 0.2)

# Advanced STDP settings
config.set("stdp_ltp_weight", 0.03)
config.set("stdp_ltd_weight", 0.025)
config.set("stdp_tau", 30.0)

# Neuromodulation configuration
config.set("dopamine_sensitivity", 0.8)
config.set("curiosity_threshold", 0.25)
config.set("novelty_decay_rate", 0.95)

# Development settings
config.set("development_enabled", True)
config.set("plasticity_decay_rate", 0.9)
config.set("critical_period_start", 60.0)

# Memory system
config.set("working_memory_size", 1024)
config.set("episodic_memory_capacity", 5000)
config.set("semantic_memory_capacity", 500)

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
```

### Advanced Agent Configuration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)      # Reward-based learning
agent.enableStructuralPlasticity(True)  # Dynamic synapse growth
agent.enableDevelopment(True)           # Developmental stages
agent.enableCuriosity(True)             # Exploration behavior

# Configure neuromodulation levels
agent.set("dopamine_level", 0.5)
agent.set("curiosity_level", 0.3)
```

### Advanced Experiment Patterns

```python
import pynlm
import time

def advanced_experiment(config_path=None):
    """Run advanced experiment with comprehensive tracking"""
    
    # Load or create configuration
    if config_path:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_path)
    else:
        config = pynlm.createAdvancedConfig()
    
    # Create brain with experiment configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup environment
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Create agent with advanced features
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning
    agent.enableAllFeatures()
    
    # Configure neuromodulation
    agent.setDevelopmentMode(True)
    agent.setStressTestMode(True)
    agent.setBenchmarkMode(True)
    
    # Run experiment with detailed tracking
    start_time = time.time()
    metrics = {
        'spikes': [],
        'rewards': [],
        'memory_usage': [],
        'performance': []
    }
    
    for step in range(config.max_steps):
        # Update world
        world.update(config.timestep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward-based learning
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Track metrics
        metrics['spikes'].append(brain.getTotalSpikeCount())
        metrics['rewards'].append(result.reward)
        metrics['memory_usage'].append(agent.getMemoryUsage())
        metrics['performance'].append(step / (time.time() - start_time))
        
        # Save checkpoint periodically
        if step % config.checkpoint_interval == 0:
            brain.save(f"checkpoint_step_{step}.bin")
        
        # Log progress
        if step % config.log_interval == 0:
            log_experiment_progress(step, metrics, brain, agent, world)
    
    # Save final results
    save_experiment_results(brain, agent, world, metrics, "final_experiment.json")
    
    return metrics, brain, agent, world

def log_experiment_progress(step, metrics, brain, agent, world):
    """Log detailed experiment progress"""
    print(f"\n=== STEP {step} ===")
    print(f"Spikes: {metrics['spikes'][-1]:,}")
    print(f"Total Reward: {sum(metrics['rewards'][-100:]):.3f} (last 100 steps)")
    print(f"Firing Neurons: {brain.getFiringNeuronCount()}")
    print(f"Development Stage: {brain.getDevelopmentalStage()}")
    print(f"Memory Usage: {metrics['memory_usage'][-1]} MB")
    print(f"Performance: {metrics['performance'][-1]:.1f} steps/sec")
    print(f"Neuromodulation - Dopamine: {agent.getNeuromodulationLevel():.3f}, "
          f"Curiosity: {agent.getCuriosityLevel():.3f}, "
          f"Novelty: {agent.getNoveltyLevel():.3f}")

def save_experiment_results(brain, agent, world, metrics, filename):
    """Save comprehensive experiment results"""
    results = {
        'final_state': {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neutrons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel()
        },
        'metrics': metrics,
        'configuration': {
            'neuron_count': brain.getConfig().getOr("neuron_count", 0),
            'simulation_timestep': brain.getConfig().getOr("simulation_timestep", 0.0),
            'connection_probability': brain.getConfig().getOr("connection_probability", 0.0)
        }
    }
    
    import json
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2, default=str)
```

---

## Performance Optimization

### SIMD and Parallel Processing

```python
# Enable SIMD optimizations (if available)
import pynlm
pynlm.enableSIMD(True)

# Configure parallel processing
pynlm.setParallelThreads(4)
pynlm.enableMemoryPools(True)
```

### Memory Management

```python
# Monitor memory usage
memory_usage = agent.getMemoryUsage()
memory_stats = agent.getMemoryStats()

# Optimize for memory-constrained environments
pynlm.optimizeForMemoryConstrained()

# Enable memory pooling for performance
pynlm.enableMemoryPools(True)
```

---

## Debugging and Diagnostics

### Debug Mode

```python
# Enable comprehensive debugging
brain.enableDebugMode(True)
world.enableDebugMode(True)
agent.enableDebugMode(True)

# Set log level
brain.setLogLevel("DEBUG")
world.setLogLevel("DEBUG")
agent.setLogLevel("DEBUG")
```

### Stress Testing

```python
# Run stress test
brain.runStressTest({
    'neuron_multiplier': 2.0,  # Double neuron count
    'max_steps': 10000,        # More steps
    'enable_memory_profiling': True,
    'checkpoint_interval': 500
})

# Monitor system resources
stress_test_results = brain.getStressTestResults()
```

### Benchmarking

```python
# Run comprehensive benchmark
benchmark = pynlm.createBenchmark()

benchmark.addTest('brain_initialization', lambda: brain.initialize())
benchmark.addTest('step_computation', lambda: brain.step(100))
benchmark.addTest('memory_allocation', lambda: agent.getMemoryUsage())
benchmark.addTest('neural_dynamics', lambda: run_simulation_loop())

benchmark.run()
benchmark.printResults()
benchmark.saveResults("benchmark_results.json")
```

---

## Advanced Experimentation

### Custom Development Stages

```python
# Implement custom development stages
class CustomDevelopment:
    def __init__(self, brain):
        self.brain = brain
        self.stage = "initial"
        self.age = 0.0
    
    def update(self, timestep):
        self.age += timestep
        
        if self.age < 30.0:
            self.stage = "rapid_growth"
            self.brain.setPlasticityModifier(1.5)
        elif self.age < 120.0:
            self.stage = "maturation"
            self.brain.setPlasticityModifier(0.8)
        elif self.age < 300.0:
            self.stage = "adulthood"
            self.brain.setPlasticityModifier(0.3)
        else:
            self.stage = "aging"
            self.brain.setPlasticityModifier(0.1)
    
    def getStage(self):
        return self.stage

# Use custom development
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
custom_dev = CustomDevelopment(brain)

for step in range(1000):
    custom_dev.update(0.1)
    brain.step(step)
```

### Advanced Memory Systems

```python
# Configure custom memory parameters
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Working memory
brain.configureWorkingMemory(
    capacity=1024,
    decay_rate=0.95,
    interference_strength=0.5
)

# Episodic memory
brain.configureEpisodicMemory(
    capacity=5000,
    replay_interval=100,
    consolidation_strength=0.8
)

# Semantic memory
brain.configureSemanticMemory(
    capacity=500,
    learning_rate=0.01,
    forgetting_rate=0.001
)

# Associative memory
brain.configureAssociativeMemory(
    pattern_capacity=100,
    association_strength=0.9,
    recall_threshold=0.7
)
```

---

## Configuration File Format

### JSON Configuration

```json
{
    "random_seed": 12345,
    "simulation_timestep": 0.0001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    
    "enable_visualization": false,
    "visualization_width": 800,
    "visualization_height": 600,
    
    "enable_debug_mode": true,
    "log_level": "DEBUG",
    
    "dopamine_sensitivity": 0.5,
    "curiosity_threshold": 0.3,
    "novelty_decay_rate": 0.99,
    
    "development_enabled": true,
    "plasticity_decay_rate": 0.95,
    "critical_period_start": 60.0,
    "critical_period_end": 300.0,
    
    "working_memory_size": 256,
    "episodic_memory_capacity": 1000,
    "semantic_memory_capacity": 100,
    
    "enable_memory_pools": true,
    "parallel_threads": 0
}
```

### Configuration API

```python
import pynlm
import json

# Load configuration from JSON
with open('config.json', 'r') as f:
    config_data = json.load(f)

config = pynlm.createDefaultConfig()
for key, value in config_data.items():
    config.set(key, value)

# Save configuration to JSON
with open('my_config.json', 'w') as f:
    json.dump(dict(config), f, indent=2)

# Validate configuration
validation_result = config.validate()
if not validation_result.is_valid:
    print("Configuration errors:", validation_result.errors)
```

---

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain initialization fails**

```python
# Check configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
print("Neuron count:", brain.getConfig().getOr("neuron_count", 0))
print("Random seed:", brain.getConfig().getOr("random_seed", 0))

# Try with explicit configuration
brain = pynlm.createBrain({
    "random_seed": 12345,
    "neuron_count": 100,
    "simulation_timestep": 0.001
})
```

**Issue: Agent not learning**

```python
# Check neuromodulation settings
agent = pynlm.createAgentBrain(brain)
agent.enableAllFeatures()  # Enable all learning features

# Verify reward modulation
agent.applyRewardModulation(1.0, 0.5)  # Apply reward
print("Prediction error:", agent.getPredictionError())
print("Dopamine level:", agent.getNeuromodulationLevel())
```

**Issue: Memory usage too high**

```python
# Check memory configuration
print("Working memory usage:", agent.getMemoryUsage())
print("Memory stats:", agent.getMemoryStats())

# Optimize memory usage
pynlm.optimizeForMemory()
pynlm.enableMemoryPools(True)
```

**Issue: Simulation too slow**

```python
# Check performance settings
print("SIMD enabled:", pynlm.isSIMDEnabled())
print("Parallel threads:", pynlm.getParallelThreads())
print("Memory pools:", pynlm.isMemoryPoolsEnabled())

# Optimize performance
pynlm.enableSIMD(True)
pynlm.setParallelThreads(4)
pynlm.enableMemoryPools(True)
```

---

## Advanced Examples

### Example 1: Multi-Agent System

```python
import pynlm

def create_multi_agent_system():
    """Create a system with multiple interacting agents"""
    
    # Shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
    
    # Multiple brains
    brains = []
    agents = []
    
    for i in range(3):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        brains.append(brain)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agents.append(agent)
    
    return brains, agents, world

# Run multi-agent simulation
brains, agents, world = create_multi_agent_system()

for step in range(1000):
    world.update(0.1)
    
    # Each agent perceives the world independently
    for agent in agents:
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
    
    # Each brain computes independently
    for i, brain in enumerate(brains):
        brain.step(step)
        action = agents[i].decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Example 2: Hierarchical Learning

```python
import pynlm

class HierarchicalBrain:
    def __init__(self):
        # High-level controller
        self.high_level_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.high_level_brain.initialize()
        
        # Low-level executor
        self.low_level_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.low_level_brain.initialize()
        
        # Connection between levels
        self.connection_strength = 0.5
    
    def processCommand(self, high_level_output):
        # Transfer high-level decisions to low-level
        self.low_level_brain.injectCurrent(high_level_output * self.connection_strength)
    
    def learnFromFeedback(self, reward, error):
        # Learning at both levels
        self.high_level_brain.applyRewardModulation(reward, error)
        self.low_level_brain.applyRewardModulation(reward * 0.5, error * 0.5)

# Use hierarchical system
hierarchical_brain = HierarchicalBrain()

for step in range(500):
    # High-level planning
    high_output = hierarchical_brain.high_level_brain.step(step)
    
    # Transfer to low-level
    hierarchical_brain.processCommand(high_output)
    
    # Low-level execution
    low_output = hierarchical_brain.low_level_brain.step(step)
    
    # Learn from experience
    hierarchical_brain.learnFromFeedback(step % 10, abs(low_output - high_output))
```

### Example 3: Real-Time Control

```python
import pynlm
import time
import threading

class RealTimeController:
    def __init__(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        self.running = False
        self.thread = None
    
    def start(self):
        """Start real-time simulation"""
        self.running = True
        self.thread = threading.Thread(target=self.run_loop)
        self.thread.daemon = True
        self.thread.start()
    
    def stop(self):
        """Stop real-time simulation"""
        self.running = False
        if self.thread:
            self.thread.join()
    
    def run_loop(self):
        """Main real-time loop"""
        last_time = time.time()
        
        while self.running:
            current_time = time.time()
            timestep = current_time - last_time
            last_time = current_time
            
            # Limit timestep to prevent large jumps
            timestep = min(timestep, 0.1)
            
            # Update world
            self.world.update(timestep)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process with agent
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(int(time.time() * 1000))  # Use milliseconds as step counter
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Maintain constant update rate
            time.sleep(0.001)  # Small delay to prevent CPU spinning

# Use real-time controller
controller = RealTimeController()
controller.start()

# Do other work while simulation runs
for i in range(100):
    print(f"Processing other task {i}")
    time.sleep(0.1)

controller.stop()
print("Real-time simulation completed")
```

---

## Migration Guide

### From Basic to Advanced

If you're coming from the basic usage guide, here are the key differences and migration steps:

1. **Configuration**: Use `createAdvancedConfig()` instead of `createDefaultConfig()` for more options
2. **Learning**: Enable all neuromodulation features with `agent.enableAllFeatures()`
3. **Monitoring**: Use `brain.getPerformanceMetrics()` for detailed statistics
4. **Error Handling**: Add try-catch blocks around critical operations
5. **Memory Management**: Enable memory pools with `pynlm.enableMemoryPools(True)`

### Common Pitfalls

**Pitfall 1: Not calling initialize()**
```python
# Wrong:
brain = pynlm.createBrain(config)
brain.step(0)  # Crash!

# Right:
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)  # Works!
```

**Pitfall 2: Not resetting world**
```python
# Wrong (world accumulates state):
world.configure(width=100, height=100)
world.update(0.1)  # State changes
world.update(0.1)  # More changes

# Right (reset for clean state):
world.reset()
world.update(0.1)
```

**Pitfall 3: Not handling configuration errors**
```python
# Wrong:
config = pynlm.createDefaultConfig()
config.loadFromFile('config.json')  # May fail silently

# Right:
config = pynlm.createDefaultConfig()
if not config.loadFromFile('config.json'):
    print("Failed to load config, using defaults")
```

---

## Further Reading

For more advanced topics, refer to:

1. **`docs/ARCHITECTURE.md`** - Detailed system architecture
2. **`docs/SCIENCE.md`** - Scientific background and principles
3. **`docs/ROADMAP.md`** - Future development plans
4. **`HOW_TO_USE.md`** - Comprehensive usage guide
5. **Python PyPI documentation** - API reference and examples

---

This advanced user guide provides comprehensive coverage of all advanced features, debugging techniques, and power user functionality for the NLM framework. Use these resources to build sophisticated neural network simulations and experiments.