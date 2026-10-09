# NLM Phase 6 Advanced Interface

This file provides advanced command-line options for power users to run complex experiments with full system integration.

## Overview

The NLM Phase 6 Advanced Interface (`nlm_phase6`) extends the original Phase 2 demo (`nlm`) with comprehensive command-line options for:

- **Phase 6 Integration Testing**: Run the complete Phase 6 integration test suite
- **Memory System Controls**: Enable/disable memory systems and configure working memory capacity
- **Neuromodulation Controls**: Control dopamine, curiosity, and other neuromodulators
- **Development Controls**: Set developmental stages and enable/disable developmental processes
- **Checkpointing Controls**: Specify checkpoint files and save intervals
- **Performance Controls**: Override neuron count, timestep, and limit simulation steps
- **Output and Visualization**: Control logging verbosity, save statistics, and show progress
- **Advanced Neural Controls**: Enable/disable plasticity, set plasticity rates, and configure E/I balance
- **Replay and Sleep Controls**: Enable/disable memory replay and sleep/wake cycles

## Usage Examples

```bash
# Run full Phase 6 integration test
./nlm_phase6 --integration-test

# Run integration verification only
./nlm_phase6 --verify-integration

# Configure specific experiment parameters
./nlm_phase6 \
  --integration-test \
  --neuron-count=2000 \
  --enable-memory \
  --enable-episodic-memory \
  --development-stage=1 \
  --dopamine-level=0.7 \
  --curiosity-level=0.5

# Run with checkpointing and replay
./nlm_phase6 \
  --integration-test \
  --max-steps=5000 \
  --checkpoint-file=./brain_state.bin \
  --save-interval=1000 \
  --enable-replay \
  --replay-interval=50

# Run with verbose output and statistics
./nlm_phase6 \
  --integration-test \
  --verbose \
  --output-stats=./experiment_stats.txt \
  --progress

# Run with specific plasticity and neural parameters
./nlm_phase6 \
  --integration-test \
  --plasticity-rate=0.2 \
  --excitatory-inhibition-ratio=0.7 \
  --enable-synaptogenesis \
  --disable-pruning
```

## Available Options

### Phase 6 Integration Testing
- `--integration-test`: Run Phase6IntegratedExperiment
- `--verify-integration`: Test integration only (no full run)
- `--phase6-config=<file>`: Specify Phase 6 config file

### Memory System Controls
- `--enable-memory`/`--disable-memory`: Control working memory systems
- `--enable-episodic-memory`/`--disable-episodic-memory`: Control episodic memory
- `--memory-capacity=<int>`: Set working memory capacity (frames)

### Neuromodulation Controls
- `--enable-neuromodulation`/`--disable-neuromodulation`: Control neuromodulation
- `--dopamine-level=<float>`: Set baseline dopamine level (0.0-1.0)
- `--curiosity-level=<float>`: Set curiosity level (0.0-1.0)

### Development Controls
- `--development-stage=<stage>`: Set developmental stage (0=juvenile, 1=adult, 2=senescent)
- `--enable-development`/`--disable-development`: Control developmental processes

### Checkpointing Controls
- `--checkpoint-file=<path>`: Specify checkpoint file path
- `--save-interval=<int>`: Set checkpoint save interval (steps)

### Performance Controls
- `--neuron-count=<int>`: Override neuron count
- `--timestep=<float>`: Set simulation timestep
- `--max-steps=<int>`: Limit simulation steps
- `--enable-sparse-conn`: Enable sparse connectivity optimization

### Output and Visualization
- `--quiet`/`--verbose`: Control logging level
- `--output-stats=<file>`: Write detailed statistics to file
- `--progress`: Show progress updates

### Advanced Neural Controls
- `--plasticity-enable`/`--plasticity-disable`: Control synaptic plasticity
- `--plasticity-rate=<float>`: Set plasticity rate (0.0-1.0)
- `--excitatory-inhibition-ratio=<float>`: Set E/I balance (0.0-1.0)

### Replay and Sleep Controls
- `--enable-replay`/`--disable-replay`: Control memory replay
- `--replay-interval=<int>`: Set replay frequency
- `--enable-sleep`/`--disable-sleep`: Control sleep/wake cycles

### Development Controls
- `--enable-synaptogenesis`/`--disable-synaptogenesis`: Control structural synaptogenesis
- `--enable-pruning`/`--disable-pruning`: Control structural pruning

## Backward Compatibility

The original Phase 2 demo (`./nlm`) remains available for backward compatibility:

```bash
# Run Phase 2 demo (original basic interface)
./nlm

# Run Phase 2 demo with custom config
./nlm --config=./my_config.cfg
```

## Config File Format

Phase 6 config files use simple key=value format:

```ini
# Phase 6 Integration Configuration
max_steps=10000
neuron_count=2000
region_count=1
connection_probability=0.1
enable_checkpointing=true
enable_replay=true
enable_development=true
checkpoint_path=./checkpoint.bin
```

## Building

Build the Phase 6 executable with CMake:

```bash
mkdir build
cd build
cmake ..
make
```

The executable `nlm_phase6` will be created in the build directory.

## Testing

To test the Phase 6 integration:

```bash
# Test integration only
./nlm_phase6 --verify-integration

# Run full integration test
./nlm_phase6 --integration-test

# Test with custom parameters
./nlm_phase6 --integration-test --neuron-count=1000 --max-steps=2000
```

See `Phase6IntegrationResult` documentation in `src/experiments/Phase6IntegratedExperiment.hpp` for detailed output information.

## Advanced Configuration

The Phase 6 interface supports all Phase 2 functionality plus advanced configuration of:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Development**: Maturation, synaptogenesis, pruning
- **Plasticity**: STDP, Hebbian, structural plasticity
- **Prediction**: Predictive coding, prediction error
- **Cognition**: Planning, concept formation, attention
- **Motor**: Action selection, motor control
- **Performance**: Checkpointing, replay, sleep/wake cycles

This makes the NLM Phase 6 interface ideal for advanced research and experimentation with artificial developmental brains.
