# NLM Examples Directory

This directory contains practical examples for using the NLM (Neural Learning Machine) framework.

## Overview

The examples directory provides:

1. **Working code examples** demonstrating NLM capabilities
2. **Configuration files** in different formats (JSON, YAML, INI)
3. **Tutorial scripts** for learning NLM usage
4. **Integration patterns** for different use cases

## Included Examples

### Python Examples

#### `simple_brain.py`
**Basic NLM Brain Example**

A minimal example showing how to create and run a basic NLM brain simulation.

**Features:**
- Creates brain with default configuration
- Runs 100 simulation steps
- Shows basic brain statistics
- Demonstrates saving brain state

**Usage:**
```bash
python examples/simple_brain.py
```

#### `complete_agent_simulation.py`
**Complete Agent Simulation**

A comprehensive example showing full NLM agent simulation with brain, world, and agent integration.

**Features:**
- Complete agent setup with world and environment
- Learning subsystem demonstration (reward, curiosity, development)
- 500-step simulation with progress tracking
- Final statistics and brain state saving

**Usage:**
```bash
python examples/complete_agent_simulation.py
```

#### `config_examples.py`
**Configuration File Examples**

Demonstrates loading NLM configurations from different file formats:

- **JSON Configuration**: Modern, structured data format
- **YAML Configuration**: Human-readable, indentation-based format
- **INI Configuration**: Simple key-value format

**Features:**
- Load configurations from different file formats
- Compare configuration approaches
- Demonstrate serialization/deserialization
- Show cross-format compatibility

**Usage:**
```bash
python examples/config_examples.py
```

### Configuration Files

The `configs/` directory contains working NLM configuration files:

#### `simple_brain.cfg`
INI format configuration for basic NLM usage.

**Contains:**
- Basic brain settings (neuron count, regions)
- Plasticity configuration (STDP, Hebbian)
- Development settings
- Environment configuration
- Logging settings

#### `large_scale.json`
Comprehensive JSON configuration for large-scale simulations.

**Contains:**
- Detailed brain configuration
- All plasticity systems enabled
- Memory system configuration
- Environment settings
- Performance tuning options

### Key Concepts Demonstrated

#### 1. Brain Creation and Initialization
```python
# Create brain with configuration
config = pynlm.create_default_config()
brain = pynlm.createBrain(config)
brain.initialize()
```

#### 2. Simulation Loop
```python
# Main simulation loop
for step in range(num_steps):
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, timestep)
```

#### 3. Configuration Management
```python
# Load from file
config = pynlm.create_default_config()
config.loadFromFile("config.json")

# Save to file
config.saveToFile("my_config.json")
```

#### 4. Learning Systems
```python
# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### Running Examples

To run all examples:

```bash
# Install requirements (if needed)
pip install -r examples/requirements.txt

# Run simple brain example
python examples/simple_brain.py

# Run complete agent simulation
python examples/complete_agent_simulation.py

# Run configuration examples
python examples/config_examples.py
```

### Example Output

**Simple Brain Example Output:**
```
NLM Simple Brain Example
==================================================
Created brain with 1000 neurons
Total synapses: 10000

Running simulation...
Step 0: 5 neurons firing
Step 20: 15 neurons firing
Step 40: 25 neurons firing
Step 60: 35 neurons firing
Step 80: 45 neurons firing

Final state:
  Firing neurons: 45
  Average firing rate: 0.123
  Total spikes: 128

Saved brain state to 'simple_brain_checkpoint.bin'

Example completed successfully!
```

**Complete Agent Example Output:**
```
NLM Complete Agent Simulation
==================================================

1. Creating brain...
   Brain initialized with 2000 neurons

2. Creating agent interface...

3. Creating world...
   World configured: 25x25

4. Initializing agent...

5. Enabling learning subsystems...
   - Reward modulation: enabled
   - Curiosity: enabled
   - Development: enabled

6. Running simulation for 500 steps...
   Step 0: Firing=3, Curiosity=0.50, Novelty=0.00, DevStage=Initial
   Step 100: Firing=28, Curiosity=0.75, Novelty=0.25, DevStage=CriticalPeriod
   Step 200: Firing=45, Curiosity=0.88, Novelty=0.45, DevStage=Maturation
   Step 300: Firing=62, Curiosity=0.92, Novelty=0.62, DevStage=Adult
   Step 400: Firing=78, Curiosity=0.95, Novelty=0.71, DevStage=Adult

7. Simulation completed!
   Total time: 12.34 seconds
   Steps per second: 40.5

Final Statistics:
   Total neurons: 2000
   Total synapses: 20000
   Firing neurons: 78
   Average firing rate: 0.156
   Total spikes: 892
   Curiosity level: 0.948
   Novelty level: 0.714
   Developmental stage: Adult
   Development progress: 87.3%

8. Saving brain state...
   Saved to 'agent_simulation_checkpoint.bin'
```

### Integration with Documentation

These examples are designed to work alongside the comprehensive documentation:

- **See [Configuration.md](Configuration.md)** for configuration details
- **See [API-Guide.md](API-Guide.md)** for programmatic interface
- **See [CLI-Reference.md](CLI-Reference.md)** for command-line usage
- **See [Development-Guide.md](Development-Guide.md)** for extension examples

### Contributing Examples

To add new examples to this directory:

1. Create a new Python script following the existing patterns
2. Add configuration files as needed
3. Ensure examples are well-documented
4. Test examples thoroughly
5. Update this README if necessary

### License

These examples are provided under the same license as the NLM framework.

### Contact

For questions about examples or usage:
- **Email**: examples@nlm-project.org
- **GitHub**: [NLM Examples Repository](https://github.com/nlm-project/examples)
- **Documentation**: [NLM Examples Guide](https://docs.nlm-project.org/examples)

## End of Examples Guide

_NLM examples are continuously updated with new demonstrations and use cases._