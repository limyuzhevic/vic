# NLM User Guide - Phase 6 Integration

Welcome to NLM (熙然) - an experimental artificial developmental brain project.

## Overview

NLM is a computational brain project that investigates whether:
- Memory systems can integrate with neural dynamics
- Neuromodulation can affect plasticity in a coordinated way  
- Prediction can become a central organizing principle
- Developmental stages can modulate learning
- Replay can reinforce memory consolidation
- The complete brain loop functions coherently

## Getting Started

### Quick Start

1. **Build the project:**
   ```bash
   mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4
   ```

2. **Run the Phase 6 integration demo:**
   ```bash
   ./nlm_phase6_demo
   ```

3. **Use the enhanced CLI utility:**
   ```bash
   nlm-cli demo --quick                    # Quick demo (50 neurons, 100 steps)
   nlm-cli demo --full                     # Full demo (500 neurons, 2000 steps)
   nlm-cli run --neurons=1000 --steps=5000  # Custom configuration
   ```

### Alternative Interface Options

The NLM project provides multiple ways to interact with the system:

1. **Command Line:** Direct execution of binaries
2. **Enhanced CLI:** `nlm-cli` utility for user-friendly workflows
3. **Configuration Files:** Pre-defined settings for different use cases
4. **Configuration Management:** Create and manage custom configs

## NLM CLI Utility

The `nlm-cli` script provides an enhanced command-line interface with:

### Commands

**`nlm-cli run [OPTIONS]`** - Run NLM simulation
- `--neurons=<count>` - Number of neurons (default: 500)
- `--steps=<count>` - Number of simulation steps (default: 2000)
- `--checkpoint=<path>` - Path for checkpoint operations
- `--load=<path>` - Load brain state from checkpoint
- `--demo-mode` - Run in demo mode (faster, fewer neurons)
- `--integration-only` - Only test integration, no learning/experiment
- `--performance-test` - Run performance benchmark
- `--verbose` - Enable verbose logging

**`nlm-cli demo [OPTIONS]`** - Run Phase 6 integration demo
- `--quick` - Quick demo (50 neurons, 100 steps)
- `--full` - Full demo (500 neurons, 2000 steps)
- `--verbose` - Enable verbose logging

**`nlm-cli config [SUBCOMMAND]`** - Manage configuration files
- `list` - List available configuration files
- `show <file>` - Show configuration file contents
- `create <file>` - Create new configuration file

**`nlm-cli info`** - Show system information and version

### Examples

```bash
# Run quick demo
nlm-cli demo --quick

# Run full demo with custom settings
nlm-cli demo --full

# Run performance benchmark
nlm-cli run --performance-test

# Load saved brain state
nlm-cli run --load=./my_checkpoint --demo-mode

# Test checkpoint functionality
nlm-cli run --checkpoint=./test_checkpoint

# List available configs
nlm-cli config list

# Show default config
nlm-cli config show default.cfg

# Create custom config
nlm-cli config create myconfig.cfg
```

## Configuration Files

### Available Configurations

The project includes several configuration files for different use cases:

1. **default.cfg** - Optimized for Phase 6 integration
2. **expert.cfg** - Optimized for performance testing
3. **beginner.cfg** - Optimized for learning and exploration
4. **performance.cfg** - Optimized for benchmarking

### Configuration File Format

Configuration files use simple `key = value` format:

```
# Example configuration
neuron_count = 1000
simulation_timestep = 0.001
enable_development = true
enable_checkpointing = true
log_level = INFO
```

### Using Configuration Files

To use a specific configuration file:

```bash
# Run with expert configuration
./nlm_phase6_demo --config=configs/expert.cfg

# Run with custom configuration
./nlm_phase6_demo --config=./myconfig.cfg

# Create and use custom configuration
nlm-cli config create myconfig.cfg
./nlm_phase6_demo --config=configs/myconfig.cfg
```

## Advanced Usage

### Running with Checkpoints

Checkpoints allow you to save and load brain state:

```bash
# Save checkpoint after simulation
./nlm_phase6_demo --checkpoint=./my_checkpoint

# Load saved brain state
./nlm_phase6_demo --load=./my_checkpoint

# Using CLI for checkpoint management
nlm-cli run --checkpoint=./my_checkpoint
nlm-cli run --load=./my_checkpoint
```

### Performance Testing

To benchmark system performance:

```bash
# Run performance benchmark
./nlm_phase6_demo --performance-test

# Or using CLI
nlm-cli run --performance-test

# With custom settings
./nlm_phase6_demo --performance-test --neurons=2000 --steps=5000
```

### Integration Testing

To verify all brain systems are connected:

```bash
# Run integration-only test
./nlm_phase6_demo --integration-only

# Or using CLI
nlm-cli run --integration-only
```

## System Architecture

### Brain Systems Integration

NLM integrates the following systems:

1. **Memory Systems**
   - Working Memory: Stores current task information
   - Episodic Memory: Stores experience episodes
   - Semantic Memory: Stores knowledge concepts
   - Procedural Memory: Stores learned skills
   - Associative Memory: Stores pattern associations

2. **Neuromodulation**
   - Dopamine: Reward prediction error signaling
   - Curiosity: Exploration drive
   - Novelty: Change detection
   - Prediction Error: Learning signal

3. **Cognition Systems**
   - Neural Planner: Action sequence evaluation
   - Concept Formation: Pattern abstraction
   - Attention: Competitive selection dynamics

4. **Prediction System**
   - Prediction Error: Learning from surprises
   - Confidence Tracking: Uncertainty quantification
   - Forward Models: Anticipating future states

5. **Development System**
   - Synaptogenesis: Formation of new connections
   - Pruning: Elimination of unused connections
   - Maturation: Improvement of connection quality

### Integration Flow

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
    ↓                     ↓                 ↓
MEMORY / PREDICTION ← MOTIVATION / NEUROMODULATION → ACTION SELECTION
    ↓                     ↑                 ↓
   PLASTICITY ← REWARD / SURPRISE / ERROR ← MOTOR OUTPUT
    ↓                     ↑                 ↓
   MEMORY / DEVELOPMENT ← WORLD CONSEQUENCE ← CHANGED BRAIN
    ↓                     ↑                 ↓
   CHANGED FUTURE BEHAVIOR ← CHANGED ENVIRONMENT ← WORLD
```

## Common Issues and Solutions

### Issue: Project Build Fails

**Solution:**
```bash
# Clean build
rm -rf build && mkdir build && cd build

# Configure with proper options
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20

# Build with parallel jobs
make -j$(nproc)
```

### Issue: NLM Demo Not Found

**Solution:**
```bash
# Check if Phase 6 demo was built
ls -la build/

# Re-build if necessary
make -j$(nproc)

# Check CMakeLists.txt for Phase 6 integration
```

### Issue: Configuration File Not Found

**Solution:**
```bash
# Check available configuration files
nlm-cli config list

# Create new configuration
nlm-cli config create myconfig.cfg

# Use default configuration
./nlm_phase6_demo
```

### Issue: Performance Too Slow

**Solution:**
```bash
# Use fewer neurons for faster testing
./nlm_phase6_demo --neurons=100 --steps=500

# Run in demo mode
./nlm_phase6_demo --demo-mode

# Disable development (faster)
./nlm_phase6_demo --config=configs/performance.cfg
```

## Troubleshooting

### Error Messages

**"Integration verification failed!"**
- Check that all required libraries and dependencies are installed
- Verify that the build process completed successfully
- Try running with `--integration-only` flag

**"Failed to load checkpoint!"**
- Ensure the checkpoint file exists and is not corrupted
- Check file permissions
- Try saving a new checkpoint first

**"Configuration file not found!"**
- Verify the configuration file path is correct
- Check if the file exists using `ls configs/`
- Create a new configuration file using `nlm-cli config create`

### Getting Help

For additional help:

1. **Check the README.md file** for detailed information
2. **Run `nlm-cli help`** for command-line help
3. **Visit the project documentation** at [project website]
4. **Check the examples directory** for usage examples

## Future Enhancements

The NLM project continues to evolve. Future improvements may include:

1. **Enhanced Visualization:** Better graphical representations of brain activity
2. **Real-time Interaction:** Live monitoring and control of simulation
3. **Advanced Learning:** More sophisticated learning algorithms
4. **Multi-agent Systems:** Interaction between multiple brain instances
5. **Hardware Acceleration:** GPU support for faster simulation
6. **Cloud Deployment:** Cloud-based simulation capabilities
7. **Extended Experiments:** More diverse experimental paradigms

## Contact and Support

For questions, issues, or suggestions:

1. **Project Repository:** [GitHub Repository]
2. **Issue Tracker:** [Issue Tracker Link]
3. **Documentation:** [Documentation Link]
4. **Discussion Forum:** [Forum Link]

## License

MIT License

---

*Last Updated: $(date +%Y-%m-%d)*
*NLM Project - An experimental artificial developmental brain research project*
