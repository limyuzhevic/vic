# NLM Unified Demo Launcher

The NLM (Neural Learning Machine) Unified Demo Launcher provides a comprehensive command-line interface for running all NLM demonstrations and experiments.

## Overview

The unified demo launcher `nlm_demo` replaces the individual executables (`nlm`, `nlm_phase3_demo`, `nlm_phase4_demo`, `nlm_phase6_demo`, etc.) with a single, consistent interface that provides:

- **Unified command-line interface** for all demos
- **Advanced configuration options** for fine-tuning simulations
- **Comprehensive help documentation** for all users
- **Consistent behavior** across all demonstrations
- **Better error handling** and user feedback
- **Support for configuration files** and command-line overrides

## Installation

After building with CMake, the unified demo launcher will be installed to:

- Unix-like systems: `/usr/local/bin/nlm_demo`
- Windows: `C:\Program Files\NLM\nlm_demo.exe`

## Usage

### Basic Usage

```bash
# Show help information
nlm_demo --help

# Run Phase 2 basic demonstration
nlm_demo phase2

# Run Phase 3 world interaction demo (default: 5 episodes, 200 steps)
nlm_demo phase3

# Run Phase 4 emerging cognition demo (default: 50 trials)
nlm_demo phase4

# Run Phase 6 integration demo
nlm_demo phase6

# Run all integration tests
nlm_demo phase6_all

# Run all available demos
nlm_demo all
```

### Configuration Options

```bash
# Load configuration from file
nlm_demo demo_name --config configs/my_config.cfg

# Override number of episodes (Phase 3)
nlm_demo phase3 --episodes 10

# Override number of trials (Phase 4)
nlm_demo phase4 --trials 100

# Override number of neurons
nlm_demo phase3 --neurons 2000

# Enable verbose output
nlm_demo phase4 --verbose

# Enable quiet mode
nlm_demo phase3 --quiet

# Enable checkpointing
nlm_demo phase6 --checkpoints

# Disable homeostatic plasticity
nlm_demo phase3 --no-homeostatic
```

### Advanced Options

```bash
# Set learning rate (0.0-1.0)
nlm_demo phase3 --learning-rate 0.005

# Set plasticity factor (0.0-2.0)
nlm_demo phase4 --plasticity-factor 1.2

# Set novelty weight (0.0-1.0)
nlm_demo phase3 --novelty-weight 0.7

# Set curiosity weight (0.0-1.0)
nlm_demo phase4 --curiosity-weight 0.6

# Custom network size
nlm_demo phase3 --neurons 5000

# Custom simulation steps
nlm_demo phase3 --steps 500
```

## Available Demos

### Phase 2: Real Neural Computation

**Description:** Demonstrates the core NLM neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

**Command:** `nlm_demo phase2`

**Default Configuration:**
- 500 neurons
- 1 region
- Connection probability: 1% (reduced from 15% for better learning control)
- Learning rates: Conservative values for stable learning
- Homeostatic plasticity enabled

**Usage:** Provides basic neural computation capabilities and demonstrates how neurons can learn and adapt.

### Phase 3: World Interaction

**Description:** Demonstrates experience -> neural activity -> action -> consequence -> learning cycle in a simulated world.
- Autonomous agent explores 2D environment
- Discovers resources (positive reward) and hazards (negative reward)
- Uses working memory, episodic memory, and neuromodulation
- Demonstrates selective attention and concept formation

**Command:** `nlm_demo phase3`

**Arguments:**
- Default: 5 episodes, 200 steps per episode
- Override: `nlm_demo phase3 --episodes 10 --steps 300`

**Configuration Options:**
- Resource reward: +30 energy
- Hazard damage: -20% health
- Action costs: Small penalties for movement/turning
- Default: 500 neurons, 2 regions

**Usage:** Shows how the brain can interact with and learn from an environment.

### Phase 4: Emerging Cognition

**Description:** Demonstrates how complex cognitive abilities emerge from neural dynamics without symbolic AI.
- Temporal prediction learning
- Working memory and episodic memory
- Concept formation and generalization
- Neural attention and selective processing
- Multi-step planning and problem solving
- Self-model and body awareness
- Social learning through observation
- Continual learning and transfer

**Command:** `nlm_demo phase4`

**Arguments:**
- Default: 50 trials
- Override: `nlm_demo phase4 --trials 100`

**Configuration Options:**
- 1000 neurons, 2 regions
- Enhanced homeostatic mechanisms
- Conservative learning rates for stability
- Multiple cognitive domains tested

**Usage:** Demonstrates that complex cognition can emerge from neural mechanisms alone.

### Phase 6: Integration Demo

**Description:** Comprehensive integration test verifying all brain systems work together.
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction and planning systems
- Concept formation and attention
- Development and structural plasticity
- Checkpoint save/load functionality
- Replay and consolidation

**Command:** `nlm_demo phase6`

**Configuration:** Uses enhanced parameters for stable integration:
- Homeostatic plasticity enabled
- Conservative plasticity parameters
- Frequent synaptic scaling for stability
- Comprehensive integration verification

### Phase 6 All Tests

**Description:** Runs complete suite of integration tests for maximum verification.

**Command:** `nlm_demo phase6_all`

**Tests Included:**
- Integration verification
- Memory system tests
- Neuromodulation tests
- Checkpoint tests
- Replay tests
- Full integration experiment

### Benchmark

**Description:** Performance benchmark measuring simulation speed for different network sizes.

**Command:** `nlm_demo benchmark`

**Test Configurations:**
- 100 neurons, 1000 steps
- 500 neurons, 1000 steps
- 1000 neurons, 500 steps
- 2000 neurons, 200 steps

### All Demos

**Description:** Runs all available demos in sequence.

**Command:** `nlm_demo all`

**Order:** Phase 2 → Phase 3 → Phase 4 → Phase 6

## Configuration Files

### File Format

Configuration files use key-value pairs with the following format:

```ini
# Comments start with #
key = value

# Empty lines are ignored

# Boolean values: true/false, yes/no
# Numbers: integer or floating point
# Strings: quoted or unquoted
```

### Example Configuration File

```ini
# NLM Configuration Example
# See configs/default.cfg for full reference

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity (reduced from 0.1 to 0.01)
connection_probability = 0.01

# Plasticity settings (conservative for stable learning)
stdp_ltp_weight = 0.005
stdp_ltd_weight = 0.008
stdp_time_constant = 15.0

# Development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.0
curiosity_weight = 0.5
novelty_weight = 0.3

# Reward settings
reward_discount_factor = 0.99

# Homeostatic plasticity
enable_homeostatic_plasticity = true
synaptic_scaling_interval = 100
synaptic_scaling_target = 1.0

# Environment settings
environment_width = 20
environment_height = 20
vision_width = 16
vision_height = 16

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

### Configuration Override Priority

1. **Hardcoded defaults** (built into the demo)
2. **Configuration file** (`--config file.cfg`)
3. **Command-line arguments** (last specified wins)
4. **Built-in demo-specific defaults** (overridden by user)

## Advanced Features

### Homeostatic Plasticity

Homeostatic plasticity prevents runaway excitation and maintains stable network activity:

- **Activation**: Enabled by default
- **Frequency**: Every 100 steps (configurable via `--synaptic-scaling-interval`)
- **Target**: 1.0 Hz average firing rate (configurable via `--synaptic-scaling-target`)
- **Effect**: Scales synaptic weights to maintain stable firing rates

### Enhanced Learning Control

The unified demo launcher provides finer control over learning parameters:

- **STDP parameters**: LTP/LTD weights and time constants
- **Hebbian learning**: Independent learning rate and weight bounds
- **Neuromodulator weights**: Novelty, curiosity, and prediction error contributions

### Checkpointing and Persistence

- **Automatic checkpoints**: Every 10,000 steps (configurable)
- **Manual checkpoints**: Save current brain state at any time
- **Load functionality**: Resume from saved checkpoints
- **Compression**: Optional checkpoint compression

### Performance Monitoring

- **Verbose mode**: Detailed logging of internal processes
- **Benchmark mode**: Performance metrics for different network sizes
- **Resource monitoring**: Memory usage and CPU performance

## Troubleshooting

### Common Issues and Solutions

**Issue:** No output or silent execution
**Solution:** Use `--verbose` flag or check log level in configuration file

**Issue:** Simulation crashes with large networks
**Solution:** Reduce `--neurons` parameter or increase `--learning-rate` for stability

**Issue:** No learning observed
**Solution:** Check plasticity parameters and ensure `--learning-rate` is set appropriately

**Issue:** Checkpoint save/load fails
**Solution:** Verify write permissions for checkpoint directory

### Getting Help

```bash
# Show help for all options
nlm_demo --help

# Show help for specific demo
nlm_demo phase3 --help

# Run with minimal output (quiet mode)
nlm_demo phase4 --quiet

# Run with verbose output (debug mode)
nlm_demo phase3 --verbose
```

### Log Files

Log files are created in the current directory or specified via:

- `nlm.log` (default)
- `configs/log_filename` (from config file)

Log levels (from config file or command line):
- **ERROR**: Critical errors only
- **WARNING**: Warnings and potential issues
- **INFO**: Normal operation information
- **DEBUG**: Detailed debugging information (not available in current build)

## Advanced Usage Examples

### Custom Simulation Setup

```bash
# Create custom configuration file
cat > my_config.cfg << EOF
random_seed = 12345
neuron_count = 2000
region_count = 3
connection_probability = 0.02
stdp_ltp_weight = 0.003
stdp_ltd_weight = 0.005
enable_homeostatic_plasticity = true
synaptic_scaling_interval = 50
synaptic_scaling_target = 0.8
EOF

# Run with custom configuration
nlm_demo phase3 --config my_config.cfg --episodes 20 --steps 400
```

### Performance Testing

```bash
# Benchmark with different network sizes
nlm_demo benchmark --neurons 500
nlm_demo benchmark --neurons 1000
nlm_demo benchmark --neurons 2000

# Run intensive Phase 4 test with high neuron count
nlm_demo phase4 --neurons 5000 --trials 200 --learning-rate 0.001
```

### Development and Debugging

```bash
# Run in verbose mode for debugging
nlm_demo phase3 --verbose --episodes 2

# Enable/disable specific features
nlm_demo phase6 --no-homeostatic --checkpoints

# Run all tests for validation
nlm_demo all --verbose
```

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
