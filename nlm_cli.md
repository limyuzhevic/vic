# NLM (Neural Learning Machine) - Command Line Interface

NLM CLI provides comprehensive control over neural simulations, experiments, and brain management from the command line.

## Installation

```bash
# Install the CLI tool
pip install .

# Or in development mode
pip install -e .

# The CLI command will be available as 'nlm'
```

## Usage

```bash
# Show help
nlm --help

# Run basic simulation
nlm simulate --config config.json

# Run Phase 3 demo
nlm demo phase3

# Run Phase 4 experiment
nlm experiment phase4 --episodes 10

# Run Phase 6 integration test
nlm experiment phase6

# Save brain state
nlm brain save brain.bin

# Load brain state
nlm brain load brain.bin

# Show statistics
nlm stats
```

## Commands

### Global Options

```bash
-n, --config PATH            Configuration file path
-v, --verbose                Enable verbose output
--quiet                      Suppress non-essential output
--log-level LEVEL            Set logging level (trace, debug, info, warn, error)
--help                       Show help for command
--version                    Show version information
```

### `nlm init [OPTIONS]` - Initialize NLM project

Initialize a new NLM project with configuration files.

```bash
# Create new project directory
nlm init my_project

# Create with default configuration
nlm init my_project --type demo

# List available project types
nlm init --types
```

### `nlm config [COMMAND]` - Configuration management

Manage NLM configuration files and settings.

```bash
# Show current configuration
nlm config show

# Set configuration value
nlm config set brain.neuron_count 1000

# Load configuration from file
nlm config load config.json

# Save configuration to file
nlm config save config.json

# List all configuration keys
nlm config list

# Validate configuration
nlm config validate

# Show configuration help
nlm config --help
```

### `nlm brain [COMMAND]` - Brain manipulation

Manage neural brains and their states.

```bash
# Create new brain
nlm brain create --name "my_brain" --neurons 1000

# Initialize brain
nlm brain init

# Load brain from checkpoint
nlm brain load brain.bin

# Save brain to checkpoint
nlm brain save brain.bin

# Show brain statistics
nlm brain stats

# Reset brain
nlm brain reset

# Show brain help
nlm brain --help
```

### `nlm simulate [OPTIONS]` - Run neural simulation

Run continuous neural simulation with optional world interaction.

```bash
# Run basic simulation
nlm simulate --steps 1000 --dt 0.001

# Run with world simulation
nlm simulate --world --world-width 20 --world-height 20

# Run with agent
nlm simulate --agent --enable-reward --enable-curiosity

# Run with periodic statistics
nlm simulate --steps 5000 --report-interval 100

# Save checkpoint periodically
nlm simulate --checkpoint interval=1000 file=brain_checkpoint.bin
```

### `nlm experiment [COMMAND]` - Run experiments

Run predefined NLM experiments and analysis.

```bash
# Run Phase 3 world interaction demo
nlm experiment phase3 --episodes 5 --steps-per-episode 200

# Run Phase 4 cognitive experiments
nlm experiment phase4 --trials 50 --experiment temporal_prediction

# Run Phase 6 integration test
nlm experiment phase6 --neurons 500 --steps 2000

# Run all experiments
nlm experiment run-all

# Export experiment results
nlm experiment phase4 export results.json
```

### `nlm debug [COMMAND]` - Debugging tools

Advanced debugging and analysis tools for NLM development.

```bash
# Monitor system performance
nlm debug profile --seconds 30

# Visualize brain connectivity
nlm debug visualize --output connectivity.png

# Analyze spike patterns
nlm debug spikes --threshold 0.5 --window 1000

# Check memory usage
nlm debug memory --monitor

# Profile performance
nlm debug profile --output profile.txt

# Show debug help
nlm debug --help
```

### `nlm admin [COMMAND]` - Administrative functions

Administrative and maintenance functions.

```bash
# List all checkpoints
nlm admin checkpoints list

# Clean old checkpoints
nlm admin checkpoints clean --older-than 7d

# Create backup
nlm admin backup --output backup.tar.gz

# Restore system
nlm admin restore backup.tar.gz

# Show system status
nlm admin status
```

### `nlm example [TYPE]` - Generate examples

Generate example configurations, scripts, and demos.

```bash
# Generate example config
nlm example config --output example_config.json

# Generate Python script
nlm example python --output agent_example.py

# Generate demo script
nlm example demo --type phase3 --output my_demo.py

# Generate complete tutorial
nlm example tutorial --output tutorial.md
```

## Configuration File Format

NLM uses JSON configuration files. Here's an example:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.01
            }
        }
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    },
    "agent": {
        "enable_reward_modulation": true,
        "enable_structural_plasticity": true,
        "enable_development": true,
        "enable_curiosity": true
    }
}
```

## Environment Variables

Set these environment variables for NLM configuration:

```bash
export NLM_CONFIG_PATH=/path/to/config.json
export NLM_LOG_LEVEL=info
export NLM_CHECKPOINT_DIR=/path/to/checkpoints
export NLM_DATA_DIR=/path/to/data
```

## Configuration Templates

```bash
# Create template configuration
nlm example config --template standard > config.json

# Create minimal configuration
nlm example config --template minimal > minimal_config.json

# Create advanced configuration
nlm example config --template advanced > advanced_config.json
```

## Advanced Usage Examples

### Running with Custom Configuration

```bash
nlm simulate \
  --config my_config.json \
  --world \
  --agent \
  --enable-reward \
  --enable-curiosity \
  --enable-development \
  --checkpoint interval=500 file=progress.bin
```

### Running Batch Experiments

```bash
#!/bin/bash
# run_batch_experiments.sh

for neurons in 100 500 1000 2000; do
    echo "Running experiment with $neurons neurons"
    nlm experiment phase6 \
        --neurons $neurons \
        --steps 1000 \
        --checkpoint \
        --checkpoint-interval 200 \
        --checkpoint-file "results_${neurons}_neurons.bin"
    
    # Run analysis
    nlm analyze \
        --input "results_${neurons}_neurons.bin" \
        --output "analysis_${neurons}_neurons.json"
done
```

### Debugging with Real-Time Monitoring

```bash
# Start simulation with monitoring
nlm simulate \
  --steps 10000 \
  --world \
  --agent \
  --enable-reward \
  --enable-curiosity \
  --report-interval 100 \
  --debug \
  --monitor-memory \
  --monitor-cpu \
  --monitor-spikes
```

## Installation and Setup

### Python Installation

```bash
# Install dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install
pip install .

# Verify installation
nlm --version
```

### Development Installation

```bash
# Clone repository
# git clone <repository-url>
# cd nlm

# Install in development mode
pip install -e .

# Build C++ components (if needed)
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Configuration File Examples

```bash
# Create configuration directory structure
mkdir -p ~/.config/nlm

# Create default configuration
nlm example config > ~/.config/nlm/default.json

# Create project-specific configuration
nlm example config --template advanced > my_project/config.json
```

## Additional Commands (Advanced)

### `nlm analyze` - Analysis tools

Analyze simulation results and generate reports.

```bash
nlm analyze --input brain.bin --output report.json --format markdown

nlm analyze --compare baseline.bin current.bin --output diff_report.txt
```

### `nlm visualize` - Visualization tools

Create visual representations of brain states and data.

```bash
nlm visualize --input brain.bin --output graph.png --type connectivity

nlm visualize --input experiment_results.json --output dashboard.html
```

### `nlm benchmark` - Performance benchmarks

Run performance benchmarks on neural simulations.

```bash
nlm benchmark --neurons 1000 --steps 1000 --repeat 5

nlm benchmark --compare 1000_neurons 5000_neurons
```

## Error Codes

- `0`: Success
- `1`: General error
- `2`: Invalid command or arguments
- `3`: Configuration file not found
- `4`: Brain initialization failed
- `5`: Simulation failed
- `6`: Experiment failed
- `7`: File I/O error
- `8`: Memory allocation error
- `9`: Timeout or interruption

## Completion

Tab completion is supported for most commands:

```bash
# Bash completion
nlm bash-completion

# Zsh completion  
zsh-completion
```

The NLM CLI provides a powerful interface for controlling neural simulations,
running experiments, and managing complex brain systems from the command line.
