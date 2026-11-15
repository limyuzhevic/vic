# NLM Configuration Manager

A command-line tool to help users configure and run NLM simulations.

## Overview

The NLM Configuration Manager is a powerful tool designed to simplify the process of working with NLM configurations. It provides a user-friendly interface for creating, validating, and managing configuration files, making it easier for both beginners and advanced users to experiment with different neural network settings.

## Installation

The configuration manager is included with the NLM source code. To use it:

1. Make sure you have a working NLM installation
2. Navigate to the workspace root directory
3. The tool is available as `nlm-config`

```bash
# Make it executable (if not already)
chmod +x nlm-config

# Run the help command
./nlm-config help
```

## Basic Usage

### Getting Help

```bash
./nlm-config help
```

This displays the complete help documentation with all available commands and options.

### Listing Available Configuration Templates

```bash
./nlm-config list-configs
```

Shows all built-in configuration templates and their descriptions.

### Validating a Configuration File

```bash
./nlm-config validate configs/your_config.cfg
```

Validates a configuration file for syntax and required parameters.

### Creating a New Configuration

```bash
./nlm-config create my_experiment phase6
```

Creates a new configuration file with the Phase 6 template (full integration).

**Available Templates:**
- `phase2` - Basic neural computation (LIF neurons, STDP plasticity)
- `phase3` - World interaction with reward and developmental stages
- `phase6` - Full integration with memory, prediction, cognition
- `debug` - High-verbosity debug configuration
- `minimal` - Minimal configuration for testing
- `research` - Research-oriented configuration with extended parameters

### Setting a Configuration Value

```bash
./nlm-config set configs/my_config.cfg random_seed 12345
```

Sets a specific configuration value. If the key already exists, it will be updated.

### Exporting Configuration to Different Formats

```bash
# Export as plain text
./nlm-config export configs/my_config.cfg

# Export as JSON
./nlm-config export configs/my_config.cfg json
```

**Available Formats:**
- `text` - Plain text format (default)
- `json` - JSON format for programmatic use

### Running a Simulation

```bash
./nlm-config run configs/my_config.cfg --verbose --steps 1000
```

This simulates running the NLM with the specified configuration.
For actual NLM execution, use:

```bash
./nlm --config=configs/my_config.cfg [options]
```

## Configuration Templates

### Phase 2: Basic Neural Computation

Perfect for understanding the fundamental neural dynamics:

- **Neurons**: LIF (Leaky Integrate-and-Fire) model
- **Plasticity**: Real STDP and Hebbian plasticity rules
- **Structural Plasticity**: Synaptogenesis and pruning
- **Neurons**: ~37k synapses for 500 neurons
- **Connections**: 2 regions, 15% connection probability
- **Use Case**: Educational demonstrations, basic neural computation studies

### Phase 3: World Interaction

Introduces environmental interaction and developmental stages:

- **Neurons**: 5,000 neurons with 4 interconnected regions
- **World**: GridWorld environment (20×20)
- **Reward**: Reward prediction error and dopamine modulation
- **Development**: Developmental stages with changing plasticity rates
- **Memory**: Working memory with 500 traces, episodic memory with 5,000 episodes
- **Use Case**: Behavioral experiments, action selection studies

### Phase 6: Full Integration

The complete neural learning machine with all systems integrated:

- **Neurons**: 10,000 neurons with 6 interconnected regions
- **Memory**: Working memory (1,000 traces), episodic memory (10,000 episodes), associative memory (5,000 capacity)
- **Prediction**: Neural prediction system with 10-step horizon
- **Cognition**: Neural planner, concept formation, attentional selection
- **Neuromodulation**: Full dopamine, curiosity, novelty, prediction error systems
- **Development**: Complete developmental stages with plasticity modulation
- **Use Case**: Research simulations, complex behavior studies

### Debug: Development Configuration

Optimized for development and troubleshooting:

- **Neurons**: Smaller scale (100 neurons) for faster debugging
- **Logging**: Maximum verbosity (DEBUG level)
- **Structure**: Single region for simplicity
- **Use Case**: Development, debugging, learning the system

### Minimal: Testing Configuration

Simplest configuration for quick tests:

- **Neurons**: Very small scale (20 neurons)
- **Single**: Single region
- **Rapid**: Fast execution for testing
- **Use Case**: Quick tests, unit testing

### Research: Extended Configuration

Research-oriented with maximum flexibility:

- **Neurons**: Large scale (50,000 neurons, 8 regions)
- **Time**: Fine-grained time steps (0.0001 seconds)
- **Memory**: Large memory capacities
- **Performance**: Optimized for research throughput
- **Use Case**: Large-scale simulations, research experiments

## Configuration File Format

NLM configuration files use a simple key-value format:

```makefile
# Configuration file header
key = value
another_key = another_value
```

### Key Types

- **Integer**: `random_seed = 42`
- **Float**: `simulation_timestep = 0.001`
- **String**: `environment_name = GridWorld`

### Common Configuration Keys

| Key | Description | Default |
|-----|-------------ash | |
| `random_seed` | Seed for random number generator | `42` |
| `simulation_timestep` | Simulation time step in seconds | `0.001` |
| `neuron_count` | Number of neurons in the brain | `1000` |
| `region_count` | Number of brain regions | `2` |
| `connection_probability` | Probability of random connections | `0.15` |
| `plasticity_learning_rate` | General plasticity learning rate | `0.01` |
| `stdp_ltp_weight` | STDP long-term potentiation weight | `0.02` |
| `stdp_ltd_weight` | STDP long-term depression weight | `0.015` |
| `development_synaptogenesis_rate` | Structural plasticity synaptogenesis rate | `0.0001` |
| `development_pruning_rate` | Structural plasticity pruning rate | `0.00001` |
| `dopamine_baseline` | Baseline dopamine level | `0.1` |
| `reward_discount_factor` | Reward discount factor | `0.99` |

### Environment-Specific Keys

For Phase 3 and 6:

| Key | Description |
|-----|ash |
| `environment_name` | World environment name | `GridWorld` |
| `environment_width` | World width | `20` |
| `environment_height` | World height | `20` |
| `enable_checkpoints` | Enable checkpoint saving | `true` |
| `checkpoint_interval` | Checkpoint save interval | `10000` |

### Memory-Specific Keys

For Phase 6:

| Key | Description | Default |
|-----|-------------ash | |
| `working_memory_capacity` | Working memory capacity | `1000` |
| `episodic_memory_max_episodes` | Maximum episodic memory episodes | `10000` |
| `associative_memory_capacity` | Associative memory capacity | `5000` |

## Tips for Users

### For Beginners

1. **Start with Phase 2**: Use `./nlm-config create demo phase2` to create a basic configuration
2. **Validate First**: Always validate your configuration before running: `./nlm-config validate configs/demo.cfg`
3. **Use Examples**: Check the `configs/` directory for example configurations
4. **Incremental Testing**: Start with minimal configurations and gradually increase complexity

### For Advanced Users

1. **Customize Templates**: Create custom templates by modifying existing configurations
2. **Batch Processing**: Use the configuration manager to run multiple experiments with different parameters
3. **Export for Analysis**: Export configurations to JSON for analysis with other tools
4. **Debug Complex Issues**: Use the debug template for troubleshooting

### Best Practices

1. **Version Control**: Commit your configuration files to version control
2. **Documentation**: Add comments to configuration files explaining parameter choices
3. **Testing**: Create test configurations for different scenarios
4. **Backup**: Use checkpointing for long-running experiments

## Command Reference

### help
Display complete help documentation.

### list-configs
List all available configuration templates with descriptions.

### validate <file>
Validate a configuration file for syntax and required parameters.

### create <name> [type]
Create a new configuration file with the specified template.

### set <file> <key> <value>
Set or update a configuration value in a file.

### export <file> [format]
Export configuration to different formats (text, json).

### run <file> [options]
Simulate running the NLM with a configuration (shows configuration options).

## Examples

### Create and Validate a Research Configuration

```bash
# Create a research configuration
./nlm-config create my_research phase6

# Validate it
./nlm-config validate configs/my_research.cfg

# Export it to JSON for analysis
./nlm-config export configs/my_research.cfg json > my_research.json

# Run the simulation
./nlm --config=configs/my_research.cfg --verbose --steps=100000
```

### Batch Experiment Management

```bash
# Create multiple experiment configurations
for exp in exp1 exp2 exp3; do
    ./nlm-config create "$exp" phase6
    ./nlm-config set "configs/${exp}.cfg" "random_seed" "$((RANDOM % 1000))"
    ./nlm-config set "configs/${exp}.cfg" "neuron_count" "$((1000 + RANDOM % 5000))"
done

# Run all experiments
for exp in exp1 exp2 exp3; do
    echo "Running experiment: $exp"
    ./nlm --config="configs/${exp}.cfg" --steps=50000
    # Save results with unique names
    mv nlm.log "${exp}_results.log"
done
```

### Configuration Debugging

```bash
# Create a debug configuration
./nlm-config create debug_run debug

# Check what configuration changes will be made
./nlm-config set configs/debug_run.cfg neuron_count 50
./nlm-config set configs/debug_run.cfg log_level DEBUG

# Validate the final configuration
./nlm-config validate configs/debug_run.cfg

# Run with the debug configuration
./nlm --config=configs/debug_run.cfg --verbose
```

## Frequently Asked Questions

### What configuration should I start with?

**For learning**: Use the `phase2` template
**For research**: Use the `phase6` template
**For debugging**: Use the `debug` template

### How do I modify a configuration value?

Use the `set` command:

```bash
./nlm-config set configs/my_config.cfg neuron_count 5000
```

### Can I backup my configuration?

Yes, use checkpointing in your configuration:

```bash
# In your config file
enable_checkpoints = true
checkpoint_interval = 10000
checkpoint_dir = ./my_checkpoints
```

### What's the difference between templates?

Each template is optimized for a different use case:
- `phase2`: Educational and basic neural computation
- `phase3`: Behavioral experiments with reward
- `phase6`: Full cognitive integration
- `debug`: Development and troubleshooting
- `minimal`: Quick testing
- `research`: Large-scale simulations

## Troubleshooting

### Configuration Not Working

1. **Validate first**: Always validate your configuration
2. **Check required keys**: Ensure all required parameters are present
3.very complex template
very simple template

The template system offers multiple configuration templates optimized for different use cases:

### Available Templates

- **Phase 2**: For educational demonstrations and basic neural computation with LIF neurons and STDP plasticity
- **Phase 3**: For world interaction experiments with reward and developmental stages
- **Phase 6**: For full integration with memory, prediction, cognition, and neuromodulation
- **Debug**: For development and troubleshooting with maximum verbosity
- **Minimal**: For quick testing with a small-scale configuration
- **Research**: For large-scale simulations with extended parameters

Each template has been designed to provide a starting point for different levels of experimentation, from simple educational demonstrations to complex research simulations.

### Configuration Validation

Always validate your configuration before running:

```bash
./nlm-config validate configs/my_config.cfg
```

This checks for:
- Required configuration keys
- File syntax
- Configuration parameter counts

### Setting Configuration Values

Modify configuration values:

```bash
./nlm-config set configs/my_config.cfg random_seed 12345
./nlm-config set configs/my_config.cfg neuron_count 5000
```

### Running the NLM

Run the neural learning machine:

```bash
./nlm --config=configs/my_config.cfg --verbose --steps=100000
```

### Configuration Export

Export for analysis:

```bash
./nlm-config export configs/my_config.cfg json > config.json
```

### Best Practices

1. **Validate first**: Always validate your configuration before running
2. **Version control**: Commit configuration files
3. **Comments**: Add explanatory comments to configuration files
4. **Incremental testing**: Start with minimal configurations
5. **Batch processing**: Use the configuration manager for multiple experiments

## Quick Start

```bash
# Create and run a Phase 2 configuration
./nlm-config create demo phase2
./nlm-config validate configs/demo.cfg
./nlm --config=configs/demo.cfg --verbose --steps=1000
```

That's it! You now have a complete configuration management system for working with NLM.
