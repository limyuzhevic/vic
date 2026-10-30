# NLM Python CLI Tool

A command-line interface for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The NLM Python CLI tool provides an easy-to-use interface for running neural simulations with the following features:

- **Multiple configuration presets** for different use cases (quick demo, detailed simulation, research)
- **Interactive mode** for real-time control and monitoring
- **Phase 6 integration demo** showing all systems working together
- **Easy configuration management** with sensible defaults
- **Comprehensive output** with metrics and progress tracking

## Installation

1. **Build the C++ extension** (if not already built):
   ```bash
   ./build.sh
   ```

2. **Install the Python package** (in development mode):
   ```bash
   pip install -e .
   ```

3. **Set up development environment** (optional):
   ```bash
   ./dev_setup.sh
   ```

## Usage

### Basic Commands

```bash
# Run quick demonstration (default)
nlm_cli quick

# Run detailed simulation with custom steps
nlm_cli detailed 5000

# Run research-grade simulation
nlm_cli research

# Run Phase 6 integration demo (comprehensive test)
nlm_cli phase6

# List available configurations
nlm_cli config
```

### Command-Line Options

```bash
nlm_cli [OPTIONS] [STEPS]

Arguments:
  command           Command to execute (default: quick)
  steps             Number of simulation steps (overrides config default)

Options:
  --interactive     Run in interactive mode with real-time controls
  --config NAME     Configuration to use (quick, detailed, research)
  --help            Show this help message
```

## Available Configurations

### Quick
- **Neurons:** 100
- **Regions:** 1  
- **Max Steps:** 1000
- **Environment:** 20x20
- **Best for:** Quick demos, testing, initial exploration

### Detailed
- **Neurons:** 500
- **Regions:** 2
- **Max Steps:** 10000
- **Environment:** 30x30
- **Best for:** Detailed experiments, performance testing

### Research
- **Neurons:** 1000
- **Regions:** 3
- **Max Steps:** 100000
- **Environment:** 50x50
- **Best for:** Research, scientific experiments, long-term studies

### Phase 6 Integration Demo
- **Neurons:** 1000
- **Regions:** 3
- **Max Steps:** 5000
- **Environment:** 40x40
- **Features:** All integrated brain systems (memory, prediction, development, neuromodulation)

## Features

### Simulation Controls

**In interactive mode**, you can:
- **q** - Quit simulation
- **s** - Save brain state to file
- **l** - Load brain state from file
- **t** - Show brain status and metrics

### Output Monitoring

The CLI provides real-time monitoring of:
- **Step progress** with simulation statistics
- **Reward accumulation** and average reward
- **Neural activity** (firing rate)
- **Neuromodulation levels** (dopamine, curiosity, novelty)
- **Integration status** of all brain systems

### Configuration Management

Configurations store settings for:
- **Brain parameters** (neuron count, regions, connectivity)
- **Simulation parameters** (timestep, max steps)
- **World parameters** (environment size, vision)
- **System parameters** (reward discount, dopamine baseline)
- **Feature flags** (enable/disable neuromodulation, development, etc.)

## Examples

### Quick Demo
```bash
# Run a simple 1000-step simulation
./nlm_cli quick
```

### Long-running Research Simulation
```bash
# Run 10000 steps with detailed configuration
./nlm_cli detailed 10000
```

### Interactive Mode
```bash
# Run with real-time controls for exploration
./nlm_cli quick --interactive
```

### Phase 6 Integration
```bash
# Run the comprehensive integration test
./nlm_cli phase6
```

## File Output

The CLI generates several types of files:

### Brain State Files
- **brain_state*.nlm** - Saved brain states for checkpointing
- Files contain neural connectivity, synaptic weights, and neuron states

### Log Output
- Console output shows real-time simulation progress
- Detailed metrics at regular intervals
- Integration status reports for comprehensive demos

## Troubleshooting

### Python Import Errors
```bash
# If pynlm is not available:
pip install -e .
./build.sh
```

### Build Issues
```bash
# If the build fails:
./build.sh --clean  # Clean and rebuild
./build.sh --debug  # Build with debug symbols
```

### Missing Dependencies
```bash
# Install system dependencies:
# Ubuntu/Debian: sudo apt-get install build-essential cmake ninja-build
# macOS: brew install make cmake ninja
# CentOS/RHEL: sudo yum install gcc-c++ cmake ninja-build
```

## Integration with Other Tools

### With the Main Build System
```bash
# The CLI tool is integrated with the main build system:
./build.sh          # Builds C++ extension and creates executables
./dev_setup.sh      # Sets up development environment
```

### Python Integration
```python
# Import pynlm directly in Python scripts
import pynlm

# Create brain with custom configuration
config = pynlm.Config()
config.set("neuron_count", 100)
brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)
```

### Command Line Pipeline
```bash
# Chain commands for automation
./build.sh && ./nlm_cli quick --steps 500 > simulation.log
```

## Performance Considerations

### Memory Usage
- **Quick mode:** ~100MB for 100 neurons
- **Detailed mode:** ~500MB for 500 neurons  
- **Research mode:** ~1GB+ for 1000 neurons

### Simulation Speed
- **Quick mode:** ~10-100 steps/second on typical hardware
- **Detailed mode:** ~5-50 steps/second
- **Research mode:** ~1-10 steps/second

### Configuration Optimization
- For faster simulations, reduce neuron count and increase timestep
- For detailed analysis, use smaller environments but more neurons
- For research, balance simulation depth with practical runtime

## Development

### Adding New Configurations
Add a new configuration to `DEFAULT_CONFIGS` in `nlm_cli.py`:

```python
"new_mode": {
    "neuron_count": 200,
    "region_count": 2,
    "max_simulation_steps": 2000,
    "environment_width": 25,
    "environment_height": 25,
    # ... other settings
}
```

### Extending Features
The CLI tool can be extended to support:
- Custom configuration files (JSON/YAML)
- Simulation visualization output
- Performance benchmarking
- Automated experiment running
- Multi-simulation comparison tools

## Related Resources

- **NLM Documentation:** `README.md`, `easy_usage.md`, `HOW_TO_USE.md`
- **Build Scripts:** `build.sh`, `dev_setup.sh`
- **Configuration Files:** `configs/default.cfg`
- **Phase 6 Demo:** `./build/nlm_phase6_demo`
- **Source Code:** `src/`, `python/`, `docs/` directories

## License

This CLI tool is part of the NLM project, licensed under the MIT License.

See `LICENSE` file for details.
