# Practical Guide to Enhanced NLM Python API

This guide provides practical examples and workflows for using the enhanced NLM Python API.

## Getting Started with Enhanced API

### 1. Basic Enhanced Setup

```python
import pynlm
from advanced_nlm_tools import PerformanceMonitor, ExpertTools

print("=== Enhanced NLM Python API Demo ===")

# Use enhanced convenience functions
print("\n1. Creating enhanced training setup...")
config = pynlm.createTrainingConfig("reinforcement_learning", 2000)
brain = pynlm.createTrainingBrain(500, 3, 0.001)
brain.initialize()

world = pynlm.createTrainingWorld(80, 80)
agent = pynlm.createExpertAgent(brain, world)

print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"   World size: {world.getWidth()}x{world.getHeight()}")
print(f"   Agent learning enabled: Reward={agent.isRewardModulationEnabled()}, "
      f"Dev={agent.isDevelopmentEnabled()}")
```

### 2. Performance Monitoring

```python
# Setup performance monitoring
monitor = PerformanceMonitor()
monitor.start()

print("\n2. Running simulation with performance monitoring...")
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if agent.isRewardModulationEnabled():
        agent.applyRewardModulation(1.0, 0.5)
    
    monitor.step(brain, world, agent)

# Get performance metrics
metrics = monitor.get_metrics()
print(f"   Simulation completed: {metrics['steps']} steps in {metrics['duration']:.1f}s")
print(f"   Average firing rate: {metrics.get('firing_rate_mean', 0):.2f}")
print(f"   Average curiosity: {metrics.get('curiosity_mean', 0):.3f}")
```

### 3. Memory Management

```python
from advanced_nlm_tools import MemoryManager

print("\n3. Memory management demo...")

# Create memory manager with 512MB limit
mem_manager = MemoryManager(max_memory_mb=512)

# Allocate brain with memory tracking
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

print(f"   Memory limit: {mem_manager._MemoryManager__max_memory / (1024*1024):.0f} MB")
print("   Memory management utilities available for large-scale simulations")
```

### 4. Advanced Simulation Control

```python
from advanced_nlm_tools import SimulationController

print("\n4. Advanced simulation control...")

# Create multiple setups for comparison
setups = []
for world_type in ["training", "survival"]:
    brain, world, agent = pynlm.createCompleteSimulationSetup(world_type, 100, True)
    setups.append({
        'world_type': world_type,
        'brain': brain,
        'world': world,
        'agent': agent
    })

# Run adaptive training on each setup
controller = SimulationController()

adaptation_rules = {
    'adapt_curiosity': True,
    'curiosity_threshold': 0.8,
    'apply_reward': True,
    'update_development': True
}

for setup in setups:
    print(f"   Running {setup['world_type']} adaptation...")
    metrics = controller.run_with_adaptation(
        setup['agent'], setup['world'], 300, adaptation_rules
    )
    print(f"     Final curiosity: {metrics['curiosity_trajectory'][-1]:.3f}")
    print(f"     Final development: {metrics['development_trajectory'][-1]}")
```

### 5. Parameter Optimization

```python
from advanced_nlm_tools import ExpertTools
import time

print("\n5. Parameter optimization demo...")

# Create parameter search space
base_config = pynlm.createDefaultConfig()
parameter_ranges = {
    "brain.neuron_count": (200, 1000, 200),
    "brain.region_count": (1, 4, 1),
    "plasticity.stdp.learning_rate": (0.0001, 0.01, 0.002)
}

print("   Generating parameter search space...")
configs = ExpertTools.create_parameter_search_space(base_config, parameter_ranges)
print(f"   Generated {len(configs)} configurations")

# Benchmark brain creation
print("   Benchmarking brain creation...")
benchmark = ExpertTools.benchmark_brain_creation(num_brains=5, neurons=500)
print(f"   Created 5 brains in {benchmark['total_time']:.2f}s "
      f"({benchmark['brains_per_second']:.1f} brains/sec)")
```

### 6. Configuration Validation

```python
print("\n6. Configuration validation...")

# Test various configurations
configs_to_test = [
    pynlm.createDefaultConfig(),
    pynlm.createTrainingConfig("rl", 1000),
    pynlm.createDevelopmentConfig(0.01, True),
    pynlm.createProductionConfig(2000, 0.2)
]

for i, config in enumerate(configs_to_test):
    warnings = ExpertTools.validate_config_compatibility(config)
    if warnings:
        print(f"   Config {i+1} warnings:")
        for warning in warnings:
            print(f"     - {warning}")
    else:
        print(f"   Config {i+1}: No warnings")
```

### 7. Experiment Series

```python
print("\n7. Experiment series demo...")

# Create experiment variations
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 500)

variations = [
    {"brain.neuron_count": 500, "plasticity_rate": 0.01, "development_enabled": True},
    {"brain.neuron_count": 1000, "plasticity_rate": 0.02, "development_enabled": False},
    {"brain.neuron_count": 2000, "plasticity_rate": 0.005, "development_enabled": True}
]

configs = ExpertTools.create_experiment_series(base_config, variations, None)
print(f"   Created {len(configs)} experiment configurations")

# Display configuration summary
for i, config in enumerate(configs):
    neurons = config.get("brain.neuron_count")
    plasticity = config.get("plasticity_rate")
    development = config.get("development_enabled")
    print(f"   Experiment {i+1}: {neurons} neurons, plasticity={plasticity}, "
          f"development={development}")
```

### 8. Reproduction Experiment

```python
from advanced_nlm_tools import run_reproduction_experiment

print("\n8. Reproduction experiment...")

# Setup reproduction baseline
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentWithDefaults(brain, world)

# Run reproduction experiment
results = run_reproduction_experiment(brain, world, agent, 500)

print("   Reproduction experiment results:")
print(f"   Total neurons: {results['baseline']['neurons']}")
print(f"   Total synapses: {results['baseline']['synapses']}")
print(f"   Average firing rate: {results['baseline']['final_firing_rate']:.3f}")
print(f"   Average curiosity: {sum(results['curiosity_trajectory'])/len(results['curiosity_trajectory']):.3f}")
print(f"   Final development stage: {results['development_trajectory'][-1]}")
print(f"   Total steps completed: {len(results['reward_trajectory'])}")
```

### 9. Complete Advanced Workflow

```python
print("\n9. Complete advanced workflow...")

# Step 1: Setup enhanced environment
print("   Step 1: Setting up enhanced environment...")
config = pynlm.createTrainingConfig("reinforcement_learning", 3000)
brain = pynlm.createTrainingBrain(800, 4, 0.001)
brain.initialize()

world = pynlm.createTrainingWorld(100, 100)
agent = pynlm.createExpertAgent(brain, world)

# Step 2: Performance monitoring
print("   Step 2: Starting performance monitoring...")
monitor = PerformanceMonitor()
monitor.start()

# Step 3: Advanced simulation control
print("   Step 3: Running adaptive training...")
controller = SimulationController()

adaptation_rules = {
    'adapt_curiosity': True,
    'curiosity_threshold': 0.85,
    'apply_reward': True,
    'update_development': True
}

# Run adaptive training
metrics = controller.run_with_adaptation(
    agent, world, 1000, adaptation_rules
)

# Step 4: Collect and analyze metrics
print("   Step 4: Analyzing results...")
for step in range(0, 1001, 200):
    step_metrics = monitor.get_metrics()
    print(f"     Step {step}: "
          f"Firing={step_metrics.get('firing_rate_mean', 0):.2f}, "
          f"Curiosity={step_metrics.get('curiosity_mean', 0):.3f}, "
          f"Novelty={step_metrics.get('novelty_mean', 0):.3f}")

# Step 5: Performance summary
print("   Step 5: Performance summary...")
final_metrics = monitor.get_metrics()
print(f"     Total steps: {final_metrics['steps']}")
print(f"     Total duration: {final_metrics['duration']:.1f}s")
print(f"     Steps per second: {final_metrics['steps'] / final_metrics['duration']:.1f}")
print(f"     Average curiosity: {final_metrics.get('curiosity_mean', 0):.3f}")
print(f"     Average novelty: {final_metrics.get('novelty_mean', 0):.3f}")
print(f"     Final development: {metrics['development_trajectory'][-1]}")

# Step 6: Configuration optimization
print("   Step 6: Configuration optimization...")
config = brain.getConfig()
neuron_count = config.get("brain.neuron_count")
region_count = config.get("brain.region_count")
plasticity_rate = config.get("plasticity_rate")

print(f"     Final configuration:")
print(f"       Neurons: {neuron_count}")
print(f"       Regions: {region_count}")
print(f"       Plasticity rate: {plasticity_rate}")
```

### 10. Batch Processing

```python
from advanced_nlm_tools import SimulationController
from concurrent.futures import ThreadPoolExecutor

print("\n10. Batch processing demo...")

# Create multiple training setups
setups = []
for i in range(3):
    config = pynlm.createTrainingBrain(500, 3, 0.001)
    config.set("brain.neuron_count", 500 + i * 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createTrainingWorld(50 + i * 10, 50 + i * 10)
    agent = pynlm.createExpertAgent(brain, world)
    
    setups.append({
        'id': i,
        'brain': brain,
        'world': world,
        'agent': agent
    })

# Run parallel simulations
print("   Running parallel simulations...")
controller = SimulationController()

# Create a simple parallel runner for demo
def run_single_setup(setup, steps):
    world = setup['world']
    agent = setup['agent']
    brain = setup['brain']
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if agent.isRewardModulationEnabled():
            agent.applyMotorCommand(action, world.getSimulationTime())
    
    return {
        'id': setup['id'],
        'neurons': brain.getTotalNeuronCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'steps_completed': steps
    }

# Use ThreadPoolExecutor for parallel execution
with ThreadPoolExecutor(max_workers=len(setups)) as executor:
    futures = []
    for setup in setups:
        future = executor.submit(run_single_setup, setup, 100)
        futures.append(future)
    
    results = []
    for future in futures:
        results.append(future.result())

print("   Batch processing results:")
for result in results:
    print(f"     Setup {result['id']}: {result['neurons']} neurons, "
          f"firing rate={result['final_firing_rate']:.3f}, "
          f"steps={result['steps_completed']}")
```

## Summary of Enhanced API Usage

### When to Use Each Feature

| Feature | Use Case | Example |
|---------|----------|---------|
| `createTrainingConfig` | Training scenarios | `createTrainingConfig("rl", 1000)` |
| `createDevelopmentConfig` | Developmental learning | `createDevelopmentConfig(0.01, True)` |
| `createProductionConfig` | Production environments | `createProductionConfig(2000, 0.2)` |
| `PerformanceMonitor` | Performance analysis | Monitor simulation metrics |
| `MemoryManager` | Memory-constrained systems | Large-scale simulations |
| `SimulationController` | Advanced control | Adaptive training, parallel execution |
| `ExpertTools` | Parameter optimization | Grid search, benchmarking |
| `run_reproduction_experiment` | Scientific validation | Reproducible experiments |

### Typical Workflows

1. **Standard Training**: Use factory functions + PerformanceMonitor
2. **Research Experiments**: Use ExpertTools + configuration validation
3. **Production Deployment**: Use MemoryManager + production configs
4. **Performance Analysis**: Use PerformanceMonitor + comparison tools
5. **Large-Scale Studies**: Use SimulationController + batch processing

## Next Steps

### For Users

1. **Read ADVANCED_PYTHON_API.md** - Comprehensive feature documentation
2. **Run test_pynlm_bindings.py** - Test the enhanced bindings
3. **Try advanced_nlm_tools.py** - Explore expert features
4. **Check easy_usage.md** - Basic usage (updated with new features)
5. **Review docs/ARCHITECTURE.md** - System architecture

### For Developers

1. **Add new factory functions** to bindings.cpp
2. **Extend advanced_nlm_tools.py** with custom tools
3. **Update documentation** with new examples
4. **Add comprehensive tests** to test_pynlm_bindings.py
5. **Improve performance** and memory management

## Troubleshooting Guide

### Common Issues and Solutions

1. **Memory Limit Exceeded**
   ```python
   mem_manager = MemoryManager(max_memory_mb=2048)  # Increase limit
   ```

2. **Performance Issues**
   ```python
   monitor = PerformanceMonitor()
   monitor.start()
   # Run with monitoring to identify bottlenecks
   ```

3. **Configuration Errors**
   ```python
   warnings = ExpertTools.validate_config_compatibility(config)
   for warning in warnings:
       print(f"Warning: {warning}")
   ```

4. **Parallel Execution**
   ```python
   controller = SimulationController()
   results = controller.run_parallel_simulations(configs, worlds, steps)
   ```

### Getting Help

1. **Documentation**: Read ADVANCED_PYTHON_API.md for comprehensive guides
2. **Tests**: Run test_pynlm_bindings.py to validate functionality
3. **Examples**: Try the practical examples in this guide
4. **Source Code**: Examine bindings.cpp and advanced_nlm_tools.py
5. **Community**: Join NLM project for support and discussions

## Conclusion

The enhanced NLM Python API provides:

1. **Simplified Setup** - Factory functions for common use cases
2. **Advanced Monitoring** - Performance tracking and analysis
3. **Expert Tools** - Parallel processing, adaptation, optimization
4. **Better Error Handling** - Validation and informative messages
5. **Comprehensive Documentation** - Practical guides and examples

These enhancements make NLM more accessible to beginners while providing powerful tools for expert users working with large-scale neural simulations.

For the latest features and updates, refer to ADVANCED_PYTHON_API.md and the main NLM documentation.