# Easy Usage Guide

This guide provides a quick start for getting started with NLM (Neural Learning Machine).

## Quick Start

### What is NLM?

NLM is an experimental artificial developmental brain system that simulates neural computation using Leaky Integrate-and-Fire (LIF) neuron dynamics. It includes:

- **Neural computation**: 2000+ neuron simulation
- **Multiple memory systems**: Working, episodic, semantic, procedural, associative memory
- **Neuromodulation**: Reward-based learning and curiosity-driven exploration
- **Cognition**: Neural planning, concept formation, attention
- **Development**: Synaptogenesis, pruning, maturation
- **Agent-system**: Integration between brain and environment
- **Environment**: Simple 2D world simulation
- **Experiments**: Phase demonstrations (Phase3-Phase6)

### Prerequisites

```bash
python --version >= 3.8
pip install nlm
```

### First Example: Creating a Simple Neural Network

```python
import nlm

# Create NLM system
nlm_system = nlm.NLM()

# Create a simple brain
brain = nlm_system.create_brain({
    "regions": 2,
    "neurons_per_region": 20,
    "learning_rate": 0.01
})

# Run a simulation
results = nlm_system.run_simulation(steps=100)

print(f"Simulation completed!")
print(f"Steps: {results.get('steps', 0)}")
print(f"Final activity: {results.get('activity', 0):.3f}")
```

### Second Example: Advanced Configuration

```python
import nlm

# Create advanced brain configuration
brain_config = nlm.BrainConfig()

# Configure neurons
brain_config.set("neuron_type", "lif")
brain_config.set("threshold", -50.0)
brain_config.set("resting_potential", -65.0)
brain_config.set("refractory_period", 10)

# Configure synapses
brain_config.set("excitatory_strength", 0.15)
brain_config.set("inhibitory_strength", 0.05)

# Create brain with custom configuration
brain = nlm.create_brain(brain_config)

# Add memory systems
memory_system = nlm.EpisodicMemory()
memory_system.initialize(brain)

# Add learning
learning_controller = nlm.LearningController()
learning_controller.setup(brain, memory_system)
```

### Third Example: Working with Memory

```python
import nlm

# Create memory system
memory = nlm.create_memory_system({
    "max_episodes": 500,
    "consolidation_threshold": 0.8,
    "working_memory_capacity": 50
})

# Store an experience
episode = nlm.EpisodicMemoryItem()
episode.timestamp = 100
episode.action = nlm.ActionType::MoveForward
episode.reward = 1.0

# Store to memory
memory.store_episode(episode)

# Retrieve similar experiences
experiences = memory.retrieve_similar(
    sensory_pattern=[0.1, 0.5, 0.9, 0.3, 0.7],
    max_results=5
)

print(f"Found {len(experiences)} similar experiences")
```

## Command Line Interface

### Basic Commands

```bash
# List available commands
nlm help

# Create a new brain
nlm create brain --regions 3 --neurons 100

# Run simulation
nlm run simulation --steps 500 --config brain_config.json

# Monitor performance
nlm monitor status

# Save results
nlm save results --output experiment_results.json
```

### Advanced Commands

```bash
# Validate configuration
nlm config validate configs/neural_network.json

# Convert between formats
nlm config convert input.yaml output.json

# Merge configurations
nlm config merge config1.json config2.json merged.json

# Show configuration statistics
nlm config stats configs/complex.json

# Create environment
nlm env create --name my_world --width 100 --height 100

# Run agent learning
nlm agent learn --brain my_brain --episodes 1000 --reward_threshold 0.8

# Evaluate agent performance
nlm agent evaluate --brain my_brain --test_scenarios scenario1 scenario2
```

## Python API Examples

### High-Level API

```python
import nlm

# High-level interface
nlm_system = nlm.NLM()

# Create and configure brain
brain = nlm_system.create_brain({
    "regions": 5,
    "neurons_per_region": 100,
    "plasticity": "hebbian"
})

# Add memory
memory = nlm_system.create_memory({
    "type": "episodic",
    "capacity": 1000
})

# Add learning
learning = nlm_system.create_learning({
    "algorithm": "td_learning",
    "learning_rate": 0.01
})

# Run experiment
experiment = nlm_system.create_experiment({
    "brain": brain,
    "memory": memory,
    "learning": learning,
    "environment": "simple_world"
})

results = experiment.run(steps=500)
```

### Low-Level API

```python
import nlm

# Low-level component access
neurons = nlm.NeuralNetwork()
neurons.initialize({
    "region_count": 3,
    "neurons_per_region": 50
})

# Configure each neuron
for neuron in neurons.get_all():
    neuron.set_type("lif")
    neuron.set_threshold(-55.0)
    neuron.set_resting_potential(-70.0)

# Create synapses
synapses = nlm.SynapseNetwork()
synapses.connect(neurons.get_region(0), neurons.get_region(1), strength=0.1)

# Create brain
brain = nlm.create_brain({
    "neurons": neurons,
    "synapses": synapses
})
```

## Files and Directories

### Default File Structure

```
projects/
├── my_project/
│   ├── configs/
│   │   ├── brain.json
│   │   ├── memory.yaml
│   │   └── learning.json
│   ├── scripts/
│   │   ├── run_experiment.py
│   │   ├── train_agent.py
│   │   └── evaluate.py
│   ├── results/
│   │   ├── experiment1.json
│   │   ├── experiment2.json
│   │   └── analysis.png
│   └── docs/
│       ├── README.md
│       ├── API_REFERENCE.md
│       └── tutorials/
```

### Configuration Files

#### JSON Configuration

```json
{
    "brain": {
        "regions": 3,
        "neurons_per_region": 100,
        "neuron_type": "lif",
        "threshold": -55.0
    },
    "memory": {
        "max_episodes": 1000,
        "consolidation_threshold": 0.8
    },
    "learning": {
        "algorithm": "td_learning",
        "learning_rate": 0.01
    }
}
```

#### YAML Configuration

```yaml
brain:
  regions: 3
  neurons_per_region: 100
  neuron_type: lif
  threshold: -55.0

memory:
  max_episodes: 1000
  consolidation_threshold: 0.8

learning:
  algorithm: td_learning
  learning_rate: 0.01
```

#### Plain Text Configuration

```
# Brain configuration
brain_regions = 3
neurons_per_region = 100
neuron_type = lif
threshold = -55.0

# Memory configuration
max_episodes = 1000
consolidation_threshold = 0.8

# Learning configuration
algorithm = td_learning
learning_rate = 0.01
```

## Common Operations

### 1. Save and Load Configurations

```python
import nlm

# Save configuration
config = nlm.Config()
config.set("brain_regions", 5)
config.set("neurons_per_region", 100)
config.save("my_config.json", format="json")

# Load configuration
loaded_config = nlm.Config()
loaded_config.load("my_config.json")

# Verify
print(f"Brain regions: {loaded_config.get('brain_regions')}")
print(f"Neurons per region: {loaded_config.get('neurons_per_region')}")
```

### 2. Compare Configurations

```python
import nlm

# Create two configurations
config1 = nlm.Config()
config1.set("brain_regions", 3)
config1.set("learning_rate", 0.01)

config2 = nlm.Config()
config2.set("brain_regions", 5)
config2.set("learning_rate", 0.02)
config2.set("memory_capacity", 1000)

# Compare configurations
comparison = nlm.compare_configs(config1, config2)
print(f"Differences: {comparison.get('differences', [])}")
print(f"Similarities: {comparison.get('similarities', [])}")
```

### 3. Validate Configuration

```python
import nlm

# Create configuration with validation
config = nlm.Config()

# Valid configuration
config.set("brain_regions", 3)
config.set("neurons_per_region", 100)

if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration has errors:")
    for error in config.get_errors():
        print(f"  - {error}")
```

### 4. Run Experiment

```python
import nlm

# Create experiment
experiment = nlm.create_experiment({
    "brain_config": {
        "regions": 3,
        "neurons_per_region": 100
    },
    "memory_config": {
        "max_episodes": 500
    },
    "learning_config": {
        "algorithm": "td_learning",
        "learning_rate": 0.01
    },
    "environment": "simple_world",
    "duration": 500
})

# Run experiment
results = experiment.run()

# Analyze results
analysis = nlm.analyze_results(results)
print(f"Success rate: {analysis.get('success_rate', 0):.2%}")
print(f"Final performance: {analysis.get('final_performance', 0):.3f}")
```

### 5. Monitor Performance

```python
import nlm

# Create monitor
monitor = nlm.create_monitor()

# Start monitoring
monitor.start()

# Run simulation
results = nlm.run_simulation(steps=100)

# Get metrics
metrics = monitor.stop()
print(f"CPU usage: {metrics.get('cpu_usage', 0):.1%}")
print(f"Memory usage: {metrics.get('memory_usage', 0):.1%}")
print(f"Network traffic: {metrics.get('network_traffic', 0):.1f} MB")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Configuration Not Loading

**Problem**: Configuration file not found or invalid format.

**Solution**:
```bash
# Check if file exists
ls -la configs/my_config.json

# Try different formats
test with config.validate
```

#### Issue 2: Simulation Not Running

**Problem**: Simulation fails to start.

**Solution**:
```python
import nlm

# Check if all components are initialized
if not nlm_system.is_initialized():
    print("Initializing NLM system...")
    nlm_system.initialize()

# Check if brain is configured
if not brain.is_configured():
    print("Configuring brain...")
    brain.configure({
        "regions": 3,
        "neurons_per_region": 100
    })

# Run simulation
results = nlm_system.run_simulation(steps=100)
```

#### Issue 3: Memory Not Working

**Problem**: Memory system not storing or retrieving data.

**Solution**:
```python
import nlm

# Check memory system status
memory_status = memory.get_status()
print(f"Memory status: {memory_status}")

# Try to store and retrieve data
memory.store_episode(episode)
retrieved = memory.retrieve_episode(index)

# Verify retrieval
if retrieved is not None:
    print("Memory retrieval successful!")
else:
    print("Memory retrieval failed!")
```

## Getting Help

### Online Resources

- **Documentation**: https://docs.nlm.ai
- **GitHub**: https://github.com/nlm/nlm-python
- **Community Forum**: https://community.nlm.ai
- **Bug Reports**: https://github.com/nlm/nlm-python/issues

### Local Help

```bash
# Show help for specific commands
nlm help command
nlm help config
nlm help brain

# Show detailed examples
nlm examples
nlm tutorials

# Get version information
nlm --version
```

## Contributing

### Reporting Issues

1. **Check existing issues** before reporting
2. **Provide detailed information**:
   - Steps to reproduce
   - Error messages
   - Configuration files
   - Logs
3. **Use proper format** for bug reports

### Making Contributions

1. **Fork the repository**
2. **Create a branch** for your changes
3. **Follow the code style**
4. **Add tests** for new functionality
5. **Update documentation**
6. **Submit a pull request**

## Support

### Commercial Support

- **Consulting**: Available for custom implementations
- **Training**: On-site and online training sessions
- **Integration**: Custom integration services

### Community Support

- **Discussions**: Ask questions and share experiences
- **Code examples**: Share your implementations
- **Feedback**: Help improve NLM for everyone

## License

NLM is licensed under the MIT License. See LICENSE file for details.

---

*For more information, visit [https://nlm.ai](https://nlm.ai) or contact support@nlm.ai*
