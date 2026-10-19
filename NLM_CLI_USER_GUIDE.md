# NLM CLI Tool - User Guide

## Overview

The NLM (Neuro-Linguistic Modeling) CLI Tool is a comprehensive command-line interface for interacting with the enhanced NLM Python API. It provides a unified interface for all NLM simulation operations, from basic brain/world setup to advanced features like batch processing, checkpointing, and performance benchmarking.

## Features

### 1. Basic Operations
- **Initialize System**: Create brain, agent brain, world with configuration
- **Run Simulation**: Execute single episodes or complete learning sessions
- **World Management**: Configure world parameters, agent start position, and random seed

### 2. Enhanced Features
- **Episode Management**: Run individual or batch episodes with automatic tracking
- **Checkpoint Operations**: Save/load simulation states for resumption
- **Metrics Collection**: Automatic performance metrics collection and reporting
- **Performance Profiling**: Built-in profiling for performance analysis
- **Learning Session Control**: Manage multi-episode learning sessions

### 3. Configuration
- **Command Line Arguments**: Comprehensive CLI argument support
- **Configuration Files**: JSON configuration file support
- **Environment Variables**: Environment variable configuration
- **Validation and Help**: Built-in validation with detailed help documentation

### 4. Output
- **Human-Readable**: Formatted, readable output for console display
- **Machine-Readable**: JSON output for automation and integration
- **Progress Indicators**: Real-time progress tracking
- **Statistics and Metrics**: Comprehensive statistics and metrics reporting
- **Logging Options**: Configurable logging levels and destinations

### 5. Advanced Features
- **Batch Processing**: Process multiple simulations in parallel or sequence
- **Parameter Tuning**: Automated parameter optimization
- **Performance Benchmarking**: Performance measurement and comparison
- **Experiment Comparison**: Compare results across different experiments
- **Visualization Export**: Export data for visualization tools

## Installation

### Prerequisites
- Python 3.8 or higher
- pip (Python Package Installer)

### Installation Steps
1. Clone the repository (if applicable)
2. Navigate to the project directory
3. Install required Python packages:

```bash
pip install numpy pandas tqdm
```

4. Make the CLI tool executable:

```bash
chmod +x nlm_cli.py
```

## Usage

### Basic Usage

#### Help
```bash
python3 nlm_cli.py --action help
```

#### Initialize System
```bash
python3 nlm_cli.py --action init
```

#### Run Simulation
```bash
python3 nlm_cli.py --action run --episodes 10 --steps 100
```

#### Checkpoint Management
```bash
python3 nlm_cli.py --action checkpoint --name training_session
python3 nlm_cli.py --action load --file checkpoint.json
```

### Configuration

#### Command Line Configuration
```bash
python3 nlm_cli.py \
  --action run \
  --episodes 20 \
  --steps 50 \
  --neurons 500 \
  --regions 5 \
  --seed 42 \
  --enable-profiling \
  --output-format json \
  --output-file results.json
```

#### Configuration File
Create a configuration file (`config.json`):

```json
{
    "brain_neurons": 500,
    "brain_regions": 5,
    "world_width": 50,
    "world_height": 50,
    "vision_width": 16,
    "vision_height": 16,
    "learning_rate": 0.01,
    "exploration_rate": 0.15,
    "reward_shaping": 0.8,
    "curiosity_exploration": 0.3,
    "max_steps_per_episode": 50,
    "episode_count": 10,
    "checkpoint_interval": 5,
    "enable_debug": false,
    "enable_profiling": true,
    "random_seed": 42
}
```

Then run with the configuration file:
```bash
python3 nlm_cli.py --action init --config-file config.json
```

#### Environment Variables
You can use environment variables for configuration:

```bash
export NLM_EPISODES=10
export NLM_STEPS=100
export NLM_NEUTRONS=1000
python3 nlm_cli.py --action run
```

### Actions

#### `init`
Initialize the NLM system with current configuration

#### `run`
Run a learning session or simulation

#### `checkpoint`
Save current simulation state as checkpoint

#### `load`
Load simulation state from checkpoint

#### `export`
Export results to file in specified format (JSON, CSV)

#### `batch`
Run batch processing with multiple simulations

#### `benchmark`
Run performance benchmark tests

#### `compare`
Compare results from multiple experiments/checkpoints

#### `tune`
Perform automated parameter tuning

### Output Formats

#### Human-Readable
Default format for console output. Includes:
- Progress indicators
- Episode summaries
- Performance statistics
- Learning trends

#### Machine-Readable (JSON)
For programmatic access and integration:
- Complete simulation data
- Performance metrics
- Episode details
- Configuration information

#### CSV
For spreadsheet and data analysis:
- Step-by-step data
- Episode results
- Performance metrics
- Time series data

## File Formats

### JSON Output
```json
{
    "session_info": {
        "total_episodes": 10,
        "total_steps": 500,
        "total_rewards": 125.5,
        "average_reward_per_episode": 12.55,
        "average_steps_per_episode": 50.0
    },
    "episodes": [...],
    "performance_metrics": [...],
    "checkpoints": [...]
}
```

### CSV Output
Columns:
- episode_id, step, action, reward, neuromodulation_level, curiosity_level, novelty_level, prediction_error, episode_score, step_time, neurons_active, neurons_firing

### Configuration Files
JSON format with snake_case keys for all configuration options

## Configuration Options

### Basic Parameters
- `episodes`: Number of episodes to run (default: 10)
- `steps`: Steps per episode (default: 100)
- `neurons`: Number of brain neurons (default: 1000)
- `regions`: Number of brain regions (default: 10)

### World Configuration
- `world_width`, `world_height`: World dimensions
- `vision_width`, `vision_height`: Agent vision sensor size

### Learning Parameters
- `learning_rate`: Learning rate for neural plasticity
- `exploration_rate`: Exploration rate for behavior
- `reward_shaping`: Reward shaping factor
- `curiosity_exploration`: Curiosity-driven exploration level

### Advanced Options
- `checkpoint_interval`: Save checkpoint every N episodes
- `enable_profiling`: Enable performance profiling
- `enable_debug`: Enable debug mode
- `enable_tuning`: Enable parameter tuning

### Output Options
- `output_format`: Output format (human, json, csv, table)
- `output_file`: Output file path
- `log_level`: Logging level (DEBUG, INFO, WARNING, ERROR)

## Examples

### Example 1: Basic Simulation
```bash
python3 nlm_cli.py --action init --episodes 5 --steps 50
```

### Example 2: Batch Processing
```bash
python3 nlm_cli.py --action batch --batch-size 5 --parameter-tune
```

### Example 3: Performance Benchmark
```bash
python3 nlm_cli.py --action benchmark --benchmark-iterations 10
```

### Example 4: Experiment Comparison
```bash
python3 nlm_cli.py --action compare --compare-files checkpoint1.json checkpoint2.json
```

### Example 5: Export Results
```bash
python3 nlm_cli.py --action run --episodes 10 --output-format json --output-file results.json
```

## Troubleshooting

### Common Issues

#### Module Not Found
If you encounter "ModuleNotFoundError: No module named 'nlm'", ensure the NLM module is installed:

```bash
pip install pynlm
```

#### Permission Errors
If you encounter permission errors when running the CLI tool:

```bash
chmod +x nlm_cli.py
```

#### Configuration File Errors
If your configuration file has errors, check the JSON syntax:

```bash
python3 -m json.tool config.json
```

### Getting Help

For detailed help on any action:

```bash
python3 nlm_cli.py --action help
```

For help on specific arguments:

```bash
python3 nlm_cli.py --help
```

### Logging

The CLI tool generates logs to both console and file (`nlm_cli.log`). For debugging, set the log level to DEBUG:

```bash
python3 nlm_cli.py --action run --log-level DEBUG
```

## License

This tool is provided under the MIT License. See LICENSE file for details.

## Contributing

Contributions to the NLM CLI Tool are welcome. Please follow the existing code style and add comprehensive tests.

## Support

For support, please refer to the documentation or contact the development team.

---

# NLM CLI Tool - Advanced Features

## Batch Processing

Batch processing allows running multiple simulations efficiently. Use the `--batch-size` parameter:

```bash
python3 nlm_cli.py --action batch --batch-size 5
```

## Parameter Tuning

Automated parameter tuning can optimize simulation parameters:

```bash
python3 nlm_cli.py --action tune --parameter-tune
```

## Experiment Comparison

Compare results from multiple experiments:

```bash
python3 nlm_cli.py --action compare --compare-files exp1_checkpoint.json exp2_checkpoint.json
```

## Visualization Export

Export data for visualization tools:

```bash
python3 nlm_cli.py --action run --episodes 10 --output-format json --output-file visualization_data.json
```

---

The NLM CLI Tool provides a comprehensive interface for NLM simulation management, combining ease of use with powerful advanced features for research and production use.