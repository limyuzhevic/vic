# NLM (Neural Learning Machine) - Enhanced Documentation

**An Experimental Artificial Developmental Brain**

## New High-Level Python API

NLM now provides a comprehensive high-level Python API that makes it easy to:

- Create and train neural networks
- Simulate complex brain-environment interactions
- Analyze neural activity patterns
- Visualize results
- Export models for analysis

### Quick Start Examples

#### 1. Basic Simulation

```python
import pynlm

# Create everything with one line
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Your brain thought {brain.getTotalSpikeCount()} times!")
```

#### 2. Complete Agent Simulation

```python
import pynlm
import matplotlib.pyplot as plt

# Setup configuration and create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation with high-level function
simulation_results = pynlm.run_simulation(brain, world, agent, 500)

# Plot results
steps = [r["step"] for r in simulation_results]
rewards = [r.get("reward", 0.1) for r in simulation_results]
firing_rates = [r["average_firing_rate"] for r in simulation_results]

data, = plt.plot(steps, rewards)
data.set_label("Reward")
plt.plot(steps, firing_rates, label="Firing Rate")
plt.xlabel("Step")
plt.ylabel("Value")
plt.legend()
plt.title("Agent Simulation Results")
plt.show()
```

#### 3. Train a Brain

```python
import pynlm

# Create experimental configuration
config = pynlm.create_experimental_config()

# Train a brain with custom settings
trained_brain = pynlm.create_trained_brain(config, training_episodes=100, steps_per_episode=200)

# Evaluate the trained brain
performance = pynlm.evaluate_brain(trained_brain, num_scenarios=10)

print("Training Results:")
print(f"  Average Performance: {performance['avg_performance']:.4f}")
print(f"  Max Performance: {performance['max_performance']:.4f}")
print(f"  Min Performance: {performance['min_performance']:.4f}")
```

#### 4. Save and Load Models

```python
import pynlm

# Create and train a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Train the brain (using a simple training example)
world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Train for a short time
pynlm.train_agent(brain, world, agent, episodes=10, steps_per_episode=50)

# Save the trained model
pynlm.save_model(brain, "my_trained_brain.nlm")

# Later, load the model
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
pynlm.load_model(loaded_brain, "my_trained_brain.nlm")
loaded_brain.initialize()
```

#### 5. Advanced Analysis

```python
import pynlm

# Create a brain and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for i in range(1000):
    brain.step(i)

# Analyze neural patterns
patterns = pynlm.analyze_patterns(brain)

print("Neural Pattern Analysis:")
for key, value in patterns.items():
    print(f"  {key}: {value:.4f}")

# Detect anomalies
anomalies = pynlm.detect_anomalies(brain)

print("Anomaly Detection:")
for key, value in anomalies.items():
    if value > 0:
        print(f"  {key}: DETECTED (score: {value:.4f})")

# Get comprehensive performance summary
summary = pynlm.summarize_performance(brain)

print("Performance Summary:")
for key, value in summary.items():
    print(f"  {key}: {value:.4f}")
```

#### 6. Custom Reward Functions

```python
import pynlm

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(15, 15, 6, 6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Custom reward function
def custom_reward(reward, predicted, state):
    # Add bonus for exploration
    exploration_bonus = 0.5 if agent.getCuriosityLevel() > 0.7 else 0.0
    
    # Add penalty for high energy consumption
    energy_penalty = 0.1 if state.get("energy", 1.0) < 0.5 else 0.0
    
    return reward + exploration_bonus - energy_penalty

# Add custom reward callback
pynlm.add_reward_callback(agent, custom_reward)

# Run simulation
pynlm.run_simulation(brain, world, agent, 200)
```

## New High-Level Functions Reference

### Core Functions

#### `run_simulation(brain, world, agent, steps)`
**Complete simulation loop with automatic world updates, sensory processing, and action execution**

- **Parameters**:
  - `brain`: Brain object
  - `world`: SimpleWorld object  
  - `agent`: AgentBrain object
  - `steps`: Number of simulation steps
- **Returns**: List of step results with rewards and observations
- **Example**: See above

#### `train_agent(brain, world, agent, episodes, steps_per_episode)`
**Training loop with episode management and experience accumulation**

- **Parameters**:
  - `brain`: Brain object
  - `world`: SimpleWorld object
  - `agent`: AgentBrain object  
  - `episodes`: Number of training episodes
  - `steps_per_episode`: Steps per episode
- **Returns**: Training metrics and performance history
- **Example**: See above

#### `create_trained_brain(config, training_episodes, steps_per_episode)`
**Factory function that creates, initializes, and trains a brain**

- **Parameters**:
  - `config`: Brain configuration
  - `training_episodes`: Number of training episodes
  - `steps_per_episode`: Steps per episode
- **Returns**: Trained brain object
- **Example**: See above

#### `evaluate_brain(brain, num_scenarios)`
**Evaluate brain performance on predefined test scenarios**

- **Parameters**:
  - `brain`: Brain object
  - `num_scenarios`: Number of scenarios to test
- **Returns**: Performance metrics, success rates, and comparison data
- **Example**: See above

### Model Persistence

#### `save_model(brain, filepath, compress=True)`
**Save trained brain model to file**

- **Parameters**:
  - `brain`: Brain object to save
  - `filepath`: Path to save file
  - `compress`: Enable compression (default: True)
- **Returns**: True if successful, raises exception otherwise

#### `load_model(brain, filepath)`
**Load saved brain model from file**

- **Parameters**:
  - `brain`: Brain object to load into
  - `filepath`: Path to load file
- **Returns**: True if successful, raises exception otherwise

#### `export_weights(brain)`
**Export synaptic weights as JSON for external analysis**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: JSON string with weight data

### Advanced Agent Behaviors

#### `enable_learning(agent, enabled)`
**Enable or disable agent learning capabilities**

- **Parameters**:
  - `agent`: AgentBrain object
  - `enabled`: True to enable, False to disable

#### `set_exploration_rate(agent, rate)`
**Set exploration-exploitation balance**

- **Parameters**:
  - `agent`: AgentBrain object
  - `rate`: Exploration rate (0.0 to 1.0)

#### `add_reward_callback(agent, callback)`
**Add custom reward function for dynamic reward shaping**

- **Parameters**:
  - `agent`: AgentBrain object
  - `callback`: Function that receives reward, predicted reward, and state

### Visualization Support

#### `plot_activity(brain)`
**Create neural activity plots**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Matplotlib figure object

#### `plot_connectivity(brain)`
**Create connectivity matrix visualizations**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Matplotlib figure object

#### `plot_development(brain)`
**Plot developmental changes over time**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Matplotlib figure object

### Performance Monitoring

#### `get_performance_stats(brain)`
**Get timing and memory statistics**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Dictionary with performance metrics

#### `benchmark(brain)`
**Run performance benchmarks across different operations**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Dictionary with benchmark results

#### `profile(brain, operation)`
**Profile specific operations with detailed timing**

- **Parameters**:
  - `brain`: Brain object
  - `operation`: Name of operation to profile
- **Returns**: Dictionary with profiling results

### Configuration Utilities

#### `create_experimental_config()`
**Create configuration optimized for research experiments**

- **Returns**: Config object with research-oriented settings

#### `create_production_config()`
**Create configuration optimized for production use**

- **Returns**: Config object with production-oriented settings

#### `optimize_config(config)`
**Automatically optimize configuration parameters**

- **Parameters**:
  - `config`: Config object to optimize
- **Returns**: Optimization report string

### Advanced Analysis Functions

#### `analyze_patterns(brain)`
**Analyze neural firing patterns and statistical properties**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Dictionary with pattern analysis results

#### `detect_anomalies(brain)`
**Detect unusual neural activity patterns or system anomalies**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Dictionary with anomaly scores

#### `summarize_performance(brain)`
**Generate comprehensive performance summary**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: Dictionary with performance summary

### Error Handling

#### `validate_config(config)`
**Validate configuration parameters with descriptive error messages**

- **Parameters**:
  - `config`: Config object to validate
- **Returns**: Success message or raises exception

#### `check_brain_initialization(brain)`
**Check if brain is properly initialized before operations**

- **Parameters**:
  - `brain`: Brain object
- **Returns**: True if initialized, raises exception otherwise

#### `safe_operation(operation, error_handler)`
**Execute operation with customizable error handling**

- **Parameters**:
  - `operation`: Function to execute
  - `error_handler`: Optional error handler function
- **Returns**: Operation result or error

## Installation and Usage

### Installation

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.4f}")
```

### Advanced Usage

```python
import pynlm

# Create experimental setup
config = pynlm.create_experimental_config()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(30, 30, 10, 10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
pynlm.enable_learning(agent, True)
pynlm.set_exploration_rate(agent, 0.3)

# Train with custom callback
def my_reward_callback(reward, predicted, state):
    return reward * 1.5  # Boost rewards

pynlm.add_reward_callback(agent, my_reward_callback)

# Run training
results = pynlm.train_agent(brain, world, agent, episodes=50, steps_per_episode=200)

# Analyze results
patterns = pynlm.analyze_patterns(brain)
anomalies = pynlm.detect_anomalies(brain)
summary = pynlm.summarize_performance(brain)

print(f"Training completed. Max reward: {results['max_performance']:.4f}")
print(f"Patterns analyzed: {len(patterns)}")
print(f"Anomalies detected: {sum(1 for v in anomalies.values() if v > 0)}")
print(f"Performance score: {summary['overall_health']:.4f}")
```

## Performance Tips

1. **Memory Optimization**: For large simulations, use `create_production_config()` which optimizes for performance
2. **Checkpointing**: Enable checkpointing in production configs to save progress periodically
3. **Parallel Processing**: The NLM core uses SIMD vectorization and parallel processing for better performance
4. **Batch Processing**: For training, consider using `train_agent()` with larger episode counts

## Common Issues and Solutions

### Issue: Brain not firing neurons
**Solution**: 
- Ensure the brain is properly initialized (`brain.initialize()`)
- Check that neurons have appropriate membrane potentials
- Verify that synaptic connections are established

### Issue: Agent not responding to world
**Solution**:
- Ensure agent is initialized with world (`agent.initialize(world)`)
- Check that sensory neurons are receiving input
- Verify that motor neurons have sufficient activity

### Issue: Training is slow
**Solution**:
- Use `create_production_config()` for better performance
- Reduce simulation steps for initial testing
- Monitor performance with `benchmark()`

### Issue: Visualization not working
**Solution**:
- Ensure matplotlib is installed: `pip install matplotlib`
- Check that required data is available for plotting
- Use `brain.getTotalNeuronCount()` to verify brain is properly initialized

## Backward Compatibility

The enhanced Python bindings maintain full backward compatibility with existing code:

```python
# Old code still works perfectly
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
```

All original functions and their behavior remain unchanged. The new functions are additional convenience methods that simplify common tasks.

## Further Reading

- **NLM Documentation**: Check the `docs/` directory for detailed scientific background
- **Architecture**: Read `docs/ARCHITECTURE.md` for system design details
- **Experiments**: Review `docs/EXPERIMENTS.md` for example applications
- **Phase 6 Audit**: See `docs/PHASE6_FINAL_AUDIT.md` for integration test results

## Support

For support with the enhanced Python API:
1. Check the examples above for common usage patterns
2. Review the function documentation for parameter details
3. Contact the NLM development team for custom implementations

The enhanced NLM Python bindings provide a powerful, user-friendly interface to the world's most comprehensive artificial developmental brain system.
