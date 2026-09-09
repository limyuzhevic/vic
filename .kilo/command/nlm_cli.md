# NLM Command Line Interface

Advanced command line tools for neural simulation control and management.

## Overview

The NLM command line interface provides comprehensive tools for:
- Running and managing neural simulations
- Analyzing and visualizing brain states  
- Advanced configuration management
- Debugging and profiling
- Batch processing and automation

## Core Commands

### nlm run [OPTIONS] - Run Simulation

Run a neural simulation with advanced options.

**Options:**
- `-c, --config <file>`: Configuration file path (required)
- `-n, --neurons <count>`: Override neuron count
- `-r, --regions <count>`: Override region count
- `-s, --steps <count>`: Number of simulation steps
- `-t, --timestep <time>`: Simulation time step
- `--max-steps <count>`: Maximum simulation steps
- `--dev-enable`: Enable developmental processes
- `--neuro-enable`: Enable neuromodulation
- `--pred-enable`: Enable prediction
- `--cog-enable`: Enable cognitive functions
- `--checkpoint <path>`: Checkpoint save path
- `--replay-interval <steps>`: Memory replay interval
- `-o, --output <file>`: Output file for results
- `--verbose`: Enable verbose logging
- `--quiet`: Suppress non-essential output

**Example:**
```bash
nlm run \
  --config=./experiments/phase6_config.yaml \
  --dev-enable \
  --neuro-enable \
  --pred-enable \
  --cog-enable \
  --checkpoint=./checkpoints/phase6 \
  --replay-interval=1000 \
  --output=./results/phase6_results.json \
  --verbose
```

### nlm config [COMMAND] [OPTIONS] - Configuration Management

Manage NLM configuration options.

**Commands:**
- `list`: List all available configuration options
- `get <key>`: Get configuration value
- `set <key> <value>`: Set configuration value
- `merge <file>`: Merge configuration from file
- `validate <file>`: Validate configuration file
- `export <file>`: Export configuration to file
- `import <file>`: Import configuration from file

**Example:**
```bash
nlm config set brain.neuron_count 2000
nlm config get brain.synapse_density
nlm config validate ./config.yaml
```

### nlm analyze <brain_file> [OPTIONS] - Analyze Brain State

Analyze brain state file for metrics and statistics.

**Options:**
- `--metrics <list>`: Comma-separated list of metrics (all, connectivity, firing_rate, memory, development, plasticity, neuromodulation)
- `--output <file>`: Output analysis file (JSON format)
- `--format <format>`: Output format (json, csv, yaml)
- `--detailed`: Generate detailed analysis
- `--compare <file>`: Compare with another brain state file
- `--trend`: Analyze trends over time

**Example:**
```bash
nlm analyze ./brain_state.nml --metrics=connectivity,memory,development --output=./analysis/report.json
```

### nlm visualize <brain_file> [OPTIONS] - Generate Visualizations

Generate visualizations of brain state.

**Options:**
- `--type <type>`: Visualization type (brain_snapshot, connectivity_matrix, activity_patterns, memory_traces, development_stages, plasticity_map, neuromodulation_levels)
- `--output <file>`: Output image file
- `--width <pixels>`: Image width
- `--height <pixels>`: Image height
- `--format <format>`: Image format (png, jpg, svg, pdf)
- `--palette <scheme>`: Color palette (sequential, divergent, categorical)
- `--annotation`: Add annotations
- `--overlay <file>`: Overlay another visualization

**Example:**
```bash
nlm visualize ./brain_state.nml --type=brain_snapshot --output=./brain_snapshot.png --width=800 --height=600
```

### nlm debug <brain_file> --step <step> [OPTIONS] - Debug Specific Step

Debug specific step in brain simulation.

**Options:**
- `--step <step>`: Step number to debug (required)
- `--output <file>`: Debug output file
- `--format <format>`: Output format (json, csv, txt)
- `--include <list>`: Include debug information (spikes, connections, memory, plasticity, neuromodulation)
- `--exclude <list>`: Exclude debug information
- `--threshold <value>`: Debug threshold for significant events

**Example:**
```bash
nlm debug ./simulation.nml --step=5000 --output=./debug/debug_info.json --include=spikes,connections,memory
```

### nlm profile <brain_file> [OPTIONS] - Profile Performance

Profile brain performance metrics.

**Options:**
- `--metrics <list>`: Metrics to profile (cpu, memory, gpu, io, simulation_time)
- `--output <file>`: Profile output file
- `--format <format>`: Output format (json, csv, txt, html)
- `--detailed`: Generate detailed profiling
- `--timeline`: Generate timeline visualization
- `--compare <file>`: Compare with another profile

### nlm checkpoint [COMMAND] [OPTIONS] - Checkpoint Management

Manage checkpoint files.

**Commands:**
- `list`: List available checkpoints
- `save --name <name>`: Save current brain state
- `save --file <file>`: Save to specific file
- `load --name <name>`: Load checkpoint by name
- `load --file <file>`: Load from specific file
- `delete --name <name>`: Delete checkpoint
- `delete --file <file>`: Delete specific file
- `copy --source <source> --target <target>`: Copy checkpoint
- `prune --older-than <time>`: Delete old checkpoints

**Example:**
```bash
nlm checkpoint save --name=pre_learning
nlm checkpoint load --name=pre_learning
```

### nlm experiment run <experiment_config> [OPTIONS] - Run Experiment Suite

Run experiment suite with advanced options.

**Options:**
- `--config <file>`: Experiment configuration file
- `--variant <name>`: Experiment variant
- `--parameters <file>`: Parameter file for variations
- `--repeats <count>`: Number of repeats
- `--parallel <count>`: Number of parallel runs
- `--output-dir <dir>`: Output directory
- `--name-prefix <prefix>`: Output file prefix
- `--continue-on-error`: Continue on errors
- `--timeout <seconds>`: Timeout per experiment

### nlm experiment compare [OPTIONS] EXPERIMENT_DIRS... - Compare Experiments

Compare results from multiple experiments.

**Options:**
- `--metrics <list>`: Metrics to compare (total_reward, avg_firing_rate, memory_episodes, dopamine_level, curiosity_level, novelty_level)
- `--output <file>`: Comparison output file
- `--format <format>`: Output format (json, csv, txt, html)
- `--statistical`: Include statistical analysis
- `--significance <alpha>`: Statistical significance level
- `--plot`: Generate comparison plots
- `--bar-chart`: Generate bar chart
- `--box-plot`: Generate box plot

### nlm batch [OPTIONS] EXPERIMENT_CONFIG... - Run Batch Simulations

Run multiple simulations in parallel or sequence.

**Options:**
- `-p, --parallel <count>`: Number of parallel simulations
- `-o, --output-dir <dir>`: Output directory
- `--name-pattern <pattern>`: Naming pattern for outputs
- `--continue-on-error`: Continue running other simulations on failure
- `--timeout <seconds>`: Timeout per simulation
- `--vars <file>`: Variable file for parameter sweeps

## Configuration File Format

NLM uses YAML format for configuration files:

```yaml
brain:
  neuron_count: 2000
  region_count: 3
  connection_probability: 0.15
  synapse_density: 0.05

plasticity:
  stdp:
    enable: true
    ltp_weight: 0.01
    ltd_weight: 0.012
    tau: 20.0
  hebbian:
    enable: true
    learning_rate: 0.001
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
  dopamine:
    enable: true
    scale: 1.0
    baseline: 0.0
  curiosity:
    enable: true
    level: 0.1
  novelty:
    enable: true

development:
  enable: true
  critical_period: 1000
  stage_progression_rate: 0.001

simulation:
  timestep: 0.001
  max_steps: 10000
  save_interval: 1000

experiment:
  type: phase6
  curiosity_level: 0.1
  reward_scale: 1.0
  enable_checkpoints: true
  enable_replay: true
```

## Configuration File Examples

### Basic Configuration

`basic_config.yaml`:
```yaml
brain:
  neuron_count: 500
  region_count: 1

simulation:
  timestep: 0.001
  max_steps: 1000
```

### Phase 6 Configuration

`phase6_config.yaml`:
```yaml
brain:
  neuron_count: 2000
  region_count: 3
  connection_probability: 0.15

memory:
  working_memory_capacity: 500
  episodic_memory_max_episodes: 5000
  enable_associative_memory: true

development:
  enable_development: true
  critical_period_duration: 1000
  stage_progression_rate: 0.001

neuromodulation:
  dopamine_enabled: true
  curiosity_enabled: true
  novelty_enabled: true
  prediction_error_enabled: true

prediction:
  enable_prediction: true
  prediction_horizon: 10

cognition:
  enable_planning: true
  planning_depth: 5
  enable_attention: true
  attention_inhibition: 0.5
  enable_concept_formation: true

experiments:
  enable_checkpointing: true
  enable_replay: true
  replay_interval: 1000
  enable_developmental_plasticity: true
```

## Command Examples

### Complete Phase 6 Experiment

```bash
nlm run \
  --config=./experiments/phase6_config.yaml \
  --dev-enable \
  --neuro-enable \
  --pred-enable \
  --cog-enable \
  --checkpoint=./checkpoints/phase6 \
  --replay-interval=1000 \
  --output=./results/phase6_results.json \
  --verbose
```

### Batch Parameter Sweep

```bash
nlm batch \
  --parallel=4 \
  --output-dir=./batch_results \
  --name-pattern="exp_{neuron_count}_neurons" \
  ./configs/base_config.yaml \
  --neurons=500 \
  --neurons=1000 \
  --neurons=2000 \
  --neurons=5000
```

### Advanced Analysis

```bash
nlm analyze \
  ./brain_state.nml \
  --metrics=connectivity,memory,development,plasticity,neuromodulation \
  --output=./analysis/report.json \
  --format=json \
  --detailed \
  --compare=./baseline_state.nml
```

### Experiment Comparison

```bash
nlm experiment compare \
  ./experiment_results/experiment_1 \
  ./experiment_results/experiment_2 \
  ./experiment_results/experiment_3 \
  --metrics=total_reward,avg_firing_rate,memory_episodes,dopamine_level \
  --output=./comparison_report.html \
  --format=html \
  --statistical \
  --significance=0.05 \
  --plot \
  --bar-chart \
  --box-plot
```

## Exit Codes

- `0`: Success
- `1`: General error
- `2`: Configuration error
- `3`: File not found
- `4`: Permission denied
- `5`: Invalid argument
- `6`: Timeout
- `7`: Memory allocation error
- `8`: Dependency missing
- `9`: Validation failed
- `10`: Profile failed
- `11`: Analysis failed
- `12`: Command not supported

## Troubleshooting

### Common Issues

1. **Configuration error**: Validate configuration file
   ```bash
   nlm config validate ./config.yaml
   ```

2. **File not found**: Check file existence
   ```bash
   ls -la ./config.yaml
   ```

3. **Permission denied**: Check file permissions
   ```bash
   chmod 755 ./config.yaml
   ```

4. **Memory allocation error**: Reduce simulation size
   ```bash
   nlm run --neurons=500 --config=./config.yaml
   ```

5. **Timeout**: Increase timeout
   ```bash
   nlm run --timeout=300 --config=./config.yaml
   ```

### Debugging Tips

```bash
# Enable detailed logging
nlm run --config=./config.yaml --verbose

# Run with validation
nlm run --config=./config.yaml --validate

# Check system resources
free -h  # Memory usage
df -h    # Disk space
```

## Additional Resources

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM Examples Repository](https://github.com/nlm-project/examples)
- [NLM Community Forum](https://community.nlm.org)
- [NLM Discord Server](https://discord.gg/nlm)

## Quick Start

1. **Get Started**: `nlm config list` to see available options
2. **Create Config**: Use `nlm config set` to configure parameters
3. **Run Simulation**: `nlm run --config=./config.yaml`
4. **Analyze Results**: `nlm analyze ./results.brain`
5. **Visualize**: `nlm visualize ./results.brain --output=./visual.png`

The NLM command line interface provides comprehensive tools for neural simulation management, analysis, and automation.