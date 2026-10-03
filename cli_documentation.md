# NLM Command-Line Interface (CLI) Documentation

## Overview

The NLM project features an enhanced command-line interface (CLI) that provides unified access to all experiments, demonstrations, and research workflows. The CLI is designed for both interactive use and automated scripting.

## CLI Architecture

The NLM CLI follows a hierarchical structure:

```
nlm <command> [options]
    └── demo phase3 --episodes 10 --steps 500
    └── experiment phase4 --config config.json
    └── train --episodes 500 --env-size 50,50
    └── analyze brain.bin --metrics all
    └── visualize brain --output brain_network.png
    └── benchmark --suite scaling
    └── sweep phase3 --parameters _sweep_params.json
    └── export config --output config.json
    └── import brain --input saved_brain.bin
    └── python train --script script.py
```

## Core Commands

### 1. demo - Run Demonstrations

**Description**: Run pre-built demonstrations showing different phases of NLM functionality.

**Usage**:
```bash
nlm demo <mode> [options]
```

**Arguments**:
- `<mode>`: Demo mode (phase3, phase4, phase6, integrated)

**Options**:
- `--episodes N`: Number of episodes to run (default: 10)
- `--steps N`: Maximum steps per episode (default: 500)
- `--help`: Show help message
- `--version`: Show version information
- `--log-level <L>`: Set logging level (debug, info, warning, error, fatal)
- `--config <file>`: Path to configuration file
- `--output-dir <dir>`: Output directory for results
- `--quiet`: Suppress non-error output
- `--json`: Output results as JSON format
- `--verbose`: Enable verbose output
- `--profile`: Enable performance profiling

**Examples**:
```bash
# Run Phase 3 demo (world interaction)
nlm demo phase3 --episodes 10 --steps 500

# Run Phase 4 demo (emerging cognition)
nlm demo phase4 --episodes 5 --steps 200

# Run Phase 6 demo (integration test)
nlm demo phase6 --episodes 5 --steps 2000

# Run comprehensive Phase 6 integration demo
nlm demo integrated --episodes 3 --steps 1000 --verbose

# Run with JSON output for programmatic analysis
nlm demo phase6 --episodes 5 --steps 2000 --json --output-dir ./demo_results
```

### 2. experiment - Run Specific Experiments

**Description**: Run controlled experiments with specific hypotheses and measurement protocols.

**Usage**:
```bash
nlm experiment <mode> [options]
```

**Arguments**:
- `<mode>`: Experiment mode (phase3, phase4, phase5, phase6)

**Options**:
- Same options as `demo` command plus:
- `--config <file>`: Path to experiment configuration
- `--ablation`: Run ablation study
- `--analysis`: Enable detailed analysis
- `--replications N`: Number of replications (default: 1)

**Examples**:
```bash
# Run Phase 3 experiment
nlm experiment phase3 --episodes 5 --steps 1000

# Run Phase 4 experiment with configuration
nlm experiment phase4 --episodes 3 --steps 500 --config phase4_config.json

# Run Phase 5 experiment (meta-learning)
nlm experiment phase5 --episodes 10 --steps 1500

# Run Phase 6 integration experiment with analysis
nlm experiment phase6 --episodes 3 --steps 2000 --analysis --output-dir ./experiment_results

# Run ablation study
nlm experiment phase4 --episodes 5 --steps 1000 --ablation
```

### 3. train - Train Agent Models

**Description**: Train agent models using reinforcement learning algorithms.

**Usage**:
```bash
nlm train [options]
```

**Options**:
- `--episodes N`: Number of training episodes (default: 500)
- `--env-size <W,H>`: Environment size (default: 50,50)
- `--agent-type <type>`: Agent type (actor_critic, ddpg, a2c)
- `--learning-rate <LR>`: Learning rate (default: 0.001)
- `--gamma <G>`: Discount factor (default: 0.99)
- `--output-model <file>`: Path to save trained model
- `--load-model <file>`: Path to load pre-trained model
- `--entropy-coef <C>`: Entropy regularization coefficient
- `--target-update <T>`: Target network update frequency (for DDPG)
- `--buffer-size <B>`: Replay buffer size (for DDPG, A2C)
- `--seed <S>`: Random seed for reproducibility
- `--device <D>`: Device to use (cpu, cuda, cuda:0)

**Examples**:
```bash
# Train actor-critic agent
nlm train --episodes 500 --env-size 50,50 --agent-type actor_critic

# Train DDPG agent with custom parameters
nlm train --episodes 1000 --env-size 30,30 --agent-type ddpg \
    --learning-rate 0.0005 --gamma 0.99 --output-model ddpg_agent.bin

# Train A2C agent with entropy regularization
nlm train --episodes 800 --env-size 40,40 --agent-type a2c \
    --learning-rate 0.0005 --entropy-coef 0.01

# Load and continue training
nlm train --episodes 200 --env-size 50,50 --agent-type actor_critic \
    --load-model previous_model.bin --output-model improved_model.bin
```

### 4. analyze - Analyze Brain States and Results

**Description**: Analyze brain states, experimental results, and performance metrics.

**Usage**:
```bash
nlm analyze <input> [options]
```

**Arguments**:
- `<input>`: Input file (brain state, results, or data)

**Options**:
- `--format <fmt>`: Input format (brain, results, network)
- `--metrics <list>`: Comma-separated list of metrics (all, basic, advanced, all)
- `--output <file>`: Output file for analysis results
- `--statistics`: Enable statistical analysis
- `--visualization`: Generate visualizations
- `--comparative`: Compare with reference data
- `--threshold <T>`: Analysis threshold
- `--report`: Generate detailed report

**Examples**:
```bash
# Analyze brain state file
nlm analyze brain_state.bin --format brain --metrics all --output analysis.json --statistics

# Analyze experimental results
nlm analyze results.json --format results --metrics advanced --visualization --output visualization.png

# Analyze network structure
nlm analyze network_data.json --format network --metrics all --output network_analysis.json

# Comparative analysis with reference
nlm analyze current_state.bin --format brain --comparative --threshold 0.95

# Generate detailed report
nlm analyze brain.bin --format brain --metrics all --report --output detailed_report.pdf
```

### 5. visualize - Create Visualizations

**Description**: Create visualizations (brain, network, training, results) for analysis and presentation.

**Usage**:
```bash
nlm visualize <type> [options]
```

**Arguments**:
- `<type>`: Visualization type (brain, network, training, results)

**Options**:
- `--input <file>`: Input data file
- `--output <file>`: Output file path
- `--format <fmt>`: Output format (png, html, svg, json)
- `--width <W>`: Output width (default: 1200)
- `--height <H>`: Output height (default: 800)
- `--interactive`: Create interactive visualization
- `--quality <Q>`: Image quality (low, medium, high)
- `--theme <theme>`: Color theme (default, dark, scientific)
- `--title <T>`: Visualization title
- `--colormap <CM>`: Colormap for heatmaps

**Examples**:
```bash
# Create interactive brain network visualization
nlm visualize brain --input brain.bin --output brain_network.html --interactive --width 1600 --height 1200

# Create training performance visualization
nlm visualize training --input training_data.json --output learning_curves.png --format png

# Create simple network visualization
nlm visualize network --input network_data.json --output network.png --format png --quality high

# Create comparative visualization
nlm visualize results --input results.json --output comparison.png --format png --theme dark
```

### 6. benchmark - Run Performance Benchmarks

**Description**: Run performance benchmarks and scaling experiments.

**Usage**:
```bash
nlm benchmark [options]
```

**Options**:
- `--suite <suite>`: Benchmark suite (scaling, integration, memory)
- `--sizes <list>`: Comma-separated list of neuron counts (e.g., 100,500,1000,5000)
- `--runs <N>`: Number of runs per configuration (default: 10)
- `--detailed`: Enable detailed measurements
- `--output <file>`: Output file for benchmark results
- `--csv`: Output results as CSV
- `--json`: Output results as JSON
- `--statistical`: Include statistical analysis
- `--plot`: Generate benchmark plots

**Examples**:
```bash
# Run comprehensive scaling benchmark
nlm benchmark --suite scaling --sizes 100,500,1000,5000,10000 --runs 10 --detailed

# Run integration benchmarks
nlm benchmark --suite integration --output integration_benchmarks.json

# Run memory benchmarks
nlm benchmark --suite memory --sizes 1000,5000,10000 --runs 5 --csv

# Generate benchmark plots
nlm benchmark --suite scaling --sizes 100,500,1000,5000 --plot --output benchmark_plots.png
```

### 7. sweep - Run Parameter Sweeps

**Description**: Run parameter sweeps for research and optimization.

**Usage**:
```bash
nlm sweep <mode> [options]
```

**Arguments**:
- `<mode>`: Experiment mode to sweep (phase3, phase4, phase5, phase6)

**Options**:
- `--parameters <file>`: Path to parameter sweep configuration
- `--name <name>`: Name for this sweep
- `--output-dir <dir>`: Output directory for sweep results
- `--parallel <N>`: Number of parallel runs (default: 1)
- `--resume`: Resume interrupted sweep
- `--verify`: Verify parameter combinations
- `--analysis`: Perform post-sweep analysis
- `--optimization`: Enable optimization recommendations

**Examples**:
```bash
# Run parameter sweep for Phase 3
nlm sweep phase3 --parameters _sweep_params.json --output-dir ./sweep_results

# Run custom sweep with specific ranges
nlm sweep phase4 --parameters custom_sweep.json --name "neuron_study"

# Run parallel parameter sweep
nlm sweep phase3 --parameters _sweep_params.json --parallel 4 --name "large_scale_study"
```

### 8. export - Export Configurations or Data

**Description**: Export configurations, brain states, or experimental results.

**Usage**:
```bash
nlm export <type> [options]
```

**Arguments**:
- `<type>`: Export type (config, brain, agent)

**Options**:
- `--input <file>`: Input file to export
- `--output <file>`: Output file path
- `--format <fmt>`: Export format (json, xml, txt, binary)
- `--metadata`: Include metadata in export
- `--compress`: Compress output file
- `--version`: Version to export (default: latest)
- `--tags <list>`: Tags to filter

**Examples**:
```bash
# Export configuration
nlm export config --input configs/default.cfg --output python_config.json --format json --metadata

# Export brain state
nlm export brain --input saved_brain.bin --output brain_data.json --format json --compress

# Export agent configuration
nlm export agent --input agent_config.json --output agent_export.xml --format xml

# Export with versioning
nlm export brain --input model.bin --output versioned_model.json --version 1.0 --metadata
```

### 9. import - Import Configurations or Data

**Description**: Import configurations, brain states, or experimental results.

**Usage**:
```bash
nlm import <type> [options]
```

**Arguments**:
- `<type>`: Import type (config, brain, agent)

**Options**:
- `--input <file>`: Input file to import
- `--output <file>`: Output file path (where to save imported data)
- `--format <fmt>`: Input format (json, xml, txt, binary)
- `--validate`: Validate imported data
- `--migrate`: Migrate legacy format to current format
- `--version`: Target version for import

**Examples**:
```bash
# Import and validate brain state
nlm import brain --input saved_brain.bin --output restored_brain.bin --validate

# Import configuration with migration
nlm import config --input old_config.xml --output new_config.json --format xml --migrate

# Import with versioning
nlm import brain --input model.json --output current_model.bin --version 1.0
```

### 10. python - Hybrid Python/C++ Integration

**Description**: Hybrid Python/C++ integration for complex workflows.

**Usage**:
```bash
nlm python <command> [options]
```

**Arguments**:
- `<command>`: Python command (train, simulate, analyze, export)

**Options**:
- `--script <file>`: Path to Python script
- `--config-file <file>`: Path to Python configuration file
- `--model-file <file>`: Path to model file
- `--output-dir <dir>`: Output directory
- `--iterations <N>`: Number of iterations (default: 100)
- `--verbose`: Enable verbose output

**Examples**:
```bash
# Train agent using hybrid Python/C++ workflow
nlm python train --script python/integration_script.py --model-file agent_model.bin

# Run Python-based simulation
nlm python simulate --script simulation.py --config-file simulation_config.json

# Analyze results with Python
nlm python analyze --script python/advanced_analysis.py --output-dir ./analysis_results
```

### 11. help - Show Help Information

**Description**: Show help information for commands and options.

**Usage**:
```bash
nlm help [command]
```

**Arguments**:
- `[command]`: Specific command to show help for (if omitted, shows general help)

**Examples**:
```bash
# Show general help
nlm help

# Show help for specific command
nlm help demo

# Show help for options
nlm help train --options
```

## Configuration Files

### Configuration File Format

NLM uses JSON format for configuration files. Examples:

```json
{
    "experiment": "phase3",
    "description": "Phase 3 parameter sweep - learning efficiency",
    
    "parameters": {
        "neuron_counts": [500, 1000, 2000, 5000],
        "learning_rates": [0.01, 0.05, 0.1, 0.2],
        "connection_probabilities": [0.05, 0.1, 0.2],
        "region_counts": [2, 4, 8],
        "plasticity_enabled": [true, false],
        "reward_modulation_enabled": [true, false]
    },
    
    "sweeps": {
        "full": {
            "type": "grid",
            "description": "Full factorial design of all parameters",
            "parameters": ["neuron_counts", "learning_rates", "connection_probabilities", "region_counts", "plasticity_enabled", "reward_modulation_enabled"]
        }
    },
    
    "output": {
        "format": "json",
        "directory": "./sweep_results",
        "prefix": "sweep_"
    },
    
    "defaults": {
        "trials": 20,
        "max_steps": 1000,
        "evaluation_metrics": ["learning_rate", "retention_score", "generalization_score", "total_reward"]
    }
}
```

### Command-Line Configuration

Many CLI commands accept common configuration options:

```bash
# Common configuration options
--help                    Show help message
--version                 Show version information
--log-level <L>           Set logging level (debug, info, warning, error, fatal)
--config <file>           Path to configuration file
--output-dir <dir>        Output directory for results
--quiet                   Suppress non-error output
--json                    Output results as JSON format
--verbose                 Enable verbose output
--profile                 Enable performance profiling
```

## Configuration File Examples

### 1. Basic Configuration (`configs/default.cfg`)

```ini
[brain]
neuron_count = 1000
region_count = 4
synapse_density = 0.1
connection_probability = 0.05
spike_threshold = -50.0
refractory_period = 10

[plasticity]
stardp_enabled = true
stardp_learning_rate = 0.001
stardp_time_constant = 20.0
hebbian_enabled = true
structural_enabled = true

[neuromodulation]
dopamine_scale = 1.0
curiosity_enabled = true
novelty_enabled = true
prediction_error_enabled = true

[development]
enabled = true
stage_duration = 1000

[memory]
working_memory_enabled = true
episodic_memory_enabled = true
associative_memory_enabled = true

[world]
width = 100
height = 100
vision_width = 64
vision_height = 64
max_energy = 100.0
energy_decay_rate = 0.01
reward_function = "task_completion"
```

### 2. Parameter Sweep Configuration (`_sweep_params.json`)

```json
{
    "experiment": "phase3",
    "description": "Phase 3 parameter sweep - learning efficiency",
    
    "parameters": {
        "neuron_counts": [500, 1000, 2000, 5000],
        "learning_rates": [0.01, 0.05, 0.1, 0.2],
        "connection_probabilities": [0.05, 0.1, 0.2],
        "region_counts": [2, 4, 8],
        "plasticity_enabled": [true, false],
        "reward_modulation_enabled": [true, false]
    },
    
    "sweeps": {
        "full": {
            "type": "grid",
            "description": "Full factorial design of all parameters",
            "parameters": ["neuron_counts", "learning_rates", "connection_probabilities", "region_counts", "plasticity_enabled", "reward_modulation_enabled"]
        },
        "random": {
            "type": "random",
            "description": "Random parameter sampling",
            "num_samples": 50,
            "seed": 42
        }
    },
    
    "output": {
        "format": "json",
        "directory": "./sweep_results",
        "prefix": "sweep_"
    },
    
    "defaults": {
        "trials": 20,
        "max_steps": 1000,
        "evaluation_metrics": ["learning_rate", "retention_score", "generalization_score", "total_reward"]
    }
}
```

## Quick Start Guide

### 1. Basic Usage

```bash
# Show help information
nlm --help

# Run a basic demo
nlm demo phase3 --episodes 10 --steps 500

# Run parameter sweep
nlm sweep phase3 --parameters _sweep_params.json

# Analyze results
nlm analyze brain_state.bin

# Create visualization
nlm visualize brain --input brain.bin
```

### 2. Advanced Usage

```bash
# Run comprehensive experiment with analysis
nlm experiment phase6 --episodes 5 --steps 2000 --analysis --output-dir ./experiment_results

# Train advanced agent with custom parameters
nlm train --episodes 1000 --env-size 50,50 --agent-type actor_critic \
    --learning-rate 0.001 --gamma 0.99 --entropy-coef 0.01

# Run hybrid Python/C++ integration
nlm python train --script python/integration_script.py
```

### 3. Python Integration

```python
# Quick Python integration start
import pynlm

# Create a medium-sized brain with plasticity
brain = pynlm.createMediumBrain()

# Create world
world = pynlm.createSimulationWorld(100, 100)

# Create agent brain
agent_brain = pynlm.createAgentBrainWithWorld(world)

# Enable all modules
agent_brain.enableAllModules()

# Run training episode (see Advanced Python Guide for details)
```

## System Requirements

### Minimum Requirements
- **CPU**: Dual-core processor (quad-core recommended)
- **RAM**: 4 GB (8 GB recommended for larger networks)
- **Storage**: 100 MB for installation, 1 GB for simulations
- **OS**: Linux, macOS, or Windows

### Recommended Requirements
- **CPU**: 8+ cores for parallel processing
- **RAM**: 16+ GB for large-scale simulations
- **Storage**: 10+ GB for comprehensive experiments
- **GPU**: NVIDIA GPU with CUDA support for acceleration

## Error Handling and Diagnostics

### Common Error Messages

1. **File Not Found**
   ```
   Error: File not found: 'specified_file.bin'
   Solution: Check file path and permissions
   ```

2. **Invalid Configuration**
   ```
   Error: Invalid configuration parameter
   Solution: Check configuration file format and parameters
   ```

3. **Memory Exhaustion**
   ```
   Error: Memory limit exceeded
   Solution: Reduce brain size or increase system memory
   ```

4. **CUDA Not Available**
   ```
   Error: CUDA device not found
   Solution: Install CUDA toolkit or use CPU mode
   ```

### Diagnostic Commands

```bash
# Check system resources
nlm --system-info

# Check CUDA availability
nlm --cuda-info

# Check Python environment
nlm python --check-environment

# Validate configuration
nlm validate --config configs/default.cfg
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain is not doing anything"
**Solution**: Ensure `brain.initialize()` is called before `brain.step()`

#### 2. "Agent isn't moving"
**Solution**: Check that `world.applyMotorCommand()` is being called

#### 3. "Everything is 0"
**Solution**: Brains need time to "warm up" - try more steps

#### 4. "Memory traces decay quickly"
**Solution**: Increase recurrent connectivity in brain configuration

#### 5. "Learning is too slow"
**Solution**: Check neuromodulation settings and plasticity parameters

### Support Resources

- [NLM Documentation Home](./README.md)
- [Phase 6 Integration Examples](./docs/PHASE6_ARCHITECTURE.md)
- [Performance Guidelines](./docs/PERFORMANCE.md)
- [Research Methods](./docs/RESEARCH_METHODS.md)
- [Troubleshooting Guide](./docs/FAILURES.md)

## Acknowledgments

This CLI documentation is based on the NLM Phase 6 integration achievements and represents the current state of the project. The documentation is continuously being updated as new features are developed and integrated.

## Version Information

- **Phase**: 6 - Final Integration
- **Version**: 1.0 (Phase 6 Complete)
- **CLI Version**: Advanced (with all experimental features)
- **Documentation**: Comprehensive (this document)

This completes the CLI documentation for the NLM project. The CLI is now a comprehensive, unified interface for all NLM operations, providing both basic functionality and advanced features for research and experimentation.