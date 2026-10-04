# NLM CLI - Command Line Interface

# Quick Start

This document provides a quick introduction to using the NLM (Neural Learning Machine) command line interface.

## Installation

Ensure NLM is installed:

```bash
pip install pynlm
```

## Basic Commands

### Create a New Simulation

Create a new NLM simulation:

```bash
nlm create my_simulation
```

You can specify different brain and world types:

```bash
# Create a simple simulation
nlm create simple_sim --brain-type simple --world-type simple

# Create an advanced simulation
nlm create advanced_sim --brain-type advanced --world-type complex

# Create a research simulation
nlm create research_sim --brain-type research --world-type complex
```

### Run a Simulation

Run an existing simulation:

```bash
nlm run my_simulation
```

Run with custom parameters:

```bash
nlm run my_simulation --steps 200 --output ./results
```

### Test NLM

Run NLM tests:

```bash
# Run quick tests
nlm test --type quick

# Run full test suite
nlm test --type full

# Run performance tests
nlm test --type performance
```

### Create Configurations

Manage configuration files:

```bash
# List all configurations
nlm config list

# Show configuration details
nlm config show my_config

# Create a new configuration
nlm config create my_config --type json
```

### Create Visualizations

Create visualizations for simulations:

```bash
# Create network graph
nlm viz my_simulation --type network

# Create activity plots
nlm viz my_simulation --type activity

# Create debug dashboard
nlm viz my_simulation --type debug

# Create all visualizations
nlm viz my_simulation --type all --output ./visualizations
```

### Profile Simulations

Profile simulation performance:

```bash
nlm profile my_simulation --steps 100 --output profile_results.json
```

### Run Benchmarks

Run performance benchmarks:

```bash
nlm benchmark --steps 100 --output benchmark_results.json
```

## Configuration Files

NLM uses configuration files to set simulation parameters. You can create configurations in three formats:

### JSON Configuration

```json
{
    "brain": {
        "neuron_count": 100,
        "region_count": 2,
        "connection_probability": 0.1,
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
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true
        },
        "structural": {
            "enable": true
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        },
        "prediction_error": {
            "enable": true
        }
    },
    "simulation": {
        "timestep": 0.001,
        "random_seed": 42
    }
}
```

### YAML Configuration

```yaml
brain:
    neuron_count: 100
    region_count: 2
    connection_probability: 0.1
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
    hebbian:
        enable: true
    structural:
        enable: true

neuromod:
    dopamine:
        scale: 1.0
    curiosity:
        enable: true
    novelty:
        enable: true
    prediction_error:
        enable: true

simulation:
    timestep: 0.001
    random_seed: 42
```

### Key Value Configuration

```ini
# configs/my_config.cfg

# Basic brain parameters
brain.neuron_count = 100
brain.region_count = 2
brain.connection_probability = 0.1
brain.initial_weight_mean = 0.5
brain.initial_weight_std = 0.1
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0

# Plasticity parameters
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation parameters
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Simulation parameters
simulation.timestep = 0.001
random_seed = 42
```

## Using NLM

### Creating a Simulation

1. **Create a new simulation**:
   ```bash
   nlm create my_simulation
   ```

2. **Specify configuration** (optional):
   ```bash
   nlm create my_simulation --brain-type advanced --world-type complex
   ```

3. **Navigate to the simulation directory**:
   ```bash
   cd simulations/my_simulation
   ```

4. **Run the simulation**:
   ```bash
   python run_simulation.py
   ```

### Running a Simulation

You can run simulations directly using the NLM CLI:

```bash
nlm run my_simulation
```

For simulations with many steps, you can specify the number of steps:

```bash
nlm run my_simulation --steps 200
```

### Viewing Results

When a simulation completes, you can:

1. **View console output**: The simulation prints progress information
2. **Check output directory**: Results are saved in the simulation directory
3. **Generate visualizations**: Use the `viz` command
4. **Analyze performance**: Use the `profile` command

### Using Visualizations

Visualizations help you understand and analyze simulation results:

```bash
# Create network graph
nlm viz my_simulation --type network

# Create activity plots
nlm viz my_simulation --type activity

# Create debug dashboard
nlm viz my_simulation --type debug

# Create all visualizations
nlm viz my_simulation --type all
```

### Profiling

Profile simulation performance to identify bottlenecks:

```bash
nlm profile my_simulation --steps 100 --output profile_results.json
```

### Testing

Test your NLM installation and verify everything works:

```bash
# Run quick tests
nlm test --type quick

# Run full test suite
nlm test --type full

# Run performance tests
nlm test --type performance
```

### Configuration Management

Manage NLM configurations:

```bash
# List all configurations
nlm config list

# Show configuration details
nlm config show my_config

# Create a new configuration
nlm config create my_config
```

## Example Simulation

Here's an example of running a complete NLM simulation:

```bash
# Create a new simulation
nlm create my_simulation

# Run the simulation with visualizations and profiling
nlm run my_simulation --steps 100 --plot --profile

# Generate visualizations
nlm viz my_simulation --type all --output ./results

# View simulation summary
ls -la simulations/my_simulation/
cat simulations/my_simulation/config.json
```

## Advanced Usage

### Using Custom Configurations

Create custom configurations for specific use cases:

```bash
# Create a configuration file
cat > my_config.json << EOF
{
    "brain": {
        "neuron_count": 500,
        "region_count": 3,
        "connection_probability": 0.15
    },
    "plasticity": {
        "stdp": { "enable": true, "learning_rate": 0.002 },
        "hebbian": { "enable": true },
        "structural": { "enable": false }
    }
}
EOF

# Run simulation with custom config
nlm config create custom_config
nlm run my_simulation --config custom_config.json
```

### Running Multiple Simulations

Run multiple simulations in parallel:

```bash
#!/bin/bash

# Array of simulation configurations
simulations=("small" "medium" "large")

# Run each simulation
for sim in "${simulations[@]}"; do
    echo "Running simulation: $sim"
    nlm run $sim --steps 100
    echo "Simulation $sim completed"
done
```

### Batch Processing

Process multiple simulations in batch:

```bash
#!/bin/bash

# Create output directory
mkdir -p results/batch

# Run simulations with different parameters
for steps in 50 100 200; do
    echo "Running simulation with $steps steps..."
    nlm run my_simulation --steps $steps --output results/batch/$steps
    echo "Completed: $steps steps"
done
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.connection_probability` | float | 0.05 | Connection probability |
| `brain.initial_weight_mean` | float | 0.5 | Initial weight mean |
| `brain.initial_weight_std` | float | 0.1 | Initial weight standard deviation |
| `brain.v_thresh` | float | -50.0 | Neuron threshold |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (positive) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (negative) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error |

### Simulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation timestep |
| `random_seed` | int | 42 | Random seed |

## Error Handling

### Common Issues

**Issue: Simulation not found**

```bash
# Error: Simulation 'my_simulation' not found
# Solution: nlm create my_simulation
```

**Issue: Configuration file not found**

```bash
# Error: Config file not found: configs/my_config.cfg
# Solution: nlm config create my_config
```

**Issue: Python dependencies not installed**

```bash
# Error: No module named 'pynlm'
# Solution: pip install pynlm
```

### Troubleshooting

1. **Check dependencies**: Ensure all required packages are installed
2. **Check file paths**: Verify file paths are correct
3. **Check permissions**: Ensure files have correct permissions
4. **Check Python version**: Ensure Python 3.8+ is used
5. **Check configuration**: Validate configuration files

## Support

For issues and support:

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Documentation**: https://docs.nlm-project.org

## License

MIT License

---

**NLM CLI** is part of the NLM (Neural Learning Machine) project.

For more information, visit: https://github.com/nlm-project/nlm