# NLM Phase 6 - Easy Usage Guide

## Overview

NLM (熙然, "serene flow") is an experimental computational brain project that implements a complete artificial neural learning machine. Phase 6 represents the final integration of all previous systems into a coherent brain architecture.

## Quick Start

### 1. Building and Running

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (using all available cores)
make -j4
```

### 2. Running the Integration Demo

```bash
# Run the comprehensive integration test (recommended for first use)
./nlm_phase6_demo
```

This runs a complete integration test that verifies all brain systems are connected and functional.

### 3. Running the Standard Test Suite

```bash
# Run the main test suite (Phase 6 integration)
./nlm_phase6

# Or run with custom configuration
./nlm_phase6 --config my_config.cfg
```

## Command Line Options

### NLM Phase 6 Integration Test (`./nlm_phase6`)

```
Usage: ./nlm_phase6 [OPTIONS]

Options:
  --help, -h              Show this help message and exit
  --config, -c <file>     Load configuration from file (default: configs/default.cfg)
  --neurons, -n <count>   Set number of neurons (default: 500)
  --steps, -s <count>     Set simulation steps (default: 2000)
  --seed, -seed           Set random seed (default: 42)
  --no-plasticity         Disable plasticity (STDP and Hebbian)
  --version, -v           Show version information and exit

Examples:
  ./nlm_phase6                        Run with default settings
  ./nlm_phase6 -c my_config.cfg           Load custom configuration
  ./nlm_phase6 -n 1000 -s 5000              Run with 1000 neurons for 5000 steps
```

### NLM Phase 6 Demo (`./nlm_phase6_demo`)

The demo provides a quick overview of the integrated brain systems:

```bash
./nlm_phase6_demo
```

This runs a lightweight integration test with the following components:
- Integration verification tests
- Individual system tests (memory, neuromodulation, checkpointing)
- Full integration experiment with performance metrics

## Configuration File Format

Create a file named `my_config.cfg` with the following key=value pairs:

```
# Random seed for reproducible simulations
random_seed = 42

# Simulation timestep in seconds (smaller = more precise but slower)
simulation_timestep = 0.001

# Number of neurons in the simulation
neuron_count = 500

# Number of neural regions (default: 1 for simple configuration)
region_count = 1

# Probability of connection between neurons
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

## Using Custom Configurations

1. Create a configuration file (e.g., `production.cfg`)
2. Use the `--config` option:

```bash
./nlm_phase6 --config production.cfg
```

## Advanced Usage

### For Researchers and Advanced Users

The following command-line options are available for advanced usage:

```bash
# Run with different neuron counts for performance testing
./nlm_phase6 -n 100    # Small scale (quick testing)
./nlm_phase6 -n 1000   # Medium scale (balanced)
./nlm_phase6 -n 5000   # Large scale (comprehensive testing)

# Run with different step counts for longer experiments
./nlm_phase6 -s 1000   # Short experiment
./nlm_phase6 -s 10000  # Long experiment

# Disable plasticity to test baseline performance
./nlm_phase6 --no-plasticity

# Set specific random seed for reproducible experiments
./nlm_phase6 --seed 12345
```

## Understanding the Output

The program produces extensive logging output. Here are the key messages to watch for:

- **Integration Verification**: Shows whether all brain systems are connected
- **Memory Systems**: Working memory and episodic memory status
- **Neuromodulation**: Dopamine, curiosity, and novelty detection
- **Prediction System**: How the brain predicts sensory input
- **Cognition Systems**: Planning, concept formation, and attention
- **Development**: How developmental stages affect learning
- **Final Status**: Complete system performance summary

## Common Issues and Solutions

### "Configuration file not found"

Solution: Use `--config` with a valid path, or ensure the default config file exists.

### Slow Performance

Solution: For quick testing, use fewer neurons and steps:
```bash
./nlm_phase6 -n 100 -s 500
```

### Memory Usage

Solution: The system uses memory pools and event queues efficiently. For very large simulations, monitor system resources.

## Integration Verification

The system performs comprehensive integration tests. Successful integration means:

- ✓ Working memory is functional
- ✓ Episodic memory stores experiences
- ✓ Neuromodulation affects learning
- ✓ Prediction system works
- ✓ Cognitive systems are connected
- ✓ Development system is active
- ✓ Checkpointing works for state preservation

## Next Steps

1. **Run the demo**: `./nlm_phase6_demo`
2. **Try custom config**: Create your own config file
3. **Explore advanced options**: Use different neuron/step combinations
4. **Study the output**: Review integration test results

## Project Documentation

For detailed information about the architecture, science, and development roadmap, see:
- `docs/ARCHITECTURE.md` - System architecture
- `docs/SCIENCE.md` - Scientific background
- `docs/ROADMAP.md` - Future development plans

## License

MIT License - Research project for computational brain modeling.

This project is experimental and makes no claims about reproducing biological brains accurately.
All systems are research prototypes demonstrating integration of multiple neural mechanisms.