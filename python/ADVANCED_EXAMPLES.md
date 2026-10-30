# Advanced NLM Python Examples

## Example 1: Advanced Brain Simulation

```python
import pynlm_advanced as nlm

# Create advanced brain with custom configuration
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Configure advanced settings
brain.configure({
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "memory.enable": True,
    "prediction.enable": True,
})

brain.initialize()

# Advanced simulation with callbacks
def progress_callback(step, brain_obj):
    activity = brain_obj.getFiringNeuronCount() / brain_obj.getTotalNeuronCount()
    if step % 100 == 0:
        print(f"Step {step}: {activity:.2%} neurons firing")

brain.runEpisode(1000, progress_callback)

# Export neural data
brain.exportNeuralData("simulation_results.csv")
```

## Example 2: Advanced Agent Training

```python
import pynlm_advanced as nlm

# Create agent brain interface
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
world.reset()

# Create advanced learning environment
agent.createLearningEnvironment(world, "reinforcement_learning", {
    "curiosity_level": 0.8,
    "reward_scaling": 1.0,
    "exploration_rate": 0.3
})

# Advanced training with progress monitoring
def training_progress(episode, total_reward, steps):
    print(f"Episode {episode}: Reward={total_reward:.2f}, Steps={steps}")

episode_rewards, episode_steps = agent.trainAgent(100, training_progress)

# Analyze training results
print(f"Average reward: {sum(episode_rewards) / len(episode_rewards):.2f}")
print(f"Average episode length: {sum(episode_steps) / len(episode_steps):.2f}")

# Evaluate on test scenarios
test_inputs = [0.1, 0.3, 0.5, 0.7, 0.9]
performance, completion = agent.evaluateAgent(test_inputs)
```

## Example 3: Advanced Plasticity Control

```python
import pynlm_advanced as nlm

# Create brain with custom plasticity rules
config = nlm.AdvancedConfig.createFromTemplate("balanced")
brain = nlm.createBrain(config)
brain.initialize()

# Configure advanced STDP parameters
stdp = nlm.STDP()
stdp.configure(ltpWeight=0.02, ltdWeight=0.015, tau=25.0)

# Apply custom plasticity rules
for region in brain.getRegions():
    for synapse in region.getSynapses():
        synapse.enablePlasticity(True, True, False)  # Hebbian + STDP

# Run learning experiment with custom plasticity
inputs = nlm.createSyntheticDataset(1000, 50, "medium")
expected = [0.1 * i for i in range(len(inputs))]

results = brain.runBatchSimulation(500, inputs, expected)

# Analyze synaptic changes
synapse_stats = brain.getSynapseStatistics()
print(f"Synaptic strengthening: {synapse_stats['strengthened']}%")
print(f"Synaptic weakening: {synapse_stats['weakened']}%")
print(f"Plasticity efficiency: {synapse_stats['efficiency']:.2%}")
```

## Example 4: Advanced Performance Benchmarking

```python
import pynlm_advanced as nlm

# Create benchmark brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Set up performance monitoring
monitor = nlm.PerformanceMonitor()
monitor.startMonitoring()

# Run different test suites
test_results = {}
for test_type in ["step_time", "spike_processing", "memory_usage"]:
    metrics = brain.benchmarkBrain(1000, test_type)
    test_results[test_type] = metrics
    monitor.logTime(test_type, metrics[1])  # Log step time

monitor.stopMonitoring()
final_metrics = monitor.getMetrics()

print("=== Performance Benchmark Results ===")
print(f"Peak memory usage: {final_metrics[0]:.2f} MB")
print(f"Average step time: {final_metrics[1]:.6f} seconds")
print(f"Spikes per step: {final_metrics[2]:.2f}")
print(f"CPU usage: {final_metrics[3]:.1f}%")
```

## Example 5: Advanced Configuration Management

```python
import pynlm_advanced as nlm

# Create configuration from template
config = nlm.AdvancedConfig.createFromTemplate("high_performance")

# Generate random configuration
random_config = nlm.AdvancedConfig.generateRandomConfig(minNeurons=1000, maxNeurons=10000)

# Validate configuration
errors, warnings = config.validateConfiguration()
if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")

# Save and load advanced sessions
brain = nlm.createBrain(config)
brain.initialize()

brain.saveSession("experiment_1", saveConfig=True, saveWeights=True, saveState=True)

# Load session (simulate)
# loaded_config, loaded_state = brain.loadSession("experiment_1")
```

## Advanced API Reference

### Brain Class Methods

- `brain.runEpisode(steps, callback)`: Run simulation with progress callback
- `brain.runBatchSimulation(steps, inputs, expected_outputs)`: Run batch learning
- `brain.getActivityPattern(samples)`: Get neural activity over time
- `brain.saveSession(prefix, saveConfig, saveWeights, saveState)`: Save brain state
- `brain.loadSession(filepath, loadConfig, loadState)`: Load brain state
- `brain.exportNeuralData(filename, regions, timePoints)`: Export to CSV

### AdvancedConfig Class

- `createFromTemplate(name)`: Create config from predefined template
- `validateConfiguration()`: Validate config and return errors/warnings
- `generateRandomConfig(minNeurons, maxNeurons)`: Generate random config
- `saveToFile(filepath)`: Save configuration
- `loadFromFile(filepath)`: Load configuration

### AdvancedAgentBrain Class

- `createLearningEnvironment(world, taskType, parameters)`: Create specialized environment
- `trainAgent(episodes, progressCallback)`: Train agent with progress tracking
- `evaluateAgent(testInputs)`: Evaluate agent performance

### PerformanceMonitor Class

- `startMonitoring()`: Start performance monitoring
- `stopMonitoring()`: Stop monitoring
- `getMetrics()`: Get current metrics
- `logTime(operation, duration)`: Log operation time
- `logMemory(memoryUsage)`: Log memory usage

### Utility Functions

- `generateNeuralNetworkTopology(neuronCount, topologyType, connectionProbability)`: Generate topology config
- `createSyntheticDataset(samples, features, noiseLevel)`: Create synthetic dataset
- `benchmarkBrain(brain, steps, testType)`: Benchmark brain performance
- `compareBrains(brains, testSuites)`: Compare multiple brains

## Installation

```bash
# Install advanced bindings
git submodule add https://github.com/your-repo/pynlm-advanced.git python_advanced
cd python_advanced
cmake -B build -S .
cmake --build build
pip install -e .

# Use in your scripts
import pynlm_advanced as nlm

# All advanced features are now available
brain = nlm.createBrain(nlm.createDefaultConfig())
```

## Configuration Templates

### Minimal
```json
{
    "brain.neuron_count": 100,
    "brain.synapse_density": 0.05,
    "plasticity.stdp.enable": true,
    "plasticity.hebbian.enable": false
}
```

### Balanced
```json
{
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": true,
    "plasticity.stdp.learning_rate": 0.001,
    "plasticity.hebbian.enable": true,
    "plasticity.hebbian.learning_rate": 0.001,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true,
    "memory.enable": true,
    "prediction.enable": true
}
```

### High Performance
```json
{
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.2,
    "plasticity.stdp.enable": true,
    "plasticity.stdp.learning_rate": 0.002,
    "plasticity.hebbian.enable": true,
    "plasticity.hebbian.learning_rate": 0.001,
    "neuromod.curiosity.enable": true,
    "neuromod.novelty.enable": true,
    "memory.enable": true,
    "prediction.enable": true,
    "plasticity.structural.enable": true,
    "development.enable": true
}
```
