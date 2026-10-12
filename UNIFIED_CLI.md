# NLM Unified CLI

## Overview

The NLM (Neural Learning Machine) Unified CLI is a comprehensive command-line interface that unifies all existing NLM executables into a single powerful tool. This unified CLI provides:

- **Modern CLI Framework**: Built with a flexible, extensible command-line parsing system
- **Multiple Phases**: Support for Phase 2, Phase 3, Phase 4, Phase 6, and unified "run" commands
- **Configuration Options**: Common options for all commands (neuron count, steps, config file, log level, output)
- **Experiment Selection**: Ability to run specific experiments within phases
- **Results Output**: Structured output in JSON, YAML, or formatted text
- **Help System**: Comprehensive help for all subcommands and options
- **Advanced Features**: Profiling, checkpointing, debugging, and custom configuration

## Installation

The unified CLI is built as part of the NLM project. After building the NLM system:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

The unified CLI executable `nlm` will be created in the build directory.

## Usage

### Basic Usage

```bash
# Show help
nlm --help

# Show help for specific subcommand
nlm phase2 --help
nlm phase3 --help
nlm phase4 --help
nlm phase6 --help
nlm run --help
```

### Common Options

All subcommands share these common options:

```bash
-h, --help              Show help message
-c, --config <file>     Configuration file (default: config.cfg)
--log-level <level>     Log level: debug, info, warn, error (default: info)
--neuron-count <n>      Number of neurons (default: 500)
--steps <n>             Number of simulation steps (default: 200)
--output-format <fmt>   Output format: json, yaml, text (default: text)
--profile               Enable profiling
--checkpoints           Enable checkpointing
--debug                 Enable debugging mode
```

### Phase 2: Real Neural Computation

```bash
# Basic Phase 2 execution
nlm phase2

# With custom neuron count and steps
nlm phase2 --neuron-count 1000 --steps 500

# With configuration file
nlm phase2 --config myconfig.cfg --output-format json

# With profiling and debugging
nlm phase2 --profile --debug --log-level debug
```

### Phase 3: World Interaction

```bash
# Basic Phase 3 demo
nlm phase3

# With custom episodes and steps per episode
nlm phase3 --episodes 10 --steps-per-episode 500

# JSON output for programmatic processing
nlm phase3 --output-format json --episodes 3
```

### Phase 4: Emerging Cognition

```bash
# Basic Phase 4 demo
nlm phase4

# With custom number of trials
nlm phase4 --trials 100

# YAML output for configuration
nlm phase4 --output-format yaml

# Run specific experiments
nlm phase4 --experiment temporal_prediction
```

### Phase 6: Integration Test

```bash
# Basic Phase 6 integration test
nlm phase6

# With custom neuron count and max steps
nlm phase6 --neurons 750 --max-steps 5000

# Enable checkpoints
nlm phase6 --checkpoints

# YAML output
nlm phase6 --output-format yaml
```

### Unified Run Command

The `run` command provides unified access to experiments across phases:

```bash
# Run all experiments
nlm run

# Run specific experiment
nlm run --experiment temporal_prediction
nlm run --experiment working_memory

# Run experiments with custom settings
nlm run --experiment concept_formation --neuron-count 1200

# Batch processing
nlm run --experiment batch --experiments exp1,exp2,exp3
```

## Output Formats

### Text (Default)

Human-readable formatted output:

```
+---------------+----------+
| Parameter     | Value    |
+---------------+----------+
| phase         | phase2   |
| neurons       | 500      |
| steps         | 200      |
| status        | completed|
+---------------+----------+
```

### JSON

Machine-readable JSON output:

```json
{
  "phase": "phase2",
  "neurons": 500,
  "steps": 200,
  "status": "completed"
}
```

### YAML

Structured YAML output:

```yaml
phase: phase2
neurons: 500
steps: 200
status: completed
```

## Configuration File

The CLI supports configuration files with the following format:

```ini
[simulation]
neuron_count = 1000
steps = 500
random_seed = 42

[brain]
region_count = 2
connection_probability = 0.1

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
```

Use the `--config` option to specify a configuration file:

```bash
nlm phase2 --config myconfig.ini
```

Configuration file values can be overridden by command line options.

## Advanced Features

### Profiling

Enable performance profiling:

```bash
nlm phase2 --profile
```

Profiling data is output to `profile.json` in the current directory.

### Checkpointing

Enable checkpoint save/load:

```bash
nlm phase6 --checkpoints
```

Checkpoints are saved to `checkpoint.bin` and can be restored:

```bash
nlm phase6 --checkpoints --load-checkpoint checkpoint.bin
```

### Debugging

Enable debugging mode for verbose output:

```bash
nlm phase2 --debug --log-level debug
```

Debug output includes detailed neural activity, synaptic changes, and system metrics.

### Experiment Selection

Run specific experiments within phases:

```bash
# Phase 4 experiments
nlm phase4 --experiment temporal_prediction
nlm phase4 --experiment working_memory
nlm phase4 --experiment episodic_recall

# Multiple experiments
nlm phase4 --experiment temporal_prediction,working_memory,episodic_recall
```

### Batch Processing

Run multiple experiment runs:

```bash
nlm run --experiment batch \
  --experiments exp1,exp2,exp3 \
  --neuron-count 500 \
  --steps 1000 \
  --output-format json > results.json
```

## Programmatic API

The CLI provides programmatic access through the NLM API:

```cpp
#include "cli/CLI.hpp"

int main(int argc, char* argv[]) {
    nlm::UnifiedCLI cli;
    if (cli.initialize(argc, argv)) {
        cli.run();
    }
    return 0;
}
```

The CLI framework is designed to be integrated into larger applications while maintaining full backward compatibility.

## Backward Compatibility

The unified CLI maintains backward compatibility with existing demo executables:

- **nlm_phase3_demo**: Now uses unified CLI via `nlm phase3`
- **nlm_phase4_demo**: Now uses unified CLI via `nlm phase4`
- **nlm_phase6_demo**: Now uses unified CLI via `nlm phase6`

Existing scripts using the old executables will continue to work with the unified CLI.

## Command Completion

Tab completion is supported for the unified CLI:

```bash
# For bash
nlm --generate-completion bash > nlm-completion.bash
source nlm-completion.bash

# For zsh
nlm --generate-completion zsh > nlm-completion.zsh
source nlm-completion.zsh
```

## Troubleshooting

### Command not found

Ensure the unified CLI is built:

```bash
cd build
./nlm --help
```

### Configuration file not found

Create a default configuration file or specify a valid path:

```bash
nlm phase2 --config configs/default.cfg
```

### Permission denied

Check file permissions:

```bash
chmod +x nlm
```

### Help not showing

Ensure you're using the correct syntax:

```bash
nlm --help  # Shows main help
nlm phase2 --help  # Shows phase2-specific help
```

## Examples

### Complete Example

Run a comprehensive experiment with profiling and output:

```bash
nlm run \
  --experiment batch \
  --experiments temporal_prediction,working_memory,concept_formation \
  --neuron-count 1000 \
  --steps 2000 \
  --profile \
  --output-format json \
  > comprehensive_results.json
```

### Integration Test

Run a Phase 6 integration test with all features:

```bash
nlm phase6 \
  --neurons 750 \
  --max-steps 5000 \
  --checkpoints \
  --debug \
  --log-level info
```

## License

MIT License. See LICENSE file for details.

## Authors

NLM Research Team

For issues and feedback, please visit the NLM GitHub repository.
