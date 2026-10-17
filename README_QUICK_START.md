# NLM Quick Start Guide

## What is NLM?

NLM (Neural Learning Machine) is an experimental computational brain project that simulates real spiking neural computation with:

- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

## Getting Started

### 1. Build the Project

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Run the Basic Simulation

```bash
./nlm
```

This will run a comprehensive integration test demonstrating:
- Basic neural connectivity
- Plasticity learning experiments
- STDP verification

### 3. Run Integration Tests

To run all unit tests:

```bash
./nlm --test
```

This tests:
- Configuration management
- Random number generation
- Neuron behavior
- Synapse dynamics
- Plasticity rules (STDP, Hebbian)
- Brain integration

## Configuration

### Default Configuration File

The project uses `configs/default.cfg` as the default configuration.

### Common Parameters

- `random_seed`: Set to 42 for reproducible results
- `neuron_count`: Number of neurons (default: 500)
- `region_count`: Number of brain regions (default: 1)
- `simulation_timestep`: Time step in seconds (default: 0.001)

### Custom Configuration

Create your own config file:

```bash
cat > my_config.cfg << EOF
random_seed = 123
neuron_count = 1000
region_count = 2
log_level = DEBUG
sensory_noise_level = 0.05
EOF

# Run with custom config
./nlm --config my_config.cfg
```

## Command Line Options

| Option | Description |
|--------|-------------|
| `--config=file` | Specify config file |
| `-v, --verbose` | Enable verbose logging |
| `-t, --test` | Run integration tests |
| `-h, --help` | Show help message |

## Advanced Usage

### Running with Custom Parameters

```bash
# Override specific values from command line
./nlm --neuron-count=2000 --region-count=3 --log-level=INFO

# Short form
./nlm -n 2000 -r 3 -v
```

### Testing Different Configurations

Create multiple config files for different scenarios:

```bash
# Fast test config
cat > fast_test.cfg << EOF
neuron_count = 100
region_count = 1
simulation_timestep = 0.01
log_level = WARNING
EOF

./nlm --config fast_test.cfg
```

## Understanding the Output

The basic simulation output shows:

1. **Banner**: Project information and capabilities
2. **Initialization**: Configuration loading and brain setup
3. **Tests**: Three main integration tests:
   - Test 1: Basic neural connectivity
   - Test 2: Plasticity learning experiment
   - Test 3: STDP verification
4. **Final Status**: Complete system overview

## Key Features Demonstrated

### 1. Real Neural Dynamics

The simulation implements biological neuron models:
- LIF neuron dynamics
- Event-driven spike propagation
- Synaptic delays and transmission

### 2. Learning Mechanisms

Multiple plasticity rules work together:
- STDP (Spike-Timing-Dependent Plasticity)
- Hebbian learning
- Structural plasticity (synaptogenesis/pruning)

### 3. Integrated Systems

All major brain components are connected:
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction systems
- Cognitive functions

## Troubleshooting

### If Tests Fail

```bash
# Run with verbose output
./nlm --test -v
```

### If Configuration Not Loaded

```bash
# Check file exists and has correct format
cat configs/default.cfg

# Try different path
./nlm --config /path/to/config.cfg
```

### If Build Issues Occur

```bash
# Clean and rebuild
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

## Next Steps

1. **Explore**: Look at `src/` directory to understand the implementation
2. **Modify**: Try changing configuration parameters to see effects
3. **Extend**: Add new plasticity rules or neuromodulators
4. **Document**: Write examples for other users

## Additional Resources

- **Documentation**: `NLM_USER_GUIDE.md` for comprehensive information
- **Architecture**: `docs/ARCHITECTURE.md` for technical details
- **Science**: `docs/SCIENCE.md` for biological background

## Version

Phase 6: Final Integration - All systems integrated and functional.

Enjoy exploring the neural learning machine! :)