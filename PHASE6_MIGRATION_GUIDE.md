# NLM Phase 2 vs Phase 6 Interface Guide

## Overview

This document explains the relationship between NLM Phase 2 (basic demo) and NLM Phase 6 (advanced interface) command-line interfaces.

## Executive Summary

- **Phase 2 (`./nlm`)**: Original basic demo with minimal options
- **Phase 6 (`./nlm_phase6`)**: Advanced interface with comprehensive controls for research and experimentation

## Usage Comparison

### Phase 2 - Basic Demo

```bash
# Basic usage (runs simple neural computation demo)
./nlm

# With custom configuration
./nlm --config=./my_config.cfg

# Help
./nlm --help
```

### Phase 6 - Advanced Interface

```bash
# Basic usage (runs Phase 2 demo as fallback)
./nlm_phase6

# Run Phase 6 integration test
./nlm_phase6 --integration-test

# Verify integration only
./nlm_phase6 --verify-integration

# Show all advanced options
./nlm_phase6 --help
```

## Key Differences

| Feature | Phase 2 (`./nlm`) | Phase 6 (`./nlm_phase6`) |
|---------|------------------|------------------------|
| Purpose | Basic demo | Advanced research interface |
| Options | ~6 basic options | 50+ advanced options |
| Systems | 3 basic systems | All 11 integrated systems |
| Memory | No working memory | Full memory system integration |
| Neuromodulation | None | Dopamine, curiosity, novelty |
| Development | None | Full developmental processes |
| Checkpointing | None | Advanced checkpoint/save/load |
| Replay | None | Memory replay and consolidation |
| Configuration | Simple file-based | Command-line + file-based |
| Testing | Limited tests | Phase 6 integration test suite |

## When to Use Each

### Use Phase 2 (`./nlm`) when:
- First-time users want a simple demo
- Need backward compatibility
- Want minimal setup
- Testing basic neural computation
- Running in constrained environments

### Use Phase 6 (`./nlm_phase6`) when:
- Researching artificial developmental brains
- Need full system integration
- Running complex experiments
- Configuring neural parameters
- Monitoring performance and checkpoints
- Analyzing results
- Testing integration of multiple systems

## Feature Matrix

### Phase 6 Integration Testing
- ✅ `--integration-test` (run complete Phase 6 suite)
- ✅ `--verify-integration` (integration test only)
- ✅ `--phase6-config=<file>` (custom config files)

### Memory Systems
- ✅ `--enable-memory`/`--disable-memory`
- ✅ `--enable-episodic-memory`/`--disable-episodic-memory`
- ✅ `--memory-capacity=<int>` (working memory size)

### Neuromodulation
- ✅ `--enable-neuromodulation`/`--disable-neuromodulation`
- ✅ `--dopamine-level=<float>` (reward signal)
- ✅ `--curiosity-level=<float>` (exploration drive)

### Development
- ✅ `--development-stage=<stage>` (life stage)
- ✅ `--enable-development`/`--disable-development`
- ✅ `--enable-synaptogenesis`/`--disable-synaptogenesis`
- ✅ `--enable-pruning`/`--disable-pruning`

### Checkpointing
- ✅ `--checkpoint-file=<path>` (save location)
- ✅ `--save-interval=<int>` (save frequency)

### Performance
- ✅ `--neuron-count=<int>` (override neuron count)
- ✅ `--timestep=<float>` (simulation time step)
- ✅ `--max-steps=<int>` (simulation limit)

### Output Control
- ✅ `--quiet` (minimal output)
- ✅ `--verbose` (debug output)
- ✅ `--output-stats=<file>` (save statistics)
- ✅ `--progress` (show progress updates)

### Neural Controls
- ✅ `--plasticity-enable`/`--plasticity-disable`
- ✅ `--plasticity-rate=<float>` (plasticity strength)
- ✅ `--excitatory-inhibition-ratio=<float>` (E/I balance)

### Replay/Sleep
- ✅ `--enable-replay`/`--disable-replay`
- ✅ `--replay-interval=<int>` (replay frequency)
- ✅ `--enable-sleep`/`--disable-sleep`

## Migration Guide

### From Phase 2 to Phase 6

If you're migrating from Phase 2, here's how to translate your experiments:

#### Phase 2 Commands
```bash
# Original Phase 2 command
./nlm --config=experiment.cfg
```

#### Equivalent Phase 6 Commands
```bash
# Modern Phase 6 equivalent
./nlm_phase6 --phase6-config=experiment.cfg

# With additional controls for memory and neuromodulation
./nlm_phase6 \
  --phase6-config=experiment.cfg \
  --enable-memory \
  --enable-episodic-memory \
  --development-stage=1
```

#### Example Experiment Translation

**Phase 2 Original:**
```ini
# configs/simple_experiment.cfg
neuron_count=500
region_count=1
connection_probability=0.15
random_seed=42
```

**Phase 6 Enhanced:**
```bash
./nlm_phase6 --integration-test \
  --neuron-count=1000 \
  --region-count=2 \
  --connection-probability=0.2 \
  --random-seed=42 \
  --enable-memory \
  --enable-episodic-memory \
  --development-stage=1 \
  --dopamine-level=0.5 \
  --curiosity-level=0.8
```

## Configuration Files

### Phase 2 Config Files
- Format: `key=value`
- Supports: integers, floats, booleans, strings
- Example: `./nlm --config=./experiments/basic.cfg`

### Phase 6 Config Files
- Format: `key=value` (same as Phase 2)
- Supports additional Phase 6 options
- Location: `./phase6_default.cfg` or custom files
- Example: `./nlm_phase6 --phase6-config=./experiments/advanced.cfg`

## System Integration

### Phase 2 Systems
- LIF neurons
- Synaptic transmission
- STDP plasticity
- Hebbian learning
- Structural plasticity

### Phase 6 Systems
- LIF neurons
- Synaptic transmission
- STDP plasticity
- Hebbian learning
- Structural plasticity
- Working memory
- Episodic memory
- Neuromodulation (dopamine, curiosity, novelty)
- Development system
- Prediction system
- Replay system
- Checkpointing system
- Sleep/wake cycles

## Performance Considerations

### Phase 2 Performance
- ~500 neurons
- ~1 region
- ~10000 steps
- Basic connectivity only

### Phase 6 Performance
- Configurable neuron count (1000-10000+)
- Multiple regions
- Configurable steps (10-1000000+)
- All systems integrated
- Advanced optimization options

## Testing and Validation

### Phase 2 Tests
- Basic connectivity test
- Plasticity learning experiment
- STDP verification

### Phase 6 Tests
- Integration verification
- System-by-system validation
- Performance benchmarking
- Memory system testing
- Neuromodulation testing
- Development process testing
- Checkpoint/save/load testing
- Replay system testing

## Help Commands

### Phase 2 Help
```bash
./nlm --help
```

### Phase 6 Help
```bash
./nlm_phase6 --help
./nlm_phase6 --help-detailed
```

## Build Instructions

Build both executables with CMake:

```bash
mkdir build
cd build
cmake ..
make
```

Both `nlm` (Phase 2) and `nlm_phase6` (Phase 6) will be built.

## Advanced Usage

### Combining Options

Phase 6 allows combining options for complex experiments:

```bash
# Complex experiment with all systems
./nlm_phase6 --integration-test \
  --neuron-count=5000 \
  --max-steps=50000 \
  --enable-memory \
  --enable-episodic-memory \
  --enable-neuromodulation \
  --enable-development \
  --enable-replay \
  --enable-sleep \
  --plasticity-rate=0.15 \
  --excitatory-inhibition-ratio=0.8 \
  --dopamine-level=0.7 \
  --curiosity-level=0.6 \
  --checkpoint-file=./complex_experiment.bin \
  --save-interval=1000 \
  --verbose \
  --progress
```

### Config File Composition

Phase 6 supports combining command-line arguments with config files:

```bash
# Command line overrides config file
./nlm_phase6 --phase6-config=experiment.cfg --neuron-count=2000 --max-steps=5000

# Config file provides defaults
./nlm_phase6 --integration-test --phase6-config=experiment.cfg
```

## Conclusion

The NLM Phase 6 Advanced Interface provides comprehensive control over all brain systems for advanced research and experimentation. Phase 2 remains as a simple fallback for basic usage and backward compatibility.

Use Phase 6 when you need:
- Full system integration
- Advanced configuration
- Complex experimental setups
- Performance monitoring
- Result analysis

Use Phase 2 when you need:
- Simple demos
- Backward compatibility
- Minimal setup
- Quick testing

Both interfaces coexist to serve different user needs while maintaining the same underlying neural computation engine.
