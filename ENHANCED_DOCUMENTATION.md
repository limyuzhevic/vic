# NLM - Enhanced Documentation and Examples
# Phase 2: Advanced Usage Guide

This document provides comprehensive guidance on using the enhanced NLM (Neural Learning Machine) system, including new features and improvements.

---

## Part 1: New Advanced Features

### 1.1 Advanced Command-Line Interface

The NLM system now includes a powerful command-line interface (`nlm_cli.py`) that provides:

- **Configuration Management**: Create, load, save, and edit configuration files
- **Simulation Control**: Run simulations with advanced parameters
- **Brain Analysis**: Analyze and debug brain states
- **Data Export**: Export simulation data in various formats
- **Batch Processing**: Run multiple simulations efficiently

#### Installation and Setup

```bash
# Install the NLM CLI
touch nlm_cli.py
chmod +x nlm_cli.py

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install pynlm
pip install -e .
```

#### Basic CLI Usage

```bash
# Show help
nlm_cli.py --help

# Show detailed help for a command
nlm_cli.py help create-config

# Create a configuration using an advanced template
nlm_cli.py create-config my-experiment --template advanced

# Run a simulation
nlm_cli.py run-simulation configs/my-experiment.json --steps 5000 --timestep 0.01

# Analyze a brain state
nlm_cli.py analyze-brain checkpoint.bin --verbose
```

### 1.2 Enhanced Python API

The Python bindings have been significantly enhanced with:

- **Comprehensive Configuration System**: Support for JSON, YAML, and JSON5 formats
- **Advanced Brain Interface**: More methods and properties for brain control
- **Improved Error Handling**: Better exception handling and error messages
- **Performance Monitoring**: Built-in benchmarking and performance analysis
- **Advanced Factory Functions**: Specialized configuration creators

#### Advanced Python Usage

```python
import pynlm
import json

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("brain.region_count", 3)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.5)

# Save as JSON
config.saveToFile("advanced_config.json")

# Load from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("advanced_config.json")

# Create brain with advanced configuration
brain = pynlm.createBrain(config2)
brain.initialize()

# Run a complete simulation loop
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Performance analysis
metrics = brain.getPerformanceMetrics()
print(f"Average firing rate: {metrics['average_firing_rate']:.4f}")
print(f"Total spikes: {metrics['total_spikes']}")
print(f"Development stage: {metrics['development_stage']}")
```

### 1.3 JSON/YAML Configuration System

The advanced configuration system supports multiple formats:

#### JSON Configuration

```json
{
  "name": "advanced_brain",
  "brain": {
    "neuron_count": 5000,
    "region_count": 3,
    "connection_probability": 0.1,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0,
      "a_plus": 0.01,
      "a_minus": 0.01
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.0005
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001,
      "max_connections": 10000
    }
  },
  "neuromod": {
    "dopamine": {
      "scale": 1.0,
      "learning_rate": 0.01,
      "tolerance": 0.1
    },
    "curiosity": {
      "enable": true,
      "scale": 0.5,
      "threshold": 0.05
    },
    "novelty": {
      "enable": true,
      "threshold": 0.1
    }
  },
  "development": {
    "enable": true,
    "plasticity_decay_rate": 0.001,
    "critical_period_length": 100.0,
    "maturation_rate": 0.01
  },
  "environment": {
    "width": 20,
    "height": 20,
    "vision_width": 8,
    "vision_height": 8,
    "max_energy": 100.0,
    "energy_decay_rate": 0.01
  },
  "created": "2024-01-20T02:31:34Z"
}
```

#### YAML Configuration

```yaml
name: advanced_brain
brain:
  neuron_count: 5000
  region_count: 3
  connection_probability: 0.1
  initial_weight_mean: 0.5
  initial_weight_std: 0.1
  v_thresh: -50.0
  v_rest: -70.0
  v_reset: -75.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
    a_plus: 0.01
    a_minus: 0.01
  hebbian:
    enable: true
    learning_rate: 0.0005
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001
    max_connections: 10000

neuromod:
  dopamine:
    scale: 1.0
    learning_rate: 0.01
    tolerance: 0.1
  curiosity:
    enable: true
    scale: 0.5
    threshold: 0.05
  novelty:
    enable: true
    threshold: 0.1

development:
  enable: true
  plasticity_decay_rate: 0.001
  critical_period_length: 100.0
  maturation_rate: 0.01

environment:
  width: 20
  height: 20
  vision_width: 8
  vision_height: 8
  max_energy: 100.0
  energy_decay_rate: 0.01

created: "2024-01-20T02:31:34Z"
```

---

## Part 2: Configuration Management

### 2.1 Configuration Templates

The system provides several predefined configuration templates:

#### Basic Template
```python
# Simple configuration for testing and basic usage
config = pynlm.createDefaultConfig()
```

#### Advanced Template
```python
config = pynlm.create_advanced_config()
```

#### Research Template
```python
config = pynlm.create_experiment_config("research")
```

#### Experiment Templates
```python
# Plasticity learning experiment
config = pynlm.create_experiment_config("plastic")

# Curiosity-driven exploration experiment
config = pynlm.create_experiment_config("curiosity")

# Prediction and learning experiment
config = pynlm.create_experiment_config("prediction")
```

### 2.2 Configuration Validation

The system includes comprehensive configuration validation:

```python
# Validate configuration
if not pynlm.validateConfiguration(config):
    print("Configuration is invalid!")
    sys.exit(1)

# Generate configuration report
report = pynlm.generateConfigurationReport(config)
print(report)

# Analyze configuration
analysis = pynlm.analyzeConfiguration(config)
for key, value in analysis.items():
    print(f"{key}: {value}")
```

### 2.3 Configuration File Management

```python
# Backup configuration
nlm::createBackup("experiment1.json", config)

# Load configuration with format detection
loadAdvancedConfig("config.json", config)  # Auto-detects format
loadAdvancedConfig("config.yaml", config, "yaml")  # YAML format
loadAdvancedConfig("config.json5", config, "json5")  # JSON5 format

# Save configuration with format choice
saveAdvancedConfig("experiment1.json", config, "json")
saveAdvancedConfig("experiment1.yaml", config, "yaml")
```

---

## Part 3: Brain Analysis and Debugging

### 3.1 Brain Inspection

```python
# Inspect brain properties
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive statistics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "e_i_ratio": brain.getExcitationInhibitionRatio(),
    "development_stage": brain.getDevelopmentalStage(),
    "most_active_neurons": brain.getMostActiveNeurons(10)
}

# Print brain status
brain.logStatus()
```

### 3.2 Connectivity Analysis

```python
# Analyze brain connectivity patterns
connectivity = pynlm.analyzeConnectivity(brain)
print("Connectivity matrix:")
for region1, targets in connectivity.items():
    print(f"  {region1}:")
    for region2, weight in targets.items():
        print(f"    -> {region2}: {weight:.4f}")
```

### 3.3 Performance Benchmarking

```python
# Benchmark brain performance
spike_count, duration_ms = pynlm.benchmarkBrain(brain, 1000)
print(f"Performance: {spike_count} spikes in {duration_ms}ms")
print(f"Rate: {spike_count/duration_ms:.2f} spikes/ms")
```

---

## Part 4: Advanced Simulation Control

### 4.1 Complete Simulation Loop

```python
def run_complete_simulation(config, steps=1000, timestep=0.1):
    """Run a complete NLM simulation with all features enabled."""
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20, 
        height=20, 
        visionWidth=8, 
        visionHeight=8
    )
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        # Update world
        world.update(timestep)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent
```

### 4.2 Batch Processing

```python
import json
import glob

def run_batch_processing(config_dir="configs", output_dir="results"):
    """Run multiple simulations in batch."""
    config_files = glob.glob(f"{config_dir}/*.json")
    
    for config_file in config_files:
        print(f"Processing {config_file}...")
        
        # Load configuration
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
        
        # Modify configuration based on name
        if "large" in config_file:
            config.set("brain.neuron_count", 10000)
        
        # Run simulation
        brain, world, agent = run_complete_simulation(config, steps=500)
        
        # Save results
        os.makedirs(output_dir, exist_ok=True)
        brain.save(f"{output_dir}/{Path(config_file).stem}_result.bin")
        
        # Generate report
        report = pynlm.generateConfigurationReport(config)
        report_file = f"{output_dir}/{Path(config_file).stem}_report.txt"
        with open(report_file, "w") as f:
            f.write(report)
        
        print(f"  Completed: {brain.getTotalSpikeCount()} spikes")
```

---

## Part 5: Integration with Command-Line Interface

### 5.1 CLI Command Examples

```bash
# Create different types of configurations
nlm_cli.py create-config basic-experiment --template basic
nlm_cli.py create-config research-project --template research
nlm_cli.py create-config advanced-demo --template advanced

# Run simulations with different parameters
nlm_cli.py run-simulation configs/basic-experiment.json --steps 1000
nlm_cli.py run-simulation configs/research-project.json --steps 5000 --timestep 0.01
nlm_cli.py run-simulation configs/advanced-demo.json --steps 2000

# Batch processing
nlm_cli.py batch-run configs/ --output results/ --steps 1000

# Analyze results
nlm_cli.py analyze-brain results/experiment1.bin --verbose --output json
nlm_cli.py visualize-connectivity results/experiment1.bin

# Configuration comparison
nlm_cli.py compare-brains config1.json config2.json

# Export data
nlm_cli.py export-data results/experiment1.bin --format csv
nlm_cli.py export-data results/experiment1.bin --format json
```

### 5.2 Configuration Templates in CLI

```bash
# Available templates
nlm_cli.py help create-config

# Template descriptions:
# - basic: Simple configuration for basic testing
# - advanced: Comprehensive configuration with all features enabled
# - research: Configuration optimized for research experiments
```

---

## Part 6: Troubleshooting and Common Issues

### 6.1 Configuration Issues

**Problem**: Configuration file not found
**Solution**:
```bash
nlm_cli.py create-config my-config --template basic
nlm_cli.py show-config configs/my-config.json
```

**Problem**: Invalid JSON/YAML syntax
**Solution**:
```bash
# Use JSON5 for more flexible syntax
nlm_cli.py create-config my-config --template advanced
```

### 6.2 Simulation Issues

**Problem**: Brain not firing
**Solution**:
```python
# Check brain initialization
if not brain.initialize():
    print("Brain initialization failed!")
    config = pynlm.create_advanced_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
```

**Problem**: Agent not moving
**Solution**:
```python
# Check world setup
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Ensure world update is called
for step in range(100):
    world.update(0.1)
```

### 6.3 Performance Issues

**Problem**: Simulation too slow
**Solution**:
```python
# Use advanced configuration for better performance
config = pynlm.create_advanced_config()

# Reduce simulation size for testing
config.set("brain.neuron_count", 1000)

# Increase timestep
world.update(0.1)  # Instead of 0.01
```

---

## Part 7: Advanced Features Reference

### 7.1 Configuration Methods

#### Factory Functions
- `pynlm.createDefaultConfig()` - Create basic configuration
- `pynlm.createAdvancedConfig()` - Create advanced configuration
- `pynlm.createExperimentConfig(type)` - Create experiment-specific configuration

#### Configuration Operations
- `config.loadFromFile(path)` - Load from file (JSON/YAML/JSON5)
- `config.saveToFile(path)` - Save to file
- `config.loadFromArgs(argc, argv)` - Load from command line
- `config.set(key, value)` - Set value
- `config.get(key)` - Get value
- `config.has(key)` - Check if key exists
- `config.validate()` - Validate configuration

### 7.2 Brain Methods

#### Core Operations
- `brain.initialize()` - Initialize brain
- `brain.step(step, time)` - Run simulation step
- `brain.reset()` - Reset brain state
- `brain.save(path)` - Save brain state
- `brain.load(path)` - Load brain state

#### Information Methods
- `brain.getTotalNeuronCount()` - Total neurons
- `brain.getTotalSynapseCount()` - Total synapses
- `brain.getFiringNeuronCount()` - Currently firing neurons
- `brain.getAverageFiringRate()` - Average firing rate
- `brain.getExcitationInhibitionRatio()` - E/I balance
- `brain.getTotalSpikeCount()` - Total spikes
- `brain.getDevelopmentalStage()` - Current developmental stage
- `brain.getMostActiveNeurons(count)` - Most active neurons

### 7.3 Agent Methods

#### Core Operations
- `agent.initialize(world)` - Initialize agent
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.reset()` - Reset agent

#### Neuromodulation
- `agent.enableRewardModulation(enable)` - Enable reward modulation
- `agent.enableCuriosity(enable)` - Enable curiosity
- `agent.enableNovelty(enable)` - Enable novelty detection
- `agent.enableStructuralPlasticity(enable)` - Enable structural plasticity
- `agent.enableDevelopment(enable)` - Enable development

#### Information Methods
- `agent.getCuriosityLevel()` - Current curiosity level
- `agent.getNoveltyLevel()` - Current novelty level
- `agent.getPredictionError()` - Current prediction error
- `agent.getNeuromodulationLevel()` - Current neuromodulation level

---

## Part 8: Quick Start Examples

### Example 1: Basic Brain Simulation
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Complete Agent Simulation
```python
import pynlm

# Setup
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Example 3: Research Experiment
```python
import pynlm
import json

# Create research configuration
config = pynlm.createExperimentConfig("research")

# Modify for specific experiment
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.curiosity.scale", 1.0)

# Save configuration
with open("research_config.json", "w") as f:
    json.dump(config, f, indent=2)

# Run simulation
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(2000):
    brain.step(step)

# Analyze results
print(f"Research simulation results:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
print(f"  Spikes: {brain.getTotalSpikeCount()}")
```

---

## Part 9: Performance Tips

### 9.1 Optimization Strategies

1. **Use Advanced Configuration**: `pynlm.createAdvancedConfig()` for better performance
2. **Reduce Neuron Count for Testing**: Use smaller configurations for development
3. **Increase Timestep**: Use larger timesteps for faster simulations
4. **Batch Processing**: Process multiple configurations efficiently
5. **Parallel Processing**: Use multiple CPU cores for independent simulations

### 9.2 Memory Management

- Clear configuration objects when no longer needed
- Use `brain.reset()` instead of creating new brain objects
- Monitor memory usage during long simulations
- Use `gc.collect()` periodically for large simulations

### 9.3 Configuration Caching

The system automatically caches frequently used configurations:
```python
# Configuration is cached for faster loading
config1 = pynlm.createDefaultConfig()
config1.loadFromFile("experiment1.json")  # Loaded

config2 = pynlm.createDefaultConfig()
config2.loadFromFile("experiment1.json")  # Loaded from cache
```

---

## Part 10: Future Enhancements

### 10.1 Planned Features

1. **GPU Acceleration**: Support for GPU-accelerated neural computation
2. **Distributed Computing**: Multi-machine simulation capabilities
3. **Web Interface**: Web-based configuration and simulation control
4. **Docker Integration**: Containerized deployment
5. **API Documentation**: Auto-generated API documentation

### 10.2 Research Directions

1. **Advanced Neuromodulation**: More sophisticated neuromodulation systems
2. **Developmental Biology**: Better models of brain development
3. **Social Learning**: Multi-agent interaction systems
4. **Cognition Models**: Advanced cognitive architectures

---

## Conclusion

The enhanced NLM system provides a comprehensive platform for neural simulation with advanced features for both beginners and researchers. The combination of:

- **Improved Python API**: More intuitive and powerful interface
- **Advanced Configuration System**: JSON/YAML support with validation
- **Command-Line Interface**: Powerful CLI for automation
- **Performance Tools**: Benchmarking and analysis utilities
- **Comprehensive Documentation**: Detailed examples and guides

makes NLM suitable for a wide range of applications, from educational demonstrations to advanced research simulations.

The system maintains backward compatibility while adding powerful new features that make NLM more accessible, powerful, and flexible for users at all levels.
